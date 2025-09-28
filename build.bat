@echo off
setlocal

REM Build script for Password Generator
REM Only compiles the executable, does not push, tag, or release

REM Ensure bin directory exists
if not exist bin mkdir bin

REM Asset embedding step (refactored for batch parenthesis safety)
if not exist embedded_assets.h goto embedAssets
set "progress=30"
set "progressMsg=Skipping asset embedding (already exists)..."
call :showProgress

set "progress=0"
set "progressMsg=Starting build..."
call :showProgress

REM Download raylib if needed
if not exist raylib\lib\raylib.lib (
    set "progressMsg=Downloading raylib..."
    call :spinnerStart
    curl -L -o raylib.zip https://github.com/raysan5/raylib/releases/download/4.5.0/raylib-4.5.0_win64_msvc16.zip >nul 2>&1
    powershell -command "Expand-Archive -Path raylib.zip -DestinationPath . -Force" >nul 2>&1
    del raylib.zip
    REM Robustly rename extracted folder if needed
    if exist raylib-4.5.0_win64_msvc16 (
        rmdir /s /q raylib 2>nul
        ren raylib-4.5.0_win64_msvc16 raylib
    )
    REM Check for raylib.lib
    if not exist raylib\lib\raylib.lib (
        echo [ERROR] raylib.lib not found after extraction. Please check the archive contents.
        exit /b 1
    )
    call :spinnerStop
)

set "progress=20"
set "progressMsg=Setting up Visual Studio environment..."
call :showProgress
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

set "progress=50"
set "progressMsg=Compiling resource file..."
call :spinnerStart
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\rc.exe" icon.rc >nul 2>&1
call :spinnerStop
call :showProgress

set "progress=70"
set "progressMsg=Compiling executable..."
call :spinnerStart
cl /EHsc /MD /I raylib\include main.cpp icon.res raylib\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib msvcrt.lib /Fe:bin\PassGen.exe /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup >nul 2>&1
call :spinnerStop
call :showProgress

set "progress=90"
set "progressMsg=Cleaning up build artifacts..."
call :showProgress

REM Clean up build artifacts
del main.obj 2>nul
del icon.res 2>nul
REM Do not delete embedded_assets.h

set "progress=100"
set "progressMsg=Build complete! Output: bin\PassGen.exe"
call :showProgress

echo [SUCCESS] Build complete. Output: bin\PassGen.exe
exit /b 0

:embedAssets
set "progress=30"
set "progressMsg=Embedding assets..."
call :showProgress
@echo // Auto-generated embedded assets > embedded_assets.h
@echo const unsigned char FONT_DATA[] = { >> embedded_assets.h
powershell -Command "$bytes = [System.IO.File]::ReadAllBytes('assets/fonts/FreePixel.ttf'); for($i=0; $i -lt $bytes.Length; $i++) { if($i -gt 0) { Write-Host -NoNewline ',' }; Write-Host -NoNewline ('0x{0:X2}' -f $bytes[$i]) }" >> embedded_assets.h
@echo. >> embedded_assets.h
@echo }; >> embedded_assets.h
powershell -Command "Write-Host ('const int FONT_SIZE = {0};' -f [System.IO.File]::ReadAllBytes('assets/fonts/FreePixel.ttf').Length)" >> embedded_assets.h
@echo. >> embedded_assets.h
@echo const unsigned char ICON_DATA[] = { >> embedded_assets.h
powershell -Command "$bytes = [System.IO.File]::ReadAllBytes('assets/icons/password_64x64.png'); for($i=0; $i -lt $bytes.Length; $i++) { if($i -gt 0) { Write-Host -NoNewline ',' }; Write-Host -NoNewline ('0x{0:X2}' -f $bytes[$i]) }" >> embedded_assets.h
@echo. >> embedded_assets.h
@echo }; >> embedded_assets.h
powershell -Command "Write-Host ('const int ICON_SIZE = {0};' -f [System.IO.File]::ReadAllBytes('assets/icons/password_64x64.png').Length)" >> embedded_assets.h
@echo. >> embedded_assets.h
exit /b 0

:showProgress
setlocal enabledelayedexpansion
set /a barLen=!progress!/10
set "bar="
for /l %%i in (1,1,!barLen!) do set "bar=!bar!#"
for /l %%i in (!barLen!,1,10) do set "bar=!bar!-"
echo [PROGRESS] !progressMsg! [!bar!] !progress!%%
endlocal
exit /b 0

:spinnerStart
setlocal enabledelayedexpansion
set "spinner=|/-\\"
set "spinIdx=0"
set "spinActive=1"
for /l %%i in (1,1,1000) do (
    if not defined spinActive exit /b 0
    set /a spinIdx=(%%i %% 4)
    set "spinChar=!spinner:~%spinIdx%,1!"
    <nul set /p="[WORKING] !progressMsg! !spinChar!\r"
    ping -n 2 127.0.0.1 >nul
)
endlocal
exit /b 0

:spinnerStop
set "spinActive="
echo.
exit /b 0
