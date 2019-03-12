void get_network_mask(char * mask) {
  strcpy(mask, WiFi.subnetMask().toString().c_str());
}
