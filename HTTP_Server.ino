//================== HTTP_Server ====================
void sendResponse() {

  String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32-S3-CAM</title>

<style>
body{
  margin:0;
  padding:20px;
  text-align:center;
  font-family:Arial;
  background:#f2f2f2;
}

h1{
  font-size:28px;
}

.box{
  display:inline-block;
  width:95vw;
  max-width:1320px;
  box-sizing:border-box;
  background:white;
  padding:20px;
  border-radius:16px;
  box-shadow:0 4px 15px rgba(0,0,0,0.15);
}

img{
  width:100%;
  max-width:1280px;
  height:auto;
  border-radius:12px;
  border:2px solid #333;
}
.ctl{ margin-top:12px; }
button{
  margin:3px;
  padding:8px 12px;
  font-size:15px;
  border-radius:8px;
  border:1px solid #888;
  background:#fafafa;
  cursor:pointer;
}
#st{ margin-top:10px; color:#06c; min-height:20px; }
</style>

</head>

<body>
<h1>ESP32-S3-CAM Live</h1>

<div class="box">
  <img src="/cam">

  <div class="ctl">
    <b>Resolution:</b>
    <button onclick="setv('framesize','QVGA')">320x240</button>
    <button onclick="setv('framesize','VGA')">640x480</button>
    <button onclick="setv('framesize','SVGA')">800x600</button>
    <button onclick="setv('framesize','XGA')">1024x768</button>
    <button onclick="setv('framesize','HD')">1280x720</button>
  </div>
  <div class="ctl">
    <b>JPEG quality:</b>
    <button onclick="setv('quality',10)">10 (best)</button>
    <button onclick="setv('quality',12)">12</button>
    <button onclick="setv('quality',15)">15</button>
    <button onclick="setv('quality',20)">20 (fast)</button>
  </div>
  <div class="ctl">
    <b>Gain ceiling:</b>
    <button onclick="setv('gainceiling',2)">8X</button>
    <button onclick="setv('gainceiling',3)">16X</button>
    <button onclick="setv('gainceiling',4)">32X</button>
    &nbsp; <b>AEC2:</b>
    <button onclick="setv('aec2',0)">Off</button>
    <button onclick="setv('aec2',1)">On</button>
  </div>
  <div id="st"></div>
</div>

<script>
function setv(v,val){
  fetch('/set?var='+v+'&val='+val)
    .then(r=>r.text()).then(t=>{document.getElementById('st').textContent=t;});
}
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", htmlPage);
}

//=================================
void setupServer() {

  server.on("/", HTTP_GET, []() {
    sendResponse();
  });

  // שינוי הגדרות בזמן אמת: /set?var=framesize&val=SVGA
  server.on("/set", HTTP_GET, []() {
    sensor_t *s = esp_camera_sensor_get();
    if (!s) { server.send(500, "text/plain", "no sensor"); return; }
    String var = server.arg("var");
    String val = server.arg("val");
    int r = -1;

    if (var == "framesize") {
      framesize_t fs = FRAMESIZE_INVALID;
      if      (val == "QVGA") fs = FRAMESIZE_QVGA;
      else if (val == "VGA")  fs = FRAMESIZE_VGA;
      else if (val == "SVGA") fs = FRAMESIZE_SVGA;
      else if (val == "XGA")  fs = FRAMESIZE_XGA;
      else if (val == "HD")   fs = FRAMESIZE_HD;
      if (fs != FRAMESIZE_INVALID) r = s->set_framesize(s, fs);
    }
    else if (var == "quality")     r = s->set_quality(s, val.toInt());
    else if (var == "gainceiling") r = s->set_gainceiling(s, (gainceiling_t)val.toInt());
    else if (var == "aec2")        r = s->set_aec2(s, val.toInt());

    String msg = (r == 0 ? "OK: " : "FAILED: ") + var + " = " + val;
    Serial.println(msg);
    server.send(r == 0 ? 200 : 400, "text/plain", msg);
  });

  server.on("/cam", HTTP_GET, []() {
    String streamUrl = "http://" + WiFi.softAPIP().toString() + ":81/stream";
    server.sendHeader("Location", streamUrl, true);
    server.send(302, "text/plain", "");
  });
}
//===================================================