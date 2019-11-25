#include <stdio.h>
#include <stdlib.h>
#ifdef  __cplusplus
extern "C" {
#endif
void* Open(const char* device, int type, int flags);
int Close(void* handle, int type);
int Ioctl(void* handle, int type, int cmd, void* input_param, void* out_param);
bool Heartbeat(void* handle, int type);
//int Read(void* handel, int type, int cmd, void* out_param, ReadCallBack callback = NULL, void* callback_param = NULL);
//int Write(void* handel, int type, int cmd, void* input_param, WriteCallBack callback = NULL, void* callback_param = NULL);

#ifdef  __cplusplus
}
#endif
