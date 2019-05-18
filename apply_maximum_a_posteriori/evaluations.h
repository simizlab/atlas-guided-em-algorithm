#pragma once

#include <iostream>
#include <vector>

template<class T>
double calc_jaccard_index(std::vector<T> &predict,
	std::vector<T> &gt,
	std::vector<T> &mask,
	std::vector<size_t> &results) {
	/*
	predict: predicted vector (0 or 1)
	gt: ground truth vector (0 or 1)
	mask: mask image (0 or 1)
	results: it contains result
	[0]: cross
	[1]: join
	[2]: false positive
	[3]: false negative
	[4]: total number of pixel in mask
	[5]: number of predicted pixel
	[6]: number of ground truth pixel
	*/

	size_t se = predict.size();
	size_t total_num_pixels = 0;
	size_t predicted_num_pixel = 0;
	size_t ans_num_pixel = 0;

	size_t cross = 0;
	size_t join = 0;
	size_t fp = 0; // false positive
	size_t fn = 0; // false negative

	for (int s = 0; s < se; s++) {
		if (mask.at(s)) {
			total_num_pixels++;
			if (predict.at(s) == 1 || gt.at(s) == 1) {
				join++;
				if (predict.at(s) == gt.at(s)) {
					cross++;
					predicted_num_pixel++;
					ans_num_pixel++;
				}
				else if (predict.at(s) == 1 && gt.at(s) == 0) {
					fp++;
					predicted_num_pixel++;
				}
				else if (predict.at(s) == 0 && gt.at(s) == 1) {
					fn++;
					ans_num_pixel++;
				}
			}
		} /* Is mask 1? */
	} /* size loop */

	results.push_back(cross);
	results.push_back(join);
	results.push_back(fp);
	results.push_back(fn);
	results.push_back(total_num_pixels);
	results.push_back(predicted_num_pixel);
	results.push_back(ans_num_pixel);

	return (double)cross / (double)join;

}