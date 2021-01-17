/**
 * -----------------------------------------------------------------------------------------
 * This uses MFRC522 library (https://github.com/miguelbalboa/rfid)
 * Example to ACCESS CONTROL by Everton P. Cruz (www.everton.mat.br)
 * 
 * Typical pin layout used to module RFID:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * VCC         3.3 V        3.3 V         3.3 V     3.3 V
 * GND         GND          GND           GND       GND        GND              GND
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Pin used to module Relay:
 * -----------------------------------------------------------------------------------------
 * Signal     Relay Pin     Arduino
 * VCC        5 V           5 V
 * GND        GND           GND
 * SIGNAL     S             4
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN          9           // Configurable, see typical pin layout above
#define SS_PIN           10          // Configurable, see typical pin layout above
#define SIGNAL_RELAY     7           // pin control relay
#define RED_LED          6           // pin RED led
#define GREEN_LED        5           // pin GREEN led
#define BLUE_LED         3           // pin BLUE led

#define DELAY_RELAY      2000        // delay in miliseconds
#define DELAY_STATE      500         // delay in miliseconds

const byte OK[3]     = { 0, 10, 0};
const byte NOT_OK[3] = {10,  0, 0};

unsigned int quant_tags       = 4;
unsigned int size_uid         = 4;
unsigned int uid_access[4][4] = {
                                  {0x79, 0xB8, 0xFB, 0xA2},
                                  {0xD9, 0x67, 0xFD, 0xA2},
                                  {0x32, 0x91, 0xFF, 0xCC},
                                  {0x65, 0xF9, 0xB7, 0xFA}
                                 };

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// * Helper routine to dump a byte array as hex values to Serial * //
void dump_byte_array( byte *buffer, byte bufferSize ){
  for( byte i = 0; i < bufferSize; i++ ){
    Serial.print( buffer[i] < 0x10 ? " 0" : " " );
    Serial.print( buffer[i], HEX );
  }
}

bool compare( byte *buffer, byte bufferSize ){
  bool state = false;
  
  if( bufferSize == size_uid ){
    for( byte i = 0; i< quant_tags; i++ ){
      state = true;
      for( byte j = 0; j < bufferSize; j++ ){
        if( uid_access[i][j] != buffer[j] ){
          state = state && false;
          break;
        }
      }
      if( state ){
        return true;
      }
    }
  }
  
  return false;
}

void setup() {
  Serial.begin( 9600 ); // Initialize serial communications with the PC
  while( !Serial );     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522 card

  pinMode( SIGNAL_RELAY, OUTPUT );
  pinMode( RED_LED, OUTPUT );
  pinMode( GREEN_LED, OUTPUT );
  pinMode( BLUE_LED, OUTPUT );
  
  digitalWrite( SIGNAL_RELAY, LOW );
  digitalWrite( RED_LED, LOW );
  digitalWrite( GREEN_LED, LOW );
  digitalWrite( BLUE_LED, LOW );

  Serial.println( F("Scan tag UID to access control." ) );
  Serial.println();
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if( ! mfrc522.PICC_IsNewCardPresent() )
    return;

  // Select one of the cards
  if( ! mfrc522.PICC_ReadCardSerial() )
    return;

  // Show some details of the PICC (that is: the tag/card)
  Serial.print( F("Card UID:") );
  dump_byte_array( mfrc522.uid.uidByte, mfrc522.uid.size );
  Serial.println();
  Serial.print( F("PICC type: ") );
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType( mfrc522.uid.sak );
  Serial.println( mfrc522.PICC_GetTypeName(piccType) );

  // Check for compatibility
  if(     piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
      &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println( F("This sample only works with MIFARE Classic cards.") );
    return;
  }

  if( compare( mfrc522.uid.uidByte, mfrc522.uid.size ) ){
    digitalWrite( SIGNAL_RELAY, HIGH );

    Serial.println( F("Access with success.") );
    
    analogWrite( RED_LED,   OK[0] );
    analogWrite( GREEN_LED, OK[1] );
    analogWrite( BLUE_LED,  OK[2] );
    
    delay( DELAY_RELAY );

    digitalWrite( RED_LED,   LOW );
    digitalWrite( GREEN_LED, LOW );
    digitalWrite( BLUE_LED,  LOW );
    
    digitalWrite( SIGNAL_RELAY, LOW );
  }
  else{
    Serial.println( F("Error with access.") );
    analogWrite( RED_LED,   NOT_OK[0] );
    analogWrite( GREEN_LED, NOT_OK[1] );
    analogWrite( BLUE_LED,  NOT_OK[2] );

    delay( DELAY_STATE );

    digitalWrite( RED_LED,   LOW );
    digitalWrite( GREEN_LED, LOW );
    digitalWrite( BLUE_LED,  LOW );
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  //mfrc522.PCD_StopCrypto1();

  Serial.println();
}
