//MAX30100 ESP8266 WebServer
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ThingSpeak.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define REPORTING_PERIOD_MS     15000

float BPM, SpO2;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   13
#define OLED_CLK   14
#define OLED_DC    2
#define OLED_CS    15
#define OLED_RESET 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Put your SSID & Password */
const char* ssid = "satya";  // Enter SSID here
const char* password = "kota12345";  // Enter Password here
WiFiClient client;
PulseOximeter pox;
uint32_t tsLastReport = 0;

ESP8266WebServer server(80);

int x =1;
bool displayOn = true; // Global variable to track the display state

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  delay(100);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to your local Wi-Fi network
  WiFi.begin(ssid, password);
  
  // Check if Wi-Fi is connected to the network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
  
  ThingSpeak.begin(client);
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  server.handleClient();
  pox.update();
  
  // Check if the display should be on
  if (displayOn==true) {
    displaytext(BPM, SpO2);
  }
 
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    display.clearDisplay();
    Serial.print("BPM: ");
    Serial.println(BPM);
    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
    Serial.println("*********************************");
    Serial.println();
    if(BPM > 120 || SpO2 < 90)
    {
      sendAlert(BPM,SpO2);
    }
    
    // Swap the order of fields if necessary
    ThingSpeak.setField(1, SpO2);
    ThingSpeak.setField(2, BPM);
    int channelNumber = 2286609; // Your ThingSpeak channel ID
    const char* apiKey = "LRXXSS6TO44ZDMOS"; // Your ThingSpeak API Key
    int httpCode = ThingSpeak.writeFields(channelNumber, apiKey);

    if (httpCode == 200) {
      Serial.println("ThingSpeak update successful");
    } else {
      Serial.println("Error updating ThingSpeak. HTTP error code: " + String(httpCode));
    }

    Serial.println("*********************************");
    Serial.println();
    tsLastReport = millis();
  }
}

void displaytext(int BPM, int Spo2) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("BPM :"));
  display.println(BPM);
  display.print(F("Spo2:"));
  display.println(Spo2);
  display.display();      // Show initial text
  delay(100);
}

void handle_OnConnect() {
  if (server.arg("display") == "on") {
    displayOn = true;
    Serial.println("satya");
  } else if (server.arg("display") == "off") {
    displayOn = false;
    // Clear the display when turned off
    display.clearDisplay();
    display.display();
    delay(1000);
  }
  server.send(200, "text/html", SendHTML(BPM, SpO2));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void sendAlert(int BPM , int Spo2)
{
  
}

String SendHTML(float BPM, float SpO2) {
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";
  //Ajax Code Start
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,1000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  //Ajax Code END
  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>MAX30100 ESP8266 WebServer</h1>";
  ptr += "</div>";
  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Sensor Readings</h2>";
  ptr += "<div class='sensors-container'>";
  //For Heart Rate
  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-heartbeat' style='color:#cc3300'></i>";
  ptr += "<span class='sensor-labels'> Heart Rate </span>";
  ptr += (int)BPM;
  ptr += "<sup class='units'>BPM</sup>";
  ptr += "</p>";
  ptr += "<hr>";
  //For Sp02
  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-burn' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> Sp02 </span>";
  ptr += (int)SpO2;
  ptr += "<sup class='units'>%</sup>";
  ptr += "</p>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  // Add a toggle button
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Display Control</h2>";
  ptr += "<label class='switch'>";
  ptr += "<input type='checkbox' id='displayToggle' ";
  ptr += displayOn ? "checked" : "";
  ptr += " onclick='toggleDisplay()'>";
  ptr += "<span class='slider round'></span>";
  ptr += "</label>";
  ptr += "<p>Display is ";
  ptr += displayOn ? "On" : "Off";
  ptr += "</p>";
  ptr += "</div>";

  ptr += "</body>";
  ptr += "</html>";
  return ptr;
}

