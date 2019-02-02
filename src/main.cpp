#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "loadPNM.h"
#include "PFMImage.h"
#include "Utils.h"

int main(int argc, char** argv) {
	std::vector<PFMImage> images;
	images.push_back(PFMImage("Office1.pfm"));
	images.push_back(PFMImage("Office2.pfm"));
	images.push_back(PFMImage("Office3.pfm"));
	images.push_back(PFMImage("Office4.pfm"));
	images.push_back(PFMImage("Office5.pfm"));
	images.push_back(PFMImage("Office6.pfm"));
	images.push_back(PFMImage("Office7.pfm"));

	for (size_t i = 0; i < images.size(); ++i) {
		auto count = count_reliable_pixels(images[i]);
		std::cout << "Image " << i << " -- ";
		std::cout << "(r: " << count.r << ") ";
		std::cout << "(g: " << count.g << ") ";
		std::cout << "(b: " << count.b << ") ";
		std::cout << "(all pixels: " << count.pixels << ") ";
		std::cout << "(all pixels% " << (float)(count.pixels) / (864 * 576) * 100 << ") " << std::endl;
	}

	auto assembled_image = assemble_pfm_sequence(images);
	assembled_image.write_to_file("result.pfm");

	auto linear_tone_mapped_image = linear_tone_map(assembled_image);
	linear_tone_mapped_image.write_to_file("result.ppm");

	int exposure_count = 6;
	for (int i = 1; i <= exposure_count; ++i) {
		auto image = exposure_tone_map(assembled_image, i);
		image.write_to_file("result_" + std::to_string(i) + ".ppm");
	}

	std::vector<float> gamma_values = {1, 1.5, 1.8, 2.2, 2.5, 3.0};

	int stops = 2;
	for (float gamma : gamma_values) {
		auto image = gamma_exposure_tone_map(assembled_image, stops, gamma);
		image.write_to_file("result_" + std::to_string(stops) + "_" + std::to_string(gamma) + ".ppm");
	}

}