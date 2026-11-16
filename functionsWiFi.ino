//============== functionsWiFi =============
void connectToWifi() {
  Serial.println();
  Serial.println("Starting WiFi Access Point...");
  Serial.print("AP SSID: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.setSleep(false);   // ביטול חיסכון בחשמל של ה-Wi-Fi – פחות השהיה

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP Address: ");
  Serial.println(ip);
}
//=========================================