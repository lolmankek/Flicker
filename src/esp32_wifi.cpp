#include "esp32_wifi.h"
#include <WiFi.h> // Подключаем тяжелую библиотеку Wi-Fi только здесь

// Сами переменные создаются тут
const char* wifi_ssid = "Flicker";
const char* password = "123456789";

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