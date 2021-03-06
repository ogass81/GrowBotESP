// Definitions.h

#ifndef _DEFINITIONS_h
#define _DEFINITIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GROWBOT_FIRMWARE 5

//Tact
#define TASK_FRQ_SEC 1
#define SENS_FRQ_SEC 5
#define HALTSTATE 20
#define MILLIS_SEC 1000

//Log
#define NUM_MONTH 4
#define SENS_VALUES_MIN (60 / SENS_FRQ_SEC)
#define SENS_VALUES_HOUR 60  //every 60 seconds for one hour
#define SENS_VALUES_DAY 288 // every 5 mintutes for one day
#define SENS_VALUES_MONTH (336 * NUM_MONTH ) // every hour for six month

//FS1000A
#define TX_DATA_PIN 16
#define RX_DATA_PIN 17
#define RC_SOCKETS 14
#define RC_SIGNALS 5
#define RC_REPEAT 5

//Arduino JSON
#define JSONBUFFER_SIZE 1000
#define JSONCHAR_SIZE 3000


//Distance
#define DIST1_TRIG 33
//Also Analog Read for Infrared Sensor
#define DIST1_ECHO 32

#define DIST2_TRIG 27
#define DIST2_ECHO 26

//Settings
#define DEBUG_RESET false

//Wifi
#define WIFI_TIMEOUT 120
#define WATCHDOG true
#define WATCHDOG_TIMEOUT 5 * (60 / SENS_FRQ_SEC)
#define NTP true
#define NTP_TIMEOUT 30

//Sensors
#define SENS_NUM 9

//Rules Enginge
//Trigger Categories and number for each category
#define TRIGGER_TYPES 12
#define TRIGGER_SETS 16

//Number of Rulesssets
#define RULESETS_NUM 32
#define RULESETS_TRIGGER 4
#define RULESETS_ACTIONS 2

//Number of Actions
#define ACTIONS_NUM RC_SOCKETS + (TRIGGER_SETS * 4) + 1
#define ACTIONCHAINS_NUM 24
#define ACTIONCHAIN_LENGTH 16

//Task Manager
#define TASK_QUEUE_LENGTH 240 //MAX 255!!!
#define ACTIONCHAIN_TASK_MAXDURATION 180
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
#define LOGBUFFER_SIZE 8
#define LOGBUFFER_FLUSH 1500

//REST API 
#define REST_URI_DEPTH 4

enum RelOp { SMALLER, EQUAL, GREATER, NOTEQUAL };
enum BoolOp { AND, OR, NOT };
enum Interval { REALTIME, TENSEC, TWENTYSEC, THIRTYSEC, ONEMIN, TWOMIN, FIVEMIN, TENMIN, QUARTER, TWENMIN, HALF, FORTMIN, ONE, TWO, THREE, FOUR, SIX, TWELVE, DAILY, BIDAILY, WEEKLY, BIWEEKLY};
enum Scope { LIST, HEADER, DETAILS, AVG, DATE_MIN, DATE_HOUR, DATE_DAY, DATE_MONTH, DATE_ALL, ACTIVE};
enum Sort {ASC, DESC, RAW};
enum TriggerTypes { TIME, SENSOR, COUNTER, SWITCH };
enum LogTypes { INFO, DEBUG, ERROR, WARNING, ACTION};
enum SensorTypes { TEMPERATURE, HUMIDITY, PRESSURE, DISTANCE, SOILMOISTURE, HEIGHT, OTHER };

#define LOGLEVEL 2

#define LOGMSG(classname, msg, par1, par2, par3)   if(LOGLEVEL >1) { Serial.print(classname); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGMSG2(classname, msg, par1, par2, par3)   if(LOGLEVEL >2) { Serial.print(classname); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3);  }
#define LOGDEBUG(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >3) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGDEBUG1(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >4) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGDEBUG2(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >5) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }
#define LOGDEBUG3(classname, method, msg, par1, par2, par3)   if(LOGLEVEL >6) { Serial.print(classname); Serial.print(" "); Serial.print(method); Serial.print(" "); Serial.print(msg); Serial.print(" "); Serial.print(par1); Serial.print(" "); Serial.print(par2); Serial.print(" "); Serial.println(par3); }

#define LOGDEBUG4(label1, value1, label2, value2, label3, value3, label4, value4)   if(LOGLEVEL > 0) { Serial.print(label1); Serial.print(":"); Serial.print(value1); Serial.print(" "); Serial.print(label2); Serial.print(":"); Serial.print(value2);  Serial.print(" ");  Serial.print(label3); Serial.print(":"); Serial.print(value3);  Serial.print(" ");  Serial.print(label4); Serial.print(":"); Serial.println(value4);}

#endif
