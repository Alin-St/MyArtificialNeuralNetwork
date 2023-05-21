#pragma once

#include <vector>
#include <string>

using Image = std::vector<std::vector<char>>;
using ColorImage = std::vector<std::vector<std::tuple<char, char, char>>>;
using LabeledImage = std::pair<int, Image>;

/// Reads the file data as text and returns the string.
std::string readTextFromFile(const std::string& filename);

/// Saves the given bytes into the specified file.
void writeBytesToFile(const std::string& filename, const std::vector<char>& bytes);

/// Returns a vector of (label, image) tuples given a csv file (format details in implementation).
std::vector<LabeledImage> imagesFromCsv(const std::string& csv);

/// Parse the csv text by splitting it by new-line characters and commas.
std::vector<std::vector<std::string>> parseCsv(const std::string& csv);

template <typename T>
/// Transform the one dimensional array into a matrix by splitting the array into lines of 'width' elements.
/// If the last line doesn't have 'width' elements default values will be inserted instead.
std::vector<std::vector<T>> arrayToMatrix(const std::vector<T>& arr, int width);

/// Converts a greyscale image with 8 bit-depth to a 24 bit-depth color image.
ColorImage grayscaleToColorImage(const Image& img);
