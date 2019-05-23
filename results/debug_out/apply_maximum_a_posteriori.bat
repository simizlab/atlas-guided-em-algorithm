@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/apply_maximum_a_posteriori.exe
set INPUT_FEATURE_DIR=%BASE_PATH%data\preprocessed\debug
set INPUT_ABD_MASK_DIR=%BASE_PATH%data\preprocessed\debug\mask
set INPUT_PARAM_DIR=%BASE_PATH%results\debug_out\apply_em_algorithm
set INPUT_LABEL_DIR=%BASE_PATH%data\preprocessed\debug\label
set OUTPUT_DIR=%BASE_PATH%results\debug_out\apply_maximum_a_posteriori
set TEST_FILENAME_LIST_PATH=%BASE_PATH%data\preprocessed\debug\training_data_list.txt
set FEATURE_NAME_LIST_PATH=%BASE_PATH%data\preprocessed\debug\feature_name_list.txt
::set INPUT_ATLAS_DIR=%BASE_PATH%results\generate_atlas

%EXE_PATH% %INPUT_FEATURE_DIR% %INPUT_ABD_MASK_DIR% %INPUT_PARAM_DIR% ^
            %INPUT_LABEL_DIR% %OUTPUT_DIR% %TEST_FILENAME_LIST_PATH% ^
            %FEATURE_NAME_LIST_PATH%

PAUSE
exit
