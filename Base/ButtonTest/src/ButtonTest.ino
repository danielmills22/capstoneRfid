/*
 * Project ButtonTest
 * Description: Test for button 
 * Author:
 * Date:
 */


#include "clickButton.h"


// the Button
const int buttonPin1 = D4;
ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);

// Button results 
int function = 0;


void setup()
{
  Serial.begin(9600);
  pinMode(D4, INPUT_PULLUP);

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
  if(button1.clicks != 0) function = button1.clicks;
  
  if(function == 1) Serial.println("SINGLE click");

  if(function == 2) Serial.println("DOUBLE click");

  if(function == 3) Serial.println("TRIPLE click");

  if(function == -1) Serial.println("SINGLE LONG click");

  if(function == -2) Serial.println("DOUBLE LONG click");

  if(function == -3) Serial.println("TRIPLE LONG click");
  
  function = 0;
  delay(5);
}
