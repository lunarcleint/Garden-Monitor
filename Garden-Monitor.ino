
// Idk if we gonna keep this
// #include "MoistureSensor.hpp"

// Animation on LED Matrix
#include "Arduino_LED_Matrix.h"
#include "DisplayLogos.h"

// Wifi Conectivity 
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <WiFiUdp.h>
#include "RTC.h"
#include <NTPClient.h>

// Sending to spreadsheet
#include <ArduinoJson.h>
#include "WIFICredentials.h"

void setup()
{
  Serial.begin(9600);
  while (!Serial) {;} // Wait for Serial for logging

  displayLogo();

  connectToWifi();
  setupRTC();
}

// Display temp logo to show program loaded
ArduinoLEDMatrix matrix;

void displayLogo() {
  matrix.begin();
  matrix.loadFrame(TestAnimation); 
}

const unsigned long PING_DISPLAY_TIME_MS = 1000;

void displayPing() {
  matrix.loadFrame(PingedAnimation);
  delay(PING_DISPLAY_TIME_MS);
  matrix.loadFrame(TestAnimation);
}

// Connect to Wifi
const unsigned long WIFI_CONNECT_RETRY_MS = 500;
bool connectingToWifi = false;

void connectToWifi() {
  if (connectingToWifi) return;

  Serial.print("Attempting to connect WiFi");
  connectingToWifi = true;

  WiFi.begin(SECRET_WIFI_NAME, SECRET_WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_CONNECT_RETRY_MS);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  connectingToWifi = false;
}

// Ping google.com
bool pingGoogle() {
  return WiFi.ping("google.com") >= 0;
}


// Time stuff to get Unix time correctly
// https://docs.arduino.cc/tutorials/uno-r4-wifi/rtc/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, 0, 60000);

const unsigned long RTC_CONNECT_RETRY_MS = 1000;

void setupRTC() {
  RTC.begin();

  timeClient.begin();

  while (!timeClient.forceUpdate()) {
    delay(RTC_CONNECT_RETRY_MS);
    Serial.println("NTP time sync failed!");
  }

  unsigned long unixTime = timeClient.getEpochTime();

  RTCTime timeToSet(unixTime);
  RTC.setTime(timeToSet);

  Serial.print("RTC synchronized. Unix time: ");
  Serial.println(unixTime);
}

unsigned long getUnixTimestamp() {
  RTCTime currentTime;
  RTC.getTime(currentTime);
  return currentTime.getUnixTime();
}

// Generate Json for Http send
String generateJson() {
  JsonDocument document;
  document["data"][0]["Unix Time Stamp"] = getUnixTimestamp();
  document["data"][0]["Moisture Reading"] = 1.0 + sin(millis()); // Random

  String jsonOutput;
  serializeJson(document, jsonOutput);

  getUnixTimestamp();

  return jsonOutput;
}

// Send over HTTPs
WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, SHEET_DB_ENDPOINT, 443); // Port 443 for https

const String contentType = "application/json";

bool postSheetDB() {
  String json = generateJson();

  Serial.println("Making POST request to SheetDB:");
  Serial.println(json + "\n");

  client.post(SHEET_DB_ROUTE, contentType, json);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.println("Status code: " + statusCode);
  Serial.println("Response: " + response);

  return statusCode == 201;
}

// Runs every 5 seconds
void timerTick() {
  if (WiFi.status() != WL_CONNECTED) 
    connectToWifi();
  else {
    if (postSheetDB()) {
      Serial.println("Uploaded to spreadsheet!");
      displayPing();
    } else {
      Serial.println("Failed ping!");
    }
  }
}

// Code that runs the tick
unsigned long prevTimerMS = 0;
const unsigned long TEST_TIMER_MS = 5000;

void loop() {
  unsigned long currentMS = millis();

  if (currentMS - prevTimerMS >= TEST_TIMER_MS) {
    prevTimerMS = currentMS;
    timerTick();
  }
}