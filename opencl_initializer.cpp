#include <iostream>
#include <fstream>
#include <string>
#include "opencl_initializer.hpp"
#include <stdlib.h>
#include <errno.h>

#ifndef NO_SETUP_ERRORS
#define CL_HPP_OPENCL_ENABLE_EXCEPTIONS
#endif

using namespace cl;

Context context;
CommandQueue queue;

Program loadProgramFromSource(const string &SOURCE_NAME = "program.txt") {
    std::ifstream source_file(SOURCE_NAME);

    #ifndef NO_SETUP_ERRORS
    if (!source_file.is_open()) {
        throw std::runtime_error("Unable to open program source file");
    }
    #endif

    string source;
    while(getline(source_file, source)) {
    }

    source_file.close();

    Program program = Program(context, source, true);

    return program;
}

void initCL(const int EXTRAS = 0, const string &SOURCE_NAME = "program.txt", const string &BINARY_NAME = "binary.bin") {
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
        platforms[i].getDevices(CL_DEVICE_TYPE_CPU, &devices);
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
    if (EXTRAS & OPENCL_INITIALIZER_OUT_OF_ORDER && EXTRAS & OPENCL_INITIALIZER_PROFILE) {
        queue = CommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_PROPERTIES);
    } else if (EXTRAS & OPENCL_INITIALIZER_OUT_OF_ORDER) {
        queue = CommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROPERTIES);
    } else if (EXTRAS & OPENCL_INITIALIZER_PROFILE) {
        queue = CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_PROPERTIES);
    } else {
        queue = CommandQueue(context, device, CL_QUEUE_PROPERTIES);
    }

    Program program;

    if (EXTRAS & OPENCL_INITIALIZER_USE_SOURCE) {
        program = loadProgramFromSource(SOURCE_NAME);
    }
    if (EXTRAS & OPENCL_INITIALIZER_CREATE_BINARY) {
        program = loadProgramFromSource(SOURCE_NAME);

        size_t binary_size;
        program.getInfo(CL_PROGRAM_BINARY_SIZES, &binary_size);

        char *binary = static_cast<char *>(malloc(binary_size));
        program.getInfo(CL_PROGRAM_BINARIES, &binary);

        std::ofstream binary_file(BINARY_NAME, std::ios::binary);
        binary_file.write(binary, binary_size);
        binary_file.close();

        free(binary);
    }
    if (EXTRAS & OPENCL_INITIALIZER_USE_BINARY) {
        std::ifstream binary_file(BINARY_NAME, std::ios::binary);

        vector<unsigned char> binary(std::istreambuf_iterator<char>(binary_file), {});

        binary_file.close();

        Program::Binaries binaries = Program::Binaries(std::make_pair(binary.data(), binary.size()));
        program = Program(context, {device}, binaries);
    }
}