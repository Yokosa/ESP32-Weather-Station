/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <GUVA-S12SD.h>

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

Adafruit_BMP280 bmp; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

Adafruit_AHTX0 aht;
GUVA_S12SD sensor(A0);

// Replace with your network credentials
const char* ssid = "Winterfell";
const char* password = "Tosia18052019";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readBMP280Temperature() {
  // Read temperature as Celsius (the default)
  float t = bmp.readTemperature();
  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  if (isnan(t)) {    
    Serial.println("Failed to read from BMP280 sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readBMP280Pressure() {
  float p = bmp.readPressure() / 100.0F;
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}

String readAHT20Temperature() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  float e = (temp.temperature);
  if (isnan(e)) {
    Serial.println("Failed to read from AHT20 sensor!");
    return "";
  }
  else {
    Serial.println(e);
    return String(e);
  }
}

String readAHT20Humidity() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  float h = (humidity.relative_humidity);
  if (isnan(h)) {
    Serial.println("Failed to read from AHT20 sensor!");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
  
}

String readGUVAUV() {
  float u = sensor.getUVIntensity();
  if (isnan(u)) {
    Serial.println("Failed to read from GUVA_S12SD sensor!");
    return "";
  }
  else {
    Serial.println(u);
    return String(u);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  bool status; 
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bmp.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAHT20Temperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAHT20Humidity().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Pressure().c_str());
  });
    server.on("/uv", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readGUVAUV().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
