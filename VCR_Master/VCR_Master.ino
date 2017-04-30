#include <Wire.h> //Library for I2C communication
#include <NMEAGPS.h> //Library for parsing NMEA GPS Strings
#include <NeoSWSerial.h> //Library to use any pins as serial Rx and Tx 
#include <Stepper.h> //Library to control stepper motor

NMEAGPS  gps;

//Setting F_CPU for library correct timing as a precaution (16MHz)
//#ifndef F_CPU
//  #define F_CPU 16000000
//#endif

//Pin Definitions for Serial Communication
#define Rxpin 9
#define Txpin 8

//Pin Definitions for Stepper Motor 
#define sPin1 4
#define sPin2 5
#define sPin3 6
#define sPin4 7
#define sleeper 3

//Pin Definition for DC Motor
#define dPin 11


//Iniitialize variables and constants
const int maxAltitude = 22680;
const int valveRotations = 1.5;
const int stepsPerRevolution = 510;
const char openvalve = 'V';
const char closevalve ='C';
//const char nichrome = 'N'; Handled by OCCAMS Board
const char fanstart = 'F';
const char fanstop = 'S';
bool opened;
bool closed;
bool fanOn;
bool fanOff;
bool descent;
bool firstCheck;
int altitudeCount;
int initAlt;
int altitude;
int c;
char incoming;

//Initialize stepper motor and gps
Stepper valveMotor(stepsPerRevolution,4,5,6,7);
NeoSWSerial gpsPort(Rxpin,Txpin);


void setup() {
  //GPS Initialization
  gpsPort.begin(9600);
  Serial.begin(9600);
  //Motor initialization and POST
  valveMotor.setSpeed(30);                      //set rpm
  digitalWrite(sleeper,HIGH);                   //recruit sleeper
  //valveMotor.step(stepsPerRevolution);        //turn clockwise (open)
  //valveMotor.step(-stepsPerRevolution);       //turn counter-clockwise (close)
  digitalWrite(sleeper,LOW);                    //assasinate sleeper
  
  //Initialize Boolean Values
  opened = false;
  closed = true;
  fanOn = false;
  fanOff = true;
  descent = false;
  
  //I2C initialization
  Wire.begin();
  incoming = '0';                                //Initialize array to ascii code zeros
}


void loop() {
  Serial.println("Checking I2C");
  Wire.requestFrom(2,1);
  incoming = Wire.read(); // receive a byte as character
  Serial.println(incoming);
  //Grab GPS packet
  //delay(500);
  //Serial.println("new loop");
  if(gps.available(gpsPort)){
    gps_fix fix = gps.read();                     //Read GPS data if GPS available
    //Serial.println("altitude");
    
    Serial.println();
    if (fix.valid.altitude){
      altitude = fix.altitude();                  //Grab altitude for max altitude check
      altitudeCount = altitudeCount + 1;
      Serial.println(altitudeCount);
      Serial.println(altitude);
      if(firstCheck == true){
        initAlt = altitude;                       //Grab initial altitude value for bouyancy check
        firstCheck = false;
      }
    }
  }

  //Check for max altitude to open valve
  if(altitudeCount == 120){                       //Wait for 120 GPS reads to ensure valid GPS differential
    int bouyancyCheck = initAlt - altitude;
    if(bouyancyCheck <= 50 && altitude >=15000 && opened){    //Ensure the descent mode isn't triggered before launch
      descent = true;
      digitalWrite(sleeper,HIGH);
      valveMotor.step(-stepsPerRevolution);       //Open valve
      digitalWrite(sleeper,LOW);
      opened = false;
      closed = true;
      digitalWrite(dPin,LOW);                     //Spin fan blade
      fanOff = true;
      fanOn = false;
      Serial.println("Bouyancy True");
    }
    altitudeCount = 0;                            //Reset altitude count
    initAlt = altitude;                           //Reset altitude check variable
    Serial.println("Bouyancy Check");
  }


  //Check for max altitude and begin venting
  if(altitude >= maxAltitude && !opened){
    digitalWrite(sleeper,HIGH);
    valveMotor.step(stepsPerRevolution);
    digitalWrite(sleeper,LOW);
    digitalWrite(dPin,HIGH);
    opened = true;
    closed = false;
    fanOff = false;
    fanOn = true;
  }


  //Grab command status from Occams
  

  //Check incoming command against known parameters
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
      digitalWrite(12,HIGH);
    digitalWrite(dPin, HIGH);
    fanOn = true;
    fanOff = false;
    }
    
  }else if(incoming == fanstop){
    if(!fanOff){
      digitalWrite(12,LOW);
    digitalWrite(dPin, LOW);
    fanOff = true;
    fanOn = false;
    }
    
  }else{
    digitalWrite(sleeper,LOW);
    digitalWrite(dPin, LOW);
    Serial.println("No Command");
  }

  //altitudeCount = altitudeCount + 1;                //Increase gps count for bouyancy check
  //Serial.println("End of Loop");
}

