
// string_helper.cpp : Transformation functions
// Austin Hester CS542o oct 2020
// g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

#include "./include/string_helper.hpp"

#include <sstream>

std::vector<std::string>
split (std::string s, char delim)
{
    // cred: https://stackoverflow.com/a/46931770
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (std::getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}
