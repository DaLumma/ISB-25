#include <wire.h>
char button [7] = {};

void setup() {
Serial.begin(9600);
String cmd;
cmd += "\"";

// Textfeld beim Start leeren
Serial.print("tStatus.txt=" + cmd + " " + cmd);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.print("tStatus.txt=" + cmd + " " + cmd);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.write(0xFF);
}

void loop() {
// Serielle Schnittstelle auslesen
if (Serial.available() > 0)
{
for (int i = 0; i < 6; i++) {
button[i] = button[i + 1];
}
button[6] = Serial.read();
}

// Button Abfrage + Aktion
if (button[2] == 1)
{
String cmd;
cmd += "\"";
Serial.print("tStatus.txt=" + cmd + "LED EIN" + cmd);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.print("tStatus.txt=" + cmd + "LED EIN" + cmd);
Serial.write(0xFF);
Serial.write(0xFF);
Serial.write(0xFF);
for (int i = 0; i < 7; i++) {
button[i] = 0;
}
}
delay(50);
}