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
#include "clickButton.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

//Var for Encoder
Encoder myEnc(D2, D3);
int oldPosition;
int newPosition;
int encoderMap;

//Initialization
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);
String DateTime, TimeOnly;

//Button Vars
const int BUTTONPIN1 = D5;
ClickButton button1(BUTTONPIN1, LOW, CLICKBTN_PULLUP);
int function = 0;


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

  //Setting Time for button clicks -- measured in millis
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register

  attachInterrupt(D5, getMode, RISING);

}

void loop() { 
  //Data and Time Display
  //DateTime = Time.timeStr(); // Current Date and Time from Particle Time class
  //TimeOnly = DateTime.substring(11 ,19) ; // Extract the Time from the DateTime String
  //display.println("The Date is: %i \n",DateTime.c_str());  //format for 
  //display.println("The time is: %i \n",TimeOnly.c_str());
  //display.display();
  //display.clear();

  //Listens for input from button
  //button1.Update();
  if(button1.clicks != 0) {
    function = button1.clicks;
  }

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
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();             //clears the display 
        display.setCursor(0,0);             // Start at top-left corner
        display.setTextColor(WHITE);
        display.printf("Menu: \n Case0 < \n Case1 \n Case2 \n Case3 \n Case4 \n ");   //Outputs Switch Case
        display.display();  

          if(function == -1){ 
            Serial.printf("SINGLE LONG click \n");  //for testing
            function = 0;
          }
        Serial.printf("Function %i", function);
      break;
      case 1: 
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();      //clears the display 
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("Menu: \n Case0 \n Case1 < \n Case2 \n Case3 \n Case4 ");   //Outputs Switch Case
        display.display();

        function = 0;
      break;
     //Start of case 2
      case 2:
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();      //clears the display 
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("Menu: \n Case0 \n Case1 \n Case2 < \n Case3 \n Case4 ");   //Outputs Switch Case
        display.display();

        function = 0;
      break;
      //Start of case 3
      case 3:
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();             //clears the display
        display.setTextColor(WHITE); //sets the display color to white
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("Menu: \n Case0 \n Case1 \n Case2 \n Case3 < \n Case4 \n ");   //Outputs Switch Case
        display.display();   //shows the display

        function = 0;
      break;
      //Start of Case 4
      case 4:
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();             //clears the display
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("Menu: \n Case0 \n Case1 \n Case2 \n Case3 \n Case4 < \n ");   //Outputs Switch Case
        display.display();

        function = 0;
      break;
      default:
        if(function == 1){ 
          Serial.printf("SINGLE click \n"); //for testing
        }
        display.clearDisplay();             //clears the display
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("Error");            //Outputs Switch Case
        display.display();
        delay(2000);                        //delays the clear display for 2 seconds

        function = 0;
      break;
    
    //function = 0;
    oldPosition = encoderMap;
  }
}

/////////////////
void getMode(){
  function++;
  if(function == 2){
    function = -1;
  }
}





