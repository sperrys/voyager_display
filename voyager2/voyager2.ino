

// Spencer Perry
// 6/17/19
// 
// This is the arduino code for the voyager mileage display.
// The program makes an http request a web scraper and then
// displays the milage via (3) 4 digit 7 segment over I2C.

#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiManager.h>

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

char* ssid      = "Tufts Secure";
char* password  = "notallpeople";

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
  Wire.begin();

  // Setup the display.
  voyagerDisplay1.begin(DISPLAY_ADDRESS_1);
  voyagerDisplay2.begin(DISPLAY_ADDRESS_2);
  voyagerDisplay3.begin(DISPLAY_ADDRESS_3);

  // Initialize WIFI Manager 
  WiFiManager wifiManager;
  wifiManager.autoConnect("AP-NAME", "AP-PASSWORD");

  //WiFi.begin(ssid, password);

  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  // }

  // Serial.println("");
  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

}

void loop() {

  String data; 
  String d1, d2, d3; 

  // Initialize HTTP Client
  HTTPClient http;
  http.setTimeout(10000);
  http.begin("http://voyagerscraper.herokuapp.com/");

  int httpCode = http.GET();

  // HttpCode will be negative on error
  
  if (httpCode > 0) {
    
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // Got a Good Response 
    if (httpCode == HTTP_CODE_OK) {

      // We know that the response will be voyager's distance traveled (in miles)       
      data = http.getString();
      USE_SERIAL.println(data);
      
      int len = data.length();

      // We split that mileage into the digits for each display.
      d1 = data.substring(0, len - 8);
      d2 = data.substring(len - 8, len - 4);
      d3 = data.substring(len - 4, len);

      USE_SERIAL.println("Display One: " + d1);
      USE_SERIAL.println("Display Two: " + d2);
      USE_SERIAL.println("Display Three: " + d3);

      // Print the value to each display, not written yet
      voyagerDisplay1.print(d1.toInt(), DEC);
      //voyagerDisplay2.print(d2.toInt(), DEC);
      //voyagerDisplay3.print(d3.toInt(), DEC);

      // Only pad the second and third displays 
      padDisplayWithZeros(d2, &voyagerDisplay2);
      padDisplayWithZeros(d3, &voyagerDisplay3);
      
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


void padDisplayWithZeros(String segmentData, Adafruit_7segment * segmentDisplay) {

  int val = segmentData.toInt();
  USE_SERIAL.println(segmentData);
 
  // writeDigitNum(location, number) 
  // All digits are zeros 
  
  if (val == 0) {
      segmentDisplay->writeDigitNum(0, 0);
      segmentDisplay->writeDigitNum(1, 0);
      segmentDisplay->writeDigitNum(2, 0);
      segmentDisplay->writeDigitNum(3, 0);
  }
  
  // All but the last digit on the display should be a zero 
  else if (val < 10) {
      segmentDisplay->writeDigitNum(0, 0);
      segmentDisplay->writeDigitNum(1, 0);
      segmentDisplay->writeDigitNum(3, 0);
      segmentDisplay->writeDigitNum(4, String(segmentData[3]).toInt());

  }
  
  // The first two digits on the display should be zeroes 
  else if (val < 100) {
      segmentDisplay->writeDigitNum(0, 0);
      segmentDisplay->writeDigitNum(1, 0);
      segmentDisplay->writeDigitNum(3, String(segmentData[2]).toInt());
      segmentDisplay->writeDigitNum(4, String(segmentData[3]).toInt());
  }
  // Only the first digit of the display should be a zero
  else if (val < 1000) {
      segmentDisplay->writeDigitNum(0, 0);
      segmentDisplay->writeDigitNum(1, String(segmentData[1]).toInt());
      segmentDisplay->writeDigitNum(3, String(segmentData[2]).toInt());
      segmentDisplay->writeDigitNum(4, String(segmentData[3]).toInt());
  }
  // No padding of zeros needed 
  else {
      segmentDisplay->print(segmentData.toInt(), DEC);
  }
}
