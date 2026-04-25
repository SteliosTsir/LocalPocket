#include "inkview.h"

static int main_handler(int event, int a, int b) {
    if (event == EVT_INIT) {
        // Just clear the screen to white
        ClearScreen();
        
        // Draw a simple black rectangle instead of text first
        // (This proves the UI engine is working without needing font files)
        FillArea(200, 200, 400, 400, BLACK);
        
        // Push to e-ink
        FullUpdate();
    }

    // Exit on any touch or button
    if (event == EVT_KEYPRESS || event == EVT_POINTERUP) {
        CloseApp();
    }

    return 0;
}

int main (int argc, char *argv[]) {
    InkViewMain(main_handler);
    return 0;
}