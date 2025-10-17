#include <Wire.h>

#define MCP3221_ADDR  0x4D   // Change if A0 pin is wired differently
#define MCP4725_ADDR  0x60   // Default for most modules

void setup() {
  Wire.begin();       // I2C master
  Serial.begin(9600);
}

void loop() {
  // ----- Read from MCP3221 -----
  Wire.requestFrom(MCP3221_ADDR, 2);
  if (Wire.available() == 2) {
    uint16_t highByte = Wire.read();
    uint16_t lowByte  = Wire.read();

    // Combine bytes (12-bit value is in upper bits)
    uint16_t adcValue = ((highByte << 8) | lowByte) & 0x0FFF;

    // Print to monitor
    Serial.print("ADC: ");
    Serial.println(adcValue);

    // ----- Write to MCP4725 -----
    Wire.beginTransmission(MCP4725_ADDR);
    Wire.write(0x40);                 // Command to update DAC register
    Wire.write(adcValue >> 4);        // Upper 8 bits
    Wire.write((adcValue & 0x0F) << 4); // Lower 4 bits
    Wire.endTransmission();
  }

  delay(10); // small delay to stabilize bus
}
