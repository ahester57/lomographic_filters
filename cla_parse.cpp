// cla_parse.cpp : Parse given command line arguments.
// Austin Hester CS542o nov 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <iostream>

#include "./include/cla_parse.hpp"


// parse command line arguments
int
parse_arguments(
    int argc,
    const char** argv,
    std::string* input_image
) {
    cv::String keys =
        "{@filename   |<none>| input image}"             // input image is the first argument (positional)
        "{help h      |      | show help message}";

    cv::CommandLineParser parser(argc, argv, keys);

    if (parser.has("h")) {
        parser.printMessage();
        return 0;
    }

    if (!parser.check()) {
        parser.printErrors();
        parser.printMessage();
        return -1;
    }

    try {
        *input_image = (std::string) parser.get<std::string>(0).c_str();
        if (input_image == NULL || input_image->size() == 0) {
            parser.printMessage();
            return -1;
        }
    } catch (...) {
        std::cerr << "Failed to parse input image argument!:" << std::endl;
        return -1;
    }

    return 1;
}
