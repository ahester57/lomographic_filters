// dir_func.hpp : Directory functions
// Austin Hester CS542o sept 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utils/filesystem.hpp>

#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <dirent.h>

#include "./include/dir_func.hpp"
#include "./include/string_helper.hpp"

// Wrapper for open_dir, returns vector of strings (filenames)
std::vector<std::string>
open_dir(const char* dir_string)
{
    std::cout << "Selected input directory:\t" << dir_string << std::endl;
    std::vector<std::string> file_paths;
    open_dir(dir_string, " \n", &file_paths);
    return file_paths;
}

// recursively open all directories and assemble a list of filenames
void
open_dir(const std::string dir_string, const std::string line_prefix, std::vector<std::string>* file_paths)
{
    DIR *dr;
    try {
        dr = opendir(dir_string.c_str()); //open all directories
    } catch (...) {
        std::cerr << "Failed to open directory: [" << dir_string << "]" << std::endl;
        return;
    }
    if (!dr) {
        std::cerr << "Failed to open directory: [" << dir_string << "]" << std::endl;
        return;
    }
    bool is_dir = false;
    struct dirent *drnt;
    while ((drnt = readdir(dr)) != NULL) {
        if (strcmp(drnt->d_name, ".") == 0 || strcmp(drnt->d_name, "..") == 0) {
            continue;
        }
        // get the relative path
        const std::string rel_path = dir_string + (dir_string.back() == '/' ? "" : std::string("/")) + std::string(drnt->d_name);
        // check if is a directory
        is_dir = is_directory(rel_path.c_str());
        // print files in an organized manner
        std::cout << line_prefix << drnt->d_name;
        if (is_dir) {
            // open next directory
            open_dir(rel_path.c_str(), line_prefix + "\t", file_paths);
        } else {
            (*file_paths).push_back(rel_path);
        }
    }
    closedir(dr); //close the directory
}

// is the given path a directory?
int
is_directory(const char* path)
{
    // cred: https://stackoverflow.com/a/146938
    struct stat s;
    if( stat(path, &s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            return true;
        }
    }
    return false;
}

std::vector<img_struct_t>
get_images_from_path_vector(std::vector<std::string> file_paths)
{
    std::vector<img_struct_t> images;
    if (file_paths.size() == 0) return images;
    std::vector<std::string>::iterator it = file_paths.begin();

    while (it != file_paths.end()) { // loop until exit condition

        std::cout << std::endl << "File info:" << std::endl;
        std::cout << ' ' << *it << std::endl;

        try {
            // attempt to read the image
            cv::Mat src = cv::imread(*it);
            if (src.empty()) {
                std::cerr << "Cannot open input image: " + *it << std::endl;
                it = file_paths.erase(it);
                continue;
            }
            std::cout << "Image size is:\t\t\t" << src.cols << "x" << src.rows << std::endl;

            // create the img_struct_t
            img_struct_t image_struct = {src, *it, *it};

            images.push_back(image_struct);
            it++;
        } catch (std::string &str) {
            std::cerr << "Error: " << *it << ": " << str << std::endl;
            return images;
        } catch (cv::Exception &e) {
            std::cerr << "Error: " << *it << ": " << e.msg << std::endl;
            return images;
        }
    }
    return images;
}

img_struct_t*
open_image(std::string file_path, bool grayscale)
{
    try {
        // attempt to read the image
        cv::Mat src;
        if (grayscale)
            src = cv::imread(file_path, cv::IMREAD_GRAYSCALE);
        else
            src = cv::imread(file_path);

        if (src.empty()) {
            std::cerr << "Cannot open input image: " + file_path << std::endl;
            return NULL;
        }
        std::cout << "Image size is:\t\t\t" << src.cols << "x" << src.rows << std::endl;

        // create the img_struct_t
        return new img_struct_t {src, file_path, file_path};
    } catch (std::string &str) {
        std::cerr << "Error: " << file_path << ": " << str << std::endl;
        return NULL;
    } catch (cv::Exception &e) {
        std::cerr << "Error: " << file_path << ": " << e.msg << std::endl;
        return NULL;
    }
}

int
create_dir_recursive(std::string dst_file)
{
    std::vector<std::string> split_dst_file = split(dst_file, '/');
    std::string output_so_far = "";
    for (std::string first_dir : split_dst_file) {
        if (output_so_far.length() > 0) {
            output_so_far = output_so_far + '/' + first_dir;
        } else output_so_far = first_dir;
        if (output_so_far == dst_file) return 1;
        if (!cv::utils::fs::createDirectory(output_so_far)) {
            std::cerr << "Could not create directory: " << output_so_far << std::endl;
            return -1;
        }
    }
    std::cerr << "this should never happen but compiler wants a return here.\n";
    return 1;
}

int
write_to_dir(std::vector<img_struct_t> image_vector, std::string output_dir, std::string file_type)
{
    // std::vector<int> compression_params;
    // compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    // compression_params.push_back(100);
    for (img_struct_t image_struct : image_vector) {
        try {
            std::string dst_file = output_dir + "/" + image_struct.metadata;
            if (!create_dir_recursive(dst_file)) {
                return -1;
            }
            std::cout << "Writing " << dst_file << std::endl;
            if (!cv::imwrite(dst_file, image_struct.image)) {
                std::cerr << "Could not write file " << dst_file << std::endl;
            }
            cv::waitKey(100);
            std::cout << "Wrote " << dst_file << std::endl;
        } catch (std::string &str) {
            std::cerr << "Error: " << str << std::endl;
            return -1;
        } catch (cv::Exception &e) {
            std::cerr << "Error: " << e.msg << std::endl;
            return -1;
        } catch (std::runtime_error &re) {
            std::cerr << "Error: " << re.what() << std::endl;
        }
    }
    return 0;
}

int
write_img_to_file(cv::Mat image, std::string output_dir, std::string file_name)
{
    try {
        std::string dst_file = output_dir + "/" + file_name;
        if (!create_dir_recursive(dst_file)) {
            std::cerr << "Could not create dir." << std::endl;
            return -1;
        }
        std::cout << "Writing " << dst_file << std::endl;
        if (!cv::imwrite(dst_file, image)) {
            std::cerr << "Could not write file " << dst_file << std::endl;
        }
        cv::waitKey(100);
        std::cout << "Wrote " << dst_file << std::endl;
    } catch (std::string &str) {
        std::cerr << "Error: " << str << std::endl;
        return -1;
    } catch (cv::Exception &e) {
        std::cerr << "Error: " << e.msg << std::endl;
        return -1;
    } catch (std::runtime_error &re) {
        std::cerr << "Error: " << re.what() << std::endl;
    }
    return 1;
}
