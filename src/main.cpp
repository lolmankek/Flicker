#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Подключаем наши модули
#include "esp32_wifi.h" 
#include "encoder_module.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Пины экрана
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int lastCounterValue = 0;
int connectedStations = 0;

void updateDisplay() {
  display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);           
  display.println("--- SYSTEM MONITOR ---");
  
  display.setCursor(0, 16);
  display.print("SSID: ");
  display.println(wifi_ssid); 
  
  display.setCursor(0, 28);
  display.print("IP: 192.168.4.1"); 
  
  display.setCursor(0, 38);
  display.print("Connected: ");
  display.print(connectedStations);
  display.println(" devices");
  
  display.setCursor(0, 50);
  display.print("Enc Value: ");
  display.print(getEncoderValue()); // Забираем значение из модуля энкодера
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Инициализируем экран
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;); 
  }

  // Запуск модулей
  setupWiFi();      // Из модуля Wi-Fi
  setupEncoder();   // Из модуля энкодера

  updateDisplay();
}

void loop() {
  // Опрашиваем кнопку энкодера
  if (isEncoderButtonPressed()) {
    resetEncoderValue();
    updateDisplay();
    Serial.println("Reset!");
  }

  // Опрашиваем количество подключенных устройств раз в секунду
  static uint32_t checkTimer = 0;
  if (millis() - checkTimer >= 1000) {
    checkTimer = millis();
    int currentConnected = getConnectedDevices(); 
    if (currentConnected != connectedStations) {
      connectedStations = currentConnected;
      updateDisplay();
    }
  }

  // Проверяем, повернули ли энкодер
  int currentCounter = getEncoderValue();
  if (currentCounter != lastCounterValue) {
    lastCounterValue = currentCounter;
    updateDisplay();
  }
}