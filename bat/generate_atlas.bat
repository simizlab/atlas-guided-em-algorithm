@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/generate_atlas.exe
set INPUT_IMAGE_DIR=%BASE_PATH%data\preprocessed\label
set OUTPUT_DIR=%BASE_PATH%results\generate_atlas
set FILENAME_LIST_PATH=%BASE_PATH%data\preprocessed\training_data_list.txt
set SIGMA=1

%EXE_PATH% %INPUT_IMAGE_DIR% %OUTPUT_DIR% %FILENAME_LIST_PATH% %SIGMA%

PAUSE
exit
