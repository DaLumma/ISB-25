#include <Adafruit_PCF8574.h>
#include <SoftwareSerial.h>

SoftwareSerial NextionSerial (3,4); // RX,TX
Adafruit_PCF8574 ButtonColumn1, ButtonColumn2, ButtonColumn3, ButtonColumn4, ButtonColumn5;

byte depressedButtons[5];

void PCFsetup(Adafruit_PCF8574 PCF, int adress){
    if (!PCF.begin(adress, &Wire))
    {   
        Serial.println("Couldn't find PCF8574" + adress);
    }
    for (size_t i = 0; i < 8; i++)
    {
        PCF.pinMode(i, INPUT_PULLUP);
    }
}

void PCFread(Adafruit_PCF8574 PCF, int column){
    for (int i = 0; i < 8; i++)
    {
        bitWrite(depressedButtons[column], i, bitRead(bit(PCF.digitalRead(i)), 1));// Convert Bool to bit
    }
}

void setup(){
    Serial.begin(9600);

    PCFsetup(ButtonColumn1, 0x20);
    PCFsetup(ButtonColumn2, 0x21);
    PCFsetup(ButtonColumn3, 0x3A);
    PCFsetup(ButtonColumn4, 0x3B);
    PCFsetup(ButtonColumn5, 0x3C);

    PCFread(ButtonColumn1, 0);
    PCFread(ButtonColumn2, 1);
    PCFread(ButtonColumn3, 2);
    PCFread(ButtonColumn4, 3);
    PCFread(ButtonColumn5, 4);

    for (int i = 0; i < 5; i++)
    {
        Serial.println(depressedButtons[i], BIN);
    }
    Serial.println(ButtonColumn1.digitalRead(1));
}

void loop(){

}
