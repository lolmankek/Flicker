#ifndef ESP32_WIFI_H
#define ESP32_WIFI_H

#include <Arduino.h>

// Объявляем внешнюю переменную (вместо inline теперь пишем extern)
extern const char* wifi_ssid;

// Объявляем функции
void setupWiFi();
int getConnectedDevices();

#endif // ESP32_WIFI_H