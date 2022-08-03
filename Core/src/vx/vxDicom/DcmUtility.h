// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmUtility.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "DcmTriple.h"
#include "DcmEnums.h"


#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

// orward declarations
class DCM_DLL DcmImage;
class DCM_DLL DcmImageCollection;
class DCM_DLL DcmCosines;
class DCM_DLL DcmHostTable;
class DCM_DLL IDcmImage;


/**
 * Implements a group of utility methods used within the dicom  library
 */
class DCM_DLL DcmUtility
{

public:

  /// Generates a GUID as a std::string
  static std::string GenerateId();

  /// Prints the status detail to the log
  static void PrintStatusDetail(DcmDataset * pStatusDetail);

  /// Adds the child node values of the section specified to the destination
  static void AddSection( STRINGLIST & destination,  xml::XmlNode & sectionNode );

  /// Adds the range of elements in the source to the destination ( no duplicates )
  static void AddRange( STRINGLIST & destination, const std::vector<std::string> & source );

  /// Dumps the syntaxes tp the output stream
  static void DumpSyntaxes( STRINGLIST & syntaxes, std::ostream os);

  /// Gets the host name
  static std::string GetHostname();

  /// Gets the modality name using the sop uid
  static std::string GetModalityName(const std::string & sop);

  /// Splits the std::string up using the delimiter
  static STRINGLIST Split(const std::string & sValue, std::string::value_type del);

  /// Copies the source to the destination
  template<class T1>
  static void Copy(const std::vector<T1> & source, std::vector<T1> & dest )
  {
    // ==========================================
    // Add all source std::strings to the destination
    // ==========================================
    std::for_each( source.begin(), source.end(), [&dest] ( T1 item )
    {
      dest.push_back(item);
    });

  } // Copy(T1 source, T2 dest )
 
    
  /// Splits the std::string into the vector type using the supplied delimiter array
  template<class T>
  static std::vector<T> Split(const std::string & sValue, std::string::value_type del )
  {
    /// Vector
    std::vector<T> v;

    if ( ! sValue.empty() )
    {
     
      T tmpValue;
      size_t start(0);
      size_t pos = sValue.find(del, start);
      while ( pos != std::string::npos )
      {
        const int len(pos - start);
        v.push_back(Convert<T>(sValue.substr(start, len)));
        start += (len + 1);
        pos = sValue.find(del, start);
      }

      v.push_back(Convert<T>(sValue.substr(start, pos - start)));
    }
   
    return v;

  } // Split(const std::string & sValue, std::string::value_type del )

  /// Merges thge field in the list into a delimited std::string
  template<class T>
  static std::string Merge(std::vector<T> list, std::string::value_type del )
  {
    std::string sMerge("");
    std::ostringstream os;

    if ( list.size() > 0 )
    {
      for ( std::vector<T>::iterator it = list.begin(); it != list.end(); it++ )
      {
        os << (*it) << del;
      }

      sMerge = os.str();

      sMerge.erase(sMerge.end() - 1);
    }

    return sMerge;

  } // Merge(std::vector<T> list, std::string::value_type del )


  /// Convert the std::string to the type specified
  template <class T>
  static T Convert(const std::string s, T defaultValue )
  { 
    T value = defaultValue;

    std::istringstream is(s);

    is >> value;

    return value;
  } // Convert(const std::string s)

  /// Convert the std::string to the type specified

  /// Converts the string to a DcmBlockingMode
  template<class T>
  static DcmBlockingMode Convert(const std::string s, DcmBlockingMode defaulValue)
  { 

    DcmBlockingMode value = defaulValue;

    if ( s == "BLOCKING" )
      value = DCM_BLOCKING;
    else if ( s == "NONBLOCKING" )
      value = DCM_NONBLOCKING;
      
    return value;
  } // Convert(const std::string s)

  /// Converts the string to a bool
  template<class T>
  static bool Convert(const std::string s, bool defaultValue)
  { 

    bool value = defaultValue;

    if ( s == "true" )
      value = true;
    else if ( s == "false" )
      value = false;
      
    return value;
  } // Convert(const std::string s)


  /// Loads the syntaxes into the destination ( The output stream is used to write each entry to )
  static void Copy( STRINGLIST & source, const char ** dest);

  /// Gets the level from the enumerator
  static std::string GetQueryRetrievalLevel(const DcmQueryRetrievalLevels & eLevel);

  /// Gets the level from the std::string
  static DcmQueryRetrievalLevels GetQueryRetrievalLevel(const std::string & sLevel);

  /// Determines if the string value is valid for the Element 
  static bool IsValid(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator, const std::string & sValue );

  /// Generates a UID
  static std::string GenerateUid(unsigned int iSiteId = 0);

 
  /// Gets the list of files 
  static STRINGLIST GetFiles(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool recursive = false)
  { STRINGLIST files; AddDirectory(files, sDirectory, sPattern, recursive); return files; }

  /// Adds the list to a current list
  static void AddDirectory(STRINGLIST & files, const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool recursive = false);

  /// Converts a hex string to nan int
  static int HexToInt(const std::string & sHexString);

  /// Creates the Dcm Tag
  static const DcmTag CreateDcmTag(const unsigned short & iGroup, const unsigned short & iElement, const std::string & sPrivateCreator = "");

    static const DcmTag CreateDcmTag(const unsigned short & iGroup, const unsigned short & iElement, const DcmEVR & vr , const std::string & sPrivateCreator = "");


  /// Creates the Dcm Tag
  static const DcmTag CreateDcmTag( const std::string & sTag, const std::string & sPrivateCreator = "");
  static const DcmTag CreateDcmTag( const std::string & sTag, const DcmEVR & vr, const std::string & sPrivateCreator = "");


  /// Converts the hex string to a unsigned short ( XXXX is the format )
  static unsigned short ToUint16(const std::string & sHexString);


  /// Generate a message id
  static unsigned short GenerateMessageId();

  /// Dumps the move command into the file specified
  static void DumpMoveCommand( const std::string & sLogFile, DcmDataset * pDcmDataset,  T_DIMSE_C_MoveRQ * pRequest, T_DIMSE_C_MoveRSP * pResponse, DcmDataset * pResponseIds, DcmDataset * pStatusDetail );

  /// Dumps the find command into the file specified
  static void DumpFindCommand( const std::string & sLogFile, DcmDataset * pDcmDataset,  T_DIMSE_C_FindRQ * pRequest, T_DIMSE_C_FindRSP * pResponse,DcmDataset * pStatusDetail );

  /// Make sure that there are no '>' or '<' characters in the string
  static inline bool IsValidXmlString(const std::string & sValue)
  { return ( sValue.find('<') == std::string::npos && sValue.find('>') == std::string::npos ); }


  /// Determines if the value representation is a string
  static bool IsStringRepresentation(const DcmTag & dcmTag);

private:


  /// Our assigned dicom id
  static const int m_iDicomId;

  static unsigned short m_iMessageId;

}; // class DcmUtility

}; // namespace dcm

#pragma warning(pop)