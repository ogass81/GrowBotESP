/*
 Name:		GrowBotDue.ino
 Created:	13.01.2017 21:11:47
 Author:	ogass
*/

////Helper
#include "Definitions.h"

//Hardware Libaries
#include <memorysaver.h>
#include <SdFat.h>
#include <SPI.h>
#include <RTCDue.h>
#include <DHT_U.h>
#include <DHT.h>
#include "RealTimeClock.h"
#include "Led.h"

//Core Features
#include "Action.h"
#include "LogEngine.h"
#include "TaskManager.h"
#include "Network.h"
#include "Setting.h"

//Controller Objects
#include "Sensor.h"
#include "Trigger.h"
#include "ActionChain.h"
#include "Ruleset.h"
#include "RCSocketController.h"


//Tact Generator
long sensor_cycles = 0;
uint8_t current_seconds = 0;
uint8_t sensor_last_seconds = 0;
uint8_t task_last_seconds = 0;


unsigned long cpu_current = 0;
unsigned long  next_task = 0;
unsigned long next_sensor = 0;

bool haltstate = false;
long haltstate_start = 0;

//Wifi and Auth
String wifi_ssid;
String wifi_pw;
String api_secret;

//Status LED
Led *led[3];


//DHT Hardware
DHT dht(DHT_DATA_PIN, DHT_TYPE);
//RealTimeClock
RealTimeClock internalRTC(RC);

//433Mhz
RCSocketController *rcsocketcontroller;

//Wifi
WebServer *webserver;
WebTimeClient *ntpclient;

//Modules
//Sensors: Abstraction of all Sensors
Sensor *sensors[SENS_NUM]; 

//Actions: Abstraction of all Actors 
Action *actions[ACTIONS_NUM];
ActionChain *actionchains[ACTIONCHAINS_NUM];

//Task Manager
TaskManager *taskmanager;

//Trigger: Constraints for particular sensors
Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];

//Rulesets: Trigger Action Bundles
RuleSet *rulesets[RULESETS_NUM];

//FileSystem
SdFat sd;

LogEngine logengine;


void setup() {
	// initialize serial for debugging
	Serial.begin(115200);

	//Initialize FileSystem / SD Card
	if (!sd.begin(SD_CONTROL_PIN, SPI_EIGHTH_SPEED)) {
		sd.initErrorHalt();
	}

	// initialize RTC
	internalRTC.begin();
	internalRTC.setDefaultTime();

	//Status Led
	led[0] = new Led(LED1, false);
	led[1] = new Led(LED2, false);
	led[2] = new Led(LED3, false);

	//LogEngine
	logengine.begin();

	String keys[] = { "" };
	String values[] = { "" };
	logengine.addLogEntry(INFO, "Setup", "Starting Bot", keys, values, 0);

	//Start Temp/Humidity Sensor
	dht.begin();

	//433Mhz
	rcsocketcontroller = new RCSocketController(TX_DATA_PIN, RX_DATA_PIN);
			
	//Initialize Sensors
	sensors[0] = new	DHTTemperature(&dht, true, F("Temperature"), F("C"), -127, -50, 100);
	sensors[1] = new 	DHTHumidity(&dht, true, F("Humidity"), F("%"), -127, 0, 100);
	sensors[2] = new 	AnalogMoistureSensor<short>(IN_MOS_1, OUT_MOS_1, true, F("Soil 1"), F("%"), -1, 0, 1000, 150, 600);
	sensors[3] = new 	AnalogMoistureSensor<short>(IN_MOS_2, OUT_MOS_2, true, F("Soil 2"), F("%"), -1, 0, 1000, 150, 600);

	//Intialize Actions
	actions[0] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 0, true);
	actions[1] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 1, true);
	actions[2] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 2, true);
	actions[3] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 3, true);
	actions[4] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 4, true);
	actions[5] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 5, true);
	actions[6] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 6, true);
	actions[7] = new NamedParameterizedSimpleAction<RCSocketController>("Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 7, true);
	
	//Define Opposite Action / Antagonist
	//RC1
	actions[0]->setAntagonist("Group 1", actions[1]);
	actions[1]->setAntagonist("Group 1", actions[0]);
	//RC2
	actions[2]->setAntagonist("Group 2", actions[3]);
	actions[3]->setAntagonist("Group 2", actions[2]);
	//RC3
	actions[4]->setAntagonist("Group 3", actions[5]);
	actions[5]->setAntagonist("Group 3", actions[4]);
	//RC4
	actions[6]->setAntagonist("Group 4", actions[7]);
	actions[7]->setAntagonist("Group 4", actions[6]);


	//Initialize ActionChains
	for (uint8_t i = 0; i < ACTIONCHAINS_NUM; i++) {
		actionchains[i] = new ActionChain(i);
	}

	//Start Taskmanager
	taskmanager = new TaskManager();

	//Initialize Trigger
	for (int tcategory = 0; tcategory < TRIGGER_TYPES; tcategory++) {
		for (int tset = 0; tset < TRIGGER_SETS; tset++) {
			if (tcategory == 0) trigger[tcategory][tset] = new TimeTrigger(tset);
			else {
				trigger[tcategory][tset] = new SensorTrigger(tset, sensors[tcategory - 1]);
			}
		}
	}

	//Initialize Rulesets
	for (uint8_t k = 0; k < RULESETS_NUM; k++) {
		rulesets[k] = new RuleSet(k);
	}
	
	if (DEBUG_RESET == false) {
		if (Setting::loadSettings("_CURRENTCONFIG.JSON") == false) {
			LOGMSG(F("[Setup]"), F("WARNING: Did not load primary config file"), F("Hardreset"), DEBUG_RESET, "");
			String keys[] = { "" };
			String values[] = { "" };
			logengine.addLogEntry(WARNING, "Main", "Did not load primary config file", keys, values, 0);

			if (Setting::loadSettings("BACKUPCONFIG.JSON") == false) {
				LOGMSG(F("[Setup]"), F("WARNING: Did not load backup config file"), F("Hardreset"), DEBUG_RESET, "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(WARNING, "Main", "Did not load backup config file", keys, values, 0);

				if (Setting::loadSettings("DEFAULTCONFIG.JSON") == false) {
					LOGMSG(F("[Setup]"), F("WARNING: Did not load default config file"), F("Hardreset"), DEBUG_RESET, "");
					String keys[] = { "" };
					String values[] = { "" };
					logengine.addLogEntry(WARNING, "Main", "Did not load default config file. Setting hard coded values", keys, values, 0);

					Setting::reset();
				}
			}
		}
	}
	else {
		LOGMSG(F("[Setup]"), F("WARNING: Hard Reset Flag set. Setting hard coded values"), F("Hardreset"), DEBUG_RESET, "");
		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(WARNING, "Main", "Reset Flag set. Setting hard coded values", keys, values, 0);

		Setting::reset();
	}
	//Wifi ESP2866
	pinMode(ESP_CONTROL_PIN, OUTPUT);
	digitalWrite(ESP_CONTROL_PIN, HIGH);
	Serial2.begin(115200);
	WiFi.init(&Serial2);

	//Convert SSID and PW to char[]
	char ssid[wifi_ssid.length()+1];
	wifi_ssid.toCharArray(ssid, wifi_ssid.length()+1);

	char pw[wifi_pw.length()+1];
	wifi_pw.toCharArray(pw, wifi_pw.length()+1);

	int status = WL_IDLE_STATUS;
	uint8_t failed = 0;
	while (status != WL_CONNECTED && failed < 5) {
		LOGMSG(F("[Setup]"), F("Info: Attempting to connect to WPA SSID: "), String(wifi_ssid), "", "");
		// Connect to WPA/WPA2 network
		status = WiFi.begin(ssid, pw);
		failed++;
	}
	//Start Webserver
	webserver = new WebServer();
	webserver->begin();

	//Sync with Internet
	ntpclient = new WebTimeClient();
	long timestamp = ntpclient->getWebTime();

	if (timestamp > 0) {
		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(INFO, "Main", "Received Internet Time", keys, values, 0);

		internalRTC.updateTime(timestamp, true);
	}
	sensor_last_seconds = internalRTC.getSeconds();
	task_last_seconds = sensor_last_seconds;
}

// the loop function runs over and over again until power down or reset
void loop() {
	//Get Seconds from Clock
	cpu_current = millis();
	if (cpu_current % 10 == 0) {
		//Webserver
		webserver->checkConnection();
	}
	
	//Freeze Sensor, Logic and Taskmanager
	if (haltstate == true) {
		led[0]->turnOn();
		//RC Socket Learning
		if (rcsocketcontroller->learning == true) {
			if (rcsocketcontroller->available()) {
				rcsocketcontroller->learnPattern();
				rcsocketcontroller->resetAvailable();
			}
		}
		//more Hardware
	}
	//Regular Cycle
	else {
		//Sensor
		if (cpu_current >= next_sensor) {
			//Cycles
			next_sensor = 5 * MILLIS_SEC - (cpu_current - next_sensor) + cpu_current;

			sensor_cycles++;

			led[0]->switchState();
			LOGMSG(F("[Loop]"), F("INFO: Sensor Cycle"), String(sensor_cycles), String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), String(RealTimeClock::printTime(internalRTC.getEpochTime())));
			//LOGDEBUG4(F("Millis Cycle"), String(sensor_cycles * SENS_FRQ_SEC), F("RTC Cycle"), String(internalRTC.getEpochTime()), F("Millis Clock"), String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), F("RTC Clock"), String(RealTimeClock::printTime(internalRTC.getEpochTime())));

			//Update Sensors
			for (uint8_t i = 0; i < SENS_NUM; i++) {
				sensors[i]->update();
			}

			//Check RuleSets
			for (uint8_t i = 0; i < RULESETS_NUM; i++) {
				rulesets[i]->execute();

			}
			
			//Save Settings to SD Card
			if ((sensor_cycles % (5 * SENS_VALUES_MIN)) == 0) {
				
				LOGMSG(F("[Loop]"), F("SaveActive"), "", "", "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(INFO, "Main", "Saved Configuration", keys, values, 0);

				Setting::saveSettings("_CURRENTCONFIG.JSON");
			}

			//Backup
			if ((sensor_cycles % (15 * SENS_VALUES_MIN)) == 0) {
				LOGMSG(F("[Loop]"), F("SaveActive"), "", "", "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(INFO, "Main", "Backup Configuration", keys, values, 0);

				Setting::copyFile("_CURRENTCONFIG.JSON", "BACKUPCONFIG.JSON");
			}		
		}

		//Get Seconds from Clock
		cpu_current = millis();
		//Task Manager
		if (cpu_current >= next_task) {
			//Cycles
			next_task = 1 * MILLIS_SEC - (cpu_current - next_task) + cpu_current;
			
			//Do
			taskmanager->execute();
		}
	}
}