#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// SSID and Password to your ESP Access Point
const char * ssid = "TagoIO Device Wifi Setup";
const char * password = "";

// Server on port 80
ESP8266WebServer server(80);

//==============================================================
//     HTTP Routes
//==============================================================
void handleRoot() {
  Serial.println("HTTP GET request on /");
  server.send(200, "application/json", "{ \"status\": true, \"message\": \"TagoIO Device Wifi Setup - Server Running\" }");
}

void handlePing() {
  Serial.println("HTTP GET request on /ping");
  server.send(200, "application/json", "{ \"pong\": true }");
}

void handleParams() {
  Serial.println("HTTP GET request on /setup/params");

  int ssidIndexes = WiFi.scanNetworks();

  // Formating SSID list as JSON Array of string
  // e.g ["My Wifi", "Neighbor's Wifi", "Other Wifi"]
  String ssidList = "";
  for (int i = 0; i < ssidIndexes; i++) {
    if (i == 0) {
      ssidList = "\"" + WiFi.SSID(i) + "\"";
    } else {
      ssidList += ",\"" + WiFi.SSID(i) + "\"";
    }
  }

  String jsonParams = "[{\"name\": \"name\",\"required\": true,\"type\": \"text\",\"placeholder\": \"Device Name\"},{\"name\": \"ssid\",\"required\": true,\"type\": \"autocomplete\",\"placeholder\": \"SSID\",\"options\": [" + ssidList + "]},{\"name\": \"password\",\"type\": \"password\",\"placeholder\": \"SSID Password\"}]";
  server.send(200, "application/json", jsonParams);
}

void handleSetup() {
  Serial.println("HTTP POST request on /setup");

  // Check if body received
  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{ \"status\": false, \"message\": \"body not sent\" }");
    return;
  }

  String message = "Body received:\n";
  message += server.arg("plain");
  message += "\n";

  Serial.println(message);

  // TODO: here you save the data of the wifi settings the user typed

  server.send(200, "application/json", "{ \"status\": true }");
}

//===============================================================
//    SETUP
//===============================================================
void setup(void) {
  Serial.begin(9600);
  Serial.println("");

  // Only Access point
  WiFi.mode(WIFI_AP);
  // Start Hotspot removing password will disable security
  WiFi.softAP(ssid, password);

  // Get IP address
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  // Bind routes with functions
  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/setup/params", handleParams);
  server.on("/setup", handleSetup);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

//===============================================================
//    LOOP
//===============================================================
void loop(void) {
  // Handle client requests
  server.handleClient();
}
