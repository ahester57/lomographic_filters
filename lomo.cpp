// lomo.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Austin Hester CS542o nov 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>

#include "./include/cla_parse.hpp"
#include "./include/dir_func.hpp"
#include "./include/img_struct.hpp"
#include "./include/string_helper.hpp"


static void on_trackbar_vignette(int, void*);


#define EEEE  2.71828
#define WINDOW_NAME "Lomography"

// CLA variable
std::string input_image;

const uint S_VALUES = 13;
const uint INTENSITY_VALUES = 256;

uchar* LUT[S_VALUES];

int slider_red_value = 0;
int slider_vig_value = 0;

uint max_radius;

cv::Mat original_image;
cv::Mat red_level_image;
cv::Mat displayed_image;
cv::Point center;


// 'event loop' for keypresses
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
            write_img_to_file(
                displayed_image,
                "./out",
                "lomo_" + std::to_string(slider_red_value) +
                    "_" + std::to_string(slider_vig_value) + input_image
            );
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

// trackbar for red level
static void
on_trackbar_red_level(int, void*)
{
    // if zero, use original
    if (slider_red_value == 0) {
        original_image.copyTo(red_level_image);
        red_level_image.copyTo(displayed_image);
        on_trackbar_vignette(slider_vig_value, &slider_vig_value);
        return;
    }

    // split the original image into 3 rgb channels
    cv::Mat rgb_values[3];
    cv::split(original_image, rgb_values);
    std::vector<cv::Mat> channels = { rgb_values[0], rgb_values[1], rgb_values[2] };

    // adjust red value for each pixel
    for (int r = 0; r < channels[2].rows; r++) {
        for (int c = 0; c < channels[2].cols; c++) {
            // apply LUT for each pixel
            channels[2].at<uchar>(r, c) =
                LUT[slider_red_value-1][(uint) channels[2].at<uchar>(r, c)];
        }
    }

    // merge channels back together
    cv:merge(channels, red_level_image);
    red_level_image.copyTo(displayed_image);
    on_trackbar_vignette(slider_vig_value, &slider_vig_value);
}

// trackbar for vignette filter
static void
on_trackbar_vignette(int, void*)
{
    // if zero, use original
    if (slider_vig_value == 0) {
        red_level_image.copyTo(displayed_image);
        cv::imshow(WINDOW_NAME, displayed_image);
        return;
    }
    //     Compute the maximum radius of the halo as the minimum of number of rows and colums in the image. Use the
    // percentage from the trackbar to draw a circle of radius as r – percentage of maximum radius. Each pixel in this
    // circle is assigned as 1 (white).
    uint radius = max_radius - ((100 - slider_vig_value) / 100.0 * max_radius) + 1;
    cv::Mat halo = cv::Mat(original_image.size(), CV_32FC3);

    // build the halo matrix
    for (int r = 0; r < halo.rows; r++) {
        for (int c = 0; c < halo.cols; c++) {
            halo.at<cv::Vec3f>(r, c) = { 0.75, 0.75, 0.75 };
        }
    }
    std::cout << radius;

    // draw circle (cv::circle does NOT work like this...) no clue
    cv::Mat halo_tmp;
    cv::circle(halo, center, radius, cv::Scalar(1,1,1), cv::FILLED);
    cv::blur(halo, halo_tmp, cv::Size(radius, radius));
    std::cout << cv_type_to_str(halo.depth(), halo.channels()) << std::endl;
    std::cout << halo.at<cv::Vec3f>(halo.cols/2, halo.rows/2) << std::endl;

    cv::Mat dst;
    red_level_image.convertTo(dst, CV_32FC3, 1/255.0);
    cv::circle(dst, center, radius, cv::Scalar(255.0, 255.0, 255.0), cv::FILLED);

    for (int r = 0; r < dst.rows; r++) {
        for (int c = 0; c < dst.cols; c++) {
            // get pixels from both
            cv::Vec3f pixel_dst = dst.at<cv::Vec3f>(r, c);
            cv::Vec3f pixel_halo = halo_tmp.at<cv::Vec3f>(r, c);

            cv::Vec3f pixel_vig = { 0.0, 0.0, 0.0 };
            pixel_vig[0] = pixel_dst[0] * pixel_halo[0];
            pixel_vig[1] = pixel_dst[1] * pixel_halo[1];
            pixel_vig[2] = pixel_dst[2] * pixel_halo[2];

            dst.at<cv::Vec3f>(r, c) = pixel_vig;
        }
    }

    dst.convertTo(displayed_image, CV_8UC3, 255.0);

    cv::imshow(WINDOW_NAME, displayed_image);
}

// ...
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
    original_image.copyTo(red_level_image);

    // display the original image
    cv::imshow(WINDOW_NAME, displayed_image);

    // initialize red level LUT
    for (uint s = 0; s < S_VALUES; s++) {
        LUT[s] = new uchar[INTENSITY_VALUES];
    }
    create_red_level_LUT(LUT);

    // define max radius and center for halo
    max_radius = original_image.rows < original_image.cols ? original_image.rows : original_image.cols;
    center = cv::Point(original_image.cols * 2, original_image.rows * 2);

    cv::createTrackbar("Red Level", WINDOW_NAME, &slider_red_value, S_VALUES, on_trackbar_red_level);
    cv::createTrackbar("Vignette", WINDOW_NAME, &slider_vig_value, 100, on_trackbar_vignette);

    // 'event loop' for keypresses
    while (wait_key());

    original_image.release();
    red_level_image.release();
    displayed_image.release();
    for (uint s = 0; s < S_VALUES; s++) {
        delete LUT[s];
    }

    return 0;
}
