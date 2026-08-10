import argparse
import time
import wave
from pathlib import Path

import sounddevice as sd


SAMPLE_RATE = 48000
CHANNELS = 2


def find_xu316(timeout=30):
    deadline = time.monotonic() + timeout
    waiting = False

    while True:
        devices = sd.query_devices()
        host_apis = sd.query_hostapis()

        for index, device in enumerate(devices):
            host_api = host_apis[device["hostapi"]]["name"]
            name = device["name"]
            if (
                device["max_input_channels"] >= CHANNELS
                and host_api == "Windows WASAPI"
                and "4Mic Broadside Beam" in name
            ):
                return index, name

        if time.monotonic() >= deadline:
            raise RuntimeError(
                "XU316 broadside beamformer was not found after 30 seconds. "
                "Start the firmware with xrun --io first."
            )

        if not waiting:
            print("Waiting for the XU316 recording endpoint...")
            waiting = True
        time.sleep(1)
        sd._terminate()
        sd._initialize()


def parse_args():
    parser = argparse.ArgumentParser(
        description="Record the duplicated stereo output of the XU316 beamformer."
    )
    parser.add_argument(
        "-t", "--seconds", type=float, default=10.0,
        help="Recording duration in seconds (default: 10).",
    )
    parser.add_argument(
        "-o", "--output", default="xu316_beamformer_recording.wav",
        help="Output WAV filename.",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    if args.seconds <= 0:
        raise ValueError("Recording duration must be greater than zero.")

    device_index, device_name = find_xu316()
    frame_count = round(SAMPLE_RATE * args.seconds)

    output_path = Path(args.output)
    if not output_path.is_absolute():
        output_path = Path(__file__).resolve().parent / output_path
    output_path.parent.mkdir(parents=True, exist_ok=True)

    print(f"Device: {device_index} - {device_name}")
    print("Channels: processed beam, processed beam")
    print(f"Recording {args.seconds:g} seconds at {SAMPLE_RATE} Hz...")

    audio = sd.rec(
        frame_count,
        samplerate=SAMPLE_RATE,
        channels=CHANNELS,
        dtype="int16",
        device=device_index,
    )
    sd.wait()

    with wave.open(str(output_path), "wb") as wav_file:
        wav_file.setnchannels(CHANNELS)
        wav_file.setsampwidth(2)
        wav_file.setframerate(SAMPLE_RATE)
        wav_file.writeframes(audio.tobytes())

    print(f"Saved: {output_path}")


if __name__ == "__main__":
    main()
