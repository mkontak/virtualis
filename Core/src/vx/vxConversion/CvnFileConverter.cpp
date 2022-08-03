// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "CvnFileConverter.h"
#include "CvnExceptions.h"
#include "CvnEnums.h"
#include "CvnLog.h"
#include "CvnUnitTest.h"
#include "DcmImage.h"
#include "DcmUtility.h"
#include "Timer.h"
#include "File.h"

using namespace cvn;
using namespace vx;


/**
 *  default constructor
 */
CvnFileConverter::CvnFileConverter( int iSiteId ) :
CvnThread(),
CvnConverter(iSiteId)
{
} // 


/**
 * Destructor
 */
CvnFileConverter::~CvnFileConverter()
{
} // ~CvnFileConverter


/**
 * Creates the file converter object and returns a chared pointer
 *
 * @param iSite     Site id
 *
 * @return shared_ptr
 */
std::shared_ptr<CvnFileConverter> CvnFileConverter::Create( int iSite )
{
  std::shared_ptr<CvnFileConverter> pConverter = std::make_shared<CvnFileConverter>(iSite);

  if ( pConverter == __nullptr )
    throw CvnOutOfMemoryException(LogRec("Failed to allocated CvnFileConverter Object", "CvnFileConverter", "Create"));

  return pConverter;
} // Create(int iSite)


/**
 * Add files from list 
 *
 * @param files     File list
 */
void CvnFileConverter::AddFiles( std::vector<std::string> files )
{
  for( std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++ )
  {
    AddFile(*it);
  } // for

} // AddFiles(std::vector<std::string> files)


/**
 * Add files from driectory
 *
 * @param sDirectory      Directory
 * @param sPattern        File pattren
 * @param bRecursive
 */
void CvnFileConverter::AddDirectory( const std::string & sDirectory, const std::string & sPattern, bool bRecursive )
{

  if( ! IsReadable(sDirectory) )
    throw CvnIOException(LogRec(util::Strings::Format("Directory does not exists or is unreadable (%s)",sDirectory.c_str()), "CvnFileConverter", "AddDirectory"));

  CvnLog::Information(util::Strings::Format("Adding dicom files with extension %s from %s", sPattern.c_str(), sDirectory.c_str()), "CvnFileConverter", "AddDriectory");

  std::string sDicomFiles = sDirectory + "\\" + ( sPattern.empty() ? "*.dcm" : sPattern );
  WIN32_FIND_DATA fileData;

  std::vector<std::string> directories;

  // File the first file in the list that matches the pattern
  HANDLE handle = ::FindFirstFile(sDicomFiles.c_str(), &fileData);

  // ==============================
  // Make sure the handle is valid
  // =================================
  if ( handle != INVALID_HANDLE_VALUE )
  {
    do 
    {     
      if ( fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        AddFile( sDirectory + "\\" + std::string(fileData.cFileName) );
      else
        directories.push_back(std::string(fileData.cFileName));
     
    } while ( ::FindNextFile(handle, &fileData ) );

    FindClose(handle);

    if ( bRecursive )
    {

      for ( std::vector<std::string>::iterator it = directories.begin(); it != directories.end(); it++ )
      {
          AddDirectory((*it),sPattern,bRecursive); 
      }
    } // if  
  } // if
 
} // AddDirectory(const std::string & sDirectory, const std::string & sPattern, bool recursive)


/**
 * Add files from list 
 *
 * @param sFile     File to be added
 */
void CvnFileConverter::AddFile( const std::string & sFile )
{ 
  if ( ! IsReadable(sFile) )
    throw CvnIOException(LogRec(util::Strings::Format("File does not exists or is unreadable (%s)",sFile.c_str()), "CvnFileConverter", "AddFile"));

  CvnLog::Debug(util::Strings::Format("Adding file %s", sFile.c_str()), "CvnFileConverter", "Run");

  m_files.push_back(sFile);

} // AddFile(const std::string & sFile)


/**
 * Creates the volume 
 *
 * @param callbackData      Pointer to object/data to be retyurned in callback
 * @param callbackFunction  Callback function pointer
 */
const std::map<uint4, std::shared_ptr<CvnSubVolumeInfo>> & CvnFileConverter::Create(std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID ) > callbackFunction, LPVOID callbackData, const std::string & sVolumeFilePath )
{

  m_callbackData      = callbackData;
  m_callbackFunction  = callbackFunction;
  m_sVolumeFilePath   = sVolumeFilePath;

  RunSynchronously();

  return m_pSubVolumes;

} // Create(LPVOID callbackData, std::function<CVN_CALLBACK_FUNCTION> callbackFunction)


/**
 * Determines if the files specified is readable
 *
 * @param  sFile      File to be checked
 *
 * @return  True if the files exists and is readable
 */
bool CvnFileConverter::IsReadable( const std::string & sFile )
{
  // Determines accessibility
  return ( _access(sFile.c_str(), 4) != -1 );
} // IsReadable(const std::string & sFile)


/**
 * Runs the thread: 
 * step 1: upload all dcm image files into m_images
 * step 2: extract sub volume informatioin
 * step 3: create all sub volumes
 */
void CvnFileConverter::RunSynchronously()
{
  m_bAbort = false;
 
  CvnLog::Debug("Starting the conversion process", "CvnFileConverter", "RunSynchronously");

  Callback(E_STARTED);
  
  try
  {	
    CvnLog::Information(util::Strings::Format("Default application type is [%s]",GetDefaultApplication().c_str()), "CvnFileConverter", "RunSynchronously");

    /// Loads files into image collection
    LoadFiles();

    CvnLog::Debug( "Finished loading dicom files", "CvnFileConverter", "Run" );

    m_sSeriesUid = m_images->GetSeriesUid();

		if( IsEnhancedDicom() )
		{ 			
 		  ///extract sub volume information
		  ExtractSubVolInfo_enh();

     /// create all sub volumes for enhanced dicom images
	    CreateAllSubVolumes_enh();
		}
		else
		{
		  /// extract sub volume information
		  ExtractSubVolInfo();

      /// create all sub volumes
	    CreateAllSubVolumes();
		} // else
		
    CvnLog::Debug("All sub-volume were created.", "CvnFileConverter", "RunSynchronously");

    /// Complete
    Callback(E_COMPLETED);

  }
  catch ( CvnAbortException & )
  {
    CvnLog::Error("Conversion was aborted", "CvnFileConverter", "RunSynchronously");

    Callback(E_ABORTED);

    throw;

  }
  catch ( CvnException & ex )
  {

    std::string sMessage(util::Strings::Format("Conversion failed : %s", ex.GetTextMessage().c_str()));

    CvnLog::Error(sMessage, "CvnFileConverter", "RunSynchronously");

    Callback(E_FAILED, sMessage);

    throw;

  }
  catch ( dcm::DcmException & ex )
  {
    std::string sMessage(util::Strings::Format("Conversion failed : %s", ex.GetTextMessage().c_str()));

    CvnLog::Error(sMessage, "CvnFileConverter", "RunSynchronously");

    Callback(E_FAILED, sMessage);

    throw CvnException(LogRec(sMessage, "CvnFileConverter", "RunSynchronously"));

  }
  catch ( ... )
  {
 
    CvnLog::Error("Conversion failed : unspecified error", "CvnFileConverter", "RunSynchronously");

    Callback(E_FAILED, "Conversion failed : unspecified error");

    throw CvnException(LogRec("Conversion failed : unspecified error", "CvnFileConverter", "RunSynchronously"));

  } // catch
  
} //RunSynchronously()


/**
  * unit test
	*
	*  @param sVolumeFilePath
	*/
void CvnFileConverter::UnitTest( const std::string & sVolumeFilePath )
{
	CvnUnitTest unitTest( sVolumeFilePath );

	unitTest.Run();
} // UnitTest



/**
 * Aborts the move operation
 */
void CvnFileConverter::Abort()
{
  CvnLog::Debug( "Aborting File Conversion", "CvnFileConverter", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  // Terminates the thread
  CvnThread::Abort();

} // Abort()


/**
 * Loads the files into the image collection
 */
void CvnFileConverter::LoadFiles()
{
  if ( m_files.empty() )
    throw CvnException(LogRec("No files specified for conversion", "CvnFileConverter", "Run"));

  /// Clears out all parameters
  ClearParameters();

  m_callbackParameters.StageType = S_LOADING;
  m_callbackParameters.Total     = (int)m_files.size();
  m_callbackParameters.Completed = 0;
  m_callbackParameters.Remaining = (int)m_files.size();

  LPCSTR file = __nullptr;

  std::shared_ptr<dcm::DcmImageCollection> images( dcm::DcmImageCollection::Create() );
  
  try
  {    
    images->Clear();

		CvnLog::Information(util::Strings::Format("Starting the loading of %d files into the collection", (int)m_files.size()), "CvnFileConverter", "Run");

    // =========================================
    // Read in all files into the collection
    // ========================================
    for ( std::vector<std::string>::iterator it = m_files.begin(); it != m_files.end(); it++ )
    {

      // Check for abort
      if ( m_bAbortRequested )
        throw CvnAbortException(LogRec("Aborting conversion","CvnFileConversion","LoafFiles"));

      file = (*it).c_str();

      std::string sFilename(util::IO::GetFilename(file));
     
      CvnLog::Debug(util::Strings::Format("Loading file %s into collection", sFilename.c_str()), "CvnFileConverter", "Run");

      m_callbackParameters.Completed++;
      m_callbackParameters.Remaining--;

      Callback(E_UPDATED, sFilename);
 
      try
      {
        images->Add(std::dynamic_pointer_cast<dcm::IDcmImage>(dcm::DcmImageFactory::Create( (*it), dcm::PT_FILE)));
      }
      catch ( dcm::DcmIOException &  )
      {
        CvnLog::Warning(util::Strings::Format("Failed to load file %s, ignoring",sFilename.c_str()), "CvnFileConverter", "LoadFiles");
      }

    } // END .. For each file

		/// sort collection based on ascending order of image number
		m_images = images->Sort( dcm::DcmIndex::Create(DcmTagKey(0x0020,0x0013)), dcm::DcmImageCollection::ASCENDING  );

  }
  catch ( dcm::DcmException & ex )
  {
    CvnLog::Error(util::Strings::Format("Failed to load files into list : %s", ex.GetTextMessage().c_str()), "CvnFileConverter","LoadFIles");
    throw;
  }
  catch ( CvnAbortException )
  {
    throw;
  }
  catch( ... )
  {
    throw CvnException(LogRec(util::Strings::Format("Failed to load file %s : unknown exception (%d)", ( file != __nullptr ? file : ""), GetLastError()), "CvnFileConverter", "LoadFiles"));
  }

} // LoadFiles()


/** 
  *  check if image is enhanced dicom format
	*
	*  @return ture if it is
	*/
bool CvnFileConverter::IsEnhancedDicom()
{
	std::string ss( m_images->GetModality() );
	/// check if the image modality is supported or not
	if( !((ss == "CT") || (ss == "MR") || (ss == "PT") ||
		    (ss == "XA") || (ss == "NM")) )
	{
    throw CvnException(LogRec("Image modality is not supported", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if

	/// set the default v3D application type 
  if( GetDefaultApplication() == "CAR" )
    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::CAR;
  else if( GetDefaultApplication() == "VAS" )
    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::VAS;
  else if( GetDefaultApplication() == "CS" )
    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::CS;
  else if( GetDefaultApplication() == "XP" )
    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XP;
  else
    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::VC;

  /// check if the image is multi-frame enhanced format
  std::shared_ptr<dcm::IDcmImage> pDcmImage(m_images->GetFirstImage());

	ss = "";
	if( pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x2110), ss, true ) && ss == "01" )
	{ 
    throw CvnException(LogRec("Compressed image is not supported.", "CvnFileConverter", "ExtractSubVolInfo"));	
  } // if

	ss = "";
	/// check if the image photometric interpretation is monochrome
	if( pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0004), ss, true ) ) /// photometric interpretation
	{
		if( !( ss == "MONOCHROME2" || ss == "MONOCHROME1" ) )
		{
      throw CvnException(LogRec("Non-monochrome image is not supported.", "CvnFileConverter", "ExtractSubVolInfo"));	
		} // if
	} //
	else
	{
    throw CvnException(LogRec("Missing photometric interpretation attribute.", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // else

	/// check if the critical dicom attributes exist 
	if( ! pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0013), ss, true ) ||  /// image number
		  ! pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0032), ss, true ))    /// image position patient
	{
    throw CvnException(LogRec("Missing critical dicom attributes for volume creation.", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if

	int4 iBitsStored(0);
	/// check if the data storage in 12 bit 
	if( ! pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0101), iBitsStored, true ) )
	{
    throw CvnException(LogRec("Miss bits stored information.", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if
	else if( iBitsStored < 9 )
	{
		int4 iBitsAllocated(0);
		if( pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0100), iBitsAllocated, true ) && (iBitsAllocated == 8) )
		{
			m_bIsDcmByteStored = true;
		}
		else
		{
      throw CvnException(LogRec("Bits stored was less than 8.", "CvnFileConverter", "ExtractSubVolInfo"));	
		}
	} // else
	else if( iBitsStored > 16 )
	{
    throw CvnException(LogRec("Bits store was greater than 12", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if

	ss = "";
  /// Check if the image is non-volume slice. If the Conversion Type attribute is showing,
  /// the image is not ORIGINAL dicom and is derived from certain image sources.
  if( pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0008, 0x0064), ss, true ) )  /// conversion type.
  {
    throw CvnException(LogRec("The dicom image was converted from other image source rather than scanning a patient.", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if Turn this on for final product


	int4 iNumOfFrame(0);
	if( pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0008), iNumOfFrame, true ) && iNumOfFrame > 1 )  /// number of multi-frame
	{ 
		CvnLog::Debug("Enhanced DICOM image", "CvnFileConverter", "IsEnhancedDicomFile"); 
		return true;
	} //

	ss = m_images->GetModality();

	if( ss == "MR" )
	{
		ss = "";
	  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1030), ss, true );
	  std::transform(ss.begin(), ss.end(), ss.begin(), std::tolower);

		if( ss.size() > 0 )
		{ 
			const uint uEndStr = std::string::npos;  /// to robust to compiler and platform variations
	    uint u = ss.find( "cine", 0 );

			if( u != uEndStr )
			{
        throw CvnException(LogRec("None volumatric MRI CINE images", "CvnFileConverter", "ExtractSubVolInfo"));	
			} // 
		} // if
	} // if

	CvnLog::Debug("Conventional DICOM image", "CvnFileConverter", "IsEnhancedDicomFile"); 
	return false;
} // IsEnhancedDicom


/**
 *   Extract sub-volume information for the input DICOM image series.
 *   This is the key to the correct data interpration. Multiple assumptions on
 *   DICOM attributes compliance to the standard have to be made.
 */
void CvnFileConverter::ExtractSubVolInfo()
{
	
	/// Check if the image number is more than 1
	if(	m_images->Count() < 1 )
    throw CvnException(LogRec("Image collection was empty", "CvnFileConverter", "ExtractSubVolInfo"));	

  CvnLog::Information("Extracting sub-volume information from dataset","CvnFileConverter","ExtractSubVolInfo_enh");

  std::shared_ptr<dcm::IDcmImage> pDcmImage(m_images->GetFirstImage());

	int4 iNum(0);
	bool bAcquistitionNumAvailable(false);
  if( pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0012), iNum, true ) && iNum > 0 )
  {
		/// If gantry tilted, it is imposible to finish a series in one acquisition. So, in gantry tilted 
		/// scenario, ignore the acquisition number information
		float fGantryTilt = 0.0f;

		if( !(pDcmImage->TryGetValue< float >( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1120), fGantryTilt, true ) && fGantryTilt != 0.0f) )
		{
			bAcquistitionNumAvailable = true;
      CvnLog::Debug("Acquisition number is available.", "CvnFileConverter", "ExtractSubVolInfo");	
		} // if

	} // if 

	std::string ss("");
	/// Check if the series is functional imaging with temporal information
	bool m_bTemporalInfoAvailable = pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0100), ss, true );  /// temporal position identifier
	
	if( m_bTemporalInfoAvailable )
	{
		m_vImgSeriesVolInfo.bIsFunTempoVol = true;
	} // if

	/// This line should be the bifircation point to split code implementation for non-/enhanced dicom series.

	m_images->Reset();

  // ====================================================
	// collecting unique image infomation for all slices
  // ====================================================
	while( m_images->MoveNext() )
	{
	  pDcmImage = m_images->Current();
		if( pDcmImage == nullptr )
		{
			throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		} //if

		ImageUniqeInfo sImgInfo;
		sImgInfo.iSubVolumeLabel        =  0;
		sImgInfo.iTemporalPosIdentifier = -1;
		sImgInfo.iImageNumber           = -1;
		sImgInfo.iAcquisiNum            = -1;

    pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0013), sImgInfo.iImageNumber, true );  /// get the image number attribute

    sImgInfo.vfImagePosition = Triple<float4>( pDcmImage->GetImagePosition().GetX(), pDcmImage->GetImagePosition().GetY(), pDcmImage->GetImagePosition().GetZ() );

    if( m_bTemporalInfoAvailable )
    {
			pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0100), sImgInfo.iTemporalPosIdentifier, true );  /// get the temporal position identifer
		} // if

		if( bAcquistitionNumAvailable )
		{
			iNum = 0;

			if( pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0012), iNum, true ) && iNum > 0 )
			{
				sImgInfo.iAcquisiNum = iNum;
			}
			else
			{
        throw CvnException(LogRec("Acquisition number was missing.", "CvnFileConverter", "ExtractSubVolInfo"));	
			}
		} // if

		m_vvImageInfoCollection.push_back( sImgInfo );

	} // end while loop through all images in the collection

	if( m_vvImageInfoCollection.size() != m_images->Count() )
	{
    throw CvnException(LogRec("Error occurred in the extraction of image unique information.", "CvnFileConverter", "ExtractSubVolInfo"));	
	} // if

	pDcmImage = m_images->GetFirstImage();

	const int4 iTotalNumOfImgs( m_vvImageInfoCollection.size() );

  m_iNumOfSubVol = 0;
	register int4 i, j;

	///=========================================================================
  /// determine sub-volume: There are 3 situations that are handled differently
	/// 1. Functional temporal volume
	/// 2. Multiple acauisition volume
	/// 3. Other volume
	///==========================================================================
	if( m_bTemporalInfoAvailable )
	{
		CvnLog::Debug("It is a temporal functional series", "CvnFileConverter", "ExtractSubVolInfo");

		std::vector<int4> viSubVolTemporalIndexes;

		/// label sub volumes based on their temporal position identifier
	  for( i=0; i<iTotalNumOfImgs; i++ )
	  {
			if( viSubVolTemporalIndexes.size() < 1 ) 
			{ 
				/// This is the first sub volume found.
				viSubVolTemporalIndexes.push_back( m_vvImageInfoCollection[i].iTemporalPosIdentifier );
			} // if

			else
			{ 
				/// look into previous temporal index identifiers
				for( j=viSubVolTemporalIndexes.size()-1; j>=0; j-- )
				{
					if( m_vvImageInfoCollection[i].iTemporalPosIdentifier == viSubVolTemporalIndexes[j] )
					{ 
						/// it belongs to a found sub volume
            m_vvImageInfoCollection[i].iSubVolumeLabel = j;

						j = -99999;   /// exit out the loop
					} // if
				} // for j
						
				if( j > -99999 )
				{ 
					/// it is a newly found sub volume
          m_vvImageInfoCollection[i].iSubVolumeLabel = viSubVolTemporalIndexes.size();

					viSubVolTemporalIndexes.push_back( m_vvImageInfoCollection[i].iTemporalPosIdentifier );

					m_iNumOfSubVol++;
				} // if

			} // else
		} // for i

		m_iNumOfSubVol++;

		CvnLog::Debug(util::Strings::Format("Number of sub volume = %d",m_iNumOfSubVol), "CvnFileConverter", "ExtractSubVolInfo"); 

		int4 iNumOfTemporalPos(0);
		if( pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0105), iNumOfTemporalPos, true ) )
		{
			if( iNumOfTemporalPos != m_iNumOfSubVol )
			{
				CvnLog::Error("Number of subvolume didn't match that in DICOM header for functional series.", "CvnFileConverter", "ExtractSubVolInfo");	
			} // if
		} // if
		else
		{
      throw CvnException(LogRec("Time position info was not available", "CvnFileConverter", "ExtractSubVolInfo"));	
		}
	} // if
	else
	{
		CvnLog::Debug("Non-temporal volume", "CvnFileConverter", "ExtractSubVolInfo");

		if( iTotalNumOfImgs < 2 )
		{
			throw CvnException(LogRec("Too few images to create a volume", "CvnFileConverter", "ExtractSubVolInfo"));
		} // if

		m_images->Reset();
		m_images->MoveNext();
		m_images->MoveNext();  /// move to the 2nd image

		/// check if it is multiple acquisition
	  for( i=1; i<iTotalNumOfImgs; i++ )
	  {
			if( m_vvImageInfoCollection[0].iAcquisiNum != m_vvImageInfoCollection[i].iAcquisiNum )
			{
				m_bIsMultiAcquisition = true;

				/// check if it is cardiac gated
	      std::string sStudyDescription("");
	      pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x1030), sStudyDescription, true );
	      std::transform(sStudyDescription.begin(), sStudyDescription.end(), sStudyDescription.begin(), std::tolower);

	      std::string sSeriesDescription("");
	      pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x103E), sSeriesDescription, true );
	      std::transform(sSeriesDescription.begin(), sSeriesDescription.end(), sSeriesDescription.begin(), std::tolower);

	      std::string sAcquisProtocolName("");
	      pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1030), sAcquisProtocolName, true );
	      std::transform(sAcquisProtocolName.begin(), sAcquisProtocolName.end(), sAcquisProtocolName.begin(), std::tolower);

	      const std::string sCardiac("cardia");
	      const std::string sCoronary("coronary");
	      const std::string sGated("gated");
				const std::string sBolus("bolus");

	      const uint uPos = std::string::npos;  /// to avoid compiler or platform inconsistency

	      uint u1 = sAcquisProtocolName.find( sCardiac, 0 );
	      uint u2 = sAcquisProtocolName.find( sCoronary, 0 );
				uint u3 = sSeriesDescription.find( sCardiac, 0 );
				uint u4 = sSeriesDescription.find( sCoronary, 0 );
				uint u5 = sStudyDescription.find( sCardiac, 0 );
				uint u6 = sStudyDescription.find( sCoronary, 0 );
				
	      if( u1 != uPos || u2 != uPos || u3 != uPos || u4 != uPos || u5 != uPos || u6 != uPos )
				{ // It is cardiac application. Need to check if the gated acquisition applies.
				
					u1 = sStudyDescription.find( sGated, 0 );
					u2 = sSeriesDescription.find( sGated, 0 );
					u3 = sAcquisProtocolName.find( sGated, 0 );

					/// gated cardiac scan assign each section a different acquisition number, but all sections are in the same volume.
          if( u1 != uPos || u2 != uPos || u3 != uPos || u4 != uPos || u5 != uPos ) 
					  m_bIsMultiAcquisition = false;
				} // 

				i = 99999;  /// exit the loop
			} // if

			m_images->MoveNext();
			pDcmImage = m_images->Current();
		} // for i

		/// temporary variable for the need of processing
		int4 iSubVolFirstIndex(0);

		if( m_bIsMultiAcquisition )
		{
			/// label sub volume based on acquisition number
	    for( i=1; i<iTotalNumOfImgs; i++ )
	    {
			  for( j=iSubVolFirstIndex; j<i; j++ )
			  {
					if( m_vvImageInfoCollection[j].iAcquisiNum != m_vvImageInfoCollection[i].iAcquisiNum )
				  {
			      m_iNumOfSubVol++;

					  m_vvImageInfoCollection[i].iSubVolumeLabel = m_iNumOfSubVol;

					  ///======================================
					  /// It is assumed that images in the series sorted and grouped based on dicom image number.
					  /// So, it just need to search the previous found image number section for a sub volume.
					  ///======================================
					  iSubVolFirstIndex = i;   

					  j = 99999;
				  } // if
				  else
				  {
					  m_vvImageInfoCollection[i].iSubVolumeLabel = m_iNumOfSubVol;
				  } // else

			  } // for j
		  } // for i

		}  // if
		else
		{ 
			/// label sub volume based on image number and position
	    for( i=1; i<iTotalNumOfImgs; i++ )
	    {
			  for( j=iSubVolFirstIndex; j<i; j++ )
			  {
				  if( m_vvImageInfoCollection[j].vfImagePosition == m_vvImageInfoCollection[i].vfImagePosition )
				  {
			      m_iNumOfSubVol++;

					  m_vvImageInfoCollection[i].iSubVolumeLabel = m_iNumOfSubVol;

					  ///======================================
					  /// It is assumed that images in the series sorted and grouped based on dicom image number.
					  /// So, it just need to search the previous found image number section for a sub volume.
					  ///======================================
					  iSubVolFirstIndex = i;   

					  j = 99999;
				  } // if
				  else
				  {
					  m_vvImageInfoCollection[i].iSubVolumeLabel = m_iNumOfSubVol;
				  } // else

			  } // for j
			} // for i

		}  // else

		m_iNumOfSubVol++;

		CvnLog::Debug("Number of sub volume = " + ToAscii(m_iNumOfSubVol), "CvnFileConverter", "ExtractSubVolInfo"); 
	} // else

	m_vImgSeriesVolInfo.iNumOfSubVol = m_iNumOfSubVol;

	DetermineAppType();

} // ExtractSubVolInfo()


/**
 *   Extract sub-volume information for the enhanced DICOM image series.
 *   This is the key to correct data interpration. 
 */
void CvnFileConverter::ExtractSubVolInfo_enh()
{
  /// get the first images
  std::shared_ptr<dcm::IDcmImage> pDcmImage(m_images->GetFirstImage());
	
	std::string ss("");
	/// check Volumetric property
	if( !pDcmImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0008, 0x9206), ss, true ) )
	{ 
    throw CvnException(LogRec("Volumetric information was not available.", "CvnFileConverter", "ExtractSubVolInfo_enh"));	
	}
	else if( ! (ss == "VOLUME") )
	{
    throw CvnException(LogRec("Images are not volumetric.", "CvnFileConverter", "ExtractSubVolInfo_enh"));	
	}		

  enum OrderingMode { ASCENDING, FLAT, DECENDING, NONE };
  OrderingMode eOrderingMode(ASCENDING);
	bool bIsFlatOrdering(false);

	int4 iSubVolIndex(0);
	int4 iStackSubVolIndexInitial(0);

  // Get the frame based on the stack id 
  dcm::DcmStackMap stacks = m_images->GetStacks();                                                        

  // ======================================================================
  // Loop through each stack. Each stack contains a list of frame images
  // =====================================================================
  std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
  {
    /// Get the indexes defined for the stack (This is the defined index from the dicom header)
    std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

    /// Sort the stack by the identified indexes
    std::shared_ptr<dcm::DcmImageCollection> sorted(__nullptr);
        
    // =========================================================================================
    // If the indexes are not empty meaning that the DimensionOrganizationSequence is present
    // ========================================================================================
    if ( ! indexes->IsEmpty()  )
    {
      // ================================================================
      // If there are indexes then the sequence existed in the header and contained 
      // elements, otherwise a count of 0 means no elements existed under the sequence
      // ==============================================================
      if (indexes->Count() > 0  )
      {
		    sorted = item.second->Sort(indexes);                      // Sort stack by defined index
	    }

	    // =======================================================================
      // Else if there is a multi-valued index specified it makes out life easier since these values are 
      // unsigned shorts  or shorts and comparisons are easier.
      // =======================================================================
      else if ( indexes->HasMultiValuedIndex() )
      {
        // Get the first image in the collection
        std::shared_ptr<dcm::IDcmImage> firstImage = item.second->GetFirstImage();

        if ( firstImage != __nullptr )
        {
          // ====================================================================================================
          // Check if the image of frame contains the multi-valued index element. The check is necessary since there is no guarantee that 
          // this element exists in the frame. This is only used in the enhanced so the image will be a frame
          // =====================================================================================================
          std::shared_ptr<dcm::DcmElement> element;
         if ( firstImage->TryGetElement(indexes->GetMultiValuedIndex()->GetFunctionalGroup(), indexes->GetMultiValuedIndex()->GetIndexElement(), element, true) )
            sorted = item.second->Sort(indexes);   // Sort the collection by the defined index which uses the multi-value.
            
		   }  // if     

	    } // else if        
    } // if

    // ===========================================================
    // If no sorting could be done then use the default The default sorting is just
    // StackID/InStackPosition. Since I check for the elements prior to the sort 
    // there is no TemporalPosition.
    // =============================================================
    if ( sorted == __nullptr )
      sorted = item.second->Sort( dcm::DcmIndexList::GetDefaultSpatialIndexes() );

	  ///--------------------------------------------------------
    item.second->Reset();

    // frame image pointer
    std::shared_ptr<dcm::IDcmImage> pFrameImage;

    item.second->MoveNext();
    pFrameImage = item.second->Current();
	  if( pFrameImage == nullptr )
		{
		  throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo_enh"));	
	  } //if

		int4 i0(-1);
    pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9057), i0,  true );

    item.second->MoveNext();
    pFrameImage = item.second->Current();
	  if( pFrameImage == nullptr )
		{
		  throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo_enh"));	
	  } //if

		int4 i1(-1);
    pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9057), i1,  true );

		eOrderingMode = ASCENDING;
		if( i1 < 0 || i0 < 0 )
		{
      CvnLog::Error("Missing in-stack position number", "CvnFileConverter", "ExtractSubVolInfo_enh");
			eOrderingMode = NONE;
		}
		else if( i1 < i0 )
		{
			eOrderingMode = DECENDING;
		}
		else if( i1 == i0 )
		{
			eOrderingMode = FLAT;
			bIsFlatOrdering = true;
		}

		item.second->Reset();

    int4 iFrameCount(0), iSubVolFrameCount(0);

	  if( eOrderingMode == NONE )
		{ 
			int4 iSubVolIndexCut(0);

			/// Dicom image didn't provide Stack position information. We have to use plane position information solely.
      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
		    FrameUniqeInfo sImgInfo;

	      pFrameImage = item.second->Current();
		    if( pFrameImage == nullptr )
		    {
		      throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		    } //if

        sImgInfo.vfImagePosition = Triple<float4>( pFrameImage->GetImagePosition().GetX(), pFrameImage->GetImagePosition().GetY(), pFrameImage->GetImagePosition().GetZ() );
			
				register int4 i(0);
				
				if( iSubVolFrameCount > 0 )
				{ /// search for new volume solely based on plane position
					/// It is assumed that the default frame order in good spatial relationship group.
				  for( i=iSubVolIndexCut; i<m_vvFrameInfoCollection.size(); i++ )
				  {
					  if( m_vvFrameInfoCollection[i].vfImagePosition == sImgInfo.vfImagePosition )
					  {
						  iSubVolIndex++;
						  
						  m_vNumOfFrameInSubVols.push_back( iSubVolFrameCount );
						  
						  iSubVolFrameCount = 0;

						  /// update sub-volume index seach bound
						  iSubVolIndexCut = m_vvFrameInfoCollection.size();

						  i = 99999;  // exit the for(i) loop
					  } // if position repeated

				  } // for i

				} // if( iSubVolFrameCount > 0 )

				iSubVolFrameCount++;
					
		    sImgInfo.iSubVolumeLabel = iSubVolIndex;
				sImgInfo.iStackId        = iSubVolIndex+1;
				sImgInfo.iInStackIndex   = iSubVolFrameCount;
				sImgInfo.iTemporalIndex  = iSubVolFrameCount;
			
			  m_vvFrameInfoCollection.push_back( sImgInfo );

      } // end while loop through all images in the collection

		} // if
		else
		{
      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
		    FrameUniqeInfo sImgInfo;
		    sImgInfo.iStackId        = -1;
		    sImgInfo.iInStackIndex   = -1;
		    sImgInfo.iTemporalIndex  = -1;

	      pFrameImage = item.second->Current();
		    if( pFrameImage == nullptr )
		    {
		      throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		    } //if

        sImgInfo.vfImagePosition = Triple<float4>( pFrameImage->GetImagePosition().GetX(), pFrameImage->GetImagePosition().GetY(), pFrameImage->GetImagePosition().GetZ() );
			
        pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9056), sImgInfo.iStackId, true );

			  if( sImgInfo.iStackId > -1 )
			  { // stack id is available
          pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9057), sImgInfo.iInStackIndex,  true );
          pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9128), sImgInfo.iTemporalIndex, true );

				  if( eOrderingMode == FLAT )
				  {
					  if( iFrameCount > 0 ) 
					  {
					    if( sImgInfo.iInStackIndex != m_vvFrameInfoCollection[iFrameCount-1].iInStackIndex )
					    {
						    iSubVolIndex = iStackSubVolIndexInitial;
					    } // if
					    else
					    {
						    iSubVolIndex++;
					    } // else
					  } // if
				  } // if
				  else if( iFrameCount > 0 )
				  {										
					  if( (eOrderingMode == ASCENDING && (sImgInfo.iInStackIndex < m_vvFrameInfoCollection[m_vvFrameInfoCollection.size()-1].iInStackIndex) ) ||
						  (eOrderingMode == DECENDING && (sImgInfo.iInStackIndex > m_vvFrameInfoCollection[m_vvFrameInfoCollection.size()-1].iInStackIndex) ) )
					  {
						  iSubVolIndex++;

						  m_vNumOfFrameInSubVols.push_back( iSubVolFrameCount );

							iSubVolFrameCount = 0;
					  } // if sub volume index changes
				  } // if
			  } // if stack id is available
 
		    sImgInfo.iSubVolumeLabel = iSubVolIndex;
			
			  m_vvFrameInfoCollection.push_back( sImgInfo );

			  iSubVolFrameCount++;
			  iFrameCount++;

      } // end while loop through all images in the collection

		} // else


		m_vNumOfFrameInSubVols.push_back( iSubVolFrameCount );

		m_iNumOfSubVol           = iSubVolIndex + 1;
		iStackSubVolIndexInitial = m_iNumOfSubVol;
		iSubVolIndex             = iStackSubVolIndexInitial;
  } ); // for

	if( bIsFlatOrdering )
	{  /// we need to re-count number of frame in each sub-volume again.
		m_vNumOfFrameInSubVols.clear();
		
		register int4 i(0);
		for( ; i<m_iNumOfSubVol; i++ ) 
			m_vNumOfFrameInSubVols.push_back(0);

		const int4 iN( m_vvFrameInfoCollection.size() );
		for( i=0; i<iN; i++ )
		  m_vNumOfFrameInSubVols[ m_vvFrameInfoCollection[i].iSubVolumeLabel ]++;

	} // if

	DetermineAppType_enh();

} // ExtractSubVolInfo_enh()


/**
 *   create all sub volumes
 */
void CvnFileConverter::CreateAllSubVolumes()
{
	register int4 i;

  CvnLog::Information(util::Strings::Format("Creating %d  sub-volume(s)",m_iNumOfSubVol),"CvnFileConverter","CreateAllSubVolumes");

	try
	{
	  for( i=0; i<m_iNumOfSubVol; i++ )
	  {
		  /// create the i-th volume		
		  CreateSubVol( i );

		  CvnLog::Information("Finish the " + ToAscii(i+1) + "-th sub volume creation", "CvnFileConverter", "ExtractSubVolInfo"); 

	  } /// for i
	}
	catch( ... )
	{
		CvnLog::Error("Abnormal termination. Remove all volume files from disk.", "CvnFileConverter", "CreateAllSubVolumes"); 

		/// remove all subvolume files writen to the disk
	  for( i=0; i<m_iNumOfSubVol; i++ )
	  {
      std::string sVolumeFile( GenerateSubVolumeFilePath(m_sVolumeFilePath, i) );

		  vx::File fileHandle( sVolumeFile ); 

			if( fileHandle.Exists() )
			  fileHandle.Delete();

		} /// for i
	} // catch

} /// CreateAllSubVolumes()


/**
 *   create all sub volumes for enhanced dicom series
 */
void CvnFileConverter::CreateAllSubVolumes_enh()
{
	register int4 i;

  CvnLog::Information(util::Strings::Format("Creating %d enchanced sub-volume(s)",m_iNumOfSubVol),"CvnFileConverter","CreateAllSubVolumes_enh");

	try
	{
	  for( i=0; i<m_iNumOfSubVol; i++ )
	  {
		  if( m_vNumOfFrameInSubVols[i] >= m_iMinImgNumber4Vol )
		  {
			  /// create the i-th volume		
		    CreateSubVol_enh( i );
        CvnLog::Information("Finish the " + ToAscii(i+1) + "-th sub volume creation", "CvnFileConverter", "CreateAllSubVolumes_enh"); 
		  }
		  else
		  {
        CvnLog::Warning("The " + ToAscii(i+1) + "-th sub-volume has too little frame to create volume", "CvnFileConverter", "CreateAllSubVolumes_enh"); 
		  }

	  } /// for i

	}
	catch( ... )
	{
		CvnLog::Error("Abnormal termination. Remove all volume files from disk.", "CvnFileConverter", "CreateAllSubVolumes_enh"); 

		/// remove all subvolume files writen to the disk
	  for( i=0; i<m_iNumOfSubVol; i++ )
	  {
      std::string sVolumeFile( GenerateSubVolumeFilePath(m_sVolumeFilePath, i) );

		  vx::File fileHandle( sVolumeFile ); 

			if( fileHandle.Exists() )
			  fileHandle.Delete();

		} /// for i

	} // catch
	
} /// CreateAllSubVolumes_enh()



/**
 * CallbackRelay 
 *
 *  @param parameters is the callback parameters
 */
void CvnFileConverter::CallbackRelay(const CVN_CALLBACK_PARAMETERS & parameters)
{
  // Only relay if progress event
  if ( parameters.EventType == E_PROGRESS )
    Callback(&const_cast<CVN_CALLBACK_PARAMETERS &>(parameters));
} // CallbackRelay
