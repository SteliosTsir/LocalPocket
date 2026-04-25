#define MG_ENABLE_THREADS 0
#include "mongoose.h"

static struct mg_mgr mgr;

// Simple internal handler
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        mg_http_reply(c, 200, "", "Sense is Online!");
    }
}

void net_init() {
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:53317", fn, NULL);
}

void net_poll() {
    mg_mgr_poll(&mgr, 0);
}

void net_free() {
    mg_mgr_free(&mgr);
}