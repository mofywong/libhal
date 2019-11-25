#include <string.h>
#include "DeviceCommand.hh"
#include "ym_lamp_device.h"
#include "YmLamp.hh"

struct ym_lamp_s {
    ym_lamp_device_ctx_t* ctx;
};

YmLamp::YmLamp()
{
}

YmLamp::~YmLamp()
{
}

int YmLamp::DevOpen(const std::string& device, int flags)
{
    lamp_ctx = (lamp_ym_t*)malloc(sizeof(ym_lamp_s));
    if (lamp_ctx) {
        ym_lamp_device_ctx_t* ctx = ym_lamp_device_new (device.c_str());
        if (ctx) {
            ym_lamp_s* temp = (ym_lamp_s*)lamp_ctx;
            temp->ctx = ctx;
            return Success;
        }
        free(lamp_ctx);
    }
    return UnkownErr;
}

int YmLamp::DevClose(void)
{
    ym_lamp_s* temp = (ym_lamp_s*)lamp_ctx;
    ym_lamp_device_free(temp->ctx);
    return Success;
}

int YmLamp::LampYmSetOnoff(int channel,int onoff)
{
    ym_lamp_s* temp = (ym_lamp_s*)lamp_ctx;
    bool_t ret = false;
    if (onoff == 0) {
        ret = ym_lamp_device_set_onoff(temp->ctx, channel, false);
    }
    else {
        ret = ym_lamp_device_set_onoff(temp->ctx, channel, true);
    }   
    if(ret) {
        return Success;
    }
    return UnkownErr;
}

int YmLamp::LampYmSetBrightness(int channel,int brightness)
{
    ym_lamp_s* temp = (ym_lamp_s*)lamp_ctx;
    bool_t ret = false;
    ret = ym_lamp_device_set_brightness(temp->ctx, channel, brightness);
    if(ret) {
        return Success;
    }
    return UnkownErr;
}

int YmLamp::LampYmGetAttr(void* output)
{
    ym_lamp_s* temp = (ym_lamp_s*)lamp_ctx;
    if (ym_lamp_device_get_attr(temp->ctx, output)){
        return Success;
    }
    return UnkownErr;
}