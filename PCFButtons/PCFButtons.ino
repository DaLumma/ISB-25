#include <Adafruit_PCF8574.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Servo.h>

#define SCK_RFID 13
#define MISO_RFID 12
#define MOSI_RFID 11
#define SDA_RFID 10
#define RST_RFID 9

#define Door_Switch2 8
#define Door_Switch1 7
#define PWM_Servo2 6
#define PWM_Servo1 5

#define LED_STAT 14

SoftwareSerial nextion(3, 4); // RX,TX
Adafruit_PCF8574 PCF1, PCF2, PCF3, PCF4, PCF5;

MFRC522 mfrc522(SDA_RFID, RST_RFID);
String rfidKey = "11 99 4C 26";
boolean lidLocked = false;
Servo Servo1, Servo2;
int lockedDeg = 0;
int unlockedDeg = 180;

byte depressedButtons[5];
int btIDs[5][5] = {
    {0, 5, 10, 15, 20},
    {1, 6, 11, 16, 21},
    {2, 7, 12, 17, 22},
    {3, 8, 13, 18, 23},
    {4, 9, 14, 19, 24}};

//  User Data
String receivedText;
String names[5][5];

void setup()
{
    Serial.begin(9600);
    nextion.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();

    Servo1.attach(PWM_Servo1);
    Servo2.attach(PWM_Servo2);
    Servo1.write(unlockedDeg);
    Servo2.write(unlockedDeg);

    pinMode(Door_Switch1, INPUT_PULLUP);
    pinMode(Door_Switch2, INPUT_PULLUP);

    pinMode(LED_STAT, OUTPUT);

    sendCommand("page 0");
    PCFsetup(PCF1, 0x23);
    PCFsetup(PCF2, 0x24);
    PCFsetup(PCF3, 0x25);
    PCFsetup(PCF4, 0x26);
    PCFsetup(PCF5, 0x27);

    PCFread(PCF1, 0);
    PCFread(PCF2, 1);
    PCFread(PCF3, 2);
    PCFread(PCF4, 3);
    PCFread(PCF5, 4);

    loadNames();
    writeNames();
    sendCommand("page 1");
    digitalWrite(LED_STAT, HIGH);
}

void loop()
{
    checkChanges(PCF1, 0);
    checkChanges(PCF2, 1);
    checkChanges(PCF3, 2);
    checkChanges(PCF4, 3);
    checkChanges(PCF5, 4);
    readNames();
    lockLid();
}

// Nextion Functions
void sendCommand(String cmd)
{
    nextion.print(cmd);
    nextion.write(0xFF);
    nextion.write(0xFF);
    nextion.write(0xFF);
}

void writeNames()
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            sendCommand("Button_page.bt" + String(btIDs[j][i]) + ".txt=" + "\"" + names[j][i] + "\"");
            sendCommand("Text_page.t" + String(btIDs[j][i]) + ".txt=" + "\"" + names[j][i] + "\"");
        }
    }
    loadEmptySlots();
}

void readNames()
{
    receivedText = nextion.readStringUntil('!');
    if (receivedText != "")
    {
        if (receivedText == "begin")
        {
            digitalWrite(LED_STAT, LOW);
            Serial.println(nextion.available());
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    receivedText = nextion.readStringUntil('!');
                    Serial.println(receivedText);
                    names[j][i] = receivedText;
                }
            }
            saveNames();
            writeNames();
            sendCommand("page 1");
            digitalWrite(LED_STAT, HIGH);
        }
    }
}

void readButtonOverrides()
{
    if (nextion.available() > 1)
    {
        String readData = nextion.readString();
        Serial.println("Button Press: " + readData);
    }
}

// PCF Functions
void PCFsetup(Adafruit_PCF8574 &PCF, int adress)
{
    if (!PCF.begin(adress, &Wire))
    {
        Serial.println("Couldn't find PCF8574" + adress);
    }
    for (size_t i = 0; i < 5; i++)
    {
        PCF.pinMode(i, INPUT_PULLUP);
    }
    PCF.pinMode(7, OUTPUT);
    PCF.digitalWrite(7, HIGH);
}

void PCFread(Adafruit_PCF8574 &PCF, int spalte)
{
    for (int i = 0; i < 5; i++)
    {
        bitWrite(depressedButtons[spalte], i, bitRead(bit(PCF.digitalRead(i)), 1)); // Convert Bool to bit
        if (!PCF.digitalRead(i))
        {
            sendCommand("Button_page.bt" + String(btIDs[i][spalte]) + ".val=1");
        }
    }
}

void checkChanges(Adafruit_PCF8574 &PCF, int spalte)
{
    for (int i = 0; i < 5; i++)
    {
        if (PCF.digitalRead(i) == LOW && bitRead(depressedButtons[i], spalte) == 1)
        {
            bitWrite(depressedButtons[i], spalte, 0);
            sendCommand("Button_page.bt" + String(btIDs[i][spalte]) + ".val=1");
        }
        else if (PCF.digitalRead(i) == HIGH && bitRead(depressedButtons[i], spalte) == 0)
        {
            bitWrite(depressedButtons[i], spalte, 1);
            sendCommand("Button_page.bt" + String(btIDs[i][spalte]) + ".val=0");
        }
    }
}

// RFID Functions
boolean readRFID()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return false;
    }
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return false;
    }
    String readKey = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        readKey.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        readKey.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    readKey.toUpperCase();
    if (readKey.substring(1) == rfidKey)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void lockLid()
{
    int rfidCooldown;
    if (rfidCooldown + 5000 <= millis())
    {
        if (!digitalRead(Door_Switch1) && !digitalRead(Door_Switch2) && !lidLocked)
        {
            if (readRFID())
            {
                lidLocked = true;
                Servo1.write(lockedDeg);
                Servo2.write(lockedDeg);
                sendCommand("page 4");
                digitalWrite(LED_STAT, LOW);
            }
        }
        else if (lidLocked)
        {
            if (readRFID())
            {
                lidLocked = false;
                Servo1.write(unlockedDeg);
                Servo2.write(unlockedDeg);
                sendCommand("page 1");
                digitalWrite(LED_STAT, HIGH);
            }
        }
        rfidCooldown = millis();
    }
}

// EEPROM Functions
void saveNames()
{
    int adress = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            writeStringToEEPROM(adress, names[i][j]);
            adress = adress + 1 * 40;
        }
    }
    saveEmptySlots();
}

void loadNames()
{
    int adress = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            names[i][j] = readStringFromEEPROM(adress);
            adress = adress + 1 * 40;
        }
    }
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
    byte len = strToWrite.length();
    EEPROM.write(addrOffset, len);
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
    }
}

String readStringFromEEPROM(int addrOffset)
{
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++)
    {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';
    return String(data);
}

void saveEmptySlots()
{
    int adress = 999;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (names[i][j] == "")
            {
                EEPROM.write(adress, 1);
            }
            else
            {
                EEPROM.write(adress, 0);
            }
            adress++;
        }
    }
}

void loadEmptySlots()
{
    int adress = 999;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (EEPROM.read(adress) == 1)
            {
                sendCommand("Button_page.bt" + String(btIDs[i][j]) + ".val=1");
            }
            else
            {
                sendCommand("Button_page.bt" + String(btIDs[i][j]) + ".val=0");
            }
            adress++;
        }
    }
    PCFread(PCF1, 0);
    PCFread(PCF2, 1);
    PCFread(PCF3, 2);
    PCFread(PCF4, 3);
    PCFread(PCF5, 4);
}