// Arduino Slave Code
// by Dennis de Jong

// Code for a slave arduino for testing purposes for the RP6 project

// Created 15 March 2016

#include <Wire.h>

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
  Serial.println(x);
}

// Prints 'W' upon request by the master
void requestEvent() {
  Wire.write("W"); 
}
