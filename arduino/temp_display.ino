#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

String candidateName = "Cyubahiro Don Durkheim";

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int scrollCursor = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 300;
String paddedName;
unsigned long lastReadTime = 0;
const int readInterval = 2000;
float currentTemp = 0.0;
void setup() {
Serial.begin(9600);
dht.begin();
lcd.init();
lcd.backlight();

if (candidateName.length() > 16) {
    paddedName = candidateName + "                ";
  } else {
    paddedName = candidateName;
  }
}
void loop() {
unsigned long currentMillis = millis();
  
if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;
    
float temp = dht.readTemperature();
   
if (!isnan(temp)) {
      currentTemp = temp;
      
Serial.print("TEMP:");
Serial.println(currentTemp);
      
lcd.setCursor(0, 1);
lcd.print("Temp: ");
lcd.print(currentTemp);
lcd.print(" C    ");
    } else {
Serial.println("ERROR: Failed to read from DHT sensor!");
    }
  }
  
if (candidateName.length() > 16) {
if (currentMillis - lastScrollTime >= scrollDelay) {
      lastScrollTime = currentMillis;
lcd.setCursor(0, 0);
lcd.print(paddedName.substring(scrollCursor, scrollCursor + 16));
      scrollCursor++;
if (scrollCursor > candidateName.length()) {
        scrollCursor = 0;
      }
    }
  } else {
lcd.setCursor(0, 0);
lcd.print(candidateName);
  }
}
