#include "esp32_wifi.h"
#include <WiFi.h> // Подключаем тяжелую библиотеку Wi-Fi только здесь

// Сами переменные создаются тут
const char* wifi_ssid = "ESP32-S3-Flicker";
const char* password = "password123";

// Реализация инициализации
void setupWiFi() {
  Serial.println("Configuring Access Point...");
  
  WiFi.mode(WIFI_AP); 
  WiFi.softAP(wifi_ssid, password);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

// Реализация подсчета подключений
int getConnectedDevices() {
  return WiFi.softAPgetStationNum();
}