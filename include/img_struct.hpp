// img_struct.hpp : Transformation functions
// Austin Hester CS542o sept 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#ifndef _H_IMG_STRUCT
#define _H_IMG_STRUCT

#include <opencv2/core/core.hpp>

typedef struct {
    cv::Mat image;
    std::string metadata;
    std::string file_name;
} img_struct_t;

#endif
