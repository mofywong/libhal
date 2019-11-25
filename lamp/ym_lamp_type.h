#ifndef __YM_LAMP_TYPE_H__
#define __YM_LAMP_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "list.h"

typedef enum
{
    YM_LAMP_DEVICE_MODEL_UNKNOW                      = 0,
    YM_LAMP_DEVICE_MODEL_103_1_L_R_KCT               = 1,
} ym_lamp_device_model_t;

/* 灯控一路灯的状态信息 */
typedef struct
{
    he_utils_list_head_t    node;
    uint8_t                 channel;
    bool_t                  time_policy;
    bool_t                  onoff;
    uint8_t                 brightness;
    uint32_t                voltage;
    uint32_t                current;
    uint16_t                active_power;
    uint32_t                active_total_energy; 
} ym_lamp_device_channel_status_t;



typedef char ym_lamp_device_addr_t[6];

#ifdef __cplusplus
}
#endif

#endif  // __YM_LAMP_TYPE_H__
