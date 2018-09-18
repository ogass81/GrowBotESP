/*
 Name:		GrowBotDue.ino
 Created:	13.01.2017 21:11:47
 Author:	ogass
*/

////Helper

#include "Definitions.h"

//Hardware Libaries
#include <Ultrasonic.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <BME280I2C.h>
#include <DHT.h>
#include "RealTimeClock.h"
#include "Led.h"
#include <WiFi.h>

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


//Temperature Sensor
BME280I2C bme;

//Ultrasonic Sensor
Ultrasonic distance1(DIST1_TRIG, DIST1_ECHO);
Ultrasonic distance2(DIST2_TRIG, DIST2_ECHO);

//RealTimeClock
RealTimeClock internalRTC;

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

LogEngine logengine;


void setup() {
	// initialize serial for debugging
	Serial.begin(115200);

	delay(3000);

	//Status Led
	led[0] = new Led(LED1, false);
	led[1] = new Led(LED2, false);
	led[2] = new Led(LED3, false);
	
	
	led[1]->turnOn();

	//Initialize FileSystem / SD Card
	if (!SD.begin()) {
		LOGMSG(F("[Setup]"), F("ERROR: Cannot initialize SD card"), "", "", "");
	}
	else {
		LOGMSG(F("[Setup]"), F("INFO: SD Card"), SD.cardType(), (long)SD.cardSize(), ""); 
	}

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
	Wire.begin();

	while (!bme.begin())
	{
		LOGMSG(F("[Setup]"), F("ERROR: Cannot initialize BME sensor"), "", "", "");
		delay(1000);
	}

	//433Mhz
	rcsocketcontroller = new RCSocketController(TX_DATA_PIN, RX_DATA_PIN);

	//Initialize Sensors
	sensors[0] = new	BMETemperature(&bme, true, F("Temperature"), F("C"), -127, -50, 100);
	sensors[1] = new 	BMEHumidity(&bme, true, F("Humidity"), F("%"), -127, 0, 100);
	sensors[2] = new 	BMEPressure(&bme, true, F("Pressure"), F("kPa"), -127, 50, 150);
	sensors[3] = new 	CapacityMoistureSensor<short>(IN_MOS_1, 12, 10, ADC_11db, true, F("Soil 1"), F("%"), -1, 0, 1000, 150, 600);
	sensors[4] = new 	CapacityMoistureSensor<short>(IN_MOS_2, 12, 10, ADC_11db, true, F("Soil 2"), F("%"), -1, 0, 1000, 150, 600);
	sensors[5] = new 	CapacityMoistureSensor<short>(IN_MOS_3, 12, 10, ADC_11db, true, F("Soil 3"), F("%"), -1, 0, 1000, 150, 600);
	sensors[6] = new 	CapacityMoistureSensor<short>(IN_MOS_4, 12, 10, ADC_11db, true, F("Soil 4"), F("%"), -1, 0, 1000, 150, 600);
	sensors[7] = new 	DistanceLampSensor(&distance2, true, F("Distance Lamp"), F("cm"), -1, 0, 400);
	sensors[8] = new 	HeightSensor(&distance1, &distance2, true, F("Height Sensor"), F("cm"), -1, 0, 400);


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
		if (Setting::loadSettings("/_CURRENTCONFIG.JSON") == false) {
			LOGMSG("[Setup]", "WARNING: Did not load primary config file", "Hardreset", DEBUG_RESET, "");
			String keys[] = { "" };
			String values[] = { "" };
			logengine.addLogEntry(WARNING, "Main", "Did not load primary config file", keys, values, 0);

			if (Setting::loadSettings("/_CURRENTCONFIG.JSON.BAK") == false) {
				LOGMSG("[Setup]", "WARNING: Did not load backup config file", "Hardreset", DEBUG_RESET, "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(WARNING, "Main", "Did not load backup config file", keys, values, 0);

				if (Setting::loadSettings("/DEFAULTCONFIG.JSON") == false) {
					LOGMSG("[Setup]", "WARNING: Did not load default config file", "Hardreset", DEBUG_RESET, "");
					String keys[] = { "" };
					String values[] = { "" };
					logengine.addLogEntry(WARNING, "Main", "Did not load default config file. Setting hard coded values", keys, values, 0);

					Setting::reset();
				}
			}
		}
	}
	else {
		LOGMSG("[Setup]", "WARNING: Hard Reset Flag set. Setting hard coded values", "Hardreset", DEBUG_RESET, "");
		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(WARNING, "Main", "Reset Flag set. Setting hard coded values", keys, values, 0);

		Setting::reset();
	}

	//Wifi
	//Convert SSID and PW to char[]
	char ssid[wifi_ssid.length()+1];
	wifi_ssid.toCharArray(ssid, wifi_ssid.length()+1);

	char pw[wifi_pw.length()+1];
	wifi_pw.toCharArray(pw, wifi_pw.length()+1);

	WiFi.begin(ssid, pw);
	LOGMSG("[Setup]", "Info: Attempting to connect to WPA SSID: ", String(wifi_ssid), "", "");

	uint8_t failed = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
		failed++;

		if (failed > WIFI_TIMEOUT) {
			while (true) {
				led[2]->switchState();
				delay(500);
			}
		}
	}

	if (WiFi.status() == WL_CONNECTED) {
		LOGMSG("[Setup]", "Connected to WPA SSID: ", String(wifi_ssid), "IP Address: ", String(WiFi.localIP()));
	}

	//Start Webserver
	webserver = new WebServer();
	webserver->begin(80);

	//Sync with Internet
	ntpclient = new WebTimeClient();
	long timestamp = ntpclient->getWebTime();

	if (timestamp > 0) {
		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(INFO, "RealTimeClock", "Received Internet Time", keys, values, 0);
		LOGDEBUG2("[RealTimeClock]", "syncSensorCycles()", "OK: Set new sensor cycle", String(sensor_cycles), "", "");

		internalRTC.updateTime(timestamp, true);
	}
}


// the loop function runs over and over again until power down or reset
void loop() {
 
	//Get Seconds from Clock
	cpu_current = millis();
	if (cpu_current % 5 == 0) {
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
			LOGMSG(F("[Loop]"), F("INFO: Sensor Cycle"), String(sensor_cycles), String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), "");
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

				xTaskCreate(Setting::saveActiveConfig, "FileAccess", 16000, NULL, 1, NULL);
			}

			//Backup
			if ((sensor_cycles % (15 * SENS_VALUES_MIN)) == 0) {
				LOGMSG(F("[Loop]"), F("SaveActive"), "", "", "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(INFO, "Main", "Backup Configuration", keys, values, 0);

				xTaskCreate(Setting::backupConfig, "FileAccess", 32000, NULL, 1, NULL);
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
