#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define LOCK_LED 6
#define UNLOCK_LED 7

enum StateLock
{
  LOCK,
  UNLOCK
};

MFRC522 nfc_reader(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

unsigned char block1 = 1;
unsigned char block2 = 2;
unsigned char len = 30;
unsigned char buff[30];

bool mode = true;

String command = "";

void setLockState(StateLock state)
{
  if(state == StateLock::LOCK)
  {
    digitalWrite(LOCK_LED,HIGH); 
    digitalWrite(UNLOCK_LED, LOW);
  }
  else
  {
    digitalWrite(LOCK_LED, LOW); 
    digitalWrite(UNLOCK_LED, HIGH);

    delay(5000);

    setLockState(StateLock::LOCK);
  }
}

void setup() 
{
  pinMode(LOCK_LED, OUTPUT);
  pinMode(UNLOCK_LED, OUTPUT);

  Serial.begin(1200);

  SPI.begin();
  nfc_reader.PCD_Init();

  for(unsigned char counter = 0; counter < 6; ++counter){ key.keyByte[counter] = 0xFF; }

  Serial.println("System check point has been started!");

  setLockState(StateLock::LOCK);
}

void loop() 
{ 

  if(Serial.available() > 0)
  {
    command = Serial.readString();
    
     if(command.indexOf("READ") >= 0)
     {
        mode = true;
        Serial.println("Mode has been changed successful. Mode: READ");
     }

     if(command.indexOf("WRITE") >= 0)
     {
        mode = false;
        Serial.println("Mode has been changed successful. Mode: WRITE");
     }

     if(command.indexOf("OPENDOOR") >= 0)
     {
        setLockState(StateLock::UNLOCK);
        Serial.println("Door has been opened successful.");
     }

     if(command.indexOf("CLOSEDOOR") >= 0)
     {
        setLockState(StateLock::LOCK);
        Serial.println("Door has been closed successful.");
     }
  }
  
  if(!nfc_reader.PICC_IsNewCardPresent()){ return; }

  if(!nfc_reader.PICC_ReadCardSerial()){ return; }

  if(mode)
  { 
    Serial.setTimeout(1000);
    
    printUid();
    printSak();
    printType();
    printGuestName();
    printGuestType();

    //nfc_reader.PICC_DumpDetailsToSerial(&(nfc_reader.uid));
    haltReadOrWrite(); 
  }
  else
  {
    memset(buff, 0, 30);
    
    Serial.setTimeout(20000L);

    Serial.println("Enter the name: ");

    Serial.readBytes(buff, 18);

    status = nfc_reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block1, &key, &(nfc_reader.uid));

    if(status != MFRC522::STATUS_OK)
    {
      Serial.print("Auth fail: ");
      Serial.println(nfc_reader.GetStatusCodeName(status));
      memset(buff, 0, 30);
      return;
    }

    status = nfc_reader.MIFARE_Write(block1, buff, 16);

    if(status != MFRC522::STATUS_OK)
    {
      Serial.print("Write failed: ");
      Serial.println(nfc_reader.GetStatusCodeName(status));
      memset(buff, 0, 30);
      return;
    }

    Serial.println("Name writing successful");

    memset(buff, 0, 30);

    Serial.println("Type the name: ");

    Serial.readBytes(buff, 18);

    status = nfc_reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block2, &key, &(nfc_reader.uid));

    if(status != MFRC522::STATUS_OK)
    {
      Serial.print("Auth fail: ");
      Serial.println(nfc_reader.GetStatusCodeName(status));
      memset(buff, 0, 30);
      return;
    }

    status = nfc_reader.MIFARE_Write(block2, buff, 16);

    if(status != MFRC522::STATUS_OK)
    {
      Serial.print("Write failed: ");
      Serial.println(nfc_reader.GetStatusCodeName(status));
      memset(buff, 0, 30);
      return;
    }

    Serial.println("Type writing successful");
        
    haltReadOrWrite();
  }
}

void printDecUid(unsigned char *buff, unsigned char bufferSize)
{
  for(unsigned char counter = 0; counter < bufferSize; ++counter)
  {
    Serial.print((buff[counter] < 0x10)? " 0" : " "); 
    Serial.print(buff[counter], DEC);
  }
}

void printUid()
{
  Serial.print("UID: ");
  printDecUid(nfc_reader.uid.uidByte, nfc_reader.uid.size);
  Serial.println();  
}

void printSak()
{
  Serial.print("SAK: 0");
  Serial.print(nfc_reader.uid.sak);
  Serial.println();
}

void printType()
{
  Serial.print("TYPE: ");
  Serial.print(nfc_reader.PICC_GetTypeName(nfc_reader.PICC_GetType(nfc_reader.uid.sak)));
  Serial.println(); 
}

void printGuestName()
{
  Serial.print("NAME: ");

  status = nfc_reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(nfc_reader.uid));

  if(status != MFRC522::STATUS_OK)
  {
    Serial.print("Auth failed. Status:  ");
    Serial.println(nfc_reader.GetStatusCodeName(status));
    memset(buff, 0, 30);
    return;
  }

  memset(buff, 0, 30);

  status = nfc_reader.MIFARE_Read(block1, buff, &len);

  if(status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed. Status:  ");
    Serial.println(nfc_reader.GetStatusCodeName(status));
    memset(buff, 0, 30);
    return;
  }

  for(unsigned char counter = 0; counter < 16; ++counter)
  {
    if(buff[counter] != 32)
    {
        Serial.write(buff[counter]);
    }
  }

  Serial.println();
}

void printGuestType()
{
  Serial.print("GUEST TYPE: ");

  status = nfc_reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(nfc_reader.uid));

  if(status != MFRC522::STATUS_OK)
  {
    Serial.print("Auth failed. Status:  ");
    Serial.println(nfc_reader.GetStatusCodeName(status));
    memset(buff, 0, 30);
    return;
  }

  memset(buff, 0, 30);

  status = nfc_reader.MIFARE_Read(block2, buff, &len);

  if(status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed. Status:  ");
    Serial.println(nfc_reader.GetStatusCodeName(status));
    memset(buff, 0, 30);
    return;
  }

  for(unsigned char counter = 0; counter < 16; ++counter)
  {
    if(buff[counter] != 32)
    {
        Serial.write(buff[counter]);
    }
  }

  Serial.println();
}

void haltReadOrWrite()
{
  nfc_reader.PICC_HaltA();
  nfc_reader.PCD_StopCrypto1();
}
