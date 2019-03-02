// ******************************************************************
// Program: WiFi scanner
// Platform: M5Stack (Fire)
// Author: WR
// Description:
// Use the 2.4Ghz Wifi chip to scan for WiFi networks.
// If SD card present, appends records to ./networks.txt.
// After a reset, allows for ./networks.txt file to be output 
// to Serial. File is then deleted.
//
// 1. For the first 1000 networks (Or until scanning is stopped
//    AND the unit reset), networks with the same BSSID are
//    deduped. RSSI is not updated for duplicates.
// 2. If over 1000 network are found, counters are reset and
//    recording continues untill the next 1000 networks have
//    been found. There may be duplicates between each 1000
//    record block.
// 3. The option to dump the networks.txt file is only available
//    when the unit is first turned on (This equates to no 
//    networks found since last turned on - I.E Scan button
//    not been pressed.
// 4. If scanning is stoped, you can scroll through the list of
//    found networks (Scroll goes down, then wraps back to the
//    first record). 
// 5. When scanning is dropped, the currently displayed network
//    RSSI can be monitored by pressing the '+' button.
// 6. Scanning is quite fast, but slows down as more networks 
//    are found. This is because of the dedup process.
//
// ********************************************************************


#include <WiFi.h>
#include <M5Stack.h>
#define MAX_NETWORKS 1300
#define LOGFILE "/networks.txt"
typedef struct {
    int channel;
    char bssid_str[20];
    char ssid[25];
    int rssi;
    int encryption;
    int network_number;
} network;
network networks[MAX_NETWORKS];
char outfile[]=LOGFILE;
int scan;
int networks_found;
int current_network;
File logfile;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.clear();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  networks_found=0;
  current_network=0;
  draw_button_menu("   Scan            Dump",YELLOW);
  delay(100);
}

void loop() {
  M5.update();
  if(M5.BtnA.isPressed()) {
    do_scan();
  }
  if(M5.BtnB.isPressed()) {
    M5.Lcd.clear();
    if(networks_found==0) {
      M5.Lcd.setCursor(5,90);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.print("No Networks found");
      draw_button_menu("   Scan",YELLOW);
    }
    else {
      if(current_network==networks_found) current_network=0;
      else current_network++;
      display_network(current_network);
    }
  }
  if(M5.BtnC.isPressed()) {
    if(networks_found==0) {
      dump_log();
      M5.Lcd.clear();    
      M5.Lcd.setCursor(5,90);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.print("No Networks found");
      draw_button_menu("   Scan",YELLOW);

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
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.printf("%s",networks[current_network].bssid_str);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(5,30);
  M5.Lcd.printf("%s",networks[current_network].ssid);
  M5.Lcd.setCursor(5,50);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("Chan: %d Rssi: %d",networks[current_network].channel,networks[current_network].rssi+100);
  M5.Lcd.setCursor(5,70);
  switch (networks[current_network].encryption) {
    case 1: {
      M5.Lcd.printf("WEP");
      break;
    }
    case 2: {
      M5.Lcd.printf("WPA PSK TKIP");
      break;
    }
    case 3: {
      M5.Lcd.printf("WPA PSK CCMP");
      break;
    }
    case 4: {
      M5.Lcd.printf("WPA2 PSK Mixed CCMP");
      break;
    }
    case 5: {
      M5.Lcd.printf("PEAP");
      break;
    }
    case 8: {
      M5.Lcd.printf("WPA WPA2 PSK");
      break;
    }
    case 0: {
      M5.Lcd.printf("Open");
      break;
    }
    default : {
      M5.Lcd.printf("Unknown");
      break;
    }
  }
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(5,160);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("%d/%d", networks[current_network].network_number+1, networks_found+1);
  M5.Lcd.setTextSize(2);
  return;
}

void do_scan() {
int n,i,j,k,m,matched;
  logfile=SD.open(outfile,FILE_APPEND);
  display_networks_found(networks_found,WHITE);
  while(1) {
      if(stopB_button()) {
        logfile.close();
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
          current_network = 0;
          display_network(current_network);
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
            if(networks_found == MAX_NETWORKS-1) networks_found=0;
            strcpy(networks[networks_found].bssid_str, WiFi.BSSIDstr(i).c_str());
            networks[networks_found].channel = WiFi.channel(i);
            k=strlen(WiFi.SSID(i).c_str());
            if(k>24) {
              memcpy(networks[networks_found].ssid,WiFi.SSID(i).c_str(),24);
              networks[networks_found].ssid[24]='\0';
            }
            else strcpy(networks[networks_found].ssid, WiFi.SSID(i).c_str());
            networks[networks_found].rssi = WiFi.RSSI(i); 
            networks[networks_found].encryption = WiFi.encryptionType(i);
            networks[networks_found].network_number=networks_found;
            if(logfile) {
              logfile.printf("%d,%s,%d,%s,%d,%d\n",networks[networks_found].network_number, 
              networks[networks_found].bssid_str,networks[networks_found].channel,
              networks[networks_found].ssid,networks[networks_found].rssi,
              networks[networks_found].encryption);
            }
            display_networks_found(networks_found,WHITE);
            networks_found++;
          }
        }
      }
    }
  }
}
void focus_on_network(char *bssid) {
  int i,n;
  while(1) {
    if(stopB_button()) {
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
            M5.Lcd.setCursor(125,100);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(5);
            M5.Lcd.printf("%d", WiFi.RSSI(i)+100);
            M5.Lcd.setTextSize(2);
          }
        }
      }
    }
  }
}
void display_networks_found(int count, int colour) {
          M5.Lcd.clear();
          draw_button_menu("           Stop   ",RED);
          M5.Lcd.setTextSize(3);
          M5.Lcd.setCursor(30,30);
          M5.Lcd.setTextColor(GREEN);
          M5.Lcd.printf("Networks found");
          M5.Lcd.drawLine(1,70,320,70,WHITE); 
          M5.Lcd.setCursor(120,130);
          M5.Lcd.setTextSize(5);
          M5.Lcd.setTextColor(colour);
          M5.Lcd.printf("%d",count);
          M5.Lcd.setTextSize(2);
}
int stopB_button() {
      M5.update();
      if(M5.BtnB.isPressed()) {
        M5.Lcd.clear();
        M5.Lcd.setCursor(50,90);
        M5.Lcd.setTextColor(RED);
        M5.Lcd.setTextSize(5);
        M5.Lcd.print("STOPPED");
        M5.Lcd.setTextSize(2);
        while(M5.BtnB.isPressed()) M5.update();
        return 1;
      }
      return 0;
}
void dump_log() {
      logfile=SD.open(outfile,FILE_READ);
      M5.Lcd.clear();    
      M5.Lcd.setCursor(5,90);
      M5.Lcd.setTextColor(RED);
      if(logfile) {
        M5.Lcd.print("Dumping to serial");
        while(logfile.available()) Serial.write(logfile.read());
        logfile.close();
        SD.remove(LOGFILE);
      }
      else {
        M5.Lcd.print("No file present");
        delay(2000);
      }
      return;
}

