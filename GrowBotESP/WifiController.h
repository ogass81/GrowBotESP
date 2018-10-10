// WifiController.h

#ifndef _WIFICONTROLLER_h
#define _WIFICONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "Setting.h"

extern Setting settings;
//extern WiFiUDP udp;
//extern NTPClient ntpclient;

class WifiHandler : WiFiClass {
public:
	volatile bool wifi_connected = false;
	WiFiUDP *udp;
	NTPClient *ntpclient;

	WifiHandler();
	
	void begin();
	long returnNetworkTime();

	void WiFiEvent(WiFiEvent_t event);


};

#endif

