#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <algorithm>

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value);

float read_float(std::istream & is);
int read_int(std::istream & is);

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char const * as_buffer(T const & value);

void write_float(float value, std::ostream & os);
void write_int(int value, std::ostream & os);

bool compare_binary_files(const std::string& filename1, const std::string& filename2);

// TODO: DELETE LATER
void binaryToText(std::string const & inputFile, std::string const & outputFile);

#endif  // UTILITY_HPP
