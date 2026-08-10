@echo off
setlocal
set "XE=%~dp0bin\app_four_mic_beamformer_broadside_20260810.xe"

where xrun >nul 2>&1
if errorlevel 1 (
  echo ERROR: xrun was not found. Open an XMOS XTC Tools 15.3.1 command prompt.
  exit /b 1
)

if not exist "%XE%" (
  echo ERROR: Firmware not found: "%XE%"
  exit /b 1
)

if "%~1"=="" goto auto_adapter
xrun --adapter-id "%~1" --io "%XE%"
exit /b %ERRORLEVEL%

:auto_adapter
xrun --io "%XE%"
exit /b %ERRORLEVEL%
