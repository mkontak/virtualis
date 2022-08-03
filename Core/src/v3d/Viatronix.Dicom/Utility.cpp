// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 

//
// File: Utility.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "Utility.h"
#include "Image.h"
#include "Group.h"

using namespace System;
using namespace System::Drawing::Imaging;
using namespace System::Xml;
using namespace System::Collections;

USING_DICOM_NS


/**
 * Generates a new Viatronix UID
 */
String ^ Utility::GenerateUid(unsigned int iSiteId)
{
  return gcnew String(dcm::DcmUtility::GenerateUid(iSiteId).c_str());
} // GenerateUid(unsigned in iSiteId)

/** 
 * Gets the list of elements contained within the section name specified. First the section is located in the current xml node 
 * and then all child nodes are assumed to be the elements.
 *
 * <sectionName>
 *  <element>VALUE</element>
 *  <element>VALUE</element>
 * </sectionName>
 *
 * @param mpNode          Parent node to search for the section in.
 * @param mpSectionName   Section name.
 *
 * @return List of element node values
 */
Generic::List<String ^> ^ Utility::GetSectionElements(XmlNode ^ mpNode, String ^ msSectionName)
{

  /// Section list
  Generic::List<String ^> ^ mpElements = gcnew Generic::List<String ^>();


  // Get the section node
  XmlNode ^ mpSectionNode = mpNode->SelectSingleNode(msSectionName);

  //=================================
  // Make sure the section was found
  //=================================
  if ( mpSectionNode != nullptr )
  {

    //======================================
    // For each element node build the list
    //======================================
    for each ( XmlNode ^ mpElementNode in mpSectionNode->ChildNodes )
    {
      if ( mpElementNode->HasChildNodes )
      {
        mpElements->Add(mpElementNode->ChildNodes[0]->Value->ToString());
      }
    } // END ... For each element node

  } // END ... If the section was found

  /// Return the list of elements from the section
  return mpElements;


} // GetSectionElements(XmlNode ^ mpNode, String ^ msSectionName)


/**
 * Try and get the attribute name specified and if it does not exist then return the default value
 *
 * @param mpNode          Node the look fro attribute in
 * @param msAttributeName Attribute name to look for
 * @param value           Default value to return if attribute is not present
 *
 * @return Attribute value as type T or defaultValue if not present
 */
generic<typename T>
bool Utility::TryGetAttribute(XmlNode ^ mpNode, String ^ msAttributeName, T % value)
{

  bool found(false);

  if ( mpNode->Attributes[msAttributeName] != nullptr )
  {
      value = (T)System::Convert::ChangeType(mpNode->Attributes[msAttributeName]->Value->ToString(), T::typeid);

      found = true;
  }

  return found;
} // TryGetAttribute(XmlNode ^ mpNode, String ^ msAttributeName, T defaultValue)


/**
 * Try and get the element value name specified and if it does not exist then return the default value
 *
 * @param mpNode          Node the look fro attribute in
 * @param msElementTag    Element Tag in the for XXXX,XXXX
 * @param defaultValue    Default value to return if attribute is not present
 *
 * @return Attribute value as type T or defaultValue if not present
 */
generic<typename T>
T Utility::TryGetElementValue(XmlNode ^ mpNode, String ^ msElementTag, T defaultValue)
{
  String ^ msQuery = String::Format("./element[@tag=\"{0}\"]",msElementTag);

  XmlNode ^ mpElementNode = mpNode->SelectSingleNode(msQuery);

  return ( mpElementNode != nullptr ? (T)System::Convert::ChangeType(mpElementNode->InnerText, T::typeid) : defaultValue );

} // TryGetElementValue(XmlNode ^ mpNode, String ^ msElementTag, T defaultValue)


/** 
 * Logs the status detail if its filled in
 *
 * 
 * @param statusDetailPtr     Unmanaged pointer to the status detail
 */
//void Utility::LogStatusDetail( String ^ msLogSource, IntPtr statusDetailPtr )
//{
//
//  StatusDetail pStatusDetail = reinterpret_cast<dcm::DcmImage *>(statusDetailPtr.ToPointer());
//
//  //==================================================
//  // dump status detail information if there is some
//  //=================================================
//  if (pStatusDetail != __nullptr) 
//  {
//    std::ostringstream os;
//
//    pStatusDetail->print(os);
//
//    Viatronix::Logging::Log::Debug(msLogSource, "Status Detail : " + gcnew String(os.str().c_str()), "Client", "Store" );
//
//  } // END ... If the status detail is __nullptr
//
//} // LogStatusDetail( System::IntPtr statusDetailPtr )
//


/** 
 * Adds items to the destination list with missing items from the source and removes those that 
 * are prepeneded with a '-'.
 *
 * @param mpDestination     List to be added to.
 * @param mpSource          List to be added
 */
void Utility::AddRange(Generic::List<System::String ^> ^ mpDestination, Generic::List<System::String ^> ^ mpSource )
{

  //========================================================================
  // Atempt to add each item in the array if it does not already exists
  //=======================================================================
  for each ( String ^ mpItem in mpSource )
  {
    // ===========================================================================
    // If there is a - at the beginning then attempt to remove it from the list
    // ===========================================================================
    if ( mpItem[0] == '-' )
      mpDestination->Remove(mpItem->Substring(1));
    else if ( ! mpDestination->Contains(mpItem) )
      mpDestination->Add(mpItem);
  } // END ... For each item in the array

} // AddRange(List<System::String ^> ^ mpList, array<System::String ^> ^ mpArray)

/**
	* Gets the major axis's direction as defined in the AxisDirections enumerator. Some degree of deviation from one of the standard orthogonal 
  * axes is allowed before deciding no major axis applies and return None.
	*
	* @param	x
	* @param	y
	* @param	z
  *
	* @return		Enumerated value indicating the major axis's direction or NONE if oblique
	*/
AxisDirections Utility::GetMajorAxisDirection(double x,double y,double z) 
{

  
	AxisDirections eDirection = AxisDirections::None;
		
	AxisDirections directionX = x < 0 ? AxisDirections::Right       :   AxisDirections::Left;
	AxisDirections directionY = y < 0 ? AxisDirections::Anterior    :   AxisDirections::Posterior;
	AxisDirections directionZ = z < 0 ? AxisDirections::FeetFirst   :   AxisDirections::HeadFirst;

	double absX = Math::Abs(x);
	double absY = Math::Abs(y);
	double absZ = Math::Abs(z);

 
	if ( absX > m_dObliquityThresholdCosineValue && absX > absY && absX > absZ) 
		eDirection = directionX;
	else if ( absY > m_dObliquityThresholdCosineValue && absY > absX && absY > absZ)
		eDirection = directionY;
	else if ( absZ > m_dObliquityThresholdCosineValue && absZ > absX && absZ > absY)
		eDirection = directionZ;

	return eDirection;
} // GetMajorAxisDescription(double x,double y,double z) 


/**
 * Gets the image plane based of the image orientation ( unit cosines )
 *
 * @param mpImageOrientation    Unit cosines
 *
 * @param Enumerated ImagePlanes value
 */
ImagePlanes Utility::GetImagePlane(Generic::List<double> ^ mpImageOrientation)
{
  ImagePlanes eImagePlane = ImagePlanes::Oblique;

  // =========================================================================================
  // The unit cosines has 6 values so make sure we have them all otherwise the paerameter
  // is wrong.
  // =========================================================================================
  if ( mpImageOrientation->Count >= 6 )
  {

    AxisDirections eRowAxisDirection = GetMajorAxisDirection(mpImageOrientation[0], mpImageOrientation[1], mpImageOrientation[2] );
    AxisDirections eColAxisDirection = GetMajorAxisDirection(mpImageOrientation[3], mpImageOrientation[4], mpImageOrientation[5] );

    // =======================================
    // Make sure that we are no oblique
    // =====================================
    if ( eRowAxisDirection !=  AxisDirections::None && eColAxisDirection != AxisDirections::None )
    {

      if (  ( ( eRowAxisDirection == AxisDirections::Left      || eRowAxisDirection == AxisDirections::Right ) && 
              ( eColAxisDirection == AxisDirections::Anterior  || eColAxisDirection == AxisDirections::Posterior ))  || 
            ( ( eColAxisDirection == AxisDirections::Left      || eColAxisDirection == AxisDirections::Right ) && 
              ( eRowAxisDirection == AxisDirections::Anterior  || eRowAxisDirection == AxisDirections::Posterior ) ) )
            eImagePlane = ImagePlanes::Axial;
 			else if ( ( ( eRowAxisDirection == AxisDirections::Right     || eRowAxisDirection == AxisDirections::Left ) && 
                  ( eColAxisDirection == AxisDirections::HeadFirst || eColAxisDirection == AxisDirections::FeetFirst ) ) ||
                ( ( eColAxisDirection == AxisDirections::Right     || eColAxisDirection == AxisDirections::Left ) && 
                  ( eRowAxisDirection == AxisDirections::HeadFirst || eRowAxisDirection == AxisDirections::FeetFirst ) ) )
            eImagePlane = ImagePlanes::Coronal;
		
			else if ( ( ( eRowAxisDirection == AxisDirections::Anterior  || eRowAxisDirection == AxisDirections::Posterior ) && 
                  ( eColAxisDirection == AxisDirections::HeadFirst || eColAxisDirection == AxisDirections::FeetFirst ) ) ||
                ( ( eColAxisDirection == AxisDirections::Anterior  || eColAxisDirection == AxisDirections::Posterior ) && 
                  ( eRowAxisDirection == AxisDirections::HeadFirst || eRowAxisDirection == AxisDirections::FeetFirst ) ) )
            eImagePlane = ImagePlanes::Sagittal;

    } // END .. If the direction does not indicate oblique

  } // END ... If we have the corect number of unit cosines
  
  return eImagePlane;

} // GetImagePlane(List<double> ^ mpImageOrientation)

/**
 *
 * Returns the image orientation enumerated value using the unit cosines
 * 
 * @param mpImageOrientation      Image orientation ( cosines )
 *
 * @return Image orientation enumeration value
 */
ImageOrientations Utility::GetImageOrientation(Generic::List<double> ^ mpImageOrientation)
{

  ImageOrientations orientation = ImageOrientations::None;

  // ===============================================
  // Check to make sure there are image orienations
  // ===============================================
  if ( mpImageOrientation->Count > 0 )
  {

    // ============================
    // HFP  (Head First Prone)
    //    -1\0\0  : 0\-1\0
    // =============================
    if ( mpImageOrientation[0] < 0.0 && mpImageOrientation[4] < 0.0  )
    {
      orientation = ImageOrientations::HFP;
    }
    // =============================
    // FFP (Feet First Prone)
    //    1\0\0  : 0\-1\0   
    // =============================
    else if ( mpImageOrientation[0] > 0.0 && mpImageOrientation[4] < 0.0 ) 
    {
      orientation = ImageOrientations::FFP;
    }  
    // ==========================
    // HFS (Head First Supine)
    //    1\0\0  : 0\1\0    
    // ==========================
    else if ( mpImageOrientation[0] > 0.0 && mpImageOrientation[4] > 0.0 )
    {
      orientation = ImageOrientations::HFS;
    }
    // ==========================
    // FFS (Heet First Supine)
    //    -1\0\0  : 0\1\0    
    // ==========================
    else if ( mpImageOrientation[0] < 0.0 && mpImageOrientation[4] > 0.0 )
    {
      orientation = ImageOrientations::FFS;
    }
    // =====================================
    // HFDL   (Head First Decubitus-Left)
    //    0\-1\0 : 1\0\0    
    // =====================================
    else if ( mpImageOrientation[1] < 0.0 && mpImageOrientation[3] > 0.0 ) 
    {
      orientation = ImageOrientations::HFDL;
    }
    // =====================================
    // FFDL   (Feet First Decubitus-Left)
    //    0\1\0  : 1\0\0    
    // =====================================
    else if ( mpImageOrientation[1] > 0.0 && mpImageOrientation[3] > 0.0 ) 
    {
      orientation = ImageOrientations::FFDL;
    }
    // =====================================
    // HFDR   (Head First Decubitus-Right)
    //    0\1\0  : -1\0\0       
    // =====================================
    else if ( mpImageOrientation[1] > 0.0 && mpImageOrientation[3] < 0.0 )
    {
      orientation = ImageOrientations::HFDR;
    }
    // =====================================
    // FFDR   (Feet First Decubitus-Right)
    //    0\-1\0 : -1\0\0          
    // =====================================
    else if ( mpImageOrientation[1] < 0.0 && mpImageOrientation[3] < 0.0 )
    {
      orientation = ImageOrientations::FFDR;
    }

  } // END ... If the orientations are available
 
  return orientation;

}   // GetImageOrientation(List<double> ^ mpImageOrientation)


/**
 * Converts the date string (Dicom) to a DateTime object
 *
 * @param msDate    Date string
 *
 * @return Returns the DateTime object
 */
DateTime Utility::ToDateTime(String ^ msDate, String ^ msTime)
{

  int year(0);
  int day(0);
  int month(0);
  int hour(0);
  int imin(0);
  float sec(0);

  String ^ msDateValue(msDate->Trim());


  ///////////////////////
  // Check for no date
  //////////////////////
  if ( msDateValue->Length == 0 )
  {
    throw gcnew InvalidDataException(String::Concat(L"Invalid date  [", msDateValue, "]"));;
  } // END ... If no date then assume none

  // Possible delimiters
  array<Char> ^ Dels = { ' ', '/', '-', '_', '.' };

  // Split the date into fields
  array<String ^> ^ sFields = msDateValue->Split(Dels);

  //////////////////////////////////
  // Check for the dates validity
  //////////////////////////////////
  if ( (msDateValue->Length != 8 && msDateValue->Length != 10 ) || (sFields->Length < 0 && sFields->Length > 3) )
  {
    throw gcnew InvalidDataException(String::Concat(L"Invalid date format specified [", msDateValue, "]"));
  } // END .. If the date is not valid

  try
  {
    ///////////////////////////////////////////////////////////////////
    // If the date could not be split then it is assumed to be in the 
    // format or YYYYMMDD
    ///////////////////////////////////////////////////////////////////
    if ( sFields->Length == 1 )
    {

      year = Convert::ToInt32(msDateValue->Substring(0,4));
      month = Convert::ToInt32(msDateValue->Substring(4,2));
      day = Convert::ToInt32(msDateValue->Substring(6,2));

    } // END ... If there are no delimiters
    else
    {

      /////////////////////////////////////
      // If the format is YYYY/MM/DD
      ////////////////////////////////////
      if ( sFields[0]->Length == 4 )
      {

        year = Convert::ToInt32(sFields[0]);
        month = Convert::ToInt32(sFields[1]);
        day = Convert::ToInt32(sFields[2]);

      } // END ... If the date is in the format of YYYY/MM/DD
      else
      {

        year = Convert::ToInt32(sFields[2]);
        month = Convert::ToInt32(sFields[0]);
        day = Convert::ToInt32(sFields[1]);

      } // END .. Else the date is in the format of MM/DD/YYYY
    
    } // END .. Else ther are delimiters


  }
  catch ( Exception ^ ex)
  {
    throw gcnew InvalidDataException(L"Invalid date format",ex);
  }

  //////////////////////////
  // Validate the month
  //////////////////////////
  if ( month < 1 && month > 12 )
  {
    throw gcnew InvalidDataException(L"Invalid month specified, must be be 1-12");
  } // END ... If the month is not valid

  /////////////////////////////
  // Validate the months day
  ////////////////////////////
  if ( day < 1 && m_iMonthDays[month] > 31 )
  {
    throw gcnew InvalidDataException(String::Format(L"Invalid day specified, must be between 1-{0}",m_iMonthDays[month]));
  } // END .. If the day is not valid

  /////////////////////////////
  // Validate the months day
  ////////////////////////////
  if ( year <= 1800 )
  {
    throw gcnew InvalidDataException(L"Invalid year specified, must be greater then 1800");
  } // END .. If the day is not valid


  if ( msTime->Length >= 4 )
  {

    
    try
    {
      hour = Int32::Parse(msTime->Substring(0,2));
      imin = Int32::Parse(msTime->Substring(2,2));
    }
    catch ( Exception ^ )
    {
      Viatronix::Logging::Log::Error(L"Invalid Time Format", "Utility", "ToDateTime");
    }

  }

  return DateTime(year, month, day, hour, imin, 0 );
} // ToDateTime(String ^ msDate)

/**
 * Determines if the supplied value is valid fr the element specified
 *
 * @param iGroup    Group id
 * @param iElement  Element Id
 * 
 * @return TRue if valied
 */
bool Utility::IsValid(unsigned short iGroup, unsigned short iElement, String ^ msPrivateCreator, String ^ msValue)
{
  return dcm::DcmUtility::IsValid(iGroup, iElement, ss(msPrivateCreator), ss(msValue));
} // IsValid(unisgned short iGroup. unsigned short iElement, String ^ msValue)


/**
 * Gets the scan date and time.
 *
 * @param   mpImage  The dataset.
 * @return  The scan date form the dataset.
 */
System::DateTime Utility::GetScanDate(Image ^ mpImage)
{
  if(mpImage->StudyDate->Length != 0)
  {
    return mpImage->StudyDateTime;
  }
  else if(mpImage->SeriesDate->Length > 0)
  {
    return mpImage->SeriesDateTime;
  }
  else if(mpImage->AcquisitionDate->Length > 0)
  {
    return mpImage->AcquisitionDateTime;
  }
  else
  {
    return mpImage->ImageDateTime;
  }
} // GetScanDate( mpImage )

/**
 * Gets the group description
 *
 * @param iGroup      Group id
 *
 * @return Returns the group description
 */
String ^ Utility::GetGroupDescription(unsigned short iGroup)
{
  return gcnew String(dcm::DcmDictionary::GetGroupDescription(iGroup).c_str());
}  // GetGroupDescription(unsigned short iGroup)

/**
 * Gets the group description
 *
 * @param sGroup      Group id
 *
 * @return Returns the group description
 */
String ^ Utility::GetGroupDescription(String ^ sGroup)
{
  return GetGroupDescription(dcm::DcmUtility::ToUint16(ss(sGroup)));
}  // GetGroupDescription(unsigned short iGroup)


/**
 * Gets the list of DICOM groups
 *
 * @return List of dicom groups
 */
Generic::List<Group ^> ^ Utility::GetGroups()
{

  Generic::List<Group ^> ^ mpGroups = gcnew Generic::List<Group ^>();

  dcm::DcmGroupMap groups = dcm::DcmDictionary::GetGroups();

  for( dcm::DcmGroupMap::iterator it(groups.begin()); it != groups.end(); it++ )
    mpGroups->Add(gcnew Group(it->second.GetId(), gcnew String(it->second.GetDescription().c_str())));

  return mpGroups;

} // GetGroups()