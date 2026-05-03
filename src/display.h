#pragma once

#include <stdint.h>

void displayBegin();
void displayStatus(const char *text);
void displayReadings(float sonarCm, int turbidity, int humidity, int batteryPct, uint16_t wakeCount);
void displayOff();
