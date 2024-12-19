#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <M5Stack.h>

// Configuración WiFi
#define WIFI_SSID "LoRaAp"
#define WIFI_PASSW "estonotieneinternet"
#define API_URL "http://192.168.1.69/api/data"

// Variables de datos
String gpsLat;
String gpsLon;
String altitude;
String temperature;
String pressure; 
String rads;


void setup() {
  M5.begin();
  M5.Lcd.clear();
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  connectToWiFi();
  drawInterface();
}

void loop() {
  fetchDataFromAPI();
  drawInterface();
  delay(500);
}

void connectToWiFi() {
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Conectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.clear();
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("Conectado a WiFi!");
  M5.Lcd.println("    IP: " + WiFi.localIP().toString());
  delay(2000);
  M5.Lcd.clear();
}

void fetchDataFromAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(10, 200);
    M5.Lcd.println("WiFi Desconectado!");
    return;
  }

  HTTPClient http;
  http.begin(API_URL);
  int httpCode = http.GET();

  M5.Lcd.setCursor(10, 200);
  M5.Lcd.setTextColor(YELLOW);

  if (httpCode > 0) {
    String payload = http.getString();

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      gpsLat = doc["gps"]["lat"].as<String>();
      gpsLon = doc["gps"]["lon"].as<String>();
      altitude = doc["alt"].as<String>();
      temperature = doc["temp"].as<String>();
      pressure = doc["press"].as<String>();
      rads = doc["rads"].as<String>();
    } else {
      M5.Lcd.setTextColor(RED);
      M5.Lcd.setCursor(10, 220);
      M5.Lcd.println("Error JSON!");
    }
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(10, 220);
    M5.Lcd.println("Error HTTP!");
  }
  http.end();
}

void drawInterface() {
  M5.Lcd.clear();
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.println("Datos de CANSAT");
  M5.Lcd.setTextColor(WHITE);

  M5.Lcd.setCursor(10, 40);
  M5.Lcd.println("GPS: Lat: " + gpsLat);
  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("     Lon: " +  gpsLon);

  M5.Lcd.setCursor(10, 90);
  M5.Lcd.printf("Altitud: %s\n", altitude);

  M5.Lcd.setCursor(10, 120);
  M5.Lcd.printf("Temp: %s\n", temperature);

  M5.Lcd.setCursor(10, 150);
  M5.Lcd.printf("Presion: %s\n", pressure);

  M5.Lcd.setCursor(10, 180);
  M5.Lcd.printf("Rads: %s", rads);
}
