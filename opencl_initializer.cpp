#include <iostream>
#include "opencl_initializer.hpp"

#ifndef NO_SETUP_ERRORS
#define CL_HPP_OPENCL_ENABLE_EXCEPTIONS
#endif

using namespace cl;

Context context;
CommandQueue queue;

void initCL(const int EXTRAS) {
    vector<Platform> platforms;
    Platform::get(&platforms);
    #ifndef NO_SETUP_ERRORS
    if (!platforms.size()) {
        throw std::runtime_error("No available OpenCL platforms");
    }
    #endif
    vector<Device> devices;
    Device device;
    for (uint i = 0; i <platforms.size(); i++) {
        platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if(!devices.size()) {
            continue;
        }
        device = devices[i];
    }
    #ifndef NO_SETUP_ERRORS
    if (!devices.size()) {
        throw std::runtime_error("No available OpenCL compatible GPUs");
    }
    #endif

    context = Context(device);
    if (EXTRAS & OPENCL_INITIALIZER_OUT_OF_ORDER) {
        QueueProperties properties = cl::QueueProperties(CL_QUEUE_PROPERTIES);
        queue = CommandQueue(context, device, properties);
    }
}