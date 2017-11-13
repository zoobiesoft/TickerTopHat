#include <ESP8266WiFi.h>
#include "TickerTopHatPage.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const char WiFiAPPSK[] = "1234567890";       // Note... need to change
WiFiServer server(80);
String value = "Ask me about M4 Reactor ... Follow me to M4 Reactor ...";
int pixelsInText = (value.length() * 8)+20;
int z=1;


#define NEO_PIN     2 // Arduino pin to NeoPixel data input
#define NEO_WIDTH  64 // Hat circumference in pixels
#define NEO_HEIGHT  8 // Number of pixel rows (round up if not equal)
#define NEO_OFFSET  (((NEO_WIDTH * NEO_HEIGHT) - 240) / 2)
Adafruit_NeoMatrix matrix(NEO_WIDTH, NEO_HEIGHT, NEO_PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB         + NEO_KHZ800);

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(MAGENTA);
  Serial.begin(115200);
  delay(10);

  setupWiFi();
  server.begin();
}

int x = matrix.width();

void loop() {

  WiFiClient client = server.available();
  if (!client) {
    //return;
  }

  // Read the first line of the request
  String command1 = client.readStringUntil('/');
  String command = client.readStringUntil('/');
  // Serial.println(command);
  
  if (command == "text") {
    value = client.readStringUntil('/');
    z=1;
    value.replace("%20", " ");
    pixelsInText = (value.length() * 8);
    Serial.println(value);
    //matrix.setTextColor(matrix.Color(random(0,255),random(0,255),random(0,255)));
  } else {  // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += file1;  
    client.flush();
  
    // Send the response to the client
    while(s.length()>2000)
    {
      String dummy = s.substring(0,2000);
      client.print(dummy);
      s.replace(dummy," ");
    }
      
    client.print(s);
    delay(1);
    //Serial.println("Client disconnected");
    
    // The client will actually be disconnected 
    // when the function returns and 'client' object is destroyed
  }
  
  if(z){ // = 1
    while(x+65 > (matrix.width() - pixelsInText)){
      matrix.fillScreen(0);
      matrix.setCursor(--x, 0);
      matrix.print(value);
      matrix.show();
      matrix.setTextColor(matrix.Color(random(0,255),random(0,255),random(0,255)));
      delay(30);
    }
    x = matrix.width();
  }
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "TickerTopHat " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
