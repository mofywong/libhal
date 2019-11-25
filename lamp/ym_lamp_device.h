#ifndef __YM_LAMP_DEVICE_H__
#define __YM_LAMP_DEVICE_H__
#include <pthread.h>
#include "types.h"
#include "serial.h"
#include "list.h"
#include "ym_lamp_type.h"
#include "ym_lamp_device_ability.h"



#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym_lamp_device_ctx_s
{
    ym_lamp_device_model_t              model;
    ym_lamp_device_ability_t*           ability;
    ym_lamp_device_addr_t               addr;
    char                                dev[32];
    he_utils_serial_port_t              port;
    pthread_mutex_t                     serial_mutex;
    he_utils_serial_t*                  serial_ctx;
    pthread_mutex_t                     status_list_mutex;
    he_utils_list_head_t                status_list; /* 节点定义为 ym_lamp_device_channel_status_t */
}ym_lamp_device_ctx_t;

ym_lamp_device_ctx_t* ym_lamp_device_new (const_string_t device);

void ym_lamp_device_free(ym_lamp_device_ctx_t* ctx);

//bool_t ym_lamp_device_connect(ym_lamp_device_ctx_t* ctx);
//void ym_lamp_device_disconnect(ym_lamp_device_ctx_t* ctx);

bool_t ym_lamp_device_set_enable_time_policy(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t enable);
bool_t ym_lamp_device_set_onoff(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t onoff);
bool_t ym_lamp_device_set_brightness(ym_lamp_device_ctx_t* ctx, uint8_t channel, uint8_t brightness);

bool_t ym_lamp_device_get_ability(ym_lamp_device_ctx_t* ctx, ym_lamp_device_ability_t** ability);
bool_t ym_lamp_device_get_attr(ym_lamp_device_ctx_t* ctx, void* output);

#ifdef __cplusplus
}
#endif

#endif  // __YM_LAMP_DEVICE_H__
