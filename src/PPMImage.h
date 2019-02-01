#ifndef PPMIMAGE_H
#define PPMIMAGE_H

#include <string>
#include <vector>
#include "loadPNM.h"

class PPMImage {
  public:
    unsigned int width;
    unsigned int height;
    unsigned int numComponents;
    std::vector<unsigned char> image_data;

    PPMImage(unsigned int width, 
             unsigned int height,
             unsigned int numComponents,
             const std::vector<unsigned char>& image_data)
    : width(width),
      height(height),
      numComponents(numComponents),
      image_data(image_data)
    {
    }
    
    PPMImage(const std::string& path) {
      unsigned char* temp_image_data = loadPNM(path.c_str(), width, height, numComponents);
      image_data.assign(temp_image_data, temp_image_data + width * height * numComponents); 
      free(temp_image_data);
    }

    void write_to_file(const std::string& path) {
      auto temp_image_data = image_data;
      WritePNM(path.c_str(), width, height, numComponents, temp_image_data.data());
    }
};

#endif