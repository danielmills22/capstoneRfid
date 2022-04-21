/*
 * Project PowerStripTest
 * Description:
 * Author:
 * Date:
 */


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
  digitalWrite(POWERSTRIP, LOW);
  Serial.printf("Power strip state %i \n", POWERSTRIP);
}