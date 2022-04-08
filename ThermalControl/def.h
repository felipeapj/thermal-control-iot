#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "webPage.h";
#include <DallasTemperature.h>
#include <OneWire.h>

int TEMP_THRESHOLD = 2;
int STATUS_CHECK_INTERVAL = 500;

uint8_t LED_POWER = D0;
uint8_t LED_COOL = D1;
uint8_t SENSOR = D2;
uint8_t BTN_PLUS = D3;
uint8_t BTN_MINUS = D4;
uint8_t COOLER = D5;
uint8_t LED_POWER_LEVEL = LOW;

OneWire oneWire(SENSOR);
DallasTemperature sensors(&oneWire);
