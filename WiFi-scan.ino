#include <TinyGPS++.h>
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
int log_record_count=1;
File logfile;
HardwareSerial ss(2);
TinyGPSPlus gps;
bool use_gps=false;

// **********************************************************************
// Setup:
// **********************************************************************

void setup() {
  M5.begin();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  networks_found=0;
  current_network=0;
  this_network=0;
  wifi_networks=0;
  M5.Lcd.clear();
  // **********************************************************************
  // Decide if GPS is going to be used (Pins 13 and 5)
  // **********************************************************************
  draw_buttons("YES",WHITE,"NO",WHITE," ",BLACK,BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(80,100);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print("Use GPS ?");
  while(1) {
    M5.update();
    if(M5.BtnA.isPressed()) {
      // **********************************************************************
      // GPS Fix wanted. Wait for a lock on.
      // **********************************************************************
      M5.Lcd.clear();
      draw_buttons("YES",RED," ",BLACK," ",BLACK,BLACK);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setCursor(50,100);
      M5.Lcd.print("WAITING FOR GPS");
      while(M5.BtnA.isPressed()) M5.update();
      ss.begin(9600,SERIAL_8N1,13,5);
      get_gps_lock();
      use_gps=true;
      break;
    }
    if(M5.BtnB.isPressed()) {
      // **********************************************************************
      // GPS Fix not wanted
      // **********************************************************************
      draw_buttons("YES",WHITE,"NO",RED," ",BLACK,BLACK);
      while(M5.BtnB.isPressed()) M5.update();
      use_gps=false;
      break;
    }
  }
  // **********************************************************************
  // Decorate the screen ready for scanning.
  // If a logfile is already present, provide the option to dump it out
  // as serial data using the CP2104.
  // **********************************************************************
  M5.Lcd.clear();
  draw_box(0,0,319,200,GREEN);
  M5.Lcd.drawLine(0,140,319,140,GREEN);
  M5.Lcd.drawLine(0,40,319,40,GREEN);
  M5.Lcd.setTextSize(2);
  if(SD.exists(LOGFILE)) draw_buttons("SCAN",WHITE," ",BLACK,"DUMP",WHITE,BLACK);
  else draw_buttons("SCAN",WHITE," ",BLACK," ",BLACK,BLACK);
  update_screen("00:00:00:00:00;00"," "," ",0,0,0,0);

}


// **********************************************************************
// Main loop.
// **********************************************************************
void loop() {
  M5.update();
  // **********************************************************************
  // If button A is pressed, start to do the WiFi scan.
  // After pressing button A, the only options at this level is to stop.
  // **********************************************************************
  if(M5.BtnA.isPressed()) {
    if(SD.exists(LOGFILE)) draw_buttons("SCAN",RED," ",BLACK,"DUMP",WHITE,BLACK);
    else draw_buttons("SCAN",RED," ",BLACK," ",BLACK,BLACK);
    while(M5.BtnA.isPressed()) M5.update();
    draw_buttons("STOP",WHITE," ",BLACK," ",BLACK,BLACK);
    do_scan();
  }
  // **********************************************************************
  // If button B is pressed, control is passed to routine focus_on_network
  // which allows you to monitor the RSSI value for the current network.
  // After a press of this button at this level, the only option is to
  // return back.
  // **********************************************************************
  if(M5.BtnB.isPressed()) {
    draw_buttons("SCAN",WHITE,"  +",RED,"DUMP",WHITE,BLACK);
    while(M5.BtnB.isPressed()) M5.update();
    draw_buttons("BACK",WHITE," ",BLACK," ",BLACK,BLACK);
    focus_on_network();
    
  }
  // **********************************************************************
  // If button C is pressed, then the log file is dumped (if available).
  // After pressing button C, the only option at this level is to 
  // perform another scan by pressing button A. All variables that
  // relate to the memory stored networks are reset. 
  // **********************************************************************
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

// **********************************************************************
// Function dump log. Callable when scanning has been stopped.
// Returns back to the main button loop with the option of scanning again
// **********************************************************************
void dump_log() {
      logfile=SD.open(outfile,FILE_READ);
      if(logfile) {
        while(logfile.available()) Serial.write(logfile.read());
        logfile.close();
      }
      SD.remove(LOGFILE);
      return;
}
// **********************************************************************
// Function show_individual_network. Callable when scanning has been
// stopped, and invoked by a button B press.
// Allows you to scroll backwards and forwards through the list of 
// networks found. Exit route is via pressing the BACK button (c). 
// **********************************************************************
void show_individual_network() {
  this_network=networks_found-1;
  draw_buttons("BACK",WHITE,"<<",WHITE,">>",WHITE,BLACK);
  while(1) {
    M5.update();
    // **********************************************************************
    // Btn A press takes you back to scanning, dumping or focus on a network
    // **********************************************************************
    if(M5.BtnA.isPressed()) {
      draw_buttons("BACK",RED,"<<",WHITE,">>",WHITE,BLACK);
      while(M5.BtnA.isPressed()) M5.update();
      draw_buttons("SCAN",WHITE,"  +",WHITE,"DUMP",WHITE,BLACK);      
      return;
    }
    // **********************************************************************
    // Btn B press scrolls back one network
    // **********************************************************************
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
    // **********************************************************************
    // Btn C press scrolls forward one network
    // **********************************************************************
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

// **********************************************************************
// Main WiFi scanning routine
//
// Updates an array of struct network records. The array is used to enable
// the dedupe of bssid's. Once MAX_NETS has been reached, the index to
// the struct array is re-set to zero, so duplicate networks can be seen
// across individual blocks of MAX_NETS. The number of duplications that
// will occur is based round factors that relate to the number of networks 
// found, travel speed and visiting the same geographic location.
//
// MAX_NETS value is currently set at an optimum. Setting the value 
// much higher will mean the dedupe process might take more time than the
// scanning fuctionality, which may mean records of new networks will
// be lost. Go figure...... 
// **********************************************************************
void do_scan() {
  int i,j,k,l,matched;
  logfile=SD.open(outfile,FILE_APPEND);
  while(1) {
    M5.update();
    // **********************************************************************
    // Btn A press stops the current scan and branches off to
    // show_individual_network 
    // **********************************************************************
    if(M5.BtnA.isPressed()) {
      draw_buttons("STOP",RED,"<<",WHITE,">>",WHITE,BLACK);
      while(M5.BtnA.isPressed()) M5.update();  
      show_individual_network();
      logfile.close();
      return;
    }
    // **********************************************************************
    // Do a scan. Number of networks found is returned.
    // **********************************************************************
    wifi_networks = WiFi.scanNetworks(false,true,true,300);
    if(wifi_networks>0) {
      // **********************************************************************
      // If GPS lock requried, get the lock.
      // **********************************************************************
      // **********************************************************************
      // Go through each found network and only add it if it's not been seen
      // before
      // **********************************************************************
      for (i = 0; i < wifi_networks; ++i) {
        matched=0;
        for(j=0;j<networks_found;j++) {
          if(strcmp(WiFi.BSSIDstr(i).c_str() , networks[j].bssid_str) == 0) {
            matched=1;
            break;
          }   
        }
        // **********************************************************************
        // If stored network count is exceeded (MAX_NETS), reset to zero.
        // **********************************************************************
        if(matched==0) {
          if(networks_found == MAX_NETS-1) {
            networks_found=0;
            current_network=0;
          }
          // **********************************************************************
          // Extract all the network details for this network.
          // **********************************************************************
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
          // **********************************************************************
          // Update the screen with details of the current network
          // **********************************************************************
          update_screen(networks[networks_found].bssid_str,
                        networks[networks_found].ssid,
                        networks[networks_found].encryption,
                        networks[networks_found].rssi,
                        networks[networks_found].channel,
                        networks_found,wifi_networks);
          // **********************************************************************
          // Log the data with or without the GPS fix.
          // **********************************************************************
          if(logfile) {
              if(use_gps) {
                get_gps_lock();
                logfile.printf("<Placemark>\n");
                logfile.printf("<name>%s</name>\n",WiFi.SSID(i).c_str());
                logfile.printf("<description>\n<![CDATA[%s<p>%d/%d/%d %d:%d:%d<p>%s %dDb,%d ]]>\n</description>\n",
                        networks[networks_found].bssid_str,
                        gps.date.day(),
                        gps.date.month(),
                        gps.date.year(),
                        gps.time.hour(),
                        gps.time.minute(),
                        gps.time.second(),
                        networks[networks_found].encryption,
                        networks[networks_found].rssi,
                        networks[networks_found].channel);
                logfile.printf("<Point id=\"%d\">\n<coordinates>%f,%f,0</coordinates>\n",log_record_count,gps.location.lng(),gps.location.lat());
                log_record_count++;
                logfile.printf("</Point>\n</Placemark>\n");
                
                //logfile.printf("%d,%d/%d/%d-%d:%d:%d,%f,%f,%d,%s,%d,%s,%d,%s\n",
                //  networks_found,
                //  gps.date.day(),
                //  gps.date.month(),
                //  gps.date.year(),
                //  gps.time.hour(),
                //  gps.time.minute(),
                //  gps.time.second(),
                //  gps.location.lat(),
                //  gps.location.lng(),
                //  networks[networks_found].network_number, 
                //  networks[networks_found].bssid_str,
                //  networks[networks_found].channel,
                //  WiFi.SSID(i).c_str(),
                //  networks[networks_found].rssi,
                //  networks[networks_found].encryption);
                  
                Serial.printf("%d,%d/%d/%d-%d:%d:%d,%f,%f,%s,%s,%s,%d,%d\n",
                  networks_found,
                  gps.date.day(),
                  gps.date.month(),
                  gps.date.year(),
                  gps.time.hour(),
                  gps.time.minute(),
                  gps.time.second(),
                  gps.location.lat(),
                  gps.location.lng(),
                  networks[networks_found].bssid_str,
                  networks[networks_found].ssid,
                  networks[networks_found].encryption,
                  networks[networks_found].rssi,
                  networks[networks_found].channel);
            }
            else {
              logfile.printf("%d,%d,%s,%d,%s,%d,%s\n",
                  networks_found,
                  networks[networks_found].network_number, 
                  networks[networks_found].bssid_str,
                  networks[networks_found].channel,
                  WiFi.SSID(i).c_str(),
                  networks[networks_found].rssi,
                  networks[networks_found].encryption);
                  
                Serial.printf("%d,%s,%s,%s,%d,%d\n",
                  networks_found,
                  networks[networks_found].bssid_str,
                  networks[networks_found].ssid,
                  networks[networks_found].encryption,
                  networks[networks_found].rssi,
                  networks[networks_found].channel);
            }
          }
          networks_found++;
        }
      }
    }
  }
}

// **********************************************************************
// Refresh the screen with the network data just found
// **********************************************************************
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

// **********************************************************************
// Draw up the menue buttons. Calls draw_text_box which in turn calls
// draw_box. Pretty dull stuff.....
// **********************************************************************
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
void draw_box(int startx, int starty, int endx, int endy, int colour) {
  M5.Lcd.drawLine(startx,starty, endx,starty,colour);
  M5.Lcd.drawLine(startx,starty, startx,endy,colour);
  M5.Lcd.drawLine(startx,endy,endx,endy,colour);
  M5.Lcd.drawLine(endx,starty,endx,endy,colour);
}
// **********************************************************************
// focus_on_network
// Provides a display of a network with constantly updated RSSI.
// Same functionality as doing a scan, but only accepts results that 
// relate to the chosen network. 
// Can't find a better way of doing this..... Ignorance probably.
// Exit is vai a button A press.
// **********************************************************************
void focus_on_network() {
  int i,n,x;
  M5.Lcd.setTextColor(GREEN);
  Serial.printf("This=%d %s %s\n",this_network, networks[this_network].bssid_str, networks[this_network].ssid);
  while(1) {
      M5.update();
      if(M5.BtnA.isPressed()) {
        draw_buttons("BACK",RED," ",BLACK," ",BLACK,BLACK);
        M5.Lcd.fillRect(5,145,310,50,BLACK);
        while(M5.BtnA.isPressed()) M5.update();          
        draw_buttons("SCAN",WHITE," ",BLACK,"DUMP",WHITE,BLACK);
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

// **********************************************************************
// get_gps_lock
// Keep reading the GPS serial connection until enough data has been
// read to provide date/time/fix information.
// **********************************************************************
void get_gps_lock() {
bool gps_loc=false, gps_date=false,gps_time=false;
    while((gps_loc==false) || (gps_date==false) || (gps_time==false))  {
        while(ss.available()>0) {
            if(gps.encode(ss.read())) {
                if(gps.location.isUpdated()) {
                  gps_loc=true;
                }
                if(gps.date.isUpdated()) {
                  gps_date=true;
                }
                if(gps.time.isUpdated()) {
                  gps_time=true;
                }
            }
        }
    }
}
