#include <parson/parson.h>

#include "JsonParse.hh"

using std::string;

bool GetRs485(const string& input, string& Model, int& Addr, string& Dev, int& Baud, int& Data, int& Stop, string& Parity) {
    bool ret = false;
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    if (input.empty())
        return false;

    root = json_parse_string(input.c_str());
    if(root) {
        root_object = json_object(root);
        if (root_object) {
            const char* model = json_object_get_string(root_object, "Model");
            Model = model;
            Addr = json_object_get_number(root_object, "Addr");
            const char* dev = json_object_get_string(root_object, "Dev");
            Dev = dev;
            Baud = json_object_dotget_number(root_object, "Port.Baud");
            const char* parity = json_object_dotget_string(root_object, "Port.Parity");
            Parity = parity;
            Data = json_object_dotget_number(root_object, "Port.Data");
            Stop = json_object_dotget_number(root_object, "Port.Stop");
            ret = true;
        }       
    }
    json_value_free(root);
    return ret;    
}

bool GetRs485Model(const string& input, string& Model) {
    bool ret = false;
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    if (input.empty())
        return false;

    root = json_parse_string(input.c_str());
    if(root) {
        root_object = json_object(root);
        if (root_object) {
            const char* model = json_object_get_string(root_object, "Model");
            Model = model;
            ret = true;
        }      
    }

    json_value_free(root);
    return ret;  
}

bool GeneralGpsMessage(double& Latitude, double& Longitude, const string& Utc, string& output) {
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_value_init_object();
    root_object = json_object(root);
    if (root_object) {
        json_object_set_number(root_object, "Latitude", Latitude);
        json_object_set_number(root_object, "Longitude", Longitude);
        json_object_set_string(root_object, "Utc", Utc.c_str());
        output = json_serialize_to_string(root);
        return true;
    }

    return false;     
}

bool GeneralUtcMessage(const string& Utc, string& output) {
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_value_init_object();
    root_object = json_object(root);
    if (root_object) {
        json_object_set_string(root_object, "Utc", Utc.c_str());
        output = json_serialize_to_string(root);
        return true;
    }

    return false;    
}

bool GeneralLongitudeMessage(const int Longitude, string& output) {
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_value_init_object();
    root_object = json_object(root);
    if (root_object) {
        json_object_set_number(root_object, "Longitude", Longitude);
        output = json_serialize_to_string(root);
        return true;
    }

    return false;  
}

bool GeneralLatitudeMessage(const int Latiutude, string& output) {
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_value_init_object();
    root_object = json_object(root);
    if (root_object) {
        json_object_set_number(root_object, "Latitude", Latiutude);
        output = json_serialize_to_string(root);
        return true;
    }

    return false;  
}