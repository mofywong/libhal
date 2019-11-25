#include <string.h>
#include "ColtCommand.hh"

#define MAX_BUFF_LEN          512

using namespace std; 

size_t HttpGetCallback(void *ptr, size_t size, size_t nmemb, void *userdata) 
{
    char* data = (char*)ptr;
    char* buf = (char*)userdata;
    size_t ret = -1;  
    printf("callback:%s\n", data);
    if (data) {
        snprintf(buf,  MAX_BUFF_LEN, "%s", data);
        ret = size * nmemb;
    }

    return ret;
}

size_t NtpCallback(void *ptr, size_t size, size_t nmemb, void *userdata) 
{  
    char* data = (char*)ptr;
    
    printf("get ntp:%s\n", data);
    return size * nmemb;
}

bool CreateNtpJson(char* string, const char*  ntp, int interval,int threshold)
{  
    char*  data;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_value_init_object(); 
    if (root) { 
        object = json_object(root);
        if (object) {
            json_object_set_string(object, "ntpserver", ntp);
            json_object_set_number(object, "ntpthreshold", threshold);
            json_object_set_number(object, "ntpinterval", interval);
            data = json_serialize_to_string(root);
            if(data) {
                snprintf(string, 128, "%s", data); 
                json_free_serialized_string(data);
                ret = true;
            }
        }
        json_value_free(root);
    }
    return ret;
}

bool GetFileName(const std::string& dir, std::map<std::string, std::string>& file_map)
{
    char key[4];
    char value[258];
    bool ret = false;
    DIR * dir_path;
    struct dirent * ptr;
    int index = 0;

    dir_path = opendir(dir.c_str());
    if (dir_path) {
        while((ptr = readdir(dir_path)) != NULL) //循环读取目录数据
        {
            if (8 == ptr->d_type) {
                printf("d_name : %s\n", ptr->d_name); //输出文件名
                memset(key, 0, sizeof(key));
                snprintf(key, sizeof(key), "f%d", ++index);
                memset(value, 0, sizeof(value));
                snprintf(value, sizeof(value), "%s/%s", dir.c_str(), ptr->d_name);
                file_map.insert(pair<std::string, std::string>(key, value));
                ret = true;
            }
        }
        closedir(dir_path);//关闭目录指针
    }
    
    return ret;
 }

bool CreateRebootJson(char*  string)
{  
    char* data;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_value_init_object(); 
    if (root) { 
        object = json_object(root);
        if (object) {
            json_object_set_string(object, "command", "reboot");
            data = json_serialize_to_string(root);
            if(data) {
                snprintf(string, 64, "%s", data); 
                json_free_serialized_string(data);
                ret = true;
            }
        }
        json_value_free(root);
    }
    return ret;
}

size_t ScreenshotCallback(void *ptr, size_t size, size_t nmemb, void *stream) 
{
    return fwrite((FILE*)ptr, size, nmemb, (FILE*)stream);
}

bool CreateBrightnessJson(char*  string, int value)
{  
    char* data;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_value_init_object(); 
    if (root) { 
        object = json_object(root);
        if (object) {
            json_object_set_number(object, "brightness", value);
            data = json_serialize_to_string(root);
            if(data) {
                snprintf(string, 64, "%s", data); 
                json_free_serialized_string(data);
                ret = true;
            }
        }
        json_value_free(root);
    }
    return ret;
}

bool CreateVolumeJson(char* string, int value)
{  
    char* data;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_value_init_object(); 
    if (root) { 
        object = json_object(root);
        if (object) {
            json_object_set_number(object, "musicvolume", value);
            data = json_serialize_to_string(root);
            if(data) {
                snprintf(string, 64, "%s", data); 
                json_free_serialized_string(data);
                ret = true;
            }
        }
        json_value_free(root);
    }
    return ret;
}

bool CreateOnoffJson(char* string, int onoff)
{  
    char* data;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_value_init_object();
    if (root) { 
        object = json_object(root);
        if (object) { 
            if (onoff == 0) {
                json_object_set_string(object, "command", "sleep");
                
            } else {
                json_object_set_string(object, "command", "wakeup");
            }
            data = json_serialize_to_string(root);
            if(data) {
                snprintf(string, 64, "%s", data); 
                json_free_serialized_string(data);
                ret = true;
            }
        }
        json_value_free(root);
    }
    return ret;
}

bool JsonGetScreenInfo(const std::string& data, colt_info_t* info)
{
    JSON_Value *root;
    JSON_Object *object;
    const char*  str_value;  
    double value;
    bool ret = false;

    if (data.empty() || !info) {
        return ret;
    }

    root = json_parse_string(data.c_str());
    if(!root) {
        return ret;
    }
    
    object = json_object (root);
    if (!object) {
        goto end;
    }

    str_value = json_object_dotget_string(object, "info.vername");
    if (!str_value) {
        goto end;
    }
    snprintf(info->ver, sizeof(info->ver), "%s", str_value);

    str_value = json_object_dotget_string(object, "info.serialno");
    if (!str_value) {
        goto end;
    }
    snprintf(info->sn, sizeof(info->sn), "%s", str_value);

    str_value = json_object_dotget_string(object, "info.model");
    if (!str_value) {
        goto end;
    }
    snprintf(info->model, sizeof(info->model), "%s", str_value);

    str_value = json_object_dotget_string(object, "info.playing.name");
    if (!str_value) {
        goto end;
    }
    snprintf(info->vsn, sizeof(info->vsn), "%s", str_value);

    value = json_object_dotget_number(object, "info.up");
    if (0 == value) {
        goto end;
    }
    info->up_time = (int)value;

    value = json_object_dotget_number(object, "info.mem.total");
    if (0 == value) {
        goto end;
    }
    info->total_mem = (int)value;

    value = json_object_dotget_number(object, "info.mem.free");
    if (0 == value) {
        goto end;
    }
    info->free_mem = (int)value;

    value = json_object_dotget_number(object, "info.storage.total");
    if (0 == value) {
        goto end;
    }
    info->total_disk = (int)value;

    value = json_object_dotget_number(object, "info.storage.free");
    if (0 == value) {
        goto end;
    }
    info->free_disk = (int)value;
    ret = true;

end:
    json_value_free(root);
    return ret;
}

bool JsonGetScreenResolution(const std::string& data, colt_resolution_t* resolution)
{
    JSON_Value *root;
    JSON_Object *object;
    double value;
    bool ret = false;

    if(data.empty() || !resolution) {
        return ret;
    }

    root = json_parse_string(data.c_str());
    if (root) {
        object = json_object(root);
        if (object) {
            value = json_object_get_number(object, "real_height");
            if (0 == value) {
                goto end;
            }
            resolution->height = (uint16_t)value;
            value = json_object_get_number(object, "real_width");
            if (0 == value) {
                goto end;
            }
            resolution->width = (uint16_t)value;
            ret = true;
        }
end:
        json_value_free(root);
    }
    return ret;
}

bool JsonGetScreenOnoff(const std::string& data, bool* onoff)
{
    double value;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_parse_string(data.c_str());
    if (root) {
        object = json_object(root);
        if (object) {
            value = json_object_get_number(object, "powerstatus");
            /*返回0则解析失败*/
            if (value == 1) {
                *onoff = true;
            } else {
                *onoff = false;
            }
            ret = true;
        }
        json_value_free(root);
    }

    return ret;
}

bool JsonGetScreenVolume(const std::string& data, int* volume)
{
    double value;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_parse_string(data.c_str());
    if (root) {
        object = json_object(root);
        if (object) {
            value = json_object_get_number(object,  "musicvolume");
            *volume = (int)value;
            ret = true;
        }
        json_value_free(root);
    }
    return ret;
}

bool JsonGetScreenBrightness(const std::string& data, int* brightness)
{
    double value;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;

    root = json_parse_string(data.c_str());
    if (root) {
        object = json_object(root);
        if (object) {
            value = json_object_get_number(object,  "brightness");
            *brightness = (int)value;
            ret = true;
        }
        json_value_free(root);
    }
    return ret;
}

bool JsonGetOpenParam(const std::string& data, std::string& ip)
{
    const char* str;
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;
    
    root = json_parse_string(data.c_str());
    if (root) {
        object = json_object(root);
        if (object) {
            str = json_object_get_string(object,  "ip");
            ip = str;
            ret = true;
        }
        json_value_free(root);
    }
    return ret;
}

bool CreatMethodOutput(method_t method, void* input_param, void* output_param)
{
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;
    char* data;
    colt_resolution_t* resolution;
    colt_info_t* info;
    bool onoff;
    int volume;
    int brightness;

    root = json_value_init_object();
    if (!root) {
        return false;
    }
    object = json_object(root);
    if (!object) {
        goto end;
    }
 
    switch(method) {
        case SCR_GET_RESOILUTION:
            resolution = (colt_resolution_t *)input_param;
            json_object_set_number(object, "Width", resolution->width);
            json_object_set_number(object, "Height", resolution->height);
            break;

        case SCR_GET_INFO:
            info = (colt_info_t *)input_param;
            json_object_set_number(object, "Uptime", info->up_time);
            json_object_set_number(object, "Totalmem", info->total_mem);
            json_object_set_number(object, "Freemem", info->free_mem);
            json_object_set_number(object, "TotalDisk", info->total_disk);
            json_object_set_number(object, "FreeDisk", info->free_disk);
            json_object_set_string(object, "Model", info->model);
            json_object_set_string(object, "Versoin", info->ver);
            json_object_set_string(object, "Sn", info->sn);
            break;

        case SCR_GET_ONOFF:
            onoff = *(bool*)input_param;
            if (onoff) {
                json_object_set_number(object, "OnOff", 1);
            } else {
                json_object_set_number(object, "OnOff", 0);
            }
            break;

        case SCR_GET_VOLUME:
            volume = *(int*)input_param;
            json_object_set_number(object, "Volume", volume);
            break;

        case SCR_GET_BRIGHTNESS:
            brightness = *(int *)input_param;
            json_object_set_number(object, "Brightness", brightness);
            break;
        default:
            break;
    }
    data = json_serialize_to_string_pretty(root);
    if (data) {
        snprintf((char*)output_param, 256, "%s", data);
        ret = true;
    }
end:
    json_value_free(root);
    return ret;
}