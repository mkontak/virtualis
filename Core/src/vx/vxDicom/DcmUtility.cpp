// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmUtility.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#include "Stdafx.h"
#include "DcmUtility.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmCosines.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmName.h"
#include "DcmDictionary.h"
#include "DcmImage.h"
#include "DcmImageCollection.h"
#include "DcmElement.h"
#include "DcmElementCollection.h"
#include "DcmSequence.h"
#include "DcmImageFactory.h"


using namespace dcm;


// Sort List ( UID,  array ( GROUP_TAG, ELEMENT_TAG ) )
typedef  std::pair<const DcmTag,const DcmTag> SORTITEM;
typedef  std::vector<SORTITEM> SORTITEM_LIST;
typedef  std::map<std::string, SORTITEM_LIST> SORTMAP;


/// Our Dicom ID Code
const int DcmUtility::m_iDicomId(114174);
unsigned short DcmUtility::m_iMessageId(0);

/**
 * Gets an ID from a generated guid
 *
 *  @return the string of id
 */
std::string DcmUtility::GenerateId()
{
  GUID guid;
  CoCreateGuid( &guid );

  LPOLESTR  lpOStr;

  HRESULT hr = StringFromCLSID(guid, &lpOStr);

  std::string sGuid = std::string((char *) (_bstr_t)lpOStr);

  return sGuid.substr(1,sGuid.length() - 2);

} // GetId() 


/**
 * Print the status detail 
 *
 *  @param pStatusDetail
 */
void DcmUtility::PrintStatusDetail(DcmDataset * pStatusDetail)
{

  if ( pStatusDetail != __nullptr )
  {
    std::ostringstream os;

    os << "Status Detail: ";
  
    pStatusDetail->print(os);

    DcmLog::Debug(os.str(), "DcmUtility", "PrintStatusDetail");
  

    delete pStatusDetail;
  } // END ... If there is detail

} //  PrintStatusDetail(DcmDataset * pStatusDetail)


/** 
 * Adds items to the destination list with missing items from the source and removes those that 
 * are pre-pended with a '-'.
 *
 * @param destination     Destination vector
 * @param source          Source vector
 */
void DcmUtility::AddSection( STRINGLIST & destination, xml::XmlNode & sectionNode)
{
  // ==============================================
  // Make sure that the section node is not null
  // ============================================
  if ( sectionNode.IsValid()  )
  {

    try
    {
      STRINGLIST source;

      /// Get the list of child node values
      for ( xml::XmlNode child = sectionNode.GetFirstChild(); child.IsValid();  child = sectionNode.GetNextChild() )
      {
        // ===========================================================
        // Check for children and if the node is an element node
        // ==========================================================
        if ( child.HasChildren() && child.GetType() == xml::XML_ELEMENT_NODE )
          source.push_back(child.GetFirstChild().GetContent());
       
      } // END ... For each child

      AddRange(destination,source);

    }
    catch ( DcmException & ex )
    {
      DcmLog::Error("vxDicom", util::Strings::Format("Failed to add section to destination : %s", ex.GetTextMessage().c_str() ), "DcmUtility","AddSection");

      throw;
    }
    catch ( ... )
    { 
      DcmLog::Error("vxDicom", "Failed to add section to destination", "DcmUtility","AddSection");

      throw DcmException(LogRec("Failed to add section to destination", "DcmUtility","AddSection")); 

    }

  } // END .. If then section node is not __nullptr

} // AddSection()


/** 
 * Adds items to the destination list with missing items from the source and removes those that 
 * are prepeneded with a '-'.
 *
 * @param destination     Destination vector
 * @param source          Source vector
 */
void DcmUtility::AddRange( STRINGLIST & destination, const STRINGLIST & source )
{

  try
  {
 
    std::for_each( source.begin(), source.end(), [&destination] ( std::string item ) 
    {
      // ===========================================================================
      // If there is a - at the beginning then attempt to remove it from the list
      // ===========================================================================
      if ( item[0] == '-' )
      {
        util::Strings::Remove(destination, item);
      }
      else if ( ! util::Strings::Contains(destination,item) )
        destination.push_back(item);

    });

   }
  catch( ... )
  { 
    DcmLog::Error("vxDicom", "Failed to add source to destination", "DcmUtility","AddRange");

    throw DcmException(LogRec("Failed to add source to destination", "DcmUtility","AddRange")); 
  }

} // AddRange( STRINGLIST & destination, const STRINGLIST & source )



/**
 * Dumps the syntaxes into the output stream
 */
void DcmUtility::DumpSyntaxes( STRINGLIST & syntaxes, std::ostream os)
{

  /// indexer
  int i = 0;

  //======================================
  // Build the transfers syntaxes array
  // =====================================
  for ( STRINGLIST::iterator it = syntaxes.begin(); it < syntaxes.end(); it++, i++ )
  {
    os << (*it) << std::endl;
  } //  END ... For each syntax 

} // DumpSyntaxes(STRINGLIST & syntaxes, ostream os)



/**
 * Gets the host name
 *
 *  @return the string of hostname
 */
std::string DcmUtility::GetHostname()
{

  char buffer[512];

  if ( gethostname(buffer,512) != 0 )
  {
    return std::string("");
  }

  return std::string(buffer);

} // GetHostname()


/**
 * Gets the modality name
 *
 * @param sopCalss    SOP class uid
 * @return the string of the modality
 */
std::string DcmUtility::GetModalityName(const std::string & sop)
{

  const char *modalityName = dcmSOPClassUIDToModality(sop.c_str());

  if ( modalityName == __nullptr ) 
    modalityName = dcmFindNameOfUID(sop.c_str());

  if ( modalityName == __nullptr ) 
    modalityName = "unknown SOP class";

  return std::string(modalityName);

} // GetModalityName(const DIC_UI & sop)


/**
 * Splits the std::string up into a vector of std::string using the del
 *
 * @param sValue      Value to be split
 * @param del         Delimiter character
 * @return the string list
 */
STRINGLIST DcmUtility::Split(const std::string & sValue, std::string::value_type del)
{
  STRINGLIST fields;

  if ( ! sValue.empty() )
  {
    size_t start(0);
    size_t pos = sValue.find(del, start);
    while ( pos != std::string::npos )
    {
      const size_t len(pos - start);
      fields.push_back(sValue.substr(start, len));
      start += (len + 1);
      pos = sValue.find(del, start);
    }
    fields.push_back(sValue.substr(start, pos - start));
  }
  else
    fields.push_back(std::string(""));

  return fields;

} // Split(const std::string & sStr, std::string::value_type ch)


/**
 * Converts the corresponding pointer array from the STRINGLIST. 
 * It is the responsibility of the caller to clean up the memory.
 *
 *  @param source
 *  @param dest
 */
void DcmUtility::Copy( STRINGLIST & source, const char ** dest)
{

  /// indexer
  int i = 0;

  //======================================
  // Build the transfers syntaxes array
  // =====================================
  for ( STRINGLIST::iterator it = source.begin(); it < source.end(); it++, i++ )
  {
    const char * str = (*it).c_str();
    dest[i] = str;

  } //  END ... For each str
 
} // Create( STRINGLIST & source, const char ** dest)


/**
 * Gets the level from the enumerator
 *
 * @param eLevel    QR Level as an enum
 *
 * @return std::string representation for the enum
 */
 std::string DcmUtility::GetQueryRetrievalLevel(const DcmQueryRetrievalLevels & eLevel)
 {
   std::string sLevel("");

   switch ( eLevel )
   {
   case L_PATIENT:  
     sLevel = "PATIENT";
     break;
   case L_STUDY:
     sLevel = "STUDY";
     break;
   case L_SERIES:
     sLevel = "SERIES";
     break;
   case L_IMAGE:
     sLevel = "IMAGE";
     break;
   };  // END ... swicth

   return sLevel;
 }  // GetQueryRetrievalLevel(const DcmQueryRetrievalLevels & eLevel)


  /** 
   * Gets the level from the std::string
   * 
   * @param sLevel    QR Level as a std::string
   *
   * @return QR Level Enumerator
   */
  DcmQueryRetrievalLevels DcmUtility::GetQueryRetrievalLevel(const std::string & sLevel)
  {
    if ( sLevel == "PATIENT" )
      return L_PATIENT;
    else if ( sLevel == "STUDY" )
      return L_STUDY;
    else if ( sLevel == "SETRIES" )
      return L_SERIES;
    else if ( sLevel == "IMAGE" )
      return L_IMAGE;
    else
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid level std::string %s",sLevel), "DcmUtility", "GetQueryRetrievalLevel"));
  } // GetQueryRetrievalLevel(const std::string & sLevel)

  
  /**
   * Determines if the value supplied in the std::string is valid for the element requested gy the group/element pair
   * 
   * @param iGroup      Group id for the element
   * @param iElement    Element is for the element
   * @param sValue      Value to be validated
   *
   * @return True is value, false otherwise
   */
  bool DcmUtility::IsValid(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator, const std::string & sValue)
  {

    bool bValid(true);

    if ( ! sValue.empty() )
    {

      DcmTagKey tagKey(iGroup, iElement);

      /// Locks the data dictaion form reading
      const DcmDataDictionary & dict = dcmDataDict.rdlock();

      DcmDictEntry * pEntry(__nullptr);

      if ( ! DcmDictionary::TryGetEntry(DcmUtility::CreateDcmTag(iGroup, iElement, sPrivateCreator), &pEntry) )
      {
        DcmLog::Error(util::Strings::Format("Failed to get dictionary entry for (%04x,%04x)", iGroup, iElement), "DcmUtility", "IsValid");
      }
      else 
      {
        DcmEVR vr = pEntry->getEVR();

        try
        {

          // =====================================
          // Swicth of the Value Representation
          // =====================================
          switch ( vr )
          {

          // Application entity (16 characters)
          case DcmEVR::EVR_AE:  
            {
              if ( sValue.length() > 16 ) throw;
            }
            break;

          // Date value (Use dcm::DcmDate to determine validity)
          case DcmEVR::EVR_DA:    
            {
              dcm::DcmDate date(sValue);
            }
            break;

          // Time value (Use dcm::DcmTime to determine validity)
          case DcmEVR::EVR_TM:    
            {
              dcm::DcmTime time(sValue);
            }
            break;

          // Float value (Use Convert<> to make sure is can be converted)
          case DcmEVR::EVR_FL:   
            {
              float fValue(Convert<float>(sValue, 0.0F));
            }
            break;

          // Decimal std::string (Max size is 16) allow it to pass through to FD to check the conversion
          case DcmEVR::EVR_DS:    // Decimal std::string
            {
              if ( sValue.length() > 16 ) throw;
            }
            // Dont break beacause we want to check that the value can be converted

          // Double value (Use Convert<> to make sure is can be converted)
          case DcmEVR::EVR_FD:    
            {
              double dValue(Convert<double>(sValue, 0.0F ));
            }
            break;
        
          // Integer std::string ( Max size is 12) allow it to pass through to SL to check the conversion
          case DcmEVR::EVR_IS:    // Integer std::string
            {
              // Max size os 12 characters
              if ( sValue.length() > 12 ) throw;
            }
            // Don't break because we want to check that the value can be converted 

          // Double value (Use Convert<> to make sure is can be converted)
          case DcmEVR::EVR_SL:    
            {
              int iValue(Convert<int>(sValue, 0));
            }
            break;

          // Signed short (Use COnvert<> to check validity)
          case DcmEVR::EVR_SS:    
            {
              short Value(Convert<short>(sValue, 0));
            }
            break;

          // Unsigned int (Use COnvert<> to check validity)
          case DcmEVR::EVR_UL:    // Unsigned int
            {
              unsigned int Value(Convert<unsigned int>(sValue, 0));
            }
            break;

          // Person name (Use the DcmName to validate the value)
          case DcmEVR::EVR_PN:    // Person Name
            {
              DcmName name(sValue);
            }
            break;


          // Catch all for unsupported checks (Assume invalid)
          default:
          
            throw;

          };  // END ... Switch

        }
        catch ( ... )
        {
          bValid = false;
        }

      
      } // END ... If the dictionary entry was found

    } // END ... If the value is not empty

    return bValid;

  } // IsValid(unsigned short iGroup, unsigned short iElement, const std::strig & sValue)
 

  /**
   * Generates a new UID
   *
   * @param iSite  
	 * @return the string of the uid
   */
  std::string DcmUtility::GenerateUid(unsigned int iSite)
  {
    static __int64 iCount(0);
    SYSTEMTIME sys;
    ::GetSystemTime(&sys);


    if ( iSite == 0 )
    {
      /// Attempt to get the VXSITE environment variable 
      char buffer[512];
      DWORD hresult = ::GetEnvironmentVariable("VXSITE",buffer,512);
  
      try
      {
        // Set the the site to 0 if not found or convert the variable which should be an integer
        iSite = (hresult == 0 ? 0 : atoi(buffer));
      }
      catch ( ... )
      {
        DcmLog::Error(util::Strings::Format("Invalid VXSITE (%s), assuming 0", buffer), "DcmUtility", "GenerateUID");
        // Ignore Failure to convert
      }
    } // END ... If the site was not supplied
    

    return util::Strings::Format("1.3.%d.%d.%04d.%02d.%02d.%02d.%02d.%02d.%d.%ld.%ld", m_iDicomId, iSite, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetTickCount(), ++iCount);
 } // GenerateUid()


 
/**
 * Gets the list of files
 *
 * @param files         List to add to
 * @param sDirectory    Directory to search
 * @param sPattern      File pattern ( *.dcm is default )
 * @param recursive     Recursive flag
 *
 */
void DcmUtility::AddDirectory(STRINGLIST & files, const std::string & sDirectory, const std::string & sPattern, bool recursive)
{

  if ( _access(sDirectory.c_str(), 4) != 0 )
    throw DcmIOException(LogRec(util::Strings::Format("Cannot read directory specified %s",sDirectory.c_str()), "DcmUtility", "GetFiles"));

  std::string sDicomFiles = sDirectory + "\\" + ( sPattern.empty() ? "*.dcm" : sPattern );
  WIN32_FIND_DATA fileData;

  std::vector<std::string> directories;

  HANDLE handle = ::FindFirstFile(sDicomFiles.c_str(), &fileData);

  
  if ( handle != INVALID_HANDLE_VALUE )
  {

    do 
    {

      if ( fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        files.push_back(sDirectory + "\\" + std::string(fileData.cFileName));
      else
        directories.push_back(sDirectory + "\\" + std::string(fileData.cFileName));
     
    } while ( ::FindNextFile(handle, &fileData ) );

    FindClose(handle);

    if ( recursive )
    {

      for ( std::vector<std::string>::iterator it = directories.begin(); it != directories.end(); it++ )
      {
        AddDirectory(files, sDirectory + "\\" + (*it), sPattern, recursive); 
      }
    }
   
  } // if
  
} // AddDirectory(STRINGLIST & files, const std::string & sDirectory, const std::string & sPattern, bool recursive)


/**
 * Converts a Hex string to integer
 *
 * @param sString
 * @return integer
 */
int DcmUtility::HexToInt(const std::string & sHexString)
{
  struct CHexMap
  {
    char chr;
    int value;
  };

  
  std::string sUpper(toupper(sHexString));

  // std::string sUpper(sHexString.size(), ' ');
  //std::transform(sHexString.begin(), sHexString.end(), sUpper.begin(), ::toupper); 

  const int iHexMapLength(16);
  CHexMap HexMap[iHexMapLength] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  
  char * pData = const_cast<char *>(sUpper.c_str());
  int result = 0;
  if (*pData == '0' && *(pData + 1) == 'X') pData += 2;
  bool firsttime = true;
  while (*pData != '\0')
  {
    bool found = false;
    for (int i = 0; i < iHexMapLength; i++)
    {
      if (*pData == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    pData++;
    firsttime = false;
  }
  
  return result;
} // HexToInt


/**
 * Creates the DcmTag
 *
 *  @param iGroup
 *  @param iElement
 *  @param sPrivaterCreator
 *  @param sVR
 *  @return the tag
 */
const DcmTag DcmUtility::CreateDcmTag(const unsigned short & iGroup, const unsigned short & iElement, const std::string & sPrivateCreator)
{
  DcmTag tag(iGroup,iElement);

  if ( ! sPrivateCreator.empty() )
  {
    tag.setPrivateCreator(sPrivateCreator.c_str());
  }

  return tag;
  
} // CreateDcmTag(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator = "", const std::string & sVR = "" )


/**
 * Creates the DcmTag
 *
 *  @param iGroup
 *  @param iElement
 *  @param sPrivaterCreator
 *  @param sVR
 *  @return the tag
 */
const DcmTag DcmUtility::CreateDcmTag(const unsigned short & iGroup, const unsigned short & iElement, const DcmEVR & vr, const std::string & sPrivateCreator )
{
  DcmTag tag(iGroup,iElement);

  if ( ! sPrivateCreator.empty() )
  {
    tag.setPrivateCreator(sPrivateCreator.c_str());

  }

  tag.setVR(vr);

  return tag;
  
} // CreateDcmTag(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator = "", const std::string & sVR = "" )

/**
 * Creates the DcmTag
 *
 *  @param sTag               Tag as a string format 0008,0008 or (0008,0008)
 *  @param sPrivaterCreator   Private creator
 *  @param sVR                Value representation
 *  @return the tag
 */
const DcmTag DcmUtility::CreateDcmTag(const std::string & sTag, const std::string & sPrivateCreator)
{
  std::string sTrimmedTag(sTag);

  sTrimmedTag = trim_right(sTrimmedTag,")");
  sTrimmedTag = trim_left(sTrimmedTag,"(");

  std::vector<std::string> fields = util::Strings::Split(sTrimmedTag, ',');


  DcmTag tag = DcmTag(0,0);

  if ( fields.size() == 2 )
    tag = DcmTag(ToUint16(fields[0]),ToUint16(fields[1]));

 
  return tag;
  
} // CreateDcmTag(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator = "", const std::string & sVR = "" )

/**
 * Creates the DcmTag
 *
 *  @param sTag               Tag as a string format 0008,0008 or (0008,0008)
 *  @param sPrivaterCreator   Private creator
 *  @param sVR                Value representation
 *  @return the tag
 */
const DcmTag DcmUtility::CreateDcmTag(const std::string & sTag, const DcmEVR & vr, const std::string & sPrivateCreator)
{
  std::string sTrimmedTag(sTag);

  sTrimmedTag = trim_right(sTrimmedTag,")");
  sTrimmedTag = trim_left(sTrimmedTag,"(");

  std::vector<std::string> fields = util::Strings::Split(sTrimmedTag, ',');

  DcmTag tag = DcmTag(0,0);

  if ( fields.size() == 2 )
    tag = DcmTag(ToUint16(fields[0]),ToUint16(fields[1]));

  tag.setVR(vr);

  return tag;
} // CreateDcmTag(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator = "", const std::string & sVR = "" )

/**
 * Converts the Hex string supplied to an unsigned short
 *
 * @param sHexString      Hex string to convert
 *
 * @return Returns the unsigned short representation of the string
 */
unsigned short DcmUtility::ToUint16(const std::string & sHexString)
{

  if ( sHexString.size() != 4 )
    throw DcmInvalidDataException(LogRec("Invalid hex string, must have a length of 4 character","DcmUtility","ToUint16"));

  unsigned short iValue(0);

  int iShift(12);
  for ( int i(0); i < 4; iShift -= 4, ++i )
  {
    unsigned int ich((unsigned int)sHexString[i]);

    if ( ich >= 48 && ich <= 57 )
      ich -= 48;
    else if ( ich >= 65 && ich <= 70 )
      ich -= 55;
    else if ( ich >= 97 && ich <= 102 )
      ich -= 87;

    iValue += ich << iShift;
  }

  return iValue;
} // ToUint16(const std::string & sHexString)


/**
 * Generates a message id
 *
 *  @return the id
 */
unsigned short DcmUtility::GenerateMessageId()
{

  if ( m_iMessageId == 0 )
  {
    // Seed the with the time so it is different each time
    srand( (unsigned)time( NULL ) );

    m_iMessageId = (unsigned short)rand();
  }

  return ++m_iMessageId;

} // GenerateMessageId()


/**
 * Dumps the move command and all results into the file specified
 *
 * @param sLogFile        Log file
 * @Param pDcmDataset     Query
 * @param pRequest        MOve request
 * @param pResponse       Move response
 * @param pResponseIds    Response Ids
 * @param pStatusDetail   Status details
 */
void DcmUtility::DumpMoveCommand( const std::string & sLogFile, DcmDataset * pDcmDataset,  T_DIMSE_C_MoveRQ * pRequest, T_DIMSE_C_MoveRSP * pResponse, DcmDataset * pResponseIds, DcmDataset * pStatusDetail )
{

  if ( ! sLogFile.empty() )
  {

    FILE * fp(fopen(sLogFile.c_str(), "a"));

    if ( fp == NULL  )
    {
      LogErr(util::Strings::Format("Failed to open log file %s",sLogFile.c_str()), "DcmUtility","DumpMoveCommand");
      return;
    }

    fprintf(fp,"============================================================================================================================================================\r\n");
    fprintf(fp,"Move Command    %s-%s\r\n\r\n", dcm::DcmDate::Now().ToString("yyyy/MM/dd").c_str(), dcm::DcmTime::Now().ToString("HH:mm:ss").c_str() );

  
    if ( pRequest != __nullptr  )
      DIMSE_printCMoveRQ(fp, pRequest);
    else
      fprintf(fp,"No Request");
 
    fprintf(fp,"\r\n\r\n");

    if ( pResponse != __nullptr )
      DIMSE_printCMoveRSP(fp, pResponse);
    else
      fprintf(fp,"No Response");

    fprintf(fp,"\r\n\r\n");
    fclose(fp);

    std::ofstream os(sLogFile.c_str(), std::ios::app );

    if ( pDcmDataset != __nullptr )
      pDcmDataset->print(os);
    else
      os << "No Dataset";

    os << std::endl << std::endl;
    
    if (pResponseIds != __nullptr) 
    {
      os << "Response Identifiers: " << std::endl;
      pResponseIds->print(os);  
      os << std::endl << std::endl;
    }

    if (pStatusDetail != __nullptr) 
    {
      os << "Status Detail: " << std::endl;
      pStatusDetail->print(os);  
      os << std::endl << std::endl;
    }

    os << std::endl << std::endl;
  } // if

} // DumpMoveCommand()


/**
 * Dumps the move command and all results into the file specified
 *
 * @param sLogFile        Log file
 * @Param pDcmDataset     Query
 * @param pRequest        MOve request
 * @param pResponse       Move response
 * @param pStatusDetail   Status details
 */
void DcmUtility::DumpFindCommand( const std::string & sLogFile, DcmDataset * pDcmDataset,  T_DIMSE_C_FindRQ * pRequest, T_DIMSE_C_FindRSP * pResponse, DcmDataset * pStatusDetail )
{

  if ( ! sLogFile.empty() )
  {

   FILE * fp(fopen(sLogFile.c_str(), "a"));

    if ( fp == NULL  )
    {
      LogErr(util::Strings::Format("Failed to open log file %s",sLogFile.c_str()), "DcmUtility","DumpFindCommand");
      return;
    }

    fprintf(fp,"============================================================================================================================================================\r\n");
    fprintf(fp,"Find Command    %s-%s\r\n\r\n", dcm::DcmDate::Now().ToString("yyyy/MM/dd").c_str(), dcm::DcmTime::Now().ToString("HH:mm:ss").c_str() );

  
     if ( pRequest != __nullptr  )
      DIMSE_printCFindRQ(fp, pRequest);
    else
      fprintf(fp,"No Request");
 
    fprintf(fp,"\r\n\r\n");

    if ( pResponse != __nullptr )
      DIMSE_printCFindRSP(fp, pResponse);
    else
      fprintf(fp,"No Response");

    fprintf(fp,"\r\n\r\n");
    fclose(fp);

    std::ofstream os(sLogFile.c_str(), std::ios::app );

    if ( pDcmDataset != __nullptr )
      pDcmDataset->print(os);
    else
      os << "No Dataset";

    os << std::endl << std::endl;

 
    if (pStatusDetail != __nullptr) 
    {
      os << "Status Detail: " << std::endl;
      pStatusDetail->print(os);  
      os << std::endl << std::endl;
    }

    os << std::endl << std::endl;
  } // if

} // DumpFindCommand()



/**
 * Determins of the value representation is only string represented
 */
bool DcmUtility::IsStringRepresentation(const DcmTag & dcmTag)
{

  return ( dcmTag.getEVR() == DcmEVR::EVR_CS ||       /* Coded String   */
           dcmTag.getEVR() == DcmEVR::EVR_AE ||       /* AE TItle       */
           dcmTag.getEVR() == DcmEVR::EVR_UI ||       /* UID            */
           dcmTag.getEVR() == DcmEVR::EVR_SH ||       /* SHort String   */
           dcmTag.getEVR() == DcmEVR::EVR_PN ||       /* Persion Name   */
           dcmTag.getEVR() == DcmEVR::EVR_AS ||       /* Age String     */
           dcmTag.getEVR() == DcmEVR::EVR_LT ||       /* Long Text      */
           dcmTag.getEVR() == DcmEVR::EVR_ST );       /* Short Text     */



}  // IsStringRepresentation(const DcmTag & dcmTag)