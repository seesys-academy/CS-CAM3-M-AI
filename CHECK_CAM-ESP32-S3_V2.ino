#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "esp_http_server.h"

const char* ssid = "ESP32-S3-CAM-AP";
const char* password = "12345678";

WebServer server(80);
httpd_handle_t stream_httpd = NULL;

//========== PROTOTYPES ==========
void connectToWifi();
void setupServer();
void sendResponse();
void startCamera();
void startCameraServer();
esp_err_t stream_handler(httpd_req_t *req);

//================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // בדיקת חומרה – אמור להופיע ~8MB PSRAM ו-16MB Flash
  Serial.printf("Chip: %s rev %d | Flash: %u bytes | PSRAM: %u bytes\n",
                ESP.getChipModel(), ESP.getChipRevision(),
                ESP.getFlashChipSize(), ESP.getPsramSize());

  connectToWifi();
  startCamera();

  setupServer();
  server.begin();

  Serial.println("System Ready");
}

void loop() {
  server.handleClient();
}
//============================================