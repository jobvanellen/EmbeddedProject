// Arduino master code
// by Dennis de Jong

// Code for the master control of a RP6 robot

// Created 15 March 2016
// Last updated 22 March 2016


#include <Wire.h>
#define ADDRESSCOMPASS 0x60 // Defines address of compass

const int pingPin = 48; // Pin that is used for the ping sensor
int compass = 0; // The direction of the compass in degrees
int distance = 0; // The distance to a object in front of the RP6 in degrees
int counter = 0; // Counter used for counting cycles of the program

// Starts the I2C and serial connections
void setup() {
  Wire.begin(); 
  Serial.begin(9600);
  printStartInfo();
}

// Reads the sensors and checks for input from the serial connections
void loop() {
  readOut();
  input();
  sendData();
}

//Prints the info to control the RP6
void printStartInfo(){
  Serial.println("#################################################################");
  Serial.println("#                                                               #");
  Serial.println("#                Controls for the RP6 robot                     #");
  Serial.println("#                                                               #");
  Serial.println("#                                                               #");
  Serial.println("#       Movement                                                #");
  Serial.println("#                                                               #");
  Serial.println("#       W - Forward                                             #");
  Serial.println("#       A - Left                                                #");
  Serial.println("#       S - Backward                                            #");
  Serial.println("#       D - Right                                               #");
  Serial.println("#       K - Faster                                              #");    
  Serial.println("#       M - Slower                                              #");
  Serial.println("#                                                               #");
  Serial.println("#       Information                                             #");
  Serial.println("#                                                               #");
  Serial.println("#       I - Information                                         #");
  Serial.println("#                                                               #");
  Serial.println("#                                                               #");
  Serial.println("#################################################################\n");
  
}

// Reads each individual sensor
void readOut(){
  readCompass();
  readPing();
}

// Checks if there is input from the serial connections and calls the functions that rely on serial input
void input(){
  if(Serial.available()){
    char serialInput = Serial.read();
    switch(serialInput) {
      case 'i' :
        printInfo();
        break;
      case 'w' :
        control(serialInput);
        break;
      case 'a' :
        control(serialInput);
        break;
      case 's' :
        control(serialInput);
        break;
      case 'd' :
        control(serialInput);
        break;
      case 'k' :
        control(serialInput);
        break;
      case 'm' :
        control(serialInput);
        break;
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

// Reads the ping sensor and stores the distance to a next object in cm
void readPing() {
  long duration;
  int cm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  distance = (duration / 29 / 2);
}

// If an 'i' is put in via serial, the direction of the RP6 is printed
void printInfo(){
  printDirection();
  printDistance();
}

// Writes chars to the RP6 via I2C for the manual control of the robot
void control(char c){
  byte x = c;
  Wire.beginTransmission(84);
  Wire.write(x);             
  Wire.endTransmission();
}

// Prints the direction of the RP6
void printDirection(){
  Serial.print("Direction:                ");
  
  if(compass > 338 || compass <= 23) {
    Serial.println("North");
  } else if(compass > 23 && compass <= 68) {
    Serial.println("Northeast");
  } else if(compass > 68 && compass <= 113) {
    Serial.println("East");
  } else if(compass > 113 && compass <= 158) {
    Serial.println("Southeast");
  } else if(compass > 158 && compass <= 203) {
    Serial.println("South");
  } else if(compass > 203 && compass <= 248) {
    Serial.println("Southwest");
  } else if(compass > 248 && compass <= 293) {
    Serial.println("West");
  } else if(compass > 293 && compass <= 338) {
    Serial.println("Northwest");
  }
}

// Prints the distance to the next object in cm
void printDistance(){
  Serial.print("Distance to next object:  ");
  if(distance > 300){
    Serial.println("No object detected");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}

// Sends data from the sensors to the slave RP6
void sendData(){

  // The value of the compass in degrees is sended to the slave
  int temp = compass;
  Wire.beginTransmission(84);
  Wire.write('i');
  Wire.write(temp & 0xFF00);
  Wire.write(temp & 0xFF);             
  Wire.endTransmission();

  // If the distance to an object is 8cm or closer for 100 cycles 
  // an 'o' is sended to the slave
  if(distance <= 8) {
    counter++;
    if(counter == 100){
      Wire.beginTransmission(84);
      Wire.write('o');
      Wire.endTransmission();
      counter = 0;
    }
  }
}



