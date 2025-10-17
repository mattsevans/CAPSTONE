#include <Wire.h>

const uint8_t SDA_PIN = A4; // UNO / Nano
const uint8_t SCL_PIN = A5;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("I2C safe scanner with bus recovery");

  Wire.begin(); // join I2C bus as master
  delay(10);
  safeBusRecoverIfNeeded();
  scanAllAddresses();
}

void loop() {
  // nothing - for demo we only scan once
  delay(10000);
}

// If either line is low, try to recover the bus.
void safeBusRecoverIfNeeded() {
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(1);

  bool sdaLow = (digitalRead(SDA_PIN) == LOW);
  bool sclLow = (digitalRead(SCL_PIN) == LOW);

  if (!sdaLow && !sclLow) {
    Serial.println("Bus idle (SDA and SCL high).");
    return;
  }

  Serial.println("Bus not idle - attempting recovery...");
  busRecover();
  // reinit Wire in case we toggled hardware lines
  Wire.end();
  Wire.begin();
  delay(5);

  // check again
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(1);
  if (digitalRead(SDA_PIN) == HIGH && digitalRead(SCL_PIN) == HIGH) {
    Serial.println("Recovery successful.");
  } else {
    Serial.println("Recovery failed: lines still low.");
  }
}

// Do manual clock pulses on SCL to free a stuck slave, then generate STOP
void busRecover() {
  // Disable TWI hardware on AVR so we can control pins manually.
  // For AVR (Uno/Nano) this is OK (TWCR register). If your board is different,
  // you may need a different method to release the I2C hardware.
#if defined(TWCR)
  TWCR = 0; // disable TWI (AVR-specific). safe on ATmega328-based Arduinos.
#endif

  // Make sure we control the pins
  pinMode(SDA_PIN, INPUT_PULLUP); // we will read SDA
  pinMode(SCL_PIN, OUTPUT);

  // Pulse SCL up to 16 times (typical advice: up to 9 pulses for stuck slaves + safety)
  for (int i = 0; i < 16; ++i) {
    // If SDA released, we can try to send STOP
    if (digitalRead(SDA_PIN) == HIGH) break;

    // drive SCL low then high
    digitalWrite(SCL_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(5);

    // small delay to allow slave to react
    delay(1);
  }

  // Try to generate a STOP condition: while SCL high, toggle SDA low->high
  // Ensure SDA is an output only for a short stop pulse
  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, LOW);
  delayMicroseconds(5);
  // release SCL and SDA
  digitalWrite(SCL_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SDA_PIN, HIGH);
  delayMicroseconds(5);

  // Set both back to inputs with pullups so TWI can own them again
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  // Small delay to settle
  delay(5);
}

// Simple address scanner using Wire. endTransmission() returns status:
void scanAllAddresses() {
  Serial.println("Scanning I2C addresses (1..127) ...");
  for (uint8_t addr = 1; addr < 128; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("Found device at 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println("  (ACK)");
    } else if (err == 4) {
      Serial.print("Unknown error at 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println();
    }
    // Optional: call recovery if bus seems hung between transactions
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);
    if (digitalRead(SCL_PIN) == LOW || digitalRead(SDA_PIN) == LOW) {
      Serial.println("Bus became busy during scan: attempting recovery...");
      safeBusRecoverIfNeeded();
    }
    delay(1); // small gap
  }
  Serial.println("Scan complete.");
}
