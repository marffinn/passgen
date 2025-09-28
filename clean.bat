@echo off
setlocal

echo [INFO] Password Generator - Cleanup Script
echo =======================================

echo [INFO] Cleaning up build artifacts...

REM Remove temporary files from root directory
del main.obj 2>nul
del icon.res 2>nul
del icon.rc 2>nul
del PassGen.exe 2>nul
del PassGenInstaller.exe 2>nul
del embedded_assets.h 2>nul
del installer.nsi 2>nul

REM Ask if user wants to clean bin directory
set /p CLEAN_BIN="Do you want to clean the bin directory? (y/n): "
if /i "%CLEAN_BIN%"=="y" (
    echo [INFO] Cleaning bin directory...
    if exist "bin\PassGen.exe" del bin\PassGen.exe 2>nul
    if exist "bin\PassGenInstaller.exe" del bin\PassGenInstaller.exe 2>nul
    
    REM Ask if user wants to clean artifacts
    set /p CLEAN_ARTIFACTS="Do you want to clean embedded artifacts too? (y/n): "
    if /i "%CLEAN_ARTIFACTS%"=="y" (
        echo [INFO] Cleaning embedded artifacts...
        if exist "bin\artifacts\embedded_assets.h" del bin\artifacts\embedded_assets.h 2>nul
        if exist "bin\artifacts\icon.rc" del bin\artifacts\icon.rc 2>nul
        if exist "bin\artifacts\icon.res" del bin\artifacts\icon.res 2>nul
        if exist "bin\artifacts\installer.nsi" del bin\artifacts\installer.nsi 2>nul
        if exist "bin\artifacts" rmdir bin\artifacts 2>nul
    )
    
    REM Remove bin directory if empty
    dir /b bin 2>nul | findstr "^" >nul || rmdir bin 2>nul
)

REM Ask if user wants to clean raylib
set /p CLEAN_RAYLIB="Do you want to clean raylib? (y/n): "
if /i "%CLEAN_RAYLIB%"=="y" (
    echo [INFO] Cleaning raylib...
    rmdir /s /q raylib 2>nul
)

echo [SUCCESS] Cleanup completed!