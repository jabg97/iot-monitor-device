#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

// Device config
const String DEVICE_ID = "4e0ab034-253f-479d-910b-42ae3b8e1079";

// DTH22 sensor config
#define DHTTYPE DHT22
#define DHT_PIN 5
DHT dht(DHT_PIN, DHTTYPE);

// Soil moisture sensor config
const int SENSOR_PIN = A0;
const int AIR_HUMIDITY = 550;
const int WATER_HUMIDITY = 250;


// Wifi|Network config
#define SERVER_ENDPOINT "https://iot-monitor-functions.azurewebsites.net/devices/read/" + DEVICE_ID

#ifndef STASSID
#define STASSID ".Ángel.21."
#define STAPSK "luisa2011"
#endif
const String BLANK_STRING = "";

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

float readAirTemperature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature();
  return t;
}

float readAirHumidity() {
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  return h;
}


long readSoilMoisture() {
  int soilMoisture = analogRead(SENSOR_PIN);
  long percentage = map(soilMoisture, AIR_HUMIDITY, WATER_HUMIDITY, 0, 100);
  if (percentage > 100) { percentage = 100; }
  return percentage;
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure server and endpoint url
    http.begin(client, SERVER_ENDPOINT);  // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST(BLANK_STRING + "{\"currentAirTemperature\":\"" + readAirTemperature() + "\", \"currentAirHumidity\":\"" + readAirHumidity() + "\", \"currentSoilMoisture\":\"" + readSoilMoisture() + "}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  //Repeat every 5 minutes
  delay(300000);
}
