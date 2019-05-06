#pragma once

#define ITK_IO_FACTORY_REGISTER_MANAGER
#include <vector>
#include <iterator>
#include <itkImage.h>
#include <itkImportImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

template<unsigned int Dim>
class ImageIO{
  typename itk::ImageRegion<Dim>::IndexType             index;
  typename itk::ImageRegion<Dim>::SizeType              size;
  typename itk::Point
    <typename itk::ImageBase<Dim>::PointValueType, Dim> origin;
  typename itk::Vector<double, Dim>                     spacing;
  typename itk::ImageIOBase::SizeType                   numOfPixels;

  template<class PixelType>
  typename itk::Image<PixelType, Dim>::Pointer ptr2img(PixelType* ptr)
  {
    typedef itk::Image<PixelType, Dim> ImageType;
    typedef itk::ImportImageFilter<PixelType, Dim> ImportFilterType;
    ImportFilterType::Pointer importFilter = ImportFilterType::New();
    importFilter->SetRegion(ImportFilterType::RegionType(index, size));
    importFilter->SetOrigin(origin);
    importFilter->SetSpacing(spacing);
    importFilter->SetImportPointer(ptr, numOfPixels, false);
    importFilter->UpdateLargestPossibleRegion();
    return importFilter->GetOutput();
  }
  
public:

  ImageIO(){}
  ImageIO(const std::string &inputFilename)
  {
    Refer(inputFilename);
  }
  ~ImageIO(){};

  unsigned int Index(unsigned int i) const { return index[i]; }
  unsigned int Size(unsigned int i) const { return size[i]; }
  double Origin(unsigned int i) const { return origin[i]; }
  double Spacing(unsigned int i) const { return spacing[i]; }

  //Set method
  void SetIndex(unsigned int i, unsigned int val) { index.SetElement(i, val); }
  void SetSize(unsigned int i, unsigned int val){ size.SetElement(i, val); }
  void SetOrigin(unsigned int i, double val){ origin.SetElement(i, val); }
  void SetSpacing(unsigned int i, double val){ spacing.SetElement(i, val); }

  unsigned int NumOfPixels(){ return numOfPixels; }

  void Refer(const std::string &referenceFilename)
  {
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::
      CreateImageIO(referenceFilename.c_str(), itk::ImageIOFactory::ReadMode);
    //std::cout << "ref: " << referenceFilename << std::endl;
    imageIO->SetFileName(referenceFilename);
    imageIO->ReadImageInformation();
    if (imageIO->GetNumberOfDimensions() != Dim){
      std::cerr << "The number of dimensions should be " << Dim << std::endl;
    }
    for (unsigned int i = 0; i != Dim; ++i){
      index.SetElement(i, 0);
      size.SetElement(i, imageIO->GetDimensions(i));
      origin.SetElement(i, imageIO->GetOrigin(i));
      spacing.SetElement(i, imageIO->GetSpacing(i));
    }
    numOfPixels = imageIO->GetImageSizeInPixels();
  }

  template<class PixelType>
  void Read(std::vector<PixelType> &Image, const std::string &inputFilename)
  {
    typedef itk::Image<PixelType, Dim> ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    Refer(inputFilename);
    Image.resize(numOfPixels);
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    reader->GraftOutput(ptr2img(Image.data()));
    try {
      reader->Update();
      std::cout << "read:  " << inputFilename << std::endl;
    }
    catch (itk::ExceptionObject e)
    {
      std::cout << e << std::endl;
    }
  }

  template<class PixelType>
  void Write(std::vector<PixelType> &Image, const std::string &outputFilename, bool UseCompression = false)
  {
    typedef itk::Image<PixelType, Dim> ImageType;
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(ptr2img(Image.data()));
    writer->SetUseCompression(UseCompression);
    try {
      writer->Update();
      std::cout << "write: " << outputFilename << std::endl;
    }
    catch (itk::ExceptionObject e)
    {
      std::cout << e << std::endl;
    }
  }

  template<class PixelType>
  typename itk::Image<PixelType, Dim>::Pointer ConvertVector2Itk(std::vector<PixelType> &Image)
  {
	  return ptr2img(Image.data());
  }

  template<class PixelType>
  void ConvertItk2Vector(typename itk::Image<PixelType, Dim>::Pointer ptr, std::vector<PixelType> &Image)
  {
	  typedef itk::Image<PixelType, Dim> ImageType;
	  Image.resize(numOfPixels);
	  itk::ImageRegionIterator<ImageType> itr(ptr, ptr->GetLargestPossibleRegion());
	  size_t i = 0;
	  for(auto it = itr.Begin(); it != itr.End(); ++it){
		  Image.at(i++) = it.Get();
	  }
  }

};