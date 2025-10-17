// ATMEL ATTINY84 / ARDUINO
//
//                           +-\/-+
//                     VCC  1|    |14  GND
//        SCLK (D 10)  PB0  2|    |13  AREF (D  0) txInd
//       micIn (D  9)  PB1  3|    |12  PA1  (D  1) fsync
//                     PB3  4|    |11  PA2  (D  2) led
//  PWM  INT0  (D  8)  PB2  5|    |10  PA3  (D  3) spkrOut
//  PWM  lrck  (D  7)  PA7  6|    |9   PA4  (D  4) hciTXD
//  PWM   bck  (D  6)  PA6  7|    |8   PA5  (D  5) hciRXD  PWM
//
void setup() {
  //define pin names (assuming CW)
  #define txInd 0 // TX_IND for BLE  
  #define fsync 1 // see above (fsync signal for ADC)
  #define led 2 // status led signal (may not work)
  #define spkrOut 3 // output to DAC
  #define hciTXD 4 // HCI_TXD signal from BLE
  #define hciRXD 5 // HCI_RXD signal to BLE
  #define bck 6 // bck signal for ADC and DAC
  #define lrck 7 // lrck signal for ADC and DAC
  #define micIn 9 // Physical 2 = tiny24 pin 10 (output from ADC)

  //define pin funtions (input/output)
  pinMode(txInd, INPUT); // txInd is input from BLE
  pinMode(fsync, OUTPUT); // fsync control signal to ADC
  pinMode(led, OUTPUT); // transmission status LED
  pinMode(spkrOut, OUTPUT); // recieved data for DAC
  pinMode(hciTXD, INPUT); // data received from BLE module
  pinMode(hciRXD, OUTPUT); // data for BLE module to transmit
  pinMode(bck, OUTPUT); // bck control signal for ADC and DAC
  pinMode(lrck, OUTPUT); // lrck control signal for ADC and DAC 
  pinMode(micIn, INPUT); // data  from ADC to be sent to BLE
}

//need to finetune delays to meet minimum delay periods defined in ADC and DAC specifications
void loop() {
  // put your main code here, to run repeatedly:
  delay(1);
  digitalWrite(fsync, HIGH);
  while(true) {
    //left channel
    digitalWrite(lrck, HIGH); //lrck high to indicate left channel
    digitalWrite(led, digitalRead(txInd)); //updates the status LED and provides delay for setup time
    //check tx_ind status and update LED for delay
    //loop iterates the number of bits in each channel for one set of data
    for (int bitNum = 1; bitNum < 25; bitNum++) {
      digitalWrite(bck, HIGH); // bck high pulse
      digitalWrite(spkrOut, digitalRead(hciTXD)); //send bit from BLE module to DAC
      digitalWrite(bck, LOW); // bck low pusle
      digitalWrite(hciRXD, digitalRead(micIn)); //send bit from ADC to BLE module
    }
    digitalWrite(lrck, LOW); //lrck low to indicate right channel
    digitalWrite(led, digitalRead(txInd)); //updates the status LED and provides delay for setup time
    //insert delay funciton if needed (LRCK setup time)
    //loop iterates the number of bits in each channel for one set of data
    for (int bitNum = 1; bitNum < 25; bitNum++) { 
      digitalWrite(bck, HIGH); // bck high pulse
      digitalWrite(spkrOut, digitalRead(hciTXD)); //send bit from BLE module to DAC
      digitalWrite(bck, LOW); // bck low pusle
      digitalWrite(hciRXD, digitalRead(micIn)); //send bit from ADC to BLE module
    }
  }
}
