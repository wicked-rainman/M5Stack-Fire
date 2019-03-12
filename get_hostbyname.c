void get_hostbyname(char *hostname, char *ip) {
  IPAddress address;
  WiFi.hostByName(hostname, address);
  strcpy(ip,address.toString().c_str());
}
