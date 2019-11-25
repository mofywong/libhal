#include "MethodParser.hh"
#include "InfoScreenColt.hh"
#include "YmLamp.hh"
#include "gps/GpsDevice.hh"
#include "brocast/BrocastDevice.hh"

void* DeviceOpen(const char* dev_str, device_type_t type)
{
    void* device = NULL;
    InfoScreenColt* colt_device;
    YmLamp* ym_device;
    GpsDevice* gpsdevice = NULL;
    BrocastDevice* bdevice = NULL;

    switch (type)
    {
        case DEV_LAMP:
            ym_device = new YmLamp();
            if (ym_device) {
                if (Success == ym_device->DevOpen(dev_str, 1)) {
                    device = (void *)ym_device;
                } else {
                    delete(ym_device);
                }
            }
            break;

        case DEV_INFOSCREEN:
            colt_device = new InfoScreenColt();
            if (colt_device) {
                if (Success == colt_device->DevOpen(dev_str, 1)) {
                    device = (void *)colt_device;
                } else {
                    delete(colt_device);
                }
            }
            break;
        case DEV_GPS:
            gpsdevice = new GpsDevice();
            if (gpsdevice) {
                if (gpsdevice->DevOpen(dev_str))
                    device = (void*)gpsdevice;
                else
                   delete(gpsdevice);     
            }
            break;
        case DEV_BROCAST:
            bdevice = new BrocastDevice();
            if (bdevice) {
                if (bdevice->DevOpen(dev_str))
                    device = (void*)bdevice;
                else
                   delete(bdevice);     
            }
            break;

        default:
            break;
    }
    return device;
}

void DeviceClose(void* handle,device_type_t type)
{
    YmLamp* ym_device;
    InfoScreenColt* colt_device;
    GpsDevice* gpsdevice = NULL;
    BrocastDevice* bdevice = NULL;

    switch (type)
    {
        case DEV_LAMP:
            ym_device = (YmLamp*)handle;
            ym_device->DevClose();
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            colt_device->DevClose();
            break;
        case DEV_GPS:
            gpsdevice = (GpsDevice*)handle;
            gpsdevice->DevClose();
            delete(gpsdevice); 
            break;
        case DEV_BROCAST:
            bdevice = (BrocastDevice*)handle;
            bdevice->DevClose();
            delete(bdevice); 
            break;
        default:
            break;
    }
}

bool DeviceHeartbeat(void* handle,device_type_t type) {
    bool ret = false;
    GpsDevice* gdevice = NULL;

    switch (type) {
        case DEV_GPS:
            gdevice = (GpsDevice*)handle;
            ret = gdevice->Heartbeat();
            break;
        default:
            break;
    }
    return ret;    
}

int GetResolutionFun(void* handle, device_type_t type, void* output)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;
    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            printf("###########1\n");
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtGetResolution(output);
            break;
        default:
            break;
    }
    return ret;
}

int GetInfoFun(void* handle, device_type_t type, void* output)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtGetInfo(output);
            break;
        default:
            break;
    }
    return ret;
}

int GetVolumeFun(void* handle, device_type_t type, void* output)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtGetVolume(output);
            break;
        default:
            break;
    }
    return ret;
}

int GetOnoffFun(void* handle, device_type_t type, void* output)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtGetOnoff(output);
            break;
        default:
            break;
    }
    return ret;
}

int GetBrightnessFun(void* handle, device_type_t type, void* output)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtGetBrightness(output);
            break;
        default:
            break;
    }
    return ret;
}

int SetOnoffFun(void* handle, device_type_t type, int channel, int onoff)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;
    YmLamp* ym_device;

    switch (type)
    {
        case DEV_LAMP:
            ym_device = (YmLamp*)handle;
            ym_device->LampYmSetOnoff(channel,onoff);
            break;
        case DEV_INFOSCREEN:
            printf("###########SetOnoffFun\n");
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtSetOnoff(onoff);
            break;
        default:
            break;
    }
    return ret;
}

int SetVolumeFun(void* handle, device_type_t type, int channel, int volume)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtSetVolume(volume);
            break;
        default:
            break;
    }
    return ret;
}

int SetBrightnessFun(void* handle, device_type_t type, int channel, int brightness)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;
    YmLamp* ym_device;

    switch (type)
    {
        case DEV_LAMP:
            ym_device = (YmLamp*)handle;
            ym_device->LampYmSetOnoff(channel,brightness);
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtSetBrightness(brightness);
            break;
        default:
            break;
    }
    return ret;
}

int ScreenshotFun(void* handle, device_type_t type, int channel, const char* path)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtScreenshot(path);
            break;
        default:
            break;
    }
    return ret;
}

int RebootFun(void* handle, device_type_t type, int channel)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtReboot();
        default:
            break;
    }
    return ret;
}

int IdleFun(void* handle, device_type_t type, int channel)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtIdle();
            break;
        default:
            break;
    }
    return ret;
}

int LoadProgramFun(void* handle, device_type_t type, int channel, const char* path)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device->LedScreenColtLoadProgram(path);
            break;
        default:
            break;
    }
    return ret;
}

int SetNtpFun(void* handle, device_type_t type, int channel, const char* ntp)
{
    int ret = UnkownErr;
    InfoScreenColt* colt_device;

    switch (type)
    {
        case DEV_LAMP:
            break;
        case DEV_INFOSCREEN:
            colt_device = (InfoScreenColt*)handle;
            ret = colt_device-> LedScreenColtScreenSetNtp(ntp);
            break;
        default:
            break;
    }
    return ret;
}

int GetGPSMsgFun(void* handle, device_type_t type, void* output) {
    int ret = UnkownErr;
    GpsDevice* gdevice = NULL;

    switch (type) {
        case DEV_GPS:
            gdevice = (GpsDevice*)handle;
            ret = gdevice->GetGPSMsg(output);
            break;
        default:
            break;
    }
    return ret;
}

int BrocastPlayFun(void* handle, device_type_t type, const char* path, void* output) {
    int ret = UnkownErr;
    BrocastDevice* gdevice = NULL;

    switch (type) {
        case DEV_BROCAST:
            gdevice = (BrocastDevice*)handle;
            ret = gdevice->Play(path, output);
            break;
        default:
            break;
    }
    return ret;
}

int BrocastPauseFun(void* handle, device_type_t type, void* output) {
    int ret = UnkownErr;
    BrocastDevice* gdevice = NULL;

    switch (type) {
        case DEV_BROCAST:
            gdevice = (BrocastDevice*)handle;
            ret = gdevice->Pause(output);
            break;
        default:
            break;
    }
    return ret;
}

int BrocastContinueFun(void* handle, device_type_t type, void* output) {
    int ret = UnkownErr;
    BrocastDevice* gdevice = NULL;

    switch (type) {
        case DEV_BROCAST:
            gdevice = (BrocastDevice*)handle;
            ret = gdevice->Continue(output);
            break;
        default:
            break;
    }
    return ret;
}

int BrocastStopFun(void* handle, device_type_t type, void* output) {
    int ret = UnkownErr;
    BrocastDevice* gdevice = NULL;

    switch (type) {
        case DEV_BROCAST:
            gdevice = (BrocastDevice*)handle;
            ret = gdevice->Stop(output);
            break;
        default:
            break;
    }
    return ret;
}