#include "MethodParamParser.hh"
#include "MethodParser.hh"

int GetResolution(void* handle, int type, void* input, void* output)
{
    printf("GetResolution:%d\n",type);
    int ret = UnkownErr;
    ret = GetResolutionFun(handle, device_type_t(type), output);
    return ret;
}

int GetInfo(void* handle, int type, void* input, void* output)
{
    printf("GetInfo\n");
    int ret = UnkownErr;
    ret = GetInfoFun(handle, device_type_t(type), output);
    return ret;
}

int SetOnoff(void* handle, int type, void* input, void* output)
{
    printf("SetOnoff:%s\n", (char*)input);
    int ret;
    setonoff_param_t onoff_t;

    if(!input){
        return InputParamErr;
    }

    ret = GetMethodParam((const char*)input, SCR_SET_ONOFF, (void*)&onoff_t);
    if (ret == Success) {
        printf("SetOnoff#:%d\n", onoff_t.onoff);
        ret = SetOnoffFun(handle, device_type_t(type), onoff_t.channel, onoff_t.onoff);
    }
    return ret;
}

int GetOnoff(void* handle, int type, void* input, void* output) 
{
    printf("GetOnoff\n");
    int ret = UnkownErr;
    ret = GetOnoffFun(handle, device_type_t(type), output);
    return ret;
}

int SetVolume(void* handle, int type, void* input, void* output) 
{
    printf("SetVolume\n");
    setvolume_param_t volume_t;
    int ret;

    if(!input){
        return InputParamErr;
    }

    ret = GetMethodParam((const char*)input, SCR_SET_VOLUME, (void*)&volume_t);
    if ((ret == Success) && volume_t.volume <=100) {
        ret = SetVolumeFun(handle, device_type_t(type), volume_t.channel,volume_t.volume);
    } else {
        ret = InputParamErr;
    }

    return ret;
}

int GetVolume(void* handle, int type, void* input, void* output) 
{
    printf("GetVolume\n");
    int ret = UnkownErr;
    ret = GetVolumeFun(handle, device_type_t(type), output);
    return ret;
}

int SetBrightness(void* handle, int type, void* input, void* output) 
{
    printf("SetBrightness\n");
    setbright_param_t bright_t;
    int ret = UnkownErr;

    if(!input){
        return InputParamErr;
    }

    ret = GetMethodParam((const char*)input, SCR_SET_BRIGHTNESS, (void*)&bright_t);
    if ((ret == Success) && bright_t.brightness <=100) {
        ret = SetBrightnessFun(handle, device_type_t(type), bright_t.channel,bright_t.brightness);
    } else {
        ret = InputParamErr;
    }
    return ret;
}   

int GetBrightness(void* handle, int type, void* input, void* output)
{
    int ret = UnkownErr;
    ret = GetBrightnessFun(handle, device_type_t(type), output);
    return ret;
} 

int Screenshot(void* handle, int type, void* input, void* output)
{
    printf("Screenshot\n");
    screenshot_param_t shot_t;
    int ret = UnkownErr;

    if(!input){
        return InputParamErr;
    }

    ret = GetMethodParam((const char*)input, SCR_SCREENSHOT, (void*)&shot_t);
    if (ret == Success) {
        ret = ScreenshotFun(handle, device_type_t(type), shot_t.channel, shot_t.path);
    } else {
        ret = InputParamErr;
    }

    return ret;
}

int Reboot(void* handle, int type, void* input, void* output)
{
    printf("Reboot\n");
    int ret;
    ret = RebootFun(handle, device_type_t(type), 0);
    return ret;
}

int Idle(void* handle, int type, void* input, void* output)
{
    printf("Idle\n");
    int ret = UnkownErr;
    ret = IdleFun(handle, device_type_t(type), 0);
    return ret;
}

int LoadProgram(void* handle, int type, void* input, void* output)
{
    printf("LoadProgram\n");
    int ret = UnkownErr;
    loadprogram_param_t program_t;

    if (Success == GetMethodParam((const char*)input, SCR_LOAD_PROGRAM, (void*)&program_t)) {
        ret = LoadProgramFun(handle, device_type_t(type), program_t.channel, program_t.program);
    }

    return ret;
}

int SetNtp(void* handle, int type, void* input, void* output)
{
    printf("ScreenSetNtp\n");
    int ret = UnkownErr;
    char ntp[128];

    if (Success == GetMethodParam((const char*)input, SCR_SET_NTP, (void*)ntp)) {
        ret = SetNtpFun(handle, device_type_t(type), 0, ntp);
    }
    return ret;
}

int GetNtp(void* handle, int type, void* input, void* output)
{
    printf("ScreenGetNtp\n");
    int ret = UnkownErr;
   // ret = GetNtpFun(handle, type, output);
    return ret;
}

int GetGPSMsg(void* handle, int type, void* input, void* output) {
    printf("GpsGetLatitude\n");
    int ret = UnkownErr;

    ret = GetGPSMsgFun(handle, device_type_t(type), output);
    return ret;
}

int BrocastPlay(void* handle, int type, void* input, void* output) {
    printf("BrocastPlay\n");
    int ret = UnkownErr;
    char pathname[128];

    if (Success == GetMethodParam((const char*)input, BROCAST_PLAY, (void*)pathname)) {
        ret = BrocastPlayFun(handle, device_type_t(type), pathname, output);
    }
    return ret;
}

int BrocastPause(void* handle, int type, void* input, void* output) {
    printf("BrocastPause\n");
    int ret = UnkownErr;

    ret = BrocastPauseFun(handle, device_type_t(type), output);
    return ret;
}

int BrocastContinue(void* handle, int type, void* input, void* output) {
    printf("BrocastContinue\n");
    int ret = UnkownErr;

    ret = BrocastContinueFun(handle, device_type_t(type), output);
    return ret;
}

int BrocastStop(void* handle, int type, void* input, void* output) {
    printf("BrocastStop\n");
    int ret = UnkownErr;

    ret = BrocastStopFun(handle, device_type_t(type), output);
    return ret;
}