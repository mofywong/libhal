#include <iostream>
#include <unistd.h>
#include <pthread.h>  
#include "BrocastDevice.hh"

void* pause(void* parament) {
    BrocastDevice* device = (BrocastDevice*)parament;

    sleep(10);
    device->Pause(NULL);
    sleep(10);
    device->Continue(NULL);
    std::cout<< "exit thread" << std::endl;
}


int main(int argc, char* argv[]) {
    bool ret = false;
    pthread_t fd; 
    std::string input;
    BrocastDevice device = BrocastDevice();
 
    ret = device.DevOpen(input);
    std::cout<< ret << std::endl;
    pthread_create(&fd, NULL, pause, &device);
    device.Play(argv[1], NULL);
    std::cout<< "exit play" << std::endl;

    std::cout<< "exit" << std::endl;
    return 0;
}