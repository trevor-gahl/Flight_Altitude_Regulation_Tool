#include <Wire.h>
#include <Stepper.h>

#ifndef F_CPU
  #define F_CPU 16000000                       //Setting F_CPU for library correct timing as a precaution (16MHz)
#endif

const int valveRotations = 1.5;
const int stepsPerRevolution = 510;
const int openvalve = 'O';
const int closevalve ='C';
const int nichrome = 'N';
const int fanstart = 'F';
const int fanstop = 'S';
bool opened;
bool closed;
volatile int c;
volatile char incoming;

Stepper valveMotor(stepsPerRevolution, 4,5,6,7);





void setup() {
  //Motor initialization and POST
  valveMotor.setSpeed(30);      //set rpm
  digitalWrite(3,HIGH);         //recruit sleeper
  //valveMotor.step(stepsPerRevolution*.5);    //turn clockwise (open)
  //valveMotor.step(-stepsPerRevolution*.5);   //turn counter-clockwise (close)
  digitalWrite(3,LOW);          //assasinate sleeper
  opened = false;
  closed = true;
  //I2C initialization
  Wire.begin();
  
  incoming = '0'; //Initialize array to ascii code zeros
}

void loop() {
  Wire.requestFrom(2,1);
  incoming = Wire.read(); // receive a byte as character
  
  if(incoming == openvalve){
    if(!opened){
    incoming = '0';  //Reset incoming to defaults
    digitalWrite(3,HIGH);
    valveMotor.step(stepsPerRevolution);
    digitalWrite(3,LOW); 
    opened = true;
    closed = false;
    }else{
      digitalWrite(3,LOW); 
    }
  }else if(incoming == closevalve){
    if(!closed){
    incoming = '0';  //Reset incoming to defaults
    digitalWrite(3,HIGH);
    valveMotor.step(-stepsPerRevolution);
    digitalWrite(3,LOW);  
    closed = true;
    opened = false;
    }else{
      digitalWrite(3,LOW);
    }
  }else{
    digitalWrite(3,LOW);
  }
}

