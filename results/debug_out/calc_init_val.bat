@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/calc_init_val.exe
set INPUT_FEATURE_DIR=%BASE_PATH%data\preprocessed\debug
set INPUT_LABEL_DIR=%BASE_PATH%data\preprocessed\debug\label
set INPUT_ABD_MASK_DIR=%BASE_PATH%data\preprocessed\debug\mask
set OUTPUT_DIR=%BASE_PATH%results\debug\calc_init_val
set TRAINING_FILENAME_LIST_PATH=%BASE_PATH%data\preprocessed\debug\training_data_list.txt
set FEATURE_NAME_LIST_PATH=%BASE_PATH%data\preprocessed\debug\feature_name_list.txt
set DISTANCE_MARGIN=0

%EXE_PATH% %INPUT_FEATURE_DIR% %INPUT_LABEL_DIR%  %INPUT_ABD_MASK_DIR% ^
            %OUTPUT_DIR% %TRAINING_FILENAME_LIST_PATH% ^
            %FEATURE_NAME_LIST_PATH% %DISTANCE_MARGIN%
PAUSE
exit
