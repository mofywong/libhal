#include <map>
#include <functional>
#include <curl/curl.h>
#include <dirent.h>
#include <parson/parson.h>
#include "DeviceCommand.hh"

size_t HttpGetCallback(void *ptr, size_t size, size_t nmemb, void *userdata);

#ifdef  __cplusplus
extern "C" {
#endif
typedef struct {
    int width;
    int height;
} colt_resolution_t;

typedef struct {
    char ntp[128];
    int interval;
    int threshold;
} ntp_param_t;

typedef struct {
    int up_time;
    int total_mem;
    int free_mem;
    int total_disk;
    int free_disk;
    char model[128];
    char ver[128];
    char vsn[128];
    char sn[32];
} colt_info_t;

size_t ScreenshotCallback(void *ptr, size_t size, size_t nmemb, void *stream);
bool CreateNtpJson(char* string, const char* ntp, int interval, int threshold);
bool CreateOnoffJson(char* string, int onoff);
bool CreateRebootJson(char* string);
bool CreateBrightnessJson(char* string, int value);
bool CreateVolumeJson(char* string, int value);
bool GetFileName(const std::string& dir, std::map<std::string, std::string>& file_map);
bool JsonGetScreenBrightness(const std::string& data, int* brightness);
bool JsonGetScreenVolume(const std::string& data, int* volume);
bool JsonGetScreenOnoff(const std::string& data, bool* onoff);
bool JsonGetScreenInfo(const std::string& data, colt_info_t* info);
bool JsonGetScreenResolution(const std::string& data, colt_resolution_t* resolution);
bool JsonGetOpenParam(const std::string& data, std::string& ip);
bool CreatMethodOutput(method_t method, void* input_param, void* output_param);

#ifdef  __cplusplus
}
#endif

