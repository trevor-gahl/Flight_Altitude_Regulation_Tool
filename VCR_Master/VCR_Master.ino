#include <Wire.h> //Library for I2C communication
#include <NMEAGPS.h> //Library for parsing NMEA GPS Strings
#include <NeoSWSerial.h> //Library to use any pins as serial Rx and Tx 
#include <Stepper.h> //Library to control stepper motor

NMEAGPS  gps;

//Setting F_CPU for library correct timing as a precaution (16MHz)
#ifndef F_CPU
  #define F_CPU 16000000
#endif

//Pin Definitions for Serial Communication
#define Rxpin A5
#define Txpin A4

//Pin Definitions for Stepper Motor 
#define sPin1 A0
#define sPin2 A1
#define sPin3 A2
#define sPin4 A3
#define sleeper 3

//Pin Definition for DC Motor
#define dPin 4

const int maxAltitude = 22680;
const int valveRotations = 1.5;
const int stepsPerRevolution = 510;
const int openvalve = 'V';
const int closevalve ='C';
//const int nichrome = 'N'; Handled by OCCAMS Board
const int fanstart = 'F';
const int fanstop = 'S';
bool opened;
bool closed;
bool fanOn;
bool fanOff;
volatile int altitude;
volatile int c;
volatile char incoming;

Stepper valveMotor(stepsPerRevolution, sPin1,sPin2,sPin3,sPin4);
NeoSWSerial gpsPort(Rxpin,Txpin);




void setup() {
  //GPS Initialization
  gpsPort.begin(9600);
  
  //Motor initialization and POST
  valveMotor.setSpeed(30);      //set rpm
  digitalWrite(sleeper,HIGH);         //recruit sleeper
  //valveMotor.step(stepsPerRevolution*.5);    //turn clockwise (open)
  //valveMotor.step(-stepsPerRevolution*.5);   //turn counter-clockwise (close)
  digitalWrite(sleeper,LOW);          //assasinate sleeper
  opened = false;
  closed = true;
  
  //I2C initialization
  Wire.begin();
  incoming = '0'; //Initialize array to ascii code zeros
}

void loop() {
  //Grab GPS packet
  if(gps.available(gpsPort)){
    gps_fix fix = gps.read();
    if (fix.valid.altitude){
      altitude = fix.altitude();
    }
  }

  //Check for max altitude to open valve

  if(altitude >= maxAltitude && !opened){
    digitalWrite(sleeper,HIGH);
    valveMotor.step(stepsPerRevolution);
    digitalWrite(sleeper,LOW);
    digitalWrite(dPin,HIGH);
    opened = true;
    fanOn = true;
  }


  //Grab command status from Occams
  Wire.requestFrom(2,1);
  incoming = Wire.read(); // receive a byte as character
  
  if(incoming == openvalve){
    if(!opened){
    incoming = '0';  //Reset incoming to defaults
    digitalWrite(sleeper,HIGH);
    valveMotor.step(stepsPerRevolution);
    digitalWrite(sleeper,LOW); 
    opened = true;
    closed = false;
    }else{
      digitalWrite(sleeper,LOW); 
    }
    
  }else if(incoming == closevalve){
    if(!closed){
    incoming = '0';  //Reset incoming to defaults
    digitalWrite(sleeper,HIGH);
    valveMotor.step(-stepsPerRevolution);
    digitalWrite(sleeper,LOW);  
    closed = true;
    opened = false;
    }else{
      digitalWrite(sleeper,LOW);
    }
    
  }else if(incoming == fanstart){
    if(!fanOn){
    digitalWrite(dPin, HIGH);
    fanOn = true;
    fanOff = false;
    }
    
  }else if(incoming == fanstop){
    if(!fanOff){
    digitalWrite(dPin, LOW);
    fanOff = true;
    fanOn = false;
    }
    
  }else{
    digitalWrite(sleeper,LOW);
    digitalWrite(dPin, LOW);
  }
  
}

