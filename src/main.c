#include "inkview.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int server_fd = -1;
int udp_fd = -1;

void draw_ready_text() {
    // Draw "READY" as a series of simple blocks/lines
    FillArea(20, 20, 100, 30, BLACK); // A "Ready" banner
    DrawLine(20, 60, 120, 60, BLACK);
}

void send_discovery_ping() {
    if (udp_fd >= 0) {
        struct sockaddr_in b_addr;
        b_addr.sin_family = AF_INET;
        b_addr.sin_port = htons(53317);
        b_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
        const char *msg = "{\"alias\":\"Sense-Eink\",\"deviceModel\":\"PB630\",\"deviceType\":\"tablet\",\"fingerprint\":\"pb630-unique\",\"version\":\"2.0\",\"protocol\":\"http\",\"download\":true}";
        sendto(udp_fd, msg, strlen(msg), 0, (struct sockaddr *)&b_addr, sizeof(b_addr));
    }
    SetWeakTimer("discovery_ping", send_discovery_ping, 2000);
}

void poll_network() {
    if (server_fd < 0) return;

    int new_socket = accept(server_fd, NULL, NULL);
    if (new_socket >= 0) {
        char buffer[4096] = {0};
        int n = read(new_socket, buffer, 4095);

        if (n > 0) {
            if (strstr(buffer, "prepare-upload")) {
                // --- FILE ID EXTRACTION ---
                char file_id[128] = "*";
                char *search = strstr(buffer, "\"files\":{\"");
                if (search) {
                    search += 10; // Move past "files":{"
                    char *end = strchr(search, '\"');
                    if (end && (end - search) < 127) {
                        int len = end - search;
                        memcpy(file_id, search, len);
                        file_id[len] = '\0';
                    }
                }

                // Construct a proper response with the actual File ID
                char response_body[512];
                snprintf(response_body, sizeof(response_body), 
                    "{\"sessionId\":\"1234\",\"files\":{\"%s\":\"accepted\"}}", file_id);

                char full_response[1024];
                snprintf(full_response, sizeof(full_response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: close\r\n\r\n%s", (int)strlen(response_body), response_body);

                send(new_socket, full_response, strlen(full_response), 0);
                
                // Bar fills halfway
                FillArea(105, 305, (ScreenWidth() - 210) / 2, 40, BLACK);
            } 
            else if (strstr(buffer, "upload") || strstr(buffer, "/v2/push")) {
                const char *ok = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n{}";
                send(new_socket, ok, strlen(ok), 0);
                
                // Bar fills completely
                FillArea(105, 305, ScreenWidth() - 210, 40, BLACK);
            }
            else {
                const char *info = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"alias\":\"Sense-Eink\",\"version\":\"2.0\"}";
                send(new_socket, info, strlen(info), 0);
            }
        }
        close(new_socket);
        FullUpdate();
    }
    SetWeakTimer("network_poll", poll_network, 100);
}

static int main_handler(int event, int a, int b) {
    if (event == EVT_INIT) {
        NetConnect(NULL);
        
        struct sockaddr_in addr;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd >= 0) {
            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            fcntl(server_fd, F_SETFL, O_NONBLOCK);
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(53317);
            bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
            listen(server_fd, 5);
        }
        
        udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_fd >= 0) {
            int br = 1;
            setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &br, sizeof(br));
        }
        
        SetWeakTimer("discovery_ping", send_discovery_ping, 2000);
        SetWeakTimer("network_poll", poll_network, 100);
    }

    if (event == EVT_SHOW) {
        ClearScreen();
        draw_ready_text();
        DrawRect(100, 300, ScreenWidth() - 200, 50, BLACK);
        FullUpdate();
    }

    if (event == EVT_KEYPRESS || event == EVT_POINTERUP) {
        ClearTimer(send_discovery_ping);
        ClearTimer(poll_network);
        if (server_fd >= 0) close(server_fd);
        if (udp_fd >= 0) close(udp_fd);
        NetDisconnect();
        CloseApp();
    }
    return 0;
}

int main(int argc, char *argv[]) {
    InkViewMain(main_handler);
    return 0;
}