#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include "PFMImage.h"
#include "PPMImage.h"

float weighting(float x);

struct pixel_count {
  long long pixels = 0;
  long long r = 0;
  long long g = 0;
  long long b = 0;
};

pixel_count count_reliable_pixels(const PFMImage& image) {
  auto width = image.width;
  auto height = image.height;
  auto numComponents = image.numComponents;
  std::vector<float> image_data;
  image_data.reserve(image.image_data.size());

  pixel_count count;

  for (size_t pixel_index = 0; pixel_index < width * height; ++pixel_index) {
    bool is_reliable_pixel = true;
    float r_val = image.image_data[pixel_index * numComponents];
    float g_val = image.image_data[pixel_index * numComponents + 1];
    float b_val = image.image_data[pixel_index * numComponents + 2];

    float lower_bound = 0.005f;
    float upper_bound = 0.92f;

    if (r_val >= lower_bound && r_val <= upper_bound) {
      ++count.r;
    } else {
      is_reliable_pixel = false;
    }

    if (g_val >= lower_bound && g_val <= upper_bound) {
      ++count.g;
    } else {
      is_reliable_pixel = false;
    }

    if (b_val >= lower_bound && b_val <= upper_bound) {
      ++count.b;
    } else {
      is_reliable_pixel = false;
    }

    if (is_reliable_pixel) {
      ++count.pixels;
    }
  }

  return count;
}

PFMImage assemble_pfm_sequence(const std::vector<PFMImage>& images) {
  auto width = images[0].width;
  auto height = images[0].height;
  auto numComponents = images[0].numComponents;
  std::vector<float> image_data;
  image_data.reserve(images[0].image_data.size());
  
  for (size_t pixel_index = 0; pixel_index < width * height; ++pixel_index) {
    for (size_t channel = 0; channel < numComponents; ++channel) {
      size_t channel_index = pixel_index * numComponents + channel;

      float weighted_sum = 0;
      float weighted_exposure_sum = 0;
      float relative_exposure = 1;

      for (auto& image : images) {
        float channel_val = image.image_data[channel_index];

        if (channel_val > 0.005f && channel_val < 0.92f) {
          weighted_sum += weighting(channel_val);
          weighted_exposure_sum += std::log(channel_val / relative_exposure) * weighting(channel_val);
        }

        relative_exposure *= 4;
      }

      float combined_channel_val = std::exp(weighted_exposure_sum / weighted_sum);
      image_data.push_back(combined_channel_val);
    }
  } 
  
  return PFMImage(width, height, numComponents, image_data);
}

PPMImage gamma_exposure_tone_map(const PFMImage& image, int stops, float gamma) {
  auto width = image.width;
  auto height = image.height;
  auto numComponents = image.numComponents;
  std::vector<unsigned char> image_data;
  image_data.reserve(image.image_data.size());
  
  std::vector<float> max_channel_val(image.numComponents, 0.0f);

  for (size_t pixel_index = 0; pixel_index < width * height; ++pixel_index) {
    for (size_t channel = 0; channel < numComponents; ++channel) {
      size_t channel_index = pixel_index * numComponents + channel;
      float channel_val = image.image_data[channel_index];

      max_channel_val[channel] = std::max(max_channel_val[channel], channel_val);
    }
  }

  for (size_t pixel_index = 0; pixel_index < image.width * image.height; ++pixel_index) {
    for (size_t channel = 0; channel < image.numComponents; ++channel) {
      size_t channel_index = pixel_index * image.numComponents + channel;
      float channel_val = image.image_data[channel_index];
      float exposure = std::pow(2, stops);
      float exposed_channel_val = std::min(channel_val / max_channel_val[channel] * exposure, 1.0f);
      float gamma_channel_val = std::pow(exposed_channel_val, 1/gamma);

      image_data.push_back(static_cast<unsigned char>(gamma_channel_val * 255.0f));
    }
  }

  return PPMImage(width, height, numComponents, image_data);
}

PPMImage exposure_tone_map(const PFMImage& image, int stops) {
  return gamma_exposure_tone_map(image, stops, 1);
}

PPMImage linear_tone_map(const PFMImage& image) {
  return exposure_tone_map(image, 0);
}

// trapezium function
float weighting(float x) {
  float lower_base = 0.005f;
  float lower_ceil = 0.4f;
  float upper_ceil = 0.8f;
  float upper_base = 0.92f;

  if (x < lower_base) {
    return 0;
  } else if (lower_base <= x && x < lower_ceil) {
    return (x - lower_base) / (lower_ceil - lower_base);
  } else if (lower_ceil <= x && x < upper_ceil) {
    return 1;
  } else if (upper_ceil <= x && x < upper_base) {
    return 1 - (x - upper_ceil) / (upper_base - upper_ceil);
  } else if (upper_base <= x) {
    return 0;
  }

  std::cout << "should not reach here - inexhaustive conditions " << x << std::endl;
  return -1;
}

#endif