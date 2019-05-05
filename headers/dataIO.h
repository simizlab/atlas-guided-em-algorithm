#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include "ItkImageIO.h"

namespace fs = boost::filesystem;

bool get_data_list(std::string list_path, std::list<std::string> &filename_list) {
	/*
	*  @list_path: input path to file that contained file list 
	*  @filename_list: data list that contained file list
	*/
	std::string filename_tmp;
	std::ifstream ifs(list_path);
	if (ifs.fail()) {
		std::cerr << "Cannot open file: " << list_path << std::endl;
		return false;
	}
	while (getline(ifs, filename_tmp)) {
		filename_list.push_back(filename_tmp);
	}
	ifs.close();

	return true;
}

bool make_dir(std::string dir_path) {
	/*
	* https://boostjp.github.io/tips/filesystem.html
	*/
	const fs::path path(dir_path);
	boost::system::error_code error;
	const bool result = fs::create_directories(path, error);
	if (!result || error) {
		return false;
	}
	return true;
}

template<class T>
void save_vector(const std::string path, std::vector<T> img, 
					const int xsize, const int ysize, const int zsize, 
					const double xspacing, const double yspacing, const double zspacing) {
	ImageIO<3> save_mhd;
	save_mhd.SetSize(0, xsize);
	save_mhd.SetSize(1, ysize);
	save_mhd.SetSize(2, zsize);
	save_mhd.SetOrigin(0, 0.);
	save_mhd.SetOrigin(1, 0.);
	save_mhd.SetOrigin(2, 0.);

	save_mhd.SetSpacing(0, xspacing);
	save_mhd.SetSpacing(1, yspacing);
	save_mhd.SetSpacing(2, zspacing);
	save_mhd.Write(img, path);
	return;
}


