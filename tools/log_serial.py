#!/usr/bin/env python3
"""Save the FootstepMonitor serial output to a CSV file (lines starting with # are shown but not saved)."""
import argparse
import os
from datetime import datetime

import serial  # pip install -r tools/requirements.txt


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("port", help="serial port, e.g. COM3 or /dev/ttyACM0")
    ap.add_argument("--baud", type=int, default=115200, help="must match SERIAL_BAUD in config.h")
    ap.add_argument("--out", help="output CSV path (default: data/session_<timestamp>.csv)")
    args = ap.parse_args()

    out = args.out or f"data/session_{datetime.now():%Y%m%d_%H%M%S}.csv"
    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    print(f"Logging to {out}. Press Ctrl+C to stop.")

    with serial.Serial(args.port, args.baud, timeout=2) as ser, open(out, "w", newline="") as f:
        try:
            while True:
                line = ser.readline().decode("utf-8", "replace").strip()
                if not line:
                    continue
                print(line)
                if not line.startswith("#"):
                    f.write(line + "\n")
                    f.flush()
        except KeyboardInterrupt:
            print("\nStopped.")


if __name__ == "__main__":
    main()
