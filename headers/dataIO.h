#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <Eigen/Core>
#include <sys/stat.h>

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
					const double xspacing, const double yspacing, const double zspacing) 
{
	ImageIO<3> save_mhd;
	save_mhd.SetIndex(0, 0);
	save_mhd.SetIndex(1, 0);
	save_mhd.SetIndex(2, 0);
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

long long get_file_size(const std::string filename)
{
	/*
	�t�@�C���̃T�C�Y���擾����v���O����
	filename : �t�@�C����
	*/

	FILE *fp;
	struct _stat64 st;
	if (fopen_s(&fp, filename.c_str(), "rb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	_fstat64(_fileno(fp), &st);
	fclose(fp);
	return st.st_size;
}

template< class T >
void read_vector(std::vector<T> &v, const std::string filename)
{
	/*
	raw�摜��ǂݍ����vector�Ɋi�[
	v : �i�[����x�N�^�[
	filename : �t�@�C����
	*/
	auto num = get_file_size(filename) / sizeof(T);
	FILE *fp;
	if (fopen_s(&fp, filename.c_str(), "rb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	v.resize(num);
	fread(v.data(), sizeof(T), num, fp);

	fclose(fp);
}

template< class T >
void write_vector(std::vector<T> &v, const std::string filename)
{
	/*
	vector��raw�摜�ɏ�������
	v : �i�[����x�N�^�[
	filename : �ۑ��ꏊ��΃p�X
	*/
	FILE *fp;
	if (fopen_s(&fp, filename.c_str(), "wb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	fwrite(v.data(), sizeof(T), v.size(), fp);
	fclose(fp);
}

template<typename T>
void write_raw_and_txt(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &data, std::string filename)
{
	/*
	eigen��matrix��raw�摜�Ƃ��ĕۑ�
	��D��ŕۑ�
	�o�͂�raw�摜�ƃT�C�Y�ƌ^���L���ꂽtxt�t�@�C��
	data : �ۑ�����f�[�^
	filename : �g���q�̑O�܂ł̃p�X
	*/
	size_t rows = data.rows();
	size_t cols = data.cols();

	std::ofstream fs1(filename + ".txt");
	fs1 << "rows = " << rows << std::endl;
	fs1 << "cols = " << cols << std::endl;
	fs1 << typeid(data).name() << std::endl;
	fs1.close();

	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> Data;
	Data = data;
	std::vector<T> save_data(rows * cols);
	Data.resize(data.rows()*data.cols(), 1);
	for (size_t i = 0; i < save_data.size(); i++)
	{
		save_data[i] = Data(i, 0);
	}
	// ���������CEigenMatrix�����̃A�h���X���m�ۂ��Ă��邩�m���߂Ă��Ȃ��̂Ŋ댯�Ɣ��f
	//std::vector<T> save_data(data.data(), data.data() + data.rows()*data.cols());
	write_vector(save_data, filename + ".raw");
	Data.resize(rows, cols); //���Ƃɖ߂��H
}


template<typename T>
void read_raw_to_eigen(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& eigen, std::string filename, int row_size)
{
	/*
	* @auther tozawa
	* @history
	* 20171127
	* load_raw_to_eigen���D��ŕۑ����ꂽraw��Eigen�ɓǂݍ��ރo�[�W����
	*/
	std::vector<T> v;
	auto num = get_file_size(filename) / sizeof(T);
	FILE *fp;
	if (fopen_s(&fp, filename.c_str(), "rb") != 0) {
		std::cerr << "Cannot open file: " << filename << std::endl;
		std::abort();
	}
	v.resize(num);
	fread(v.data(), sizeof(T), num, fp);

	fclose(fp);

	size_t col_size = v.size() / row_size;

	eigen = Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>(&v[0], row_size, col_size);
}

