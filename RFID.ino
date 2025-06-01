/* Sources: 
  https://github.com/miguelbalboa/rfid/blob/master/examples/ReadAndWrite/ReadAndWrite.ino
  https://www.instructables.com/ESP32-With-RFID-Access-Control/
*/

#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

const byte RFID_DATA_BLOCK = 4;


//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);


// Strings to write and read
String data_string_read = "";
String data_string_write = "";


/*
  PUBLIC FUNCTION. Should be called in the primary arduino setup.
*/
void rfid_setup()  {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  
  // Init MFRC522
  mfrc522.PCD_Init(); 

}


/*
  PUBLIC FUNCTION. Should be called in the primary arduino loop.
*/
void rfid_loop()  {
  if (check_card()) {
    write_data();
    read_data();
    exit();
  }

}

/*
  PUBLIC FUNCTION. Returns the last string that was read when an RFID card was registered/tapped.
*/
String rfid_get_last_read() {
  return data_string_read;
}


/*
PUBLIC FUNCTION. The string passed to this will be written to the RFID card the next time
it is registered/tapped.
*/
void rfid_set_next_write(String data_string) {
  data_string_write = data_string;
}


/*
 * Checks if card is valid to "connect" to.
 * Returns true if card is valid, else false.
 */
bool check_card() {

    // Exit if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent()) return false;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) return false;


    // Check for compatibility
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This only works with MIFARE Classic cards."));
        return false;
    }

    speaker_beep();

    return true;
}


/*
 * Read Data from the RFID Card/Tag. If no data or cannot read card,
 * returns empty string.
 */
bool read_data() {

  if (! authenticate()) return false;

  //buffer for read data
  byte buffer[SIZE_BUFFER] = {0};
  byte size = SIZE_BUFFER;

  //read data from block
  status = mfrc522.MIFARE_Read(RFID_DATA_BLOCK, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Reading failed: " + String(mfrc522.GetStatusCodeName(status)));
    return false;
  }

  // Convert buffer to String
  String data_string = ""; // Initialize empty string
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++) {
      if (buffer[i] != 0) { // Exclude null or unused bytes
          data_string += (char)buffer[i]; // Append character to string
      }
  }

  data_string_read = data_string; // Copy so string isn't modified while reading
  Serial.println("Read RFID: " + data_string_read);
  return true;
}


/*
 * Write inputed string to the RFID Card/Tag.
 * Returns true if write was successful, else false.
 */
bool write_data() {

  if (data_string_write == "") return false;
  String data_string = data_string_write;  // Copy so string isn't modified while writing
  
  if (! authenticate()) return false;
  
  byte buffer[MAX_SIZE_BLOCK] = "";
  byte data_size = data_string.length();

  for(byte i=0; i < data_size; i++) {
    buffer[i] = data_string[i];
  }
 
  //void positions that are left in the buffer will be filled with whitespace
  for(byte i=data_size; i < MAX_SIZE_BLOCK; i++) {
    buffer[i] = ' ';
  }

  //Writes in the block
  status = mfrc522.MIFARE_Write(RFID_DATA_BLOCK, buffer, MAX_SIZE_BLOCK);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("MIFARE_Write() failed: " + String(mfrc522.GetStatusCodeName(status)));
    return false;
  }

  return true;
}


/*
 * Authenticates tag.
 * Returns true if authentication was successful, else false.
*/
bool authenticate() {
  //prepare the key - all keys are set to FFFFFFFFFFFF
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Authenticates the block to operate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
                                    RFID_DATA_BLOCK, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.println("PCD_Authenticate() failed: " + String(mfrc522.GetStatusCodeName(status)));
    return false;
  }

  return true;
}


/*
 * Closes connection to RFID card.
 */
void exit() {
    //instructs the PICC when in the ACTIVE state to go to a "STOP" state
    mfrc522.PICC_HaltA(); 
    // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
    mfrc522.PCD_StopCrypto1();  
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

