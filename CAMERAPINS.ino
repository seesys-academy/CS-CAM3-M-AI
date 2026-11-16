//================== CAMERA_MODEL_ESP32S3_EYE ==================
// מיפוי פינים מקובל ללוחות ESP32-S3-CAM (מודול N16R8 + OV3660)
#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5

#define Y2_GPIO_NUM     11
#define Y3_GPIO_NUM      9
#define Y4_GPIO_NUM      8
#define Y5_GPIO_NUM     10
#define Y6_GPIO_NUM     12
#define Y7_GPIO_NUM     18
#define Y8_GPIO_NUM     17
#define Y9_GPIO_NUM     16

#define VSYNC_GPIO_NUM   6
#define HREF_GPIO_NUM    7
#define PCLK_GPIO_NUM   13

//==============================================================
void startCamera() {

  // [1] איפוס המבנה – בלי זה שדות שלא נקבעו מכילים ערכים אקראיים
  camera_config_t config = {};

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk  = XCLK_GPIO_NUM;
  config.pin_pclk  = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href  = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn  = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;   // [8] 20MHz – הערך היציב ל-OV3660
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    Serial.printf("PSRAM found: %u bytes, free: %u bytes\n",
                  ESP.getPsramSize(), ESP.getFreePsram());
    config.fb_location  = CAMERA_FB_IN_PSRAM;   // [1] buffers ב-PSRAM
    // מאתחלים ברזולוציה המקסימלית כדי שה-buffers יהיו גדולים מספיק
    // לכל רזולוציה. אחרי האתחול יורדים לרזולוציית השידור.
    config.frame_size   = FRAMESIZE_QXGA;       // 2048x1536
    config.jpeg_quality = 12;                   // נמוך = איכות גבוהה
    config.fb_count     = 2;                    // 2 מספיקים עם GRAB_LATEST
    config.grab_mode    = CAMERA_GRAB_LATEST;
  } else {
    Serial.println("PSRAM NOT found!  Check: Tools > PSRAM > OPI PSRAM");
    config.fb_location  = CAMERA_FB_IN_DRAM;
    config.frame_size   = FRAMESIZE_QVGA;       // 320x240
    config.jpeg_quality = 18;
    config.fb_count     = 1;
    config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    // [4] זיהוי החיישן בפועל
    Serial.printf("Sensor PID: 0x%x\n", s->id.PID);
    if (s->id.PID == OV3660_PID) {
      Serial.println("Sensor: OV3660 detected");
    } else {
      Serial.println("WARNING: sensor is NOT OV3660");
    }

    // [6] ניגודיות וחדות – נקודת התחלה ניטרלית
    s->set_contrast(s, 0);
    s->set_sharpness(s, 0);

    // אוטומטיקה בסיסית
    s->set_gain_ctrl(s, 1);
    s->set_exposure_ctrl(s, 1);
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);

    // תיקון פיקסלים שחורים/לבנים
    s->set_bpc(s, 1);
    s->set_wpc(s, 1);

    // [5] הגדרות תמונה מתקדמות
    s->set_lenc(s, 1);                       // תיקון החשכה בפינות
    s->set_raw_gma(s, 1);                    // גמא – טונים טבעיים
    s->set_dcw(s, 1);                        // הקטנה איכותית
    // AEC2 (DSP) מאט את תגובת החשיפה – כבוי לתגובה מהירה
    s->set_aec2(s, 0);
    s->set_ae_level(s, 0);                   // רמת חשיפה (-2..2)
    // הגבר גבוה יותר = חשיפה קצרה יותר = פחות מריחה בתנועה ויותר FPS
    // (מחיר: מעט יותר רעש באור חלש)
    s->set_gainceiling(s, GAINCEILING_16X);

    // [4] התאמה ל-OV3660: בהירות למעלה, רוויה למטה
    if (s->id.PID == OV3660_PID) {
      s->set_brightness(s, 1);
      s->set_saturation(s, -2);
    }

    // [7] כיוון תמונה – שני אלה ביחד = סיבוב 180°.
    // אם התמונה הפוכה/ראי, שנה אחד מהם ל-0.
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);

    // ברירת מחדל לשידור (נבדק על הלוח): HD 1280x720, איכות JPEG 10.
    // אפשר להחליף מדף האינטרנט בלי לצרוב מחדש.
    if (psramFound()) {
      s->set_framesize(s, FRAMESIZE_HD);
      s->set_quality(s, 10);
    }
  }

  startCameraServer();
}

//==============================================================
esp_err_t stream_handler(httpd_req_t *req) {

  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char part[96];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=frame");
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  // גבול ראשון נשלח פעם אחת. אחרי כל פריים נשלח הגבול הבא מיד,
  // כך שהדפדפן יודע שהפריים הסתיים ומציג אותו בלי לחכות לפריים הבא.
  res = httpd_resp_send_chunk(req, "--frame\r\n", 9);

  // מדידות – מודפסות ל-Serial כל 5 שניות
  uint32_t frames = 0, t0 = millis();
  uint32_t capSum = 0, sendSum = 0;
  size_t   lenSum = 0;

  while (res == ESP_OK) {

    uint32_t tc = millis();
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Capture failed");
      res = ESP_FAIL;
      break;
    }
    uint32_t ts = millis();
    capSum += ts - tc;

    // כותרת החלק כולל Content-Length – הדפדפן יודע בדיוק מתי הפריים שלם
    int hlen = snprintf(part, sizeof(part),
                        "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                        (unsigned)fb->len);
    res = httpd_resp_send_chunk(req, part, hlen);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, "\r\n--frame\r\n", 11);

    lenSum += fb->len;
    esp_camera_fb_return(fb);
    sendSum += millis() - ts;

    frames++;
    uint32_t dt = millis() - t0;
    if (dt >= 5000) {
      Serial.printf("FPS: %.1f | avg frame: %u KB | capture: %u ms | send: %u ms | free PSRAM: %u\n",
                    frames * 1000.0f / dt,
                    (unsigned)(lenSum / frames / 1024),
                    (unsigned)(capSum / frames),
                    (unsigned)(sendSum / frames),
                    ESP.getFreePsram());
      frames = 0; capSum = 0; sendSum = 0; lenSum = 0;
      t0 = millis();
    }
  }

  return res;
}

//==============================================================
void startCameraServer() {

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 81;
  config.ctrl_port   = 32769;

  httpd_uri_t stream_uri = {
    .uri      = "/stream",
    .method   = HTTP_GET,
    .handler  = stream_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    Serial.println("Camera Stream Started");
  } else {
    Serial.println("Stream Failed");
  }
}
