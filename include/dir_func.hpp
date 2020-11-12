// dir_func.hpp : Directory functions
// Austin Hester CS542o sept 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#ifndef _H_DIR_FUNC
#define _H_DIR_FUNC

#include "img_struct.hpp"

img_struct_t* open_image(std::string file_path, bool grayscale);
int create_dir_recursive(std::string dst_file);
int write_img_to_file(cv::Mat image, std::string output_dir, std::string file_name);

#endif
