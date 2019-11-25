#include <parson/parson.h>
#include "DeviceInterface.hh"

bool creatjson(char* buf)
{
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;
    char* data;
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
    //json_object_set_string(object, "Program", "/home/liaoxw/7g_box/flask-svr/screen-svr/libInfoScreen/4028b18a67780f07016927d29e6d00ad");
    json_object_set_number(object, "OnOff", 1);
	data = json_serialize_to_string_pretty(root);
    if (data) {
        snprintf(buf, 128, "%s", data);
        ret = true;
    }
end:
    json_value_free(root);
    return ret;
}

bool creatjson1(char* buf)
{
    JSON_Value *root;
    JSON_Object *object;
    bool ret = false;
    char* data;

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
    json_object_set_string(object, "ip", "192.168.53.42");
    data = json_serialize_to_string_pretty(root);
    if (data) {
        snprintf(buf, 128, "%s", data);
        ret = true;
    }
end:
    json_value_free(root);
    return ret;
}

int main()
{   
    char input[128];
    
    char output[256];
    char dev[256];
    int ret = -1;
    creatjson(input);
    creatjson1(dev);
    printf("dev:%s\n", dev);
    void* handle = Open(dev, 2,1);
    if (handle) {
        printf("input:%s\n", input);
        ret = Ioctl(handle, 2,5,input, output);
        printf("ret:%d\n",ret);
        printf("put:%s\n", output);
    }
    
    
    return 1;
}