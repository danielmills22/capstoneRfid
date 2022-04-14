/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Capstone_Project/src/Capstone_Project.ino"
/*
 * Project Capstone_Project
 * Description: RFID Capstone Project
 * Author: Daniel Mills
 * Date:04-13-2022
 */

//SYSTEM_MODE(SEMI_AUTOMATIC);

// Piezo Sensor Var
void setup();
void loop();
#line 11 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Capstone_Project/src/Capstone_Project.ino"
const int PIEZOSENSORPIN = A4;      //pin for the pressure sensor
float pSensor;         //stores the piezo sensor readings

//time vars
int startTime;

//Button Vars
const int BUTTON = D5;
bool buttonState;
bool lastButtonState;
bool clickState;
bool onOff;


// setup() runs once, when the device is first turned on.
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(PIEZOSENSORPIN, INPUT); //pin for collecting data from Pressure data from Piezo Sensor
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  pSensor = analogRead(PIEZOSENSORPIN);   //reads the vibration sensor values 
  buttonState = digitalRead(BUTTON);
  Serial.printf("Waiting for input \n");

    if((millis()-startTime) > 500) {   
      Serial.printf("StartTime %i | Vibs %.02f \n", startTime, pSensor);
      Particle.publish("pSensor", String(pSensor));
      startTime = millis();   
    }
  
}

