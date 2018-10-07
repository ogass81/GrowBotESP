// 
// 
// 

#include "WifiController.h"

WifiHandler::WifiHandler()
{
}

void WifiHandler::begin()
{
	WiFi.disconnect(true);
	WiFi.onEvent(std::bind(&WifiHandler::WiFiEvent, this, std::placeholders::_1));
	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.softAP(settings.ap_ssid, settings.ap_pw);
	WiFi.begin(settings.wifi_ssid, settings.wifi_pw);
	ntpclient.begin();
}

long WifiHandler::returnNetworkTime()
{
	if (WiFi.status() == WL_CONNECTED) {
		ntpclient.update();
		return ntpclient.getEpochTime();
		LOGDEBUG("WifiHandler", "returnNetworkTime()", "UTC: ", ntpclient.getFormattedDate(), ntpclient.getFormattedTime(), "");
	}
	else return 0;
}

void WifiHandler::wifiOnConnect()
{
	Serial.println("STA Connected");
	Serial.print("STA IPv4: ");
	Serial.println(WiFi.localIP());
}

void WifiHandler::wifiOnDisconnect()
{
	Serial.println("STA Disconnected");
	delay(1000);
	WiFi.begin(settings.wifi_ssid, settings.wifi_pw);
}

void WifiHandler::WiFiEvent(WiFiEvent_t event)
{
	switch (event) {

	case SYSTEM_EVENT_AP_START:
		//can set ap hostname here
		WiFi.softAPsetHostname(settings.ap_ssid);
		Serial.print("AP IPv4: ");
		Serial.println(WiFi.softAPIP());
		//enable ap ipv6 here
		WiFi.softAPenableIpV6();
		break;

	case SYSTEM_EVENT_STA_START:
		//set sta hostname here
		WiFi.setHostname(settings.ap_ssid);
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6:
		//both interfaces get the same event
		Serial.print("STA IPv6: ");
		Serial.println(WiFi.localIPv6());
		Serial.print("AP IPv6: ");
		Serial.println(WiFi.softAPIPv6());
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		wifiOnConnect();
		wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		wifi_connected = false;
		wifiOnDisconnect();
		break;
	default:
		break;
	}
}
