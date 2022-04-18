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

void setup();
void loop();
void rfidBegin();
void rfidCardRead();
bool isMatched (uint8_t uid[4], uint8_t masterKey[4]);
#line 8 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Capstone_Project/src/Capstone_Project.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

///////////////////////////////
//-----Include and Objects Block
#include "credential.h"                    //creddential for ada fruit dashboard
#include "math.h"
#include <Adafruit_GFX.h>                   //library for graphics
#include <Adafruit_SSD1306.h>               //library for OLED
#include <Adafruit_MQTT.h>                  //Library for publishing to Adafruit
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "clickButton.h"
#include <Encoder.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

//Var for Encoder
Encoder myEnc(D2, D3);
int oldPosition;
int newPosition;
int encoderMap;

//Initialization
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);
String DateTime, TimeOnly;

// Piezo Sensor Var
//const int PIEZOSENSORPIN = A4;      //pin for the pressure sensor
//float pSensor;         //stores the piezo sensor readings

//Rfid Vars
const int PN532_IRQ  = A0;
const int PN532_RESET = A1;  
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
uint8_t valid;

//Validation System for Rfid
uint8_t masterKey[4] = {0x13, 0x99, 0xC9, 0x1E};
uint8_t uidArray[4];
bool correctKey;
bool powerAccess;

//Button Vars
const int BUTTONPIN1 = D5;  //sets encoder button to pin D5
ClickButton button1(BUTTONPIN1, LOW, CLICKBTN_PULLUP);  //clickbutton object for encoder
int function = 0;  //set button to zero

///////////////
//Vars For Current and Vibration
// PECMAC125A I2C address is 0x2A(42) //Piezo is 0x5-
#define Addr 0x2A
#define AddrP 0x50

//Piezo ((Pvalues)
long vibdat[4096][2];
int i;
int j;
byte dataP[2];

int raw_adc = 0;

//Current Vars
byte data[36];
int typeOfSensor = 0;
int maxCurrent = 0;
int noOfChannel = 0;
float current = 0.0;

//array vars
float size;
float a;
float z;
float b;
float n;
float num;

//time vars
int startTime;

void setup() {
  Serial.begin(115200);
  
  //Setting the Pinmode
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  //pinMode(PIEZOSENSORPIN, INPUT); //pin for collecting data from Pressure data from Piezo Sensor

  //Setup For Rfid
  Serial.printf("Init Reader \n");
  nfc.begin();
  
  
  /////////////////////////////////////////////
  //Rfid
  rfidBegin();
  
  //Setting Time for button clicks -- measured in millis
  button1.debounceTime   = 15;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register

  //Getting Time Info -- This is for if info wanted to be recorded with the date
  Time.zone(-7);
  Particle.syncTime();

  powerAccess = false;

  //##Setup Information for the OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display(); // show splashscreen

  
  display.setTextSize(1);
  display.clearDisplay();             //clears the display 
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextColor(WHITE);
  display.printf("Please Scan Card");   //Outputs Switch Case
  display.display(); 

  //Setup for Reading Current and Piezo through I2C
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Command header byte-1
  Wire.write(0x92);
  // Command header byte-2
  Wire.write(0x6A);
  // Command 2 is used to read no of sensor type, Max current, No. of channel
  Wire.write(0x02);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // CheckSum
  Wire.write(0xFE);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 6 bytes of data
  Wire.requestFrom(Addr, 6);

  // Read 6 bytes of data
  if (Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }
  typeOfSensor = data[0];
  maxCurrent = data[1];
  noOfChannel = data[2];

  // Output data to dashboard
  //Serial.printf("Type of Sensor %i \n",typeOfSensor);
  //Serial.printf("Max Current: %i \n", maxCurrent);
  //Serial.printf("No. of Channels: %i 'n", noOfChannel);
  //delay(5000);
}


void loop() {
 
  //Listens for input from button
  button1.Update();  
  if(button1.clicks != 0) {
    function = button1.clicks;
  }
  

  //pSensor = analogRead(PIEZOSENSORPIN);   //reads the vibration sensor values without the use of I2C
  //if((millis()-startTime) > 500) {   
  //   Serial.printf("StartTime %i | Vibs %.02f \n", startTime, pSensor);
  //   //Particle.publish("pSensor", String(pSensor));
  //  startTime = millis();   
  // } 
  
  rfidCardRead();
  
  ///////////////////////////////
  //Info will only show if Access is granted
  if (powerAccess == TRUE){

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

    //maps the encoder to the Switch Case
    encoderMap = map(newPosition, 0, 20, 0, 4);


    // enter switch case
    switch(encoderMap)
    {
      //Start
      case 0: 

        display.clearDisplay();             //clears the display 
        display.setCursor(0,0);             // Start at top-left corner
        display.setTextColor(WHITE);
        display.printf("MENU: \n-> Current Logging  \n Case1 \n Case2 \n Case3 \n Case4 \n ");   //Outputs Switch Case
        display.display(); 

        for (int j = 1; j < noOfChannel + 1; j++){
        
        //Commands for Reading Data
        Wire.beginTransmission(Addr);
        Wire.write(0x92);  //Command for byte-1
        Wire.write(0x6A);  //Command for byt-2
        Wire.write(0x01);
        Wire.write(j);
        Wire.write(j);
        Wire.write(0x00);
        Wire.write(0x00);
        // CheckSum
        Wire.write((0x92 + 0x6A + 0x01 + j + j + 0x00 + 0x00) & 0xFF);
        // Stop I2C Transmission
        Wire.endTransmission();
    
        // Request 3 bytes of data
        Wire.requestFrom(Addr, 3);

        // Read 3 bytes of data
        // msb1, msb, lsb  
        int msb1 = Wire.read();
        int msb = Wire.read();
        int lsb = Wire.read();
        current = (msb1 * 65536) + (msb * 256) + lsb;

        // Convert the data to ampere
        current = current / 1000;

        // Output data to dashboard
        Serial.printf("Channel: %i \n", j);
        Serial.printf("Current Value: %0.2f \n", current); 
        }

        //if(function == -1){ 
        //  Serial.printf("SINGLE LONG click \n");  //for testing
        //  break;
        //} 

      break;
      //Start
      case 1: 
 
        Serial.printf("Function %i \n", function);

        if (function == 0){
          display.clearDisplay();      //clears the display
          display.setTextColor(WHITE);
          display.setCursor(0,0);             // Start at top-left corner
          display.printf("MENU: \n Case0 \n->Vib  \n Case2 \n Case3 \n Case4 ");   //Outputs Switch Case
          display.display();
        }
        else{
          ////////////////////////////////////////
          //Loop For Getting Piezo Values
          if (function == 1){//}
            for(i=0;i<4096;i++) {
            // Start I2C transmission
            Wire.beginTransmission(AddrP);  
            // Calling conversion result register, 0x00(0)
            Wire.write(0x00);
            // Stop I2C transmission
            Wire.endTransmission();

            // Request 2 bytes
            Wire.requestFrom(AddrP, 2);
    
            // Read 2 bytes of data, raw_adc msb, raw_adc lsb
            if(Wire.available() == 2)
            {  
              dataP[0] = Wire.read();
              dataP[1] = Wire.read();
            }
    
            // Convert the data to 12 bits
            raw_adc = ((dataP[0] * 256) + dataP[1]) & 0x0FFF;

            vibdat[i][0] = micros();
            vibdat[i][1] = raw_adc;

            //12-bit Resolution output every 1 second
            if ((millis() - startTime > 1000)){
              Serial.printf("\n%i,%i\n",millis(), raw_adc);

              display.clearDisplay();      //clears the display 
              display.setTextColor(WHITE);
              display.setCursor(0,0);             // Start at top-left corner
              display.printf("Hold Down Button to Return to Menu");   //Prints values to OLED
              display.printf("\n%i,%i\n", startTime, raw_adc);   //Prints values to OLED
              display.display();

              startTime = millis();
            }
          }
          }
        }

        //for(j=0;j<4096;j++) {
        //  Serial.print(vibdat[j][0]);
        //  Serial.print(",");
        //  Serial.println(vibdat[j][1]);
        //  Serial.print(".");
        //}



        if(function == -1){ 
          Serial.printf("SINGLE LONG click \n");  //for testing
          function = 0;
        } 
      break;
      case 2:
        display.clearDisplay();      //clears the display 
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("MENU: \n Case0 \n Case1 \n->Case2  \n Case3 \n Case4 ");   //Outputs Switch Case
        display.display();
      break;
      //Start of case 3
      case 3:
        display.clearDisplay();             //clears the display
        display.setTextColor(WHITE); //sets the display color to white
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("MENU: \n Case0 \n Case1 \n Case2 \n ->Case3 \n Case4 \n ");   //Outputs Switch Case
        display.display();   //shows the display
      break;
      //Start of Case 4
      case 4:
        display.clearDisplay();             //clears the display
        display.setTextColor(WHITE);
        display.setCursor(0,0);             // Start at top-left corner
        display.printf("MENU: \n Case0 \n Case1 \n Case2 \n Case3 \n ->Case4  \n ");   //Outputs Switch Case
        display.display();
      break;
      default:
     
        //if(function == 1){ 
        //  Serial.printf("SINGLE click \n"); //for testing
        //}
        //display.clearDisplay();             //clears the display
        //display.setTextColor(WHITE);
        //display.setCursor(0,0);             // Start at top-left corner
        //display.printf("Error");            //Outputs Switch Case
        //display.display();
  
        //if(function == -1){ 
        //  Serial.printf("SINGLE LONG click \n");  //for testing
        //} 

      break;
      oldPosition = encoderMap;
    }
  }
}


///////////////////////////
//////////////////////////
//Void Functions
void rfidBegin(){
   uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata){
    Serial.printf("Didn't find PN53x board");
    while (1);
  }

  Serial.printf("\nFound chip PN5"); Serial.print((versiondata>>24) & 0xFF, HEX);
  Serial.printf("\nFirmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.printf("."); Serial.print((versiondata >>8) & 0xFF, DEC);

  nfc.SAMConfig();
  nfc.setPassiveActivationRetries(0xFF);
  Serial.printf("\nWaiting for Card \n");
}

//Function for Looping Rfid
void rfidCardRead(){
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; //Buffer to store the returned UID
  uint8_t uidLength;  //Length of the UID (4 or 7 bytes)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.printf("\nSuccess getting target ID \n");

    // Display some basic information about the card
    Serial.printf("Card Type ISO14443A card \n");
    Serial.printf("UID Value: ");
    nfc.PrintHex(uid, uidLength);


    Serial.printf("............. \n");

    if (uidLength == 4){
      Serial.printf("Mifare Classic Card \n");
      Serial.printf("Authenticating Card \n");
      uint8_t keya[6] = { 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF };
	  
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success){
        //Serial.printf("Sector 1 has been authenticated \n");
        uint8_t data[16];

        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if(success){
          Serial.printf("Reading... \n");
          nfc.PrintHexChar(data, 16);

          correctKey = isMatched(uid, masterKey);
          if (correctKey == 1) {
            Serial.printf("Valid Access Card \n"); 
          }
          else{
            Serial.printf("Invalid Access Card \n"); 
          }
        }
        else{
          Serial.printf("Card Reading Error \n");
        } 
      }
    }
  }
  Serial.printf("#####BREAK######");
}

//Function for Validating master card 
bool isMatched (uint8_t uid[4], uint8_t masterKey[4]) {
  int i;
  for(i=0; i < 4; i++){
    if(uid[i] != masterKey[i]){
      Serial.printf("*Invalid Key \n");
      Serial.printf("Master key is {0x49, 0xB8, 0x2D, 0x7A} \n");
      Serial.printf("Your UID Value: %i \n", uid);

   

      display.clearDisplay();             
      display.setTextColor(WHITE);
      display.setCursor(0,0);           
      display.printf("Invalid Key - Access Denied"); //Outputs Switch Case Access
      display.display();
      //delay(1000);

      powerAccess = FALSE;
      

      Serial.printf("PowerAccess %i \n", powerAccess);
      return false;
    }
  }
  Serial.printf("*Valid Key \n");

  display.clearDisplay();            
  display.setTextColor(WHITE);
  display.setCursor(0,0);             
  display.printf("Valid Key - Access Granted");     //Output for Switch Case access
  display.display();
  //delay(1000);

  powerAccess = TRUE;

  Serial.printf("PowerAccess %i \n", powerAccess);
  return true;
}

//Function for sorting an array
//void sortArray(){
//  for (float a = 0; a < n; ++a){  //loops through values in an array
//    for (z = a + 1; z < n; ++z){
//      if (num[i] > num[j]){
//        b = num[i];
//        num[a] = num[j];
//        num[z] = a;
//      }
//    }
//  }
//}
