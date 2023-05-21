#include "utility.h"
#include <fstream>
#include <sstream>
#include <tuple>

using namespace std;

string readTextFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for reading.");
    }

    string text;
    try {
        file.seekg(0, ios::end);
        text.resize(file.tellg());
        file.seekg(0, ios::beg);
        file.read(&text[0], (int)text.size());
    } catch (...) {
        file.close();
        throw;
    }

    file.close();
    return text;
}

void writeBytesToFile(const string &filename, const vector<char> &bytes) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing.");
    }

    try {
        file.write(bytes.data(), (int)bytes.size());
    } catch (...) {
        file.close();
        throw;
    }

    file.close();
}

vector<LabeledImage> imagesFromCsv(const string &csv) {

    constexpr int IMG_WIDTH = 28;
    constexpr int IMG_HEIGHT = 28;

    auto parsedCsv = parseCsv(csv);

    // Check the format of the csv thoroughly.
    if (parsedCsv.size() < 2)
        throw runtime_error("Invalid csv.");
    for (auto& row : parsedCsv) {
        if (row.size() != IMG_WIDTH * IMG_HEIGHT + 1)
            throw runtime_error("Invalid csv.");
    }
    if (parsedCsv.at(0).at(0) != "label")
        throw runtime_error("Invalid csv.");
    for (int i = 1; i < 1 + IMG_WIDTH * IMG_HEIGHT; ++i) {
        auto& current_header = parsedCsv.at(0).at(i);
        int arr_ind = i - 1;
        auto correct_header = to_string(arr_ind / IMG_WIDTH + 1) + "x" + to_string(arr_ind % IMG_WIDTH + 1);
        if (current_header != correct_header)
            throw runtime_error("Invalid csv.");
    }
    for (int i = 1; i < parsedCsv.size(); ++i) {
        int label = stoi(parsedCsv.at(i).at(0));
        if (label < 0 || label > 9)
            throw runtime_error("Invalid csv.");
    }
    for (int i = 1; i < parsedCsv.size(); ++i) {
        for (int j = 1; j < 1 + IMG_WIDTH * IMG_HEIGHT; ++j) {
            int cell = stoi(parsedCsv.at(i).at(j));
            if (cell < 0 || cell >= 256)
                throw runtime_error("Invalid csv.");
        }
    }

    // Get the image list.
    vector<LabeledImage> images;

    for (int i = 1; i < parsedCsv.size(); ++i) {
        // Get the label.
        auto& label_str = parsedCsv.at(i).at(0);
        int label = stoi(label_str);

        // Convert the array of strings (raw csv data) into an array of integers (pixels).
        vector<char> pixel_array;
        for (int j = 1; j < 1 + IMG_WIDTH * IMG_HEIGHT; ++j) {
            auto& pixel_str = parsedCsv.at(i).at(j);
            pixel_array.push_back((char)stoi(pixel_str));
        }

        // Convert the array of pixels into a matrix of pixels (a grayscale image).
        auto pixel_matrix = arrayToMatrix(pixel_array, IMG_WIDTH);

        // Add the image to the list.
        images.emplace_back(label, pixel_matrix);
    }

    return images;
}

vector<vector<string>> parseCsv(const std::string& csv) {
    vector<vector<string>> result;
    istringstream stream(csv);
    string line;

    while (getline(stream, line)) {
        vector<string> row;
        istringstream lineStream(line);
        string cell;

        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }

        result.push_back(row);
    }

    if (!stream.eof()) {
        throw runtime_error("Error occurred while parsing CSV.");
    }

    return result;
}

template<typename T>
vector<vector<T>> arrayToMatrix(const vector<T>& arr, int width) {
    vector<vector<T>> matrix;
    int height = (arr.size() + width - 1) / width;  // Calculate the number of rows

    for (int i = 0; i < height; ++i) {
        vector<T> row;

        for (int j = 0; j < width; ++j) {
            int index = i * width + j;

            if (index < arr.size()) {
                row.push_back(arr[index]);
            } else {
                T defaultValue{};
                row.push_back(defaultValue);
            }
        }

        matrix.push_back(row);
    }

    return matrix;
}

ColorImage grayscaleToColorImage(const Image& img) {
    ColorImage colorImg;

    for (const auto& row : img) {
        vector<tuple<char, char, char>> colorRow;

        for (const auto& pixel : row) {
            char intensity = pixel;
            colorRow.emplace_back(intensity, intensity, intensity);
        }

        colorImg.push_back(colorRow);
    }

    return colorImg;
}
