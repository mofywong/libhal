#include <stdio.h>

#include <unistd.h>
#include <pthread.h>
#include <gst/gst.h>  

#include "gst_play.hh"

#define DEFAULT_UDP_ADDR      "239.0.2.255"
#define DEFAULT_UDP_RTP_PORT  30000
#define DEFAULT_UDP_HAND_PORT 30001
#define DEFAULT_AUDIO_CAPS    "application/x-rtp, media=(string)audio, clock-rate=(int)44000, channels=(int)2"

struct gst_play_handle_s {
    GMainLoop *loop;  
    GstElement *pipeline;
    GstElement *source;
    GstElement *decoder;
    GstElement *rtp;
    GstElement *mpeg;
    GstElement *audioconvert;
    GstElement *rtpbuffer;
    GstElement *pulsesink;
    GstElement *sink;//定义组件  
    GstBus *bus;
    gst_play_role_t role;
} ;

gst_play_handle_t* gst_play_handle_new(gst_play_role_t role) {
    gst_play_handle_t* ctx = NULL;

    gst_init(NULL, NULL);
    ctx = (gst_play_handle_t*)malloc(sizeof(gst_play_handle_t));
    if (ctx) {
        memset(ctx, 0 , sizeof(gst_play_handle_t));
        ctx->role = role;
        ctx->loop = g_main_loop_new(NULL, FALSE);
        ctx->pipeline = gst_pipeline_new("audio");    
        if (!ctx->loop || !ctx->pipeline)
            goto error;
        if (role == GST_PLAY_UDPSINK) { 
            ctx->source = gst_element_factory_make("filesrc", "file-source");  
            ctx->mpeg =  gst_element_factory_make("mpegaudioparse", "mpeg");
            ctx->decoder = gst_element_factory_make("mpg123audiodec", "mad-decoder");
            ctx->rtp = gst_element_factory_make("rtpL16pay", "rtpL16pay");
            ctx->audioconvert = gst_element_factory_make("audioconvert", "audioconvert");  
            ctx->sink = gst_element_factory_make("udpsink", "sink");
            if (!ctx->source || !ctx->mpeg || !ctx->decoder || !ctx->sink || !ctx->audioconvert || !ctx->rtp)
                goto error;                      
        } else if (role == GST_PLAY_UDPSRC){
            ctx->source = gst_element_factory_make("udpsrc", "file-udpsrc");  
            ctx->rtpbuffer = gst_element_factory_make("rtpjitterbuffer", "rtpbuffer");
            ctx->audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
            ctx->rtp = gst_element_factory_make("rtpL16depay", "rtpL16depay");  
            ctx->pulsesink = gst_element_factory_make("pulsesink", "pulse-sink");  
            if (!ctx->source || !ctx->rtpbuffer || !ctx->pulsesink || !ctx->audioconvert || !ctx->rtp)
                goto error;       
        } else if(role == GST_PLAY_FILESRC) {
            ctx->source = gst_element_factory_make("filesrc", "file-source");  
            ctx->mpeg =  gst_element_factory_make("mpegaudioparse", "mpeg");
            ctx->decoder = gst_element_factory_make("mpg123audiodec", "mad-decoder");  
            ctx->sink = gst_element_factory_make("pulsesink", "pulsesink");
            if (!ctx->source || !ctx->mpeg || !ctx->decoder || !ctx->sink)
                goto error;             
        } else {
            printf("error: gst_play_handle_new failed0\n");
            goto error;
        } 
    }

    return ctx;

error:
    printf("error: gst_play_handle_new failed\n");
    if (ctx->loop) {
        g_main_loop_unref(ctx->loop);
        ctx->loop = NULL;
    }
    if (ctx->pipeline ) {
        gst_object_unref(ctx->pipeline);
        ctx->pipeline = NULL;
    }
    if (ctx->rtp ) {
        gst_object_unref(ctx->rtp);
        ctx->rtp = NULL;        
    }
    if (ctx->source ) {
        gst_object_unref(ctx->source);
        ctx->source = NULL;  
    }
    if (ctx->mpeg ) {
        gst_object_unref(ctx->mpeg);
        ctx->mpeg = NULL;          
    }
    if (ctx->decoder ) {
        gst_object_unref(ctx->decoder);
        ctx->decoder = NULL;        
    }
    if (ctx->sink ) {
        gst_object_unref(ctx->sink);
        ctx->sink = NULL;         
    }
    if (ctx->audioconvert ) {
        gst_object_unref(ctx->audioconvert);
        ctx->audioconvert = NULL;         
    }
    if (ctx->rtpbuffer ) {
        gst_object_unref(ctx->rtpbuffer);
        ctx->rtpbuffer = NULL;          
    }
    if (ctx->pulsesink ) {
        gst_object_unref(ctx->pulsesink);
        ctx->pulsesink = NULL;         
    }
    free((void*)ctx);
    return NULL;
}

bool gst_play_set_paraments(gst_play_handle_t* ctx, const char* pathname) {
    bool is_ok = false;

    if (ctx && pathname && ctx->role == GST_PLAY_UDPSINK) {
        g_object_set(G_OBJECT(ctx->source), "location", pathname, NULL);
        g_object_set(ctx->sink, "port", DEFAULT_UDP_RTP_PORT, "host", DEFAULT_UDP_ADDR, NULL);
        gst_bin_add_many(GST_BIN(ctx->pipeline), ctx->source, ctx->mpeg, ctx->decoder, ctx->audioconvert, ctx->rtp, ctx->sink, NULL);  
        is_ok = (gst_element_link_many(ctx->source, ctx->mpeg, ctx->decoder, ctx->audioconvert, ctx->rtp, ctx->sink, NULL) == TRUE) ? true : false;
    } else if(ctx && ctx->role == GST_PLAY_UDPSRC) {
        GstCaps *caps;
        caps = gst_caps_from_string(DEFAULT_AUDIO_CAPS);
        g_object_set(ctx->source, "port", DEFAULT_UDP_RTP_PORT, "multicast-group", DEFAULT_UDP_ADDR, "caps", caps, NULL);
        gst_caps_unref(caps);
        g_object_set(ctx->rtpbuffer, "latency", 400, NULL);
        gst_bin_add_many(GST_BIN(ctx->pipeline), ctx->source, ctx->rtpbuffer, ctx->rtp, ctx->audioconvert, ctx->pulsesink, NULL);  
        is_ok = (gst_element_link_many(ctx->source, ctx->rtpbuffer, ctx->rtp, ctx->audioconvert, ctx->pulsesink, NULL) == TRUE) ? true : false;
    } else if(ctx && pathname && ctx->role == GST_PLAY_FILESRC) {
        g_object_set(G_OBJECT(ctx->source), "location", pathname, NULL);
        gst_bin_add_many(GST_BIN(ctx->pipeline), ctx->source, ctx->mpeg, ctx->decoder, ctx->sink,NULL);  
        is_ok = (gst_element_link_many(ctx->source, ctx->mpeg, ctx->decoder, ctx->sink, NULL) == TRUE) ? true : false;       
    } else {
        printf("error: gst_play_set_paraments failed\n");
    }

    return is_ok;
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {  
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE(msg)) {  
        case GST_MESSAGE_EOS:  
            g_print("End of stream\n");  
            g_main_loop_quit(loop);  
            break;  
        case GST_MESSAGE_ERROR: {  
               gchar *debug;  
               GError *error;  
               gst_message_parse_error(msg,&error,&debug);  
               g_free(debug);  
               g_printerr("ERROR:%s\n",error->message);  
               g_error_free(error);  
               g_main_loop_quit(loop);  
  
                break;  
        }  
        default:  
             break;  
    }  
    return TRUE;  
} 

bool gst_play_start(gst_play_handle_t* ctx, gst_play_start_cb_t cb) {
    GstBus *bus;
    bool is_ok = false;

    if (ctx) {
        bus = gst_pipeline_get_bus(GST_PIPELINE(ctx->pipeline));  
        gst_bus_add_watch(bus, bus_call, ctx->loop);
        gst_object_unref(bus);
        gst_element_set_state(ctx->pipeline, GST_STATE_PLAYING);
        //cb ? cb(ctx) : false;
        g_main_loop_run(ctx->loop);            
        is_ok = true;
    }
    return is_ok;
}

bool gst_play_pause(gst_play_handle_t* ctx) {
    if(ctx) {
        gst_element_set_state(ctx->pipeline, GST_STATE_PAUSED);
        return true;
    }

    return false;
}

bool gst_play_continue(gst_play_handle_t* ctx) {
    if (ctx) {
        gst_element_set_state(ctx->pipeline, GST_STATE_PLAYING);
        return true;
    }
   
    return false;    
}

bool gst_play_hang(gst_play_handle_t* ctx) {
    if (ctx) {
        g_object_set(ctx->sink, "port", DEFAULT_UDP_HAND_PORT, NULL);
        return true;
    }
    
    return false;
}

bool gst_play_wake(gst_play_handle_t* ctx) {
    if (ctx) {
        g_object_set(ctx->sink, "port", DEFAULT_UDP_RTP_PORT, NULL) ;
        return true;
    }
    
    return false;
}

void gst_play_release(gst_play_handle_t* ctx) {
    if (ctx) {
        gst_element_set_state(ctx->pipeline, GST_STATE_NULL);  
        gst_object_unref(GST_OBJECT(ctx->pipeline));    
        g_main_loop_unref(ctx->loop); 
        memset(ctx, 0, sizeof(gst_play_handle_t));
        free((void*)ctx);        
    }
}
