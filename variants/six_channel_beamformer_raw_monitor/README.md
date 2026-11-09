# XU316 Four-Microphone Beam Plus Raw Six-Channel Firmware

## USB Channel Mapping

| USB channel | Signal |
| --- | --- |
| 1 | Four-microphone broadside beam result |
| 2 | Copy of the beam result |
| 3 | Raw DATA0 PCM reference |
| 4 | Raw DATA1 PCM reference |
| 5 | Raw DATA2 PCM reference |
| 6 | Raw DATA3 PCM reference |

The device uses 48 kHz, 16-bit, six-channel UAC1 capture. It retains the
verified zero-delay four-microphone beam algorithm. The four raw references
are preserved before the beam callback replaces USB channels 1 and 2.

The updated source drives X0D32/LED_R low while lit and blinks it at 2 Hz
only while the USB capture stream is active. X0D33/LED_G remains high.
Apply `source/lib_xua_record_led.patch` to the already SQ66-patched `lib_xua`
before building this LED variant. The previously verified `.xe` remains unchanged.

LED build: `bin/app_six_channel_beam_raw_led_20261109.xe`
SHA-256: `947A24DF384A4AEFDA05B2DB79FFB0E7E9C859F3BF67BBAAD29D2F59B403E70C`
This binary was rebuilt from the included 16-Mbit XN configuration with XTC
15.3.1. The build passed; this exact binary has not yet been programmed to
the board.

On 2026-11-09 the same LED code was tested in a binary built with a 32-Mbit
XN Flash geometry (`CA570B7D3D26950C199D124F3DA9D11B755F138B9368265D7596357E42C908F6`).
It was programmed to a W25Q16JW (ID `EF6015`); the generated 64 KiB image
and Flash readback matched byte-for-byte. With XTAG4 disconnected, the board
cold-booted as a six-channel USB audio input. A 20-second recording confirmed
LED_R blinks during capture and turns off at stream stop. The recorded PCM
was near zero, so microphone audio quality was not evaluated in this LED test.

The archived `.xe` was first verified through XTAG4 with `xrun --io`. On the
2026-08-30 revised board it was subsequently programmed into a W25Q16JW QSPI
Flash and passed readback plus cold-boot USB enumeration. USB DFU is not yet
part of this application, so XTAG4 remains the supported programming path.

## Verified Build

The binary was loaded through XTAG4 to RAM on 2026-08-11. Windows enumerated
`XU316 4Mic Beam + Raw 6Ch 1` as a 48 kHz, 16-bit, six-channel WASAPI input.
A 20-second silence recording completed with no clipping or large transient
events. Beam channels differed by at most two 16-bit LSBs, and all four raw
reference channels carried PCM data.

The revised board later enumerated without XTAG4 as
`XU316 4Mic Beam + Raw 6Ch 1` after a full power cycle. QSPI was operated at
10 MHz; three consecutive 64 KiB readbacks matched the reference image with
zero byte errors.

| File | SHA256 |
| --- | --- |
| `bin/app_six_channel_beam_raw_20260811.xe` | `0347F5A8B7EDE4942FD38320193A1D2F6BE11B8810A8450FCA33276DEF2E2B89` |
| `test_evidence/six_channel_beam_raw_silence_20s_20260811.wav` | `6E538AE74BDD8E0F0AC23EE38F8DB57205BD07ADD743CAE33BB99EDB12A7796F` |

Record from Windows with:

```powershell
python .\host_tools\record_six_channel_beamformer.py -t 10 -o C:\xmos_work\six_channel_recording.wav
```
