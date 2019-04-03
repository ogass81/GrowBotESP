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
	classWebServer.on("/restart", HTTP_GET, std::bind(&Webhandler::restart, this, std::placeholders::_1));

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

	LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings GET"), "Initial Login", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action GET"), F("List"), "");

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
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action: GET"), String(uri[1]), "");

			response->setLength();
			request->send(response);
		}
		else if (uri[2] == "execute") {
			actions[uri[1].toInt()]->execute();
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action EXECUTE"), String(uri[1]), "");
			request->send(200);
		}
		else {
			LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ActionChain GET"), F("List"), "");

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
			if (uri[2] == "") {
				AsyncJsonResponse * response = new AsyncJsonResponse();
				response->addHeader("Server", "GrowAI");

				JsonObject& root = response->getRoot();
				actionchains[uri[1].toInt()]->serializeJSON(root, DETAILS);
				LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ActionChain GET"), String(uri[1]), "");

				response->setLength();
				request->send(response);
		}

		else if (uri[2] == "execute") {
			actionchains[uri[1].toInt()]->execute();
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ActionChain EXECUTE"), String(uri[1]), "");
			request->send(200);
		}
	}
	else {
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: LogEntry GET Range"), F("Recent Entries"), "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		DynamicJsonBuffer& buffer = response->getBuffer();

		logengine.serializeJSON(root, buffer, 0, 0);

		response->setLength();
		request->send(response);
	}
	else if (uri[1] == "reset") {
		logengine.reset();
		request->send(200);
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Reset Log file"), "", "");
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

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: LogEntry GET Range from"), String(uri[1].toInt()), String(0));
		}
		else {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			DynamicJsonBuffer& buffer = response->getBuffer();

			logengine.serializeJSON(root, buffer, uri[1].toInt(), uri[2].toInt());
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: LogEntry GET Range from"), String(uri[1].toInt()), String(uri[2].toInt()));
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Controlled Socket GET"), F("List"), "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();

		DynamicJsonBuffer& buffer = response->getBuffer();

		rcsocketcontroller->serializeJSON(root, LIST);

		response->setLength();
		request->send(response);
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
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Controlled Socket GET"), F("Set"), String(uri[1]));

		}
		else if (uri[2] == "learn_on") {
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Learning mode activated"), F("Set"), String(uri[1]));
			rcsocketcontroller->learningmode_on(uri[1].toInt());
			request->send(200);
		}
		else if (uri[2] == "learn_off") {
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Learning mode deactivated"), F("Set"), String(uri[1]));
			rcsocketcontroller->learningmode_off();
			request->send(200);
		}
		else if (uri[2] == "reset") {
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Reset"), F("Set"), String(uri[1]));
			rcsocketcontroller->resetSettings(uri[1].toInt());
			Serial.println(uri[1].toInt());
			request->send(200);
		}
		else {
			LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: RuleSet GET"), F("List"), "");

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

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: GET"), F("Set"), String(uri[1]));
	}
	else {
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
		request->send(404);
	}
}

void Webhandler::sensorGet(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();
	Sort sort = RAW;

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(settings.http_user.c_str(), settings.http_pw.c_str()))
		return request->requestAuthentication();;
	
	if (uri[3] != "") {
		if (uri[3] == "desc") sort = DESC;
		else if (uri[3] == "asc") sort = ASC;
		else sort = RAW;
	}
	else sort = RAW;

	LOGDEBUG(F("[WebServer]"), F("sensorGet()"), F("Sort"), sort, uri[3], "");

	if (uri[1] == "") {
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), F("List"), "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");

		JsonObject& root = response->getRoot();
		root["obj"] = "SENSOR";

		JsonArray&	list = root.createNestedArray("list");
		for (uint8_t i = 0; i < SENS_NUM; i++) {
			JsonObject& element = list.createNestedObject();
			sensors[i]->serializeJSON(element, LIST, sort);
		}
		response->setLength();
		request->send(response);
	}
	else if (uri[1] != "" && uri[1].toInt() < SENS_NUM) {
		//Decide what kind of sensor data to send
		if (uri[2] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();

			sensors[uri[1].toInt()]->serializeJSON(root, HEADER, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: Header"));
		}
		else if (uri[2] == "details") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DETAILS, sort);
			response->setLength();
			request->send(response);
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: Details"));
		}
		else if (uri[2] == "avg") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, AVG, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: AVG"));
		}
		else if (uri[2] == "minute") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_MIN, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: MIN"));
		}
		else if (uri[2] == "hour") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_HOUR, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: HOUR"));
		}
		else if (uri[2] == "day") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_DAY, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: DAY"));
		}
		else if (uri[2] == "month") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_MONTH, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: MONTH"));
		}
		else if (uri[2] == "all") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			sensors[uri[1].toInt()]->serializeJSON(root, DATE_ALL, sort);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action GET"), String(uri[1]), F("Scope: ALL"));
		}
		//Commands
		else if (uri[2] == "lower") {
			sensors[uri[1].toInt()]->setLowerThreshold();
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor set Lower Raw Value"), String(uri[1]), "");
			request->send(200);
		}
		else if (uri[2] == "upper") {
			sensors[uri[1].toInt()]->setUpperThreshold();
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor set upper Raw Value"), String(uri[1]), "");
			request->send(200);
		}
		else if (uri[2] == "reset") {
			sensors[uri[1].toInt()]->reset();
			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor reset"), String(uri[1]), "");
			request->send(200);
		}
		else {
			LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
			request->send(404);
		}
	}
	else {
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings GET"), F("Current Settings"), "");
	}
	else if (uri[1] == "default") {
		settings.loadDefaultConfig();
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action LOAD"), F("Default Config"), "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings GET"), F("Default Config"), "");
	}
	else if (uri[1] == "active") {
		settings.loadActiveConfig();
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action LOAD"), F("Active Config"), "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings GET"), F("Active Config"), "");
	}
	else if (uri[1] == "reset") {
		settings.reset();
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings RESET all"), "", "");

		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		settings.serializeJSON(root);
		response->setLength();
		request->send(response);

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings GET"), F("Active Config"), "");
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
				else if (trigger[i][0]->type == 1) {
					item["tit"] = "Comparator";
				}
				else if (trigger[i][0]->type == 2) {
					item["tit"] = "Counter";
				}
				else if (trigger[i][0]->type == 3) {
					item["tit"] = "Switch";
				}
				item["src"] = trigger[i][0]->getSource();
			}
		}
		response->setLength();
		request->send(response);

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Categories GET"), F("List"), "");
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

		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Categories & Trigger GET"), F("List"), "");
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

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger for Category GET"), F("List"), String(uri[1]));
		}
		else if (uri[2] != "" && uri[2].toInt() < TRIGGER_SETS) {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();

			trigger[uri[1].toInt()][uri[2].toInt()]->serializeJSON(root, DETAILS);
			response->setLength();
			request->send(response);

			LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger GET"), String(uri[1]), String(uri[2]));
		}
		else {
			request->send(404);
			LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
		}
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
	}
}

void Webhandler::restart(AsyncWebServerRequest * request)
{
	settings.saveActiveConfig();
	LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action SAVE"), F("Active Config"), "");
	request->send(200);
	LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Restart Machine"), F("Delay"), F("Delay"));
	delay(5000);
	ESP.restart();
}

void Webhandler::unknownGet(AsyncWebServerRequest * request)
{
	LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actionchain Action SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action SET"), String(uri[1]), "");

	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Sensor Action SET"), String(uri[1]), "");
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Actio: SET"), "", "");
	}
	else if (uri[1] == "default") {
		settings.deserializeJSON(json);
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action SET"), "", "");
		settings.saveDefaultConfig();
		request->send(200);
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action SAVE"), F("Default Config"), "");
	}
	else if (uri[1] == "active") {
		settings.deserializeJSON(json);
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action SET"), "", "");
		settings.saveActiveConfig();
		request->send(200);
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action SAVE"), F("Active Config"), "");
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
		LOGMSG2(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action SET"), String(uri[1]), String(uri[2]));
	}
	else {
		request->send(404);
		LOGMSG2(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI UNKOWN"), "", "");
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
