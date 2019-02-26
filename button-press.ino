#include <M5Stack.h>

/* Just scroll the leds set R,G or B based on button press */

#include <Adafruit_NeoPixel.h>

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  M5.begin();
  pixels.begin();
}

void loop()  {
  int ctr;
  int red,green,blue;

  for(ctr=0;ctr<10;ctr++) {
    M5.update();
    if (M5.BtnA.wasPressed()) {
      red=7;green=0;blue=0;
    }
    if (M5.BtnB.wasPressed()) {
      red=0;green=7;blue=0;
    }
    if (M5.BtnC.wasPressed()) {
      red=0;green=0;blue=7;
    }
    pixels.setPixelColor(ctr, pixels.Color(red,green,blue));
    if(ctr==0) {
      pixels.setPixelColor(9,pixels.Color(0,0,0));
    }
    else {
      pixels.setPixelColor(ctr-1,pixels.Color(0,0,0));
    }
    pixels.show();
    delay(50);
  }
}
