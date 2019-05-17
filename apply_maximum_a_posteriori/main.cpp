#include <iostream>
#include <mist/mist.h>
#include <Eigen/Core>
#include "ItkImageIO.h"
#include "dataIO.h"
#include "configs.h"
#include "maximum_a_posteriori.h"

typedef unsigned char mask_t;
typedef unsigned char label_t;
typedef double feat_t;
typedef double atlas_t;

int main(int argc, char **argv) {

	if (argc != 8 && argc != 9) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <input feature directory> <input abdominal cavity mask directory> "
			<< " <input param directory> <input true label directory> "
			<< " <output directory> <test filename list> <feature name list> "
			<< " <input atlas directory>"
			<< std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "----- Read data list -----" << std::endl;
	const std::string input_feat_dir = std::string(argv[1]) + "/";
	const std::string input_abd_mask_dir = std::string(argv[2]) + "/";
	const std::string input_param_dir = std::string(argv[3]) + "/";
	const std::string input_true_label_dir = std::string(argv[4]) + "/";
	const std::string output_dir = std::string(argv[5]) + "/";
	const std::string test_filename_list_path = argv[6];
	const std::string feature_name_list_path = argv[7];
	std::string input_atlas_dir;
	if (argc == 9) {
		input_atlas_dir = std::string(argv[8]) + "/";
	}

	std::list<std::string> test_filename_list;
	std::list<std::string> feature_name_list;
	if (!get_data_list(test_filename_list_path, test_filename_list) ||
		!get_data_list(feature_name_list_path, feature_name_list)) {
		return EXIT_FAILURE;
	}
	const int num_cases = test_filename_list.size();
	const int num_features = feature_name_list.size();

	std::cout << "----- Load estimated param -----" << std::endl;
	std::vector<Eigen::MatrixXd> mean(TESTTIME_NUM_LABELS, Eigen::MatrixXd(num_features, 1));
	std::vector<Eigen::MatrixXd> covariance(TESTTIME_NUM_LABELS, Eigen::MatrixXd(num_features, num_features));
	for (int l = 0; l < TESTTIME_NUM_LABELS; l++) {
		read_raw_to_eigen(mean[l], input_param_dir + TESTTIME_LABEL_NAMES[l] + "/estimated_mean_param.raw", num_features);
		read_raw_to_eigen(covariance[l], input_param_dir + TESTTIME_LABEL_NAMES[l] + "/estimated_covariance_param.raw", num_features);
	}

	std::cout << "----- Apply maximum a posteriori -----" << std::endl;
	for (auto case_itr = test_filename_list.begin(); case_itr != test_filename_list.end(); ++case_itr) {
		std::cout << "Case: " << *case_itr << std::endl;
		std::vector<mask_t> abd_mask_img;
		ImageIO<NDIMS> abd_mask_mhd;
		abd_mask_mhd.Read(abd_mask_img, input_abd_mask_dir + *case_itr);
		const int xe = abd_mask_mhd.Size(0);
		const int ye = abd_mask_mhd.Size(1);
		const int ze = abd_mask_mhd.Size(2);
		const int se = xe*ye*ze;
		const double x_spacing = abd_mask_mhd.Spacing(0);
		const double y_spacing = abd_mask_mhd.Spacing(1);
		const double z_spacing = abd_mask_mhd.Spacing(2);

		size_t num_pixel = 0;
		for (int z = 0; z < ze; z++) {
			for (int y = 0; y < ye; y++) {
				for (int x = 0; x < xe; x++) {
					if (abd_mask_img.at(z * xe * ye + y * xe + x)) num_pixel++;
				}
			}
		}

		std::cout << "---- Load feature ----" << std::endl;
		std::vector<feat_t> feature_array(num_pixel * num_features);
		for (auto feat_itr = feature_name_list.begin(); feat_itr != feature_name_list.end(); ++feat_itr) {
			std::vector<feat_t> feat_img;
			ImageIO<NDIMS> feature_mhd;
			feature_mhd.Read(feat_img, input_feat_dir + *feat_itr + "/" + *case_itr);

			size_t tmp = 0;
			size_t i = std::distance(feature_name_list.begin(), feat_itr);
			for (int z = 0; z < ze; z++) {
				for (int y = 0; y < ye; y++) {
					for (int x = 0; x < xe; x++) {
						if (abd_mask_img.at(z * xe * ye + y * xe + x)) {
							feature_array.at(i*num_pixel + tmp) = feat_img.at(z * xe * ye + y * xe + x);
							tmp++;
						}
					}
				}
			}
		}

		std::vector<feat_t> atlas_array(num_pixel * TESTTIME_NUM_LABELS);
		if (argc == 9) { // Atlas distribution
			std::cout << "---- Load atlas ----" << std::endl;
			for (int l = 0; l < TESTTIME_NUM_LABELS; l++) {
				std::vector<atlas_t> atlas_img;
				ImageIO<NDIMS> atlas_mhd;
				atlas_mhd.Read(atlas_img, input_atlas_dir + TESTTIME_LABEL_NAMES[l] + "/atlas.mhd");

				size_t tmp = 0;
				for (int z = 0; z < ze; z++) {
					for (int y = 0; y < ye; y++) {
						for (int x = 0; x < xe; x++) {
							if (abd_mask_img.at(z * xe * ye + y * xe + x)) {
								atlas_array.at(l*num_pixel + tmp) = atlas_img.at(z * xe * ye + y * xe + x);
								tmp++;
							}
						}
					}
				}
			}/* Label loop */
			// If atlas is 0, label is assigned others class
			for (int n = 0; n < num_pixel; n++) {
				double tmp = 0.;
				for (int l = 0; l < TESTTIME_NUM_LABELS; l++) {
					tmp += atlas_array.at(l*num_pixel + n);
				}
				if (tmp <= 0) {
					atlas_array.at((TESTTIME_NUM_LABELS - 1)*num_pixel + n) = 1.;
				}
			}

		}
		else {
			// Uniform distribution
			for (int s = 0; s < num_pixel*TESTTIME_NUM_LABELS; s++) {
				atlas_array.at(s) = 1.0 / (atlas_t)TESTTIME_NUM_LABELS;
			}
		}

		// Normalize atlas
		std::cout << "----- Normalize atlas probability -----" << std::endl;
		maximum_a_posteriori::normalize_probability(atlas_array, num_pixel, TESTTIME_NUM_LABELS);

		// Calc posterior probability
		std::vector<double> posterior(TESTTIME_NUM_LABELS*num_pixel);
		for (int l = 0; l < TESTTIME_NUM_LABELS; l++) {
			std::cout << "Calc poterior probability at: " << TESTTIME_LABEL_NAMES[l] << std::endl;
			atlas_t *atlas_tmp = atlas_array.data() + l*num_pixel;
			double *posterior_tmp = posterior.data() + l*num_pixel;
			maximum_a_posteriori::calculate_posterior(feature_array,
														mean[l], 
														covariance[l],
														atlas_tmp,
														num_pixel,
														num_features,
														posterior_tmp);
		} /* Label loop */
		
		 // Normalize posterior
		std::cout << "----- Normalize posterior probability -----" << std::endl;
		maximum_a_posteriori::normalize_probability(posterior, num_pixel, TESTTIME_NUM_LABELS);

		for (int l = 0; l < TESTTIME_NUM_LABELS; l++) {
			std::cout << "Save posterior: " << TESTTIME_LABEL_NAMES[l] << std::endl;
			double *posterior_tmp = posterior.data() + l*num_pixel;
			std::vector<double> posterior_img(abd_mask_img.size(), 0);

			size_t tmp = 0;
			for (int s = 0; s < se; s++) {
				if (abd_mask_img.at(s)) {
					posterior_img.at(s) = posterior.at(tmp++);
				}
			}
			std::string result_dir = output_dir + *case_itr + "/posterior/";
			make_dir(result_dir);
			save_vector(result_dir + TESTTIME_LABEL_NAMES[l] + ".mhd", 
						posterior_img, xe, ye, ze,
						x_spacing, y_spacing, z_spacing);
		} /* Label loop */
		
		std::cout << "MAP Segmentation" << std::endl;
		std::vector<label_t> label_array(num_pixel);
		maximum_a_posteriori::segmentation(posterior, num_pixel, TESTTIME_NUM_LABELS, label_array);

		std::vector<label_t> label_img(abd_mask_img.size(), 0);
		size_t tmp = 0;
		for (int s = 0; s < se; s++) {
			if (abd_mask_img.at(s)) {
				label_img.at(s) = label_array.at(tmp++);
			}
		}
		for (int s = 0; s < se; s++) {
			if (label_img.at(s) >= REMOVE_LABEL_NUM) {
				label_img.at(s) += 1;
			}
		}
		std::string result_dir = output_dir + *case_itr;
		make_dir(result_dir);
		save_vector(result_dir + "/map_segmentation.mhd",
			label_img, xe, ye, ze,
			x_spacing, y_spacing, z_spacing);

		std::cout << "Start evaluations" << std::endl;
	} /* Case loop */

	return EXIT_SUCCESS;
}