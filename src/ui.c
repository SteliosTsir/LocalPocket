#include "inkview.h"
#include <stdio.h>
#include "state.h"
#include "ui.h"

ifont *main_font = NULL;

void ui_init() {
    main_font = OpenFont("Liberation Sans", 35, 1);
    if (!main_font) main_font = OpenFont("PT Sans", 35, 1);
    if (!main_font) main_font = OpenFont("Arial", 35, 1);
    if (!main_font) main_font = OpenFont("default", 35, 1); 
}

void draw_ui() {
    ClearScreen();
    
    if (main_font) {
        SetFont(main_font, BLACK);
    }
    
    // Header
    DrawTextRect(0, 40, ScreenWidth(), 60, "LocalSend for PocketBook", ALIGN_CENTER);
    DrawLine(20, 110, ScreenWidth() - 20, 110, BLACK);

    if (app_state == STATE_WAITING) {
        DrawTextRect(0, 250, ScreenWidth(), 60, "Waiting for files...", ALIGN_CENTER);
        DrawTextRect(0, 320, ScreenWidth(), 60, "Device Name: PB-Sense", ALIGN_CENTER);
    } 
    else if (app_state == STATE_RECEIVING) {
        char msg[300];
        snprintf(msg, sizeof(msg), "Receiving: %s", current_filename);
        DrawTextRect(0, 250, ScreenWidth(), 60, msg, ALIGN_CENTER);
        
        // Progress bar outline 
        int bar_w = 500;
        int bar_h = 60;
        int bar_x = (ScreenWidth() - bar_w) / 2;
        int bar_y = 350;
        DrawRect(bar_x, bar_y, bar_w, bar_h, BLACK); 
    } 
    else if (app_state == STATE_SUCCESS) {
        DrawTextRect(0, 200, ScreenWidth(), 60, "Transfer Complete!", ALIGN_CENTER);
        char msg[300];
        snprintf(msg, sizeof(msg), "Saved: %s", current_filename);
        DrawTextRect(0, 270, ScreenWidth(), 60, msg, ALIGN_CENTER);

        // Receive Another Button
        int btn_w = 400;
        int btn_h = 70;
        int btn_x = (ScreenWidth() - btn_w) / 2;
        int btn_y = 360;
        DrawRect(btn_x, btn_y, btn_w, btn_h, BLACK);
        DrawTextRect(btn_x, btn_y, btn_w, btn_h, "Receive Another", ALIGN_CENTER);
    }

    // Exit Button
    int exit_w = 200;
    int exit_h = 70;
    int exit_x = (ScreenWidth() - exit_w) / 2;
    int exit_y = ScreenHeight() - 150;
    DrawRect(exit_x, exit_y, exit_w, exit_h, BLACK);
    DrawTextRect(exit_x, exit_y, exit_w, exit_h, "EXIT", ALIGN_CENTER);

    FullUpdate();
}