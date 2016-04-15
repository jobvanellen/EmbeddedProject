// Arduino master code
// by Dennis de Jong

// Code for the master control of a RP6 robot

// Created 15 March 2016
// Last updated 22 March 2016


#include <Wire.h>
#define ADDRESSCOMPASS 0x60 // Defines address of compass
#include <avr/io.h>
#include <util/delay.h>

#define US_PORT PORTL
#define US_PIN   PINL
#define US_DDR    DDRL

#define US_POS PL1      //PORTA0

#define US_ERROR 0xffff
#define US_NO_OBSTACLE 0xfffe


uint16_t getPulseWidth()
{
    uint32_t i,result;

    //Wait for the rising edge
    for(i=0; i<600000; i++)
    {
        if(!(US_PIN & (1<<US_POS))) continue;
        else break;
    }
    if(i==600000)
        return 0xffff; //Indicates time out
    //High Edge Found
    //Setup Timer1
    TCCR1A=0X00;
    TCCR1B=(1<<CS11); //Prescaler = Fcpu/8
    TCNT1=0x00;       //Init counter
    //Now wait for the falling edge
    for(i=0; i<600000; i++)
    {
        if(US_PIN & (1<<US_POS))
        {
            if(TCNT1 > 60000) break;
            else continue;
        }
        else
            break;
    }
    if(i==600000)
        return 0xffff; //Indicates time out
    //Falling edge found
    result=TCNT1;
    //Stop Timer
    TCCR1B=0x00;
    if(result > 60000)
        return 0xfffe; //No obstacle
    else
        return (result>>1);
}

const int pingPin = 48; // Pin that is used for the ping sensor
int compass = 0; // The direction of the compass in degrees
int distance = 0; // The distance to a object in front of the RP6 in degrees
int distanceDriven = 0; // The distance the RP6 has driven in cm
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
  //askData();
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
  Serial.println("#       O - Stop                                                #");
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
      case 't' :
        control(serialInput);
        break;  
      case 'o' :
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
  duration = getPulseWidth();

  distance = (duration / 29 / 2);
}

// If an 'i' is put in via serial, the direction of the RP6 is printed
void printInfo(){
  printDirection();
  printDistance();
  printDistanceDriven();
}

// Writes chars to the RP6 via I2C for the manual control of the robot
void control(char c){
  byte x = c;
  Wire.beginTransmission(0x20);
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

void printDistanceDriven() {
   Serial.print("Distance driven:          ");
   Serial.print(distanceDriven);
   Serial.println(" cm\n");
}

// Sends data from the sensors to the slave RP6
void sendData(){

  
  //The value of the compass in degrees is sended to the slave
  int temp = compass;
  uint8_t tempL = temp & 0xFF;
  uint8_t tempH = (temp >> 8) & 0xFF; 
  Wire.beginTransmission(0x20);
  Wire.write('i');
  Wire.write(tempH);
  Wire.write(tempL);             
  Wire.endTransmission();
  
  // If the distance to an object is 8cm or closer for 100 cycles 
  // an 'o' is sended to the slave
  if(distance <= 8) {
    counter++;
    if(counter == 100){
      Serial.println("Stop!");
      byte x = 'o';
      Wire.beginTransmission(0x20);
      Wire.write(x);             
      Wire.endTransmission();
      counter = 0;
    }
  }
}

// Requests distanceDriven from the slave 
void askData(){
  Wire.beginTransmission(0x20);
  Wire.write('p');
  Wire.endTransmission();
  Wire.requestFrom(0x20, 2);
  while(Wire.available()){
    Serial.println(Wire.available());
    Serial.println("waiting on data from rp6");
  }
  byte highByte = Wire.read();
  byte lowByte = Wire.read();
  distanceDriven = ((highByte<<8)+lowByte);
}



