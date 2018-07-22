#ifndef Config_H
#define Config_H

#include "EEPROMAnything.h"

#define MAX_STR_LEN 17
#define MIN_STR_LEN 8
#define MAX_MSG_LEN 50

#define SSID_DEFAULT "StrangerThings"
#define PASS_DEFAULT "password"

struct config_t{
  char ssid[MAX_STR_LEN] = SSID_DEFAULT;
  char pass[MAX_STR_LEN] = PASS_DEFAULT;
  char message[MAX_MSG_LEN] = "Help Me";
} config;

// Writes the config values back to EEPROM
void SaveConfig() {
  // Store the new settings to EEPROM
  EEPROM_writeAnything(0,  config);    
}

// Loads the config values from EEPROM, leaves the default values if the EEPROM hasn't been set yet
void LoadConfig() {
   // Declare an area of eeprom where the variables are stored
  EEPROM.begin(sizeof(config));

  // Load the config variables from EEPROM
  config_t eepromConfig;
  EEPROM_readAnything(0, eepromConfig);

  //Check to see if the config variables loaded from eeprom are valid
  if(((strlen(eepromConfig.ssid) >= MIN_STR_LEN) && (strlen(eepromConfig.ssid) < MAX_STR_LEN)) || 
     (strlen(eepromConfig.pass) < MAX_STR_LEN)) {
    strcpy(config.ssid, eepromConfig.ssid);
    strcpy(config.pass, eepromConfig.pass);
    strcpy(config.message, eepromConfig.message);
  }  
  else {
    // If the EEROM isn't valid then create a unique name for the wifi
    SaveConfig();
  }
}

// Sends a copy of the config values out to the serial port
void PrintConfig() {
  Serial.printf("SSID: '%s'\n", config.ssid);  
  Serial.printf("Pass: '%s'\n", config.pass);
  Serial.printf("Message: '%s'\n", config.message);
}

#endif
