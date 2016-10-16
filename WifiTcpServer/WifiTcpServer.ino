
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Hardcode WiFi parameters as this isn't going to be moving around.
const char* ssid = "Livebox-BC43";
const char* password = "C03294468E618A316C6759A245";

// Start a TCP Server on port 5045
WiFiServer server(5045);
WiFiClient client;
void setup() {
  Serial.begin(74880);
  WiFi.begin(ssid,password);
  Serial.println("");
  //Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  
  // Start the TCP server
  server.begin();
}


void TCPServer () {
    client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println("Connected to client");    
    }
    if (client.available() > 0) {
      // Read incoming message
      char inChar = client.read();
      // Send back something to the clinet
      server.write(inChar);
      // Echo input on Serial monitor
      Serial.write(inChar);
    }
  }
}
   void loop() {
  TCPServer();
}

