#include <Adafruit_PCF8574.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define SCK_RFID 13
#define MISO_RFID 12
#define MOSI_RFID 11
#define SDA_RFID 10
#define RST_RFID 9

#define Door_Switch2 8
#define Door_Switch1 7
#define PWM_Servo2 6
#define PWM_Servo1 5

#define LED_STAT 2 //NOT FINAL!!!

SoftwareSerial nextion (3,4); // RX,TX
Adafruit_PCF8574 PCF1, PCF2, PCF3, PCF4, PCF5;
MFRC522 mfrc522(SDA_RFID, RST_RFID);

byte depressedButtons[5];
int btIDs[5][5] = {
    {0,5,10,15,20},
    {1,6,11,16,21},
    {2,7,12,17,22},
    {3,8,13,18,23},
    {4,9,14,19,24}
};

//  User Data
String receivedText;
String names[5][5] = {
    {"Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann"},
    {"Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann"},
    {"Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann"},
    {"Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann"},
    {"Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann","Max\\rMustermann"}};

void setup(){
    Serial.begin(9600);
    nextion.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();

    pinMode(Door_Switch1, INPUT_PULLUP);
    pinMode(Door_Switch2, INPUT_PULLUP);

    pinMode(PWM_Servo1, OUTPUT);
    pinMode(PWM_Servo2, OUTPUT);
    
    pinMode(LED_STAT, OUTPUT);

    PCFsetup(PCF1, 0x20);
    PCFsetup(PCF2, 0x21);
    PCFsetup(PCF3, 0x3A);
    PCFsetup(PCF4, 0x3B);
    PCFsetup(PCF5, 0x3C);

    PCFread(PCF1, 0);
    PCFread(PCF2, 1);
    PCFread(PCF3, 2);
    PCFread(PCF4, 3);
    PCFread(PCF5, 4);
    
    sendCommand("page 1");
    writeNames();
}

void loop(){
    checkChanges(PCF1, 0);
    checkChanges(PCF2, 1);
    checkChanges(PCF3, 2);
    checkChanges(PCF4, 3);
    checkChanges(PCF5, 4);
    readNames();
}

//Nextion Functions
void sendCommand(String cmd) {
    nextion.print(cmd);
    nextion.write(0xFF);
    nextion.write(0xFF);
    nextion.write(0xFF);
}

void writeNames(){
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            sendCommand("bt" + String(btIDs[j][i]) + ".txt=" + "\"" + names[j][i] + "\"");
        }
    }
}

void readNames(){
    receivedText = nextion.readStringUntil('!');
    if (!(receivedText == ""))
    {   
        if (receivedText == "begin")
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    receivedText = nextion.readStringUntil('!');
                    Serial.println(receivedText);
                    names[j][i] = receivedText;
                }
            }
            writeNames();
        }
    }
}

//PCF Functions
void PCFsetup(Adafruit_PCF8574 &PCF, int adress){
    if (!PCF.begin(adress, &Wire))
        {   
            Serial.println("Couldn't find PCF8574" + adress);
        }
    for (size_t i = 0; i < 5; i++)
        {
            PCF.pinMode(i, INPUT_PULLUP);
        }
}

void PCFread(Adafruit_PCF8574 &PCF, int spalte){
    for (int i = 0; i < 8; i++)
    {
        bitWrite(depressedButtons[spalte], i, bitRead(bit(PCF.digitalRead(i)), 1));// Convert Bool to bit
    }
}

void checkChanges(Adafruit_PCF8574 &PCF, int spalte){
    for (int i = 0; i < 5; i++)
    {
        if (PCF.digitalRead(i) == LOW && bitRead(depressedButtons[spalte],i) == 1)
        {
            bitWrite(depressedButtons[spalte], i, 0);
            sendCommand("bt" + String(btIDs[spalte][i]) + ".val=1");
        }
        else if (PCF.digitalRead(i) == HIGH && bitRead(depressedButtons[spalte],i) == 0)
        {
            bitWrite(depressedButtons[spalte], i, 1);
            sendCommand("bt" + String(btIDs[spalte][i]) + ".val=0");
        }
    }
    
}

// RFID Functions
void readRFID() {
    if (!mfrc522.PICC_IsNewCardPresent()) // Wenn eine Karte in Reichweite //ist...
    {
        return; // gehe weiter...
    }
    if (!mfrc522.PICC_ReadCardSerial()) // Wenn ein RFID-Sender ausgewählt wurde
    {
        return; // gehe weiter...
    }
    Serial.print("Die ID des RFID-TAGS lautet:");
    //"Die ID des RFID-TAGS lautet:" wird auf den Serial Monitor geschrieben
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        // Dann wird die UID ausgelesen,die aus vier einzelnen Blöcken besteht und der Reihe nach an den Serial Monitor gesendet. Die Endung Hex bedeutet, dass die vier Blöcke der UID als HEX-Zahl (also auch mit Buchstaben) ausgegeben wird
        Serial.print(" ");
        // Der Befehl „Serial.print(" ");“ sorgt dafür, dass zwischen den einzelnen ausgelesenen Blöcken ein Leerzeichen steht.
    }
    Serial.println();
    // Mit dieser Zeile wird auf dem Serial Monitor nur ein Zeilenumbruch gemacht.
}