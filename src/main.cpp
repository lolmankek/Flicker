#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h>
#include <WiFi.h> // Встроенная библиотека для работы с Wi-Fi

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Пины экрана
#define I2C_SDA 8
#define I2C_SCL 9

// Пины энкодера
#define ENCODER_CLK 3
#define ENCODER_DT  4
#define ENCODER_SW  5

// Настройки нашей Wi-Fi точки доступа
const char* ssid = "ESP32-S3-Flicker"; // Имя твоей сети Wi-Fi
const char* password = "12345678";  // Пароль (минимум 8 символов!)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GButton encButton(ENCODER_SW);

volatile int menuCounter = 0;
int lastCounterValue = 0;
volatile int lastClkState;
int connectedStations = 0; // Переменная для хранения числа подключенных клиентов

// Быстрое прерывание для обработки поворота энкодера
void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  if (clkState != lastClkState) {
    if (digitalRead(ENCODER_DT) != clkState) {
      menuCounter++;
    } else {
      menuCounter--;
    }
    lastClkState = clkState;
  }
}

// Функция отрисовки интерфейса на OLED
void updateDisplay() {
  display.clearDisplay();
  
  // Шапка
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);           
  display.println("--- Wi-Fi MONITOR ---");
  
  // Информация о Wi-Fi
  display.setCursor(0, 16);
  display.print("SSID: ");
  display.println(ssid);
  
  display.setCursor(0, 28);
  display.print("IP: ");
  display.println(WiFi.softAPIP().toString()); // Выводим IP-адрес платы
  
  // Статистика подключений
  display.setCursor(0, 38);
  display.print("Connected: ");
  display.print(connectedStations);
  display.println(" devices");
  
  // Значение энкодера снизу
  display.setCursor(0, 52);
  display.print("Enc Value: ");
  display.print(menuCounter / 2); 
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Настраиваем пины энкодера
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lastClkState = digitalRead(ENCODER_CLK);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  
  // Запуск экрана
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;); 
  }

  // ИНИЦИАЛИЗАЦИЯ WI-FI ТОЧКИ ДОСТУПА
  Serial.println("Configuring Access Point...");
  
  // Запускаем режим AP (Access Point)
  WiFi.mode(WIFI_AP); 
  
  // Стартуем сеть с нашим именем и паролем
  WiFi.softAP(ssid, password);

  // Печатаем IP в монитор порта
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  updateDisplay();
}

void loop() {
  encButton.tick();

  // Каждую секунду проверяем, не подключился ли кто-то к нашему Wi-Fi
  static uint32_t checkTimer = 0;
  if (millis() - checkTimer >= 1000) {
    checkTimer = millis();
    
    // Получаем количество подключенных устройств
    int currentConnected = WiFi.softAPgetStationNum(); 
    
    if (currentConnected != connectedStations) {
      connectedStations = currentConnected;
      updateDisplay(); // Обновляем экран, если кто-то подключился/отключился
      Serial.print("Stations connected: ");
      Serial.println(connectedStations);
    }
  }

  // Обновляем дисплей при вращении энкодера
  if (menuCounter != lastCounterValue) {
    lastCounterValue = menuCounter;
    updateDisplay();
  }

  // Кнопка энкодера сбрасывает счетчик
  if (encButton.isPress()) {
    menuCounter = 0;
    lastCounterValue = 0;
    updateDisplay();
    Serial.println("Reset!");
  }
}