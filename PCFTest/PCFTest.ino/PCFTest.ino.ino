#include <Wire.h>
#include <Adafruit_PCF8574.h>

Adafruit_PCF8574 ButtonColumn1;



void setup() {
  // put your setup code here, to run once:
/*   ButtonColumn1.begin(0x20);
  ButtonColumn1.pinMode(0, INPUT_PULLUP); */
  setPCF(ButtonColumn1);
  Serial.begin(9600);
  Serial.println(ButtonColumn1.digitalRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:

}

void setPCF(Adafruit_PCF8574 &PCF){
  PCF.begin(0x20);
  PCF.pinMode(0,INPUT_PULLUP);
}