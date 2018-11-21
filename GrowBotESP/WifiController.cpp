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
	WiFi.softAP(settings.ap_ssid.c_str(), settings.ap_pw.c_str());
	WiFi.begin(settings.wifi_ssid.c_str(), settings.wifi_pw.c_str());
	
	udp = new WiFiUDP();
	ntpclient = new NTPClient(*udp);
	ntpclient->begin();
}

long WifiHandler::returnNetworkTime()
{
	ntpclient->update();
	LOGMSG(F("[WifiHandler]"), F("UTC:"), String(ntpclient->getFormattedDate()), String(ntpclient->getFormattedTime()), String(ntpclient->getEpochTime()));

	return ntpclient->getEpochTime();
}

void WifiHandler::WiFiEvent(WiFiEvent_t event)
{
	switch (event) {

	case SYSTEM_EVENT_AP_START:
		//can set ap hostname here
		WiFi.softAPsetHostname(settings.ap_ssid.c_str());
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_START)"), F("IP"), WiFi.softAPIP(), "");
		//enable ap ipv6 here
		WiFi.softAPenableIpV6();
		break;
	case SYSTEM_EVENT_STA_START:
		//set sta hostname here
		WiFi.setHostname(settings.ap_ssid.c_str());
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_START)"), F("SSID"), settings.ap_ssid, "");
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_CONNECTED)"), F("Enable IpV6"), "", "");
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6:
		//both interfaces get the same event
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STA_GOT_IP6)"), F("Local IPv6"), WiFi.localIPv6(), "");
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STA_GOT_IP6)"), F("Soft AP IPv6"), WiFi.softAPIPv6(), "");
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_GOT_IP)"), F("Connected to Local Wifi"), F("Local IPv4"), WiFi.localIP());
		wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_GOT_IP)"), F("Disconnected with Local Wifi"), F("Reconnecting ..."), "");
		wifi_connected = false;
		delay(5000);
		WiFi.begin(settings.wifi_ssid.c_str(), settings.wifi_pw.c_str());
		break;
	default:
		break;
	}
}
