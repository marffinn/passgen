@echo off
echo Password Generator - Push and Release Script
echo ============================================

:: Get the latest tag
for /f "tokens=*" %%i in ('git describe --tags --abbrev=0 2^>nul') do set LATEST_TAG=%%i

:: If no tags exist, start with v1.0.0
if "%LATEST_TAG%"=="" (
    set NEW_TAG=v1.0.0
    echo No previous tags found. Starting with v1.0.0
) else (
    echo Latest tag: %LATEST_TAG%
    
    :: Extract version numbers (assuming format v1.2.3)
    set TAG_NO_V=%LATEST_TAG:~1%
    for /f "tokens=1,2,3 delims=." %%a in ("%TAG_NO_V%") do (
        set MAJOR=%%a
        set MINOR=%%b
        set PATCH=%%c
    )
    
    :: Increment patch version
    set /a PATCH+=1
    set NEW_TAG=v%MAJOR%.%MINOR%.%PATCH%
)

echo New version will be: %NEW_TAG%
echo.

:: Confirm with user
set /p CONFIRM="Continue with release %NEW_TAG%? (y/n): "
if /i not "%CONFIRM%"=="y" (
    echo Release cancelled.
    pause
    exit /b 1
)

echo.
echo Pushing changes to GitHub...
git add .
git commit -m "Release %NEW_TAG%"
git push origin main

echo.
echo Creating and pushing tag %NEW_TAG%...
git tag %NEW_TAG%
git push origin %NEW_TAG%

echo.
echo ============================================
echo Release %NEW_TAG% has been triggered!
echo Check GitHub Actions for build progress.
echo ============================================
pause