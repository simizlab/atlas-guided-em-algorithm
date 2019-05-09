@echo off
set A=%~dp0
set A=%A:~0,-2%
for %%A in (%A%) do set A=%%~dpA

set BASE_PATH=%A%
set EXE_PATH=%BASE_PATH%x64/Release/apply_em_algorithm.exe
set INPUT_FEATURE_DIR=%BASE_PATH%results\compute_features
set INPUT_ABD_MASK_DIR=%BASE_PATH%data\preprocessed\abdominal_cavity_mask
set INPUT_PARAM_DIR=%BASE_PATH%results\calc_init_val
set OUTPUT_DIR=%BASE_PATH%results\apply_em_algorithm\using_atlas
set TEST_FILENAME_LIST_PATH=%BASE_PATH%data\preprocessed\test_data_list.txt
set FEATURE_NAME_LIST_PATH=%BASE_PATH%results\compute_features\feature_name_list.txt
set INPUT_ATLAS_DIR=%BASE_PATH%results\generate_atlas

%EXE_PATH% %INPUT_FEATURE_DIR% %INPUT_ABD_MASK_DIR% %INPUT_PARAM_DIR% ^
            %OUTPUT_DIR% %TEST_FILENAME_LIST_PATH% %FEATURE_NAME_LIST_PATH% ^
            %INPUT_ATLAS_DIR%
PAUSE
exit
