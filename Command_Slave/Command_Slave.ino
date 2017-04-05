#include <Wire.h>

#ifndef F_CPU
  #define F_CPU 16000000                       //Setting F_CPU for library correct timing as a precaution (16MHz)
#endif

volatile int test;
volatile int count;
char incoming;
char lastincome;
const char openvalve = 'O';
const char closevalve = 'C';

void setup() {
  // put your setup code here, to run once:
  Wire.begin(2);
  Wire.onRequest(requestEvent);
  //Serial Initialization for XBEE Communication:
  Serial.begin(9600);                           //Initialize Serial Com with baudrate
  Serial.setTimeout(1000);                      //Set recieve timeout in milliseconds
  while(Serial.available() > 0){
      Serial.read();
  }
   incoming = '0'; //Initialize array to ascii code zeros

}

void loop() {
  // put your main code here, to run repeatedly:
  
while(Serial.available() > 0){                                    //Need to let the buffer build up between reads                   
  incoming = Serial.read();                     //New character added to array
if(incoming == openvalve){  //Reset incoming to defaults
    lastincome = incoming;
    incoming = '0';
    digitalWrite(13,HIGH);
    delay(10);
    digitalWrite(13,LOW);
    while(Serial.available() > 0){+
      Serial.read();                            //Empty any characters in buffer (prevents multiple commands from queuing)
    }
}else if(incoming == closevalve){
  lastincome = incoming;
    incoming = '0';
    digitalWrite(13,HIGH);
    delay(10);
    digitalWrite(13,LOW);
    while(Serial.available() > 0){
      Serial.read();                            //Empty any characters in buffer (prevents multiple commands from queuing)
    }
}else{
    lastincome = '0';
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    while(Serial.available() > 0){
      Serial.read();                            //Empty any characters in buffer (prevents multiple commands from queuing)
    }
  }
  //if(count < 2){
      //incoming = 0;
  //}else{
      //incoming = 1;
  }
}
void requestEvent(){
  Wire.write(lastincome);
}


