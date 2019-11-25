#ifndef _JSON_PARSE_HH_
#define _JSON_PARSE_HH_

#include <string>

/*RS485设备配置json格式解析:Ventor:区分厂家,Model：区分型号
json名：RS485
{
   "Model": "84-D",
   "Addr": 1,
   "Dev": "/dev/ttyUSB0",
   "Port": {
      "Baud": 9600,
      "Data": 8,
      "Stop": 1,
      "Parity": "N"
   }
} */

using std::string;

bool GetRs485(const string& input, string& Model, int& Addr, string& Dev, int& Baud, int& Data, int& Stop, string& Parity);
bool GetRs485Model(const string& input, string& Model);
bool GeneralGpsMessage(double& Latitude, double& Longitude, const string& Utc, string& output);
bool GeneralUtcMessage(const string& Utc, string& output);
bool GeneralLongitudeMessage(const int Longitude, string& output);
bool GeneralLatitudeMessage(const int Latiutude, string& output);

#endif