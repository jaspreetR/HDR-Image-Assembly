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

	auto assembled_image = assemble_pfm_sequence(images);
	assembled_image.write_to_file("result.pfm");

	auto linear_tone_mapped_image = linear_tone_map(assembled_image);
	linear_tone_mapped_image.write_to_file("result.ppm");

	std::vector<float> gamma_values = {1, 1.5, 1.8, 2.2, 2.5, 3.0};

	int exposed_image_count = 7;
	for (int i = 1; i <= exposed_image_count; ++i) {
		for (float gamma : gamma_values) {
			auto image = gamma_exposure_tone_map(assembled_image, i, gamma);
			image.write_to_file("result_" + std::to_string(i) + "_" + std::to_string(gamma) + ".ppm");
		}
	}

}