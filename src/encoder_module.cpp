#include "encoder_module.h"
#include <GyverButton.h>

// Создаем объект кнопки внутри .cpp (скрываем от main.cpp)
GButton encButton(ENCODER_SW);

// Внутренние переменные энкодера
volatile int menuCounter = 0;
volatile int lastClkState;

// Быстрое прерывание для обработки поворота (живет в оперативной памяти IRAM)
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

void setupEncoder() {
  // Настраиваем пины энкодера на вход с подтяжкой
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  // Запоминаем стартовое состояние CLK
  lastClkState = digitalRead(ENCODER_CLK);

  // Вешаем аппаратное прерывание
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
}

// Возвращает текущее значение (делим на 2, чтобы один щелчок равен был единице)
int getEncoderValue() {
  return menuCounter / 2;
}

// Сброс значения счетчика
void resetEncoderValue() {
  menuCounter = 0;
}

// Опрос кнопки энкодера (вызываем в loop)
bool isEncoderButtonPressed() {
  encButton.tick();
  return encButton.isPress();
}