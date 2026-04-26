#include "inkview.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "ui.h"
#include "network.h"

int server_fd = -1;
int udp_fd = -1;

void net_init() {
    struct sockaddr_in addr;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd >= 0) {
        int opt = 1; setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        fcntl(server_fd, F_SETFL, O_NONBLOCK);
        addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(53317);
        bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
        listen(server_fd, 5);
    }
    
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd >= 0) {
        int br = 1; setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &br, sizeof(br));
    }
}

void net_free() {
    if (server_fd >= 0) close(server_fd); 
    if (udp_fd >= 0) close(udp_fd);
}

void send_discovery_ping() {
    if (udp_fd >= 0) {
        struct sockaddr_in b_addr;
        b_addr.sin_family = AF_INET; b_addr.sin_port = htons(53317); b_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
        const char *msg = "{\"alias\":\"PB-Sense\",\"deviceModel\":\"PB630\",\"deviceType\":\"tablet\",\"fingerprint\":\"pb630-unique\",\"version\":\"2.0\",\"protocol\":\"http\",\"download\":true}";
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
            // PREPARE UPLOAD
            if (strstr(buffer, "prepare-upload")) {
                char file_id[128] = "*";
                char *id_search = strstr(buffer, "\"files\":{\"");
                if (id_search) {
                    id_search += 10; 
                    char *id_end = strchr(id_search, '\"');
                    if (id_end) {
                        int len = id_end - id_search;
                        memcpy(file_id, id_search, len); file_id[len] = '\0';
                    }
                }

                char *name_search = strstr(buffer, "\"fileName\":\"");
                if (name_search) {
                    name_search += 12;
                    char *name_end = strchr(name_search, '\"');
                    if (name_end) {
                        int len = name_end - name_search;
                        memcpy(current_filename, name_search, len); current_filename[len] = '\0';
                    }
                }

                char response_body[512];
                snprintf(response_body, sizeof(response_body), "{\"sessionId\":\"1234\",\"files\":{\"%s\":\"accepted\"}}", file_id);

                char full_response[1024];
                snprintf(full_response, sizeof(full_response),
                    "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s", 
                    (int)strlen(response_body), response_body);

                send(new_socket, full_response, strlen(full_response), 0);
                
                app_state = STATE_RECEIVING;
                draw_ui();
            } 
            
            // BIG FILE UPLOAD
            else if (strstr(buffer, "upload") || strstr(buffer, "/v2/push")) {
                
                if (strstr(buffer, "100-continue")) {
                    const char *cont = "HTTP/1.1 100 Continue\r\n\r\n";
                    send(new_socket, cont, strlen(cont), 0);
                }

                int content_length = 0;
                char *len_str = strstr(buffer, "Content-Length: ");
                if (!len_str) len_str = strstr(buffer, "content-length: ");
                if (!len_str) len_str = strstr(buffer, "Content-length: ");
                
                if (len_str) content_length = atoi(len_str + 16);

                char *body_start = strstr(buffer, "\r\n\r\n");
                if (body_start) {
                    body_start += 4; 
                    int header_len = body_start - buffer;
                    int initial_body_len = n - header_len;

                    char filepath[512];
                    snprintf(filepath, sizeof(filepath), "/mnt/ext1/Downloads/%s", current_filename);
                    FILE *file = fopen(filepath, "wb");

                    if (file) {
                        if (initial_body_len > 0) fwrite(body_start, 1, initial_body_len, file);
                        
                        int remaining = content_length - initial_body_len;
                        int total_downloaded = initial_body_len;
                        int last_percent = 0;

                        fcntl(new_socket, F_SETFL, 0); 
                        
                        char chunk[2048];
                        while (remaining > 0) {
                            int bytes_to_read = remaining > 2048 ? 2048 : remaining;
                            int chunk_size = read(new_socket, chunk, bytes_to_read);
                            
                            if (chunk_size <= 0) break; 
                            
                            fwrite(chunk, 1, chunk_size, file);
                            remaining -= chunk_size;
                            total_downloaded += chunk_size;

                            int percent = (total_downloaded * 100) / content_length;
                            if (percent >= last_percent + 2) { 
                                last_percent = percent;
                                
                                int bar_w = 500;
                                int bar_h = 60;
                                int bar_x = (ScreenWidth() - bar_w) / 2;
                                int bar_y = 350;
                                
                                int fill_w = bar_w * ((float)percent / 100.0);
                                
                                FillArea(bar_x, bar_y, fill_w, bar_h, BLACK);
                                PartialUpdate(bar_x, bar_y, fill_w, bar_h); 
                            }
                        }

                        fflush(file); fsync(fileno(file)); fclose(file); sync(); 
                        
                        app_state = STATE_SUCCESS;
                        draw_ui();
                    }
                }

                const char *ok = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n{}";
                send(new_socket, ok, strlen(ok), 0);
            }
            
            // DISCOVERY PINGS
            else {
                const char *info = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"alias\":\"Sense-Eink\",\"version\":\"2.0\"}";
                send(new_socket, info, strlen(info), 0);
            }
        }
        close(new_socket);
    }
    SetWeakTimer("network_poll", poll_network, 100);
}
