/* neo pixel test on M5stack Fire */
/* Just scroll the leds set to green */

#include <Adafruit_NeoPixel.h>

#define LED_COUNT 10
#define NEO_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  pixels.begin(); 
}

void loop()  {
int ctr=0;
  for(ctr=0;ctr<10;ctr++) {
    pixels.setPixelColor(ctr, pixels.Color(0,7,0));
    if(ctr==0) {
      pixels.setPixelColor(9, pixels.Color(0,0,0));
    }
    else {
      pixels.setPixelColor(ctr-1,pixels.Color(0,0,0));
    }     
    pixels.show();
    delay(500);
  }
}
