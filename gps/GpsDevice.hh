#ifndef _GPS_DEVICE_HH_
#define _GPS_DEVICE_HH_

#include <string>
#include "GpsDeviceBN.hh"
#include "GpsBaseClass.hh"

class GpsDevice {
public:
    GpsDevice();
    ~GpsDevice();

public:
    bool DevOpen(const std::string& device);
    bool DevClose();
    bool Heartbeat();

    bool GetGPSMsg(void* output);
    bool GetLatitude(void* output);
    bool GetLongitude(void* output);
    bool GetUtc(void* output);

private:
    GpsBaseClass* devicebase;
};

#endif