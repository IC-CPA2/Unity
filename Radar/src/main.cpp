#include <Arduino.h>
#include <Radar.h>

Radar radar;

void setup() {
  radar.setup();
}

void loop() {
  radar.fan_detect();
}