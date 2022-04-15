/*
 * Project Capstone_Project
 * Description: RFID Capstone Project
 * Author: Daniel Mills
 * Date:04-13-2022
 */

//SYSTEM_MODE(SEMI_AUTOMATIC);

///////////////////////////////
//-----Include and Objects Block
#include "credential.h"                    //creddential for ada fruit dashboard
#include "math.h"
#include <Adafruit_GFX.h>                   //library for graphics
#include <Adafruit_SSD1306.h>               //library for OLED
#include <Adafruit_MQTT.h>                  //Library for publishing to Adafruit
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"


//Var for Encoder
Encoder myEnc(A3, A4);
int oldPosition;
int newPosition;
int encoderMap;

//Initialization
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);
String DateTime, TimeOnly;

// Piezo Sensor Var
const int PIEZOSENSORPIN = A4;      //pin for the pressure sensor
float pSensor;         //stores the piezo sensor readings

//time vars
int startTime;
int lastTime;

//Button Vars
const int BUTTON = D5;
bool buttonState;
bool lastButtonState;
bool clickState;
bool onOff;

void setup() {
  Serial.begin(9600);
  
  //Setting the Pinmode
  pinMode(BUTTON, INPUT);
  pinMode(PIEZOSENSORPIN, INPUT); //pin for collecting data from Pressure data from Piezo Sensor

  //Getting Time Info
  Time.zone(-7);
  Particle.syncTime();

  //##Setup Information for the OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display(); // show splashscreen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("Monitoring System \n");
  //display.printf(DateTime.c_str());
  //display.printf(TimeOnly.c_str());
  display.display();
  delay(2000);
}


void loop() {
  pSensor = analogRead(PIEZOSENSORPIN);   //reads the vibration sensor values 
  buttonState = digitalRead(BUTTON);
  Serial.printf("Waiting for input \n");
  //if((millis()-startTime) > 500) {   
  //   Serial.printf("StartTime %i | Vibs %.02f \n", startTime, pSensor);
  //   //Particle.publish("pSensor", String(pSensor));
  //  startTime = millis();   
  // } 

  //Encoder Info for Menu
  int newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    if (newPosition > 20){
      myEnc.write(20);
    }
    if (newPosition < 0){
      myEnc.write(0);
    }
  }

  encoderMap = map(newPosition, 0, 20, 0, 4);

  // enter switch case
  switch(encoderMap){
    case 0: 
      display.clearDisplay();             //clears the display 
      display.setCursor(0,0);             // Start at top-left corner
      display.setTextColor(WHITE);
      display.printf("Menu: \n Case0 < \n Case1 \n Case2 \n Case3 \n Case4 \n ");   //Outputs Switch Case
      display.display();         
    break;
    case 1: 
      display.clearDisplay();      //clears the display 
      display.setTextColor(WHITE);
      display.setCursor(0,0);             // Start at top-left corner
      display.printf("Menu: \n Case0 \n Case1 < \n Case2 \n Case3 \n Case4 ");   //Outputs Switch Case
      display.display();
    break;
    //Start of case 2
    case 2:
      //display.setTextSize(1);                                // Draw 2X-scale text (too large for screen)
      display.clearDisplay();      //clears the display 
      display.setTextColor(WHITE);
      display.setCursor(0,0);             // Start at top-left corner
      display.printf("Menu: \n Case0 \n Case1 \n Case2 < \n Case3 \n Case4 ");   //Outputs Switch Case
      display.display();
    break;
    //Start of case 3
    case 3:
      display.clearDisplay();             //clears the display
      display.setTextColor(WHITE); //sets the display color to white
      display.setCursor(0,0);             // Start at top-left corner
      display.printf("Menu: \n Case0 \n Case1 \n Case2 \n Case3 < \n Case4 \n ");   //Outputs Switch Case
      display.display();   //shows the display
    break;
    //Start of Case 4
    case 4:
      display.clearDisplay();             //clears the display
      display.setTextColor(WHITE);
      display.setCursor(0,0);             // Start at top-left corner
      display.printf("Menu: \n Case0 \n Case1 \n Case2 \n Case3 \n Case4 < \n ");   //Outputs Switch Case
      display.display();
    break;
    default:
      display.clearDisplay();             //clears the display
      display.setTextColor(WHITE);
      display.setCursor(0,0);             // Start at top-left corner
      display.printf("Error");            //Outputs Switch Case
      display.display();
    break;
    
    oldPosition = encoderMap;
  }

}

