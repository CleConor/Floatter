#include "floatter_lib.h"

uint32_t startMillis;
uint32_t currentMillis;
const uint32_t period = SENDING_TIME;

void setup() {
  Serial.begin(115200);
  for(auto timeOut = millis() + 5000; millis() < timeOut && !Serial; delay(100));
  wifiSetup();
  sensorSetup();
}

void loop() {
  client.loop();
  sendWaveStat();

  currentMillis = millis();
  if (currentMillis - startMillis >= period){
    sendLoop();
    startMillis = currentMillis;
  }
}
