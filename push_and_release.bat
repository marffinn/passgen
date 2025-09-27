@echo off
setlocal

echo [INFO] Password Generator - Automated Release Process
echo =====================================================

REM Check if .env file exists
if not exist ".env" (
    echo [ERROR] .env file not found
    echo [INFO] Create .env file with: GITHUB_TOKEN=your_token_here
    pause
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
    pause
    exit /b 1
)

echo [DEBUG] Token loaded successfully (first 10 chars): %GH_TOKEN:~0,10%...

REM Fetch latest tags
git fetch --tags >nul 2>&1

REM Get latest version tag
set LATEST_TAG=
for /f "delims=" %%i in ('git tag -l "v*.*.*" 2^>nul') do set LATEST_TAG=%%i

REM If no tags exist, start with v1.0.0
if "%LATEST_TAG%"=="" (
    set NEW_TAG=v1.0.0
    echo [INFO] No previous tags found. Starting with v1.0.0
) else (
    echo [INFO] Latest tag: %LATEST_TAG%
    
    REM Simple increment: extract numbers and add 1 to patch
    set TAG_NO_V=%LATEST_TAG:~1%
    for /f "tokens=1,2,3 delims=." %%a in ("%TAG_NO_V%") do (
        set MAJOR=%%a
        set MINOR=%%b
        set PATCH=%%c
    )
    
    REM Increment patch version
    set /a PATCH+=1
    set NEW_TAG=v%MAJOR%.%MINOR%.%PATCH%
)

echo [INFO] New version will be: %NEW_TAG%
echo.

REM Confirm with user
set /p CONFIRM="Continue with release %NEW_TAG%? (y/n): "
if /i not "%CONFIRM%"=="y" (
    echo [INFO] Release cancelled.
    pause
    exit /b 1
)

REM Set up GitHub repository URL with token (token as username, empty password)
set "REPO_URL=https://%GH_TOKEN%@github.com/marffinn/passgen.git"
echo [DEBUG] Using repository URL with token authentication

REM Check if tag already exists
echo [INFO] Checking if tag %NEW_TAG% already exists...
git ls-remote --tags origin %NEW_TAG% | findstr /C:"refs/tags/%NEW_TAG%" >nul 2>&1
if %errorlevel% equ 0 (
    echo [ERROR] Tag %NEW_TAG% already exists on remote!
    echo [INFO] Please delete it manually or wait for auto-increment
    pause
    exit /b 1
)

echo [INFO] Committing and pushing changes...
git add .
git commit -m "Release %NEW_TAG%"
git push %REPO_URL% master
if %errorlevel% neq 0 (
    echo [ERROR] Failed to push changes
    pause
    exit /b 1
)

echo [INFO] Creating new tag %NEW_TAG%...
git tag %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create tag (may already exist locally)
    pause
    exit /b 1
)

echo [INFO] Pushing tag to GitHub...
git push %REPO_URL% %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to push tag
    pause
    exit /b 1
)

echo [INFO] Building executable locally...
if exist "compile.bat" (
    call compile.bat
    if exist "PassGen.exe" (
        echo [SUCCESS] Executable built successfully: PassGen.exe
    ) else (
        echo [ERROR] Build failed - PassGen.exe not found
        pause
        exit /b 1
    )
) else (
    echo [ERROR] compile.bat not found
    pause
    exit /b 1
)

echo [INFO] Creating GitHub release with executable...
gh release create %NEW_TAG% PassGen.exe --title "Password Generator %NEW_TAG%" --notes "Secure password generator with encrypted storage. Download PassGen.exe and run directly - no installation required. Windows 10/11 x64."
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create GitHub release
    echo [INFO] Manual steps:
    echo [INFO] 1. Go to: https://github.com/marffinn/passgen/releases
    echo [INFO] 2. Click "Create a new release"
    echo [INFO] 3. Choose tag: %NEW_TAG%
    echo [INFO] 4. Upload PassGen.exe
    pause
    exit /b 1
)

echo [SUCCESS] Release %NEW_TAG% created successfully!
echo [INFO] Release URL: https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%
echo [INFO] Executable uploaded: PassGen.exe
start https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%
pause