---
description: Repository Information Overview
alwaysApply: true
---

# Password Generator Information

## Summary
A secure, portable password generator with encrypted storage built using C++ and Raylib. The application provides password generation with customizable complexity, encrypted password storage, and a clean pixel art UI. It's designed to be lightweight and portable with a single executable containing all embedded assets.

## Structure
- **assets/**: Contains fonts and icons used in the application
- **bin/**: Stores compiled executables and installer
- **build/**: CMake build directory with build artifacts
- **docs/**: Simple website for GitHub Pages
- **raylib/**: External dependency - Raylib graphics library
- **.github/**: GitHub Actions workflow for deploying GitHub Pages

## Language & Runtime
**Language**: C++17
**Compiler**: Microsoft Visual C++ (MSVC)
**Build System**: Manual compilation with cl.exe and CMake
**External Libraries**: Raylib 4.5.0

## Dependencies
**Main Dependencies**:
- Raylib 4.5.0 (Graphics library)
- Standard C++ libraries (random, algorithm, fstream, vector)

## Build & Installation
```bash
# Setup Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# Compile resource file
rc icon.rc

# Compile executable
cl /EHsc /MD /I raylib\include main.cpp icon.res raylib\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib msvcrt.lib /Fe:PassGen.exe /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

# Build NSIS installer (optional)
makensis installer.nsi
```

## Packaging
**Installer**: NSIS (Nullsoft Scriptable Install System)
**Installer Script**: installer.nsi
**Portable Executable**: PassGen.exe (single file with embedded assets)
**Asset Embedding**: Assets are embedded directly into the executable during build

## Release Process
The project includes an automated release script (`push_and_release.bat`) that:
1. Commits and pushes changes to GitHub
2. Creates a new version tag
3. Downloads Raylib if needed
4. Embeds assets into the executable
5. Compiles the application
6. Builds the NSIS installer
7. Creates a GitHub release with the executable and installer

## Security Features
- XOR Encryption for password storage
- Local-only storage in encrypted passwords.dat file
- No network connectivity
- Memory-safe password handling