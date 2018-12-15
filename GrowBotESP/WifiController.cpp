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
	String keys[] = { "Wifi Event" };
	String values[] = { String(event) };

	switch (event) {
	case SYSTEM_EVENT_WIFI_READY:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_WIFI_READY)"), F("Wifi Interface ready"), "", "");
		break;
	case SYSTEM_EVENT_SCAN_DONE:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_SCAN_DONE)"), F("Wifi Scan completed"), "", "");
		break;
	case SYSTEM_EVENT_AP_START:
		WiFi.softAPsetHostname(settings.ap_ssid.c_str());
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_START)"), F("IP"), WiFi.softAPIP(), "");
		WiFi.softAPenableIpV6();
		break;
	case SYSTEM_EVENT_AP_STOP:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_SCAN_DONE)"), F("WiFi access point  stopped"), "", "");
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STACONNECTED)"), F("Client connected to internal Access point"), "", "");
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STACONNECTED)"), F("Client disconnected internal from access point"), "", "");
		break;
	case SYSTEM_EVENT_STA_START:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_START)"), F("Wifi station started"), "", "");
		WiFi.setHostname(settings.ap_ssid.c_str());
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_START)"), F("SSID"), settings.ap_ssid, "");
		break;
	case SYSTEM_EVENT_STA_STOP:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_START)"), F("Wifi station stopped"), "", "");
		wifi_connected = false; 

		logengine.addLogEntry(ACTION, "Wifi", "Wifi station stopped", keys, values, 0);

		delay(1000);
		WiFi.begin(settings.wifi_ssid.c_str(), settings.wifi_pw.c_str());
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_CONNECTED)"), F("Client connected to router"), "", "");
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_GOT_IP)"), F("Client disconnected from router"), F("Reconnecting ..."), "");
		wifi_connected = false;

		logengine.addLogEntry(ACTION, "Wifi", "Client disconnected from router", keys, values, 0);

		delay(1000);
		WiFi.reconnect();
		break;
	case SYSTEM_EVENT_GOT_IP6:
		//both interfaces get the same event
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STA_GOT_IP6)"), F("Local IPv6"), WiFi.localIPv6(), "");
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_AP_STA_GOT_IP6)"), F("Soft AP IPv6"), WiFi.softAPIPv6(), "");
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_GOT_IP)"), F("Connected to Local Wifi"), F("Local IPv4"), WiFi.localIP());
		wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
		LOGMSG(F("[WifiHandler]"), F("WifiEvent(SYSTEM_EVENT_STA_LOST_IP)"), F("Lost IP Address"), F("Reconnecting ..."), "");
		wifi_connected = false;
		
		logengine.addLogEntry(ACTION, "Wifi", "Lost IP Address", keys, values, 0);

		delay(1000);
		WiFi.reconnect();
		break;
	default:
		break;
	}
}
