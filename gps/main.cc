#include <stdio.h>
#include <string>

#include "GpsDevice.hh"

int main(int argc, char* argv[]) {
    double latitude = 0;
    GpsDevice device;
    char value[100] = {0};
    std::string input = "{\"Model\":\"bn\",\"Adde\":1,\"Dev\":\"/dev/ttyUSB0\",\"Port\":{\"Baud\":9600,\"Parity\":\"N\",\"Data\":8,\"Stop\":1}}";

    printf("begin\n");
    if (device.DevOpen(input)) {
        printf("start\n");
        device.GetLatitude((void *)value);
        printf("value = %s\n", value);
        device.GetUtc((void *)value); 
        printf("value = %s\n", value);
        device.DevClose();
    }
    
    printf("main exit\n");
    return 0;
}