void get_netmask(char * mask) {
  strcpy(mask, WiFi.subnetMask().toString().c_str());
}
