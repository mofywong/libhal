#include <string.h>

#include "GpsDevice.hh"
#include "JsonParse.hh"

GpsDevice::GpsDevice() {
    devicebase = NULL;
}

GpsDevice::~GpsDevice() {
    DevClose();
    printf("GpsDevice\n");
}

//
bool GpsDevice::DevOpen(const std::string& device) {
    bool ok = false;
    std::string Dev, Model, Parity;
    int Baud, Data, Stop, Addr;
    GpsDeviceBN* deviceBN = NULL;

    if (!devicebase && (deviceBN = new GpsDeviceBN())) {
        if (GetRs485(device, Model, Addr, Dev, Baud, Data, Stop, Parity)) {
            ok = deviceBN->GpsOpen(Dev, Baud, Data, Stop, Parity);       
        }
        else
            ok = deviceBN->GpsOpen();
        if (!ok) {
            delete deviceBN;
        } else {
            devicebase = (GpsBaseClass*)deviceBN;
        }
    }
    
    return ok;
}

bool GpsDevice::Heartbeat() {
    bool ret = false;

    if (devicebase)
        ret = devicebase->Heartbeat(); 

    return ret;
}

bool GpsDevice::DevClose() {
    if (devicebase) {
        devicebase->GpsClose();
        GpsDeviceBN* acv = (GpsDeviceBN*)devicebase;
        delete acv;
        devicebase = NULL;
    }  

    return true;    
}

bool GpsDevice::GetGPSMsg(void* output) {
    bool ret = false;
    double latitude, longitude;
    string json, utc;

    if (devicebase && devicebase->GetGPSMsg(latitude, longitude, utc)) {
        GeneralGpsMessage(latitude, longitude, utc, json);
        memcpy(output, json.c_str(), json.size());
        ret = true;
    }

    printf("latitude = %.5f\n", latitude);
    return ret;    
}

bool GpsDevice::GetLatitude(void* output) {
    bool ret = false;
    double latitude = 0;
    string json;

    if (devicebase && devicebase->GetLatitude(latitude)) {
        GeneralLatitudeMessage((int)latitude, json);
        memcpy(output, json.c_str(), json.size());
        ret = true;
    }

    printf("latitude = %.5f\n", latitude);
    return ret;
}

bool GpsDevice::GetLongitude(void* output) {
    bool ret = false;
    double latitude = 0;
    string json;

    if (devicebase && devicebase->GetLongitude(latitude)) {
        GeneralLongitudeMessage((int)latitude, json);
        memcpy(output, json.c_str(), json.size());
        ret = true;
    }
    
    printf("longitude = %.5f, ret = %d\n", latitude, ret);
    return ret;
}

bool GpsDevice::GetUtc(void* output) {
    bool ret = false;
    string utc, json;

    if (devicebase && devicebase->GetUtc(utc)) {
        GeneralUtcMessage(utc, json);
        memcpy(output, json.c_str(), json.size());
        ret = true;        
    }

    printf("longitude = %s\n", utc.c_str());   
    return ret; 
}