/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/CurrentMonitor/src/CurrentMonitor.ino"
/*
 * Project PECMAC125A
 * Description: NCD Current Monitor
 * Author: Brian Rashap
 * Date:  22-APR-2020
 */

// PECMAC125A I2C address is 0x2A(42)
void setup();
void loop();
#line 9 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/CurrentMonitor/src/CurrentMonitor.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);
#define Addr 0x2A
#define AddrP 0x50

//IFTTT
float pSensor;

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

void setup(){
  //PValues Vars
  Particle.variable("i2cdevice", "ADC121C021");
  Particle.variable("rawADC", raw_adc);

  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  Wire.beginTransmission(AddrP);
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
  Serial.printf("Type of Sensor %i \n",typeOfSensor);
  Serial.printf("Max Current: %i \n", maxCurrent);
  //Serial.printf("No. of Channels: %i 'n", noOfChannel);
  //delay(5000);
}

void loop()
{
  ////////////////////////////////////////
  //PValues Loop
  delay(100);
  for(i=0;i<4096;i++) {
    // Start I2C transmission
    //Wire.beginTransmission(AddrP);  
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
    //delay(9);
    
}
//for(j=0;j<4096;j++) {
    //Serial.print(vibdat[j][0]);
    //Serial.print(",");
    //Serial.println(vibdat[j][1]);
    //Serial.print(".");
//    }

//delayMicroseconds(500);


  /////////////////////////////////////////
  //Current Loop

  for (int j = 1; j < noOfChannel + 1; j++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Command header byte-1
    Wire.write(0x92);
    // Command header byte-2
    Wire.write(0x6A);
    // Command 1
    Wire.write(0x01);
    // Start Channel No.
    Wire.write(j);
    // End Channel No.
    Wire.write(j);
    // Reserved
    Wire.write(0x00);
    // Reserved
    Wire.write(0x00);
    // CheckSum
    Wire.write((0x92 + 0x6A + 0x01 + j + j + 0x00 + 0x00) & 0xFF);
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(500);

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
    pSensor = current;
    //Particle.publish("pSensor", String(pSensor));
  }
}