
#include <iostream>
#include <mist/mist.h>
#include <mist/filter/linear.h>
#include "ItkImageIO.h"
#include "dataIO.h"
#include "configs.h"

typedef unsigned char label_t;
typedef double atlas_t;

int main(int argc, char **argv) {
	
	if (argc != 5) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <input directory> <output directory> <filename list> <sigma>" 
				  << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "----- Read data list -----" << std::endl;
	const std::string input_dir = std::string(argv[1]) + "/";
	const std::string output_dir = std::string(argv[2]) + "/";
	const std::string filename_list_path = argv[3];
	const double sigma = std::stod(argv[4]);
	
	// https://fa11enprince.hatenablog.com/entry/2014/04/03/233500
	std::list<std::string> filename_list;
	if(!get_data_list(filename_list_path, filename_list)) {
		return EXIT_FAILURE;
	}
	const int num_cases = filename_list.size();

	std::cout << "----- Generate atlas for organs -----" << std::endl;
	std::vector<label_t> label_tmp;
	ImageIO<NDIMS> mhd;
	mhd.Read(label_tmp, input_dir + *filename_list.begin());
	const int xe = mhd.Size(0);
	const int ye = mhd.Size(1);
	const int ze = mhd.Size(2);
	const int se = xe*ye*ze;
	const double x_spacing = mhd.Spacing(0);
	const double y_spacing = mhd.Spacing(1);
	const double z_spacing = mhd.Spacing(2);

	std::cout << "----- Organ's atlas -----" << std::endl;
	for (int l = 0; l < (NUM_LABELS - 1); ++l) {
		std::cout << "---- Label num: " << l << std::endl;
		std::vector<atlas_t> atlas(se, 0);
		for (auto itr = filename_list.begin(); itr != filename_list.end(); ++itr) {

			std::vector<label_t> label_img;
			ImageIO<NDIMS> label_mhd;
			label_mhd.Read(label_img, input_dir + *itr);

			for (int z = 0; z < ze; z++)
				for (int y = 0; y < ye; y++)
					for (int x = 0; x < xe; x++)
						if (label_img.at(z * xe * ye + y * xe + x) == l+1) 
							atlas.at(z * xe * ye + y * xe + x)++;
		}

		for (int s = 0; s < se; s++) {
			atlas.at(s) /= (double)num_cases;
		}

		std::vector<atlas_t> output(se, 0);
		mist::array3<atlas_t> mistInputImg(xe, ye, ze, x_spacing, y_spacing, z_spacing, atlas.data(), se);
		mist::array3<atlas_t> mistOutputImg(xe, ye, ze, x_spacing, y_spacing, z_spacing, output.data(), se);

		mist::gaussian_filter(mistInputImg, mistOutputImg, sigma);
		std::string result_dir = output_dir + LABEL_NAMES[l];
		make_dir(result_dir);
		save_vector(result_dir + "/atlas.mhd", output, xe, ye, ze,
					x_spacing, y_spacing, z_spacing);
	}

	std::cout << " ----- Other's atlas -----" << std::endl;
	std::vector<atlas_t> other_atlas(se, 1.0);
	for (int l = 0; l < (NUM_LABELS - 1); ++l) {
		std::vector<atlas_t> atlas_tmp;
		ImageIO<NDIMS> atlas_mhd;
		atlas_mhd.Read(atlas_tmp, output_dir + LABEL_NAMES[l] + "/atlas.mhd");

		for (int z = 0; z < ze; z++) {
			for (int y = 0; y < ye; y++) {
				for (int x = 0; x < xe; x++) {
					other_atlas.at(z * xe * ye + y * xe + x) -= atlas_tmp.at(z * xe * ye + y * xe + x);
					if (other_atlas.at(z * xe * ye + y * xe + x) < 0) {
						other_atlas.at(z * xe * ye + y * xe + x) = 0;
					}
				}
			}
		}
	}
	std::string result_dir = output_dir + LABEL_NAMES[NUM_LABELS - 1];
	make_dir(result_dir);
	save_vector(result_dir + "/atlas.mhd", other_atlas, xe, ye, ze,
				x_spacing, y_spacing, z_spacing);

	return EXIT_SUCCESS;
}