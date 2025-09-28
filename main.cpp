int main() {
    const int screenWidth = 450;
    const int screenHeight = 280;

    InitWindow(screenWidth, screenHeight, "Password Generator");
    SetTargetFPS(60);

    // Load FreePixel fonts from embedded data
    Font font20 = LoadFontFromMemory(".ttf", FONT_DATA, FONT_SIZE, 20, 0, 95);
    Font font16 = LoadFontFromMemory(".ttf", FONT_DATA, FONT_SIZE, 16, 0, 95);
    Font font14 = LoadFontFromMemory(".ttf", FONT_DATA, FONT_SIZE, 14, 0, 95);
    Font font12 = LoadFontFromMemory(".ttf", FONT_DATA, FONT_SIZE, 12, 0, 95);
    Font font10 = LoadFontFromMemory(".ttf", FONT_DATA, FONT_SIZE, 10, 0, 95);

    SetTextureFilter(font20.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(font16.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(font14.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(font12.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(font10.texture, TEXTURE_FILTER_POINT);

    // Set window icon from embedded data
    Image iconImage = LoadImageFromMemory(".png", ICON_DATA, ICON_SIZE);
    if (iconImage.data != NULL) {
        SetWindowIcon(iconImage);
        UnloadImage(iconImage);
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
        ClearBackground(RAYWHITE); // Changed from DARKGRAY

        // Top bar with title (centered)
        DrawRectangle(0, 0, screenWidth, 35, BLUE); // Changed from DARKBLUE
        Vector2 titleSize = MeasureTextEx(font16, "Password Generator", 16, 0);
        DrawTextEx(font16, "Password Generator", {centerX - titleSize.x/2, 10}, 16, 0, WHITE); // Text on colored background can remain white

        // Password length with slider (centered)
        const char* lengthText = TextFormat("Length: %d", passwordLength);
        Vector2 lengthSize = MeasureTextEx(font16, lengthText, 16, 0);
        DrawTextEx(font16, lengthText, {centerX - lengthSize.x/2, 45}, 16, 0, BLACK); // Changed from WHITE

        DrawRectangleRec(sliderBar, GRAY); // Changed from LIGHTGRAY
        DrawRectangleRec(sliderKnob, BLUE); // Changed from DARKBLUE
        DrawTextEx(font16, "4", {centerX - 140.0f, 65.0f}, 16, 0, DARKGRAY); // Changed from GRAY
        DrawTextEx(font16, "50", {centerX + 130.0f, 65.0f}, 16, 0, DARKGRAY); // Changed from GRAY

        // Generate button area (auto-width, centered)
        Vector2 genSize = MeasureTextEx(font16, "Generate (SPACE)", 16, 0);
        float buttonWidth = genSize.x + 20.0f; // Add padding
        Rectangle genButton = {centerX - buttonWidth/2.0f, 95.0f, buttonWidth, 30.0f};
        DrawRectangleRec(genButton, BLUE); // Changed from DARKBLUE
        float genButtonCenterY = genButton.y + genButton.height/2.0f - genSize.y/2.0f;
        DrawTextEx(font16, "Generate (SPACE)", {centerX - genSize.x/2, genButtonCenterY}, 16, 0, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), genButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            password = passGen.generate(passwordLength);
            copied = false;
        }

        if (!showLibrary) {
            // Main generator view
            Rectangle passBox = {15.0f, 140.0f, (float)(screenWidth - 30), 45.0f};
            DrawRectangleRec(passBox, LIGHTGRAY); // Changed from WHITE
            DrawRectangleLinesEx(passBox, 1, GRAY); // Changed from DARKGRAY

            if (!password.empty()) {
                const char* passText = password.c_str();
                Vector2 textSize = MeasureTextEx(font16, passText, 16, 0);
                if (textSize.x > screenWidth - 50) {
                    DrawTextEx(font16, passText, {centerX - textSize.x/2.0f, 150.0f}, 12, 0, BLACK); // Changed from DARKGREEN
                } else {
                    DrawTextEx(font16, passText, {centerX - textSize.x/2.0f, 155.0f}, 16, 0, BLACK); // Changed from DARKGREEN
                }
            } else {
                DrawTextEx(font16, "Generated password appears here", {centerX - 140.0f, 155.0f}, 16, 0, DARKGRAY); // Changed from GRAY
            }

            // Copy and Library buttons
            Rectangle copyButton = {15.0f, 200.0f, 200.0f, 30.0f};
            Rectangle libraryButton = {235.0f, 200.0f, 200.0f, 30.0f};

            DrawRectangleRec(copyButton, GREEN); // Changed from DARKGREEN
            DrawRectangleRec(libraryButton, BLUE); // Changed from DARKBLUE

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
            DrawTextEx(font16, "Password Library (Encrypted)", {centerX - 120.0f, 145}, 16, 0, BLUE); // Changed from DARKBLUE

            Rectangle libraryArea = {15.0f, 170.0f, (float)(screenWidth - 30), 180.0f};
            DrawRectangleRec(libraryArea, {220, 220, 220, 255}); // A light gray
            DrawRectangleLinesEx(libraryArea, 1, GRAY); // Changed from DARKGRAY

            // Table headers (always visible)
            DrawTextEx(font12, "Service Name", {25, 175}, 12, 0, BLUE); // Changed from DARKBLUE
            DrawTextEx(font12, "Password", {150, 175}, 12, 0, BLUE); // Changed from DARKBLUE
            DrawTextEx(font12, "Actions", {320, 175}, 12, 0, BLUE); // Changed from DARKBLUE

            // Header separator line
            DrawLine(20, 190, 415, 190, GRAY); // Changed from DARKGRAY

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
                    DrawRectangleLinesEx(editBox, 1, BLUE); // Changed from DARKBLUE
                    DrawTextEx(font12, editBuffer, {30, yPos}, 12, 0, BLACK); // Changed from WHITE

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
                    DrawTextEx(font12, password.c_str(), {150, yPos}, 12, 0, BLACK); // Changed from WHITE
                } else {
                    // Display mode - Service name column
                    Rectangle nameArea = {25.0f, yPos - 2.0f, 120.0f, 16.0f};
                    if (CheckCollisionPointRec(GetMousePosition(), nameArea) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        editingIndex = itemIndex;
                        strcpy(editBuffer, serviceNames[itemIndex].c_str());
                    }

                    std::string serviceName = serviceNames[itemIndex];
                    if (serviceName.length() > 12) serviceName = serviceName.substr(0, 12) + "...";
                    DrawTextEx(font12, serviceName.c_str(), {25, yPos}, 12, 0, BLACK); // Changed from WHITE

                    // Password column
                    std::string password = libraryPasswords[itemIndex];
                    if (password.length() > 15) password = password.substr(0, 15) + "...";
                    DrawTextEx(font12, password.c_str(), {150, yPos}, 12, 0, BLACK); // Changed from WHITE
                }

                Vector2 copyTextSize = MeasureTextEx(font12, "COPY", 12, 0);
                float copyBtnWidth = copyTextSize.x + 8.0f;

                Rectangle copyBtn = {280.0f, yPos - 2.0f, copyBtnWidth, 14.0f};
                Rectangle genBtn = {285.0f + copyBtnWidth, yPos - 2.0f, 30.0f, 14.0f};
                Rectangle delBtn = {320.0f + copyBtnWidth, yPos - 2.0f, 30.0f, 14.0f};

                DrawRectangleRec(copyBtn, BLUE); // Changed from DARKBLUE
                DrawRectangleRec(genBtn, GREEN); // Changed from DARKGREEN
                DrawRectangleRec(delBtn, RED); // Changed from MAROON

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
                    DrawLine(20, yPos + 14, 410, yPos + 14, SKYBLUE); // Changed from BLUE
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
            DrawRectangleRec(backButton, LIGHTGRAY); // Changed from GRAY
            DrawTextEx(font12, "BACK", {40.0f, 368.0f}, 12, 0, BLACK); // Changed from WHITE

            // Add new entry button (right side)
            Rectangle addButton = {355.0f, 360.0f, 80.0f, 25.0f};
            DrawRectangleRec(addButton, BLUE); // Changed from DARKBLUE
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