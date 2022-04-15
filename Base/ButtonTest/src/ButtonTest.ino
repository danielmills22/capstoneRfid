/*
 * Project ButtonTest
 * Description: Test for button 
 * Author:
 * Date:
 */


#include "clickButton.h"
SYSTEM_MODE(SEMI_AUTOMATIC);


//Button Vars
const int BUTTONPIN1 = D5;
ClickButton button1(BUTTONPIN1, LOW, CLICKBTN_PULLUP);
int function = 0;


void setup()
{
  Serial.begin(9600);
 
  //Setting Time for button clicks -- measured in millis
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register
}


void loop()
{
  //Listens for input from button
  button1.Update();
  if(button1.clicks != 0) {
    function = button1.clicks;
  }

  //Button Click Functions
  if(function == 1){ 
      Serial.printf("SINGLE click \n"); //for testing
  }

  if(function == 2){
     Serial.printf("DOUBLE click \n");
  }

  if(function == 3){
    Serial.printf("TRIPLE click \n");
  }

  if(function == -1){ 
    Serial.printf("SINGLE LONG click \n");  //for testing
  }

  if(function == -2){ 
    Serial.printf("DOUBLE LONG click \n");
  }

  if(function == -3){
     Serial.printf("TRIPLE LONG click \n");
  }
  
  function = 0;
}
