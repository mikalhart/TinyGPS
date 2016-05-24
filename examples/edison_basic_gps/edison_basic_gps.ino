#define SerialGPS Serial1
#include "TinyGPS.h"

TinyGPS gps;

int edisonRxPin = 0; // connected to Tx pin of the GPS
int edisonTxPin = 1; // connected to Rx pin of the GPS

long startMillis;
long secondsToFirstLocation = 0;

void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  startMillis = millis();
  Serial.print("SETUP COMPLETE! ");
  Serial.println(startMillis);
}


float pflat, pflon;
void loop()
{
  Serial.print(".");
  bool newData = false;
  unsigned long chars = 0;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (SerialGPS.available())
    {
      Serial.print("-");
      int c = SerialGPS.read();
      ++chars;
      if (gps.encode(c)) {// Did a new valid sentence come in?
        Serial.print("*");
        newData = true;
      }
    }
  }

  if (newData){
    if(secondsToFirstLocation == 0){
      secondsToFirstLocation = (millis() - startMillis) / 1000;
    }
    
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);

    if(pflat!=flat || pflon!=flon){
      Serial.println();
      Serial.print("NEW DATA:  ");
      pflat = flat;
      pflon = flon;
    } else {
      Serial.println();
      Serial.print("SAME DATA:  ");
    }
    Serial.println();
    Serial.print("LAT:");
    Serial.print(flat,6);
    Serial.print(",");
    Serial.print("LON:");
    Serial.println(flon,6 );

    char strValue[200];
    String strLat  = "";

    sprintf(strValue, "%.7f", flat);
    strLat = strValue;

    String strLong = "";
    sprintf(strValue, "%.7f", flon);
    strLong = strValue;      

    String jsonData = "{\"lat\":\"" +strLat + "\",\"long\":\""+strLong+"\"}";
    Serial.print("JSON: ");
    Serial.println(jsonData);
  }
  
  if (chars == 0){
    // if you haven't got any chars then likely a wiring issue
    Serial.println("No GPS: check wiring");
  }
  else if(secondsToFirstLocation == 0){
    // if you have received some chars but not yet got a fix then indicate still searching and elapsed time

    long seconds = (millis() - startMillis) / 1000;
    
    Serial.println("Searching ");
    for(int i = 0; i < seconds % 4; ++i){
      Serial.print(".");
    }
    
    Serial.print("Elapsed time:");
    Serial.print(seconds);
    Serial.println("s");
  }
}
