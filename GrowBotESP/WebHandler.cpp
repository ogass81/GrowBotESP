// 
// 
// 

#include "WebHandler.h"

Webhandler::Webhandler()
{
}

void Webhandler::begin()
{
	classWebServer.on("/", HTTP_GET, std::bind(&Webhandler::loginGet, this, std::placeholders::_1));
	classWebServer.on("/action", HTTP_GET, std::bind(&Webhandler::actionGet, this, std::placeholders::_1));
	classWebServer.on("/actionchain", HTTP_GET, std::bind(&Webhandler::actionchainGet, this, std::placeholders::_1));
	classWebServer.on("/log", HTTP_GET, std::bind(&Webhandler::logGet, this, std::placeholders::_1));
	classWebServer.on("/rcsocket", HTTP_GET, std::bind(&Webhandler::rcsocketGet, this, std::placeholders::_1));
	classWebServer.on("/ruleset", HTTP_GET, std::bind(&Webhandler::rulesetGet, this, std::placeholders::_1));
	classWebServer.on("/sensor", HTTP_GET, std::bind(&Webhandler::sensorGet, this, std::placeholders::_1));
	classWebServer.on("/setting", HTTP_GET, std::bind(&Webhandler::settingGet, this, std::placeholders::_1));
	classWebServer.on("/trigger", HTTP_GET, std::bind(&Webhandler::triggerGet, this, std::placeholders::_1));
	
	AsyncCallbackJsonWebHandler* handler;

	handler = new AsyncCallbackJsonWebHandler("/actionchain", std::bind(&Webhandler::actionchainPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	handler = new AsyncCallbackJsonWebHandler("/rcsocket", std::bind(&Webhandler::rcsocketPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	handler = new AsyncCallbackJsonWebHandler("/ruleset", std::bind(&Webhandler::rulesetPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	handler = new AsyncCallbackJsonWebHandler("/sensor", std::bind(&Webhandler::sensorPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	handler = new AsyncCallbackJsonWebHandler("/setting", std::bind(&Webhandler::settingPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	handler = new AsyncCallbackJsonWebHandler("/trigger", std::bind(&Webhandler::triggerPatch, this, std::placeholders::_1, std::placeholders::_2));
	classWebServer.addHandler(handler);

	classWebServer.onNotFound(std::bind(&Webhandler::unknownGet, this, std::placeholders::_1));

	classWebServer.begin();
}

void Webhandler::loginGet(AsyncWebServerRequest * request)
{
	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	AsyncJsonResponse * response = new AsyncJsonResponse();
	response->addHeader("Server", "GrowAI");
	JsonObject& root = response->getRoot();
	settings.serializeJSON(root);
	response->setLength();
	request->send(response);

	LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings: GET"), "Initial Login", "");
}

void Webhandler::actionGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "ACTION";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < ACTIONS_NUM; i++) {
			JsonObject& element = list.createNestedObject();
			actions[i]->serializeJSON(element, LIST);
		}
		response->setLength();
		request->send(response);
	}
	else if (uri[1] != "" && uri[1].toInt() < ACTIONS_NUM) {
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			actions[uri[1].toInt()]->serializeJSON(root, DETAILS);
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), String(uri[1]), "");

			response->setLength();
			request->send(response);
		}
		else if (uri[2] == "execute") {
			actions[uri[1].toInt()]->execute();
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: EXECUTE"), String(uri[1]), "");
			//actions[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), String(uri[1]), "");
			request->send(200);
		}
		else {
			LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
			request->send(404);
		}
	}
	else request->send(404);
}

void Webhandler::actionchainGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "ACTIONCHAIN";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < ACTIONCHAINS_NUM; i++) {
			JsonObject& element = list.createNestedObject();
			actionchains[i]->serializeJSON(element, LIST);
		}
		response->setLength();
		request->send(response);
	}
	else if (uri[1] != "" && uri[1].toInt() < ACTIONCHAINS_NUM) {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		actionchains[uri[1].toInt()]->serializeJSON(root, DETAILS);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), String(uri[1]), "");

		response->setLength();
		request->send(response);
	}
	else {
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
		request->send(404);
	}
}

void Webhandler::logGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		DynamicJsonBuffer& buffer = response->getBuffer();

		logengine.serializeJSON(root, buffer, 0, 0);

		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(0), String(0));
	}
	else if (uri[1] == "reset") {
		logengine.reset();
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Reset Log file"), "", "");
	}
	else {
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			DynamicJsonBuffer& buffer = response->getBuffer();

			logengine.serializeJSON(root, buffer, uri[1].toInt(), 0);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(uri[1].toInt()), String(0));
		}
		else {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			DynamicJsonBuffer& buffer = response->getBuffer();

			logengine.serializeJSON(root, buffer, uri[1].toInt(), uri[2].toInt());
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(uri[1].toInt()), String(uri[2].toInt()));
		}
	}
}

void Webhandler::rcsocketGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "" || uri[1].toInt() >= RC_SOCKETS) {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();

		DynamicJsonBuffer& buffer = response->getBuffer();

		rcsocketcontroller->serializeJSON(root, LIST);

		response->setLength();
		request->send(response);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ### Remote Controlled Socket: GET"), F("List View"), "");
	}
	else if (uri[1] != "" && uri[1].toInt() < RC_SOCKETS) {
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();

			DynamicJsonBuffer& buffer = response->getBuffer();

			rcsocketcontroller->serializeJSON(root, DETAILS, uri[1].toInt());

			response->setLength();
			request->send(response);
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ### Remote Controlled Socket: GET"), F("List View"), "");

		}
		else if (uri[2] != "" && uri[2] == "learn_on") {
			rcsocketcontroller->learningmode_on(uri[1].toInt());
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: Learning mode activated"), String(uri[1]), "");
			request->send(200);
		}
		else if (uri[2] != "" && uri[2] == "learn_off") {
			rcsocketcontroller->learningmode_off();
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: Learning mode deactivated"), String(uri[1]), "");
			request->send(200);
		}
		else if (uri[2] != "" && uri[2] == "reset") {
			rcsocketcontroller->resetSettings(uri[1].toInt());
			Serial.println(uri[1].toInt());
			request->send(200);
		}
		else {
			LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
			request->send(404);
		}
	}
}

void Webhandler::rulesetGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "RULESET";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < RULESETS_NUM; i++) {
			JsonObject& element = list.createNestedObject();
			rulesets[i]->serializeJSON(element, LIST);
		}
		response->setLength();
		request->send(response);
	}
	else if (uri[1] != "" && uri[1].toInt() < RULESETS_NUM) {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();

		rulesets[uri[1].toInt()]->serializeJSON(root, DETAILS);
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: GET"), String(uri[1]), "");
	}
	else {
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
		request->send(404);
	}
}

void Webhandler::sensorGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();;

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "SENSOR";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < SENS_NUM; i++) {
			JsonObject& element = list.createNestedObject();
			sensors[i]->serializeJSON(element, LIST);
		}
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), F("List View"), "");
	}
	else if (uri[1] != "" && uri[1].toInt() < SENS_NUM) {
		//Decide what kind of sensor data to send
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();

			sensors[uri[1].toInt()]->serializeJSON(root, HEADER);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: DETAILS"));
		}
		else if (uri[2] == "details") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DETAILS);
			response->setLength();
			request->send(response);
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: AVG"));
		}
		else if (uri[2] == "avg") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, AVG);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: AVG"));
		}
		else if (uri[2] == "date_min") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_MIN);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: MIN"));
		}
		else if (uri[2] == "date_hour") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_HOUR);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: HOUR"));
		}
		else if (uri[2] == "date_day") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_DAY);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: DAY"));
		}
		else if (uri[2] == "date_month") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_MONTH);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: MONTH"));
		}
		else if (uri[2] == "date_all") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_ALL);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: ALL"));
		}
		//Commands
		else if (uri[2] == "lower") {
			sensors[uri[1].toInt()]->setLowerThreshold();
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Set Lower Threshold"));
			request->send(200);
		}
		else if (uri[2] == "upper") {
			sensors[uri[1].toInt()]->setUpperThreshold();
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Set Upper Threshold"));
			request->send(200);
		}
		else if (uri[2] == "reset") {
			sensors[uri[1].toInt()]->reset();
			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Reset"));
			request->send(200);
		}
		else {
			LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
			request->send(404);
		}
	}
	else {
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
		request->send(404);
	}

}

void Webhandler::settingGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings: GET"), "Current Settings", "");
	}
	else if (uri[1] == "default") {
		settings.loadDefaultConfig();
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: LOAD"), "Default Config", "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings: GET"), "Default Settings", "");
	}
	else if (uri[1] == "active") {
		settings.loadActiveConfig();
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: LOAD"), "Active Config", "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings: GET"), "Active Settings", "");
	}
	else if (uri[1] == "reset") {
		settings.reset();
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: RESET"), "", "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings: GET"), "Active Settings", "");
	}
	else request->send(404);
}

void Webhandler::triggerGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "TCAT";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < TRIGGER_TYPES; i++) {
			if (trigger[i][0] != NULL) {
				JsonObject& item = list.createNestedObject();
				item["typ"] = static_cast<int>(trigger[i][0]->type);

				if (trigger[i][0]->type == 0) {
					item["tit"] = "Timer";
				}
				else {
					item["tit"] = "Comparator";
				}
				item["src"] = trigger[i][0]->getSource();
			}
		}
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET Categories"), String(uri[1]), String(uri[2]));
	}
	else if (uri[1] == "all") {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "TCAT";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < TRIGGER_TYPES; i++) {
			JsonObject& cat = list.createNestedObject();
			cat["src"] = trigger[i][0]->getSource();
			cat["typ"] = static_cast<int>(trigger[i][0]->type);
			JsonArray& trig = cat.createNestedArray("trig");
			for (uint8_t j = 0; j < TRIGGER_SETS; j++) {
				JsonObject& item = trig.createNestedObject();
				item["tit"] = trigger[i][j]->getTitle();
				item["act"] = trigger[i][j]->active;
			}
		}
		response->setLength();
		request->send(response);

		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET Categories & Trigger"), String(uri[1]), String(uri[2]));
	}
	else if (uri[1] != "" && uri[1].toInt() < TRIGGER_TYPES) {
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			root["obj"] = "TRIGGER";

			JsonArray&	list = root.createNestedArray("list");
			for (uint8_t i = 0; i < TRIGGER_SETS; i++) {
				JsonObject& element = list.createNestedObject();
				trigger[uri[1].toInt()][i]->serializeJSON(element, LIST);
			}
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET"), F("List View for category"), String(uri[1]));
		}
		else if (uri[2] != "" && uri[2].toInt() < TRIGGER_SETS) {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();

			trigger[uri[1].toInt()][uri[2].toInt()]->serializeJSON(root, DETAILS);
			response->setLength();
			request->send(response);

			LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET"), String(uri[1]), String(uri[2]));
		}
		else {
			request->send(404);
			LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
		}
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::unknownGet(AsyncWebServerRequest * request)
{
	request->send(404);
}

void Webhandler::actionchainPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();
	
	if (uri[1] != "" && uri[1].toInt() < ACTIONS_NUM) {
		actionchains[uri[1].toInt()]->deserializeJSON(json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actionchain Action: SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::rcsocketPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] != "" && uri[1].toInt() < RC_SOCKETS) {
		rcsocketcontroller->deserializeJSON(uri[1].toInt(), json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}

}

void Webhandler::rulesetPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] != "" && uri[1].toInt() < RULESETS_NUM) {
		rulesets[uri[1].toInt()]->deserializeJSON(json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: SET"), String(uri[1]), "");

	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::sensorPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] != "" && uri[1].toInt() < SENS_NUM) {
		sensors[uri[1].toInt()]->deserializeJSON(json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Sensor Action: SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::settingPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[1] == "") {
		settings.deserializeJSON(json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
	}
	else if (uri[1] == "default") {
		settings.deserializeJSON(json);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
		settings.saveDefaultConfig();
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SAVE to Default"), "", "");
	}
	else if (uri[1] == "active") {
		settings.deserializeJSON(json);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
		settings.saveActiveConfig();
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SAVE to Active"), "", "");
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::triggerPatch(AsyncWebServerRequest * request, JsonVariant & json)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();

	if (uri[2] != "" && uri[2].toInt() < TRIGGER_SETS) {
		trigger[uri[1].toInt()][uri[2].toInt()]->deserializeJSON(json);
		request->send(200);
		LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: SET"), String(uri[1]), String(uri[2]));
	}
	else {
		request->send(404);
		LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
	}
}

void Webhandler::breakupURL(String uri[REST_URI_DEPTH], char * url)
{
	url = strtok(url, "/");

	int8_t i = 0;
	while (i < REST_URI_DEPTH) {
		if (url != NULL) {
			uri[i] = url;
			url = strtok(NULL, "/");
		}
		else uri[i] = "";
		i++;
	}
}
