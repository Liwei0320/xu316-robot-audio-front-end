@echo off
setlocal
set ADAPTER=%~1
if "%ADAPTER%"=="" set ADAPTER=U8NUL5P2
"C:\Program Files\XMOS\XTC\15.3.1\bin\xrun.exe" --adapter-id %ADAPTER% --io "%~dp0bin\app_six_channel_beam_raw_20260811.xe"
