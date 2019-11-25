#ifndef _BROCAST_DEVICE_HH_
#define _BROCAST_DEVICE_HH_

#include <string>
#include "gst_play.hh"

class BrocastDevice {
public:
    BrocastDevice();
    ~BrocastDevice();

public:
    bool DevOpen(const std::string& device);
    bool DevClose();
    bool Heartbeat();

    bool Play(std::string file, void* output);
    bool Pause(void* output);
    bool Continue(void* output);
    bool Stop(void* output);

private:
    gst_play_handle_t* gst_play_handle;
};

#endif