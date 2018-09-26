// 
// 
// 

#include "WebHandler.h"

Webhandler::Webhandler()
{
}

void Webhandler::begin()
{
	classWebServer.on("/", HTTP_ANY, std::bind(&Webhandler::loginRequest, this, std::placeholders::_1));
	classWebServer.on("/action", HTTP_ANY, std::bind(&Webhandler::actionRequest, this, std::placeholders::_1));
	classWebServer.on("/chain", HTTP_ANY, std::bind(&Webhandler::actionchainRequest, this, std::placeholders::_1));
	classWebServer.on("/log", HTTP_ANY, std::bind(&Webhandler::logRequest, this, std::placeholders::_1));
	classWebServer.on("/rcsocket", HTTP_ANY, std::bind(&Webhandler::rcsocketRequest, this, std::placeholders::_1));
	classWebServer.on("/ruleset", HTTP_ANY, std::bind(&Webhandler::rulesetRequest, this, std::placeholders::_1));
	classWebServer.on("/sensor", HTTP_ANY, std::bind(&Webhandler::sensorRequest, this, std::placeholders::_1));
	classWebServer.on("/setting", HTTP_ANY, std::bind(&Webhandler::settingRequest, this, std::placeholders::_1));
	classWebServer.on("/trigger", HTTP_ANY, std::bind(&Webhandler::triggerRequest, this, std::placeholders::_1));

	classWebServer.begin();
}

void Webhandler::loginRequest(AsyncWebServerRequest * request)
{
	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
		AsyncJsonResponse * response = new AsyncJsonResponse();
		response->addHeader("Server", "GrowAI");
		JsonObject& root = response->getRoot();
		Setting::serializeJSON(root);
		response->setLength();
		request->send(response);
	}
	else request->send(404);
}

void Webhandler::actionRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);
	
	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
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
	}
	else request->send(404);
}

void Webhandler::actionchainRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
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
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
}

void Webhandler::logRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
		if (uri[1] == "") {
			AsyncJsonResponse * response = new AsyncJsonResponse();
			response->addHeader("Server", "GrowAI");

			JsonObject& root = response->getRoot();
			DynamicJsonBuffer& buffer = response->getBuffer();

			logengine.serializeJSON(root, buffer, 0 ,0);

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
	else if (request->method() == HTTP_POST) {
		request->send(404);
	}
	else request->send(404);
}

void Webhandler::rcsocketRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
		if (uri[1] == "") {
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

				rcsocketcontroller->serializeJSON(root, DETAILS, uri[2].toInt());

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
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
}

void Webhandler::rulesetRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
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
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
}

void Webhandler::sensorRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
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
			else if (uri[2] == "date_minute") {
				AsyncJsonResponse * response = new AsyncJsonResponse();
				response->addHeader("Server", "GrowAI");

				JsonObject& root = response->getRoot();
				sensors[uri[1].toInt()]->serializeJSON(root, DATE_MINUTE);
				response->setLength();
				request->send(response);

				LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: MINUTE"));
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
			else if (uri[2] == "date_year") {
				AsyncJsonResponse * response = new AsyncJsonResponse();
				response->addHeader("Server", "GrowAI");

				JsonObject& root = response->getRoot();
				sensors[uri[1].toInt()]->serializeJSON(root, DATE_YEAR);
				response->setLength();
				request->send(response);

				LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: YEAR"));
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
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
}

void Webhandler::settingRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
		if (uri[1] == "") {

		}
		else {

		}
	}
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
}

void Webhandler::triggerRequest(AsyncWebServerRequest * request)
{
	String uri[REST_URI_DEPTH];
	String url = request->url();

	char temp[url.length() + 1];
	url.toCharArray(temp, url.length() + 1);
	breakupURL(uri, temp);

	if (!request->authenticate(http_user, http_password))
		return request->requestAuthentication();

	if (request->method() == HTTP_GET) {
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
	else if (request->method() == HTTP_POST) {

	}
	else request->send(404);
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
