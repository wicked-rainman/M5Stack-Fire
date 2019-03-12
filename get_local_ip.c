void get_local_ip(char *ip) {
  strcpy(ip, WiFi.localIP().toString().c_str());
}
