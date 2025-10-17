#include <TinyWireS.h>

#define I2C_SLAVE_ADDR 0x12  // 7-bit I2C address

volatile uint8_t receivedByte = 0;

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDR);  // Start I2C slave at address 0x12
}

void loop() {
  // If the master sent data, read it
  if (TinyWireS.available()) {
    receivedByte = TinyWireS.receive();  // Store the byte
  }

  // Must call this frequently to handle I2C events
  TinyWireS_stop_check();
}

// This function is called when the master requests data (I2C read)
void requestEvent() {
  TinyWireS.send(receivedByte);  // Send back the last received byte
}
