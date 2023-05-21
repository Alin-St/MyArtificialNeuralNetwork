#pragma once

#include <vector>
#include <tuple>
#include <cstdint>
#include <fstream>
#include <iostream>

// Dummy declaration to solve IDE inspection bug (https://github.com/clangd/clangd/issues/1167).
static_assert(true);

#pragma pack(push, 1)

// Bitmap file header structure
struct BitmapFileHeader {
    uint16_t fileType{0x4D42};   // "BM" signature
    uint32_t fileSize{0};        // Size of the file in bytes
    uint16_t reserved1{0};       // Reserved, set to 0
    uint16_t reserved2{0};       // Reserved, set to 0
    uint32_t offset{0};          // Offset to the pixel data
};

// Bitmap info header structure
struct BitmapInfoHeader {
    uint32_t size{40};           // Size of this header in bytes
    int32_t width{0};            // Width of the image in pixels
    int32_t height{0};           // Height of the image in pixels
    uint16_t planes{1};          // Number of color planes, must be 1
    uint16_t bitCount{24};       // Number of bits per pixel (24 for RGB)
    uint32_t compression{0};     // Compression type (0 for uncompressed)
    uint32_t imageSize{0};       // Size of the image data in bytes
    int32_t xPixelsPerMeter{0};  // Horizontal resolution (pixels per meter)
    int32_t yPixelsPerMeter{0};  // Vertical resolution (pixels per meter)
    uint32_t colorsUsed{0};      // Number of colors used (0 for full color)
    uint32_t colorsImportant{0}; // Number of important colors (0 for all)
};

/// Converts a 24 bit color depth image into a bitmap and returns the bytes.
std::vector<char> imageToBmp(const std::vector<std::vector<std::tuple<char, char, char>>>& img) {
    std::vector<char> bmpData;

    // Calculate image dimensions
    int width = (int)img[0].size();
    int height = (int)img.size();

    // Calculate padding bytes (if any)
    int padding = (4 - (width * 3) % 4) % 4;

    // Calculate total file size
    int imageSize = (width * 3 + padding) * height;
    int fileSize = (int)sizeof(BitmapFileHeader) + (int)sizeof(BitmapInfoHeader) + imageSize;

    // Create and initialize the bitmap file header
    BitmapFileHeader fileHeader;
    fileHeader.fileSize = fileSize;
    fileHeader.offset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

    // Create and initialize the bitmap info header
    BitmapInfoHeader infoHeader;
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.imageSize = imageSize;

    // Copy the bitmap headers to the output data
    bmpData.insert(bmpData.end(), reinterpret_cast<const char*>(&fileHeader), reinterpret_cast<const char*>(&fileHeader + 1));
    bmpData.insert(bmpData.end(), reinterpret_cast<const char*>(&infoHeader), reinterpret_cast<const char*>(&infoHeader + 1));

    // Iterate over each pixel in the image (in reverse since bmp stores pixels from bottom to top).
    for (int i = (int)img.size() - 1; i >= 0; --i) {
        const auto& row = img[i];
        for (const auto& pixel : row) {
            // Unpack RGB values from the tuple
            char red, green, blue;
            std::tie(red, green, blue) = pixel;

            // Write pixel data in BGR order (little-endian)
            bmpData.push_back(blue);
            bmpData.push_back(green);
            bmpData.push_back(red);
        }

        // Add padding bytes (if necessary)
        for (int j = 0; j < padding; ++j) {
            bmpData.push_back(0);
        }
    }

    return bmpData;
}

#pragma pack(pop)
