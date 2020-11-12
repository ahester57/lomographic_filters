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

#define EEEE  2.71828
#define WINDOW_NAME "Lomography"

const uint S_VALUES = 13;

int slider_red_value = 0;
int slider_vig_value = 0;
cv::Mat original_image;

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
    double alpha = (double) slider_red_value / S_VALUES;
    double beta = 1.0 - alpha;
    // original_image = run_image_matching(original_image, original_image);
    cv::imshow(WINDOW_NAME, original_image*alpha);
}

static void
on_trackbar_vignette(int, void*)
{
    double alpha = (double) slider_vig_value / 100;
    double beta = 1.0 - alpha;
    // original_image = run_image_matching(original_image, original_image);
    cv::imshow(WINDOW_NAME, original_image/(alpha));
}

void
create_LUT(uchar** LUT)
{
    float s_real[] = { 0.08, 0.09, 0.1, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2 };
    // for each s value
    for (uint s = 0; s < S_VALUES; s++) {
        // each red value
        for (uint r = 0; r < 256; r++) {
            LUT[s][r] =
            256 /
                (1 +
                    pow(
                        EEEE ,
                        -1 * (
                            (r / 256.0 - 0.5) /
                                s_real[s]
                        )
                    )
                );
        }
    }
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
    img_struct_t* og_image = open_image(input_image.c_str(), false);

    if (og_image == NULL) {
        std::cerr << "Could not open image :( " << input_image << std::endl;
        return -1;
    }

    original_image = og_image->image;
    // display the original image
    cv::imshow(WINDOW_NAME, original_image);

    // initialize LUT
    uchar* LUT[S_VALUES];
    for (uint s = 0; s < S_VALUES; s++) {
        LUT[s] = new uchar[256];
    }
    create_LUT(LUT);

    cv::createTrackbar("Red Level", WINDOW_NAME, &slider_red_value, S_VALUES, on_trackbar_red_level);
    cv::createTrackbar("Vignette", WINDOW_NAME, &slider_vig_value, 100, on_trackbar_vignette);

    wait_key();

    for (uint s = 0; s < S_VALUES; s++) {
        delete LUT[s];
    }
    return 0;
}
