#include<iostream>
#include "ItkImageIO.h"
#include "dataIO.h"
#include "random.h"

#define X_SIZE 170
#define Y_SIZE 170
#define Z_SIZE 1

#define MIXTURE_RATIO 0.8
#define MEAN1 7.0
#define VAR1 5.0
#define MEAN2 -4.0
#define VAR2 3.0

typedef double feat_t;
typedef unsigned char mask_t;
typedef unsigned char label_t;

int main(int argc, char **argv) {
	
	if (argc != 2) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <output directory> "
			<< std::endl;
		return EXIT_FAILURE;
	}
	std::string output_dir = std::string(argv[1]) + "/";

	long idum = -1;

	int xe = X_SIZE;
	int ye = Y_SIZE;
	int ze = Z_SIZE;
	int se = xe*ye*ze;
	std::vector<feat_t> img(se, 0);
	std::vector<label_t> label_img(se, 0);


	// class 1
	int x1 = (int)(xe*MIXTURE_RATIO + 0.5);
	for (int y = 0; y < ye; y++) {
		for (int x = 0; x < x1; x++) {
			img.at(xe*y + x) = normal(&idum, sqrt(VAR1)) + MEAN1;
			label_img.at(xe*y + x) = 0;

		}
	}

	// class 2
	for (int y = 0; y < ye; y++) {
		for (int x = x1; x < xe; x++) {
			img.at(xe*y + x) = normal(&idum, sqrt(VAR2)) + MEAN2;
			label_img.at(xe*y + x) = 1;
		}
	}

	// Save image 
	std::string result_dir = output_dir+"org";
	make_dir(result_dir);
	save_vector(result_dir + "/debug.mhd", img, xe, ye, ze, 1., 1., 1.);

	result_dir = output_dir + "label";
	make_dir(result_dir);
	save_vector(result_dir + "/debug.mhd", label_img, xe, ye, ze, 1., 1., 1.);


	// For mask
	std::vector<mask_t> mask(se, 1);
	result_dir = output_dir + "mask";
	make_dir(result_dir);
	save_vector(result_dir + "/debug.mhd", mask, xe, ye, ze, 1., 1., 1.);

	return EXIT_SUCCESS;
}