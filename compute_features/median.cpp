#define ITK_IO_FACTORY_REGISTER_MANAGER

#include <iostream>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMedianImageFilter.h>
#include "dataIO.h"
#include "configs.h"

typedef short input_t;
typedef double feat_t;

int main(int argc, char **argv) {
	if (argc != 5) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <input directory> <output directory> <filename list> <radius>"
					<< std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "----- Read data list -----" << std::endl;
	const std::string input_dir = std::string(argv[1]) + "/";
	const std::string output_dir = std::string(argv[2]) + "/";
	const std::string filename_list_path = argv[3];
	const double radius = std::stod(argv[4]);
	std::string result_dir = output_dir + "median/radius_" + std::_Floating_to_string("%.1f", radius) + "/";
	make_dir(result_dir);
	//result_dir.push_back('/');

	std::list<std::string> filename_list;
	if (!get_data_list(filename_list_path, filename_list)) {
		return EXIT_FAILURE;
	}
	const int num_cases = filename_list.size();

	std::cout << "----- Apply median filter -----" << std::endl;
	for (auto itr = filename_list.begin(); itr != filename_list.end(); ++itr) {
		std::cout << "Case: " << *itr << std::endl;
		using InputPixelType = input_t;
		using OutputPixelType = feat_t;
		using InputImageType = itk::Image<InputPixelType, NDIMS>;
		using OutputImageType = itk::Image<OutputPixelType, NDIMS>;

		using ReaderType = itk::ImageFileReader<InputImageType>;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(input_dir + *itr);
		reader->Update();

		// Apply median filter
		using FilterType = itk::MedianImageFilter<InputImageType, OutputImageType>;
		FilterType::Pointer filter = FilterType::New();
		FilterType::InputSizeType indexRadius;

		for (int i = 0; i < NDIMS; i++) {
			indexRadius[i] = radius;
		}
		filter->SetRadius(indexRadius);
		filter->SetInput(reader->GetOutput());

		using WriterType = itk::ImageFileWriter< OutputImageType >;
		WriterType::Pointer writer = WriterType::New();
		writer->SetInput(filter->GetOutput());
		writer->SetFileName(result_dir + *itr);
		try
		{
			writer->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			std::cerr << "Error: " << error << std::endl;
			return EXIT_FAILURE;
		}
	}/* case loop */

	return EXIT_SUCCESS;
}