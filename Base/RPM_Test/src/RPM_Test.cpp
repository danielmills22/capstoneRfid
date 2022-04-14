/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/RPM_Test/src/RPM_Test.ino"
/*
 * Project ADC121
 * Description: ACD121 Piezo Vibraiton Sensor
 * Author: Brian Rashap
 * Date:    22-APR-2020
 */

// I2C address of the ADC121C021 0x50 
void setup();
void loop();
#line 9 "c:/Users/Daniel/Documents/IoT/capstoneRfid/Base/RPM_Test/src/RPM_Test.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);
#define Addr 0x50

long vibdat[4096][2];
int i;
int j;
byte data[2];

int raw_adc = 0;
void setup()
{ 
    // Set variable
    //Particle.variable("i2cdevice", "ADC121C021");
    //Particle.variable("rawADC", raw_adc);
    
    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise serial communication, set baud rate = 9600
    Serial.begin(9600);
    //delay(300);
    //Serial.println("Serial Monitor Started");
}

void loop() 
{
//delay(5000);
//Serial.println("Starting Data Collection");
//delay(100);
for(i=0;i<4096;i++) {
    // Start I2C transmission
    Wire.beginTransmission(Addr); 
    // Calling conversion result register, 0x00(0)
    Wire.write(0x00);
    // Stop I2C transmission
    Wire.endTransmission();

    // Request 2 bytes
    Wire.requestFrom(Addr, 2);
    
    // Read 2 bytes of data, raw_adc msb, raw_adc lsb
    if(Wire.available() == 2)
    {  
        data[0] = Wire.read();
        data[1] = Wire.read();
    }
    
    // Convert the data to 12 bits
    raw_adc = ((data[0] * 256) + data[1]) & 0x0FFF;

    vibdat[i][0] = micros();
    vibdat[i][1] = raw_adc;
    //delay(9);
    
}
//for(j=0;j<4096;j++) {
    Serial.print(vibdat[j][0]);
    Serial.print(",");
    Serial.println(vibdat[j][1]);
//    }

//delayMicroseconds(500);
}