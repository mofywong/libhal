#include <sys/stat.h>
#include <list>
#include <string.h>
#include "InfoScreenColt.hh"
#include "CreatProgramVsn.hh"
#include "ColtCommand.hh"

#define TYPE_JSON      "Content-Type: application/json; charset=utf-8"
#define TYPE_FROM      "Content-Type: multipart/form-data"
#define PROGRAM_NAME   "FutureCity.vsn"

typedef enum {
    METHOD_POST = 0,
    METHOD_PUT 
} method_type_t;

InfoScreenColt::InfoScreenColt(void)
{

}

InfoScreenColt:: ~InfoScreenColt(void)
{

}

int InfoScreenColt::DevOpen(const std::string& device, int flags)
{
    std::string Ip = "";
    int ret = ConnectFailure;
    bool onoff = true;
    char output[256];
    printf("###ip:%s\n",device.c_str());
    if (!JsonGetOpenParam(device, Ip)) {
        return InputParamErr;
    }
    scr = LedScreenColtNew(Ip);
    if (scr) {
        if (Success == LedScreenColtGetResolution(output)) {
            scr->flags = flags;
            ret = Success;
        }
    }

    return ret;
}

int InfoScreenColt::DevClose(void)
{
    LedScreenColtFree();
    return Success;
}

bool InfoScreenColt::HttpGetOperation(CURL *handle, const std::string& url, std::string& data)
{
    CURLcode res;
    char buf[512];
    memset(buf, 0, sizeof(buf));
    curl_easy_reset(handle);
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, buf);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &HttpGetCallback);
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(handle);
    data = buf;
    return (res == CURLE_OK) ? true : false;
}

bool InfoScreenColt::HttpJsonPostOperation(CURL *handle, const std::string& url, const std::string& data)
{
    CURLcode res;
    struct curl_slist* header_list = NULL;

    header_list = curl_slist_append(header_list, TYPE_JSON);
    if (header_list) {
        curl_easy_reset(handle);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 3);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(handle, CURLOPT_POST, 1);
        res = curl_easy_perform(handle);
        curl_slist_free_all(header_list);

        return (res == CURLE_OK) ? true : false;
    }
    return false;
}

bool InfoScreenColt::HttpJsonFileOperation(CURL *handle, const std::string& url, std::map<std::string, std::string>& file_map)
{
    struct curl_slist *headerlist = NULL;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    CURLcode res;
    std::string key = "";
    std::string value = "";

    headerlist = curl_slist_append(headerlist, TYPE_FROM);
    if (headerlist) {
        for (auto it = file_map.begin(); it != file_map.end(); it++) {
            key = it->first;
            value = it->second;
           // printf("key:%s, value:%s\n", key.c_str(), value.c_str());
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, key.c_str(),
                            CURLFORM_FILE, value.c_str(), CURLFORM_END);
        }
        curl_easy_reset(handle);
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(handle, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
        res = curl_easy_perform(handle);
        curl_formfree(formpost);
        curl_slist_free_all(headerlist);
        return (res == CURLE_OK) ? true : false;
    }
    return false;
}

bool InfoScreenColt::HttpJsonPutOperation(CURL *handle, const std::string& url, const std::string& data)
{
    CURLcode res;
    struct curl_slist* header_list = NULL;

    header_list = curl_slist_append(header_list, TYPE_JSON);
    if (header_list) {
        curl_easy_reset(handle);
        if (!data.empty()) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
        }
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 3);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
        res = curl_easy_perform(handle);
        curl_slist_free_all(header_list);

        return (res == CURLE_OK) ? true : false;
    }
    return false;
}

bool InfoScreenColt::HttpJsonDeleteOperation(CURL *handle, const std::string& url)
{
    CURLcode res;
    struct curl_slist* header_list = NULL;

    header_list = curl_slist_append(header_list, TYPE_JSON);
    if (header_list) {
        curl_easy_reset(handle);
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 3);
        res = curl_easy_perform(handle);
        curl_slist_free_all(header_list);
        return (res == CURLE_OK) ? true : false;
    }

    return false;
}

led_screen_colt_t* InfoScreenColt::LedScreenColtNew(const std::string& ip)
{
    led_screen_colt_t* handle;

    if(ip.empty()) {
        return NULL;  
    }
    handle = new led_screen_colt_t;
    if(handle) {
        curl_global_init(CURL_GLOBAL_ALL); 
        handle->curl = curl_easy_init();
        if (handle->curl) {
            snprintf(handle->ip, sizeof(handle->ip), "%s", ip.c_str());
            return handle;
        }
        delete handle;
    }
    return NULL;
}

int InfoScreenColt::LedScreenColtGetResolution(void* output)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;
    colt_resolution_t resolution;
    if(scr) {
        snprintf(url, sizeof(url), "http://%s/api/dimension.json", scr->ip);
        if (HttpGetOperation(scr->curl, url, data)) {
            if (JsonGetScreenResolution(data, &resolution)) {
                scr->width = resolution.width;
                scr->height = resolution.height;
                CreatMethodOutput(SCR_GET_RESOILUTION, &resolution, output);
                printf("output:%s\n",(const char*)output);
                ret = Success;
            }
        }
    }

    return ret; 
}

int InfoScreenColt::LedScreenColtGetInfo(void* output)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;
    colt_info_t info;

    if (scr) {
        snprintf(url, sizeof(url), "http://%s/api/info.json", scr->ip);
        if(HttpGetOperation(scr->curl, url, data)) {
            if (JsonGetScreenInfo(data, &info)) {
                CreatMethodOutput(SCR_GET_INFO, &info, output);
                ret = Success;
            }
        }
    }
    
    return ret;
}

int InfoScreenColt::LedScreenColtSetOnoff(int onoff)
{
    char url[128];
    char json_str[64];
    int ret = AccessFailure;

    if (!scr) {
        return NotReady;
    }
    printf("onoff:%d\n",onoff);
    if (CreateOnoffJson(json_str, onoff)) {
        printf("json_str:%s\n",json_str);
        snprintf(url, sizeof(url), "http://%s/api/action", scr->ip);
        if (HttpJsonPostOperation(scr->curl, url, json_str)) {
            ret = Success;
        }
    }
    
    return ret;
}

int InfoScreenColt::LedScreenColtGetOnoff(void* output)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;
    bool onoff;

    if (!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/powerstatus.json", scr->ip);
    if(HttpGetOperation(scr->curl, url, data)) {
        if (JsonGetScreenOnoff(data, &onoff)) {
            CreatMethodOutput(SCR_GET_ONOFF, &onoff, output);
            ret = Success;
        }
    }
    return ret;
}

int InfoScreenColt::LedScreenColtSetVolume(int volume)
{
    char url[128];
    char json_str[64];
    int ret = AccessFailure;

    if (!scr) {
        return NotReady;
    }

    if (CreateVolumeJson(json_str, volume)) {
        snprintf(url, sizeof(url), "http://%s/api/volume", scr->ip);
        if (HttpJsonPutOperation(scr->curl, url, json_str)) {
            ret = Success; 
        }
    }
    return ret;
}

int InfoScreenColt::LedScreenColtGetVolume(void* output)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;
    int volume;

    if (!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/volume.json", scr->ip);
    if(HttpGetOperation(scr->curl, url, data)) {
        if (JsonGetScreenVolume(data, &volume)) {
            CreatMethodOutput(SCR_GET_VOLUME, &volume, output);
            ret = Success; 
        }
    }
    
    return ret;
}

int InfoScreenColt::LedScreenColtSetBrightness(int brightness)
{
    char url[128];
    char json_str[64];
    int value;
    int ret = AccessFailure;

    if (!scr) {
        return NotReady;
    }
    value = brightness*255/100;
    if (CreateBrightnessJson(json_str, value)) {
        snprintf(url, sizeof(url), "http://%s/api/brightness", scr->ip);
        if (HttpJsonPutOperation(scr->curl, url, json_str)) {
            memset(url, 0, sizeof(url));
            snprintf(url, sizeof(url), "http://%s/api/savebrightnessandcolortemp", scr->ip);
            ret = Success; 
        }
    }
    return ret;
}

int InfoScreenColt::LedScreenColtGetBrightness(void* output)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;
    int brightness;
    int value;

    if (!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/brightnessandcolortemp.json", scr->ip);
    if(HttpGetOperation(scr->curl, url, data)) {
        if(JsonGetScreenBrightness(data, &brightness)) {
            value = brightness*100/255;
            CreatMethodOutput(SCR_GET_BRIGHTNESS, &value, output);
            ret = Success; 
        }
    }

    return ret;
}

bool ScreenshotOperation(led_screen_colt_t* scr, FILE* write_file,
curl_off_t local_file_len, int32_t use_resume, double* filesize)
{   
    CURLcode result;
    char url[128];
    
    snprintf(url, sizeof(url), "http://%s/api/screenshot", scr->ip);
    curl_easy_reset(scr->curl);
    curl_easy_setopt(scr->curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(scr->curl, CURLOPT_URL, url);
    curl_easy_setopt(scr->curl, CURLOPT_CONNECTTIMEOUT, 3);
    /*设置文件续传的位置给libcurl*/ 
    curl_easy_setopt(scr->curl, CURLOPT_RESUME_FROM_LARGE, use_resume ? local_file_len : 0);
    curl_easy_setopt(scr->curl, CURLOPT_WRITEDATA, write_file);
    curl_easy_setopt(scr->curl, CURLOPT_WRITEFUNCTION, &ScreenshotCallback);
    curl_easy_setopt(scr->curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(scr->curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(scr->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(scr->curl, CURLOPT_SSL_VERIFYHOST, 0L);
    result = curl_easy_perform(scr->curl);
    return (result == CURLE_OK) ? true : false;
}

int InfoScreenColt::LedScreenColtScreenshot(const std::string& file) 
{
    FILE* write_file;
    struct stat file_info;
    curl_off_t local_file_len;
    int32_t use_resume = 0;
    double  filesize = 0;
    char temp_file[288];
    int ret = AccessFailure;

    if (!scr) {
        return NotReady;
    }
    snprintf(temp_file, sizeof(temp_file), "%s", file.c_str());
    remove(file.c_str());
    if (stat(temp_file, &file_info) == 0) {
        local_file_len = file_info.st_size;
        use_resume = 1;
    }
    write_file = fopen(temp_file, "ab+");
    if (write_file) {   
        if (ScreenshotOperation(scr, write_file, local_file_len, use_resume, &filesize)) {
            ret = Success; 
        }
        fclose(write_file);
    }
    return ret;
}

int InfoScreenColt::LedScreenColtReboot(void)
{
    char url[128];
    char json_str[64];
    int ret = AccessFailure;

    if (!scr) {
        return NotReady;
    }
    if (CreateRebootJson(json_str)) {
        snprintf(url, sizeof(url), "http://%s/api/action", scr->ip);
        if (HttpJsonPostOperation(scr->curl, url, json_str)) {
            ret = Success; 
        }
    }

    return ret;
}

int InfoScreenColt::LedScreenColtIdle(void)
{
    int ret = AccessFailure;
    if (Success == LedScreenColtScreenDeleteProgram(PROGRAM_NAME)) {
        ret = LedScreenColtSetOnoff(0);
    }
    return ret;
}

int InfoScreenColt::LedScreenColtLoadProgram(const std::string& dir_path)
{
    struct curl_slist *headerlist = NULL;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    std::map<std::string, std::string> file_map;
    screen_info_t info;
    CURLcode res;
    char url[128];
    int ret = AccessFailure;

    if(!scr) {
        return NotReady;
    }
    info.width = scr->width;
    info.height = scr->height;
    snprintf(info.url,sizeof(info.url),"http://%s:%d",scr->ip,9001);
    if (CreatAllProgramVsn(dir_path.c_str(), &info)) {
        snprintf(url, sizeof(url), "http://%s/api/program/%s", scr->ip,PROGRAM_NAME);
        if(GetFileName(dir_path, file_map)) {
            if(HttpJsonFileOperation(scr->curl, url, file_map)) {
                LedScreenColtSetOnoff(1);
                ret = Success; 
            }
        }
    }
    

    return ret;
}

int InfoScreenColt::LedScreenColtScreenWitchProgram(const std::string& name)
{
    char url[128];
    struct curl_slist* header_list = NULL;
    int ret = AccessFailure;

    if(!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/vsns/sources/lan/vsns/%s/activated", scr->ip, name.c_str());
    if (HttpJsonPutOperation(scr->curl, url, NULL)) {
        ret = Success; 
    }

    return ret;
}

int InfoScreenColt::LedScreenColtScreenSetNtp(const std::string& ntp)
{
    char url[128];
    char json_str[128];
    int ret = AccessFailure;

    if(!scr) {
        return NotReady;
    }
    if (CreateNtpJson(json_str, ntp.c_str(), 3000, 5000)) {
        snprintf(url, sizeof(url), "http://%s/api/ntp", scr->ip);
        if (HttpJsonPutOperation(scr->curl, url, json_str)) {
            ret = Success;
        }
    }
    return ret;
}

int InfoScreenColt::LedScreenColtScreenGetNtp(void)
{
    char url[128];
    std::string data;
    int ret = AccessFailure;

    if(!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/ntp.json", scr->ip);
    if (HttpGetOperation(scr->curl, url, data)) {
        ret = Success;
    }

    return ret;
}

int InfoScreenColt::LedScreenColtScreenDeleteProgram(const std::string& name)
{
    char url[128];
    int ret = AccessFailure;

    if(!scr) {
        return NotReady;
    }
    snprintf(url, sizeof(url), "http://%s/api/vsns/sources/lan/vsns/%s", scr->ip, name.c_str());
    if (HttpJsonDeleteOperation(scr->curl, url)) {
        ret = Success;
    }

    return ret;
}

void InfoScreenColt::LedScreenColtFree(void)
{
    if(scr) {
        curl_easy_cleanup(scr->curl);
        delete(scr);
    }
}