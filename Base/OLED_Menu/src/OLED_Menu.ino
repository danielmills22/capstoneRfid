/*
 * Project OLED_Menu
 * Description:
 * Author:
 * Date:
 */

//Header Files
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <Encoder.h>
SYSTEM_MODE(SEMI_AUTOMATIC);

//Var for Encoder
Encoder myEnc(A3, A4);
int oldPosition;
int newPosition;
int encoderMap;

//Initialization
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);
String DateTime, TimeOnly;


void setup() {
  Serial.begin(9600);
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

// loop() runs over and over again, as quickly as it can execute.
void loop() { 
  //Data and Time Display
  //DateTime = Time.timeStr(); // Current Date and Time from Particle Time class
  //TimeOnly = DateTime.substring(11 ,19) ; // Extract the Time from the DateTime String
  //display.println("The Date is: %i \n",DateTime.c_str());  //format for 
  //display.println("The time is: %i \n",TimeOnly.c_str());
  //display.display();
  //display.clear();

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
  switch(encoderMap)
    {
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
        delay(2000);                        //delays the clear display for 2 seconds
      break;

    oldPosition = encoderMap;
  }
}





