/*
 Name:		GrowBotDue.ino
 Created:	13.01.2017 21:11:47
 Author:	ogass
*/



////Helper
#include "Definitions.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

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
#include "WifiController.h"


//Core Features
#include "Action.h"
#include "LogEngine.h"
#include "TaskManager.h"
#include "Setting.h"
#include "WebHandler.h"

//Controller Objects
#include "Trigger.h"
#include "ActionChain.h"
#include "Ruleset.h"
#include "RCSocketController.h"
#include "AdvancedSensor.h"


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


//Status LED
Led *led[3];

//Temperature Sensor
BME280I2C bme;

//Ultrasonic Sensor
Ultrasonic distance1(DIST1_TRIG, DIST1_ECHO);
Ultrasonic distance2(DIST2_TRIG, DIST2_ECHO);

//Settings
Setting settings("/_CURRENTCONFIG.JSON", "/DEFAULTCONFIG.JSON.JSON", "/_CURRENTCONFIG.JSON.BAK", "/LOG.JSON", "wgempire", "ert456sdf233sa!!!", "growAI", "1234qwert", "admin", "");

//RealTimeClock
RealTimeClock internalRTC;

//433Mhz
RCSocketController *rcsocketcontroller;

//Wifi
//WebServer *webserver;
WifiHandler wifihandler;
Webhandler webhandler;
//WiFiUDP udp;
//NTPClient ntpclient(udp);

//Modules
//Sensors: Abstraction of all Sensors
SensorInterface *sensors[SENS_NUM];

//Actions: Abstraction of all Actors 
Action *actions[ACTIONS_NUM];
ActionChain *actionchains[ACTIONCHAINS_NUM];

//Task Manager
TaskManager *taskmanager;

//Trigger: Constraints for particular sensors
Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];

//Rulesets: Trigger Action Bundles
RuleSet *rulesets[RULESETS_NUM];

LogEngine logengine("/log.json");


void setup() {
	// initialize serial for debugging
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector   
	Serial.begin(115200);

	delay(3000);

	//Status Led
	led[0] = new Led(LED1, false);
	led[1] = new Led(LED2, false);
	led[2] = new Led(LED3, false);

	LOGMSG("[Setup]", "Beginning Boot Sequence", "", "", "");
	led[0]->turnOn();

	//Initialize FileSystem / SD Card
	LOGMSG("[Setup]", "Initializing SD Card", "", "", "");
	//Hardware Fix for unstable SD init
	pinMode(23, INPUT_PULLUP);
	if (!SD.begin()) {
		LOGMSG(F("[Setup]"), F("ERROR: Cannot initialize SD card"), "", "", "");
	}
	else {
		LOGMSG(F("[Setup]"), F("INFO: SD Card"), SD.cardType(), (long)SD.cardSize(), ""); 
	}

	//Start Temp/Humidity Sensor
	LOGMSG("[Setup]", "Initializing BME Sensor", "", "", "");
	Wire.begin();

	while (!bme.begin())
	{
		LOGMSG(F("[Setup]"), F("ERROR: Cannot initialize BME sensor"), "", "", "");
		delay(1000);
	}

	//433Mhz
	LOGMSG("[Setup]", "Initializing 433 Mhz Controller", "", "", "");
	rcsocketcontroller = new RCSocketController(TX_DATA_PIN, RX_DATA_PIN);

	//Initialize Sensors
	LOGMSG("[Setup]", "Initializing Sensors", "", "", "");
	sensors[0] = new	BMETemperature(0, &bme, true, F("Temperature"), F("C"), -127.0, -50, 100);
	sensors[1] = new 	BMEHumidity(1, &bme, true, F("Humidity"), F("%"), -1, 0, 100);
	sensors[2] = new 	BMEPressure(2, &bme, true, F("Pressure"), F("kPa"), -1.0, 95.0, 105.0);
	sensors[3] = new 	CapacityMoistureSensor(3, IN_MOS_1, 12, 10, ADC_11db, true, F("Soil 1"), F("%"), -1, 0, 1000);
	sensors[4] = new 	CapacityMoistureSensor(4, IN_MOS_2, 12, 10, ADC_11db, true, F("Soil 2"), F("%"), -1, 0, 1000);
	sensors[5] = new 	CapacityMoistureSensor(5, IN_MOS_3, 12, 10, ADC_11db, true, F("Soil 3"), F("%"), -1, 0, 1000);
	sensors[6] = new 	CapacityMoistureSensor(6, IN_MOS_4, 12, 10, ADC_11db, true, F("Soil 4"), F("%"), -1, 0, 1000);
	sensors[7] = new 	DistanceLampSensor(7, &distance2, true, F("Distance Lamp"), F("cm"), -1, 0, 400);
	sensors[8] = new 	HeightSensor(8, &distance1, &distance2, true, F("Height Sensor"), F("cm"), -1, 0, 400);
	

	//Intialize Actions
	LOGMSG("[Setup]", "Initializing Actions", "", "", "");
	actions[0] = new NamedParameterizedSimpleAction<RCSocketController>(0, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 0, true);
	actions[1] = new NamedParameterizedSimpleAction<RCSocketController>(1, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 1, true);
	actions[2] = new NamedParameterizedSimpleAction<RCSocketController>(2, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 2, true);
	actions[3] = new NamedParameterizedSimpleAction<RCSocketController>(3, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 3, true);
	actions[4] = new NamedParameterizedSimpleAction<RCSocketController>(4, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 4, true);
	actions[5] = new NamedParameterizedSimpleAction<RCSocketController>(5, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 5, true);
	actions[6] = new NamedParameterizedSimpleAction<RCSocketController>(6, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 6, true);
	actions[7] = new NamedParameterizedSimpleAction<RCSocketController>(7, "Send", rcsocketcontroller, &RCSocketController::sendCode, &RCSocketController::getTitle, 7, true);

  
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
	LOGMSG("[Setup]", "Initializing Actionchains", "", "", "");
	for (uint8_t i = 0; i < ACTIONCHAINS_NUM; i++) {
		actionchains[i] = new ActionChain(i);
	}


	//Initialize Trigger
	LOGMSG("[Setup]", "Initializing Triggers", "", "", "");
	for (int tcategory = 0; tcategory < TRIGGER_TYPES; tcategory++) {
		for (int tset = 0; tset < TRIGGER_SETS; tset++) {
			if (tcategory == 0) trigger[tcategory][tset] = new TimeTrigger(tset, 0);
			else {
				trigger[tcategory][tset] = new SensorTrigger(tset, tcategory, sensors[tcategory - 1]);
			}
		}
	}

	//Initialize Rulesets
	LOGMSG("[Setup]", "Initializing Rulesets", "", "", "");
	for (uint8_t k = 0; k < RULESETS_NUM; k++) {
		rulesets[k] = new RuleSet(k);
	}

	//Initialize Settings from File
	LOGMSG("[Setup]", "Loading Settings from SD Card", "", "", "");
	settings.begin();

	//Start Taskmanager
	LOGMSG("[Setup]", "Initializing Task Manager", "", "", "");
	taskmanager = new TaskManager();

	//Wifi
	LOGMSG("[Setup]", "Initializing Wifi", "", "", "");
	wifihandler.begin();

	uint8_t timeout = 0;
	while (WiFi.status() != WL_CONNECTED) {
		if (timeout > WIFI_TIMEOUT) {
			for (uint8_t i = 0; i < WIFI_TIMEOUT; i++) {
				led[0]->switchState();
				led[1]->switchState();
				led[2]->switchState();
			}
			//ESP.restart();
		}
		else {
			delay(1000);
			led[0]->switchState();
			timeout++;
		}
	}
	led[0]->turnOn();

	//Start Webserver
	LOGMSG("[Setup]", "Starting Webserver", "", "", "");
	webhandler.begin();

	//NTP
	LOGMSG("[Setup]", "Retrieving Network Time", "", "", "");
	long timestamp = 0; 
	timeout = 0;


	while(NTP) {
		if (timeout > NTP_TIMEOUT) {
			LOGMSG("[Setup]", "ERROR: Could not retrieve Network Time", "", "", "");
			break;
		}
		else {
			if ((timestamp = wifihandler.returnNetworkTime()) == 0) {
				timeout++;
				delay(1000);
				led[0]->switchState();
			}
			else {
				internalRTC.updateTime(timestamp, true);
				break;
			}
		}
	}
	led[0]->turnOn();

	//LogEngine
	LOGMSG("[Setup]", "Initializing Log Engine", "", "", "");
	logengine.begin();

	LOGMSG("[Setup]", "Boot Sequence complete"	, String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), String(ESP.getFreeHeap()), "");
	String keys[] = { "Time", "Free Heap" };
	String values[] = { String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), String(ESP.getFreeHeap()) };
	logengine.addLogEntry(INFO, "Setup", "Bot started", keys, values, 1);
}


// the loop function runs over and over again until power down or reset
void loop() {
 
	//Get Seconds from Clock
	cpu_current = millis();

	
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
			next_sensor = 1 * MILLIS_SEC - (cpu_current - next_sensor) + cpu_current;

			sensor_cycles++;

			led[0]->switchState();
			LOGMSG(F("[Loop]"), F("INFO: Sensor Cycle"), String(sensor_cycles), String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), String(ESP.getFreeHeap()));
			//LOGDEBUG4(F("Millis Cycle"), String(sensor_cycles * SENS_FRQ_SEC), F("RTC Cycle"), String(internalRTC.getEpochTime()), F("Millis Clock"), String(RealTimeClock::printTime(sensor_cycles * SENS_FRQ_SEC)), F("RTC Clock"), String(RealTimeClock::printTime(internalRTC.getEpochTime())));

			//Update Sensors
			for (uint8_t i = 0; i < SENS_NUM; i++) {
				sensors[i]->update();
			}

			//Check RuleSets
			for (uint8_t i = 0; i < RULESETS_NUM; i++) {
				rulesets[i]->execute();

			}
			
			//Backup Settings and save Settings to SD Card
			if ((sensor_cycles % (5 * SENS_VALUES_MIN)) == 0) {
				
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(INFO, "Main", "Saved Configuration", keys, values, 0);

				xTaskCreate(Setting::asyncSaveActiveConfig, "Save Config", 16384, &settings, 1, NULL);
			}	
		}

		//Get Seconds from Clock
		cpu_current = millis();
		//Task Manager
		if (cpu_current >= next_task) {
			//Cycles
			next_task = TASK_FRQ_SEC * MILLIS_SEC - (cpu_current - next_task) + cpu_current;
			
			//Do
			taskmanager->execute();
		}
	}
}
