#include <list>
#include "CreatProgramVsn.hh"
extern "C" {
    #include <parson/parson.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}
#define PROGRAM_NAME           "FutureCity.vsn"

typedef enum{
    PRO_TEXT = 0,
    PRO_OVERTEXT = 1,
    PRO_MEDIA = 2,
    PRO_UNKONW = 3,
}program_type_t;

typedef enum{
    SRC_IMAGE   = 0,
    SRC_VIDEO   = 1,
} media_type_t;

typedef enum{
    SRC_BOTTOM   = 0,
    SRC_HEAD     = 1,
    SRC_FULL     = 2,
    SRC_BOTTOM_HEAD = 3,
    SRC_UNKONW   = 4
} text_position_t;

typedef struct media_s {
    char filename[128];
    int time;
    int repeat;
    int volume;
    media_type_t type;
} media_t;

typedef struct overlay_s {
    bool _static;
    text_position_t position;
    char foreground[11];
    char background[11];
    int size;
    int time;
    char url[256];
    char* content;
} text_t;

int GetVideoTime(const char* file_name)
{
    int ret = -1;
    AVFormatContext* pFormatCtx = NULL;
   
    av_register_all();
    pFormatCtx = avformat_alloc_context();
    if (pFormatCtx) {
        if (0 == avformat_open_input(&pFormatCtx, file_name, NULL, NULL)) {
             if (avformat_find_stream_info(pFormatCtx, NULL) >= 0) {
                 ret = pFormatCtx->duration/1000000;
             }
             avformat_close_input(&pFormatCtx);
        }
        avformat_free_context(pFormatCtx);
    }
    return ret;
}

bool GetMediaNode(const char* dir, std::list<media_t>& list_media, JSON_Object *node_object)
{

    const char* type_str;
    const char* file_str;
    double value;
    char time_str[64];
    char file_path[128];
    bool ret = false;
    char* data;
    media_t media;
    
    type_str = json_object_get_string(node_object, "Type");
    if (type_str) {
        memset(&media, 0, sizeof(media_t));
        if (0 == strcmp(type_str, "Image")) {
            file_str = json_object_dotget_string(node_object, "Image.File");
            if (file_str) {
                snprintf(media.filename, sizeof(media.filename), "%s", file_str);
                value = (int)json_object_dotget_number(node_object, "Image.Time");
                media.time = value;
                media.type = SRC_IMAGE;
                media.repeat = 0;
                media.volume = 0;
                list_media.push_back(media);
                ret = true;
            }
        } else if (0 == strcmp(type_str, "Video")) {
            file_str = json_object_dotget_string(node_object, "Video.File");
            if (file_str) {
                snprintf(media.filename, sizeof(media.filename), "%s", file_str);
                value = (int)json_object_dotget_number(node_object, "Video.Repeat");
                media.repeat = value;
                value = (int)json_object_dotget_number(node_object, "Video.Volume");
                media.volume = value;
                media.type = SRC_VIDEO;
                snprintf(file_path, sizeof(file_path), "%s/%s", dir, file_str);
                media.time = GetVideoTime(file_path);
                list_media.push_back(media);
                ret = true;
            }
            
        } 
    }
    
    return ret;
}

bool ParseItemField(const char* dir, std::list<media_t>& list_media)
{
    JSON_Value *root = NULL;
    JSON_Object *object = NULL;
    JSON_Value *node_value = NULL;
    JSON_Object *node_object = NULL;
    JSON_Array *temp_array = NULL;
    JSON_Array *array = NULL;
    int size;
    int index;
    bool ret = false;
    char path[128];

    snprintf(path, sizeof(path), "%s/test.json", dir);
    root = json_parse_file(path);
    if (!root) {
       return ret;
    }
    object = json_object(root);
    array = json_object_get_array(object, "Programs");
    if (array) {
        node_value = json_array_get_value(array, 0);
        if (node_value) {
            node_object = json_object(node_value);
            temp_array = json_object_get_array(node_object, "Items");
            if (temp_array) {
                size = json_array_get_count(temp_array);
                for(index = 0; index < size; index++) {
                    node_value = json_array_get_value(temp_array, index);
                    if (node_value) {
                        node_object = json_object(node_value);
                        if (node_object) {
                            ret = GetMediaNode(dir, list_media, node_object);
                        }
                    }
                }
            }       
        }
    }
    
    json_serialize_to_file_pretty(root, path);
    json_value_free(root);
    return ret;
}

bool ParseOverlayTextField(const char* dir, text_t* text)
{
    JSON_Value *root = NULL;
    JSON_Value *node_value = NULL;
    JSON_Object *object = NULL;
    JSON_Object *node_object = NULL;
    JSON_Object *temp_object = NULL;
    JSON_Array *array = NULL;
    bool ret = false;
    char path[128];
    const char* str;
    int flag;

    snprintf(path, sizeof(path), "%s/test.json", dir);
    root = json_parse_file(path);
    if (!root) {
       return ret;
    }
    object = json_object(root);
    array = json_object_get_array(object, "Programs");
    if (array) {
        node_value = json_array_get_value(array, 0);
        if (node_value) {
            node_object = json_object(node_value);
            temp_object = json_object_get_object(node_object, "Text");
            if (temp_object) {
                str = json_object_get_string(temp_object, "Foreground");
                if (str) {
                    snprintf(text->foreground, sizeof(text->foreground), "%s", str);
                }
                str = json_object_get_string(temp_object, "Background");
                if (str) {
                    snprintf(text->background, sizeof(text->background), "%s", str);
                }
                text->size = json_object_get_number(temp_object, "Size");
                flag = json_object_get_number(temp_object, "Position");
                switch (flag)
                {
                case 1:
                    text->position = SRC_HEAD;
                    break;
                case 2:
                    text->position = SRC_BOTTOM;
                    break;
                case 3:
                    text->position = SRC_FULL;
                    break;
                default:
                    text->position = SRC_UNKONW;

                    break;
                }
                flag = json_object_get_boolean(temp_object, "Static");
                if (flag > 0) {
                    text->_static = true;
                    str = json_object_dotget_string(temp_object, "In.Content");
                    if (str) {
                        snprintf(text->content, sizeof(char)*1024*10, "%s", str);
                    }
                    ret = true;
                } else {
                    text->_static = false;
                    text->time = (int)json_object_dotget_number(temp_object, "In.Time");
                    str = json_object_dotget_string(temp_object, "In.Url");
                    if (str) {
                        snprintf(text->url, sizeof(text->url), "%s", str);
                        ret = true;
                    }
                }
            }       
        }
    }
    json_serialize_to_file_pretty(root, path);
    json_value_free(root);
    return ret;
}


program_type_t GetProgramType(const char* dir)
{
    JSON_Value *root = NULL;
    JSON_Object *object = NULL;
    JSON_Object *node_object = NULL;
    JSON_Value *temp_value = NULL;
    JSON_Value *node_value = NULL;
    JSON_Array *array = NULL;
    program_type_t ret = PRO_UNKONW;
    char path[128];
    int type;

    snprintf(path, sizeof(path), "%s/test.json", dir);
    root = json_parse_file(path);
    if (!root) {
       return ret;
    }
    object = json_object(root);
    array = json_object_get_array(object, "Programs");
    if (array) {
        node_value = json_array_get_value(array, 0);
        if (node_value) {
            node_object = json_object(node_value);
            if (node_object) {
                type = json_object_get_number(node_object, "Type");
                switch (type)
                {
                case 1:
                    ret = PRO_TEXT;
                    break;
                case 2:
                    ret = PRO_MEDIA;
                    break;
                case 3:
                    ret = PRO_OVERTEXT;
                    break;
                default:
                    ret = PRO_MEDIA;
                    break;
                }
                
            }
        }
    }

end:  
    json_serialize_to_file_pretty(root, path);
    json_value_free(root);
    return ret;
}

static bool InitMediaArrayNode(JSON_Array *array, media_t* media)
{
    JSON_Value *temp_value;
    JSON_Object *temp_object;    
    const char* type_str;
    const char* file_str;
    double value;
    char time_str[64];
    char file_path[256];
    bool ret = false;
    char* data;
    
    temp_value = json_value_init_object(); 
    if (temp_value) {
        temp_object = json_object(temp_value);
        if (temp_object) {
            if (SRC_IMAGE == media->type) {
                json_object_set_string(temp_object, "Type", "2");
                snprintf(time_str, sizeof(time_str), "%d", 1000*media->time);
                json_object_set_string(temp_object, "Duration", time_str); 
                json_object_dotset_string(temp_object, "inEffect.Type", "2");
                json_object_dotset_string(temp_object, "inEffect.Time", "1500");
            }
            if (SRC_VIDEO == media->type) {
                snprintf(time_str, sizeof(time_str), "%d", 1000*media->time);
                json_object_set_string(temp_object, "Duration", time_str); 
                json_object_set_string(temp_object, "Type", "3");
                json_object_set_string(temp_object, "Volume", "1.0");
                json_object_set_string(temp_object, "ReserveAS", "0");
            }
            json_object_dotset_string(temp_object, "FileSource.IsRelative", "1");
            snprintf(file_path, sizeof(file_path), ".\\FutureCity.files\\%s", (const char*)media->filename);
            json_object_dotset_string(temp_object, "FileSource.FilePath", file_path);
        }
    }

    json_array_append_value(array, temp_value);
    ret = true;
    
end:
   // json_value_free(temp_value);
    return ret;
}

bool CreatMediaItemsArray(JSON_Array* items, std::list<media_t>& list_media)
{
    media_t media;
    bool ret = false;
    
    for(auto it=list_media.begin();it!=list_media.end();it++) {
        media = *it;
        ret = InitMediaArrayNode(items, &media);
    }
        
    return ret;
}

bool CreatMediaRegionsObject(JSON_Object* media_object,std::list<media_t>& list_media, screen_info_t* info)
{
    JSON_Value* array_value;
    JSON_Array* items;
    char data[10];
    bool ret = false;
   
    json_object_set_string(media_object, "Name", "sync_program");
    json_object_set_string(media_object, "type", "1");
    json_object_set_number(media_object, "Layer", 1);
    json_object_dotset_string(media_object, "Rect.X", "0");
    json_object_dotset_string(media_object, "Rect.Y", "0");
    snprintf(data, sizeof(data), "%d", info->width);
    json_object_dotset_string(media_object, "Rect.Width", data);
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "%d", info->height);
    json_object_dotset_string(media_object, "Rect.Height", data);
    json_object_dotset_string(media_object, "Rect.BorderWidth", "0");
    json_object_dotset_string(media_object, "Rect.BorderColor", "0xFFFFFF00");

    array_value = json_value_init_array();
    if (array_value) {
        items = json_array(array_value);
        if (items) {
            if (CreatMediaItemsArray(items, list_media)) {
                json_object_set_value(media_object, "Items", array_value);
                ret = true;
            }
        }
    }
    return ret;
}

bool CreatActiveTextItemsArray(JSON_Array* items, const char* url, text_t* active_text) 
{
    JSON_Value *temp_value;
    JSON_Object *temp_object; 
    bool ret = false;
    char string[512];
    char text_size[4];
   
    temp_value = json_value_init_object(); 
    if (temp_value) {
        temp_object = json_object(temp_value);
        if (temp_object) {
            if (active_text->position == SRC_FULL) {
                json_object_set_string(temp_object, "Name", "active text");
                json_object_set_string(temp_object, "Type", "5");

            } else {
                json_object_set_string(temp_object, "Name", "Single line text.");
                json_object_set_string(temp_object, "Type", "4");
            }
            
            json_object_set_string(temp_object, "BackColor", active_text->background);
            json_object_set_string(temp_object, "Alpha", "1.000000");
            json_object_set_string(temp_object, "BeGlaring", "0");
            json_object_set_string(temp_object, "TextColor", active_text->foreground);
            snprintf(text_size, sizeof(text_size), "%d", active_text->size);
            json_object_dotset_string(temp_object, "LogFont.lfHeight", text_size);
            json_object_dotset_string(temp_object, "LogFont.lfWeight", "0");
            json_object_dotset_string(temp_object, "LogFont.lfItalic", "0");
            json_object_dotset_string(temp_object, "LogFont.lfUnderline", "0");
            json_object_dotset_string(temp_object, "LogFont.lfStrikeOut", "0");
            json_object_dotset_string(temp_object, "LogFont.lfFaceName", "System");
            if (active_text->_static) {
                json_object_set_string(temp_object, "IsScroll", "0");
                json_object_set_string(temp_object, "Text", active_text->content);
            } else {
                if (active_text->position == SRC_FULL) {
                    json_object_set_string(temp_object, "IsScroll", "0");

                } else {
                    json_object_set_string(temp_object, "IsScroll", "1");
                    json_object_set_string(temp_object, "Speed", "40.000000");
                }
                json_object_dotset_string(temp_object, "MultiPicInfo.OnePicDuration", "6000");
                json_object_set_string(temp_object, "IsNeedUpdate", "1");
                json_object_set_string(temp_object, "RepeatCount", "1");
                json_object_set_string(temp_object, "IsHeadConnectTail", "1");
                json_object_set_string(temp_object, "PlayLenth", "300000");
                memset(string, 0, sizeof(string));
                snprintf(string, sizeof(string), "%d", active_text->time*1000);
                json_object_set_string(temp_object, "UpdateInterval", string);
                memset(string, 0, sizeof(string));
                snprintf(string, sizeof(string), "CLT_JSON{\"url\":\"%s\",\"filter\":\"$.Text.Content\"}CLT_JSON", active_text->url);
                json_object_set_string(temp_object, "Text", string);
            }
            
            json_array_append_value(items, temp_value);
            ret = true;
        }
    }
    return ret;
}

bool CreatProgramVsn(screen_info_t* info, const char* dir, JSON_Value *regions)
{
    JSON_Value *temp_value;
    JSON_Object *temp_object;
    JSON_Value *temp_value1;
    JSON_Array *temp_array;
    JSON_Value *temp_pages;
    JSON_Array *pages_array;
    
    bool ret = false;
    char data[8];
    char name[128];
    
    temp_value1 = json_value_init_object(); 
    if (temp_value1) {
        temp_object = json_object(temp_value1);
        if (temp_object) {
            json_object_set_string(temp_object, "Name", "program 1");
            json_object_set_string(temp_object, "AppointDuration", "3600000");
            json_object_set_string(temp_object, "LoopType", "1");
            json_object_set_value(temp_object, "Regions", regions);
        } else {
            goto end;
        } 
    }else {
        goto end;
    }
    
    temp_pages = json_value_init_array();
    if (temp_pages) {
        pages_array = json_array(temp_pages);
        json_array_append_value(pages_array, temp_value1);
        temp_value = json_value_init_object(); 
        if (temp_value) {
            temp_object = json_object(temp_value);
            if (temp_object) {
                memset(data, 0, sizeof(data));
                snprintf(data, sizeof(data), "%d", info->width);
                json_object_dotset_string(temp_object, "Programs.Program.Information.Width", data);
                memset(data, 0, sizeof(data));
                snprintf(data, sizeof(data), "%d", info->height);
                json_object_dotset_string(temp_object, "Programs.Program.Information.Height", data);
                json_object_dotset_value(temp_object, "Programs.Program.Pages", temp_pages);
            } else {
                    goto end;
            }
        } else {
            goto end;
        }
    }else {
        goto end;
    }
    
    snprintf(name, 128, "%s/%s", dir, PROGRAM_NAME);
    json_serialize_to_file_pretty(temp_value, name);
    ret = true;
  
end:
    if (temp_value) {
       json_value_free(temp_value);
    }
    return ret;
}

bool CreatTextItemsArray(JSON_Array* items, text_t* text) 
{
    JSON_Value *temp_value;
    JSON_Object *temp_object; 
    bool ret = false;
    char text_size[4];
   
    temp_value = json_value_init_object(); 
    if (temp_value) {
        temp_object = json_object(temp_value);
        if (temp_object) {
            json_object_set_string(temp_object, "Name", "Text");
            json_object_set_string(temp_object, "BackColor", text->background);
            json_object_set_string(temp_object, "Alpha", "1.000000");
            json_object_set_string(temp_object, "BeGlaring", "0");
            //json_object_dotset_string(temp_object, "MultiPicInfo.OnePicDuration", "6000");
            json_object_set_string(temp_object, "TextColor", text->foreground);
            snprintf(text_size, sizeof(text_size), "%d", text->size);
            json_object_dotset_string(temp_object, "LogFont.lfHeight", text_size);
            json_object_dotset_string(temp_object, "LogFont.lfWeight", "0");
            json_object_dotset_string(temp_object, "LogFont.lfItalic", "0");
            json_object_dotset_string(temp_object, "LogFont.lfUnderline", "0");
            json_object_dotset_string(temp_object, "LogFont.lfStrikeOut", "0");
            json_object_dotset_string(temp_object, "LogFont.lfFaceName", "System");
            if (text->position == SRC_FULL) {
                json_object_set_string(temp_object, "IsScroll", "0");
                json_object_set_string(temp_object, "Type", "5");
            } else {
                json_object_set_string(temp_object, "IsScroll", "1");
                json_object_set_string(temp_object, "Type", "4");
                json_object_set_string(temp_object, "Speed", "40.000000");
            }
            json_object_dotset_string(temp_object, "MultiPicInfo.OnePicDuration", "6000");
            //json_object_set_string(temp_object, "IsNeedUpdate", "1");
            json_object_set_string(temp_object, "RepeatCount", "1");
            json_object_set_string(temp_object, "IsHeadConnectTail", "1");
            json_object_set_string(temp_object, "PlayLenth", "300000");
           // json_object_set_string(temp_object, "UpdateInterval", "10000");
            //printf("len:%d, text:%s\n", strlen(text->content), text->content);
            json_object_set_string(temp_object, "Text", text->content);
          
            json_array_append_value(items, temp_value);
            ret = true;
        }
    }
    return ret;
}

bool CreatTextRegionsObject(JSON_Object* text_object, text_t* text, screen_info_t* info)
{
    JSON_Value* array_value;
    JSON_Array* items;
    char x[10];
    char y[10];
    char width[10];
    char height[10];
    bool ret = false;
    json_object_set_string(text_object, "Layer", "1");
    switch (text->position)
    {
    case SRC_HEAD:
        snprintf(x, sizeof(x), "%d", 0);
        snprintf(y, sizeof(y), "%d", 0);
        snprintf(width, sizeof(width), "%d", info->width);
        snprintf(height, sizeof(height), "%d", text->size+8);
        break;
    case SRC_BOTTOM:
        snprintf(x, sizeof(x), "%d", info->width);
        snprintf(y, sizeof(y), "%d", info->height-text->size-8);
        snprintf(width, sizeof(width), "%d", info->width);
        snprintf(height, sizeof(height), "%d", text->size+8);
        break;
    case SRC_FULL:
        snprintf(x, sizeof(x), "%d", 0);
        snprintf(y, sizeof(y), "%d", 0);
        snprintf(width, sizeof(width), "%d", info->width);
        snprintf(height, sizeof(height), "%d", info->height);
        break;
    default:
        break;
    }
   
    json_object_dotset_string(text_object, "Rect.X", x);
    json_object_dotset_string(text_object, "Rect.Y", y);
    json_object_dotset_string(text_object, "Rect.Width", width);
    json_object_dotset_string(text_object, "Rect.Height", height);
    json_object_dotset_string(text_object, "Rect.BorderWidth", "0");
    json_object_dotset_string(text_object, "Rect.BorderColor", "0xFFFFFF00");
    array_value = json_value_init_array();
    if (array_value) {
        items = json_array(array_value);
        if (items) {
            if (text->_static) {
                ret = CreatTextItemsArray(items, text);
            } else {
                ret = CreatActiveTextItemsArray(items, info->url, text);
            }
            if (ret) {
                json_object_set_value(text_object, "Items", array_value);
            }
        }
    }
    return ret;
}

bool CreatTextProgramVsn(const char* dir, text_t* text, screen_info_t* info)
{
    JSON_Value* text_value;
    JSON_Object* text_object;
    JSON_Value* regions_value;
    JSON_Array* regions_array;
    bool ret = false;
   
    text_value = json_value_init_object();
    if (text_value) {
        text_object = json_object(text_value);
        if (CreatTextRegionsObject(text_object, text, info)) {
            regions_value = json_value_init_array();
            if (regions_value) {
                regions_array = json_array(regions_value);
                json_array_append_value(regions_array, text_value);
                ret = CreatProgramVsn(info, dir, regions_value);
            } 
        }
    }
    
    return ret;
}

bool CreatOverlayTextProgramVsn(std::list<media_t>& list_media, text_t* text, screen_info_t* info, const char* dir)
{
    JSON_Value* media_value;
    JSON_Object* media_object;
    JSON_Value* active_value;
    JSON_Value* active_value1;
    JSON_Object* active_object;
    JSON_Object* active_object1;
    JSON_Value* regions_value;
    JSON_Array* regions_array;
    bool ret = false;
    
    regions_value = json_value_init_array();
    if (!regions_value) {
        return false;
    } 
    regions_array = json_array(regions_value);
    media_value = json_value_init_object();
    if (media_value) {
        media_object = json_object(media_value);
        if (CreatMediaRegionsObject(media_object, list_media, info)) {
            json_array_append_value(regions_array, media_value);
            active_value = json_value_init_object();
            if (active_value) {
                active_object = json_object(active_value);
                if (text->position == SRC_BOTTOM_HEAD) {
                    text->position = SRC_HEAD;
                    snprintf(text->url, sizeof(text->url), "%s/header.json", info->url);
                    if (CreatTextRegionsObject(active_object, text, info)) {
                        json_array_append_value(regions_array, active_value);
                        text->position = SRC_BOTTOM;
                        memset(text->url, 0, sizeof(text->url));
                        snprintf(text->url, sizeof(text->url), "%s/footer.json", info->url);
                        active_value1 = json_value_init_object();
                        if (active_value1) {
                            active_object1 = json_object(active_value1);
                            if (CreatTextRegionsObject(active_object1, text, info)) {
                                json_array_append_value(regions_array, active_value1);
                            }
                        }
                    }
                } else {
                    CreatTextRegionsObject(active_object, text, info);
                    json_array_append_value(regions_array, active_value);
                }
                ret = CreatProgramVsn(info, dir, regions_value);
            }
        }
    }
    
    return ret; 
}


bool CreatItemProgramVsn(std::list<media_t>& list_media, screen_info_t* info, const char* dir)
{
    JSON_Value* media_value;
    JSON_Object* media_object;
    JSON_Value* regions_value;
    JSON_Array* regions_array;
    bool ret = false;
   
    media_value = json_value_init_object();
    if (media_value) {
        media_object = json_object(media_value);
        if (CreatMediaRegionsObject(media_object, list_media, info)) {
            regions_value = json_value_init_array();
            if (regions_value) {
                regions_array = json_array(regions_value);
                json_array_append_value(regions_array, media_value);
                
                ret = CreatProgramVsn(info, dir, regions_value);
            } 
        }
    }
    
    return ret;
}

bool CreatAllProgramVsn(const char* dir, screen_info_t* info)
{
    text_t* text;
    program_type_t type;
    bool ret = false;
    
    printf("CreatAllProgramVsn:%s\n",dir);
    type = GetProgramType(dir);
    switch (type)
    {
        case PRO_TEXT:
            printf("PRO_TEXT\n");
            text = new text_t;
            if (text) {
                text->content = new char[10240];
                if(text->content) {
                    memset(text->content, 0, sizeof(char)*1024*10);
                    if (ParseOverlayTextField(dir, text)){
                        ret = CreatTextProgramVsn(dir, text, info);
                    }
                   delete(text->content); 
                }
                delete(text); 
            }
            break;
        case PRO_OVERTEXT:
            printf("PRO_OVERTEXT\n");
            text = new text_t;
            if (text) {
                memset(text, 0, sizeof(text_t));
                text->content = new char[10240];
                if(text->content) {
                    memset(text->content, 0, sizeof(char)*1024*10);
                    std::list<media_t> list_media;
                    if (ParseItemField(dir,list_media)) {
                        if (ParseOverlayTextField(dir, text)) {
                            ret = CreatOverlayTextProgramVsn(list_media, text, info, dir);
                        }
                    }
                    delete(text->content); 
                }
                delete(text); 
            }
            break;
        case PRO_MEDIA:
            printf("PRO_MEDIA\n");
            text = new text_t;
            if (text) {
                text->_static = false;
                text->position = SRC_BOTTOM_HEAD;
                text->size = 16;
                text->time = 10;
                snprintf(text->foreground, sizeof(text->foreground), "0xFFFF0000");
                snprintf(text->background, sizeof(text->background), "0x00000000");
                snprintf(text->url, sizeof(text->url), "%s", info->url);
                std::list<media_t> list_media;
                if (ParseItemField(dir, list_media)) {
                    ret = CreatOverlayTextProgramVsn(list_media, text, info, dir);
                }
                delete(text);
            }
            break;
        default:
            printf("Error Type\n");
            break;
    }
    return ret;      
}

bool CreatIdelVsn(const char* dir, screen_info_t* info)
{
    text_t* media_text;
    bool ret = false;
    media_text = new text_t;
    if (media_text) {
        memset(media_text, 0, sizeof(text_t));
        media_text->size = 16;
        media_text->_static = true;
        snprintf(media_text->background, sizeof(media_text->background), "0x00000000");
        snprintf(media_text->foreground, sizeof(media_text->foreground), "0x00000000");
        ret = CreatTextProgramVsn(dir, media_text, info);
        delete(media_text); 
    }
    return ret;
}



