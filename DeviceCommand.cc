#include <parson/parson.h>
#include "DeviceCommand.hh"

int GetMethodParam(const char* data, method_t method,void* input_param)
{
    double channel;
    double value1;
    const char* str;
    JSON_Value *root;
    JSON_Object *object;
    setonoff_param_t* onoff_t;
    setvolume_param_t* volume_t;
    setbright_param_t* bright_t;
    screenshot_param_t* screenshot_t;
    loadprogram_param_t* program_t;

    int ret = InputParamErr; 
    root = json_parse_string(data);
    if (!root) {
        return false;
    }
    object = json_object(root);
    if (!object) {
        goto end;
    }
    switch(method) {
        case SCR_SET_ONOFF:
            value1 = json_object_get_number(object, "OnOff");
            channel = json_object_get_number(object, "Channel");
            onoff_t = (setonoff_param_t*)input_param;
            onoff_t->channel = channel;
            onoff_t->onoff = value1;
            ret = Success;
            break;

        case SCR_SET_VOLUME:
            value1 = json_object_get_number(object, "Volume");
            channel = json_object_get_number(object, "Channel");
            volume_t = (setvolume_param_t*)input_param;
            volume_t->channel = channel;
            volume_t->volume = value1;
            ret = Success;
            break;

        case SCR_SET_BRIGHTNESS:
            value1 = json_object_get_number(object, "Brightness");
            channel = json_object_get_number(object, "Channel");
            bright_t = (setbright_param_t*)input_param;
            bright_t->channel = channel;
            bright_t->brightness = value1;
            ret = Success;
            break;

        case SCR_SCREENSHOT:
            str = json_object_get_string(object, "Path");
            if (str) {
                screenshot_t = (screenshot_param_t*)input_param;
                screenshot_t->channel = channel;
                snprintf(screenshot_t->path, 128, "%s", str);
                ret = Success;
            }
            break;

        case SCR_LOAD_PROGRAM:
            str = json_object_get_string(object, "Program");
            if (str) {
                channel = json_object_get_number(object, "Channel");
                program_t = (loadprogram_param_t*)input_param;
                program_t->channel = channel;
                snprintf(program_t->program, 128, "%s", str);
                ret = Success;
            }
            break;

        case SCR_WITCH_PROGRAM:
            str = json_object_get_string(object, "Program");
            if (str) {
                channel = json_object_get_number(object, "Channel");
                snprintf((char*)input_param, 128, "%s", str);
                ret = Success;
            }
            break;

        case SCR_SET_NTP:
            str = json_object_get_string(object, "Ntp");
            if (str) {
                snprintf((char*)input_param, 128, "%s", str);
                printf("s:%s\n", str);
                ret = Success;
            }
            break;
        case BROCAST_PLAY:
            str = json_object_get_string(object, "Pathname");
            if (str) {
                snprintf((char*)input_param, 128, "%s", str);
                printf("s:%s\n", str);
                ret = Success;
            }
            break;
        default:
            break;
    }

end:
    json_value_free(root);
    return ret;
}


