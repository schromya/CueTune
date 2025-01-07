/* Sources: 
  https://github.com/miguelbalboa/rfid/blob/master/examples/ReadAndWrite/ReadAndWrite.ino
  https://www.instructables.com/ESP32-With-RFID-Access-Control/
*/

#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16


//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup()  {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus

  
  // Init MFRC522
  mfrc522.PCD_Init(); 

  // Start Webserver
  server_setup();
}

void loop() {
  server_loop();

    


   //read_data();
    //write_data();

    

}

bool check_card() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return false;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return false;

    // Check for compatibility
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This only works with MIFARE Classic cards."));
        return false;
    }

    return true;
}

void exit_card() {
    //instructs the PICC when in the ACTIVE state to go to a "STOP" state
    mfrc522.PICC_HaltA(); 
    // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
    mfrc522.PCD_StopCrypto1();  
}


/*
 * Read Data from the RFID Card/Tag
 */
String read_data() {

  if (!check_card()) return "";

  //prints the technical details of the card/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  
  //prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  //buffer for read data
  byte buffer[SIZE_BUFFER] = {0};
 
  //the block to operate
  byte block = 1;
  byte size = SIZE_BUFFER; //authenticates the block to operate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    exit_card();
    return "";
  }

  //read data from block
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    exit_card();
    return "";
  }

  // Convert buffer to String
  String data_string = ""; // Initialize empty string
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++) {
      if (buffer[i] != 0) { // Exclude null or unused bytes
          data_string += (char)buffer[i]; // Append character to string
      }
  }

  // Print the converted string
  Serial.println(data_string);

  exit_card();
  return data_string;
}


/*
 * Write 'Hello' to the RFID Card/Tag
 */
void write_data(String str) {

  if (!check_card()) return;
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  

  //prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  
  byte block; //the block to operate

  byte buffer[MAX_SIZE_BLOCK] = "";
  byte data_size = str.length();

  for(byte i=0; i < data_size; i++) {
    buffer[i] = str[i];
  }
 
  //void positions that are left in the buffer will be filled with whitespace
  for(byte i=data_size; i < MAX_SIZE_BLOCK; i++) {
    buffer[i] = ' ';
  }
 
  block = 1; //the block to operate
  
  Serial.println(str);

   //authenticates the block to operate
   //Authenticate is a command to hability a secure communication
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    exit_card();
    return;
  }
  //else Serial.println(F("PCD_Authenticate() success: "));
 
  //Writes in the block
  status = mfrc522.MIFARE_Write(block, buffer, MAX_SIZE_BLOCK);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    exit_card();
    return;
  }
  else{
    Serial.println(F("MIFARE_Write() success!"));
  }
  exit_card();
}


/*
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

