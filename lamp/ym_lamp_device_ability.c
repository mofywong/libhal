#include "ym_lamp_device_ability.h"

ym_lamp_device_ability_t* ym_lamp_device_ability_create (ym_lamp_device_model_t model)
{
    ym_lamp_device_ability_t* ability = (ym_lamp_device_ability_t*)malloc(sizeof(ym_lamp_device_ability_t));

    if (ability)
    {
        switch (model)
        {
            case YM_LAMP_DEVICE_MODEL_103_1_L_R_KCT:
            {
                ability->channel = 1;

                ability->attribute.time_policy = true;
                ability->attribute.onoff = true;
                ability->attribute.brightness = true;
                ability->attribute.voltage = true;
                ability->attribute.current = true;
                ability->attribute.active_power = true;
                ability->attribute.active_total_energy = true;

                ability->method.set_onoff = true;
                ability->method.set_brightness = true;
                ability->method.set_onoff_with_time = true;
                ability->method.set_brightness_with_time = true;
                ability->method.set_onoff_with_resume = true;
                ability->method.set_brightness_with_resume = true;
                ability->method.clear_time_task = true;
                ability->method.enable_time_policy = true;
                break;
            }
            default:
            {
                printf("ym_lamp_device_ability_create model error\n");
                break;
            }
        }
    }
    return ability;
}