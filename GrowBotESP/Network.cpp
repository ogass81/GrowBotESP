// 
// 
// 

#include "Network.h"

String WebServer::createPostRequest(char *json)
{
	String load, request;

	load = String(json) + "\r\n";
	//request = "POST / HTTP/1.1\r\n";
	request = "HTTP/1.1 200 OK\r\n";
	//request += "Host: " + ip.printTo;
	//request += "\r\n";
	request += "Accept: */*\r\n";
	request += "Content-Length: ";
	request += load.length();
	request += "\r\n";
	request += "Content-Type: application/json\r\n";
	request += "\r\n";
	request += load;
	//request += "\r\n";

	return String(request);
}

String WebServer::createHtmlResponse(String code, String text)
{
	String request;

	request = "HTTP/1.1 " + (String)code;
	request += "\r\n";
	request += "Content-Type: text/html\r\n";
	request += "\r\n";
	request += "<html><title>GrowBot V1.0<\\title><body>";
	request += (String)text;
	request += "<\\body><\\html>";
	//request += "\r\n";

	return String(request);
}

void WebServer::sendPayload(WiFiClient client, String payload)
{
	int size = payload.length();

	if (size < PACKAGE_SIZE) {
		client.print(payload);
	}
	else {
		int package_count = size / PACKAGE_SIZE;
		for (int i = 0; i <= package_count; i++) {
			int lower_boundery = i * PACKAGE_SIZE;
			int upper_boundery = (i + 1)*PACKAGE_SIZE;
			if (upper_boundery > size) upper_boundery = size;

			String package = payload.substring(lower_boundery, upper_boundery);
			client.print(package);
			delay(25);
		}
	}
}

WebServer::WebServer() : WiFiServer(80)
{
	// print your WiFi shield's IP address
	IPAddress ip = WiFi.localIP();
	LOGMSG(F("[WebServer]"), F("OK: Webserver started"), F("IPV4"), ip, "");
}

void WebServer::checkConnection()
{
	WiFiClient client = this->available();

	String line = "";
	int8_t line_count = 0;
	bool header = true;

	String http_method = "";
	String uri[REST_URI_DEPTH];
	bool content_type = false;
	int content_length = 0;
	String password = "";
	String payload = "";
	//Debug
	String http_request;

	bool success;


	if (client) {
		LOGMSG(F("[WebServer]"), F("OK: New Client connected"), "@" + RealTimeClock::printTime(SENS_FRQ_SEC*sensor_cycles), F("IPV4"), client.remoteIP());

		while (client.connected()) {
			if (client.available() > 0) {
				char c = client.read();
				line += c;
				http_request += c;

				if (line.endsWith("\n") && line != "\r\n") {
					if (line_count == 0) {

						String parts[3];
						int8_t i = 0;

						//Break up line in Command and Resource
						char temp[line.length() + 1];
						line.toCharArray(temp, line.length() + 1);
						char *token = strtok(temp, " ");

						while (token != NULL && i < 3) {
							parts[i] = token;
							token = strtok(NULL, " ");
							i++;
						}

						//Parse command
						if (parts[0] != NULL) {
							if ((parts[0].indexOf("GET")) > -1) {
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("HTTP Method detected"), F("GET"), "", "");
								http_method = "GET";
							}
							else if ((parts[0].indexOf("POST")) > -1) {
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("HTTP Method detected"), F("POST"), "", "");
								http_method = "POST";
							}
							else if ((parts[0].indexOf("PATCH")) > -1) {
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("HTTP Method detected"), F("PATCH"), "", "");
								http_method = "PATCH";
							}
						}

						//Parse URI
						if (parts[1] != NULL) {
							int8_t i = 0;

							char temp[parts[1].length() + 1];
							parts[1].toCharArray(temp, parts[1].length() + 1);
							char *token = strtok(temp, "/");

							while (token != NULL && i < REST_URI_DEPTH) {
								uri[i] = token;
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("URI Collection / Element found"), i, uri[i], "");
								token = strtok(NULL, "/");
								i++;
							}
						}
					}
					//Check for Authorization or other tags
					else {

						//Check for Authorization Token		
						String searchbase = line;
						searchbase.toLowerCase();

						if (searchbase.startsWith("authorization:")) {

							String parts[3];
							int8_t i = 0;

							//Break up line in Command and Resource
							char temp[line.length() + 1];
							line.toCharArray(temp, line.length() + 1);
							char *token = strtok(temp, " ");

							while (token != NULL && i < 3) {
								parts[i] = token;
								token = strtok(NULL, " ");
								i++;
							}

							if (parts[1] == "Basic" && parts[2] != NULL) {
								password = parts[2];
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("Authorization Token detected"), password, "", "");
							}
						}

						//Check for Content Type
						if (searchbase.startsWith("content-type:") == true && searchbase.indexOf("json") > -1) {

							content_type = true;
							LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("Content Type"), F("JSON"), "", "");
						}


						//Check for Content Length Token
						if (searchbase.startsWith("content-length:")) {

							String parts[3];
							int8_t i = 0;

							//Break up line in Command and Resource
							char temp[line.length() + 1];
							line.toCharArray(temp, line.length() + 1);
							char *token = strtok(temp, " ");

							while (token != NULL && i < 3) {
								parts[i] = token;
								token = strtok(NULL, " ");
								i++;
							}

							if (parts[1] != NULL) {
								content_length = parts[1].toInt();
								LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("Content Lenght"), content_length, "", "");
							}
						}

					}
					line_count++;
					line = "";
				}
				//Detect Empty Line at end of header
				else if (line == "\r\n") {
					header = false;
					line = "";
					LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("End of HTTP Header detected"), "", "", "");
				}

				//Read Http Body
				if (header == false) {
					if (content_length > 0) {
						if (line.length() == content_length) {
							payload = line;
							LOGDEBUG(F("[WebServer]"), F("checkConnection()"), F("Payload detected"), payload, "", "");
							break;
						}
					}
					else break;
				}
			}
		}

		if (http_method == "GET") {
			//Variable for Outgoing Data
			char json[JSONCHAR_SIZE];

			//Send information about Growbot and statics
			if (uri[0] == "") {
				Setting::serializeJSON(json, JSONCHAR_SIZE);
				LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Constants Action: GET"), "", "");
				client.print(createPostRequest(json));
			}
			else if (uri[0] == "setting") {
				if (uri[1] == "") {
					Setting::serializeJSON(json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), "", "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] == "default") {
					Setting::loadSettings("DEFAULTCONFIG.JSON");
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: LOAD"), "Default Config", "");
					Setting::serializeJSON(json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), "", "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] == "active") {
					Setting::loadSettings("_CURRENTCONFIG.JSON");
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: LOAD"), "Active Config", "");
					Setting::serializeJSON(json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), "", "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] == "reset") {
					Setting::reset();
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: RESET"), "", "");
					Setting::serializeJSON(json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), "", "");
					client.print(createPostRequest(json));
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "action") {
				if (uri[1] == "") {
					ListGenerator<Action> list(actions, ACTIONS_NUM);
					list.generateList(F("ACTION"), json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actions - Action: GET"), F("List View"), "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < ACTIONS_NUM) {
					if (uri[2] == "") {
						actions[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), String(uri[1]), "");
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "execute") {
						actions[uri[1].toInt()]->execute();
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: EXECUTE"), String(uri[1]), "");
						//actions[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Action Object Action: GET"), String(uri[1]), "");
						client.print(createHtmlResponse("200 OK", "Action executed"));
					}
					else {
						LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
					}
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "actionchain") {
				if (uri[1] == "") {
					ListGenerator<ActionChain> list(actionchains, ACTIONCHAINS_NUM);
					list.generateList(F("ACTIONCHAIN"), json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actionchain Action: GET"), F("List View"), "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < ACTIONCHAINS_NUM) {
					actionchains[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actionchain Action: GET"), String(uri[1]), "");
					client.print(createPostRequest(json));
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "rcsocket") {
				if (uri[1] == "") {
					rcsocketcontroller->serializeJSON(json, JSONCHAR_SIZE, LIST);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: ### Action: GET"), F("List View"), "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < RC_SOCKETS) {
					if (uri[2] == "") {
						rcsocketcontroller->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: GET"), String(uri[1]), "");
						client.print(createPostRequest(json));
					}
					else if (uri[2] != "" && uri[2] == "learn_on") {
						rcsocketcontroller->learningmode_on(uri[1].toInt());
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: Learning mode activated"), String(uri[1]), "");
						client.print(createHtmlResponse("200 OK", "Learning mode on"));
					}
					else if (uri[2] != "" && uri[2] == "learn_off") {
						rcsocketcontroller->learningmode_off();
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: Learning mode deactivated"), String(uri[1]), "");
						client.print(createHtmlResponse("200 OK", "Learning mode off"));
					}
					else if (uri[2] != "" && uri[2] == "reset") {
						rcsocketcontroller->resetSettings(uri[1].toInt());
						Serial.println(uri[1].toInt());
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: RESET"), String(uri[1]), "");
						client.print(createHtmlResponse("200 OK", "Reset signals"));
					}
					else {
						LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
					}
				}
			}
			else if (uri[0] == "ruleset") {
				if (uri[1] == "") {
					ListGenerator<RuleSet> list(rulesets, RULESETS_NUM);
					list.generateList(F("RULE"), json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: GET"), F("List View"), "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < RULESETS_NUM) {
					rulesets[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: GET"), String(uri[1]), "");
					client.print(createPostRequest(json));
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "sensor") {
				if (uri[1] == "") {
					ListGenerator<Sensor> list(sensors, SENS_NUM);
					list.generateList(F("SENSOR"), json, JSONCHAR_SIZE);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), F("List View"), "");
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < SENS_NUM) {
					//Decide what kind of sensor data to send
					if (uri[2] == "") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, HEADER);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: DETAILS"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "details") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DETAILS);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: AVG"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "avg") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, AVG);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: AVG"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_minute") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_MINUTE);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: MINUTE"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_hour") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_HOUR);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: HOUR"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_day") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_DAY);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: DAY"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_month") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_MONTH);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: MONTH"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_year") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_YEAR);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: YEAR"));
						client.print(createPostRequest(json));
					}
					else if (uri[2] == "date_all") {
						sensors[uri[1].toInt()]->serializeJSON(uri[1].toInt(), json, JSONCHAR_SIZE, DATE_ALL);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: ALL"));
						client.print(createPostRequest(json));
					}
					//Commands
					else if (uri[2] == "lower") {
						sensors[uri[1].toInt()]->setLowerThreshold();
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Set Lower Threshold"));
						client.print(createHtmlResponse("200 OK", "Set Lower Threshold"));
					}
					else if (uri[2] == "upper") {
						sensors[uri[1].toInt()]->setUpperThreshold();
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Set Upper Threshold"));
						client.print(createHtmlResponse("200 OK", "Set Upper Threshold"));
					}
					else if (uri[2] == "reset") {
						sensors[uri[1].toInt()]->reset();
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Sensor Action: GET"), String(uri[1]), F("Mode: Reset"));
						client.print(createHtmlResponse("200 OK", "Reset"));
					}
					else {
						LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
					}
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "trigger") {
				if (uri[1] == "") {
					TriggerCategory::serializeJSON(trigger, json, JSONCHAR_SIZE, LIST);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET Categories"), String(uri[1]), String(uri[2]));
					client.print(createPostRequest(json));
				}
				else if (uri[1] == "all") {
					TriggerCategory::serializeJSON(trigger, json, JSONCHAR_SIZE, DETAILS);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET Flat List"), String(uri[1]), String(uri[2]));
					client.print(createPostRequest(json));
				}
				else if (uri[1] != "" && uri[1].toInt() < TRIGGER_TYPES) {
					if (uri[2] == "") {
						ListGenerator<Trigger> list(trigger[uri[1].toInt()], TRIGGER_SETS);
						list.generateList(F("TRIGGER"), uri[1].toInt(), json, JSONCHAR_SIZE);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET"), F("List View for category"), String(uri[1]));
						client.print(createPostRequest(json));
					}
					else if (uri[2] != "" && uri[2].toInt() < TRIGGER_SETS) {
						trigger[uri[1].toInt()][uri[2].toInt()]->serializeJSON(uri[1].toInt(), uri[2].toInt(), json, JSONCHAR_SIZE, DETAILS);
						client.print(createPostRequest(json));
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: GET"), String(uri[1]), String(uri[2]));
					}
					else {
						LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
					}
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
				}
			}
			else if (uri[0] == "log") {
				if (uri[1] == "") {
					logengine.serializeJSON(json, JSONCHAR_SIZE, 0, 0);
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(0), String(0));
					client.print(createPostRequest(json));
				}
				else if (uri[1] == "reset") {
					logengine.reset();
					client.print(createHtmlResponse("200 OK", "Reset Log file"));
					LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Reset Log file"), "", "");
				}
				else {
					if (uri[2] == "") {
						logengine.serializeJSON(json, JSONCHAR_SIZE, uri[1].toInt(), 0);
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(uri[1].toInt()), String(0));
						client.print(createPostRequest(json));
					}
					else {
						logengine.serializeJSON(json, JSONCHAR_SIZE, uri[1].toInt(), uri[2].toInt());
						LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Read Log file"), String(uri[1].toInt()), String(uri[2].toInt()));
						client.print(createPostRequest(json));
					}
				}
			}
			else {
				LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
				client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
			}
		}
		else if (http_method == "PATCH") {
			LOGMSG(F("[WebServer]"), F("INFO: Payload"), payload, String(content_type), String(payload.length()));

			if (payload.length() > 0 && content_type == true) {
				//Incoming Data
				DynamicJsonBuffer jsonBuffer;
				JsonObject& node = jsonBuffer.parseObject(payload);
				if (node.success() == true) {

					if (uri[0] == "setting") {
						if (uri[1] == "") {
							success = Setting::deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
						}
						else if (uri[1] == "default") {
							success = Setting::deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SAVE to Default"), "", "");
							Setting::saveSettings("DEFAULTCONFIG.JSON");
						}
						else if (uri[1] == "active") {
							success = Setting::deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SET"), "", "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Settings Action: SAVE to Active"), "", "");
							Setting::saveSettings("_CURRENTCONFIG.JSON");
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else if (uri[0] == "action") {
						LOGMSG(F("[WebServer]"), F("OK: Invalid HTTP Request"), F("Type: Action Object Action: SET "), "Not Supported", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Not supported"));
					}
					else if (uri[0] == "actionchain") {
						if (uri[1] != "" && uri[1].toInt() < ACTIONS_NUM) {
							success = actionchains[uri[1].toInt()]->deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Actionchain Action: SET"), String(uri[1]), "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else if (uri[0] == "rcsocket") {
						if (uri[1] != "" && uri[1].toInt() < RC_SOCKETS) {
							success = rcsocketcontroller->deserializeJSON(uri[1].toInt(), node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Socket Action: SET"), String(uri[1]), "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else if (uri[0] == "ruleset") {
						if (uri[1] != "" && uri[1].toInt() < RULESETS_NUM) {
							success = rulesets[uri[1].toInt()]->deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Ruleset Action: SET"), String(uri[1]), "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else if (uri[0] == "sensor") {
						if (uri[1] != "" && uri[1].toInt() < SENS_NUM) {
							success = sensors[uri[1].toInt()]->deserializeJSON(node);
							LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Remote Sensor Action: SET"), String(uri[1]), "");
							client.print(createHtmlResponse("200 OK", "JSON received"));
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else if (uri[0] == "trigger") {
						if (uri[1] != "" && uri[1].toInt() < TRIGGER_TYPES) {
							if (uri[2] != "" && uri[2].toInt() < TRIGGER_SETS) {
								success = trigger[uri[1].toInt()][uri[2].toInt()]->deserializeJSON(node);
								LOGMSG(F("[WebServer]"), F("OK: Valid HTTP Request"), F("Type: Trigger Action: SET"), String(uri[1]), String(uri[2]));
								client.print(createHtmlResponse("200 OK", "JSON received"));
							}
							else {
								LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
								client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
							}
						}
						else {
							LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
							client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
						}
					}
					else {
						LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: URI: UNKOWN"), "", "");
						client.print(createHtmlResponse("400 BAD REQUEST", "Unknown URI"));
					}
				}
				else {
					LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: Invalid Payload"), "", "");
					client.print(createHtmlResponse("400 BAD REQUEST", "Invalid Payload"));
				}
			}
			else {
				LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: Invalid Payload"), "", "");
				client.print(createHtmlResponse("400 BAD REQUEST", "Invalid Payload"));
			}
		}

		else {
			LOGMSG(F("[WebServer]"), F("ERROR: Invalid HTTP Request"), F("Type: Unsupported Method"), "", "");
			client.print(createHtmlResponse("400 BAD REQUEST", "Not supported Method"));
		}


		// give the web browser time to receive the data
		delay(150);
		// close the connection:
		client.stop();
		LOGMSG(F("[WebServer]"), F("OK: Client disconnected"), "@" + RealTimeClock::printTime(SENS_FRQ_SEC*sensor_cycles), F("IPV4"), client.remoteIP());
	}
}

template<class ObjectType>
ListGenerator<ObjectType>::ListGenerator(ObjectType **objects, int8_t num)
{
	this->objectarray = objects;
	this->object_count = num;
}


template<class ObjectType>
inline void ListGenerator<ObjectType>::generateList(String object_type, char * json, int size)
{
	String jsonarray = "{\"obj\": \"" + object_type;
	jsonarray += "\",\"list\":[";

	for (uint8_t i = 0; i < object_count; i++) {
		objectarray[i]->serializeJSON(i, json, size, LIST);
		jsonarray += json;
		if (object_count - i > 1) {
			jsonarray += ",";
		}
		else jsonarray += "]}";
	}
	if (jsonarray.length() > 0) {
		jsonarray.toCharArray(json, jsonarray.length() + 1);
	}
}

template<class ObjectType>
void ListGenerator<ObjectType>::generateList(String object_type, uint8_t cat, char * json, int size)
{
	String jsonarray = "{\"obj\": \"" + object_type;
	jsonarray += "\",\"list\":[";

	for (uint8_t i = 0; i < object_count; i++) {
		objectarray[i]->serializeJSON(cat, i, json, size, LIST);
		jsonarray += json;
		if (object_count - i > 1) {
			jsonarray += ",";
		}
		else jsonarray += "]}";
	}
	if (jsonarray.length() > 0) {
		jsonarray.toCharArray(json, jsonarray.length() + 1);
	}
}


unsigned long WebTimeClient::getWebTime()
{
	unsigned long time = 0;

	// Just choose any reasonably busy web server, the load is really low
	if (this->connect("google.com", 80))
	{
		// Make an HTTP 1.1 request which is missing a Host: header
		// compliant servers are required to answer with an error that includes
		// a Date: header.
		this->print(F("GET / HTTP/1.1 \r\n\r\n"));

		char buf[5];			// temporary buffer for characters
		this->setTimeout(5000);
		if (this->find((char *)"\r\nDate: ") // look for Date: header
			&& this->readBytes(buf, 5) == 5) // discard
		{
			unsigned day = this->parseInt();	   // day
			this->readBytes(buf, 1);	   // discard
			this->readBytes(buf, 3);	   // month
			int year = this->parseInt();	   // year
			byte hour = this->parseInt();   // hour
			byte minute = this->parseInt(); // minute
			byte second = this->parseInt(); // second

			int daysInPrevMonths;
			switch (buf[0])
			{
			case 'F': daysInPrevMonths = 31; break; // Feb
			case 'S': daysInPrevMonths = 243; break; // Sep
			case 'O': daysInPrevMonths = 273; break; // Oct
			case 'N': daysInPrevMonths = 304; break; // Nov
			case 'D': daysInPrevMonths = 334; break; // Dec
			default:
				if (buf[0] == 'J' && buf[1] == 'a')
					daysInPrevMonths = 0;		// Jan
				else if (buf[0] == 'A' && buf[1] == 'p')
					daysInPrevMonths = 90;		// Apr
				else switch (buf[2])
				{
				case 'r': daysInPrevMonths = 59; break; // Mar
				case 'y': daysInPrevMonths = 120; break; // May
				case 'n': daysInPrevMonths = 151; break; // Jun
				case 'l': daysInPrevMonths = 181; break; // Jul
				default: // add a default label here to avoid compiler warning
				case 'g': daysInPrevMonths = 212; break; // Aug
				}
			}

			// This code will not work after February 2100
			// because it does not account for 2100 not being a leap year and because
			// we use the day variable as accumulator, which would overflow in 2149
			day += (year - 1970) * 365;	// days from 1970 to the whole past year
			day += (year - 1969) >> 2;	// plus one day per leap year 
			day += daysInPrevMonths;	// plus days for previous months this year
			if (daysInPrevMonths >= 59	// if we are past February
				&& ((year & 3) == 0))	// and this is a leap year
				day += 1;			// add one day
									// Remove today, add hours, minutes and seconds this month
			time = (((day - 1ul) * 24 + hour) * 60 + minute) * 60 + second;
		}
	}
	delay(10);
	this->flush();
	this->stop();

	return time;
}
