#include "Stdafx.h"
#include "Utility.h"
#include "Frame.h"
#include "Element.h"
#include "Buffer.h"
#include "Sequence.h"
#include "ElementCollection.h"
#include "Element.h"
#include "ImageCollection.h"


using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

USING_DICOM_NS

/**
 * Constructor
 */
Frame::Frame() :
ImageBase()
{

} // Image()


/**
 * Destructor
 */
Frame::~Frame()
{
  this->!Frame();
} // ~Image()
  
/**
 * Finalizer
 */
Frame::!Frame()
{
 
} // !Frame

/**
 * Clones the dataset
 */
IImage ^ Frame::Clone()
{

  Frame ^ mpImage(gcnew Frame());

  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image for clone");

  mpImage->SetIDcmImagePtr(System::IntPtr(new std::shared_ptr<dcm::IDcmImage>((*m_pDcmImage)->Clone())));


  return mpImage;

} // Clone()

