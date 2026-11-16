# CS-CAM3-M-AI
<div dir="rtl">

<header>
  <div><b>ESP32-S3-CAM · OV3660 · Arduino IDE</b></div>
  <h1>מדריך ESP32-S3-CAM עם מצלמת OV3660</h1>
  <p>הסבר על הלוח ועל המצלמה, ההגדרות לצריבה ב-Arduino IDE, ואיך משתמשים בפקדים בדף האינטרנט כדי לקבל תמונה טובה יותר.</p>
</header>

<hr>

<nav>
  <b>תוכן העניינים</b>
  <ol>
    <li><a href="#s1">מה זה ESP32-S3</a></li>
    <li><a href="#s2">המודול N16R8</a></li>
    <li><a href="#s3">מצלמת OV3660 ‏(3MP)</a></li>
    <li><a href="#s4">הגדרות צריבה ב-Arduino IDE</a></li>
    <li><a href="#s5">הגדרות המצלמה בקוד</a></li>
    <li><a href="#s6">הפקדים בדף האינטרנט</a></li>
    <li><a href="#s7">שילובים מומלצים</a></li>
    <li><a href="#s8">קריאת נתוני הביצועים</a></li>
    <li><a href="#s9">פתרון תקלות</a></li>
  </ol>
</nav>

<hr>

<!-- 1 -->
<section id="s1">
  <h2>1. מה זה ESP32-S3</h2>
  <p>ה-ESP32-S3 הוא מיקרו-בקר של חברת Espressif. יש בו מעבד כפול ליבות, Wi-Fi ו-Bluetooth, והוא בנוי לעבודה עם מצלמה, מסך ויישומי בינה מלאכותית קטנים. בהשוואה ל-ESP32 הקלאסי (שנמצא בלוח ESP32-CAM המוכר), הוא מהיר יותר, תומך ב-PSRAM מהיר יותר, ויש לו חיבור USB מובנה.</p>
  <table>
    <tr><th>מאפיין</th><th>מפרט</th></tr>
    <tr><td>מעבד</td><td>2 ליבות Xtensa LX7</td></tr>
    <tr><td>מהירות שעון</td><td>עד 240MHz</td></tr>
    <tr><td>זיכרון פנימי (SRAM)</td><td>512KB</td></tr>
    <tr><td>תקשורת</td><td>Wi-Fi 2.4GHz + BLE 5</td></tr>
    <tr><td>ממשק מצלמה</td><td>DVP 8-bit</td></tr>
    <tr><td>USB</td><td>USB נייטיב מובנה</td></tr>
  </table>
  <p>הזיכרון הפנימי (512KB) קטן מדי לתמונות ברזולוציה גבוהה. לכן ללוחות מצלמה יש <b>PSRAM</b>, זיכרון חיצוני גדול שבו נשמרות התמונות.</p>
</section>

<!-- 2 -->
<section id="s2">
  <h2>2. המודול N16R8</h2>
  <p>על המודול המתכתי בלוח כתוב <code>S3-N16R8</code>. הסימון הזה מגדיר את כמות הזיכרון:</p>
  <table>
    <tr><th>סימון</th><th>משמעות</th><th>למה זה חשוב</th></tr>
    <tr><td><code>N16</code></td><td>זיכרון Flash של 16MB</td><td>כאן נשמרת התוכנה. יש מקום רב גם לקבצים ולדפי אינטרנט.</td></tr>
    <tr><td><code>R8</code></td><td>PSRAM של 8MB מסוג OPI (Octal)</td><td>כאן נשמרות התמונות. מסוג OPI עובר נתונים מהר יותר מ-PSRAM רגיל (QSPI).</td></tr>
  </table>
  <p>בהרצה על הלוח נמדדו בפועל: <code>Flash: 16777216 bytes</code> ו-<code>PSRAM: 8388608 bytes</code>, כלומר 16MB ו-8MB.</p>
</div>

> [!WARNING]
> **שימו לב: GPIO 35, 36 ו-37 תפוסים**  
> ה-PSRAM מסוג OPI משתמש בפינים האלה. לא לחבר אליהם רכיבים נוספים.

<div dir="rtl">
</section>

<!-- 3 -->
<section id="s3">
  <h2>3. מצלמת OV3660 ‏(3MP)</h2>
  <p>ה-OV3660 הוא חיישן תמונה של חברת OmniVision ברזולוציה של 3 מגה-פיקסל. הוא מחובר ללוח בכבל סרט כתום, ושם החיישן מודפס על הכבל.</p>
  <table>
    <tr><th>מאפיין</th><th>ערך</th></tr>
    <tr><td>רזולוציה מקסימלית</td><td>2048×1536</td></tr>
    <tr><td>פלט</td><td>JPEG מובנה בחיישן</td></tr>
    <tr><td>פוקוס</td><td>קבוע (ידני)</td></tr>
    <tr><td>מזהה חיישן (PID)</td><td><code>0x3660</code></td></tr>
  </table>
  <h3>הדחיסה ל-JPEG נעשית בתוך החיישן</h3>
  <p>החיישן דוחס כל תמונה ל-JPEG בעצמו, כך שה-ESP32-S3 לא צריך לעבד את התמונה אלא רק להעביר אותה ב-Wi-Fi. בזכות זה אפשר לשדר וידאו חי ממיקרו-בקר קטן.</p>
  <h3>אין פוקוס אוטומטי</h3>
  <p>הפוקוס קבוע. אם התמונה מטושטשת גם כשהמצלמה עומדת במקום, אפשר לסובב את העדשה בעדינות כדי לכוונן. בחלק מהמודולים העדשה מקובעת בדבק. במקרה כזה לא להפעיל כוח.</p>
  <p>מה שנראה כמו "מתפקס לאט" אחרי תנועה הוא בדרך כלל <b>החשיפה האוטומטית</b>: החיישן מכוונן את הבהירות לסצנה החדשה. אפשר להשפיע על זה בפקדים בדף האינטרנט (סעיף 6).</p>
</section>

<!-- 4 -->
<section id="s4">
  <h2>4. הגדרות צריבה ב-Arduino IDE</h2>
  <p>ההגדרות נמצאות בתפריט <b>Tools</b>. אלה ההגדרות שנבדקו ועבדו על הלוח (Arduino IDE 2.3.10):</p>
  <table>
    <tr><th>הגדרה</th><th>ערך</th><th>הסבר</th></tr>
    <tr><td>Board</td><td><code>ESP32S3 Dev Module</code></td><td>הגדרה כללית שמתאימה ללוחות ESP32-S3.</td></tr>
    <tr><td>CPU Frequency</td><td><code>240MHz (WiFi)</code></td><td>המהירות המקסימלית.</td></tr>
    <tr><td>Flash Mode</td><td><code>QIO 80MHz</code></td><td>קריאה מהירה מה-Flash.</td></tr>
    <tr><td>Flash Size</td><td><code>16MB (128Mb)</code></td><td>מתאים ל-N16.</td></tr>
    <tr><td><b>PSRAM</b></td><td><b><code>OPI PSRAM</code></b></td><td><b>ההגדרה החשובה ביותר.</b> בלי הערך הזה הלוח לא מזהה את ה-PSRAM.</td></tr>
    <tr><td>Partition Scheme</td><td><code>16M Flash (3MB APP/9.9MB FATFS)</code></td><td>3MB לתוכנה. הקוד הנוכחי תופס כ-0.8MB.</td></tr>
    <tr><td>USB CDC On Boot</td><td><code>Disabled</code></td><td>כשהכבל מחובר ליציאת UART/COM. ביציאת ה-USB הנייטיב משנים ל-<code>Enabled</code>.</td></tr>
    <tr><td>Upload Speed</td><td><code>921600</code></td><td>אם ההעלאה נכשלת, להוריד ל-<code>460800</code>.</td></tr>
    <tr><td>Core Debug Level</td><td><code>None</code></td><td>לאבחון תקלות אפשר להעביר זמנית ל-<code>Info</code>.</td></tr>
  </table>
</div>

> [!WARNING]
> **אם PSRAM לא מוגדר ל-OPI**  
> הקוד לא יציג שגיאה. הוא יעבור בשקט למצב חלופי של ‎320×240 באיכות נמוכה, וב-Serial Monitor תופיע השורה `PSRAM NOT found`.

<div dir="rtl">
  <h3>מה אמור להופיע ב-Serial Monitor (115200)</h3>
<pre dir="ltr">Chip: ESP32-S3 rev 2 | Flash: 16777216 bytes | PSRAM: 8388608 bytes
AP SSID: ESP32-S3-CAM-AP
AP IP Address: 192.168.4.1
PSRAM found: 8388608 bytes
Sensor PID: 0x3660
Sensor: OV3660 detected
Camera Stream Started
System Ready</pre>
  <p>אחר כך מתחברים לרשת ה-Wi-Fi <code>ESP32-S3-CAM-AP</code> וגולשים ל-<code>http://192.168.4.1</code>.</p>
</section>

<!-- 5 -->
<section id="s5">
  <h2>5. הגדרות המצלמה בקוד</h2>
  <p>ההגדרות האלה נקבעות בקוד (בקובץ <code>CAMERAPINS.ino</code>) ונטענות בכל הפעלה.</p>

  <h3>זיכרון ותזמון</h3>
  <table>
    <tr><th>הגדרה</th><th>ערך</th><th>מה זה עושה</th></tr>
    <tr><td><code>camera_config_t config = {}</code></td><td>איפוס</td><td>מאפס את כל השדות, כך ששדה שהקוד לא קובע לא יקבל ערך אקראי מהזיכרון.</td></tr>
    <tr><td><code>fb_location</code></td><td>PSRAM</td><td>התמונות נשמרות ב-PSRAM הגדול.</td></tr>
    <tr><td><code>frame_size</code> באתחול</td><td>QXGA ‏(2048×1536)</td><td>מקצה זיכרון לתמונה הגדולה ביותר, כך שאפשר לעבור לכל רזולוציה בלי לצרוב מחדש.</td></tr>
    <tr><td><code>fb_count</code></td><td>2</td><td>שני מאגרי תמונה: החיישן ממלא אחד בזמן שהשני נשלח.</td></tr>
    <tr><td><code>grab_mode</code></td><td>GRAB_LATEST</td><td>תמיד נשלחת התמונה העדכנית ביותר, וכך ההשהיה קטנה.</td></tr>
    <tr><td><code>xclk_freq_hz</code></td><td>20MHz</td><td>שעון החיישן. 20MHz הוא ערך יציב ל-OV3660, ותדר גבוה יותר עלול לגרום לפסים בתמונה.</td></tr>
    <tr><td><code>WiFi.setSleep(false)</code></td><td>כבוי</td><td>מבטל את מצב חיסכון החשמל של ה-Wi-Fi. במצב הזה השליחה מתעכבת לפעמים.</td></tr>
  </table>

  <h3>איכות תמונה</h3>
  <table>
    <tr><th>הגדרה</th><th>ערך</th><th>מה זה עושה</th></tr>
    <tr><td>Brightness</td><td>1</td><td>מעט בהיר יותר. ה-OV3660 נוטה לתמונה כהה.</td></tr>
    <tr><td>Saturation</td><td>‎-2</td><td>מוריד את רוויית הצבעים. ה-OV3660 נוטה לצבעים רוויים מדי.</td></tr>
    <tr><td>Contrast / Sharpness</td><td>0 / 0</td><td>נקודת התחלה ניטרלית. ערכים גבוהים מוסיפים הילות סביב קצוות ומגבירים רעש.</td></tr>
    <tr><td>LENC</td><td>פעיל</td><td>תיקון עדשה: מונע החשכה בפינות התמונה.</td></tr>
    <tr><td>RAW GMA</td><td>פעיל</td><td>תיקון גמא: מעברים טבעיים בין אזורים כהים לבהירים.</td></tr>
    <tr><td>DCW</td><td>פעיל</td><td>הקטנה איכותית כשהרזולוציה נמוכה מהמקסימום.</td></tr>
    <tr><td>BPC / WPC</td><td>פעיל</td><td>מתקן פיקסלים פגומים (שחורים ולבנים).</td></tr>
    <tr><td>AWB / AGC / AEC</td><td>אוטומטי</td><td>איזון לבן, הגבר וחשיפה אוטומטיים.</td></tr>
    <tr><td>VFLIP + HMIRROR</td><td>1 + 1</td><td>שניהם יחד מסובבים את התמונה ב-180°. אם התמונה הפוכה או בתמונת ראי, משנים אחד מהם ל-0.</td></tr>
  </table>

  <h3>ברירות מחדל לשידור</h3>
  <p>רזולוציה <b>‎1280×720 (HD)</b> ואיכות JPEG <b>10</b>, תקרת הגבר <b>16X</b> ו-AEC2 <b>כבוי</b>. אפשר לשנות את כולן מדף האינטרנט.</p>
</section>

<!-- 6 -->
<section id="s6">
  <h2>6. הפקדים בדף האינטרנט</h2>
  <p>מתחת לתמונה החיה יש כפתורים שמשנים הגדרות <b>בזמן אמת, בלי לצרוב מחדש</b>. אחרי כל לחיצה מופיעה הודעה כחולה, למשל <code>OK: framesize = HD</code>, שמאשרת שהשינוי התקבל.</p>
</div>

> [!NOTE]
> **חשוב לדעת**  
> השינויים בדף נשמרים רק עד הכיבוי הבא. אחרי הפעלה מחדש חוזרות ברירות המחדל מהקוד.

<div dir="rtl">
  <h3>Resolution: רזולוציה</h3>
  <p>קובעת את גודל התמונה. רזולוציה גבוהה יותר נותנת תמונה חדה יותר, אבל כל פריים גדול יותר, אז נשלחים פחות פריימים בשנייה וההשהיה גדלה.</p>
  <table>
    <tr><th>כפתור</th><th>מתאים ל...</th></tr>
    <tr><td><code>320x240</code></td><td>תגובה מהירה ביותר, קליטת Wi-Fi חלשה, זיהוי תנועה.</td></tr>
    <tr><td><code>640x480</code></td><td>שידור חלק ומהיר עם תמונה סבירה.</td></tr>
    <tr><td><code>800x600</code></td><td>איזון טוב לשידור יומיומי.</td></tr>
    <tr><td><code>1024x768</code></td><td>תמונה חדה עם תגובה טובה.</td></tr>
    <tr><td><code>1280x720</code></td><td>החדות הגבוהה ביותר בשידור, בפורמט רחב (16:9). זו ברירת המחדל.</td></tr>
  </table>

  <h3>JPEG quality: איכות הדחיסה</h3>
  <p>בסולם הזה <b>מספר נמוך יותר נותן איכות גבוהה יותר</b>, וגם קובץ גדול יותר לכל פריים.</p>
  <table>
    <tr><th>כפתור</th><th>תוצאה</th></tr>
    <tr><td><code>10 (best)</code></td><td>הכי מעט "ריבועים" ופרטים חדים. פריים גדול יותר ושליחה איטית יותר.</td></tr>
    <tr><td><code>12</code></td><td>כמעט אותה איכות, פריים קטן יותר.</td></tr>
    <tr><td><code>15</code></td><td>מתאים לשידור חלק. אובדן איכות קטן.</td></tr>
    <tr><td><code>20 (fast)</code></td><td>פריים קטן ושליחה מהירה. דחיסה נראית לעין.</td></tr>
  </table>

  <h3>Gain ceiling: תקרת ההגבר</h3>
  <p>קובעת עד כמה החיישן מורשה להגביר את האות כשחסר אור. <b>משפיעה כמעט רק באור חלש</b>. באור טוב החיישן לא מגיע לתקרה, ולכן אין הבדל בין הכפתורים.</p>
  <table>
    <tr><th>כפתור</th><th>באור חלש</th></tr>
    <tr><td><code>8X</code></td><td>מעט רעש, אבל התמונה כהה יותר, החשיפה ארוכה, יש מריחה בתנועה ופחות פריימים.</td></tr>
    <tr><td><code>16X</code></td><td>איזון. זו ברירת המחדל.</td></tr>
    <tr><td><code>32X</code></td><td>תמונה בהירה ותגובה מהירה, אבל יותר "גרגרים" (רעש).</td></tr>
  </table>
  <p>כדי לראות את ההבדל: להחשיך את החדר או לכסות חלקית את העדשה, ולעבור בין 8X ל-32X.</p>

  <h3>AEC2: חישוב חשיפה מתקדם</h3>
  <p>משנה את אופן חישוב החשיפה האוטומטית. ההבדל עדין, ובעיקר רואים אותו במעבר מהיר מאזור כהה לבהיר.</p>
  <ul>
    <li><code>Off</code>: תגובה מהירה יותר לשינויי תאורה. זו ברירת המחדל.</li>
    <li><code>On</code>: לפעמים מאוזן יותר בסצנה קבועה, אבל מגיב לאט יותר.</li>
  </ul>
</section>

<!-- 7 -->
<section id="s7">
  <h2>7. שילובים מומלצים</h2>
  <h4>איכות תמונה מקסימלית</h4>
  <ul>
    <li>Resolution: <b>1280x720</b></li>
    <li>JPEG quality: <b>10</b></li>
    <li>Gain ceiling: <b>16X</b></li>
    <li>AEC2: <b>Off</b></li>
  </ul>

  <h4>שידור חלק ותגובה מהירה</h4>
  <ul>
    <li>Resolution: <b>640x480</b> או <b>800x600</b></li>
    <li>JPEG quality: <b>12–15</b></li>
    <li>Gain ceiling: <b>16X</b></li>
    <li>AEC2: <b>Off</b></li>
  </ul>

  <h4>תאורה חלשה</h4>
  <ul>
    <li>Resolution: <b>800x600</b></li>
    <li>JPEG quality: <b>12</b></li>
    <li>Gain ceiling: <b>32X</b></li>
    <li>AEC2: לנסות <b>On</b> ו-<b>Off</b></li>
  </ul>

  <h4>קליטת Wi-Fi חלשה</h4>
  <ul>
    <li>Resolution: <b>320x240</b> או <b>640x480</b></li>
    <li>JPEG quality: <b>15–20</b></li>
    <li>להתקרב ללוח או לחבר אנטנה חיצונית</li>
  </ul>
</section>

<!-- 8 -->
<section id="s8">
  <h2>8. קריאת נתוני הביצועים</h2>
  <p>בזמן השידור מודפסת ב-Serial Monitor שורה כל 5 שניות:</p>
<pre dir="ltr">FPS: 17.4 | avg frame: 23 KB | capture: 32 ms | send: 24 ms | free PSRAM: 7101628</pre>
  <table>
    <tr><th>שדה</th><th>משמעות</th><th>אם הערך גבוה</th></tr>
    <tr><td>FPS</td><td>פריימים בשנייה שבאמת נשלחו.</td><td>(כאן ערך גבוה הוא טוב)</td></tr>
    <tr><td>avg frame</td><td>גודל ממוצע של פריים.</td><td>להוריד רזולוציה או להעלות את מספר ה-JPEG quality.</td></tr>
    <tr><td>capture</td><td>זמן ההמתנה לפריים מהחיישן.</td><td>החיישן הוא צוואר הבקבוק, בדרך כלל בגלל תאורה חלשה. להוסיף אור או להעלות את ה-Gain ceiling.</td></tr>
    <tr><td>send</td><td>זמן שליחת הפריים ב-Wi-Fi.</td><td>ה-Wi-Fi הוא צוואר הבקבוק. להוריד רזולוציה או איכות, או לשפר את הקליטה.</td></tr>
    <tr><td>free PSRAM</td><td>זיכרון PSRAM פנוי.</td><td>צריך להישאר יציב. ירידה מתמשכת מעידה על דליפת זיכרון.</td></tr>
  </table>
  <p>הדוגמה שלמעלה נמדדה על הלוח בבדיקה הראשונה: כ-17.4 פריימים בשנייה, יציב, וה-PSRAM לא ירד לאורך הבדיקה.</p>
</section>

<!-- 9 -->
<section id="s9">
  <h2>9. פתרון תקלות</h2>
  <table>
    <tr><th>תסמין</th><th>סיבה אפשרית</th><th>פתרון</th></tr>
    <tr><td><code>PSRAM NOT found</code></td><td>הגדרת PSRAM שגויה.</td><td>Tools ← PSRAM ← <code>OPI PSRAM</code>.</td></tr>
    <tr><td><code>Camera init failed 0x105</code> או <code>0x20001</code></td><td>החיישן לא מזוהה.</td><td>לבדוק שכבל הסרט מחובר עד הסוף ושהתפס סגור, ולבדוק את מיפוי הפינים.</td></tr>
    <tr><td>Serial Monitor ריק</td><td>מהירות או הגדרת USB.</td><td>לבחור 115200. ביציאת ה-USB הנייטיב להגדיר <code>USB CDC On Boot: Enabled</code>.</td></tr>
    <tr><td>ההעלאה נתקעת על <code>Connecting...</code></td><td>הלוח לא נכנס למצב צריבה.</td><td>להחזיק BOOT, ללחוץ ולשחרר RST, ואז לשחרר BOOT.</td></tr>
    <tr><td>פסים או תמונה שבורה</td><td>שעון החיישן גבוה מדי או כבל רופף.</td><td>להשאיר XCLK על 20MHz ולבדוק את הכבל.</td></tr>
    <tr><td>תמונה מטושטשת גם כשהמצלמה במנוחה</td><td>פוקוס העדשה.</td><td>לסובב את העדשה בעדינות.</td></tr>
    <tr><td>מריחה בתנועה</td><td>חשיפה ארוכה באור חלש.</td><td>להוסיף אור, להעלות את ה-Gain ceiling ולכבות את AEC2.</td></tr>
    <tr><td>תמונה הפוכה או בתמונת ראי</td><td>כיוון ההתקנה.</td><td>לשנות את <code>set_vflip</code> או את <code>set_hmirror</code> ל-0.</td></tr>
  </table>
</section>

<hr>
<footer>מדריך ESP32-S3-CAM · OV3660 · עודכן 31.10.2026</footer>

</div>
