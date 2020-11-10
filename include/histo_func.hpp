// histo_func.hpp : Transformation functions
// Austin Hester CS542o sept 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#ifndef _H_HISTO_FUNC
#define _H_HISTO_FUNC

#include <opencv2/core/core.hpp>

cv::Mat run_equalization(cv::Mat image);
cv::Mat run_image_matching(cv::Mat image, cv::Mat match_to);
cv::Mat run_histogram_matching(cv::Mat image, std::string file_name);
void build_intensity_map(cv::Mat src, uint* intensity_counts);
void normalize_histogram(uint* intensity_counts, uint num_pixels, float* normalized_histogram);
void cdf_from_normalized(float* normalized_histogram, float* cdf);
void create_lookup_table(float* cdf, uint* lookup_table);
cv::Mat apply_histogram(cv::Mat src, uint* lookup_table);

#endif
