#include "raylib.h"
#include <string>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

class PasswordGenerator {
private:
    std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::random_device rd;
    std::mt19937 gen;
    
public:
    PasswordGenerator() : gen(rd()) {}
    
    std::string generate(int length) {
        std::string password;
        std::uniform_int_distribution<> dis(0, chars.size() - 1);
        
        for (int i = 0; i < length; ++i) {
            password += chars[dis(gen)];
        }
        return password;
    }
};

// Custom multi-layer encryption
class CustomCrypto {
private:
    static const std::string key;
    static const unsigned char sbox[256];
    
    unsigned char rotateLeft(unsigned char value, int shift) {
        return (value << shift) | (value >> (8 - shift));
    }
    
    unsigned char rotateRight(unsigned char value, int shift) {
        return (value >> shift) | (value << (8 - shift));
    }
    
public:
    std::string encrypt(const std::string& data) {
        std::string result = data;
        
        // Layer 1: Substitution with S-box
        for (size_t i = 0; i < result.length(); i++) {
            result[i] = sbox[(unsigned char)result[i]];
        }
        
        // Layer 2: Key-based XOR with position dependency
        for (size_t i = 0; i < result.length(); i++) {
            result[i] ^= key[i % key.length()] ^ (i & 0xFF);
        }
        
        // Layer 3: Bit rotation based on position
        for (size_t i = 0; i < result.length(); i++) {
            result[i] = rotateLeft(result[i], (i % 7) + 1);
        }
        
        // Layer 4: Reverse and scramble
        std::reverse(result.begin(), result.end());
        for (size_t i = 0; i < result.length() - 1; i += 2) {
            std::swap(result[i], result[i + 1]);
        }
        
        return result;
    }
    
    std::string decrypt(const std::string& data) {
        std::string result = data;
        
        // Reverse Layer 4: Unscramble and reverse
        for (size_t i = 0; i < result.length() - 1; i += 2) {
            std::swap(result[i], result[i + 1]);
        }
        std::reverse(result.begin(), result.end());
        
        // Reverse Layer 3: Reverse bit rotation
        for (size_t i = 0; i < result.length(); i++) {
            result[i] = rotateRight(result[i], (i % 7) + 1);
        }
        
        // Reverse Layer 2: Key-based XOR
        for (size_t i = 0; i < result.length(); i++) {
            result[i] ^= key[i % key.length()] ^ (i & 0xFF);
        }
        
        // Reverse Layer 1: Inverse S-box substitution
        for (size_t i = 0; i < result.length(); i++) {
            for (int j = 0; j < 256; j++) {
                if (sbox[j] == (unsigned char)result[i]) {
                    result[i] = j;
                    break;
                }
            }
        }
        
        return result;
    }
};

// Static members
const std::string CustomCrypto::key = "PassGen2024!SecureKey#";
const unsigned char CustomCrypto::sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x89, 0xb9, 0xc1, 0x56, 0x65, 0x4e, 0xbd, 0xc6, 0x91,
    0x08, 0xa9, 0x03, 0x0d, 0x42, 0x1c, 0x35, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4,
    0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74,
    0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57,
    0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87,
    0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d
};

static CustomCrypto crypto;

std::string encrypt(const std::string& data) {
    return crypto.encrypt(data);
}

std::string decrypt(const std::string& data) {
    return crypto.decrypt(data);
}

int main() {
    const int screenWidth = 450;
    const int screenHeight = 280;
    
    InitWindow(screenWidth, screenHeight, "Password Generator");
    SetTargetFPS(60);
    
    // Load FreePixel fonts from external files
    Font font20, font16, font14, font12, font10;
    
    if (FileExists("FreePixel.ttf")) {
        font20 = LoadFontEx("FreePixel.ttf", 20, 0, 95);
        font16 = LoadFontEx("FreePixel.ttf", 16, 0, 95);
        font14 = LoadFontEx("FreePixel.ttf", 14, 0, 95);
        font12 = LoadFontEx("FreePixel.ttf", 12, 0, 95);
        font10 = LoadFontEx("FreePixel.ttf", 10, 0, 95);
        
        SetTextureFilter(font20.texture, TEXTURE_FILTER_POINT);
        SetTextureFilter(font16.texture, TEXTURE_FILTER_POINT);
        SetTextureFilter(font14.texture, TEXTURE_FILTER_POINT);
        SetTextureFilter(font12.texture, TEXTURE_FILTER_POINT);
        SetTextureFilter(font10.texture, TEXTURE_FILTER_POINT);
    } else {
        // Fallback to default font if FreePixel.ttf not found
        font20 = GetFontDefault();
        font16 = GetFontDefault();
        font14 = GetFontDefault();
        font12 = GetFontDefault();
        font10 = GetFontDefault();
    }
    
    // Set window icon from external file
    if (FileExists("password_64x64.png")) {
        Image iconImage = LoadImage("password_64x64.png");
        if (iconImage.data != NULL) {
            SetWindowIcon(iconImage);
            UnloadImage(iconImage);
        }
    }
    
    PasswordGenerator passGen;
    std::string password = "";
    int passwordLength = 12;
    bool copied = false;
    int copiedTimer = 0;
    bool showLibrary = false;
    int editingIndex = -1;
    char editBuffer[32] = "";
    int scrollOffset = 0;
    std::vector<std::string> libraryPasswords = {"aBc123XyZ!", "P@ssW0rd789", "SecureKey456", "MyS3cur3P@ss"};
    std::vector<std::string> serviceNames = {"facebook", "gmail", "github", "twitter"};
    
    // Load from encrypted file
    std::ifstream inFile("passwords.dat", std::ios::binary);
    if (inFile.is_open()) {
        serviceNames.clear();
        libraryPasswords.clear();
        std::string encryptedData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        
        if (!encryptedData.empty()) {
            std::string decryptedData = decrypt(encryptedData);
            std::istringstream iss(decryptedData);
            std::string line;
            while (std::getline(iss, line)) {
                size_t pos = line.find('|');
                if (pos != std::string::npos) {
                    serviceNames.push_back(line.substr(0, pos));
                    libraryPasswords.push_back(line.substr(pos + 1));
                }
            }
        }
    }
    
    while (!WindowShouldClose()) {
        // Center calculations
        float centerX = screenWidth / 2.0f;
        
        // Slider for password length (wider)
        Rectangle sliderBar = {centerX - 120.0f, 68.0f, 240.0f, 10.0f};
        Rectangle sliderKnob = {centerX - 120.0f + (passwordLength - 4) * 240.0f / 46.0f - 5.0f, 63.0f, 10.0f, 20.0f};
        
        if (CheckCollisionPointRec(GetMousePosition(), sliderBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float mouseX = GetMousePosition().x;
            passwordLength = 4 + (int)((mouseX - (centerX - 120.0f)) * 46.0f / 240.0f);
            if (passwordLength < 4) passwordLength = 4;
            if (passwordLength > 50) passwordLength = 50;
        }
        
        // Input handling
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            password = passGen.generate(passwordLength);
            copied = false;
        }
        if (IsKeyPressed(KEY_C) && !password.empty()) {
            SetClipboardText(password.c_str());
            copied = true;
            copiedTimer = 120; // 2 seconds at 60 FPS
        }
        
        if (copiedTimer > 0) copiedTimer--;
        if (copiedTimer == 0) copied = false;
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Top bar with title (centered)
        DrawRectangle(0, 0, screenWidth, 35, DARKBLUE);
        Vector2 titleSize = MeasureTextEx(font16, "Password Generator", 16, 0);
        DrawTextEx(font16, "Password Generator", {centerX - titleSize.x/2, 10}, 16, 0, WHITE);
        
        // Password length with slider (centered)
        const char* lengthText = TextFormat("Length: %d", passwordLength);
        Vector2 lengthSize = MeasureTextEx(font16, lengthText, 16, 0);
        DrawTextEx(font16, lengthText, {centerX - lengthSize.x/2, 45}, 16, 0, BLACK);
        
        DrawRectangleRec(sliderBar, LIGHTGRAY);
        DrawRectangleRec(sliderKnob, DARKBLUE);
        DrawTextEx(font16, "4", {centerX - 140.0f, 65.0f}, 16, 0, GRAY);
        DrawTextEx(font16, "50", {centerX + 130.0f, 65.0f}, 16, 0, GRAY);
        
        // Generate button area (auto-width, centered)
        Vector2 genSize = MeasureTextEx(font16, "Generate (SPACE)", 16, 0);
        float buttonWidth = genSize.x + 20.0f; // Add padding
        Rectangle genButton = {centerX - buttonWidth/2.0f, 95.0f, buttonWidth, 30.0f};
        DrawRectangleRec(genButton, DARKBLUE);
        float genButtonCenterY = genButton.y + genButton.height/2.0f - genSize.y/2.0f;
        DrawTextEx(font16, "Generate (SPACE)", {centerX - genSize.x/2, genButtonCenterY}, 16, 0, WHITE);
        
        if (CheckCollisionPointRec(GetMousePosition(), genButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            password = passGen.generate(passwordLength);
            copied = false;
        }
        
        if (!showLibrary) {
            // Main generator view
            Rectangle passBox = {15.0f, 140.0f, (float)(screenWidth - 30), 45.0f};
            DrawRectangleRec(passBox, WHITE);
            DrawRectangleLinesEx(passBox, 1, DARKGRAY);
            
            if (!password.empty()) {
                const char* passText = password.c_str();
                Vector2 textSize = MeasureTextEx(font16, passText, 16, 0);
                if (textSize.x > screenWidth - 50) {
                    DrawTextEx(font16, passText, {centerX - textSize.x/2.0f, 150.0f}, 12, 0, DARKGREEN);
                } else {
                    DrawTextEx(font16, passText, {centerX - textSize.x/2.0f, 155.0f}, 16, 0, DARKGREEN);
                }
            } else {
                DrawTextEx(font16, "Generated password appears here", {centerX - 140.0f, 155.0f}, 16, 0, GRAY);
            }
            
            // Copy and Library buttons
            Rectangle copyButton = {15.0f, 200.0f, 200.0f, 30.0f};
            Rectangle libraryButton = {235.0f, 200.0f, 200.0f, 30.0f};
            
            DrawRectangleRec(copyButton, DARKGREEN);
            DrawRectangleRec(libraryButton, DARKBLUE);
            
            const char* copyText = copied ? "Copied to clipboard!" : "COPY";
            Vector2 copySize = MeasureTextEx(font16, copyText, 16, 0);
            Vector2 libSize = MeasureTextEx(font16, "LIBRARY", 16, 0);
            
            DrawTextEx(font16, copyText, {115.0f - copySize.x/2.0f, 210.0f}, 16, 0, WHITE);
            DrawTextEx(font16, "LIBRARY", {335.0f - libSize.x/2.0f, 210.0f}, 16, 0, WHITE);
            
            if (CheckCollisionPointRec(GetMousePosition(), copyButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !password.empty()) {
                SetClipboardText(password.c_str());
                copied = true;
                copiedTimer = 120;
            }
            
            if (CheckCollisionPointRec(GetMousePosition(), libraryButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showLibrary = true;
                SetWindowSize(screenWidth, 400);
            }
        } else {
            // Library view
            DrawTextEx(font16, "Password Library (Encrypted)", {centerX - 120.0f, 145}, 16, 0, DARKBLUE);
            
            Rectangle libraryArea = {15.0f, 170.0f, (float)(screenWidth - 30), 180.0f};
            DrawRectangleRec(libraryArea, {245, 245, 245, 255});
            DrawRectangleLinesEx(libraryArea, 1, DARKGRAY);
            
            // Table headers (always visible)
            DrawTextEx(font12, "Service Name", {25, 175}, 12, 0, DARKBLUE);
            DrawTextEx(font12, "Password", {150, 175}, 12, 0, DARKBLUE);
            DrawTextEx(font12, "Actions", {320, 175}, 12, 0, DARKBLUE);
            
            // Header separator line
            DrawLine(20, 190, 415, 190, DARKGRAY);
            
            // Enable scissor test for clipping content only
            BeginScissorMode(15, 195, screenWidth - 35, 155);
            
            // Scroll handling
            int maxVisible = 7; // Reduced to leave space below table
            int totalItems = (int)serviceNames.size();
            
            if (totalItems > maxVisible) {
                float mouseWheel = GetMouseWheelMove();
                scrollOffset -= (int)mouseWheel;
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > totalItems - maxVisible) scrollOffset = totalItems - maxVisible; // Remove extra padding
            }
            
            // Display services
            for (int i = 0; i < std::min(totalItems - scrollOffset, maxVisible); i++) {
                int itemIndex = i + scrollOffset;
                float yPos = 195.0f + i * 20.0f;
                
                if (editingIndex == itemIndex) {
                    // Edit mode for service name
                    Rectangle editBox = {25.0f, yPos - 2.0f, 120.0f, 16.0f};
                    DrawRectangleRec(editBox, WHITE);
                    DrawRectangleLinesEx(editBox, 1, DARKBLUE);
                    DrawTextEx(font12, editBuffer, {30, yPos}, 12, 0, BLACK);
                    
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 32) && (key <= 125) && (strlen(editBuffer) < 30)) {
                            int len = strlen(editBuffer);
                            editBuffer[len] = (char)key;
                            editBuffer[len+1] = '\0';
                        }
                        key = GetCharPressed();
                    }
                    
                    if (IsKeyPressed(KEY_BACKSPACE) && strlen(editBuffer) > 0) {
                        editBuffer[strlen(editBuffer)-1] = '\0';
                    }
                    
                    if (IsKeyPressed(KEY_ENTER) && strlen(editBuffer) > 0) {
                        serviceNames[itemIndex] = std::string(editBuffer);
                        editingIndex = -1;
                        
                        // Save to encrypted file
                        std::ostringstream oss;
                        for (size_t j = 0; j < serviceNames.size(); j++) {
                            oss << serviceNames[j] << "|" << libraryPasswords[j] << std::endl;
                        }
                        std::string encryptedData = encrypt(oss.str());
                        std::ofstream outFile("passwords.dat", std::ios::binary);
                        if (outFile.is_open()) {
                            outFile.write(encryptedData.c_str(), encryptedData.length());
                            outFile.close();
                        }
                    }
                    
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        editingIndex = -1;
                    }
                    
                    // Show password in second column during edit
                    std::string password = libraryPasswords[itemIndex];
                    if (password.length() > 15) password = password.substr(0, 15) + "...";
                    DrawTextEx(font12, password.c_str(), {150, yPos}, 12, 0, BLACK);
                } else {
                    // Display mode - Service name column
                    Rectangle nameArea = {25.0f, yPos - 2.0f, 120.0f, 16.0f};
                    if (CheckCollisionPointRec(GetMousePosition(), nameArea) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        editingIndex = itemIndex;
                        strcpy(editBuffer, serviceNames[itemIndex].c_str());
                    }
                    
                    std::string serviceName = serviceNames[itemIndex];
                    if (serviceName.length() > 12) serviceName = serviceName.substr(0, 12) + "...";
                    DrawTextEx(font12, serviceName.c_str(), {25, yPos}, 12, 0, BLACK);
                    
                    // Password column
                    std::string password = libraryPasswords[itemIndex];
                    if (password.length() > 15) password = password.substr(0, 15) + "...";
                    DrawTextEx(font12, password.c_str(), {150, yPos}, 12, 0, BLACK);
                }
                
                Vector2 copyTextSize = MeasureTextEx(font12, "COPY", 12, 0);
                float copyBtnWidth = copyTextSize.x + 8.0f;
                
                Rectangle copyBtn = {280.0f, yPos - 2.0f, copyBtnWidth, 14.0f};
                Rectangle genBtn = {285.0f + copyBtnWidth, yPos - 2.0f, 30.0f, 14.0f};
                Rectangle delBtn = {320.0f + copyBtnWidth, yPos - 2.0f, 30.0f, 14.0f};
                
                DrawRectangleRec(copyBtn, DARKBLUE);
                DrawRectangleRec(genBtn, DARKGREEN);
                DrawRectangleRec(delBtn, MAROON);
                
                DrawTextEx(font12, "COPY", {284.0f, yPos}, 12, 0, WHITE);
                DrawTextEx(font12, "GEN", {290.0f + copyBtnWidth, yPos}, 12, 0, WHITE);
                DrawTextEx(font12, "DEL", {325.0f + copyBtnWidth, yPos}, 12, 0, WHITE);
                
                if (CheckCollisionPointRec(GetMousePosition(), copyBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    SetClipboardText(libraryPasswords[itemIndex].c_str());
                }
                
                if (CheckCollisionPointRec(GetMousePosition(), genBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    libraryPasswords[itemIndex] = passGen.generate(passwordLength);
                    
                    // Save to encrypted file
                    std::ostringstream oss;
                    for (size_t j = 0; j < serviceNames.size(); j++) {
                        oss << serviceNames[j] << "|" << libraryPasswords[j] << std::endl;
                    }
                    std::string encryptedData = encrypt(oss.str());
                    std::ofstream outFile("passwords.dat", std::ios::binary);
                    if (outFile.is_open()) {
                        outFile.write(encryptedData.c_str(), encryptedData.length());
                        outFile.close();
                    }
                }
                
                if (CheckCollisionPointRec(GetMousePosition(), delBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    serviceNames.erase(serviceNames.begin() + itemIndex);
                    libraryPasswords.erase(libraryPasswords.begin() + itemIndex);
                    if (scrollOffset > 0 && itemIndex == totalItems - 1) scrollOffset--;
                    
                    // Save to encrypted file
                    std::ostringstream oss;
                    for (size_t j = 0; j < serviceNames.size(); j++) {
                        oss << serviceNames[j] << "|" << libraryPasswords[j] << std::endl;
                    }
                    std::string encryptedData = encrypt(oss.str());
                    std::ofstream outFile("passwords.dat", std::ios::binary);
                    if (outFile.is_open()) {
                        outFile.write(encryptedData.c_str(), encryptedData.length());
                        outFile.close();
                    }
                }
                
                // Row separator line
                if (i < std::min(totalItems - scrollOffset, maxVisible) - 1) {
                    DrawLine(20, yPos + 14, 410, yPos + 14, BLUE);
                }
            }
            
            EndScissorMode();
            
            // Scrollbar (inside library area)
            if (totalItems > maxVisible) {
                float scrollBarHeight = (maxVisible - 1) * 20.0f + 14.0f; // Height based on actual content area
                Rectangle scrollBar = {412.0f, 195.0f, 3.0f, scrollBarHeight};
                DrawRectangleRec(scrollBar, LIGHTGRAY);
                
                float thumbHeight = (float)maxVisible / totalItems * scrollBarHeight;
                if (thumbHeight < 10.0f) thumbHeight = 10.0f; // Minimum thumb size
                float thumbY = 195.0f + ((float)scrollOffset / (totalItems - maxVisible)) * (scrollBarHeight - thumbHeight);
                Rectangle scrollThumb = {412.0f, thumbY, 3.0f, thumbHeight};
                DrawRectangleRec(scrollThumb, DARKGRAY);
            }
            
            // Back button (left side)
            Rectangle backButton = {15.0f, 360.0f, 80.0f, 25.0f};
            DrawRectangleRec(backButton, GRAY);
            DrawTextEx(font12, "BACK", {40.0f, 368.0f}, 12, 0, WHITE);
            
            // Add new entry button (right side)
            Rectangle addButton = {355.0f, 360.0f, 80.0f, 25.0f};
            DrawRectangleRec(addButton, DARKBLUE);
            DrawTextEx(font12, "ADD NEW", {365.0f, 368.0f}, 12, 0, WHITE);
            
            if (CheckCollisionPointRec(GetMousePosition(), addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                serviceNames.push_back("new_service");
                libraryPasswords.push_back(passGen.generate(passwordLength));
                
                // Save to encrypted file
                std::ostringstream oss;
                for (size_t j = 0; j < serviceNames.size(); j++) {
                    oss << serviceNames[j] << "|" << libraryPasswords[j] << std::endl;
                }
                std::string encryptedData = encrypt(oss.str());
                std::ofstream outFile("passwords.dat", std::ios::binary);
                if (outFile.is_open()) {
                    outFile.write(encryptedData.c_str(), encryptedData.length());
                    outFile.close();
                }
            }
            
            if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showLibrary = false;
                SetWindowSize(screenWidth, screenHeight);
                editingIndex = -1;
            }
        }
        
        EndDrawing();
    }
    
    UnloadFont(font20);
    UnloadFont(font16);
    UnloadFont(font14);
    UnloadFont(font12);
    UnloadFont(font10);
    CloseWindow();
    return 0;
}