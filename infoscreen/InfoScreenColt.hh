#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
#include <map>

using namespace std;  

typedef struct led_screen_colt_s {
    CURL *curl;
    char ip[24];
    int flags;
    int width;
    int height;
} led_screen_colt_t;

class InfoScreenColt
{
public:
    InfoScreenColt(void);
    virtual ~InfoScreenColt(void);

public:
    int DevOpen(const std::string& device, int flags);
    int DevClose(void);
    led_screen_colt_t* LedScreenColtNew(const std::string& ip);
    int LedScreenColtGetResolution(void* output);
    int LedScreenColtGetInfo(void* output);
    int LedScreenColtSetOnoff(int onoff);
    int LedScreenColtGetOnoff(void* output);
    int LedScreenColtSetVolume(int volume);
    int LedScreenColtGetVolume(void* output);
    int LedScreenColtSetBrightness(int brightness);
    int LedScreenColtGetBrightness(void* output);
    int LedScreenColtScreenshot(const std::string& file);
    int LedScreenColtReboot(void);
    int LedScreenColtIdle(void);
    int LedScreenColtLoadProgram(const std::string& dir_path);
    int LedScreenColtScreenWitchProgram(const std::string& name);
    int LedScreenColtScreenSetNtp(const std::string& ntp);
    int LedScreenColtScreenGetNtp(void);
    int LedScreenColtScreenDeleteProgram(const std::string& name);
    void LedScreenColtFree(void);
    bool HttpJsonDeleteOperation(CURL *handle, const std::string& url);
    bool HttpJsonPutOperation(CURL *handle, const std::string& url, const std::string& data);
    bool HttpJsonFileOperation(CURL *handle, const std::string& url, std::map<std::string, std::string>& file_map);
    bool HttpJsonPostOperation(CURL *handle, const std::string& url, const std::string& data);
    bool HttpGetOperation(CURL *handle, const std::string& url, std::string& data);
private:
    led_screen_colt_t* scr;
}; 

