/*
  config.h  -  all settings for FootstepMonitor.ino

  Every value marked "FILL IN" depends on YOUR hardware and was NOT supplied
  with the project, so it is left blank/neutral on purpose. The sketch refuses
  to measure until CONFIG_CONFIRMED is set to true, so a half-configured board
  cannot silently give wrong numbers.
*/
#pragma once
#include <Arduino.h>

// Set to true only after you have filled in and double-checked every value below.
const bool CONFIG_CONFIRMED = false;

// ---- Input -----------------------------------------------------------------
// Analog pin wired to the (divided) rectifier output.
const uint8_t PIN_VOLTAGE = A0;

// ---- Reference voltage -----------------------------------------------------
// UNO default analog reference is the board's 5 V supply rail (AVcc).
// When powered over USB this is not exactly 5.00 V: measure the 5V pin with a
// multimeter and enter that value. Nominal value shown.
const float ADC_REF_V = 5.0f;

// ---- Voltage divider (between rectifier output and A0) ----------------------
//   Rectifier DC(+) --[R_TOP_OHMS]--+-- A0
//                                   |
//                           [R_BOTTOM_OHMS]
//                                   |
//   Rectifier DC(-) ----------------+-- UNO GND
// FILL IN: use the real (multimeter-measured) resistor values.
const bool  USE_VOLTAGE_DIVIDER = true;   // false = rectifier wired straight to A0 (only safe if it can never exceed ADC_REF_V)
const float R_TOP_OHMS          = 0.0f;   // FILL IN (ohms)
const float R_BOTTOM_OHMS       = 0.0f;   // FILL IN (ohms)

// ---- Safety check input ----------------------------------------------------
// FILL IN: the HIGHEST voltage the rectifier output can reach (volts), e.g. the
// open-circuit worst case. The sketch halts if this would push A0 above ADC_REF_V.
const float EXPECTED_MAX_INPUT_V = 0.0f;  // FILL IN (volts)

// ---- Calibration -----------------------------------------------------------
// 1.0 = no correction. See docs/hardware.md, "Calibration".
const float VOLTAGE_CAL_FACTOR = 1.0f;

// ---- Optional power / energy ------------------------------------------------
// Only valid if the voltage you measure is the voltage ACROSS a resistive load
// of known value that is the only load on the rectifier output.
// 0 = disabled (power and energy columns print NA).
const float LOAD_RESISTANCE_OHMS = 0.0f;  // optional (ohms)

// ---- 16x2 LCD (one is visible in the project photo) -------------------------
// The photo does not show whether the LCD has an I2C backpack or is wired in
// parallel. This sketch supports an I2C LCD only, using the "LiquidCrystal_I2C"
// library. Leave disabled until you have confirmed your LCD type.
#define LCD_ENABLED     0       // set to 1 only if your LCD has an I2C backpack
#define LCD_I2C_ADDRESS 0x00    // FILL IN: find it with an I2C scanner sketch
const bool LCD_SHOW_PEAK = false;  // false = show window mean, true = show window peak

// ---- Reporting -------------------------------------------------------------
const unsigned long REPORT_INTERVAL_MS = 100;   // one CSV line per window
const unsigned long SERIAL_BAUD        = 115200;
