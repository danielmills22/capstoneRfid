/*
 * Project CurrentMonitor_Basic
 * Description:
 * Author:
 * Date:
 */

/*
 * Project PECMAC125A
 * Description: NCD Current Monitor
 * Author: Brian Rashap
 * Date:  22-APR-2020
 */

// PECMAC125A I2C address is 0x2A(42)
SYSTEM_MODE(SEMI_AUTOMATIC);
#define Addr 0x2A

byte data[36];
int typeOfSensor = 0;
int maxCurrent = 0;
int noOfChannel = 0;
float current = 0.0;
void setup()

{
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
  Serial.printf("Type of Sensor %i \n",typeOfSensor);
  Serial.printf("Max Current: %i \n", maxCurrent);
  Serial.printf("No. of Channels: %i 'n", noOfChannel);
  delay(5000);
}

void loop()
{
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
    //Serial.printf("Channel: %i \n", j);
    Serial.printf("%0.2f \n", current); 
  }
}