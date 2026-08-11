#!/usr/bin/env python3
"""Calculate Q8 sample delays for a four-microphone delay-and-sum beam."""

import argparse
import json
import math
import sys
from pathlib import Path


def direction_vector(azimuth_deg: float, elevation_deg: float):
    azimuth = math.radians(azimuth_deg)
    elevation = math.radians(elevation_deg)
    horizontal = math.cos(elevation)
    return (
        horizontal * math.cos(azimuth),
        horizontal * math.sin(azimuth),
        math.sin(elevation),
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--geometry", type=Path, default=Path(__file__).with_name("mic_geometry.json"))
    parser.add_argument("--azimuth", type=float, default=0.0,
                        help="Look direction in the XY plane, degrees")
    parser.add_argument("--elevation", type=float, default=90.0,
                        help="0 is in-plane; 90 is normal to the PCB")
    args = parser.parse_args()

    config = json.loads(args.geometry.read_text(encoding="utf-8"))
    microphones = config["microphones_mm"]
    if len(microphones) != 4 or any(len(point) != 3 for point in microphones):
        raise SystemExit("microphones_mm must contain four [x, y, z] positions")

    direction = direction_vector(args.azimuth, args.elevation)
    sample_rate = float(config["sample_rate_hz"])
    sound_speed = float(config["speed_of_sound_mm_s"])

    points = [[float(coordinate) for coordinate in point] for point in microphones]
    if len({tuple(point) for point in points}) == 1:
        print(
            "WARNING: microphone coordinates are placeholders; only the "
            "zero-delay broadside result is meaningful.",
            file=sys.stderr,
        )

    projections = []
    for point in points:
        projection_mm = sum(point[i] * direction[i] for i in range(3))
        projections.append(projection_mm * sample_rate / sound_speed)

    # A microphone farther toward the source receives the wave earlier, so it
    # needs the larger compensation delay.
    earliest_projection = min(projections)
    compensation = [value - earliest_projection for value in projections]
    delays_q8 = [max(0, round(value * 256.0)) for value in compensation]

    print("/* Paste into beamformer_config.h */")
    for index, value in enumerate(delays_q8):
        print(f"#define BF_DELAY_Q8_{index} {value}")
    print("\nDelay samples:", ", ".join(f"{value / 256.0:.4f}" for value in delays_q8))


if __name__ == "__main__":
    main()
