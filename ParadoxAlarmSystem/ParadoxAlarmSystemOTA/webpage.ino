


String getpage(){
String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  
  page += FPSTR(HTTP_HEAD_END);
  page += F("<dl>");
  page += F("<dt>Hostname</dt><dd>");
  page += Hostname;
  page += F("</dd>");
  page += F("<dt>Firmware</dt><dd>");
  page += F(firmware);
  page += F("</dd>");
  page += F("<dt>Hassio</dt><dd>");
  page += Hassio;
  page += F("</dd>");

   page += F("<dt>Homekit</dt><dd>");
  page += HomeKit;
  page += F("</dd>");
  
  page += F("<dt>Chip ID</dt><dd>");
  page += ESP.getChipId();
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
  page += ESP.getFlashChipId();
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  page += F("<dt>Real Flash Size</dt><dd>");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</dd>");
  page += F("<dt>Local IP</dt><dd>");
  page += WiFi.localIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  page += FPSTR(HTTP_END);
return page;

}
