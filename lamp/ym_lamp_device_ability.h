#ifndef __YM_LAMP_DEVICE_ABILITY_H__
#define __YM_LAMP_DEVICE_ABILITY_H__


#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
#include "ym_lamp_type.h"
/* 方法能力 */
typedef struct
{
    bool_t set_onoff;
    bool_t set_brightness;
    bool_t set_onoff_with_time;
    bool_t set_brightness_with_time;
    bool_t set_onoff_with_resume;
    bool_t set_brightness_with_resume;
    bool_t clear_time_task;
    bool_t enable_time_policy;
} ym_lamp_device_method_ability_t;

/* 属性能力 */
typedef struct
{
    bool_t time_policy;
    bool_t onoff;
    bool_t brightness;
    bool_t voltage;
    bool_t current;
    bool_t active_power;            /* 有功功率 */
    bool_t active_total_energy;     /* 有功总电能（电度值）*/
} ym_lamp_device_attribute_ability_t;

typedef struct
{
    uint8_t                                channel;    /* 该控制器支持的灯的路数 */
    ym_lamp_device_method_ability_t        method;
    ym_lamp_device_attribute_ability_t     attribute;
} ym_lamp_device_ability_t;

ym_lamp_device_ability_t* ym_lamp_device_ability_create (ym_lamp_device_model_t model);

#ifdef __cplusplus
}
#endif

#endif  // __YM_LAMP_DEVICE_ABILITY_H__
