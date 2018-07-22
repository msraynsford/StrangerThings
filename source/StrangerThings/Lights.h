#ifndef Lights_H
#define Lights_H

#define LED_PIN   D1
#define NUMLEDS  26

const byte BRIGHT = 2;

CRGB leds[NUMLEDS];

uint8_t pos = 0;
uint8_t ledNo = 0;
uint8_t msglen = 0;

// Start the mapping array at 1 because everyone knows a=1 b=2...
uint8_p MAP[] PROGMEM = {0, 0, 1, 2, 3, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 9, 8, 17, 18, 19, 20, 21, 22, 23, 24, 25 };


// Setup the LEDs in the lid
void InitLights() {
  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUMLEDS);
  
  for(int i=0; i<NUMLEDS; i += 1)
    switch(i % 4) {
      case 0: leds[i] = CRGB(0, 0, BRIGHT); break;
      case 1: leds[i] = CRGB(0, BRIGHT, 0); break;
      case 2: leds[i] = CRGB(BRIGHT, 0, 0); break;
      case 3: leds[i] = CRGB(BRIGHT, BRIGHT, 0); break;
    }
  
  FastLED.show();

  msglen = strlen(config.message);
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

void UpdateLights() {
  uint8_t color;
  
  if((pos % 2) && (pos <= (msglen * 2))) {
    char letter = config.message[pos/2];
    char mapped = mapChar(letter);
  
//      Serial.printf("pos: %d, char: %c, num: %d\n", pos, letter, mapped);
  
    if(mapped > 0) {
      ledNo = MAP[mapped];
      leds[ledNo] *= 10;
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

#endif

