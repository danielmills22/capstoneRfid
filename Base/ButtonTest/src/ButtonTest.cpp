/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/ButtonTest/src/ButtonTest.ino"
/*
 * Project ButtonTest
 * Description: Test for button 
 * Author:
 * Date:
 */


#include "clickButton.h"
void setup();
void loop();
#line 10 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/ButtonTest/src/ButtonTest.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);


// the Button
const int BUTTONPIN1 = D5;
ClickButton button1(BUTTONPIN1, LOW, CLICKBTN_PULLUP);

// Button results 
int function = 0;


void setup()
{
  Serial.begin(9600);
  pinMode(BUTTONPIN1, INPUT_PULLUP);

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register
}


void loop()
{
  // Update button state
  button1.Update();

  // Save click codes in LEDfunction, as click codes are reset at next Update()
  if(button1.clicks != 0) {
    function = button1.clicks;
  }

  if(function == 1){ 
      Serial.printf("SINGLE click");
  }

  if(function == 2){
     Serial.printf("DOUBLE click");
  }

  if(function == 3){
    Serial.printf("TRIPLE click");
  }

  if(function == -1){ 
    Serial.printf("SINGLE LONG click");
  }

  if(function == -2){ 
    Serial.printf("DOUBLE LONG click");
  }

  if(function == -3){
     Serial.printf("TRIPLE LONG click");
  }
  
  function = 0;
}
