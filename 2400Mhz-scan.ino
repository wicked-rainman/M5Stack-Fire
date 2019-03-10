#include <WiFi.h>
#include <M5Stack.h>
#define MAX_NETS 310
#define LOGFILE "/networks.txt"
typedef struct {
    int channel;
    char bssid_str[20];
    char ssid[25];
    int rssi;
    char encryption[25];
    int network_number;
} network;

network networks[MAX_NETS];
char outfile[]=LOGFILE;
int scan;
int networks_found;
int current_network;
int this_network;
int wifi_networks;
File logfile;

void setup() {
  M5.begin();
  M5.Lcd.clear();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  networks_found=0;
  current_network=0;
  this_network=0;
  wifi_networks=0;
  M5.Lcd.clear();
  draw_box(0,0,319,200,GREEN);
  M5.Lcd.drawLine(0,140,319,140,GREEN);
  M5.Lcd.drawLine(0,40,319,40,GREEN);
  M5.Lcd.setTextSize(2);
  if(SD.exists(LOGFILE)) draw_buttons("SCAN",WHITE," ",BLACK,"DUMP",WHITE,BLACK);
  else draw_buttons("SCAN",WHITE," ",BLACK," ",BLACK,BLACK);
  update_screen("00:00:00:00:00;00"," "," ",0,0,0,0);

}

void loop() {
  M5.update();
  if(M5.BtnA.isPressed()) {
    if(SD.exists(LOGFILE)) draw_buttons("SCAN",RED," ",BLACK,"DUMP",WHITE,BLACK);
    else draw_buttons("SCAN",RED," ",BLACK," ",BLACK,BLACK);
    while(M5.BtnA.isPressed()) M5.update();
    draw_buttons("STOP",WHITE," ",BLACK," ",BLACK,BLACK);
    do_scan();
  }
  if(M5.BtnB.isPressed()) {
    draw_buttons("SCAN",WHITE,"  +",RED,"DUMP",WHITE,BLACK);
    while(M5.BtnB.isPressed()) M5.update();
    draw_buttons("BACK",WHITE," ",BLACK," ",BLACK,BLACK);
    focus_on_network();
    
  }
  if(M5.BtnC.isPressed()) {
    draw_buttons(" ",BLACK," ",BLACK,"DUMP",RED,BLACK);
    while(M5.BtnC.isPressed())M5.update();
    if(SD.exists(LOGFILE)) {
      dump_log();
    }
    networks_found=0;
    current_network=0;
    wifi_networks=0;
    update_screen("00:00:00:00:00;00"," "," ",0,0,0,0);
    draw_buttons("SCAN",WHITE," ",BLACK," ",BLACK,BLACK);
  }
  delay(100);
}

void dump_log() {
      logfile=SD.open(outfile,FILE_READ);
      if(logfile) {
        while(logfile.available()) Serial.write(logfile.read());
        logfile.close();
      }
      SD.remove(LOGFILE);
      return;
}


void show_individual_network() {
  this_network=networks_found-1;
  draw_buttons("BACK",WHITE,"<<",WHITE,">>",WHITE,BLACK);
  while(1) {
    M5.update();
    if(M5.BtnA.isPressed()) {
      draw_buttons("BACK",RED,"<<",WHITE,">>",WHITE,BLACK);
      while(M5.BtnA.isPressed()) M5.update();
      draw_buttons("SCAN",WHITE,"  +",WHITE,"DUMP",WHITE,BLACK);
      return;
    }
    if(M5.BtnB.isPressed()) {
      draw_buttons("BACK",WHITE,"<<",RED,">>",WHITE,BLACK);
      while(M5.BtnB.isPressed()) M5.update();
      draw_buttons("BACK",WHITE,"<<",WHITE,">>",WHITE,BLACK);
      Serial.printf("this_network=%d, networks_found=%d\n",this_network,networks_found);
      if (this_network==0) this_network=networks_found-1;
      else this_network--;
      update_screen(networks[this_network].bssid_str,
                        networks[this_network].ssid,
                        networks[this_network].encryption,
                        networks[this_network].rssi,
                        networks[this_network].channel,
                        this_network,wifi_networks);
    }
    if(M5.BtnC.isPressed()) {
      draw_buttons("BACK",WHITE,"<<",WHITE,">>",RED,BLACK);
      while(M5.BtnC.isPressed()) M5.update();
      draw_buttons("BACK",WHITE,"<<",WHITE,">>",WHITE,BLACK);
      if (this_network>=networks_found-1) this_network=0;
      else this_network++;
      update_screen(networks[this_network].bssid_str,
                        networks[this_network].ssid,
                        networks[this_network].encryption,
                        networks[this_network].rssi,
                        networks[this_network].channel,
                        this_network,wifi_networks);
    }
  }
}
void do_scan() {
  int i,j,k,l,matched;
  logfile=SD.open(outfile,FILE_APPEND);
  while(1) {
    M5.update();
    if(M5.BtnA.isPressed()) {
      draw_buttons("STOP",RED,"<<",WHITE,">>",WHITE,BLACK);
      while(M5.BtnA.isPressed()) M5.update();  
      show_individual_network();
      logfile.close();
      return;
    }
    wifi_networks = WiFi.scanNetworks(false,true,true,300);
    l=WiFi.scanComplete();
        while(l<=0) {
          Serial.printf("%d %d\n",l, wifi_networks);
          delay(1000);
        }

    if(wifi_networks>0) {
      for (i = 0; i < wifi_networks; ++i) {
        matched=0;
        for(j=0;j<networks_found;j++) {
          if(strcmp(WiFi.BSSIDstr(i).c_str() , networks[j].bssid_str) == 0) {
            matched=1;
            break;
          }   
        }
        if(matched==0) {
          if(networks_found == MAX_NETS-1) {
            networks_found=0;
            current_network=0;
          }
          strcpy(networks[networks_found].bssid_str, WiFi.BSSIDstr(i).c_str());
          networks[networks_found].channel = WiFi.channel(i);
          k=strlen(WiFi.SSID(i).c_str()); 
          if(k>24) {
            memcpy(networks[networks_found].ssid,WiFi.SSID(i).c_str(),24);
            networks[networks_found].ssid[24]='\0';
          }
          else strcpy(networks[networks_found].ssid, WiFi.SSID(i).c_str());
          networks[networks_found].rssi = WiFi.RSSI(i);
          switch (WiFi.encryptionType(i)) {
            case 1: {
              strcpy(networks[networks_found].encryption,"WEP");
              break;
            }
            case 2: {
              strcpy(networks[networks_found].encryption,"WPA-PSK-TKIP");
              break;
            }
            case 3: {
              strcpy(networks[networks_found].encryption,"WPA-PSK-CCMP");
              break;
            }
            case 4: {
              strcpy(networks[networks_found].encryption,"WPA2-PSK-Mixed-CCMP");
              break;
            }
            case 5: {
              strcpy(networks[networks_found].encryption,"PEAP");
              break;
            }
            case 8: {
              strcpy(networks[networks_found].encryption,"WPA-WPA2-PSK");
              break;
            }
            case 0: {
              strcpy(networks[networks_found].encryption,"Open");
              break;
            }
            default : {
              strcpy(networks[networks_found].encryption,"Unknown");
              break;
            }
          }
          networks[networks_found].network_number=networks_found;
          update_screen(networks[networks_found].bssid_str,
                        networks[networks_found].ssid,
                        networks[networks_found].encryption,
                        networks[networks_found].rssi,
                        networks[networks_found].channel,
                        networks_found,wifi_networks);
          if(logfile) {
              logfile.printf("%d,%s,%d,%s,%d,%s\n",networks[networks_found].network_number, 
              networks[networks_found].bssid_str,networks[networks_found].channel,
              WiFi.SSID(i).c_str(),networks[networks_found].rssi,
              networks[networks_found].encryption);
          }
          networks_found++;
      }
    }
  }
  }
}
void update_screen(char *mac, char *ssid, char *encryption, int rssi, int chan, int count, int nw_count) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.fillRect(8, 10, 210, 22, BLACK);
  M5.Lcd.setCursor(10,13);
  M5.Lcd.print(mac);
  M5.Lcd.fillRect(10, 50, 300, 20, BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(10, 50);
  M5.Lcd.print(ssid);
  M5.Lcd.fillRect(10, 80, 300, 20, BLACK);
  M5.Lcd.setCursor(10, 80);
  M5.Lcd.print(encryption);
  M5.Lcd.fillRect(70, 107, 55, 20, BLACK);
  M5.Lcd.setCursor(10,110);
  M5.Lcd.printf("CHAN: %d",chan);
  M5.Lcd.setCursor(130,110) ;  
  M5.Lcd.fillRect(190, 107, 70, 20, BLACK);
  M5.Lcd.printf("RSSI: %d",rssi);
  M5.Lcd.fillRect(240,10,70,22,BLACK);
  M5.Lcd.setCursor(265,13);
  //M5.Lcd.setCursor(265,174);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("%3d",count);
  M5.Lcd.fillRect(5,145,310,50,BLACK);
  M5.Lcd.fillRect(5,145,count,50,GREEN);
  M5.Lcd.fillRect(5,145,nw_count,50,RED);


}

void draw_buttons(char *btna, int cola, char* btnb, int colb, char* btnc, int colc, int border) {
  M5.Lcd.fillRect(5,205,310,28,BLACK);
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

void focus_on_network() {
  int i,n,x;
  M5.Lcd.setTextColor(GREEN);
  Serial.printf("This=%d %s %s\n",this_network, networks[this_network].bssid_str, networks[this_network].ssid);
  //New start
  //while(1) {
  //  Serial.printf("RSSI=%d\n",WiFi.RSSI(this_network));
  //  delay(1000);
  //}
  //New end
  while(1) {
      M5.update();
      if(M5.BtnA.isPressed()) {
        draw_buttons("BACK",RED," ",BLACK," ",BLACK,BLACK);
        while(M5.BtnA.isPressed()) M5.update();
        draw_buttons("SCAN",WHITE," ",BLACK,"DUMP",WHITE,BLACK);
        //WiFi.disconnect();
        return;
      }
      n = WiFi.scanNetworks(false,true,true,100);
      if(n>0) {
        for (i = 0; i < n; ++i) {
          if(strcmp(networks[this_network].bssid_str, WiFi.BSSIDstr(i).c_str())==0) {
            //Serial.printf("%s, %d\n",WiFi.BSSIDstr(i).c_str(),WiFi.RSSI(i));
            M5.Lcd.setCursor(130,110) ;  
            M5.Lcd.fillRect(190, 107, 70, 20, BLACK);
            M5.Lcd.printf("RSSI: %d",WiFi.RSSI(i));
            x=(100+WiFi.RSSI(i))*3;
            M5.Lcd.fillRect(x,145,(310-x),50,BLACK);
            M5.Lcd.fillRect(5,145,x,50,WHITE);
          }
        }
      }
    }
  }
