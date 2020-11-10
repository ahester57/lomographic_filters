// histo_func.hpp : Historgram unctions
// Austin Hester CS542o oct 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <math.h>

#include "./include/histo_func.hpp"


cv::Mat
run_equalization(cv::Mat image)
{
    // map for intensity counts
    uint intensity_counts[256] = { 0 };
    build_intensity_map(image, intensity_counts);

    // normalize histogram of original image
    float normalized_histogram[256] = { 0 };
    normalize_histogram(intensity_counts, image.rows * image.cols, normalized_histogram);

    // compute the cumulative distribution function
    float cdf[256] = { 0 };
    cdf_from_normalized(normalized_histogram, cdf);

    // add lookup table
    uint lookup_table[256] = { 0 };    // map for intensity counts
    create_lookup_table(cdf, lookup_table);

    // apply the equalization to the image
    cv::Mat equalized_image = apply_histogram(image, lookup_table);
    return equalized_image;
}

cv::Mat
run_image_matching(cv::Mat image, cv::Mat match_to)
{
    // map for intensity counts
    uint intensity_counts[256] = { 0 };
    build_intensity_map(match_to, intensity_counts);

    // normalize histogram of original image
    float normalized_histogram[256] = { 0 };
    normalize_histogram(intensity_counts, match_to.rows * match_to.cols, normalized_histogram);

    // compute the cumulative distribution function
    float cdf[256] = { 0 };
    cdf_from_normalized(normalized_histogram, cdf);

    // add lookup table
    uint lookup_table[256] = { 0 };    // map for intensity counts
    create_lookup_table(cdf, lookup_table);

    // apply the equalization to the image
    cv::Mat matched_image = apply_histogram(image, lookup_table);
    return matched_image;
}

cv::Mat
run_histogram_matching(cv::Mat image, std::string file_name)
{
    // apply the equalization to the image
    std::ifstream infile(file_name);
    float num;
    uint count = 0;
    float normalized_histogram_file [256] = { 0 }; // for mode 3

    while (infile >> num)
    {
        // read normalized histogram from file
        normalized_histogram_file[count++] = num;
    }

    // compute the cumulative distribution function
    float cdf[256] = { 0 };
    cdf_from_normalized(normalized_histogram_file, cdf);

    // add lookup table
    uint lookup_table[256] = { 0 };    // map for intensity counts
    create_lookup_table(cdf, lookup_table);

    cv::Mat matched_image = apply_histogram(image, lookup_table);
    return matched_image;
}

// take an image, and array[256]. fill array with count of intensity levels
void
build_intensity_map(cv::Mat src, uint* intensity_counts)
{
    if (src.channels() != 1) {
        std::cerr << "Not a Grayscale Image" << std::endl;
        throw "Not a Grayscale Image";
    }
    for (int r = 0; r < src.rows; r++) {
        for (int c = 0; c < src.cols; c++) {
            intensity_counts[(uint)src.at<uchar>(r, c)]++;
        }
    }
}

// normalize an intensity level array, such as the one provided by build_intensity_map.
void
normalize_histogram(uint* intensity_counts, uint num_pixels, float* normalized_histogram)
{
    for (uint i = 0; i < 256; i++) {
        normalized_histogram[i] = (float) intensity_counts[i] / num_pixels;
    }
}

// normalize an intensity level array, such as the one provided by build_intensity_map.
void
cdf_from_normalized(float* normalized_histogram, float* cdf)
{
    cdf[0] = normalized_histogram[0];
    for (uint i = 1; i < 256; i++) {
        cdf[i] = normalized_histogram[i] + cdf[i-1];
    }
}

void
create_lookup_table(float* cdf, uint* lookup_table)
{
    for (uint i = 0; i < 256; i++) {
        lookup_table[i] = std::round(cdf[i] * 255.0);
    }
}


cv::Mat
apply_histogram(cv::Mat src, uint* lookup_table)
{
    cv::Mat dst = cv::Mat::zeros(cv::Size(src.cols, src.rows), src.type());
    if (src.channels() == 1) {
        std::cout << "Grayscale Image <uchar>" << std::endl;
    } else {
        std::cout << "Unknown Image. Goodbye." << std::endl;
        return src;
    }
    for (int r = 0; r < src.rows; r++) {
        for (int c = 0; c < src.cols; c++) {
            // channels is 1. grayscale
            uint pixel = (uint) src.at<uchar>(r, c);
            dst.at<uchar>(r, c) = (uchar) lookup_table[pixel];
        }
    }
    return dst;
}
