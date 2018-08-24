#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>

#include <FastLED.h>
#include <PGMWrap.h>

#include "Config.h"
#include "FirmwareReset.h"
#include "AdminPage.h"
#include "Lights.h"

#define DNS_NAME  "Stranger"
#define DNS_URL   "http://" DNS_NAME ".local/"

#define HTTP_PORT 80
#define DNS_PORT  53
#define WS_PORT   2205

#define PAUSE     350

DNSServer dnsServer;
ESP8266WebServer webServer(HTTP_PORT);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  Serial.printf("\nStranger Things\n");
    
  InitConfig();
  
  //Start the wifi with the required username and password
  WiFi.mode(WIFI_AP);

  LoadConfig();
  //ResetConfig();
  PrintConfig();

  //Check to see if the flag is still set from the previous boot
  if(checkResetFlag()) {
    //Do the firmware reset here
    Serial.printf("Reset Firmware\n");

    //Set the ssid to default value and turn off the password
    WiFi.softAP("APConfig", "", 1, false, 1);
  } 
  else {
      WiFi.softAP(config.ssid, config.pass, 1, false, 1);
  }

  //Serve a very basic page on the root url
  webServer.on("/", serveMessage);

  // Set up the admin page
  webServer.on("/admin", std::bind(serveAdmin, &webServer));

  // Redirect all unknown traffic back to the homepage
  webServer.onNotFound([](){
    webServer.sendHeader("Location", DNS_URL);
    webServer.send(303);
  });

  webServer.begin();

  MDNS.begin(DNS_NAME);
  MDNS.addService("http", "tcp", HTTP_PORT);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  InitLights();
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();  

  EVERY_N_MILLISECONDS(PAUSE) { 
    UpdateLights();
  }
}

void serveMessage() {
  if(webServer.hasArg("message")) {
    webServer.arg("message").toCharArray(config.message, sizeof(config.message));
    msglen = strlen(config.message);
    pos = (msglen * 2);

    // Save the new message to EEPROM
    SaveConfig();
 }
  
  //Serve a HTML page containing an input button
  String htmlPage = "<html>"
                      "<head>"
                        "<style>input {font-size: 1.2em; width: 90%; display: block; margin: 5px auto; } </style>"
                      "</head>"
                      "<body>"
                        "<form action='/'>"
                        "<input type='text' name='message' value='$v' maxlength='49'><input type='submit' value='Update Message'>"
                      "</body>"
                    "</form>";
  
  htmlPage.replace("$v", config.message);
  webServer.send(200, "text/html", htmlPage);
}
