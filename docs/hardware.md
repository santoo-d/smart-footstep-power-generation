# Hardware and wiring

Only two parts are confirmed for this project: an **Arduino UNO** and a **rectifier**. The voltage divider below is required for safe measurement, but its resistor values are not decided; choose them for your own rectifier output.

## Safety

- The ATmega328P datasheet limits any pin to between -0.5 V and VCC + 0.5 V. Anything outside this can damage the UNO. Never connect the rectifier output directly to A0 unless it can never exceed the 5 V supply rail.
- Find the **highest** voltage the rectifier output can reach (for example, with the output unloaded) before connecting it. Enter this as `EXPECTED_MAX_INPUT_V` in `config.h`. The sketch refuses to run if that voltage would push A0 above `ADC_REF_V`.
- Reverse polarity or a negative voltage at A0 is also outside the limit. Check the rectifier's polarity with a multimeter first.
- Disconnect the rectifier before uploading new firmware.

## Wiring

```
Rectifier DC (+) ---[ R_TOP ]---+--- A0
                                |
                           [ R_BOTTOM ]
                                |
Rectifier DC (-) ---------------+--- GND (UNO)
```

The rectifier negative and the UNO GND must be joined so both share the same reference.

## Choosing the divider

- Ratio: `(R_TOP + R_BOTTOM) / R_BOTTOM`
- Voltage at A0: `V_rectifier / ratio`
- Required: `EXPECTED_MAX_INPUT_V / ratio` must be **below** the ADC reference (about 5 V).
- The ATmega328P datasheet recommends a source impedance of about 10 kΩ or less for the ADC. The impedance seen by A0 is `R_TOP × R_BOTTOM / (R_TOP + R_BOTTOM)`; the sketch warns if it is higher.
- Measure the real resistor values with a multimeter and enter those in `config.h`.
- The divider also draws current from the rectifier output. If the output can only supply tiny currents, this loading affects what you measure.

## Reference voltage

The UNO's default analog reference is its own 5 V supply rail, which is not exactly 5.00 V. Measure the **5V** pin against GND with a multimeter (powered the same way you will use it) and set `ADC_REF_V`.

## Calibration

1. Set `VOLTAGE_CAL_FACTOR = 1.0`.
2. Apply a steady, known DC voltage within your expected range through the same divider, and read it with a multimeter.
3. Compare with `v_mean_V` from the serial output.
4. Set `VOLTAGE_CAL_FACTOR = multimeter reading / reported v_mean_V`.
5. Re-test at a second voltage to confirm. Also confirm that a shorted input reads close to 0 V.

Calibration with steady DC does not prove accuracy for short footstep pulses. Treat pulse peaks as approximate.

## Not specified

The footstep generator element, any smoothing capacitor, energy storage and load were not specified, so none are assumed here. Record yours in `setup-record.md`.
