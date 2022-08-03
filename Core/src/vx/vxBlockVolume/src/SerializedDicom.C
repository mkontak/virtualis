// $Id: SerializedDicom.C,v 1.7 2006/06/08 18:19:03 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Mark Kontak,   mkontak@viatronix.com
//
// Complete history at bottom of file.

#include "stdafx.h"
#include "Exception.h"
#include "SerializedDicom.h"
#include "File.h"
#include <map>
#include <vector>
#include <iostream>
#include "Triple.h"

#define FILE_REVISION "$Revision: 1.7 $"

const std::string SerializedDicom::m_sClassName("SerializedDicom");


/**
 * Constructor
 */
SerializedDicom::SerializedDicom() : m_sSerializationString("~~")
{

}

/**
 * Constructor
 */
SerializedDicom::SerializedDicom(const std::string & sSerializationString) : m_sSerializationString(sSerializationString)
{

}


/**
 *  Destructor
 */
SerializedDicom::~SerializedDicom()
{

}


SerializedDicom::PatientOrientation SerializedDicom::GetPatientOrientation()
{
  PatientOrientation eOrientation(UNKNOWN);

  std::string patientOrientation(GetFieldValue("0018", "5100", ""));

  if ( patientOrientation.size() > 0 )
  {
    switch(patientOrientation[2])
    {
    case 'S':
      eOrientation = SUPINE;
      break;
    case 'P':
      eOrientation =  PRONE;
      break;
    case 'D':
      if ( patientOrientation[3] == 'L' )
      {
        eOrientation = DECUBITUS_LEFT;
      }
      else if ( patientOrientation[3] == 'R' )
      {
        eOrientation = DECUBITUS_RIGHT;
      }
      else
      {
        eOrientation =  DECUBITUS;
      }
      break;
    default:

      std::vector< Triple<float> > cosines(GetImageOrientation());

      if ( (cosines[ROW]).Y() < 0 || (cosines[COLUMN]).Y() < 0 )
      {
        eOrientation = PRONE;
      }
      else if ( cosines[ROW].Y() > 0 || cosines[COLUMN].Y() > 0 )
      {
        eOrientation = SUPINE;
      }

      break;
    }

  }

  return ( eOrientation );

} // GetPatientOrientation()



/**
 * Gets the unit cosines or image orientation
 *
 * @return Triple containing the coodinates
 */
Triple<float> SerializedDicom::GetCoordinates()
{

  Triple<float> coordinates;
  std::vector<float> vCoordinates;

  // Get the valye first
  std::string sValue(GetFieldValue("0020", "0032", ""));

  /////////////////////////////////////////////////
  // Make sure the field is present in the header
  /////////////////////////////////////////////////
  if ( sValue.size() > 0 )
  {

    int start(0);

    while ( true )
    {
      int index(sValue.find("/",start));

      if ( index == std::string::npos )
      {
        float uc(atof(sValue.substr(start, sValue.size() - start).c_str()));
        vCoordinates.push_back(uc);

        break;
      }
      else
      {
        float uc(atof(sValue.substr(start, index - start).c_str()));
        vCoordinates.push_back(uc);
      }

      start = index + 1;
    }


  } // END ... If there is no unit cosines 

  coordinates.SetXYZ(vCoordinates[0], vCoordinates[1], vCoordinates[2]);
 
  return coordinates;

} // GetImageOrientation()


/**
 * Gets the unit cosines or image orientation
 *
 * @return Vector of cosines
 */
std::vector<Coordinate> SerializedDicom::GetImageOrientation()
{

  std::vector<Coordinate> cosines;
  std::vector<float> vCosines;

  // Get the valye first
  std::string sValue(GetFieldValue("0020", "0037", ""));

  /////////////////////////////////////////////////
  // Make sure the field is present in the header
  /////////////////////////////////////////////////
  if ( sValue.size() > 0 )
  {

    int start(0);

    while ( true )
    {
      int index(sValue.find("/",start));

      if ( index == std::string::npos )
      {
        float uc(atof(sValue.substr(start, sValue.size() - start).c_str()));
        vCosines.push_back(uc);

        break;
      }
      else
      {
        float uc(atof(sValue.substr(start, index - start).c_str()));
        vCosines.push_back(uc);
      }

      start = index + 1;
    }


  } // END ... If there is no unit cosines 


  /// ROW unit cosines
  Coordinate row;
  row.SetXYZ(vCosines[0], vCosines[1], vCosines[2]);
  cosines.push_back(row);

  /// Column unit cosines
  Coordinate column;
  column.SetXYZ(vCosines[3], vCosines[4], vCosines[5]);
  cosines.push_back(column);

  return cosines;

} // GetImageOrientation()

/**
 * Returns the field value or nothing if not present
 */
std::string SerializedDicom::GetFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sDefaultValue /* = "" */)
{
  std::string sValue(sDefaultValue);

  ///////////////////////////////////////////////
  // Make sure the elements list is not empty
  ///////////////////////////////////////////////
  if ( m_Elements.empty() )
  {
    throw ex::VException(LogRec("Object not initialized, no data elements found", m_sClassName, "GetFieldValue"));
  }

  //////////////////////////////////////////////////////
  // Make sure the gourp and field codes are filled in
  //////////////////////////////////////////////////////
  if ( sGroup.empty() || sField.empty() )
  {
    throw ex::InvalidDataException(LogRec("Invalid group or field code", m_sClassName, "GetFieldValue"));
  }


  // Build the tag
  std::string sTag(sGroup + "_" + sField);

  // Find the tag in the map
  ELEMENTS::iterator m_iter = m_Elements.find(toupper(sTag));

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Look for the tag as upper case first and if that is found then set the value otherwise look
  // for the tag in lower case.
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  if ( m_iter != m_Elements.end() )
  {
    sValue = (*m_iter).second;
  } // END ... The tag was found
  else 
  {
    // Find the tag in the map
    ELEMENTS::iterator m_iter = m_Elements.find(tolower(sTag));

    ///////////////////////////////////////////////////////
    // If the lower case tag was found return the value
    //////////////////////////////////////////////////////
    if ( m_iter != m_Elements.end() )
    {
      sValue = (*m_iter).second;
    } // END ... The tag was found

  } // END ... Else if the upper case tag was not found
  
  return(sValue);

} // GetFieldValue

/**
 * Sets the field value of of the speicifed gourp./field
 *
 * @return Returns true if the field was set false if the field does not exist.
 */
bool SerializedDicom::SetFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sValue)
{

  bool bResult(false);

  ///////////////////////////////////////////////
  // Make sure the elements list is not empty
  ///////////////////////////////////////////////
  if ( m_Elements.empty() )
  {
    throw ex::VException(LogRec("Object not initialized, no data elements found", m_sClassName, "GetFieldValue"));
  }

  //////////////////////////////////////////////////////
  // Make sure the gourp and field codes are filled in
  //////////////////////////////////////////////////////
  if ( sGroup.empty() || sField.empty() )
  {
    throw ex::InvalidDataException(LogRec("Invalid group or field code", m_sClassName, "GetFieldValue"));
  }


  // Build the tag
  std::string sTag(sGroup + "_" + sField);

  // Find the tag in the map
  ELEMENTS::iterator m_iter = m_Elements.find(toupper(sTag));

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Look for the upper case tag to see if it exists. If it does then set the value and return true. Otherwise
  // look for the lower case tag.
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ( m_iter != m_Elements.end() )
  {
    (*m_iter).second = sValue;
    bResult = true;
  } // END ... The tag was found
  else
  {
    // Find the tag in the map
    m_iter = m_Elements.find(tolower(sTag));

    /////////////////////////////////////////
    // If the tag was found return the value
    /////////////////////////////////////////
    if ( m_iter != m_Elements.end() )
    {
      (*m_iter).second = sValue;
      bResult = true;
    } // END ... The lower case tag was found

  } // END ... Else if the upper case tag was not found

  return(bResult);

} // SetFieldValue()

/**
 * Adds/Sets the value 
 *
 * @return Returns true if the field was set false if the field does not exist.
 */
void SerializedDicom::AddFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sValue)
{
  //////////////////////////////////////////////////////
  // Make sure the gourp and field codes are filled in
  //////////////////////////////////////////////////////
  if ( sGroup.empty() || sField.empty() )
  {
    throw ex::InvalidDataException(LogRec("Invalid group or field code", m_sClassName, "GetFieldValue"));
  }


  // Build the tag
  std::string sTag(sGroup + "_" + sField);

  // Find the tag in the map
  ELEMENTS::iterator m_iter = m_Elements.find(toupper(sTag));

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Look for the upper case tag to see if it exists. If it does then set the value and return true. Otherwise
  // look for the lower case tag to see if we need to set that value. If none were found we add the new tag.
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ( m_iter != m_Elements.end() )
  {
    (*m_iter).second = sValue;
  } // END ... The tag was found
  else
  {
    // Find the tag in the map
    m_iter = m_Elements.find(tolower(sTag));

    /////////////////////////////////////////
    // If the tag was found return the value
    /////////////////////////////////////////
    if ( m_iter != m_Elements.end() )
    {
      (*m_iter).second = sValue;
    } // END ... The tag was found
    else
    {
      m_Elements.insert(ELEMENTS::value_type(toupper(sTag), sValue));
    } // END ... If the tag was not found

  } // END ... the tag was not found

} // AddFieldValue()


/**
 * Converts the elements to a string for serialization
 */
std::string SerializedDicom::ToString()
{
  
  std::ostringstream os;


  if ( ! m_Elements.empty() )
  {
    ELEMENTS::iterator m_iter;
    for ( m_iter = m_Elements.begin(); m_iter != m_Elements.end(); ++m_iter )
    {
      os << toupper((*m_iter).first) + " " + SerializeField((*m_iter).second) << " ";
    }
  }
  
  return(os.str());
} // ToString()

/**
 * Initializes the elements from a input stream
 */
SerializedDicom SerializedDicom::Create(const std::string & sDicomHeader)
{
  

  std::istrstream ss(sDicomHeader.c_str());

  SerializedDicom dicom;
  ss >> dicom;

  return dicom;

} // FromString()

/**
 * Initializes the elements from a input stream
 */
std::istream & SerializedDicom::SerializeFromIStream(std::istream & is)
{

  m_Elements.clear();

  std::string sTag;
  std::string sValue;

  do
  {
    is >> sTag >> sValue;
    
    
    if ( ! sTag.empty() )
    {

      /// Make sure th tag is upper
      sTag = toupper(sTag);

      const std::string sSearch("_");
      std::string sNewTag("");
      
      int iStart(0);

      std::vector<std::string> vFields;
      for ( int4 iPos = static_cast<int4>( sTag.find(sSearch,iStart) ); iPos != std::string::npos; iStart = iPos + 1, iPos = static_cast< int4>( sTag.find(sSearch,iStart)))
      {
        vFields.push_back(sTag.substr(iStart, iPos - iStart));
      }

      vFields.push_back(sTag.substr(iStart, sTag.size() - iStart));
      
      sTag = vFields[0] + "_" + vFields[1];

      m_Elements.insert(ELEMENTS::value_type(sTag, DeSerializeField(sValue)));
    }

  } while ( ! is.eof() );

  return(is);
} // SerializeFromIStream()

/**
 * Serialize Field Value
 */
std::string SerializedDicom::SerializeField(const std::string & sFieldValue)
{

  std::string sSearch(" ");
  std::string sReplace(m_sSerializationString);

  std::string sValue(sFieldValue);

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }


  ///
  sSearch = "\r";
  sReplace = "!!";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }


  sSearch = "\n";
  sReplace = "@@";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }

  sSearch = "#";
  sReplace = "**";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }


  return(sValue);

} // SerializeField()


/**
 * DeSerialize Field Value
 */
std::string SerializedDicom::DeSerializeField(const std::string & sFieldValue)
{

  std::string sSearch(m_sSerializationString);
  std::string sReplace(" ");

  std::string sValue(sFieldValue);

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }

  sSearch = "!!";
  sReplace = "\r";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }

  sSearch = "@@";
  sReplace = "\n";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }

  sSearch = "**";
  sReplace = "#";

  while ( true )
  {
    std::string::size_type iPos = sValue.find(sSearch,0);
    if ( iPos == std::string::npos )
    {
      break;
    }

    sValue.replace(iPos,sSearch.size(),sReplace);
  }

  return(sValue);

} // DeSerializeField()



/**
 * Dump  the element list to a specified file
 */
void SerializedDicom::DumpElementList(const std::string & sLogFile)
{
  std::ofstream ofs;
  ofs.open(sLogFile.c_str());

  ELEMENTS_ITERATOR iter;

  ofs << "Group  Field" << std::endl;
  ofs << "Code   Code   Value" << std::endl;
  ofs << "=====  =====  ==========================================================================================" << std::endl;
  for ( iter = m_Elements.begin(); iter != m_Elements.end(); ++iter )
  {
    ofs << (*iter).first.substr(0,4) << "   " << (*iter).first.substr(5,4) << "   " << (*iter).second << std::endl;     
  }

  ofs.close();
} // DumpSerializedDicom()



#undef FILE_REVISION 



// $Log: SerializedDicom.C,v $
// Revision 1.7  2006/06/08 18:19:03  frank
// moved the enumerations into the class to avoid ambiguity
//
// Revision 1.6  2006/04/01 04:05:31  mkontak
// Added funtianality
//
// Revision 1.5  2006/03/30 18:57:54  mkontak
// Added GetImageOrientation() and GetCoordinates()
//
// Revision 1.4  2005/01/31 20:02:52  mkontak
// Fixed problem with upeer/lower case group and field values
//
// Revision 1.3  2005/01/31 19:55:52  mkontak
// Fixed problem with upeer/lower case group and field values
//
// Revision 1.2  2004/06/04 19:59:24  mkontak
// Fix problem with the serialization and the # causing the std::stream to get a eof
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:13:52  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13  2004/02/24 20:16:48  mkontak
// Fix for overreading the tag field
//
// Revision 1.12.4.1  2004/02/24 19:50:30  michael
// fixed bug that was due to iPos being tha maximum value find()  can return and
// iPos - iStart did thus not make sence. Using sTag.size() - iStart now.
//
// Revision 1.12  2003/05/16 13:15:17  frank
// formatting
//
// Revision 1.11  2003/05/16 11:44:21  michael
// code review
//
// Revision 1.10  2003/03/25 21:44:11  mkontak
// Fixed serialization problem
//
// Revision 1.9  2003/01/29 15:37:56  mkontak
// Support newer version of the serialized dicom that contains the value type in the tag field
//
// Revision 1.8  2002/11/07 22:36:28  michael
// fixed return value issue
//
// Revision 1.7  2002/11/07 21:42:29  mkontak
// Added ASddFieldValue()
//
// Revision 1.6  2002/09/11 21:46:23  jmeade
// Id header.
//
// Revision 1.5  2002/05/20 03:52:14  mkontak
// Added SetFieldVlaue() so that anonymization can be done.
//
// Revision 1.4  2002/04/30 14:14:53  mkontak
// Moved constructor and destrcutor into .C
//
// Revision 1.3  2002/04/26 20:43:36  mkontak
// *** empty log message ***
//
// Revision 1.2  2002/04/26 20:28:15  mkontak
// Modified constructors
//
// Revision 1.1  2002/04/26 20:03:25  mkontak
// Again a name change, but only the file names were missspelled.
//
// Revision 1.1  2002/04/26 19:44:23  mkontak
// Changed class from DicomHeader to SerializedDicom
//
// Revision 1.2  2002/04/26 18:10:42  mkontak
// Fixed compiling
//
// Revision 1.1  2002/04/26 18:05:14  mkontak
// Initial Check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/SerializedDicom.C,v 1.7 2006/06/08 18:19:03 frank Exp $
// $Id: SerializedDicom.C,v 1.7 2006/06/08 18:19:03 frank Exp $
