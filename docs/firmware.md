# Firmware notes

File: `firmware/FootstepMonitor/FootstepMonitor.ino`, settings in `config.h`. Target: Arduino UNO.

## How it measures

1. Reads A0 repeatedly for one report window (default 100 ms).
2. Converts each reading using the datasheet relation `ADC = Vin × 1024 / Vref`, so `Vin = ADC × Vref / 1024`.
3. Multiplies by the divider ratio and `VOLTAGE_CAL_FACTOR` to get the rectifier-side voltage.
4. Prints the window mean, the largest single sample (peak), and the number of samples.

With default Arduino settings one conversion takes about 104 µs (about 9,600 readings per second at most). The `samples` column shows what was achieved.

## Settings (`config.h`)

| Setting | Meaning | Default |
|---|---|---|
| `CONFIG_CONFIRMED` | Must be `true` before it measures | `false` |
| `PIN_VOLTAGE` | Analog input | `A0` |
| `ADC_REF_V` | Measured 5 V rail | `5.0` |
| `USE_VOLTAGE_DIVIDER` | Divider fitted | `true` |
| `R_TOP_OHMS`, `R_BOTTOM_OHMS` | Real divider values | blank (0), you fill in |
| `EXPECTED_MAX_INPUT_V` | Highest rectifier voltage, for the safety check | blank (0), you fill in |
| `VOLTAGE_CAL_FACTOR` | Calibration multiplier | `1.0` |
| `LOAD_RESISTANCE_OHMS` | Optional, enables power and energy | `0` (disabled) |
| `LCD_ENABLED` | `1` to drive an I2C 16x2 LCD (needs the LiquidCrystal_I2C library) | `0` |
| `LCD_I2C_ADDRESS` | I2C address of the LCD backpack | blank (0), you fill in |
| `LCD_SHOW_PEAK` | LCD shows window peak instead of window mean | `false` |
| `REPORT_INTERVAL_MS` | Window length | `100` |
| `SERIAL_BAUD` | Serial speed | `115200` |

## LCD behaviour

With `LCD_ENABLED` set to `1` the first line shows `FOOT STEP POWER` and the second line shows `Volt: x.xxV`, updated once per window. The photo shows an LCD with these two lines. The display's interface was not visible, so only I2C is supported. Common I2C backpacks use address 0x27 or 0x3F, but confirm yours with an I2C scanner. LCD updates take a few milliseconds, during which no readings are taken.

## Output columns

| Column | Unit | Meaning |
|---|---|---|
| `time_ms` | ms | Time since the UNO started, at the end of the window |
| `v_mean_V` | V | Average voltage in the window |
| `v_peak_V` | V | Largest single reading in the window |
| `samples` | count | Readings taken in the window |
| `p_mean_W` | W | Mean of V²/R in the window, or `NA` |
| `energy_J` | J | Running total of `p_mean_W × window time`, or `NA` |

## Power and energy: when they are valid

They are computed only when `LOAD_RESISTANCE_OHMS > 0`, and are valid only if the voltage you measure is the voltage across that resistor and it is the only load. The sketch does not measure current. The energy total also leaves out the short time spent printing each line between windows, so it is an underestimate.

## Known limits

- Peaks shorter than one conversion can be missed.
- A single analog channel; no isolation.
- Timing relies on `millis()` (1 ms resolution).
- Not tested on hardware yet. The sketch was only syntax-checked on a PC against stubs of the Arduino and LCD libraries, with the LCD both off and on.

## Troubleshooting

| Symptom | Check |
|---|---|
| Serial Monitor shows `# ERROR` and no data | A `config.h` value is unset or unsafe; follow the message |
| Reads 0 V always | Ground not shared, or rectifier output not connected |
| Reads near the maximum | A0 is at the reference voltage; the divider ratio is too low |
| Garbled characters | Serial Monitor baud must match `SERIAL_BAUD` |
| Readings drift with no input | Floating pin; confirm the divider's bottom resistor is connected to GND |
