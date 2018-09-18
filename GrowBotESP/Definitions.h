// Definitions.h

#ifndef _DEFINITIONS_h
#define _DEFINITIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GROWBOT_FIRMWARE 2

#define SERIAL_BUFFER_SIZE 256

//SD Card Control Pin
#define SD_CONTROL_PIN 5

//Wifi
#define WIFI_TIMEOUT 30

//FS1000A
#define TX_DATA_PIN 17
#define RX_DATA_PIN 16
#define RC_SOCKETS 8
#define RC_SIGNALS 5
#define RC_REPEAT 5

//Arduino JSON
#define ARDUINOJSON_ENABLE_PROGMEM 1
#define JSONCHAR_SIZE 7500
#define JSONBUFFER_SIZE 1000
#define JSONBUFFER_SMALL 750
#define JSONBUFFER_BIG 1500

//Distance
#define DIST1_TRIG 33
#define DIST1_ECHO 32

#define DIST2_TRIG 27
#define DIST2_ECHO 26

//Network
#define PACKAGE_SIZE 1024

//Settings
#define DEBUG_RESET false

//Tact
#define TASK_FRQ_SEC 1
#define SENS_FRQ_SEC 5
#define HALTSTATE 20
#define MILLIS_SEC 1000

//Log
#define SENS_VALUES_MIN (60 / SENS_FRQ_SEC) //every 5 sec
#define SENS_VALUES_HOUR 60 //every Minute
#define SENS_VALUES_DAY 96 // every 15 Minutes
#define SENS_VALUES_MONTH 56// every 12 Hours
#define SENS_VALUES_YEAR 52 // every week

//Sensors
#define SENS_NUM 9

//Rules Enginge
//Trigger Categories and number for each category
#define TRIGGER_TYPES 5
#define TRIGGER_SETS 8

//Number of Rulesssets
#define RULESETS_NUM 32

//Number of Actions
#define ACTIONS_NUM 8
#define ACTIONCHAINS_NUM 16
#define ACTIONCHAIN_LENGTH 4

//Task Manager
#define TASK_QUEUE_LENGTH 120 //MAX 255!!!
#define ACTIONCHAIN_TASK_MAXDURATION (TASK_QUEUE_LENGTH / ACTIONCHAIN_LENGTH)
#define TASK_PARALLEL_SEC 4

//Front Panel
#define IN_MOS_1 36
#define IN_MOS_2 39
#define IN_MOS_3 34
#define IN_MOS_4 35

//LED
#define LED1 2
#define LED2 4
#define LED3 25

//LogEngine
#define LOGBUFFER_SIZE 5

//REST API 
#define REST_URI_DEPTH 4

enum RelOp { SMALLER, EQUAL, GREATER, NOTEQUAL };
enum BoolOp { AND, OR, NOT };
enum Interval { REALTIME, TENSEC, TWENTYSEC, THIRTYSEC, ONEMIN, TWOMIN, FIVEMIN, QUARTER, HALF, ONE, TWO, THREE, FOUR, SIX, TWELVE, DAILY, BIDAILY, WEEKLY, BIWEEKLY};
enum Scope { LIST, HEADER, DETAILS, AVG, DATE_MINUTE, DATE_HOUR, DATE_DAY, DATE_MONTH, DATE_YEAR, DATE_ALL};
enum TriggerTypes { TIME, SENSOR };
enum LogTypes { INFO, DEBUG, ERROR, WARNING, ACTION};
enum SensorTypes { TEMPERATURE, HUMIDITY, PRESSURE, DISTANCE, SOILMOISTURE, HEIGHT, OTHER };

#define LOGLEVEL 3

#define LOGMSG(classname, msg, par1, par2, par3)   if(LOGLEVEL >1) { Serial.print(classname); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGMSG2(classname, msg, par1, par2, par3, par4)   if(LOGLEVEL >2) { Serial.print(classname); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.print(par3);  }
#define LOGDEBUG(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >3) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGDEBUG2(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >4) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGDEBUG3(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >5) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }

#define LOGDEBUG4(label1, value1, label2, value2, label3, value3, label4, value4)   if(LOGLEVEL > 0) { Serial.print(label1); Serial.print(":"); Serial.print(value1); Serial.print(" "); Serial.print(label2); Serial.print(":"); Serial.print(value2);  Serial.print(" ");  Serial.print(label3); Serial.print(":"); Serial.print(value3);  Serial.print(" ");  Serial.print(label4); Serial.print(":"); Serial.println(value4);}

#endif