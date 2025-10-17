#include <TinyWireM.h>

void setup() {
  TinyWireM.begin();  // Initialize as master
}

void loop() {
  TinyWireM.beginTransmission(0x12); // any slave address
  TinyWireM.endTransmission();
  delay(1000);
}

