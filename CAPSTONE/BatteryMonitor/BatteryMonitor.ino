#include <TinyWireM.h>


// ATMEL ATTINY84 / ARDUINO
//
//                           +-\/-+
//                     VCC  1|    |14  GND
//             (D 10)  PB0  2|    |13  AREF (D  0) 
//             (D  9)  PB1  3|    |12  PA1  (D  1) 
//                     PB3  4|    |11  PA2  (D  2) 
//  PWM  LEDR  (D  8)  PB2  5|    |10  PA3  (D  3) 
//  PWM  LEDG  (D  7)  PA7  6|    |9   PA4  (D  4) 
//  PWM  LEDB  (D  6)  PA6  7|    |8   PA5  (D  5) PWM
#include <TinyWireM.h>  // For ATtiny I2C master

// I2C address of BQ28Z610 (7-bit format)
const uint8_t BQ28Z610_ADDR = 0x55;

// Command code for Relative State of Charge
const uint8_t SOC_CMD = 0x2C;

// LED output pin
const uint8_t LED_R = 8; 
const uint8_t LED_G = 7;
const uint8_t LED_B = 6;

// Reads a 2-byte word from the specified register of BQ28Z610
uint16_t readWordFromBQ28Z610(uint8_t command) {
  TinyWireM.beginTransmission(BQ28Z610_ADDR);
  TinyWireM.write(command);             // Command register (e.g., 0x2C for SOC)
  TinyWireM.endTransmission(false);     // Send repeated start (no stop)

  TinyWireM.requestFrom(BQ28Z610_ADDR, 2); // Request 2 bytes from device
  uint8_t lsb = TinyWireM.read();
  uint8_t msb = TinyWireM.read();

  return (msb << 8) | lsb; // Combine into 16-bit word
}

void setup() {
  TinyWireM.begin();           // Initialize I2C
  pinMode(LED_R, OUTPUT);    // Set LED pins as output
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
}

void loop() {
  uint16_t soc = readWordFromBQ28Z610(SOC_CMD); // Read battery % (0–100)

  if (soc > 40) { //green
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  } 
  else if (soc > 20 ){ //yellow
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }
  else if (soc > 10){ //solid red
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }
  else { //blinking red
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
    digitalWrite(LED_R, HIGH);
    delay(250);
    digitalWrite(LED_R, LOW);
  }
  delay(250); // Wait 0.25 seconds before reading again
}