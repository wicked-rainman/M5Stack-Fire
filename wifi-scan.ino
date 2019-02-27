#include <WiFi.h>
#include <M5Stack.h>
typedef struct {
    int channel;
    char bssid_str[40];
    char ssid[50];
    int rssi;
    int encryption;
    int network_number;
} network;
network networks[100];

int scan;
int networks_found;
int current_network;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.clear();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  draw_button_menu("   Scan",YELLOW);
  networks_found=0;
  current_network=0;
  delay(100);
}

void loop() {

  M5.update();
  if(M5.BtnA.isPressed()) {
    do_scan();
  }
  if(M5.BtnB.isPressed()) {
    M5.Lcd.clear();
    draw_button_menu("   Scan   Scroll    +",YELLOW);
    if(networks_found==0) {
      M5.Lcd.setTextColor(RED);
      M5.Lcd.print("No Networks found");
    }
    else {
      if(current_network==networks_found) current_network=0;
      else current_network++;
      display_network(current_network);
    }
  }
  if(M5.BtnC.isPressed()) {
    
        draw_button_menu("   Scan   Scroll    +",YELLOW);
    if(networks_found==0) {
      M5.Lcd.setTextColor(RED);
      M5.Lcd.print("No Networks found");
    }
    else {
      M5.Lcd.clear();
      draw_button_menu("           Back   ",GREEN);
      focus_on_network(networks[current_network].bssid_str);
    }
  }
  delay(200);
}
void draw_button_menu(char *button_str, int color) {
  M5.Lcd.drawLine(1,210,320,210,WHITE);  
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(5, 220);
  M5.Lcd.printf(button_str);
}

void display_network(int net) {
  M5.Lcd.clear();
      draw_button_menu("   Scan   Scroll    +",YELLOW);

  //draw_button_menu("   Scan    Down     Up",YELLOW);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.printf("%s",networks[current_network].bssid_str);
  M5.Lcd.setCursor(5,30);
  M5.Lcd.printf("%s",networks[current_network].ssid);
  M5.Lcd.setCursor(5,50);
  M5.Lcd.printf("Chan: %d",networks[current_network].channel);
  M5.Lcd.setCursor(5,70);
  M5.Lcd.printf("Rssi: %d",networks[current_network].rssi);
  M5.Lcd.setCursor(5,90);
  M5.Lcd.printf("Encr: %d",networks[current_network].encryption);
  M5.Lcd.setCursor(5,110);
  M5.Lcd.printf("Network %d of %d", networks[current_network].network_number, networks_found);
  return;
}

void do_scan() {
  int n,i,j,matched,stored_network_count=0;
  M5.Lcd.clear();
  draw_button_menu("           Stop   ",RED);
  while(1) {
    M5.update();
    if(M5.BtnB.isPressed()) {
      if(networks_found==0) {
        M5.Lcd.clear();
        draw_button_menu("   Scan",YELLOW);
        M5.Lcd.setCursor(5,90);
        M5.Lcd.setTextColor(RED);
        M5.Lcd.print("No Networks found"); 
        return;
      }
      else {
        networks_found--;
        M5.Lcd.clear();
        draw_button_menu("   Scan    Scroll   +",YELLOW);
        current_network = 0;
        display_network(0);
        return;
      }
   }
   else {
    n = WiFi.scanNetworks();
    if(n>0) {
      for (i = 0; i < n; ++i) {
        matched=0;
        for(j=0;j<networks_found;j++) {
          if(strcmp(WiFi.BSSIDstr(i).c_str() , networks[j].bssid_str) == 0) {
            matched=1;
            break;
          }
        }
        if(matched==0) {
          strcpy(networks[networks_found].bssid_str, WiFi.BSSIDstr(i).c_str());
          networks[networks_found].channel = WiFi.channel(i);
          strcpy(networks[networks_found].ssid, WiFi.SSID(i).c_str());
          networks[networks_found].rssi = WiFi.RSSI(i); 
          networks[networks_found].encryption = WiFi.encryptionType(i);
          networks[networks_found].network_number=networks_found;
          Serial.printf("%d:\t%s %d %s %d %d\n",
              networks[networks_found].network_number, 
              networks[networks_found].bssid_str,
              networks[networks_found].channel,
              networks[networks_found].ssid,
              networks[networks_found].rssi,
              networks[networks_found].encryption);
          networks_found++;
        }
      }
      if(networks_found !=stored_network_count) {
        M5.Lcd.clear();
        draw_button_menu("           Stop   ",RED);
        M5.Lcd.setCursor(5,30);
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.printf("Found %d Networks",networks_found-1);
        stored_network_count = networks_found;
      }
    }
  }
  delay(100);
  }
}
void focus_on_network(char *bssid) {
  int i,n;
  while(1) {
    M5.update();
    if(M5.BtnB.isPressed()) {
        M5.Lcd.clear();
        draw_button_menu("   Scan    Scroll   +",YELLOW);
        display_network(current_network);
        return;
    }
    else {
      n = WiFi.scanNetworks();
      if(n>0) {
        for (i = 0; i < n; ++i) {
          if(strcmp(bssid, WiFi.BSSIDstr(i).c_str())==0) {
            M5.Lcd.clear();
            draw_button_menu("           Back   ",GREEN);
            M5.Lcd.setTextColor(GREEN);
            M5.Lcd.setCursor(5, 10);
            M5.Lcd.printf("%s",networks[current_network].bssid_str);
            M5.Lcd.setCursor(5,30);
            M5.Lcd.printf("%s",networks[current_network].ssid);
            M5.Lcd.setCursor(5,50);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.printf("Rssi: %d", WiFi.RSSI(i));
          }
        }
      }
    }
  }
}
