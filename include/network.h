#ifndef NETWORK_H
#define NETWORK_H

void net_init();
void net_free();
void send_discovery_ping();
void poll_network();

#endif