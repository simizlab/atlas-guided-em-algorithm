@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/compute_features.exe
set INPUT_IMAGE_DIR=%BASE_PATH%data\preprocessed\org
set OUTPUT_DIR=%BASE_PATH%results\compute_features
set FILENAME_LIST_PATH=%BASE_PATH%data\preprocessed\data_list.txt
set RADIUS=3

%EXE_PATH% %INPUT_IMAGE_DIR% %OUTPUT_DIR% ^
            %FILENAME_LIST_PATH% %RADIUS%

PAUSE
exit
