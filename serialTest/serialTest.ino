#include <Wire.h>

int ButtonOverrides[5][5] = {
    {0,0,0,0,0},
    {0,1,0,0,0},
    {0,0,1,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}};

int ButtonFieldsHex[5][5] = {
    {0x00,0x01,0x02,0x03,0x04},
    {0x05,0x06,0x07,0x08,0x09},
    {0x10,0x11,0x12,0x13,0x14},
    {0x15,0x16,0x17,0x18,0x19},
    {0x20,0x21,0x22,0x23,0x24}};

void setup() {
    Serial.begin(9600); // Initialize serial communication
}

void loop(){
    readOverrides();
}

void readOverrides() {
    if (Serial.available() >= 5) { // Ensure we have at least 5 bytes
        byte identifier = Serial.read();
        byte FieldID = Serial.read();
        byte end1 = Serial.read();
        byte end2 = Serial.read();
        byte end3 = Serial.read();

        // Validate the packet structure
        if (identifier == 0x02 && end1 == 0xFF && end2 == 0xFF && end3 == 0xFF) {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    if (FieldID == ButtonFieldsHex[i][j])
                    {
                        if (ButtonOverrides[i][j] == 0){
                        ButtonOverrides[i][j] = 1;
                        }else
                        {
                            ButtonOverrides[i][j] = 0;
                        }
                    }
                }
            }
        }
    }
}