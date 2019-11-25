 #include "GpsDeviceBN.hh" 

GpsDeviceBN::GpsDeviceBN(): GpsBaseClass() {
    serial = NULL;
}

GpsDeviceBN::~GpsDeviceBN() {
    this->GpsClose();
    printf("GpsDeviceBN\n");
}

bool GpsDeviceBN::GpsOpen() {
    string dev; 
    char parity;
    bool ok = false;
    int baud, data_bit, stop;

    dev = "/dev/ttyUSB0";
    baud = 9600;
    data_bit = 8;
    stop = 1;
    parity = 'N';
    if (!serial && (serial = serial_open(dev.c_str(), baud, data_bit, stop, parity)))
        ok = true;
         
    return ok;
}

bool GpsDeviceBN::GpsOpen(string& dev, int baud, int data_bit, int stop, string& parity) {
    bool ok = false;

    if(!serial && (serial = serial_open(dev.c_str(), baud, data_bit, stop, parity[0])))
        ok = true;

    return ok;
}

bool GpsDeviceBN::Heartbeat() {
    int repeat_time = 3;
    bool is_ok = false;
    char buf[200] = {0};
    int32_t ret = 0, us = 3000 * 1000;     

    while(repeat_time --) {
        ret = serial_read(serial, buf, sizeof(buf), us);
        if (ret > 0) {
            is_ok = true;
            break;
        }

    }

    printf("exit\n");
    return is_ok;
}

bool GpsDeviceBN::GetGPSMsg(double& latitude, double& longitude, std::string& utc) {
    nmeaINFO info;
    nmeaTIME Utc;
    int repeat_time = 3;
    bool is_ok = false;
    char buf[200] = {0};
    int32_t ret = 0, us = 3000 * 1000;     

    nmea_zero_INFO(&info);

    while(repeat_time --) {
        ret = serial_read(serial, buf, sizeof(buf), us);
        if (ret > 0) {
             parase(buf, &info);
             if (info.sig > 0) {
                latitude = info.lat;
                longitude = info.lon;
                Utc = info.utc;
                utc = to_string(Utc.year + 1900) + '-';
                utc += to_string(Utc.mon + 1) + '-';
                utc += to_string(Utc.day) + 'T';
                utc += to_string(Utc.hour) + ':';
                utc += to_string(Utc.min) + ':';
                utc += to_string(Utc.sec) + 'Z';
                is_ok = true;
                break;
             }
        }

    }

    printf("exit\n");
    return is_ok;    
}

bool GpsDeviceBN::GetLatitude(double& latitude) {
    nmeaINFO info;
    int repeat_time = 3;
    bool is_ok = false;
    char buf[200] = {0};
    int32_t ret = 0, us = 10000 * 1000;     

    nmea_zero_INFO(&info);

    while(repeat_time --) {
        ret = serial_read(serial, buf, sizeof(buf), us);
        if (ret > 0) {
             parase(buf, &info);
             if (info.sig > 0) {
                latitude = info.lat;
                is_ok = true;
                break;
             }
        }

    }

    printf("exit\n");
    return is_ok;
}

bool GpsDeviceBN::GetLongitude(double& longitude) {
    nmeaINFO info;
    int repeat_time = 3;
    bool is_ok = false;
    char buf[200] = {0};
    int32_t ret = 0, us = 10000 * 1000;     

    nmea_zero_INFO(&info);

    while(repeat_time --) {
        ret = serial_read(serial, buf, sizeof(buf), us);
        if (ret > 0) {
             parase(buf, &info);
             if (info.sig > 0) {
                longitude = info.lon;
                is_ok = true;
                break;
             }
        }

    }

    printf("exit\n");
    return is_ok;
}

bool GpsDeviceBN::GetUtc(string& Json) {
    nmeaINFO info;
    nmeaTIME utc;
    int repeat_time = 3;
    bool is_ok = false;
    char buf[200] = {0};
    int32_t ret = 0, us = 10000 * 1000;     

    nmea_zero_INFO(&info);

    while(repeat_time --) {
        ret = serial_read(serial, buf, sizeof(buf), us);
        if (ret > 0) {
             parase(buf, &info);
             if (info.sig > 0) {
                utc = info.utc;
                Json = to_string(utc.year + 1900) + '-';
                Json += to_string(utc.mon + 1) + '-';
                Json += to_string(utc.day) + 'T';
                Json += to_string(utc.hour) + ':';
                Json += to_string(utc.min) + ':';
                Json += to_string(utc.sec) + 'Z';
                is_ok = true;
                break;
             }
        }

    }

    printf("exit\n");
    return is_ok;
}

void GpsDeviceBN::parase(const char* buf, nmeaINFO* info) {
    int ret = 0;
    nmeaPARSER parser;
    string GNRMC = "$GNRMC";
    string data = buf;

    if ((ret = data.find(GNRMC)) != string::npos) {
        string tmp = data.substr(ret);
        if (ret = tmp.find('\n', 0)) {
            tmp = tmp.substr(0,ret);   
            tmp = "$GPGGA,213916.199,4221.0377,N,07102.9778,W,1,03,13.4,-32.4,M,-33.7,M,,0000*45\r\n";
            printf("tmp = %s\n", tmp.c_str());
            nmea_parser_init(&parser);
            nmea_parse(&parser, tmp.c_str(), tmp.size(), info);
            nmea_parser_destroy(&parser);
        }
        
    }
}

void GpsDeviceBN::GpsClose() {
    if (serial) {
        serial_close(serial);
        free(serial); 
        serial = NULL;
    }
}