#include <map>
#include <functional>
#include "DeviceInterface.hh"
#include "MethodParamParser.hh"
#include "MethodParser.hh"

using namespace std::placeholders; 

typedef std::function<bool(void*,int,void*,void*)> Func; 

void MethodMapInit(std::map<int, Func>& _funcMap)
{
    _funcMap.insert(std::make_pair(SCR_GET_RESOILUTION, std::bind(&GetResolution, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_GET_INFO, std::bind(&GetInfo, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_SET_ONOFF, std::bind(&SetOnoff, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_GET_ONOFF, std::bind(&GetOnoff, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_SET_VOLUME, std::bind(&SetVolume, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_GET_VOLUME, std::bind(&GetVolume, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_SET_BRIGHTNESS, std::bind(&SetBrightness, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_GET_BRIGHTNESS, std::bind(&GetBrightness, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_SCREENSHOT, std::bind(&Screenshot, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_REBOOT, std::bind(&Reboot, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_IDEL, std::bind(&Idle, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_LOAD_PROGRAM, std::bind(&LoadProgram, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(SCR_SET_NTP, std::bind(&SetNtp, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(GET_GPS_MSG, std::bind(&GetGPSMsg, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(BROCAST_PLAY, std::bind(&BrocastPlay, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(BROCAST_PAUSE, std::bind(&BrocastPause, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(BROCAST_CONTINUE, std::bind(&BrocastContinue, _1, _2, _3, _4)));
    _funcMap.insert(std::make_pair(BROCAST_STOP, std::bind(&BrocastStop, _1, _2, _3, _4)));
}

void* Open(const char* device, int type, int flags)
{   
    void* handle = DeviceOpen(device, device_type_t(type));
    return handle;
}

int Close(void* handle, int type)
{
    int ret = -1;
    if (handle) {
        DeviceClose(handle, device_type_t(type));
        ret = 0;
    }
    return ret;
}

int Ioctl(void* handle, int type, int cmd, void* input_param, void* out_param)
{
    int ret = UnkownErr;
    std::map<int, Func> _funcMap; 

    if (handle) {
        MethodMapInit(_funcMap);
        method_t method = method_t(cmd);
        ret = _funcMap[method](handle, type, input_param, out_param);
    }
    
    return ret;
}

bool Heartbeat(void* handle, int type) {
    int ret = UnkownErr;

    if (handle) {
        ret = DeviceHeartbeat(handle, device_type_t(type));
    }

    return ret;    
}