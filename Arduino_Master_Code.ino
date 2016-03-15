// Arduino master code
// by Dennis de Jong

// Code for the master control of a RP6 robot

// Created 15 March 2016


#include <Wire.h>
#define ADDRESSCOMPASS 0x60 //defines address of compass

int compass = 0;

// Starts the I2C and serial connections
void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}

// Reads the sensors and checks for input from the serial connections
void loop() {
  readOut();
  input();
}

// Reads each individual sensor (for now only the compass is implemented)
void readOut(){
  readCompass();
}

// Checks if there is input from the serial connections and calls the functions that rely on serial input
void input(){
  if(Serial.available()){
    char serialInput = Serial.read();
    
    if(serialInput == 'i') {
      printInfo();
    }
    if(serialInput == 'w' || serialInput == 'd' || serialInput == 'a' || serialInput == 's'){
      control(serialInput);
    }
  }    
}

// Reads the compass and puts the value in compass
void readCompass(){
  byte highByte;
  byte lowByte;
  
  Wire.beginTransmission(ADDRESSCOMPASS);      //starts communication with cmps03
  Wire.write(2);                         //Sends the register we wish to read
  Wire.endTransmission();

  Wire.requestFrom(ADDRESSCOMPASS, 2);        //requests high byte
  while(Wire.available() < 2);         //while there is a byte to receive
  highByte = Wire.read();           //reads the byte as an integer
  lowByte = Wire.read();
  compass = ((highByte<<8)+lowByte)/10; 
}

// If an 'i' is put in via serial, the direction of the robot is printed
void printInfo(){
  printDirection();
}

// Writes chars to the RP6 via I2C for the manual control of the robot
void control(char c){
  byte x = c;
  Wire.beginTransmission(84);
  Wire.write(x);             
  Wire.endTransmission();
}

// Prints the direction of the robot
void printDirection(){
  Serial.print("The direction is: ");
  
  if(compass > 315 || compass <= 45) {
    Serial.println("North");
  } else if(compass > 45 && compass <= 135) {
    Serial.println("East");
  } else if(compass > 135 && compass <= 225) {
    Serial.println("South");
  } else if(compass > 225 && compass <= 315) {
    Serial.println("West");
  }
}



