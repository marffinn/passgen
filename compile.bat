@echo off
echo Downloading raylib...
if not exist raylib (
    curl -L -o raylib.zip https://github.com/raysan5/raylib/releases/download/4.5.0/raylib-4.5.0_win64_msvc16.zip
    powershell -command "Expand-Archive -Path raylib.zip -DestinationPath . -Force"
    ren raylib-4.5.0_win64_msvc16 raylib
    del raylib.zip
)

echo Using default raylib font...

echo Using existing PNG icon...

echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

echo Compiling resource file...
rc icon.rc

echo Embedding assets...
call embed_assets.bat

echo Compiling...
cl /EHsc /MD /I raylib\include main.cpp icon.res raylib\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib msvcrt.lib /Fe:PassGen.exe /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

echo Cleaning up build artifacts...
del main.obj 2>nul
del icon.res 2>nul

echo.
if exist PassGen.exe (
    echo Build successful! Run: PassGen.exe
) else (
    echo Build failed!
)