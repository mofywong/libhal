#include <string>

#include "BrocastDevice.hh"

BrocastDevice::BrocastDevice() {
    gst_play_handle = NULL;
}

BrocastDevice::~BrocastDevice() {
    this->DevClose();
    printf("GpsDevice\n");
}

//
bool BrocastDevice::DevOpen(const std::string& device) {
    return true;
}

//无法检测是否在线，总是返回true，兼容
bool BrocastDevice::Heartbeat() {
    return true;
}

bool BrocastDevice::DevClose() {
    this->Stop(NULL);
    return true;    
}

bool BrocastDevice::Play(std::string file, void* output) {
    bool ok = false;

    if (file.empty())
        return ok;

    if ((gst_play_handle = gst_play_handle_new(GST_PLAY_FILESRC)) && gst_play_set_paraments(gst_play_handle, file.c_str())) {
        gst_play_start(gst_play_handle, NULL);
        ok = true;
        printf("play %s finnash\n", file.c_str());
        gst_play_release(gst_play_handle);
        gst_play_handle = NULL;
    }
    
    return ok;
}

bool BrocastDevice::Pause(void* output) {
    if (gst_play_handle)
        return gst_play_pause(gst_play_handle);
    else
        return false;
}

bool BrocastDevice::Continue(void* output) {
    if (gst_play_handle)
        return gst_play_continue(gst_play_handle);
    else
        return false;
}

bool BrocastDevice::Stop(void* output) {
    if (gst_play_handle) {
        gst_play_pause(gst_play_handle);
        gst_play_release(gst_play_handle);
        gst_play_handle = NULL;
    }  

    return true; 
}