#ifndef STATE_H
#define STATE_H

#define STATE_WAITING 0
#define STATE_RECEIVING 1
#define STATE_SUCCESS 2
#define STATE_ERROR 3

extern int app_state;
extern char current_filename[256];

#endif