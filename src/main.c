#include "inkview.h"

int main(void) {
    // Initialize the PocketBook UI
    InkViewMain(NULL);
    OpenScreen();
    ClearScreen();
    SetFont(GetDefaultFont(), 30);
    
    // Draw some text
    DrawTextRect(0, 200, ScreenWidth(), 50, "LocalSend PocketBook", ALIGN_CENTER);
    DrawTextRect(0, 300, ScreenWidth(), 50, "Press 'Back' to exit.", ALIGN_CENTER);
    
    // Push the changes to the e-ink display
    FullUpdate();

    // Wait until the user presses a button
    int running = 1;
    while (running) {
        int event = GetEvent();
        if (event == EVT_KEYPRESS && GetEventData() == KEY_PREV) {
            running = 0;
        }
    }

    CloseApp();
    return 0;
}