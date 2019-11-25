#include <stdio.h>
#include <stdlib.h>

typedef void(*active_text_cb_t)(const char* url);
typedef struct screen_info_s {
    int width;
    int height;
    char url[128];
} screen_info_t;

#ifdef __cplusplus
extern "C" {
#endif
bool CreatAllProgramVsn(const char* dir, screen_info_t* info);
bool CreatIdelVsn(const char* dir, screen_info_t* info);
bool GetActiveTextUrl(const char* dir, char* url);
#ifdef __cplusplus
}
#endif
