#include <M5Stack.h>
#include "WiFi.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void loop() {
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) Serial.println("Nothing found");
    else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(":\t");
            Serial.print(WiFi.channel(i));
            Serial.print("\t");
            Serial.print(WiFi.BSSIDstr(i));
            Serial.print(" ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" ");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" ");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");
    delay(5000);
}
