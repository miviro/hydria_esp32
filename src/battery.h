#pragma once

// Returns battery charge level as 0–100 %.
// Reads the on-board voltage divider (Vbat/2) via VBAT_ADC_PIN,
// gated by VBAT_ADC_CTRL (active LOW).
int batteryPercent();
