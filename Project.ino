// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

int compass = 0;
char serialInput = '0';

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}


void loop() {
  input();
  besturing();
  
//    if(c == 'i') {
//       Wire.requestFrom(8, 1);
//       while(Wire.available()) {
//        char c = Wire.read();
//        printDirection(c);
//       }
//    }
}
void input(){
  if(Serial.available()){
    serialInput = Serial.read();
  }    
}
void besturing(){
   byte x = 0;
   if(serialInput == 'w' || serialInput == 'd' || serialInput == 'a' || serialInput == 's'){
      x = serialInput;
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write(x);              // sends one byte
      Wire.endTransmission();    // stop transmitting
    }
}
void printDirection(char c) {
  Serial.print("The direction is: ");
  switch (c) {
      case 'N':
        Serial.println("North");
        break;
      case 'E':
        Serial.println("East");
        break;
      case 'W':
        Serial.println("West");
        break;
      case 'S':
        Serial.println("South");
        break;
      default:
        Serial.println("Unkown");
        break;
  }
}

