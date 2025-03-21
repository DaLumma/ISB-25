#include <Adafruit_PCF8574.h> // Bibliothek einbinden
#include <SoftwareSerial.h>
Adafruit_PCF8574 pcf_1; //Ersten PCF definieren
Adafruit_PCF8574 pcf_2; //Ersten PCF definieren

SoftwareSerial NextionSerial (3,4); // RX,TX

int quersumme_ports_alt = 0;
int quersumme_ports_neu = 0;
int quersumme_ports_active = 0;
bool ports[8] = {true,true,true,true,true,true,true,true};
byte data[8];
bool ports_active[8] = {true,true,true,true,true,true,true,true};
bool changes = false;

unsigned long timeElapsed;

void setup() 

{
  NextionSerial.begin (9600);
  
  while (!Serial) { delay(10); }
  Serial.begin(9600);   //Seriellen Monitor aktivieren

  if (!pcf_1.begin(0x38, &Wire)) //Adresse den ersten PCF's deklarieren und verbindung aufbauen
    {
    Serial.println("PCF 1 konnte nicht gefunden werden");
    while (1);
    }
  for (uint8_t p=0; p<8; p++) //Variable p von 0 bis 7 Zählen lassen, um alle Ports von PCF 1 als Eingang mit Pullup zu setzen
    {
    pcf_1.pinMode(p, INPUT_PULLUP);
    }
  if (!pcf_2.begin(0x39, &Wire)) //Adresse für den zweiten PCF's deklarieren und verbindung aufbauen
    {
    Serial.println("PCF 2 konnte nicht gefunden werden");
    while (1);
    }
  for (uint8_t p=0; p<8; p++) //Variable p von 0 bis 7 Zählen lassen, um alle Ports von PCF 2 als Ausgang zu setzen
    {
    pcf_2.pinMode(p, OUTPUT);
    }
  }

void loop() 
{
  quersumme_ports_alt = 0;
  for(int q = 0; q < 8; q++)
  {
    //quersumme_alt = quersumme_alt + ports[q];
    quersumme_ports_alt = quersumme_ports_alt + (ports[q] * pow (2, q));
  }

 

  for (uint8_t p=0; p<8; p++) 
    {
      if(pcf_1.digitalRead(p) == LOW && ports[p])
        { 
        Serial.println("ja");
        ports[p] = false;
        pcf_2.digitalWrite(p, LOW);
        Serial.print(ports[p]);
        Serial.println("i2c ports array");
        }

       
        else if(pcf_1.digitalRead(p) == HIGH && !ports[p])
        {
        pcf_2.digitalWrite(p, HIGH);
        ports[p] = true;
        Serial.println("nein");
        Serial.print(ports[p]);
        Serial.println("i2c ports array2");
        }
      
    }

    quersumme_ports_neu = 0;
    for(int q = 0; q < 8; q++)
    {
    //  quersumme_neu = quersumme_neu + ports[q];
      quersumme_ports_neu = quersumme_ports_neu + (ports[q] * pow (2, q));
    }


quersumme_ports_active = 0;

  while(NextionSerial.available())
  {
    for(int d = 0; d < 8; d++)
    {
      data[d] = NextionSerial.read();
     // Serial.print(NextionSerial.read());
      Serial.print(data[d]);
      quersumme_ports_active = quersumme_ports_active + ports_active[d] * pow(2, d);
    }
  Serial.println("=data_ports_active_array");
  changes = true;
  }

 if(changes)
 {
 submit_changes();
 }

 if(quersumme_ports_neu == quersumme_ports_active)
 {
  Serial.println("ka");
 }
}


void submit_changes ()
{
   for(int i = 0; i < 6; i++)
 {
  if(data[0] == i + 1) 
  {
    if(data[4] == 1)
    {
      ports_active[i] = true;
    }
    else
    {
      ports_active[i] = false;
    }
  }
 }
   for(int i = 0; i < 6; i++)
  {
    Serial.print(ports_active[i]);
    delay(10);
  }
  Serial.println("=portactive");
  changes = false;
}
