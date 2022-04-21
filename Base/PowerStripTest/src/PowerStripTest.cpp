/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/PowerStripTest/src/PowerStripTest.ino"
/*
 * Project PowerStripTest
 * Description:
 * Author:
 * Date:
 */


void setup();
void loop();
#line 9 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/PowerStripTest/src/PowerStripTest.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// setup() runs once, when the device is first turned on.
const int POWERSTRIP = A5;

void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(POWERSTRIP, OUTPUT);

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  digitalWrite(POWERSTRIP, HIGH);
  Serial.printf("Power strip state %i \n", POWERSTRIP);
}