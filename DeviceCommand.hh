#include <stdio.h>
#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif
typedef enum {
    SCR_SET_ONOFF = 1,
    SCR_SET_VOLUME = 2,
    SCR_SET_BRIGHTNESS = 3,
    SCR_SCREENSHOT = 4,
    SCR_LOAD_PROGRAM = 5,
    SCR_WITCH_PROGRAM = 6,
    SCR_SET_NTP = 7,
    SCR_GET_RESOILUTION = 8,
    SCR_GET_INFO = 9,
    SCR_GET_ONOFF = 10,
    SCR_GET_VOLUME = 11,
    SCR_GET_BRIGHTNESS = 12,
    SCR_REBOOT = 13,
    SCR_IDEL = 14,
    GET_GPS_MSG = 15,
    BROCAST_PLAY = 16,
    BROCAST_PAUSE = 17,
    BROCAST_CONTINUE = 18,
    BROCAST_STOP = 19
}method_t;

typedef enum {
    DEV_LAMP         = 1,
    DEV_INFOSCREEN   = 2,
    DEV_GPS          = 4,
    DEV_BROCAST      = 5
} device_type_t;

enum error_code{
    Success = 0,
    UnkownErr = 1,
    InputParamErr = 2,
    NoSupport = 3,
    Timeout = 4,
    AccessFailure = 5,
    CreateFailure = 6,
    DataInvalid = 7,
    NotReady = 8,
    FuncErr = 9,
    CmdInvalid = 10,
    ConnectFailure = 11,
};

typedef struct setonoff_param_s {
    int channel;
    int onoff;
} setonoff_param_t;

typedef struct setbright_param_s {
    int channel;
    int brightness;
} setbright_param_t;

typedef struct setvolume_param_s {
    int channel;
    int volume;
} setvolume_param_t;


typedef struct loadprogram_param_s {
    int channel;
    char program[128];
} loadprogram_param_t;

typedef struct screenshot_param_s {
    int channel;
    char path[128];
} screenshot_param_t;

int GetMethodParam(const char* data, method_t method, void* input_param);

#ifdef  __cplusplus
}
#endif



