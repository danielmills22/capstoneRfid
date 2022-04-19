/*
 * Project ADC121
 * Description: ACD121 Piezo Vibraiton Sensor
 * Author: Brian Rashap
 * Date:    22-APR-2020
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

// I2C address of the ADC121C021 0x50 
#define Addr 0x2A
#define AddrP 0x50

long vibdat[4096][2];
int i;
int j;
byte data[2];

int raw_adc = 0;

//Graphics OLED Vars
#include "math.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

const int yMax = 100;
const int yMin = 0;

const float freq = .1;
const int amp = 40;
const int offset = 50;
const unsigned int plotPeriod = 2; //x-axis steps in ms
unsigned int currentTime;
unsigned int lastTime;


void setup()
{ 
    // Set variable
    Particle.variable("i2cdevice", "ADC121C021");
    Particle.variable("rawADC", raw_adc);
    
    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise serial communication, set baud rate = 9600
    Serial.begin(9600);
    delay(300);
    Serial.println("Serial Monitor Started");

    //OLED Setup
    Serial.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    display.setRotation(2);
    display.display(); // show splashscreen
    delay(2000);
    display.clearDisplay();   // clears the screen and buffer
}

void loop() 
{
  delay(5000);
  Serial.println("Starting Data Collection");
  delay(100);
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
    delay(9);
    
}
for(j=0;j<4096;j++) {
    Serial.print(vibdat[j][0]);
    Serial.print(",");
    Serial.println(vibdat[j][1]); 
    }

  currentTime = millis();
  if((currentTime-lastTime)>plotPeriod) {
    oledPlot(raw_adc);
    lastTime = currentTime;
    Serial.printf("Time = %f, Data = %i \n",currentTime/1000.0,data);
  }
}

void oledPlot(int y) {
  static int x=0;

  y = mapFloat(y,yMin,yMax,63,0);
  display.drawPixel(x, y, WHITE);
  display.display();
  x++;
  if(x>127) {
    x=0;
    display.clearDisplay();
  }
}

int mapFloat(float fromY, float fromMin, float fromMax, float toMin, float toMax) {
  return (fromY - fromMin) * (toMax - toMin) / (fromMax - fromMin) + fromMin;
}