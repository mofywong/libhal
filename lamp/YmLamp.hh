#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;  

typedef struct  lamp_ym_s lamp_ym_t;
class YmLamp
{
public:
    YmLamp(void);
    virtual ~YmLamp(void);

public:
    int DevOpen(const std::string& device, int flags);
    int DevClose(void);
    int LampYmSetOnoff(int channel,int onoff);
    int LampYmSetBrightness(int channel,int onoff);
    int LampYmGetAttr(void* output);
    
private:
    lamp_ym_t* lamp_ctx;
}; 





