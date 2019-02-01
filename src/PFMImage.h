#ifndef PFMIMAGE_H
#define PFMIMAGE_H

#include <string>
#include <vector>
#include "loadPNM.h"

class PFMImage {
  public:
    unsigned int width;
    unsigned int height;
    unsigned int numComponents;
    std::vector<float> image_data;

    PFMImage(unsigned int width, 
             unsigned int height,
             unsigned int numComponents,
             const std::vector<float>& image_data)
    : width(width),
      height(height),
      numComponents(numComponents),
      image_data(image_data)
    {
    }
    
    PFMImage(const std::string& path) {
      float* temp_image_data = loadPFM(path.c_str(), width, height, numComponents);
      image_data.assign(temp_image_data, temp_image_data + width * height * numComponents); 
      free(temp_image_data);
    }

    void write_to_file(const std::string& path) {
      auto temp_image_data = image_data;
      WritePFM(path.c_str(), width, height, numComponents, temp_image_data.data());
    }

    /*
    void write_to_ppm(const std::string& path, ToneMap tone_map_type) {
      std::vector<unsigned char> temp_image_data;
      temp_image_data.reserve(image_data.size());

      for (float pixel_channel : image_data) {
        temp_image_data.push_back(static_cast<unsigned char>(pixel_channel * 255.0f));
      }

      WritePNM(path.c_str(), width, height, numComponents, temp_image_data.data());
    }
    */
};

#endif