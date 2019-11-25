#include <stdio.h>
#include <stdlib.h>
int GetResolution(void* handel, int type, void* input, void* output);
int GetInfo(void* handel, int type, void* input, void* output);
int SetOnoff(void* handel, int type, void* input, void* output);
int GetOnoff(void* handel, int type, void* input, void* output);
int SetVolume(void* handel, int type, void* input, void* output);
int GetVolume(void* handel, int type, void* input, void* output);
int SetBrightness(void* handel, int type, void* input, void* output);
int GetBrightness(void* handel, int type, void* input, void* output);
int Screenshot(void* handel, int type, void* input, void* output);
int Reboot(void* handel, int type, void* input, void* output);
int Idle(void* handel, int type, void* input, void* output);
int LoadProgram(void* handel, int type, void* input, void* output);
int SetNtp(void* handel, int type, void* input, void* output);
int GetNtp(void* handel, int type, void* input, void* output);
int GetGPSMsg(void* handle, int type, void* input, void* output);
int BrocastPlay(void* handle, int type, void* input, void* output);
int BrocastPause(void* handle, int type, void* input, void* output);
int BrocastContinue(void* handle, int type, void* input, void* output);
int BrocastStop(void* handle, int type, void* input, void* output);