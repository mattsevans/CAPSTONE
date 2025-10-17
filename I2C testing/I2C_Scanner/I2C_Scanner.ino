#include <TinyWireM.h>  // For ATtiny44A master mode

void setup() {
  TinyWireM.begin();      // Initialize USI as I2C master
  pinMode(0, OUTPUT);     // Optional: for LED indicator
}

void loop() {
  TinyWireM.beginTransmission(0x12);
  byte error = TinyWireM.endTransmission();

}
