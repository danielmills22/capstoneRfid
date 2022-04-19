/*
 * Project Capstone_Project
 * Description: RFID Capstone Project
 * Author: Daniel Mills
 * Date:04-13-2022
 */

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
uint8_t masterKey2[4];
uint8_t uidArray[4];
bool correctKey;
bool powerAccess;

//Button Vars
const int BUTTONPIN1 = D5;
ClickButton button1(BUTTONPIN1, LOW, CLICKBTN_PULLUP);
int function = 0;


//Vars For Current and Vibration
// PECMAC125A I2C address is 0x2A(42) //Piezo is 0x50
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

//Vibration Bin Sorting Values
int x, y, c, d;
static int xx, yy, cc, dd;
    
//time vars
int startTime;
int last;
int lastTime2;

//Connecting to Adafruit Webservice
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

/**** Feeds *********/ 
// Setup Feeds to publish or subscribe 
Adafruit_MQTT_Publish mqttvib = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/vib");
Adafruit_MQTT_Publish mqttcurrent = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/current");
Adafruit_MQTT_Subscribe mqttObj2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/PowerState");  

void setup() {
  Serial.begin(115200);

  attachInterrupt(D5, getMode, RISING);
  
  //Setup For Rfid
  Serial.printf("Init Reader \n");
  nfc.begin();
  
  ////////////
  //Rfid
  rfidBegin();
  
  //Setting Time for button clicks -- measured in millis
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  //button1.debounceTime   = 20;   // Debounce timer in ms
  //button1.multiclickTime = 250;  // Time limit for multi clicks
  //button1.longClickTime  = 1000; // time until "held-down clicks" register


  //Getting Time Info -- This is for if info wanted to be recorded with the date
  Time.zone(-7);
  Particle.syncTime();

  //Connect to Wifi
  WiFi.connect();
  while(WiFi.connecting()){
    Serial.printf(".");
  }

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
}


void loop() {

  //*MQTT Start
  MQTT_connect();
  // Ping MQTT Broker every 2 minutes to keep connection alive
  if ((millis()-last)>120000) {               //function to ping the MQTT broker
      Serial.printf("Pinging MQTT \n");             
      if(! mqtt.ping()) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
    last = millis();
  }

  ////Current Reader
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
  }
  
  //pSensor = analogRead(PIEZOSENSORPIN);   //reads the vibration sensor values without the use of I2C
  //if((millis()-startTime) > 500) {   
  //   Serial.printf("StartTime %i | Vibs %.02f \n", startTime, pSensor);
  //   //Particle.publish("pSensor", String(pSensor));
  //  startTime = millis();   
  // } 
  
  rfidCardRead();

  //Listens for input from button
  //button1.Update();
  //if(button1.clicks != 0) {
  //  function = button1.clicks;
  //}

  ///////////////////////////////
  //Info will only show if Access is granted
  if (powerAccess == TRUE){

    //Encoder Info for Menu
    int newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      function = 0;
      oldPosition = newPosition;
      if (newPosition > 20){
        myEnc.write(20);
      }
      if (newPosition < 0){
        myEnc.write(0);
      }
    }

    //maps the encoder to the Switch Case
    encoderMap = map(newPosition, 0, 20, 0, 2);

     // enter switch case
    switch(encoderMap)
    {
      //Start
      case 0: 
        if (function == 0){
          display.clearDisplay();             //clears the display 
          display.setCursor(0,0);             // Start at top-left corner
          display.setTextColor(WHITE);
          display.printf("MENU: \n->Current Read  \nVib Read \nPublish Values \n");   //Outputs Switch Case
          display.display(); 
        }
        else{
          if(function == 1){ 
            display.clearDisplay();             //clears the display 
            display.setCursor(0,0);             // Start at top-left corner
            display.setTextColor(WHITE);
            display.printf("Channel: %i \n", j);
            display.printf("Current Value: %0.2f \n", current); 
            display.display();
          }
        }

        //if(function == 1){ 
        //  Serial.printf("SINGLE click \n"); //for testing
        //}
       
        // Output data to dashboard
        Serial.printf("Channel: %i \n", j);
        Serial.printf("Current Value: %0.2f \n", current); 

        //Second Click ends logging and returns to menu
        if(function == -1){ 
          Serial.printf("SINGLE LONG click \n");  //for testing
          function = 0;
          break;
        }
        Serial.printf("Function %i", function);
      break;
      //Start
      case 1: 
        Serial.printf("Function %i \n", function);

        if (function == 0){
          display.clearDisplay();      //clears the display
          display.setTextColor(WHITE);
          display.setCursor(0,0);             // Start at top-left corner
          display.printf("MENU: \nCurrent Read \n->Vib Read  \nPublish Values \n");   //Outputs Switch Case
          display.display();
        }
        else{
          piezoRead();  // function collects values from the piezo sensor
          if (function == 1){
            //12-bit Resolution output every 1 second
            if ((millis() - startTime > 1000)){
              Serial.printf("\n%i,%i\n",millis(), raw_adc);
              if(raw_adc >= 0 && raw_adc <= 25 ){
                xx = 0;
                xx = x++;
              }
              if(raw_adc >= 26 && raw_adc <= 50 ){
                yy = 0;
                yy = y++;
              }
              if(raw_adc >= 51 && raw_adc <=  75){
                cc = 0;
                cc = c++;
              }
              if(raw_adc >= 76){
                dd = 0;
                dd = d++;
              }

              display.clearDisplay();      //clears the display 
              display.setTextColor(WHITE);
              display.setCursor(0,0);             // Start at top-left corner
              display.printf("Vib Values: %i\n", raw_adc);   //Prints values to OLED
              display.printf("0-25:  %i\n", xx);   //Prints values to OLED
              display.printf("26-50: %i\n", yy);   //Prints values to OLED
              display.printf("51-75: %i\n", cc);   //Prints values to OLED
              display.printf("Vib > 76:%i\n", dd);   //Prints values to OLED
              display.display();
              startTime = millis();
            }
          }
        }

        if(function == -1){ 
          Serial.printf("SINGLE LONG click \n");  //for testing
          function = 0;
          break;
        } 
      break;
      case 2:
        Serial.printf("Function %i \n", function);

        if (function == 0){
          display.clearDisplay();      //clears the display 
          display.setTextColor(WHITE);
          display.setCursor(0,0);             // Start at top-left corner
          display.printf("MENU: \nCurrent Read \nVib Read \n->Publish Values  \n ");   //Outputs Switch Case
          display.display();
        }
        else{
          if (function == 1){  
            piezoRead();  // function collects values from the piezo sensor
            //*Reads and Publishes Values to Adafruit 
            if((millis()-lastTime2 > 2000)) {
              if(mqtt.Update()) {  //starts MQTT updats
                mqttcurrent.publish(current);                                         //publishes the current values  Adafruit
                Serial.printf("Publishing Current %0.2f \n", current);             //prints current values to serial monitor
                mqttvib.publish(raw_adc);                                          //pub the piezo values
                Serial.printf("Publishing Vib %i \n", raw_adc);              //prints piezo to serial monitor
                
                //Prints Values to the OLED
                display.clearDisplay();      //clears the display 
                display.setTextColor(WHITE);
                display.setCursor(0,0);             // Start at top-left corner
                display.printf("Publishing Current: \n"); 
                display.printf("%0.2f \n", current);  
                display.printf("Publishing Vib: \n"); 
                display.printf("%i \n", raw_adc); 
                display.display();
              }
              lastTime2 = millis();
            }
          }
        }

        if(function == -1){ 
          Serial.printf("SINGLE LONG click \n");  //for testing
          function = 0;
          break;
        } 
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
      function = 0;
      break;
      oldPosition = encoderMap;
    }
  }
}

   

///////////////////////////
//////////////////////////
//Void Functions
//Function Starts Rfid
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
          //correctKey = isMatched(uid, masterKey2);
          
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
      Serial.printf(" \n");
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

//Function for changing button state
void getMode(){
  function++;
  if(function == 2){
    function = -1;
  }
}


// Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {  //this function is important to include for connecting to MQTT
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
      Serial.printf("%s\n",(char *)mqtt.connectErrorString(ret));
      Serial.printf("Retrying MQTT connection in 5 seconds..\n");
      mqtt.disconnect();
      delay(5000);  // wait 5 seconds
  }
  Serial.printf("MQTT Connected!\n");   //output for if connection was successful
}

////////////////////////////
//Function For Getting Piezo Values
void piezoRead(){

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
    if(Wire.available() == 2){  
      dataP[0] = Wire.read();
      dataP[1] = Wire.read();
    }
    
    // Convert the data to 12 bits
    raw_adc = ((dataP[0] * 256) + dataP[1]) & 0x0FFF;

    vibdat[i][0] = micros();
    vibdat[i][1] = raw_adc;            
  }
}


