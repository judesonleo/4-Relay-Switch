#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "wifi_info.h"
#include <EEPROM.h>


#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__)

// Define pin numbers for switches
#define PIN_SWITCH 5  // D1
#define PIN_SWITCH2 4 // D2
#define PIN_SWITCH3 0 // D3
#define PIN_SWITCH4 2 // D4

#define EEPROM_SIZE 512
#define SWITCH1_ADDR 510
#define SWITCH2_ADDR 511
#define SWITCH3_ADDR 512
#define SWITCH4_ADDR 509

ESP8266WebServer server(80);
int lastStatusCode = 200; // Global variable to hold the last status code

// Function prototypes
void setup();
void loop();
void my_homekit_setup();
void my_homekit_loop();
void cha_switch_on_setter(const homekit_value_t value);
void cha_switch_on_setter2(const homekit_value_t value);
void cha_switch_on_setter3(const homekit_value_t value);
void cha_switch_on_setter4(const homekit_value_t value);

// Access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on;
extern "C" homekit_characteristic_t cha_switch_on2;
extern "C" homekit_characteristic_t cha_switch_on3;
extern "C" homekit_characteristic_t cha_switch_on4;

void setup() {
  Serial.begin(115200);

  wifi_connect();
  //homekit_storage_reset();
  my_homekit_setup();

  server.on("/", HTTP_GET, []() {
    String html = "<html><body>";
    html += "<h1>Hello from your ESP8266!</h1>";
    html += "<p><a href=\"/switch1\">Toggle Switch 1</a></p>";
    html += "<p><a href=\"/switch2\">Toggle Switch 2</a></p>";
    html += "<p><a href=\"/switch3\">Toggle Switch 3</a></p>";
    html += "<p><a href=\"/switch4\">Toggle Switch 4</a></p>";
    html += "<p><a href=\"/status\">View Switch Status</a></p>";
    html += "</body></html>";

    server.send(200, "text/html", html);
    lastStatusCode = 200;
  });

  server.on("/switch1", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow requests from any origin
    cha_switch_on_setter(HOMEKIT_BOOL(!cha_switch_on.value.bool_value));
    server.send(200, "text/plain", cha_switch_on.value.bool_value ? "Switch 1 ON" : "Switch 1 OFF");
    lastStatusCode = 200;
  });

  server.on("/switch2", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow requests from any origin
    cha_switch_on_setter2(HOMEKIT_BOOL(!cha_switch_on2.value.bool_value));
    server.send(200, "text/plain", cha_switch_on2.value.bool_value ? "Switch 2 ON" : "Switch 2 OFF");
    lastStatusCode = 200;
  });

  server.on("/switch3", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow requests from any origin
    cha_switch_on_setter3(HOMEKIT_BOOL(!cha_switch_on3.value.bool_value));
    server.send(200, "text/plain", cha_switch_on3.value.bool_value ? "Switch 3 ON" : "Switch 3 OFF");
    lastStatusCode = 200;
  });

  server.on("/switch4", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow requests from any origin
    cha_switch_on_setter4(HOMEKIT_BOOL(!cha_switch_on4.value.bool_value));
    server.send(200, "text/plain", cha_switch_on4.value.bool_value ? "Switch 4 ON" : "Switch 4 OFF");
    lastStatusCode = 200;
  });

  server.on("/status", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow requests from any origin
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["switch1"] = cha_switch_on.value.bool_value ? "ON" : "OFF";
    jsonDoc["switch2"] = cha_switch_on2.value.bool_value ? "ON" : "OFF";
    jsonDoc["switch3"] = cha_switch_on3.value.bool_value ? "ON" : "OFF";
    jsonDoc["switch4"] = cha_switch_on4.value.bool_value ? "ON" : "OFF";
    
    String response;
    serializeJson(jsonDoc, response);
    server.send(200, "application/json", response);
  });

  server.begin();
}

void loop() {
  my_homekit_loop();
  server.handleClient();
  delay(10);
}

// HomeKit setup
void my_homekit_setup() {
  EEPROM.begin(EEPROM_SIZE);

  pinMode(PIN_SWITCH, OUTPUT);
  bool state1 = EEPROM.read(SWITCH1_ADDR);
  digitalWrite(PIN_SWITCH, state1 ? LOW : HIGH);

  pinMode(PIN_SWITCH2, OUTPUT);
  bool state2 = EEPROM.read(SWITCH2_ADDR);
  digitalWrite(PIN_SWITCH2, state2 ? LOW : HIGH);

  pinMode(PIN_SWITCH3, OUTPUT);
  bool state3 = EEPROM.read(SWITCH3_ADDR);
  digitalWrite(PIN_SWITCH3, state3 ? LOW : HIGH);

  pinMode(PIN_SWITCH4, OUTPUT);
  bool state4 = EEPROM.read(SWITCH4_ADDR);
  digitalWrite(PIN_SWITCH4, state4 ? LOW : HIGH);

  // Setters for each switch
  cha_switch_on.setter = cha_switch_on_setter;
  cha_switch_on2.setter = cha_switch_on_setter2;
  cha_switch_on3.setter = cha_switch_on_setter3;
  cha_switch_on4.setter = cha_switch_on_setter4;

  // Initialize HomeKit
  arduino_homekit_setup(&config);
}

// HomeKit loop
void my_homekit_loop() {
  arduino_homekit_loop();
  static uint32_t next_heap_millis = 0;
  const uint32_t t = millis();
  if (t > next_heap_millis) {
    // Show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
  }
}

// Setters for switch characteristics
void cha_switch_on_setter(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on.value.bool_value = on; // Sync the value
  LOG_D("Switch 1: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH, on ? LOW : HIGH);
  EEPROM.write(SWITCH1_ADDR, on);
  EEPROM.commit();  
}

void cha_switch_on_setter2(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on2.value.bool_value = on; // Sync the value
  LOG_D("Switch 2: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH2, on ? LOW : HIGH);
  EEPROM.write(SWITCH2_ADDR, on);
  EEPROM.commit(); 
}

void cha_switch_on_setter3(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on3.value.bool_value = on; // Sync the value
  LOG_D("Switch 3: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH3, on ? LOW : HIGH);
  EEPROM.write(SWITCH3_ADDR, on);
  EEPROM.commit(); 
}

void cha_switch_on_setter4(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on4.value.bool_value = on; // Sync the value
  LOG_D("Switch 4: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH4, on ? LOW : HIGH);
  EEPROM.write(SWITCH4_ADDR, on);
  EEPROM.commit(); 
}
