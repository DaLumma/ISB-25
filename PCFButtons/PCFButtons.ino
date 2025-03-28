#include <Adafruit_PCF8574.h>
#include <SoftwareSerial.h>

SoftwareSerial nextion (3,4); // RX,TX
Adafruit_PCF8574 PCF1, PCF2, PCF3, PCF4, PCF5;

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
    for (size_t i = 0; i < 8; i++)
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