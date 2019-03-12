void get_gateway_ip(char *gw) {
      strcpy(gw, WiFi.gatewayIP().toString().c_str());
}
