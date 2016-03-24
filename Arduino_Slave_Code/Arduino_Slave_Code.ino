// Arduino Slave Code
// by Dennis de Jong

// Code for a slave arduino for testing purposes for the RP6 project

// Created 15 March 2016
// Last updated 22 March 2016

#include <Wire.h>

int compass = 0;

// Starts I2C connetion
void setup() {
  Wire.begin(84);                
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent);
  Serial.begin(9600);           
}

// Does nothing but wait for the master to require something
void loop() {
  delay(100);
}

// Prints received data as a char
void receiveEvent(int howMany) {
  char x = Wire.read();
  if(x == 'i') {
    // Gets the value for the compass variable
    while(Wire.available() < 2);
    byte highByte = Wire.read();
    byte lowByte = Wire.read();
    compass = ((highByte<<8)+lowByte);
      
  } else if(x == 'o') {
    // Prints "Object detected" if an 'o' is received
    Serial.println("Object detected");
  } else {
    // All other received chars are printed via Serial
    Serial.println(x);
  }
}

// Prints 'W' upon request by the master (Not used)
void requestEvent() {
  Wire.write("W"); 
}
