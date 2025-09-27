@echo off
setlocal

echo [INFO] Password Generator - Automated Release Process
echo =====================================================

REM Load GitHub token from .env file
for /f "tokens=1* delims==" %%a in ('type .env ^| findstr /b "GITHUB_TOKEN="') do (
    set "GH_TOKEN=%%b"
)

if not defined GH_TOKEN (
    echo [ERROR] GITHUB_TOKEN not found in .env file
    echo [INFO] Create .env file with: GITHUB_TOKEN=your_token_here
    pause
    exit /b 1
)

REM Get the latest tag
for /f "tokens=*" %%i in ('git describe --tags --abbrev=0 2^>nul') do set LATEST_TAG=%%i

REM If no tags exist, start with v1.0.0
if "%LATEST_TAG%"=="" (
    set NEW_TAG=v1.0.0
    echo [INFO] No previous tags found. Starting with v1.0.0
) else (
    echo [INFO] Latest tag: %LATEST_TAG%
    
    REM Extract version numbers (assuming format v1.2.3)
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

REM Set up GitHub repository URL with token
set "REPO_URL=https://marffinn:%GH_TOKEN%@github.com/marffinn/passgen.git"

REM Check if tag exists and delete if necessary
echo [INFO] Checking for existing tag %NEW_TAG%...
git tag -l %NEW_TAG% >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] Deleting existing local tag %NEW_TAG%...
    git tag -d %NEW_TAG%
)

git ls-remote --tags origin %NEW_TAG% | findstr /C:"refs/tags/%NEW_TAG%" >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] Deleting existing remote tag %NEW_TAG%...
    git push %REPO_URL% :refs/tags/%NEW_TAG%
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

echo [INFO] Creating tag %NEW_TAG%...
git tag %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create tag
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

echo [SUCCESS] Release %NEW_TAG% created successfully!
echo [INFO] GitHub Actions should trigger automatically to create the release
echo [INFO] Check: https://github.com/marffinn/passgen/releases
pause