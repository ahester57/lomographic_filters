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
const uint INTENSITY_VALUES = 256;

uchar* LUT[S_VALUES];

int slider_red_value = 0;
int slider_vig_value = 0;

uint max_radius;

cv::Mat original_image;
cv::Mat displayed_image;

int
wait_key()
{
    char key_pressed = cv::waitKey(0) & 255;
    // 's' saves the current image
    if (cv::getWindowProperty(WINDOW_NAME, cv::WND_PROP_VISIBLE) < 1) {
        // this ends the program if window is closed
        return 0;
    }
    if (key_pressed == 's') {
        if (!displayed_image.empty()) {
            write_img_to_file(displayed_image, "./out", "lomo_output.jpg");
            cv::destroyAllWindows();
            return 0;
        }
    }
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
    // if zero, use original
    if (slider_red_value == 0) {
        original_image.copyTo(displayed_image);
        cv::imshow(WINDOW_NAME, displayed_image);
        return;
    }

    // split the original image into 3 rgb channels
    cv::Mat rgb_values[3];
    cv::split(original_image, rgb_values);
    std::vector<cv::Mat> channels = { rgb_values[0], rgb_values[1], rgb_values[2] };

    // adjust red value for each pixel
    for (int r = 0; r < channels[1].rows; r++) {
        for (int c = 0; c < channels[1].cols; c++) {
            // apply LUT for each pixel
            uint pixel = (uint) channels[1].at<uchar>(r, c);
            channels[1].at<uchar>(r, c) = LUT[slider_red_value-1][pixel];
        }
    }

    // merge channels back together
    cv:merge(channels, displayed_image);
    cv::imshow(WINDOW_NAME, displayed_image);
}

static void
on_trackbar_vignette(int, void*)
{
    //     Compute the maximum radius of the halo as the minimum of number of rows and colums in the image. Use the
    // percentage from the trackbar to draw a circle of radius as r – percentage of maximum radius. Each pixel in this
    // circle is assigned as 1 (white).
    cv::Mat matrix = cv::Mat(original_image.size(), CV_32FC3);
    for (int r = 0; r < matrix.rows; r++) {
        for (int c = 0; c < matrix.cols; c++) {
            matrix.at<float>(r, c) = 0.75;
        }
    }
    cv::imshow(WINDOW_NAME, matrix);
}

void
create_red_level_LUT(uchar** LUT)
{
    // compute the LUT for each s value so we don't have to recompute every time the slider changes.
    float s_real[] = { 0.08, 0.09, 0.1, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2 };
    // for each s value
    for (uint s = 0; s < S_VALUES; s++) {
        // each red value
        for (uint r = 0; r < INTENSITY_VALUES; r++) {
            LUT[s][r] =
                INTENSITY_VALUES /
                    (1 +
                        pow(
                            EEEE ,
                            -1 * (
                                (r / (float)INTENSITY_VALUES - 0.5) /
                                    s_real[S_VALUES-s-1]
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
    original_image.copyTo(displayed_image);

    // display the original image
    cv::imshow(WINDOW_NAME, original_image);

    // initialize LUT
    for (uint s = 0; s < S_VALUES; s++) {
        LUT[s] = new uchar[INTENSITY_VALUES];
    }
    create_red_level_LUT(LUT);

    // compute max radius for halo
    max_radius = original_image.rows < original_image.cols ? original_image.cols : original_image.rows;

    cv::createTrackbar("Red Level", WINDOW_NAME, &slider_red_value, S_VALUES, on_trackbar_red_level);
    cv::createTrackbar("Vignette", WINDOW_NAME, &slider_vig_value, 100, on_trackbar_vignette);

    while (wait_key());

    original_image.release();
    displayed_image.release();

    for (uint s = 0; s < S_VALUES; s++) {
        delete LUT[s];
    }
    return 0;
}
