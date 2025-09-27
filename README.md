# Password Generator

A secure, portable password generator with encrypted storage built using C++ and Raylib.

![Password Generator Screenshot](https://img.shields.io/badge/Platform-Windows-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Features

- **🔐 Secure Password Generation**: Generate passwords from 4-50 characters with customizable complexity
- **📚 Encrypted Password Library**: Store and manage passwords with XOR encryption
- **🎨 Clean Pixel Art UI**: Retro-style interface with embedded custom fonts
- **📱 Portable**: Single executable with all assets embedded
- **⚡ Fast & Lightweight**: Built with Raylib for optimal performance
- **🔄 Real-time Generation**: Generate passwords instantly with spacebar
- **📋 One-Click Copy**: Copy passwords to clipboard with a single click

## Screenshots

### Main Generator
- Adjustable password length slider (4-50 characters)
- Real-time password generation
- Instant clipboard copying

### Password Library
- Encrypted password storage
- Editable service names
- Individual password generation per service
- Scrollable interface for unlimited entries

## Usage

### Password Generation
- **Adjust Length**: Use the slider to set password length (4-50 characters)
- **Generate**: Click "Generate" button or press `SPACE`/`ENTER`
- **Copy**: Click "COPY" button or press `C` to copy to clipboard

### Password Library
- **Access Library**: Click "LIBRARY" button from main screen
- **Add Entry**: Click "ADD NEW" to create a new password entry
- **Edit Service Name**: Click on any service name to edit it
- **Generate New Password**: Click "GEN" button for any entry
- **Copy Password**: Click "COPY" button to copy full password
- **Delete Entry**: Click "DEL" button to remove an entry
- **Navigate**: Use mouse wheel to scroll through entries

### Keyboard Shortcuts
- `SPACE` / `ENTER` - Generate new password
- `C` - Copy current password to clipboard
- `ESC` - Cancel editing (in library)

## Security

- **XOR Encryption**: Password library is encrypted using XOR cipher
- **Local Storage**: All data stored locally in encrypted `passwords.dat` file
- **No Network**: Application works completely offline
- **Memory Safe**: Passwords cleared from memory when not in use

## File Structure

```
passgen/
├── main.cpp              # Main application source
├── assets/
│   ├── fonts/
│   │   └── FreePixel.ttf # Custom pixel font
│   └── icons/
│       ├── password_64x64.png # PNG icon
│       └── password_32x32.ico # ICO icon
└── README.md
```

## Technical Details

- **Language**: C++17
- **Graphics**: Raylib 4.x
- **Platform**: Windows (x64)
- **Encryption**: XOR cipher with 0x7F key
- **UI**: Custom pixel-perfect interface

## License

MIT License - feel free to use and modify for your projects.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Acknowledgments

- **Raylib** - Amazing graphics library
- **FreePixel Font** - Crisp pixel art typography
- Built with ❤️ for secure password management