/* neo pixel test on M5stack Fire */
/* Just scroll the leds set to green */

#include <Adafruit_NeoPixel.h>

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

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
