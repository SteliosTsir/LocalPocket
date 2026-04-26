#include "inkview.h"
#include "state.h"
#include "ui.h"
#include "network.h"

int app_state = STATE_WAITING;
char current_filename[256] = "file";

static int main_handler(int event, int a, int b) {
    if (event == EVT_INIT) {
        ui_init();         
        NetConnect(NULL);  
        net_init();        
        
        SetWeakTimer("discovery_ping", send_discovery_ping, 2000);
        SetWeakTimer("network_poll", poll_network, 100);
    }

    if (event == EVT_SHOW) {
        draw_ui();
    }

    if (event == EVT_POINTERUP) {
        int x = a;
        int y = b;

        int exit_w = 200;
        int exit_h = 70;
        int exit_x = (ScreenWidth() - exit_w) / 2;
        int exit_y = ScreenHeight() - 150;

        if (x > exit_x && x < exit_x + exit_w && y > exit_y && y < exit_y + exit_h) {
            ClearTimer(send_discovery_ping); 
            ClearTimer(poll_network);
            net_free();
            NetDisconnect(); 
            CloseApp();
            return 0;
        }

        if (app_state == STATE_SUCCESS || app_state == STATE_ERROR) {
            int btn_w = 400;
            int btn_h = 70;
            int btn_x = (ScreenWidth() - btn_w) / 2;
            int btn_y = 360;

            if (x > btn_x && x < btn_x + btn_w && y > btn_y && y < btn_y + btn_h) {
                app_state = STATE_WAITING;
                draw_ui(); 
            }
        }
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    InkViewMain(main_handler);
    return 0;
}