#include <WiFi.h>
#include <M5Stack.h>

void setup() {
 M5.begin();
 M5.Lcd.clear();

}

void loop() {
  draw_box(0,0,319, 200,GREEN);
  draw_buttons("Start",YELLOW,"Stop ",YELLOW,"Go   ",YELLOW, BLACK);
  //draw_text_box(0,0,WHITE,GREEN,"AA:BB:CC:11:22:33");
  delay(1000);

}
void draw_buttons(char *btna, int cola, char* btnb, int colb, char* btnc, int colc, int border) {
  draw_text_box(35,208,border, cola, btna);
  draw_text_box(130,208,border, colb, btnb);
  draw_text_box(225,208,border, colc, btnc);
}

void draw_text_box(int startx, int starty,int border_color, int text_color, char* text) {
  int text_x_offset=5;
  int text_y_offset=6;
  int char_width=12;
  int char_height=24;
  int endx,endy,char_count;
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(text_color);
  M5.Lcd.setCursor((startx+text_x_offset), (starty+text_y_offset));
  M5.Lcd.print(text);
  draw_box(startx,starty,(char_width*strlen(text)+text_x_offset+startx)+2,char_height+text_y_offset+starty,border_color);
}
//Max dim X=0 to 319, Y=0 to 239
void draw_box(int startx, int starty, int endx, int endy, int colour) {
  M5.Lcd.drawLine(startx,starty, endx,starty,colour);
  M5.Lcd.drawLine(startx,starty, startx,endy,colour);
  M5.Lcd.drawLine(startx,endy,endx,endy,colour);
  M5.Lcd.drawLine(endx,starty,endx,endy,colour);
}
