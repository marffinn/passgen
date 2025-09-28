@echo off
setlocal

echo [INFO] Password Generator - Automated Release Process
echo =====================================================

REM Check if .env file exists
if not exist ".env" (
    echo [ERROR] .env file not found
    echo [INFO] Create .env file with: GITHUB_TOKEN=your_token_here
    exit /b 1
)

echo [DEBUG] .env file found, loading token...

REM Load GitHub token from .env file
for /f "tokens=1* delims==" %%a in ('type .env ^| findstr /b "GITHUB_TOKEN="') do (
    set "GH_TOKEN=%%b"
)

if not defined GH_TOKEN (
    echo [ERROR] GITHUB_TOKEN not found in .env file
    echo [DEBUG] .env file contents:
    type .env
    exit /b 1
)

echo [DEBUG] Token loaded successfully (first 10 chars): %GH_TOKEN:~0,10%...

REM Generate automatic version using random number
set /a "BUILD_NUM=%RANDOM%"
set NEW_TAG=v1.0.%BUILD_NUM%

echo [INFO] Auto-generating version: %NEW_TAG%

REM Set up GitHub repository URL with token (token as username, empty password)
set "REPO_URL=https://%GH_TOKEN%@github.com/marffinn/passgen.git"
echo [DEBUG] Using repository URL with token authentication

echo [INFO] Using unique timestamp-based version to avoid conflicts

echo [INFO] Committing and pushing changes...
git add .
git diff --cached --quiet
if %errorlevel% neq 0 (
    git commit -m "Release %NEW_TAG%"
    git push %REPO_URL% master
) else (
    echo [INFO] No changes to commit, proceeding with tag creation
)

echo [INFO] Creating new tag %NEW_TAG%...
REM Delete tag if it exists locally
git tag -d %NEW_TAG% >nul 2>&1
REM Create new tag
git tag %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create tag
    exit /b 1
)
echo [DEBUG] Tag created successfully

echo [INFO] Pushing tag to GitHub...
git push %REPO_URL% %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to push tag
    exit /b 1
)
echo [DEBUG] Tag pushed successfully

echo [INFO] Building executable and installer locally...

REM Download raylib if needed
echo [INFO] Downloading raylib...
if not exist raylib (
    curl -L -o raylib.zip https://github.com/raysan5/raylib/releases/download/4.5.0/raylib-4.5.0_win64_msvc16.zip
    powershell -command "Expand-Archive -Path raylib.zip -DestinationPath . -Force"
    ren raylib-4.5.0_win64_msvc16 raylib
    del raylib.zip
)

REM Setup Visual Studio environment
echo [INFO] Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

REM Check if icon.rc exists in bin\artifacts
if not exist "bin\artifacts\icon.rc" (
    echo [ERROR] icon.rc not found in bin\artifacts
    exit /b 1
)

REM Copy the icon.rc file to the root directory
echo [INFO] Copying icon.rc to root directory...
if exist "icon.rc" del icon.rc
copy bin\artifacts\icon.rc icon.rc >nul

REM Compile resource file
echo [INFO] Compiling resource file...
rc icon.rc

REM Create bin directory structure if it doesn't exist
if not exist "bin" (
    echo [INFO] Creating bin directory...
    mkdir bin
)

if not exist "bin\artifacts" (
    echo [INFO] Creating bin\artifacts directory...
    mkdir bin\artifacts
)

REM Embed assets only if the file doesn't exist
if not exist "bin\artifacts\embedded_assets.h" (
    echo [INFO] Embedding assets...
    echo // Auto-generated embedded assets > bin\artifacts\embedded_assets.h
    echo. >> bin\artifacts\embedded_assets.h

    REM Embed font
    echo const unsigned char FONT_DATA[] = { >> bin\artifacts\embedded_assets.h
    powershell -Command "$bytes = [System.IO.File]::ReadAllBytes('assets/fonts/FreePixel.ttf'); for($i=0; $i -lt $bytes.Length; $i++) { if($i -gt 0) { Write-Host -NoNewline ',' }; Write-Host -NoNewline ('0x{0:X2}' -f $bytes[$i]) }" >> bin\artifacts\embedded_assets.h
    echo. >> bin\artifacts\embedded_assets.h
    echo }; >> bin\artifacts\embedded_assets.h

    powershell -Command "Write-Host ('const int FONT_SIZE = {0};' -f [System.IO.File]::ReadAllBytes('assets/fonts/FreePixel.ttf').Length)" >> bin\artifacts\embedded_assets.h
    echo. >> bin\artifacts\embedded_assets.h

    REM Embed icon
    echo const unsigned char ICON_DATA[] = { >> bin\artifacts\embedded_assets.h
    powershell -Command "$bytes = [System.IO.File]::ReadAllBytes('assets/icons/password_64x64.png'); for($i=0; $i -lt $bytes.Length; $i++) { if($i -gt 0) { Write-Host -NoNewline ',' }; Write-Host -NoNewline ('0x{0:X2}' -f $bytes[$i]) }" >> bin\artifacts\embedded_assets.h
    echo. >> bin\artifacts\embedded_assets.h
    echo }; >> bin\artifacts\embedded_assets.h

    powershell -Command "Write-Host ('const int ICON_SIZE = {0};' -f [System.IO.File]::ReadAllBytes('assets/icons/password_64x64.png').Length)" >> bin\artifacts\embedded_assets.h

    echo [DEBUG] Assets embedded successfully
) else (
    echo [INFO] Using existing embedded_assets.h file from bin\artifacts
)

REM Copy the embedded_assets.h file to the root directory for compilation
echo [INFO] Copying embedded_assets.h to root directory...
copy bin\artifacts\embedded_assets.h embedded_assets.h >nul

REM Compile executable
echo [INFO] Compiling executable...
cl /EHsc /MD /I raylib\include main.cpp icon.res raylib\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib msvcrt.lib /Fe:bin\PassGen.exe /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

REM Clean up build artifacts
del main.obj 2>nul

REM Move icon.res to bin\artifacts after compilation
echo [INFO] Moving icon.res to bin\artifacts...
copy icon.res bin\artifacts\icon.res >nul
del icon.res 2>nul

REM Clean up temporary files in root directory
echo [INFO] Cleaning up temporary files from root directory...
del icon.rc 2>nul
del embedded_assets.h 2>nul

if exist "bin\PassGen.exe" (
    echo [SUCCESS] Executable built successfully: bin\PassGen.exe
    echo [INFO] Copying executable to root for release...
    copy bin\PassGen.exe PassGen.exe >nul
) else (
    echo [ERROR] Build failed - bin\PassGen.exe not found
    exit /b 1
)

REM Copy installer.nsi to bin\artifacts if it doesn't exist there
if not exist "bin\artifacts\installer.nsi" (
    echo [INFO] Copying installer.nsi to bin\artifacts...
    copy installer.nsi bin\artifacts\installer.nsi >nul
)

REM Copy installer.nsi from bin\artifacts to root for building
echo [INFO] Copying installer.nsi to root directory...
copy bin\artifacts\installer.nsi installer.nsi >nul

REM Build NSIS installer
echo [INFO] Building NSIS installer...
where makensis >nul 2>&1
if %errorlevel% equ 0 (
    makensis installer.nsi
    if exist "PassGenInstaller.exe" (
        echo [SUCCESS] Installer built successfully: PassGenInstaller.exe
        echo [INFO] Moving PassGenInstaller.exe to bin directory...
        move /y PassGenInstaller.exe bin\PassGenInstaller.exe >nul
    ) else (
        echo [WARNING] Installer build failed - continuing without installer
    )
) else (
    echo [WARNING] NSIS not found - skipping installer build
)

echo [INFO] Creating GitHub release with executable and installer...
if exist "bin\PassGenInstaller.exe" (
    echo [INFO] Copying executables to root for release...
    copy bin\PassGen.exe PassGen.exe >nul
    copy bin\PassGenInstaller.exe PassGenInstaller.exe >nul
    
    echo [DEBUG] Uploading both PassGen.exe and PassGenInstaller.exe
    gh release create %NEW_TAG% PassGen.exe PassGenInstaller.exe --title "Password Generator %NEW_TAG%" --notes "Secure password generator with custom encryption. Download PassGen.exe (portable) or PassGenInstaller.exe (installer). Windows 10/11 x64."
) else (
    echo [INFO] Copying executable to root for release...
    copy bin\PassGen.exe PassGen.exe >nul
    
    echo [DEBUG] Uploading only PassGen.exe
    gh release create %NEW_TAG% PassGen.exe --title "Password Generator %NEW_TAG%" --notes "Secure password generator with custom encryption. Download PassGen.exe and run directly - no installation required. Windows 10/11 x64."
)
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create GitHub release (exit code: %errorlevel%)
    echo [INFO] Manual steps:
    echo [INFO] 1. Go to: https://github.com/marffinn/passgen/releases
    echo [INFO] 2. Click "Create a new release"
    echo [INFO] 3. Choose tag: %NEW_TAG%
    echo [INFO] 4. Upload PassGen.exe and PassGenInstaller.exe
    exit /b 1
) else (
    echo [DEBUG] GitHub release created successfully
)

echo [SUCCESS] Release %NEW_TAG% created successfully!
echo [INFO] Release URL: https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%
echo [INFO] Executable uploaded: PassGen.exe

echo [INFO] Cleaning up build artifacts...
del PassGen.exe 2>nul
del PassGenInstaller.exe 2>nul
del embedded_assets.h 2>nul
del icon.rc 2>nul
del installer.nsi 2>nul
REM Keep bin\artifacts\embedded_assets.h, bin\artifacts\icon.rc, and bin\artifacts\installer.nsi for future builds
REM Keep the bin directory with the executable for local testing
rmdir /s /q raylib 2>nul

start https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%
