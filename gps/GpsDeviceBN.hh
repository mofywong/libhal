#ifndef __GPS_DEVICEBN_HH_
#define __GPS_DEVICEBN_HH_

/*imformation: gps vendor: BN */

#include <pthread.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "nmea/nmea.h"
#include "GpsBaseClass.hh"
#include "hserial.h"

using namespace std;

class GpsDeviceBN : public GpsBaseClass {
public:
    GpsDeviceBN();
    ~GpsDeviceBN();

public:
    bool GpsOpen();
    bool GpsOpen(string& dev, int baud, int data_bit, int stop, string& parity);
    bool Heartbeat();

    bool GetGPSMsg(double& latitude, double& longitude, std::string& utc);
    bool GetLatitude(double& latitude);
    bool GetLongitude(double& longitude);
    bool GetUtc(string& Json);

    void GpsClose();

private:
    void parase(const char* buf, nmeaINFO* info);

private:
    serial_t* serial;
};

#endif
