#include <SoftwareSerial.h>


SoftwareSerial nextion(3, 4);  // RX, TX

void sendCommand(String cmd) {
  nextion.print(cmd);
  nextion.write(0xFF);
  nextion.write(0xFF);
  nextion.write(0xFF);
}

void setup() {
  Serial.begin(9600);
  nextion.begin(9600);
  delay(500);
  
  // Setze den Text in der Textbox "t0" auf "Hello"
  sendCommand("t0.txt=\"Martin Weishäupl\"");
  sendCommand("t1.txt=\"Anwesend\"");
  sendCommand("b0.txt=\"sound\"");
}

void loop() {
  if (nextion.available()) {
    String message = nextion.readString();  // Lese empfangene Daten
    Serial.println("Received: " + message);
    
    if (message.indexOf("65") >= 0) {  // 65 = Button Event (b0)
      Serial.println("Button Pressed!");
      sendCommand("t0.txt=\"Button gedrückt!\"");
    }
  }
}