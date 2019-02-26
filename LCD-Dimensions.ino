#include <M5Stack.h>

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
}

void loop() {
  //For text size 3
  M5.Lcd.clear();
  M5.Lcd.setTextSize(3);
  
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.printf("ABCDEFGHIJKLMNOPQ");
  
  M5.Lcd.setCursor(5, 40);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("01234567890123456");

  
  M5.Lcd.setCursor(5, 75);
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.printf("abcdefghijklmnopq");

  
  M5.Lcd.setCursor(5, 110);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.printf("Display Test4");

  M5.Lcd.setCursor(5, 145);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("Display Test5");

  M5.Lcd.setCursor(5, 180);
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.printf("Display Test6");

  M5.Lcd.setCursor(5, 215);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.printf("Display Test7");
  
  delay(5000);
  M5.Lcd.clear();
  
  //For text size 2
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(GREEN);
  
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.printf("12345678901234567890123456");

  M5.Lcd.setCursor(5, 30);
  M5.Lcd.printf("abcdefghijklmnopqrstuvwxyz");
  
  M5.Lcd.setCursor(5,50);
  M5.Lcd.printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  M5.Lcd.setCursor(5, 70);
  M5.Lcd.printf("12345678901234567890123456");

  M5.Lcd.setCursor(5, 90);
  M5.Lcd.printf("abcdefghijklmnopqrstuvwxyz");
  
  M5.Lcd.setCursor(5,110);
  M5.Lcd.printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  
  M5.Lcd.setCursor(5, 130);
  M5.Lcd.printf("12345678901234567890123456");

  M5.Lcd.setCursor(5, 150);
  M5.Lcd.printf("abcdefghijklmnopqrstuvwxyz");
  
  M5.Lcd.setCursor(5,170);
  M5.Lcd.printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  
  M5.Lcd.setCursor(5,190);
  M5.Lcd.printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  
  M5.Lcd.setCursor(5, 210);
  M5.Lcd.printf("abcdefghijklmnopqrstuvwxyz");
  
  delay(5000);
}
