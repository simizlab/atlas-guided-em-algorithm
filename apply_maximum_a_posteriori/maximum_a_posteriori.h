#pragma once
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif


namespace maximum_a_posteriori
{
	template <class ARRAY_T>
	void normalize_probability( std::vector<ARRAY_T> &probability_vector, 
								const size_t num_samples, 
								const size_t num_labels) {
		/*
		probability_vector: data array vector (num_features*num_samples) i.e. how to access (f+num_samples+s) 
		num_samples: Number of samples i.e. number of voxel
		num_labels: Number of labels
		*/
		for (int s = 0; s < num_samples; s++) {
			double tmp = 0.;
			for (int l = 0; l < num_labels; l++) {
				tmp += probability_vector.at(l*num_samples + s);
			}
			if (tmp > 0) {
				for (int l = 0; l < num_labels; l++) {
					probability_vector.at(l*num_samples + s) /= tmp;
				}
			}
		} /* Sample loop */
	}

	template<class ARRAY_T, class MATRIX_T, class ATRAS_T>
	bool calculate_posterior(const std::vector<ARRAY_T> &feat_samples,
								Eigen::Matrix<MATRIX_T, Eigen::Dynamic, Eigen::Dynamic> &mean,
								Eigen::Matrix<MATRIX_T, Eigen::Dynamic, Eigen::Dynamic> &covariance,
								const ATRAS_T *atlas,
								const size_t num_samples,
								const size_t num_features,
								double *posterior) {

		const double eps0Weight = 0;
		double covariance_det_sqrt;
		Eigen::MatrixXd covariance_inv(num_features, num_features);

		// Calc Inverse matrix and Determinant of covariance
		covariance_det_sqrt = covariance.determinant();
		if (covariance_det_sqrt <= 0.) {
			std::cout << "Det error >> "
				<< covariance_det_sqrt << std::endl;
			return false;
		}
		covariance_det_sqrt = sqrt(covariance_det_sqrt);
		Eigen::FullPivLU<Eigen::MatrixXd> lu(covariance);
		covariance_inv = lu.inverse();

#ifdef _OPENMP
#pragma omp parallel for
#endif
		// Calc posteriori
		for (int s = 0; s < num_samples; s++) {
			Eigen::MatrixXd myu(num_features, 1);
			if (atlas[s] > eps0Weight) {

				for (int f = 0; f < num_features; f++) {
					myu(f, 0) = feat_samples.at(f*num_samples + s) - mean(f, 0);
				}
				double maha_term = (myu.transpose() * covariance_inv * myu)(0);
				double exp_term = std::exp(-1. * maha_term / 2.);
				posterior[s] = atlas[s] * exp_term / covariance_det_sqrt;
			}
			else {
				posterior[s] = 0;
			}
		} /* Sample loop */


		return true;

	} /* Func calculate_posterior */

	template<class ARRAY_T, class LABEL_T>
	void segmentation(const std::vector<ARRAY_T> &posterior,
						const size_t num_samples,
						const size_t num_labels,
						std::vector<LABEL_T> &label ) {

		for (int s = 0; s < num_samples; s++) {
			double max_posterior = 0.;
			for (int l = 0; l < num_labels; l++) {
				if (posterior.at(l*num_samples + s)>max_posterior) {
					max_posterior = posterior.at(l*num_samples + s);
					label.at(s) = l + 1;
				}
			}
			if (max_posterior == 0) {
				label.at(s) = num_labels;
			}
		}
	}

} /* Namespace maximum_a_posteriori */