#ifndef ENCODER_MODULE_H
#define ENCODER_MODULE_H

#include <Arduino.h>

// Объявляем пины энкодера
#define ENCODER_CLK 3
#define ENCODER_DT  4
#define ENCODER_SW  5

// Объявляем функции инициализации и работы с энкодером
void setupEncoder();
int getEncoderValue();
void resetEncoderValue();
bool isEncoderButtonPressed();

#endif // ENCODER_MODULE_H