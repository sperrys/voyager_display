

// Spencer Perry
// 6/17/19
// 
// This is the arduino code for the voyager mileage display.
// The program makes an http request a web scraper and then
// displays the milage via (3) 4 digit 7 segment over I2C.

#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

char* ssid      = "74Brom1";
char* password  = "GoodJob!";

#define DISPLAY_ADDRESS_1   0x70
#define DISPLAY_ADDRESS_2   0x71
#define DISPLAY_ADDRESS_3   0x72

#define USE_SERIAL Serial

// Create display object
Adafruit_7segment voyagerDisplay1 = Adafruit_7segment();
Adafruit_7segment voyagerDisplay2 = Adafruit_7segment();
Adafruit_7segment voyagerDisplay3 = Adafruit_7segment();

void setup() {

  // Setup Serial port to print debug output.
  Serial.begin(115200);
  Wire.begin(4,5);

  // Setup the display.
  voyagerDisplay1.begin(DISPLAY_ADDRESS_1);
  voyagerDisplay2.begin(DISPLAY_ADDRESS_2);
  voyagerDisplay3.begin(DISPLAY_ADDRESS_3);

  // Start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  String data, d1, d2, d3; 

  // Initialize HTTP Client
  HTTPClient http;
  http.setTimeout(10000);
  http.begin("http://voyagerscraper.herokuapp.com/");

  int httpCode = http.GET();

  // HttpCode will be negative on error
  
  if (httpCode > 0) {
    
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // File found at server
    if (httpCode == HTTP_CODE_OK) {

      // We know that voyager distance travel
      // will be simple ASCII string of digits
      data = http.getString();
      USE_SERIAL.println(data);
      int len = data.length();
      
      d3 = data.substring(len - 4, len);
      d2 = data.substring(len - 8, len - 4);
      d1 = data.substring(0, len - 8);

      // Print the value to each display, not written yet
      voyagerDisplay1.print(d1.toInt(), DEC);
      voyagerDisplay2.print(d2.toInt(), DEC);
      voyagerDisplay3.print(d3.toInt(), DEC);

      // Write the printed value to the actual physical display
      voyagerDisplay3.writeDisplay();
      voyagerDisplay2.writeDisplay();
      voyagerDisplay1.writeDisplay();

    }
    
  } 
  else {
    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  delay(1000);
}

