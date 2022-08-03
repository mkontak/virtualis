#pragma once

/// includes
#include "Enums.h"
#include "ImageBase.h"



/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;
using namespace GalaSoft::MvvmLight;


OPEN_DICOM_NS


// Forward declarations
ref class Buffer;
ref class Element;
ref class ElementCollection;
ref class ImageCollection;


/** 
 * Implements a DICOM dataset which consists of an list of elements
 */
public ref class Frame : public ImageBase
{

#pragma region constructor/destructors

public:
  /// constructor
  Frame();


  /// Destructor
  ~Frame();

  /// Finalizer
  !Frame();


#pragma endregion


  virtual IImage ^ Clone() override;




};  // class Frame


CLOSE_DICOM_NS