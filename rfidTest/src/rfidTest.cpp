/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Daniel/Documents/IoT/capstoneRfid/rfidTest/src/rfidTest.ino"
/*
 * Project rfidTest
 * Description:
 * Author:
 * Date:
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

void setup(void);
void loop(void);
bool isMatched (uint8_t uid[4], uint8_t masterKey[4]);
#line 12 "c:/Users/Daniel/Documents/IoT/capstoneRfid/rfidTest/src/rfidTest.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

const int PN532_IRQ  = A0;
const int PN532_RESET = A1;  

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

uint8_t valid;

//Validation System
uint8_t masterKey[4] = {0x13, 0x99, 0xC9, 0x1E};
uint8_t uidArray[4];
bool correctKey;


void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.printf("Init Reader \n");

  nfc.begin();

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

void loop(void) {
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; //Buffer to store the returned UID
  uint8_t uidLength;  //Length of the UID (4 or 7 bytes)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.printf("\nSuccess \n");

    // Display some basic information about the card
    Serial.printf("Card Type ISO14443A card \n");
    Serial.printf("UID Length: ");Serial.print(uidLength, DEC);Serial.printf(" bytes \n");
    Serial.printf("UID Value: ");
    nfc.PrintHex(uid, uidLength);


    Serial.printf("..... \n");

    if (uidLength == 4){
      Serial.printf("Mifare Classic Card \n");
      Serial.printf("Authenticating Card \n");
      uint8_t keya[6] = { 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF };
	  
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success){
        Serial.printf("Sector 1 has been authenticated \n");
        uint8_t data[16];

        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if(success){
          Serial.printf("Reading... \n");
          nfc.PrintHexChar(data, 16);

          correctKey = isMatched(uid, masterKey);
          if (correctKey) {
            printf("Valid Access Card \n"); 
          }
          else{
            printf("Invalid Access Card \n"); 
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

//void function
bool isMatched (uint8_t uid[4], uint8_t masterKey[4]) {
  int i;
  for(i=0; i < 4; i++){
    if(uid[i] != masterKey[i]){
      Serial.printf("Invalid Key \n");
      Serial.printf("Master key is {0x49, 0xB8, 0x2D, 0x7A}");
      return false;
    }
  }
  Serial.printf("Valid Key \n");
  return true;
}