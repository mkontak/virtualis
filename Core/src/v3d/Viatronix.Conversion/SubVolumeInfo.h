#pragma once

#include "scoped_ptr.h"

using namespace System;
using namespace System::Collections::Generic;


OPEN_CONVERSION_NS


/*
 * Sub-Volume information object used to store information for each subvolume created to
 * be later written to STDOUT
 */
public ref class SubVolumeInfo
{

protected:

  // Constructor
  SubVolumeInfo();

  // Destructor
  ~SubVolumeInfo() 
  { this->!SubVolumeInfo(); }

  // Finalizer
  !SubVolumeInfo();

public:

  /// Craetes the 
  static SubVolumeInfo ^ Create(System::IntPtr subVolumeInformationPtr);


  /// Xml representation of the object
  String ^ ToXml();

  // Gets the hash code
  virtual int GetHashCode() override
  { return Number; } 

public:

  /// Gets the image count
  property unsigned int ImageCount
  { unsigned int get(); }

  /// Gets the application type
  property String ^ Application 
  { String ^ get(); }

  /// Gets the sub-volume number (USed as an index)
  property unsigned int Number
  { unsigned int get(); }
  
  /// Gets the functional temporal flag
  property bool IsFunctionalTemporalVolume
  { bool get(); }

  /// Gets the file [ath
  property String ^ FilePath
  { String ^ get(); }

  /// Gets the sub type ( this is a string the indicates a subtype such as Contrast and Non-Contrast)
  property String ^ SubType
  { String ^ get(); }


  /// Gets the properties for this sub-volume
  property Dictionary<String ^, Dictionary<String ^, String^> ^> ^ Properties
  { Dictionary<String ^, Dictionary<String ^, String^> ^> ^ get(); }

private:

  /// Sub Volume Information
  scoped_ptr<std::shared_ptr<cvn::CvnSubVolumeInfo>> m_pSubVolumeInfo;


  /// Properties
  Dictionary<String ^, Dictionary<String ^, String^> ^> ^ m_properties;
  


};



CLOSE_CONVERSION_NS
