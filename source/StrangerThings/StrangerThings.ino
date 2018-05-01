#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <DNSServer.h>
#include <FS.h>

#include <FastLED.h>
#include <PGMWrap.h>

#include <EEPROMAnything.h>

#define HTTP_PORT 80
#define DNS_PORT  53
#define WS_PORT   2205

#define NUMLEDS   50
#define LED_PIN   D1

#define BRIGHT    2
#define PAUSE     350

DNSServer dnsServer;
ESP8266WebServer webServer(HTTP_PORT);

CRGB leds[NUMLEDS];

uint8_t pos = 0;
uint8_t ledNo = 0;

// Start the mapping array at 1 because everyone knows a=1 b=2...
uint8_p MAP[] PROGMEM = {0, 0, 1, 2, 3, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 9, 8, 17, 18, 19, 20, 21, 22, 23, 24, 25 };

uint8_t msglen = 0;

struct config_t{
  char ssid[17] = "StrangerThing2";
  char password[17] = "password";
  char message[50] = "help me";
}  config;

const String dnsName = "stranger";

void setup() {
  Serial.begin(115200); Serial.println();
   
  EEPROM.begin(sizeof(config.message));
  EEPROM_readAnything(0, config.message);
  msglen = strlen(config.message);
  
  if((msglen == 0)||(msglen > 50)){
    strcpy(config.message, "Help Me");
    EEPROM_writeAnything(0, config.message);
    msglen = strlen(config.message);
  }

  pos = (msglen * 2);
  Serial.println(config.message);
  
  FastLED.addLeds<WS2812, LED_PIN>(leds, NUMLEDS);

  Serial.printf("SSID: '%s'\n", config.ssid);  
  Serial.printf("Password: '%s'\n", config.password);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(config.ssid, config.password, 1, false, 1);

  SPIFFS.begin();  

  // Create a simple homepage
  webServer.on("/", serveMessage);
  
  // Redirect all unknown traffic back to the homepage
  webServer.onNotFound([](){
    webServer.sendHeader("Location", "http://" + dnsName + ".local/");
    webServer.send(303);
  });

  MDNS.begin(dnsName.c_str());
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  webServer.begin();

  msglen = strlen(config.message);
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();  

  EVERY_N_MILLISECONDS(PAUSE) { 
    uint8_t color;
    
    if((pos % 2) && (pos <= (msglen * 2))) {
      char letter = config.message[pos/2];
      char mapped = mapChar(letter);
    
//      Serial.printf("pos: %d, char: %c, num: %d\n", pos, letter, mapped);
    
      if(mapped > 0) {
        ledNo = MAP[mapped];

        color = (ledNo % 4);
        
        switch(color) {
          case 0: leds[ledNo] *= 10; break;
          case 1: leds[ledNo] *= 10; break;
          case 2: leds[ledNo] *= 10; break;
          case 3: leds[ledNo] *= 10; break;
        }                 
      }
    } 
    else { 
      for(int i=0; i<NUMLEDS; i += 1) {
        color = (i % 4);
        
        switch(color) {
          case 0: leds[i] = CRGB(0, 0, BRIGHT); break;
          case 1: leds[i] = CRGB(0, BRIGHT, 0); break;
          case 2: leds[i] = CRGB(BRIGHT, 0, 0); break;
          case 3: leds[i] = CRGB(BRIGHT, BRIGHT, 0); break;
        }
      }
    }
    
    FastLED.show();
    
    // Create a gap at the end of the message
    pos += 1;
    if(pos == (msglen * 2) + 4) {
      pos = 0;
    }
  }
}

char mapChar(char letter) {
  if((letter >= 0x61) && (letter <= 0x7A))
    letter -= 0x60;
  
  else if((letter >= 0x41) && (letter <= 0x5A))
    letter -= 0x40;

  else
    letter = 0;

  return letter;
}

void serveMessage() {
  if(webServer.hasArg("msg")) {
    webServer.arg("msg").toCharArray(config.message, sizeof(config.message));
    msglen = strlen(config.message);
    pos = (msglen * 2);

    // Save the new message to EEPROM
    EEPROM_writeAnything(0, config.message);    
    EEPROM.commit();
  }
  
  //Serve a HTML page containing an input button
  String htmlPage = "<form action='/'><input type='text' name='msg' value='$v' maxlength='49'><input type='submit' value='Update Message'></form>";
  htmlPage.replace("$v", config.message);
  webServer.send(200, "text/html", htmlPage);
}
