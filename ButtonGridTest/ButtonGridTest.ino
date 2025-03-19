#include <Adafruit_MCP23X17.h>
#include <SoftwareSerial.h>

#define RX 3
#define TX 4
SoftwareSerial nextion (3, 4);
Adafruit_MCP23X17 mcp;

//  User Data
String EGS12A[5][5] = {
    {"Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans"},
    {"Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans"},
    {"Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans"},
    {"Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans"},
    {"Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans","Mandl\nHans"}};

//  Button Data
int cols = 5;
int rows = 5;
int ButtonColumns[5] = {1,2,3,4,5};
int ButtonRows[5] = {8,9,10,11,12};

int depressedButtons[5][5] = {
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}};

int ButtonOverrides[5][5] = {
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,1,0},
    {0,0,0,0,1}};

// Display Data
int ButtonFields[5][5] = {
    {0,1,2,3,4},
    {5,6,7,8,9},
    {10,11,12,13,14},
    {15,16,17,18,19},
    {20,21,22,23,24}};

int ButtonFieldsHex[5][5] = {
    {0x00,0x01,0x02,0x03,0x04},
    {0x05,0x06,0x07,0x08,0x09},
    {0x10,0x11,0x12,0x13,0x14},
    {0x15,0x16,0x17,0x18,0x19},
    {0x20,0x21,0x22,0x23,0x24}};

String inactiveField = "42260";
String activeField = "62210";

void setup() {
    Serial.begin(9600);
    nextion.begin(9600);
    //MCP Setup und IO vergabe
    mcp.begin_I2C();
    for (int i = 0; i < cols; i++)
    {
        mcp.pinMode(ButtonColumns[i], OUTPUT);
        mcp.digitalWrite(ButtonColumns[i], LOW);
    }
    for (int i = 0; i < rows; i++)
    {
        mcp.pinMode(ButtonRows[i], INPUT);
    }

    delay(500); //Warten Bis Display Aktiv
    sendCommand("page 1");

    readButtons();
    refreshFields();
    writeNames();
}

void loop() {
    readOverrides();
    if (nextion.find("\x01")){
        readButtons();
        applyOverrides();
        refreshFields();
    }
}

void print2DArray() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Serial.print(ButtonOverrides[i][j]);
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.println();
}


//  Button Grid Read Functions
void readButtons() {
    for (int i = 0; i < cols; i++)
    {
        mcp.digitalWrite(ButtonColumns[i],HIGH);
        for (int j = 0; j < rows; j++)
        {            
            depressedButtons[j][i] = mcp.digitalRead(ButtonRows[j]);
        }
        mcp.digitalWrite(ButtonColumns[i],LOW);
    }
}

void applyOverrides() {
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (ButtonOverrides[i][j] == 1)
            {
                depressedButtons[i][j] = 1;
            }
        }
    }
}

//  Display Write Functions
void sendCommand(String cmd) {
    nextion.print(cmd);
    nextion.write(0xFF);
    nextion.write(0xFF);
    nextion.write(0xFF);
}

void writeFieldColors(int FieldID, bool active) {
    nextion.print("t");
    nextion.print(String(FieldID));
    nextion.print(".bco=");
    if (active)
    {
        nextion.print(activeField);
    }
    else
    {
        nextion.print(inactiveField);
    }
    nextion.write(0xFF);
    nextion.write(0xFF);
    nextion.write(0xFF);
}

void refreshFields() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
        {
            writeFieldColors(ButtonFields[i][j], depressedButtons[i][j] == 1);
        }
    }
}

void writeNames() {
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            sendCommand("t" + String(ButtonFields[i][j]) + ".txt=" + EGS12A[i][j]);
        }
        
    }
    
}

int readOverrides() {
    String readData;
    delay(30);
    while (nextion.available())
    {
        readData += nextion.read();
    }
    //Serial.println(readData);
    readData = decodeASCIIString(readData);
    //Serial.println(readData.substring(0,4));
        if (readData.substring(0,4) == "grid")
    {
/*         Serial.print("grid schei=e gelesen   ");
        Serial.println(readData.substring(4).toInt()); */
        return readData.substring(4).toInt();
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (readData.substring(4).toInt() == ButtonFields[i][j])
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
    readData = "";
}

String decodeASCIIString(String encoded) {
    String result = "";
    int i = 0;
    while (i < encoded.length()) {
        if (i + 3 <= encoded.length()) { // Try extracting a 3-digit ASCII code
            String charCode = encoded.substring(i, i + 3);
            int asciiVal = charCode.toInt();
            if (asciiVal >= 65 && asciiVal <= 122) { // Check if it's a valid letter
                result += (char)asciiVal;
                i += 3;
                continue;
            }
        }
        // If not a 3-digit letter, treat it as a 2-digit number
        if (i + 2 <= encoded.length()) {
            String charCode = encoded.substring(i, i + 2);
            result += (char)charCode.toInt();
            i += 2;
        }
    }
    return result;
}

//  Display Read Functions
/* void readOverrides() {
    byte identifier = Serial.read();
    byte FieldID;
    byte end1;
    byte end2;
    byte end3;
    if (identifier == 0x02)
    {
        FieldID = Serial.read();
        end1 = Serial.read();
        end2 = Serial.read();
        end3 = Serial.read();
        Serial.println("Signal erhalten");
    
        if (identifier == 0x02 && end1 == 0xFF && end2 == 0xFF && end3 == 0xFF) {
            Serial.println("Signal korrekt gelesen: " + String(FieldID));
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
 */

