#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__)

// Pin definitions for switches
#define PIN_SWITCH 5  // D1
#define PIN_SWITCH2 4 // D2
#define PIN_SWITCH3 0 // D3
#define PIN_SWITCH4 2 // D4

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
  
  // Uncomment these lines to reset HomeKit pairing
   //homekit_server_reset();
   //ESP.restart();
  
  wifi_connect(); // Defined in wifi_info.h
  
  my_homekit_setup();
}

void loop() {
  my_homekit_loop();
  delay(10);
}

// HomeKit setup
void my_homekit_setup() {
  pinMode(PIN_SWITCH, OUTPUT);
  digitalWrite(PIN_SWITCH, HIGH);
  pinMode(PIN_SWITCH2, OUTPUT);
  digitalWrite(PIN_SWITCH2, HIGH);
  pinMode(PIN_SWITCH3, OUTPUT);
  digitalWrite(PIN_SWITCH3, HIGH);
  pinMode(PIN_SWITCH4, OUTPUT);
  digitalWrite(PIN_SWITCH4, HIGH);

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
}

void cha_switch_on_setter2(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on2.value.bool_value = on; // Sync the value
  LOG_D("Switch 2: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH2, on ? LOW : HIGH);
}

void cha_switch_on_setter3(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on3.value.bool_value = on; // Sync the value
  LOG_D("Switch 3: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH3, on ? LOW : HIGH);
}

void cha_switch_on_setter4(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on4.value.bool_value = on; // Sync the value
  LOG_D("Switch 4: %s", on ? "ON" : "OFF");
  digitalWrite(PIN_SWITCH4, on ? LOW : HIGH);
}
