// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 

//
// File: Utility.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)


#pragma once


/// Includes
#include "Enums.h"

using namespace System;
using namespace System::Collections;

OPEN_DICOM_NS

/// Forward declarations
ref class WindowLevel;
ref class Image;
ref class Group;

public ref class Utility
{

#pragma  region properties

public:

  /// Gets the LogSource for the dicom library
  static property System::String ^ LogSource
  {
    System::String ^ get() { return "Viatronix.Dicom"; }
  } // LogSource

#pragma endregion

#pragma region methods

public:

  /// Generate a new UID
  static String ^ GenerateUid(unsigned int iSiteId);
  static String ^ GenerateUid()
  { return GenerateUid(0); }

  /// Gets the image plane based on the image orientation
  static ImagePlanes GetImagePlane(Generic::List<double> ^ mpImageOrientation);

  /// Gets all elements from the section node and returns the list 
  static Generic::List<System::String ^> ^ GetSectionElements(System::Xml::XmlNode ^ mpNode, System::String ^ msSectionName);

    /// Try and get the xml attribute and returns the default if not found
  generic<typename T>
  static bool TryGetAttribute(System::Xml::XmlNode ^ mpNode, System::String ^ msAttributeName, T % value);

  /// Try and get the element value from the xml node 
  generic<typename T>
  static T TryGetElementValue(System::Xml::XmlNode ^ mpNode, System::String ^ msElementTag, T defaultValue);

  /// Logs the status detail
  //static void LogStatusDetail( System::String ^ msLogSource, System::IntPtr statusDetailPtr );


  /// Add missing items and removes items pre-prended with a -
  static void AddRange(Generic::List<System::String ^> ^ mpDestination ,Generic::List<System::String ^> ^ mpSource);

  
  /// Gets the major axis description X (Left,Right), Y(Anterior,Posterior), Z(Feet,Head)
  static AxisDirections GetMajorAxisDirection(double x,double y,double z) ;

  /// Gets the image orientation (FFS, FFP, HFS, FFS, HFDL, FFDL, HFDR, FFDR )
  static ImageOrientations GetImageOrientation(Generic::List<double> ^ mpImageOrientation) ;


  /// Returns the DateTime type using the Dicom Date and Time types
  static System::DateTime ToDateTime(String ^ msDate, String ^ msTime);

    /// Determines if the value is valid for the specified element
  static bool IsValid(unsigned short iGroup, unsigned short iElement, String ^ msPrivateCreator, String ^ msValue);

  /// Gets the scan date
  static System::DateTime GetScanDate(Image ^ mpImage);

  /// Gets the group description
  static String ^ GetGroupDescription(unsigned short iGroup);

    /// Gets the group description
  static String ^ GetGroupDescription(String ^ sGroup);

  /// Gets the DICOM groups
  static Generic::List<Group ^> ^ GetGroups();


#pragma endregion

private:

  /// Deviation of axis allowed
  static double m_dObliquityThresholdCosineValue = 0.8;

   /// Number of days in a month
  static array<int> ^ m_iMonthDays = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 

};  // class Utility


CLOSE_DICOM_NS