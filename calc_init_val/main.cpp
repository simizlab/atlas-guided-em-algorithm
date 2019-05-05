#include <iostream>
#include <mist/mist.h>
#include "ItkImageIO.h"
#include "dataIO.h"
#include "configs.h"

typedef unsigned char label_t;
typedef double feat_t;
typedef double atlas_t;

int main(int argc, char **argv) {

	if(argc != 6) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <input feature directory> <input label directory> " 
				  << " <output directory> <filename list> <feature name list>"
			      << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "----- Read data list -----" << std::endl;
	const std::string input_feat_dir = std::string(argv[1]) + "/";
	const std::string input_label_dir = std::string(argv[2]) + "/";
	const std::string output_dir = std::string(argv[3]) + "/";
	const std::string filename_list_path = argv[4];
	const std::string feature_name_list_path = argv[5];

	std::list<std::string> filename_list;
	std::list<std::string> feature_name_list;
	if (!get_data_list(filename_list_path, filename_list) || 
		!get_data_list(feature_name_list_path, feature_name_list)) {
		return EXIT_FAILURE;
	}
	const int num_cases = filename_list.size();
	const int num_features = feature_name_list.size();

	std::cout << "----- Compute initial value for each organs -----" << std::endl;
	for (int l = 0; l < NUM_LABELS; ++l) {
		std::cout << "Label: " << LABEL_NAMES[l] << std::endl;

		for (auto case_itr = filename_list.begin(); case_itr != filename_list.end(); ++case_itr) {
			std::cout << "Case: " << *case_itr << std::endl;
			std::cout << "---- Load feature img ----" << std::endl;

			std::vector<std::vector<feat_t>> feature_img_list(num_features);
			std::vector<ImageIO<NDIMS>> feature_mhd_list(num_features);
			for (auto feat_itr = feature_name_list.begin(); feat_itr != feature_name_list.end(); ++feat_itr) {
				size_t i = std::distance(feature_name_list.begin(), feat_itr);
				feature_mhd_list[i].Read(feature_img_list[i], 
											input_feat_dir + *feat_itr + "/" + *case_itr);
			}

			std::cout << "---- Load label img ----" << std::endl;
			std::vector<label_t> label_img;
			ImageIO<NDIMS> label_mhd;
			label_mhd.Read(label_img, input_label_dir + *case_itr);
			const int xe = label_mhd.Size(0);
			const int ye = label_mhd.Size(1);
			const int ze = label_mhd.Size(2);
			const int se = xe*ye*ze;
			const double x_spacing = label_mhd.Spacing(0);
			const double y_spacing = label_mhd.Spacing(1);
			const double z_spacing = label_mhd.Spacing(2);

			if (l == NUM_LABELS - 1) {
				for (int s = 0; s < se; s++) {
					if (label_img.at(s) == REMOVE_LABEL_NUM) label_img.at(s) = 0;
					else if (!label_img.at(s)) label_img.at(s) = 1;
					else label_img.at(s) = 0;
				}
			}
			else {
				for (int s = 0; s < se; s++) {
					if (label_img.at(s) == l + 1) label_img.at(s) = 1;
					else label_img.at(s) = 0;
				}
			}




		}/* Case loop */


		std::cout << "----- Save param -----" << std::endl;

	} /* Label loop */





	return EXIT_SUCCESS;
}