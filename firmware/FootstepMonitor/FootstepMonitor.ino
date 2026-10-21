/*
  FootstepMonitor.ino
  Board : Arduino UNO (ATmega328P)
  Job   : MEASURE the voltage at the rectifier output on A0 and report it
          over USB serial as CSV.

  Per report window (REPORT_INTERVAL_MS) it reports:
    v_mean_V  - average voltage over the window
    v_peak_V  - largest single ADC sample in the window
    samples   - number of ADC readings taken in the window
    p_mean_W  - mean of V^2/R over the window   (NA unless LOAD_RESISTANCE_OHMS > 0)
    energy_J  - running sum of p_mean_W * window time (NA unless LOAD_RESISTANCE_OHMS > 0)

  Not implemented (no hardware for it was specified): step counting/detection,
  current measurement, storage, display, logging to SD or wireless.
  Edit settings in config.h, not here.
*/
#include "config.h"

static float energyJ = 0.0f;

static float dividerRatio() {
  return USE_VOLTAGE_DIVIDER ? (R_TOP_OHMS + R_BOTTOM_OHMS) / R_BOTTOM_OHMS : 1.0f;
}

static bool configIsValid() {
  if (!CONFIG_CONFIRMED) {
    Serial.println(F("# ERROR: CONFIG_CONFIRMED is false. Fill in config.h, then set it to true."));
    return false;
  }
  bool ok = true;
  if (ADC_REF_V <= 0.0f)           { Serial.println(F("# ERROR: ADC_REF_V must be > 0."));              ok = false; }
  if (VOLTAGE_CAL_FACTOR <= 0.0f)  { Serial.println(F("# ERROR: VOLTAGE_CAL_FACTOR must be > 0."));     ok = false; }
  if (EXPECTED_MAX_INPUT_V <= 0.0f){ Serial.println(F("# ERROR: set EXPECTED_MAX_INPUT_V (> 0)."));     ok = false; }
  if (LOAD_RESISTANCE_OHMS < 0.0f) { Serial.println(F("# ERROR: LOAD_RESISTANCE_OHMS cannot be negative.")); ok = false; }
  if (USE_VOLTAGE_DIVIDER && (R_TOP_OHMS <= 0.0f || R_BOTTOM_OHMS <= 0.0f)) {
    Serial.println(F("# ERROR: R_TOP_OHMS and R_BOTTOM_OHMS must both be > 0."));
    ok = false;
  }
  if (!ok) return false;

  const float pinV = EXPECTED_MAX_INPUT_V / dividerRatio();
  if (pinV > ADC_REF_V) {
    Serial.print(F("# ERROR: at EXPECTED_MAX_INPUT_V, A0 would see "));
    Serial.print(pinV, 2);
    Serial.println(F(" V, above ADC_REF_V. Change the divider."));
    return false;
  }
  if (USE_VOLTAGE_DIVIDER) {
    const float sourceOhms = (R_TOP_OHMS * R_BOTTOM_OHMS) / (R_TOP_OHMS + R_BOTTOM_OHMS);
    if (sourceOhms > 10000.0f) {
      Serial.println(F("# WARNING: divider source impedance is above ~10 kOhm (ATmega328P datasheet recommendation); readings may be less accurate."));
    }
  }
  return true;
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  analogReference(DEFAULT);
  Serial.println(F("# FootstepMonitor v0.1.0 (Arduino UNO)"));
  if (!configIsValid()) {
    for (;;) {                       // halt, repeating the message so it is seen when the monitor opens
      delay(3000);
      configIsValid();
    }
  }
  Serial.println(F("time_ms,v_mean_V,v_peak_V,samples,p_mean_W,energy_J"));
}

void loop() {
  uint32_t n = 0, sumCode = 0;
  uint16_t peakCode = 0;
  float sumSqCode = 0.0f;

  const unsigned long startMs = millis();
  do {
    const uint16_t c = analogRead(PIN_VOLTAGE);
    sumCode += c;
    sumSqCode += (float)c * (float)c;
    if (c > peakCode) peakCode = c;
    n++;
  } while (millis() - startMs < REPORT_INTERVAL_MS);
  const unsigned long endMs = millis();
  const float windowS = (endMs - startMs) / 1000.0f;

  // ATmega328P datasheet: ADC = Vin * 1024 / Vref
  const float k = (ADC_REF_V / 1024.0f) * dividerRatio() * VOLTAGE_CAL_FACTOR;
  const float vMean = k * ((float)sumCode / (float)n);
  const float vPeak = k * (float)peakCode;

  Serial.print(endMs);    Serial.print(',');
  Serial.print(vMean, 3); Serial.print(',');
  Serial.print(vPeak, 3); Serial.print(',');
  Serial.print(n);        Serial.print(',');
  if (LOAD_RESISTANCE_OHMS > 0.0f) {
    const float pMean = (k * k * (sumSqCode / (float)n)) / LOAD_RESISTANCE_OHMS;
    energyJ += pMean * windowS;
    Serial.print(pMean, 6);   Serial.print(',');
    Serial.println(energyJ, 6);
  } else {
    Serial.println(F("NA,NA"));
  }
}
