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
git tag %NEW_TAG%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create tag (may already exist locally)
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
if exist "compile.bat" (
    call compile.bat
    if exist "PassGen.exe" (
        echo [SUCCESS] Executable built successfully: PassGen.exe
    ) else (
        echo [ERROR] Build failed - PassGen.exe not found
        exit /b 1
    )
) else (
    echo [ERROR] compile.bat not found
    exit /b 1
)

echo [INFO] Building NSIS installer...
if exist "build_installer.bat" (
    call build_installer.bat
    if exist "PassGenInstaller.exe" (
        echo [SUCCESS] Installer built successfully: PassGenInstaller.exe
    ) else (
        echo [WARNING] Installer build failed - continuing without installer
    )
) else (
    echo [WARNING] build_installer.bat not found - skipping installer
)

echo [INFO] Creating GitHub release with executable and installer...
if exist "PassGenInstaller.exe" (
    gh release create %NEW_TAG% PassGen.exe PassGenInstaller.exe --title "Password Generator %NEW_TAG%" --notes "Secure password generator with encrypted storage. Download PassGen.exe (portable) or PassGenInstaller.exe (installer). Windows 10/11 x64."
) else (
    gh release create %NEW_TAG% PassGen.exe --title "Password Generator %NEW_TAG%" --notes "Secure password generator with encrypted storage. Download PassGen.exe and run directly - no installation required. Windows 10/11 x64."
)
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create GitHub release
    echo [INFO] Manual steps:
    echo [INFO] 1. Go to: https://github.com/marffinn/passgen/releases
    echo [INFO] 2. Click "Create a new release"
    echo [INFO] 3. Choose tag: %NEW_TAG%
    echo [INFO] 4. Upload PassGen.exe
    exit /b 1
)

echo [SUCCESS] Release %NEW_TAG% created successfully!
echo [INFO] Release URL: https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%
echo [INFO] Executable uploaded: PassGen.exe
start https://github.com/marffinn/passgen/releases/tag/%NEW_TAG%