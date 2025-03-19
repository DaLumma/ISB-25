
// Inklusive der Adafruit-MCP23017 Bibliothek
#include <Adafruit_MCP23X17.h>

// Instanz des MCP23017 erstellen
Adafruit_MCP23X17 mcp;

// Pin-Definitionen
#define LED_PIN 0   // LED an Pin GPA0
#define BUTTON_PIN 1 // Button an Pin GPA1

void setup() {
  // Startet die serielle Kommunikation
  Serial.begin(9600);
  Serial.println("MCP23017 Test");

  // MCP23017 initialisieren
  mcp.begin_I2C(); // Standard-Adresse 0x20

  // Konfiguration des LED-Pins als Ausgang
  mcp.pinMode(LED_PIN, OUTPUT);

  // Konfiguration des Button-Pins als Eingang mit Pull-Up
  mcp.pinMode(BUTTON_PIN, INPUT_PULLUP);
  //mcp.pullUp(BUTTON_PIN, HIGH); // Internen Pull-Up aktivieren

  // LED ausschalten
  mcp.digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Status des Buttons auslesen
  int buttonState = mcp.digitalRead(BUTTON_PIN);

  // Wenn der Button gedrückt ist (Low wegen Pull-Up)
  if (buttonState == LOW) {
    Serial.println("Button gedrückt!");
    mcp.digitalWrite(LED_PIN, HIGH); // LED einschalten
  } else {
    mcp.digitalWrite(LED_PIN, LOW); // LED ausschalten
  }
  mcp.setupInterruptPin(1, HIGH);
  delay(100); // Kurze Verzögerung zur Entprellung
}
