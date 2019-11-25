#ifndef _GST_PLAY_H_
#define _GST_PLAY_H_

#include <stdio.h>

typedef struct gst_play_handle_s gst_play_handle_t;
typedef void(*gst_play_start_cb_t)(gst_play_handle_t* ctx);

typedef enum gst_play_role_s {
    GST_PLAY_FILESRC,
    GST_PLAY_UDPSINK,
    GST_PLAY_UDPSRC,
} gst_play_role_t;

gst_play_handle_t* gst_play_handle_new(gst_play_role_t role);
bool gst_play_set_paraments(gst_play_handle_t* ctx, const char* pathname);
bool gst_play_start(gst_play_handle_t* ctx, gst_play_start_cb_t cb);//blocking function
bool gst_play_pause(gst_play_handle_t* ctx);
bool gst_play_continue(gst_play_handle_t* ctx);
bool gst_play_hang(gst_play_handle_t* ctx);
bool gst_play_wake(gst_play_handle_t* ctx);
void gst_play_release(gst_play_handle_t* ctx);

#endif