#include "DeviceCommand.hh"

#ifdef  __cplusplus
extern "C" {
#endif

void* DeviceOpen(const char* dev_str, device_type_t type);
void DeviceClose(void* handle,device_type_t type);
bool DeviceHeartbeat(void* handle,device_type_t type);
int GetResolutionFun(void* handle, device_type_t type, void* output);
int GetInfoFun(void* handle, device_type_t type, void* output);
int GetVolumeFun(void* handle, device_type_t type, void* output);
int GetOnoffFun(void* handle, device_type_t type, void* output);
int GetBrightnessFun(void* handle, device_type_t type, void* output);
int ScreenGetNtpFun(void* handle, device_type_t type, void* output);
int SetOnoffFun(void* handle, device_type_t type, int channel, int onoff);
int SetVolumeFun(void* handle, device_type_t type, int channel, int volume);
int SetBrightnessFun(void* handle, device_type_t type, int channel, int brightness);
int ScreenshotFun(void* handle, device_type_t type, int channel, const char* path);
int RebootFun(void* handle, device_type_t type, int channel);
int IdleFun(void* handle, device_type_t type, int channel);
int LoadProgramFun(void* handle, device_type_t type, int channel,const char* path);
int SetNtpFun(void* handle, device_type_t type, int channel, const char* ntp);
int GetGPSMsgFun(void* handle, device_type_t type, void* output);
int BrocastPlayFun(void* handle, device_type_t type, const char* path, void* output);
int BrocastPauseFun(void* handle, device_type_t type, void* output);
int BrocastContinueFun(void* handle, device_type_t type, void* output);
int BrocastStopFun(void* handle, device_type_t type, void* output);

#ifdef  __cplusplus
}
#endif