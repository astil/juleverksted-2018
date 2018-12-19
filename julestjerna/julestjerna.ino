/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "nettverk";
const char* password = "passord";

const char* host = "hooks.slack.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";

String Data = "";

boolean harStjernaFaattVann = false;
boolean erStjernaToerr = false;
boolean harSendtMelding = false;
int meldingCounter = 0;

void post(String data) {
// Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "Legg inn webhooks addresse her";
  Serial.print("requesting URL: ");
  Serial.println(url);

  String PostData = "payload={\"text\":\"" + data + "\"}";

  client.println("POST " + url + " HTTP/1.1");
  client.println("Host:  hooks.slack.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded;");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  String data = "Julestjerna er på nett!";
  post(data);
}

void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.println(sensorValue);

  if(sensorValue > 500 && harSendtMelding == false){
    String data = "Jeg trenger vann! :hibiscus:";
    erStjernaToerr = true;
    harStjernaFaattVann = false;
    post(data);
    harSendtMelding = true;
  } 

  if(erStjernaToerr == true && harStjernaFaattVann == false && sensorValue < 500) {
    String data = "Det var godt! :hibiscus:";
    erStjernaToerr = false;
    harStjernaFaattVann = true;
    post(data);
  }

  meldingCounter += 1;

  if(meldingCounter > 7) {
    harSendtMelding = false;
    meldingCounter = 0;
  }
  Serial.println(meldingCounter);
  
  delay(10000);   
 
}
