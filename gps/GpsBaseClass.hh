#ifndef _GPS_BASE_CLASS_HH_
#define _GPS_BASE_CLASS_HH_

#include <string>

typedef enum {
    BN,
    UNKONW,
} GpsType; //distinguish vendor

class GpsBaseClass {
public:
    GpsBaseClass() {
        gtype = BN;
    };
    GpsBaseClass(GpsType type) {
        gtype = type;
    };
    ~GpsBaseClass() {
        printf("~GpsBaseClas\n");
    };

public:
    virtual bool GpsOpen() = 0; //must have default init fun
    virtual bool GpsOpen(std::string& dev, int baud, int data_bit, int stop, char parity) {};
    virtual bool Heartbeat() =0;

    virtual bool GetGPSMsg(double& latitude, double& longitude, std::string& utc) = 0;
    virtual bool GetLatitude(double& latitude) = 0;
    virtual bool GetLongitude(double& longitude) = 0;
    virtual bool GetUtc(std::string& Json) = 0;// Gps method

    virtual void GpsClose() = 0; //default release

private:
    GpsType GetGpsType() {
        return gtype;
    };

private:
    GpsType gtype;
};

#endif