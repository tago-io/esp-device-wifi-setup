#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


//SSID and Password to your ESP Access Point
const char * ssid = "TagoIO Device Wifi Setup";
const char * password = "";


ESP8266WebServer server(80); //Server on port 80

//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
  Serial.println("http request on /");
  server.send(200, "application/json", "{\"status\": true }");
}

void handlePing() {
  Serial.println("http request on /ping");
  server.send(200, "application/json", "{\"status\": true, \"type\": \"ping\" }");
}

void handleParams() {
  int n = WiFi.scanNetworks();
  String ssid_list = "";
  for (int i = 0; i < n; i++) {
    if (i == 0) {
      ssid_list = "\"" + WiFi.SSID(i) + "\"";
    } else {
      ssid_list += ",\"" + WiFi.SSID(i) + "\"";
    }
  }

  String result = "[{\"name\": \"name\",\"required\": true,\"type\": \"text\",\"placeholder\": \"Device Name\"},{\"name\": \"ssid\",\"required\": true,\"type\": \"autocomplete\",\"placeholder\": \"SSID\",\"options\": [" + ssid_list + "]},{\"name\": \"password\",\"type\": \"password\",\"placeholder\": \"SSID Password\"}]";
  server.send(200, "application/json", result);
}

void handleSetup() { //Handler for the body path
  // TODO here you save the data the wifi settings the user typed
  if (server.hasArg("plain") == false) { //Check if body received
    server.send(400, "application/json", "{\"status\": false, \"message\": \"body not sent\" }");
    return;
  }

  String message = "Body received:\n";
  message += server.arg("plain");
  message += "\n";

  Serial.println(message);
  server.send(200, "application/json", "{\"status\": true, \"type\": \"setup\" }");
}

//===============================================================
//                  SETUP
//===============================================================
void setup(void) {
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP); //Only Access point
  WiFi.softAP(ssid, password); //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  server.on("/", handleRoot); //Which routine to handle at root location
  server.on("/ping", handlePing); //Which routine to handle at root location
  server.on("/setup/params", handleParams); //Which routine to handle at root location
  server.on("/setup", handleSetup);

  server.begin(); //Start server
  Serial.println("HTTP server started");
}
//===============================================================
//                     LOOP
//===============================================================
void loop(void) {
  server.handleClient(); //Handle client requests
}