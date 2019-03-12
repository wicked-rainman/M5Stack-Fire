int network_connect(char *ssid, char *password) {
    int y,k=0;
    WiFi.begin(ssid, password);
    delay(3000);
    y=WiFi.status();
    while ((y != WL_CONNECTED || y == WL_CONNECT_FAILED) && k<5) {
        if(y==WL_CONNECT_FAILED) {
          WiFi.begin(ssid, password);
          delay(3000);
        }
        else delay(1000);
        y=WiFi.status();
        k++;
    }
    if(y==WL_CONNECTED) return 1;
    return 0;
}
