// lomo.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Austin Hester CS542o nov 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <iostream>

#include "./include/cla_parse.hpp"
#include "./include/dir_func.hpp"
#include "./include/img_struct.hpp"
#include "./include/histo_func.hpp"


const std::string window_name = "Lomography";
int slider_red_value = 0;
int slider_vig_value = 0;
cv::Mat original_image;
cv::Mat blended_image;

int
wait_key()
{
    char key_pressed = cv::waitKey(0) & 255;
    // 'q' or  <escape> quits out
    if (key_pressed == 27 || key_pressed == 'q') {
        cv::destroyAllWindows();
        return 0;
    }
    return 1;
}

static void
on_trackbar_red_level(int, void*)
{
    double alpha = (double) slider_red_value / 12;
    double beta = 1.0 - alpha;
    original_image = run_image_matching(original_image, blended_image);
    cv::imshow(window_name, original_image);
    blended_image = original_image;
}

static void
on_trackbar_vignette(int, void*)
{
    double alpha = (double) slider_vig_value / 100;
    double beta = 1.0 - alpha;
    original_image = run_image_matching(original_image, blended_image);
    cv::imshow(window_name, original_image);
    blended_image = original_image;
}

int
main(int argc, const char** argv)
{
    // CLA variables
    std::string input_image;

    // parse and save command line args
    int parse_result = parse_arguments(
        argc, argv,
        &input_image
    );
    if (parse_result != 1) return parse_result;

    // open image
    img_struct_t* og_image = open_image(input_image.c_str(), true);

    if (og_image == NULL) {
        std::cerr << "Could not open image :( " << input_image << std::endl;
        return -1;
    }
    blended_image = open_image("bear.jpg", true)->image;
    original_image = og_image->image;
    // display the original image
    cv::imshow(window_name, original_image);

    cv::createTrackbar("Red Level", window_name, &slider_red_value, 12, on_trackbar_red_level);
    cv::createTrackbar("Vignette", window_name, &slider_vig_value, 100, on_trackbar_vignette);

    wait_key();

    return 0;
}
