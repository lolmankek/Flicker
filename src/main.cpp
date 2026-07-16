#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h>

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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GButton encButton(ENCODER_SW);

volatile int menuCounter = 0;
int lastCounterValue = 0;
volatile int lastClkState;

// Быстрое прерывание для обработки поворота
void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  if (clkState != lastClkState) {
    // Если состояние CLK изменилось, смотрим на DT для определения направления
    if (digitalRead(ENCODER_DT) != clkState) {
      menuCounter++;
    } else {
      menuCounter--;
    }
    lastClkState = clkState;
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE); 

  display.setCursor(0, 0);           
  display.println("--- ENCODER TEST ---");
  
  display.setCursor(0, 20);
  display.print("Rotate knob to scroll");
  
  display.setCursor(0, 35);
  display.print("Value: ");
  
  display.setTextSize(2); 
  display.setCursor(64, 32);
  // Делим на 2 или на 4 в зависимости от шага твоего энкодера (обычно у KY-040 делим на 2 при ручном чтении прерываний)
  display.println(menuCounter / 2); 
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  // Настраиваем пины энкодера на вход с подтяжкой к питанию
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  // Запоминаем стартовое состояние CLK
  lastClkState = digitalRead(ENCODER_CLK);

  // Вешаем аппаратное прерывание на пин CLK. Оно будет мгновенно срабатывать при любом изменении
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  
  // Запуск экрана
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;); 
  }

  updateDisplay();
}

void loop() {
  encButton.tick();

  // Обновляем дисплей только если значение реально изменилось
  if (menuCounter != lastCounterValue) {
    lastCounterValue = menuCounter;
    updateDisplay();
    Serial.print("Position: ");
    Serial.println(menuCounter / 2);
  }

  // Кнопка сбрасывает счетчик
  if (encButton.isPress()) {
    menuCounter = 0;
    lastCounterValue = 0;
    updateDisplay();
    Serial.println("Reset!");
  }
}