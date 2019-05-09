#pragma once

#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace em_algorithm
{
	template <class ARRAY_T, class ATLAS_T>
	bool estimate_mixture_using_atlas(const std::vector<ARRAY_T> &feat_samples,
									  std::vector<Eigen::MatrixXd> &mean,
									  std::vector<Eigen::MatrixXd> &covariance,
									  const std::vector<ATLAS_T> &_atlas,
									  size_t num_samples,
									  size_t num_features,
									  size_t num_labels,
									  size_t max_iteration,
		                              double eps,
									  size_t &num_iteration)
	{
		/* 
		feat_samples: data array vector (num_features*num_samples) i.e. how to access (f+num_samples+s) 
		mean: [num_labels][(num_features, 1)]
		covariance: [num_labels][(num_features, num_features)]
		atlas: (num_labels*num_samples) i.e. how to access (l+num_samples+s)
		num_samples: Number of samples i.e. number of voxel
		num_features: Number of features e.g. median, gaussian, ... i.e. this indicate dimentions
		num_labels: Number of labels
		max_iteration: Maximum iteration
		eps: Allowable error
		num_iteration: Number of iteration on EM step
		*/
		if (num_labels == 0 || num_samples == 0 || num_features == 0) {
			std::cout << "Number error" << std::endl;
			return false;
		}
		for (int l = 0; l < num_labels; l++) {
			if (mean.size() != num_labels
				|| covariance.size() != num_labels
				|| mean[l].rows() != num_features
				|| mean[l].cols() != 1
				|| covariance[l].rows() != num_features
				|| covariance[l].cols() != num_features) {
				std::cout << "Parameter error >> " << l << std::endl;
				return false;
			}
		}
		const double pi = 3.1415926535897932384626433832795;
		const double _2pi = std::pow(2.0 * pi, (double)num_features / 2.0);
		const double _log_2pi = std::log(_2pi);
		double fLastLikelihood = -1.0e30;
		const double eps0Weight = 0;

		Eigen::ArrayXXd weight(num_samples, num_labels);
		
		std::vector<double> covariance_det_sqrt(num_labels);

		std::vector<Eigen::MatrixXd> covariance_inv(num_labels, Eigen::MatrixXd(num_features, num_features));
		std::vector<Eigen::MatrixXd> mean_old(num_labels, Eigen::MatrixXd(num_features, 1));
		std::vector<Eigen::MatrixXd> covariance_old(num_labels, Eigen::MatrixXd(num_features, num_features));

		// Normalize sum of weight of init data distribution to 1
		std::vector<ATLAS_T> atlas(_atlas);
		for (int s = 0; s < num_samples; s++) {
			double tmp = 0.;

			for (int l = 0; l < num_labels; l++) {
				tmp += atlas.at(l*num_samples + s);
			}
			if (tmp <= 0.) {
				std::cout << "Atlas error : sum " << tmp << " at " << s << std::endl;
				return false;
			}
			for (int l = 0; l < num_labels; l++) {
				weight(s, l) = atlas.at(l*num_samples + s) / tmp;
			}
		}

		// Start EM algorithm iteration
		for (num_iteration = 0; num_iteration < max_iteration; num_iteration++) {
			std::cout << "Iteration = " << num_iteration + 1 << std::endl;
			double fLikelihood = 0.0;

			std::cout << "E-step" << std::endl;
			for (int l = 0; l < num_labels; l++) {
				covariance_det_sqrt[l] = covariance[l].determinant();
				if (covariance_det_sqrt[l] <= 0.) {
					std::cout << "Det error[" << l << "] >> " 
						<< covariance_det_sqrt[l] << std::endl;
					return false;
				}
				covariance_det_sqrt[l] = sqrt(covariance_det_sqrt[l]);
				Eigen::FullPivLU<Eigen::MatrixXd> lu(covariance[l]);
				covariance_inv[l] = lu.inverse();
			}
#ifndef _OPENMP
			Eigen::MatrixXd myu = Eigen::MatrixXd::Zero(num_features, 1);;
#else
			int nThreads;
#pragma omp parallel
			{
				nThreads = omp_get_num_threads();
			}
			std::vector<Eigen::MatrixXd> myu(nThreads, Eigen::MatrixXd(num_features, 1));
#endif
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic) reduction(+ : fLikelihood)
#endif
			for (int s = 0; s < num_samples; s++) {
#ifdef _OPENMP
				int threadNum = omp_get_thread_num();
#endif
				double tmp = 0.;
				for (int l = 0; l < num_labels; l++) {
					if (weight(s, l) > eps0Weight) {
#ifndef _OPENMP
						// Calc Likelihood
						for (int f = 0; f < num_features; f++) {
							myu(f, 0) = feat_samples.at(f*num_samples + s) - mean[l](f, 0);
						}
						//std::cout << (myu.transpose() * covariance_inv[l] * myu) << std::endl;
						double maha_term = (myu.transpose() * covariance_inv[l] * myu)(0);
#else
						for (int f = 0; f < num_features; f++){
							myu[threadNum](f, 0) = feat_samples.at(f*num_samples + s) - mean[l](f, 0);
						}
						double maha_term = (myu[threadNum].transpose() * covariance_inv[l] * myu[threadNum])(0);
#endif
						double expTerm = std::exp(-1.0 * maha_term / 2.0);
						weight(s, l) *= expTerm / covariance_det_sqrt[l];
						tmp += weight(s, l);
					}
				}/* Label loop */

				// Normalize Q function
				if (tmp == 0) {
					for (int l = 0; l < num_labels; l++) {
						weight(s, l) = 0;
					}
				}
				else {
					for (int l = 0; l < num_labels; l++) {
						weight(s, l) /= tmp;
					}
					fLikelihood += std::log(tmp) - _log_2pi;
				}
			}/* Sample loop*/

			
			
			std::cout << "fLikelihood = " << fLikelihood << std::endl;
			// Is End??
			if (num_iteration != 0) {
				if (fLastLikelihood >= fLikelihood ||
					2.0 * std::abs(fLastLikelihood - fLikelihood) < eps * (std::abs(fLastLikelihood) + std::abs(fLikelihood))) {
					if (fLastLikelihood >= fLikelihood){
						mean = mean_old;
						covariance = covariance_old;
					}
					else {
						for (int l = 0; l < num_labels; l++) {
							for (int s = 0; s < num_samples; s++) {
								atlas.at(l*num_samples + s) = weight(s, l);
							}
						}
					}
					std::cout << "fLikelihood = " << fLikelihood << std::endl;
					break;
				}
			}
			else if (num_iteration == max_iteration-1) {
				break;
			}
			for (int l = 0; l < num_labels; l++) {
				for (int s = 0; s < num_samples; s++) {
					atlas.at(l*num_samples + s) = weight(s, l);
				}
			}
			fLastLikelihood = fLikelihood;

			mean_old = mean;
			covariance_old = covariance;

			std::cout << "M-step" << std::endl;;
			for (int l = 0; l < num_labels; l++) {
				double weight_sum = 0.;
				Eigen::MatrixXd ave = Eigen::MatrixXd::Zero(num_features, 1);
				Eigen::MatrixXd cov = Eigen::MatrixXd::Zero(num_features, num_features);
				for (int s = 0; s < num_samples; s++) {
					if (weight(s, l) > eps0Weight) {
						weight_sum += weight(s, l);
						for (int f = 0; f < num_features; f++) {
							ave(f, 0) += static_cast<double>(feat_samples.at(f*num_samples + s)) + weight(s, l);
						}
					}
				} /* Sample loop */

				if (weight_sum > 0.) {
					mean[l] = ave / weight_sum;
					
					for (int s = 0; s < num_samples; s++) {

						if (weight(s, l) > eps0Weight) {
							// Weighted covariance by Q func.
							for (int f = 0; f < num_features; f++) {
								for (int ff = 0; ff < num_features; ff++) {
									cov(f, ff) += weight(s, l) 
										* (feat_samples.at(f*num_samples + s) - mean[l](f, 0))
										* (feat_samples.at(ff*num_samples + s) - mean[l](ff, 0));
								}
							}
						}

					}/* Sample loop */
	
					covariance[l] = cov / weight_sum;

				}
				else {
					std::cout << "Weight error >> " << weight_sum << " " << ave(0) << std::endl;
					return false;
				}
			} /* Label loop */

		} /* EM iteration */

		return true;
	} /* Function End */
}
