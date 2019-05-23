@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/generate_debug_img.exe
set OUTPUT_DIR=%BASE_PATH%data\preprocessed\debug

%EXE_PATH% %OUTPUT_DIR%

PAUSE
exit
