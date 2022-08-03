// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "Timer.h"
#include "CvnEnums.h"
#include "CvnLog.h"
#include "CvnConverter.h"
#include "CvnExceptions.h"
#include "vxBlockVolumeIterator.h"
#include "CvnSubVolumeInfo.h"
#include <algorithm>


using namespace cvn;


/**
 * Default constructor
 */
CvnConverter::CvnConverter( int iSiteId ) :
 m_iBias(0),
 m_iSiteId(iSiteId),
 m_sPatientName(""),
 m_bIsDcmByteStored(false),
 m_bIsMultiAcquisition(false),
 m_iMaxValueRange(4095),
 m_iMinImgNumber4Vol(2),
 m_sSeriesUid(""),
 m_sDefaultApplication(""),
 m_sVolumeFilePath("")
{


	/// initialize image series infomation container with defult value
	m_vImgSeriesVolInfo.iNumOfSubVol     = 1;
	m_vImgSeriesVolInfo.bIsFunTempoVol   = false;
	m_vImgSeriesVolInfo.sV3DAppType      = ApplicationTypes::VC;  /// This should be set as system default type which the customer selects for their v3D system.

  RtlZeroMemory(&m_parameters,sizeof(CVN_CALLBACK_PARAMETERS));
} // CvnConverter()


/**
 * Destructor
 */
CvnConverter::~CvnConverter()
{
  ReleaseSubVolumes();
} // ~CvnConverter()


/**
 * Release the member volume in the memory
 */
void CvnConverter::ReleaseSubVolumes()
{
  m_pSubVolumes.clear();
} // ReleaseVolume()


/**
 * Determine the pixel bias
 *
 *  @param iSubVolIndex the sub volume index
 *  @param pDcmImgBuffer the pointer to the dicom image data content
 */
void CvnConverter::DetermineBias( const int4 & iSubVolIndex, std::shared_ptr<short> & pDcmImgBuffer )
{
	if( m_vvfDcmImagePosSub.size() > 0 )
  {
		m_images->Reset();
		m_images->MoveNext();
		std::shared_ptr<dcm::IDcmImage> pDcmImage( m_images->Current() );

    std::string sManufacturer = pDcmImage->GetManufacturer();
		std::string sModality     = pDcmImage->GetModality();
    float4 fRescaleIntercept  = pDcmImage->GetRescaleIntercept();

		const uint uPos = std::string::npos;  /// for compatibility to variation of compiler and OS platform

	  std::transform( sManufacturer.begin(), sManufacturer.end(), sManufacturer.begin(), std::toupper);        
    // ========================================================================================================================
    // Due to the fact that the Toshiba does not fill in the rescale intercept from the CT we set it here to a default of 1000
    // ========================================================================================================================
    if ( sManufacturer.find("TOSHIBA") != uPos && sModality == "CT" && fRescaleIntercept == 0.0F )
    {
      m_iBias = 1000;
      return;
    } // if

    register float4 fRescaleSlope;

	  const int4 iNumPixelInSlice( m_viDcmImageDim.m_x * m_viDcmImageDim.m_y );

	  register int4 i, j(0), k, iMin, iHUmin, iPixelPaddingValue;

		m_iBias = SHRT_MAX;

		if( sModality == "CT" )
		{  // CT HU value has special meaning for tissue type. It shall be treated uniquely.
		  for( i = 0; i < (int)m_images->Count(); i++ )
	    {
		    /// check if the image is in the current sub volume
		    if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
		    {
			    k = pDcmImage->GetPixelPaddingValue();
			    iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
          fRescaleSlope      = pDcmImage->GetRescaleSlope();
          fRescaleIntercept  = pDcmImage->GetRescaleIntercept(); 

				  iHUmin = int4( (-1200.0f - fRescaleIntercept) / fRescaleSlope + 0.5f );  /// minimum pixel value in image data, not modality HU value

				  iMin = SHRT_MAX;

				  /// determine minimum pixel value for the current dicom image
				  for( k=iNumPixelInSlice; k>=0; k--, j++ )
				  {
		        if( pDcmImgBuffer.get()[j] != iPixelPaddingValue )
		        {
		          if( pDcmImgBuffer.get()[j] < iMin ) iMin = pDcmImgBuffer.get()[j];
		        } // if
				  } // for j

				  if( iMin < iHUmin ) iMin = iHUmin;  /// This guard the bias goes too low to make sense on tissue HU value.

          int4 iMinNew = (int4)( fRescaleSlope * (float4)(iMin) + fRescaleIntercept + 0.5f );

				  if( m_iBias > iMinNew ) m_iBias = iMinNew;

			  } // if

				m_images->MoveNext();
			  pDcmImage = m_images->Current();

			  if( pDcmImage == nullptr )
			  {
				  CvnLog::Debug("Dcm image pointer was nullptr", "CvnConverter", "DetermineBias");	
          i = 99999;  /// exit the for loop
			  } // if
		  }  // for i
		} // if

		else  /// non-CT modality
		{
			register int4 iMax(SHRT_MIN);

		  for( i = 0; i < (int)m_images->Count(); i++ )
	    {
		    /// check if the image is in the current sub volume
		    if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
		    {
			    k = pDcmImage->GetPixelPaddingValue();
			    iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
          fRescaleSlope      = pDcmImage->GetRescaleSlope();
          fRescaleIntercept  = pDcmImage->GetRescaleIntercept(); 

				  iMin = SHRT_MAX;

				  /// determine minimum pixel value for the current slice dicom image
				  for( k=iNumPixelInSlice; k>=0; k--, j++ )
				  {
		        if( pDcmImgBuffer.get()[j] != iPixelPaddingValue )
		        {
		          if( pDcmImgBuffer.get()[j] < iMin ) iMin = pDcmImgBuffer.get()[j];
							if( pDcmImgBuffer.get()[j] > iMax ) iMax = pDcmImgBuffer.get()[j];
		        } // if
				  } // for j

					float4 f(float4(iMax - iMin)*fRescaleSlope);
					if( f > float4(m_iMaxValueRange) ) m_iMaxValueRange = int4(f);

          int4 iMinNew = (int4)( fRescaleSlope * (float4)(iMin) + fRescaleIntercept + 0.5f );

				  if( m_iBias > iMinNew ) m_iBias = iMinNew;
			  } // if

				m_images->MoveNext();
			  pDcmImage = m_images->Current();

			  if( pDcmImage == nullptr )
			  {
				  CvnLog::Debug("Dcm image pointer was nullptr", "CvnConverter", "DetermineBias");	
          i = 99999;  /// exit the for loop
			  } // if
		  }  // for i
		} // else if it is not CT modality

		/// bias is an offset value that is greater than 0.
		m_iBias = (m_iBias < 0 ? abs(m_iBias) : 0 );

  } // END ... If there are images in the list

} // DetermineBias()


/** 
 *  This is designed for the PACS neutral solution. The dicom images collection
 *  shall be passed in a memory object rather than dicom file reading
 *
 *  @param dcmImages the input dicom image collection
 */
void CvnConverter::Create( const dcm::DcmImageCollection & dcmImages )
{
  /// need to implement for getting m_images from memory rather than from files
} // Create()


/**
 *  Initialize block volume variables in the header for the iSubVolIndex sub volume.
 *
 * @param iSubVolIndex    Sub-volume index
 * @patam pVolume         Volume whose header is to be initialized
 */
void CvnConverter::InitializeHeader( const int4 & iSubVolIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume  )
{

  std::shared_ptr<dcm::DcmElement> element;


  // Set Dimensions/Units and Type
  pVolume->GetHeader().SetVolDim       ( m_viVolDim           );
  pVolume->GetHeader().SetVoxelUnits   ( m_vfVolVoxelUnits    );
  pVolume->GetHeader().SetVoxelTypeEnum( vxVolumeHeader::UINT2);

	try
  {
    m_images->Reset();
		m_images->MoveNext();
    std::shared_ptr<dcm::IDcmImage> pDcmImage;

		for( register uint4 i(0); i < m_images->Count(); i++ )
	  {
		  /// check if the image in the current sub volume
		  if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
		  {
				i = 99999;  /// exit the loop

			} // if
			else
			{
			  if( ! m_images->MoveNext() )
				{
					CvnLog::Debug("Dcm image collection was corrupted", "CvnConverter", "InitializeHeader");
					i = 99999;
				} // if
			} // else

		} // for i

		/// get the current dicom image
		pDcmImage = m_images->Current();

    // patient weight
    std::string sPatientWeight( "" );
  
    if ( pDcmImage->GetElements()->TryGetElement(DCM_PatientWeight, element, true) )
      sPatientWeight = element->GetValue<std::string>();

    // patient sex
    std::string sPatientSex( "" );

    if ( pDcmImage->GetElements()->TryGetElement(DCM_PatientSex, element, true) )
      sPatientSex = element->GetValue<std::string>();


    std::string sPatientAgeUnits( "Y" );
    std::string sPatientAge("" );

    if ( pDcmImage->GetElements()->TryGetElement(DCM_PatientAge, element, true) )
      sPatientAge = element->GetValue<std::string>();

    if( sPatientAge.size() > 0 )
    {
      int ch = (int)sPatientAge.back();
      sPatientAge.pop_back();

      if ( ch <= 0 && ch >= 9  )
        sPatientAgeUnits = ("" + toupper(ch));
    } // if
   
    // patient birth date
    std::string sPatientBirthDate("");
    pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0010, 0x0030), sPatientBirthDate, true );



    // patient History
    std::string sPatientHistory("" );
  
    if ( pDcmImage->GetElements()->TryGetElement(DCM_AdditionalPatientHistory, element, true) )
      sPatientHistory = util::Strings::Replace(util::Strings::Replace(element->GetValue<std::string>(),'\n',' '),'\r',' ');

    // operator Name
    std::string sOperatorName("");
  
    if ( pDcmImage->GetElements()->TryGetElement(DCM_OperatorsName, element, true) )
      sOperatorName = element->GetValue<std::string>();

    // referring phys.
    std::string sPhysicianName( pDcmImage->GetReferringPhysician().ToString() );
  
    // Series Instance UID
    std::string sSeriesInstanceUID( pDcmImage->GetSeriesInstanceUid() );

    // Study Instance UID
    std::string sStudyInstanceUID( pDcmImage->GetStudyInstanceUid() );

    // Institution name
    std::string sInstitutionName( pDcmImage->GetInstitutionName() );

    // Manufacturer
    std::string sManufacturer( pDcmImage->GetManufacturer() );

    // Manufacture Model
    std::string sManufacturerModel( pDcmImage->GetManufacturerModel() );

    // Patient Position
    std::string sPatientPosition( pDcmImage->GetPatientPosition() );
  
    // slice Thickness
    float fSliceThickness( 0.0F );

    if ( pDcmImage->GetElements()->TryGetElement(DCM_SliceThickness, element, true) )
      fSliceThickness = element->GetValue<float>();


    float fSliceSpacing( m_vfDcmPixelSpacing.m_z ); 
		  
    float fPitch = ( fSliceThickness != 0 ? fSliceSpacing / fSliceThickness : 0.0F );

    // space Between slices
    std::string sPitch( util::Strings::Format("%f",fPitch) );
  
    // KVP
    std::string sKVP( "" );
  
    if ( pDcmImage->GetElements()->TryGetElement(DCM_KVP, element, true) )
      sKVP = element->GetValue<std::string>();

    // mA
    std::string sMA( "" );
 
    if ( pDcmImage->GetElements()->TryGetElement(DCM_XRayTubeCurrent, element, true) )
      sMA = element->GetValue<std::string>();

    // Reconstruction diameter
    std::string sReconstructionDiameter("" );

     if ( pDcmImage->GetElements()->TryGetElement(DCM_ReconstructionDiameter, element, true) )
      sReconstructionDiameter = element->GetValue<std::string>();

    std::string sFOV( sReconstructionDiameter.size() > 2 ? sReconstructionDiameter.substr(0,2) : "0" );

    // tilt
    std::string sTilt( "");

    if ( pDcmImage->GetElements()->TryGetElement(DCM_GantryDetectorTilt, element, true) )
      sTilt = element->GetValue<std::string>();

  
    // Set Header Variables related to CT scanner
    VarMap varMapHeader;

    varMapHeader.SetHdrVar("mA",              sMA , "ACQ x-ray tube current in mA");
    varMapHeader.SetHdrVar("kv",              sKVP, "ACQ Peak kilo voltage output of the x-ray generator used");
    varMapHeader.SetHdrVar("sliceThickness",  util::Strings::Format("%f",fSliceThickness) , "slice Thickness in mm");
    varMapHeader.SetHdrVar("pitch",           sPitch, "pitch ");
    varMapHeader.SetHdrVar("tilt",            sTilt, "tilt in degrees of the scanning gantry");
    varMapHeader.SetHdrVar("FOV",             sFOV, "FOV in cm");

    pVolume->GetHeader().SetVariableVolumeHeader( varMapHeader );

    std::string sStudyDate( pDcmImage->GetStudyDate().ToString("yyyy-MM-dd") );
      
    dcm::DcmDate studyDate = pDcmImage->GetStudyDate();
    dcm::DcmTime studyTime = pDcmImage->GetStudyTime();

    // Study/Scan date
    vx::Date scanDate(studyDate.GetYear(), (uint1)studyDate.GetMonth(), (uint1)studyDate.GetDay(), (uint1)studyTime.GetHour(), (uint1)studyTime.GetMinutes(), studyTime.GetSeconds() );

    // Patient name
    m_sPatientName = pDcmImage->GetPatientName().ToString();
  
    // Patient ID
    std::string sPatientID( pDcmImage->GetPatientId() ); 
  
    // Title
    std::string sTitle( util::Strings::Format("%s scan", sPatientPosition.c_str()) );

    // Author
    std::string sAuthor( util::Strings::Format("%s on %s,%s", sInstitutionName.c_str(), sManufacturer.c_str(), sManufacturerModel.c_str()) );
  
    pVolume->GetHeader().SetTitle(sTitle);
    pVolume->GetHeader().SetAuthor(sAuthor);
    pVolume->GetHeader().SetCreationDate(scanDate);

    /// Name of file
    pVolume->GetHeader().AppendHistory("");

    const int iVersion = 5; //Get enhanced DICOM and non-orthogoanl plane supported
    std::string sHeader(pDcmImage->GetElements()->GetSerializedHeader());

    // Set General Header Variables
    varMapHeader.SetHdrVar("version",          iVersion,                        "Header version");
    varMapHeader.SetHdrVar("siteID",           m_iSiteId,                        "Site ID");
    varMapHeader.SetHdrVar("studyDate",        pDcmImage->GetStudyDate().ToString(),  "Study Date");
    varMapHeader.SetHdrVar("studyTime",        pDcmImage->GetStudyTime().ToString(),  "Study Time");
    varMapHeader.SetHdrVar("seriesDate",       pDcmImage->GetSeriesDate().ToString(), "Series Date");
    varMapHeader.SetHdrVar("seriesTime",       pDcmImage->GetSeriesTime().ToString(), "Series Time");
    varMapHeader.SetHdrVar("seriesUID",        sSeriesInstanceUID,              "Series Instance UID");
    varMapHeader.SetHdrVar("studyUID",         sStudyInstanceUID,               "Study Instance UID");
    varMapHeader.SetHdrVar("physicianName",    sPhysicianName,                  "ID Referring physician name");
    varMapHeader.SetHdrVar("patientID",        sPatientID,                      "Patient ID/Medical ID");
    varMapHeader.SetHdrVar("institutionName",  sInstitutionName,                "ID Institution");
    varMapHeader.SetHdrVar("operatorName",     sOperatorName,                   "ID operator Name");
    varMapHeader.SetHdrVar("patientName",      m_sPatientName,                  "Patient Name");
    varMapHeader.SetHdrVar("patientBirthDate", sPatientBirthDate,               "Birth Date of the patient");
    varMapHeader.SetHdrVar("patientAge",       sPatientAge,                     "Age of the patient");
    varMapHeader.SetHdrVar("pateintAgeUnits",  sPatientAgeUnits,                "Units of age (D, M, Y)");
    varMapHeader.SetHdrVar("patientWeight",    sPatientWeight,                  "Weight of the patient");
    varMapHeader.SetHdrVar("patientSex",       sPatientSex,                     "Sex of the patient");
    varMapHeader.SetHdrVar("patientHistory",   sPatientHistory,                 "Additional patient history");
    varMapHeader.SetHdrVar("patientPosition",  sPatientPosition,                "Position of patient during scan");
		int4 i = static_cast<int4>(m_vImgSeriesVolInfo.sV3DAppType);
    varMapHeader.SetHdrVar("studyType",        i,                               "Study Type"); 
    varMapHeader.SetHdrVar("zOrderReversed",   0,                               "Slices are in reverse order");
    varMapHeader.SetHdrVar("manufacturer",     sManufacturer,                   "Device manufacturer");
    varMapHeader.SetHdrVar("bias",             m_iBias,                         "Viatronix bias value");

		/// vxVolumeHeader 5.0 contents
    varMapHeader.SetHdrVar("volumeOrigin",     m_vfOrigin,                      "RCS coordinate of volume origin");
		varMapHeader.SetHdrVar("subVolIndex",      iSubVolIndex,                    "Index of the sub volume");
		varMapHeader.SetHdrVar("numOfSubvol",      m_vImgSeriesVolInfo.iNumOfSubVol,"Total number of subvolume in series");
		varMapHeader.SetHdrVar("maxValueRange",    m_iMaxValueRange,                "Maximum voxel value range");

		if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::CAR )
      varMapHeader.SetHdrVar("cardiaPhase", ExtractCardiacPhaseInfo(iSubVolIndex), "Cardiac Phase Percentage");

		/// serilized dicom header
    varMapHeader.SetHdrVar("dicomHeader",      util::Strings::Remove(sHeader,'\n'),          "Dicom Header Serialized");

    pVolume->GetHeader().SetVariableVolumeHeader(varMapHeader);

  }
  catch ( ... )
  {
    throw CvnException(LogRec("Failed to initialize the header, unspecified exception", "CvnConverter", "InitializeHeader"));
  }

} // InitializeHeader()


/**
 *  Scale the dicom image pixel values. It may clamp CT HU values for rendering.
 *
 *  @param iSubVolIndex the sub volume index
 *  @param pDcmImgBuffer the pointer to the dicom image data
 *  @prarm puDcmImgBuffer the pointer to the unsigned short volume
 */
void CvnConverter::ScaleDcmImgPixel( const int4 & iSubVolIndex, std::shared_ptr<short> &  pDcmImgBuffer, std::shared_ptr<uint2> & puDcmImgBuffer )
{
  m_images->Reset();
	m_images->MoveNext();
  std::shared_ptr<dcm::IDcmImage> pDcmImage( m_images->Current() );

  std::string sModality = pDcmImage->GetModality();

	/// This bound is tied to the rendering preset/colormaping. Any change here must trigger
	/// code change in the rendering codes.
	int4 iLowerBound(0), iUpperBound(USHRT_MAX);
  if( sModality == "CT" )
  {
    iLowerBound = 0;
    iUpperBound = SHRT_MAX;
  } // if

	register float4 fRescaleSlope, fRescaleIntercept;
	register int4   iPixelPaddingValue;

  const int4 iNumPixelInSlice( m_viDcmImageDim.m_x * m_viDcmImageDim.m_y );

  register int4 i, j(0), k, iVoxelRaw;

  for( i = 0; i < (int)m_images->Count(); i++ )
  {
		/// check if the image in the current sub volume
	  if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
		{
			k = pDcmImage->GetPixelPaddingValue();
			iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
      fRescaleSlope      = pDcmImage->GetRescaleSlope();
      fRescaleIntercept  = pDcmImage->GetRescaleIntercept();

	    if( fRescaleSlope == 1.0f )
	    {
			  /// loop over all pixels in the current dicom slice to scale
			  for( k=iNumPixelInSlice-1; k>=0; k--, j++ )
			  {
          if ( pDcmImgBuffer.get()[j] == iPixelPaddingValue )
          { 
            puDcmImgBuffer.get()[j] = 0;
          }  // if
          else
          {   
            iVoxelRaw = (int4)((float4)(pDcmImgBuffer.get()[j]) + fRescaleIntercept + 0.5f) + m_iBias;  

            puDcmImgBuffer.get()[j] = uint2(Bound<int4>(iLowerBound, iVoxelRaw, iUpperBound));

          }  // else

			  } // for j
		  } // if
			else
			{
			  /// loop over all pixels in the current dicom slice to scale
			  for( k=iNumPixelInSlice-1; k>=0; k--, j++ )
			  {
          if ( pDcmImgBuffer.get()[j] == iPixelPaddingValue )
          { 
            puDcmImgBuffer.get()[j] = 0;
          }  // if
          else
          {   
            iVoxelRaw = (int4)( fRescaleSlope * (float4)(pDcmImgBuffer.get()[j]) + fRescaleIntercept + 0.5f) + m_iBias;

            puDcmImgBuffer.get()[j] = uint2(Bound<int4>(iLowerBound, iVoxelRaw, iUpperBound));
          }  // else

			  } // for j
			} // else

		} // if it belongs to this sub volume

		if( !m_images->MoveNext() )
		{
		   CvnLog::Debug("Dcm image collection indexing error", "CvnConverter", "ScaleDcmImgPixel");	
       i = 99999;  /// exit the for loop
		} // if

		pDcmImage = m_images->Current();

    if( pDcmImage == nullptr )
		{
		   CvnLog::Debug("Dcm image pointer was nullptr", "CvnConverter", "ScaleDcmImgPixel");	
       i = 99999;  /// exit the for loop
	  } // if

  }  // for i

} // ScaleDcmImgPixel


/**
 *  create the sub volume
 *
 *  @param iSubVolIndex the index for the sub volume
 */
void CvnConverter::CreateSubVol( const int4 & iSubVolIndex )
{
	const int4 iNumOfImgInSeries( m_images->Count() );

	register int4 i, j;

  /// the dicom image buffer
  std::shared_ptr<short> pDcmVolData(__nullptr);
  std::shared_ptr<uint2> puDcmVolData(__nullptr);

	try
	{
    RtlZeroMemory(&m_parameters, sizeof(CVN_CALLBACK_PARAMETERS));

    m_parameters.EventType = E_STARTED;
    m_parameters.StageType = S_CREATION;
    m_parameters.Completed = 0;
    m_parameters.Remaining = iNumOfImgInSeries;
    m_parameters.Total     = iNumOfImgInSeries;
 
    CallbackRelay(m_parameters);

		m_images->Reset();
		m_images->MoveNext();
		std::shared_ptr<dcm::IDcmImage> pDcmImage = m_images->Current();

		if( pDcmImage == nullptr )
		  throw CvnException(LogRec("Dicom image collection was corrupted", "CvnConverter", "CreateSubVol"));

	  int4 iNumOfDcmImgs(0);
	  bool bInfoNotReady( true );

	  /// determine sub volume image number
	  for( i=0; i<iNumOfImgInSeries; i++ )
	  {
		  if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex ) 
			{
				iNumOfDcmImgs++;

				if( bInfoNotReady )
		    {
					pDcmImage = m_images->Current();
					if( pDcmImage == nullptr)
				  {
				    throw CvnException(LogRec("Dicom image collection was corrupted", "CvnConverter", "CreateSubVol"));
				  } // if

		      /// Extract sub volume dimension information. 
		      /// Assume that all images have the identical dimensional parameter. So, do it for only one slice.
          m_vfNormalRow    = Normal3D<float4>( (float4)pDcmImage->GetImageOrientation().GetRowX(), 
				       (float4)pDcmImage->GetImageOrientation().GetRowY(), (float4)pDcmImage->GetImageOrientation().GetRowZ() );

			    m_vfNormalColumn = Normal3D<float4>( (float4)pDcmImage->GetImageOrientation().GetColumnX(), 
				     (float4)pDcmImage->GetImageOrientation().GetColumnY(), (float4)pDcmImage->GetImageOrientation().GetColumnZ() );
	    
			    m_viDcmImageDim.m_x = pDcmImage->GetColumns();
	        m_viDcmImageDim.m_y = pDcmImage->GetRows();

			    m_vfDcmPixelSpacing.m_x = (float4)pDcmImage->GetPixelSpacing().GetX();
			    m_vfDcmPixelSpacing.m_y = (float4)pDcmImage->GetPixelSpacing().GetY();

			    bInfoNotReady = false;

		    } // if need to collect information
			} // if in subvol
				
			if( !m_images->MoveNext() )
			{
			  CvnLog::Debug("Dicom image collection index error", "CvnConverter", "CreateSubVol");
			  i = 99999;  /// exit the loop
			} // if
	  } // for i
	        
		/// set the dcm image number for the sub volume
		m_viDcmImageDim.m_z = iNumOfDcmImgs;

		/// assume image in the series has identical image dimension
	  const int4 iDcmImgPixelNum( m_viDcmImageDim.m_x * m_viDcmImageDim.m_y );
	  const int4 iDcmVolSize    ( iDcmImgPixelNum * iNumOfDcmImgs );

		/// allocate memory for the sub volume dicom images
    pDcmVolData = std::shared_ptr<short>( new  short[iDcmVolSize],  [] ( short * value) {  delete [] value; } ); 
	  if( pDcmVolData == nullptr )
      throw CvnOutOfMemoryException(LogRec("Failed to allocate sub-volume", "CvnConverter", "CreateSubVol_enh"));

	  /// clear the sub volume image position container
	  if( m_vvfDcmImagePosSub.size() > 0 ) m_vvfDcmImagePosSub.clear();

	  m_images->Reset();
		m_images->MoveNext();
		pDcmImage = m_images->Current();
	  
	  int4 iOffset(0), kk;

	  /// get the sub volume image data
	  for( i=0; i<iNumOfImgInSeries; i++ )
	  {
		  /// check if the image in the current sub volume
		  if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
		  {
			  m_vvfDcmImagePosSub.push_back( Vector3D<float4>(m_vvImageInfoCollection[i].vfImagePosition.m_x, 
					                                              m_vvImageInfoCollection[i].vfImagePosition.m_y, 
																												m_vvImageInfoCollection[i].vfImagePosition.m_z) );

        pDcmImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0020, 0x0013), kk, true );  /// get the image number attribute

				if( m_bIsDcmByteStored )
				{ /// The dicom image pixel value is in byte
			    uint1 * puTmp = pDcmImage->GetPixelData()->GetData<uint1>();
			
			    for( j=0; j<iDcmImgPixelNum; j++, iOffset++)
			    {
				    pDcmVolData.get()[iOffset] = static_cast<short>(int4(puTmp[j]));				
			    } // for j
				} // if
				else
				{ /// the dicom image pixel value is in short
			    short * psTmp = pDcmImage->GetPixelData()->GetData<short>();
			
			    for( j=0; j<iDcmImgPixelNum; j++, iOffset++)
			    {
				    pDcmVolData.get()[iOffset] = psTmp[j];				
			    } // for j
				} //else 

		  } // if the image in the current sub volume 

			if( m_images->MoveNext() ) 
			{
				pDcmImage = m_images->Current();
				if( pDcmImage == nullptr )
				  throw CvnException(LogRec("Dicom image collection was corrupted", "CvnConverter", "CreateSubVol"));
			}
			else
			{
				CvnLog::Debug("Dicom image collection index error", "CvnConverter", "CreateSubVol");
				i = 99999;  /// exit the loop
			}
	  } // for i
			
		bool bIsOrthoPlane( true );
	  /// Check the image plane orientation
    if( (0.0f != abs(m_vfNormalColumn.X()) && 1.0f != abs(m_vfNormalColumn.X())) || 
	  	  (0.0f != abs(m_vfNormalColumn.Y()) && 1.0f != abs(m_vfNormalColumn.Y())) || 
		    (0.0f != abs(m_vfNormalColumn.Z()) && 1.0f != abs(m_vfNormalColumn.Z())) ||
		    (0.0f != abs(m_vfNormalRow.X())    && 1.0f != abs(m_vfNormalRow.X()))    || 
			  (0.0f != abs(m_vfNormalRow.Y())    && 1.0f != abs(m_vfNormalRow.Y()))    || 
			  (0.0f != abs(m_vfNormalRow.Z())    && 1.0f != abs(m_vfNormalRow.Z())) )
	  {
	    /// The image plane is non-orthogonal.
	    CalculateSubVolParams();
			bIsOrthoPlane = false;
	  } // if
	  else
	  {
	    /// get the index conversion function ready
      CalculateParam4IndexFun();
	  } // else

		DetermineBias( iSubVolIndex, pDcmVolData );
		
		/// we need an uint2 buffer to store modality value images
    puDcmVolData = std::shared_ptr<uint2>( new  uint2[iDcmVolSize],  [] ( uint2 * value) {  delete [] value; } ); 
	  if( puDcmVolData == nullptr )
	    throw CvnOutOfMemoryException(LogRec("Fialed to allocate unsigned modality buffer", "CvnConverter", "CreateSubVol"));

		/// Scale the dicom image to modality value images. If the modality value range 
		/// is greater than USHRT_MAX, the pixel value will be clampped. 
		ScaleDcmImgPixel( iSubVolIndex, pDcmVolData, puDcmVolData );

    std::shared_ptr<vxBlockVolume<uint2>> pVolume(std::make_shared<vxBlockVolume<uint2>>());

 		// ==================================================
    // Make sure the volume memory was allocated OK
    // ====================================================
    if ( pVolume == __nullptr )
      throw CvnOutOfMemoryException(LogRec("Failed to allocate vxBlockVolume", "CvnConverter", "CreateSubVol"));

 
		InitializeHeader( iSubVolIndex, pVolume ); 

	  /// Check the image plane orientation
		if( bIsOrthoPlane )
	  {
		  /// The image plane is orthogonal. 
		  EvaluateVol4OrthoImgs( pVolume, puDcmVolData );
	  } // if
	  else
	  {
		  /// none-orthogonal imaging plane
		  EvaluateVol4NoneOrthoImgs(pVolume,  puDcmVolData );
	  } // else
		
    /// Make sure that we do not have more then 32 sub-volumes
		if( iSubVolIndex > 32 )
       throw CvnException(LogRec("More than 32 sub volume is unrealistic", "CvnConverter", "CreateSubVol"));


    // =================================================================================================================
    // Only add the volume into the list iff then file path is not specified. If the filepath is specified then
    // we are generating each volume as we go and writing them to disk.
    // =================================================================================================================
    std::shared_ptr<CvnSubVolumeInfo> subvolume( m_sVolumeFilePath.empty() ? 
                                                 CvnSubVolumeInfo::Create( iSubVolIndex, iNumOfDcmImgs, pVolume) :
                                                 CvnSubVolumeInfo::Create( iSubVolIndex, iNumOfDcmImgs, Write( m_sVolumeFilePath, iSubVolIndex, pVolume) ) );
    
    subvolume->SetSeriesUid(m_images->GetSeriesUid());
		subvolume->SetImageCount(m_viDcmImageDim.m_z);
    subvolume->SetApplicationType(m_vImgSeriesVolInfo.sV3DAppType);
    subvolume->IsFunctionalTemporalVolume(m_vImgSeriesVolInfo.bIsFunTempoVol);

    pDcmImage = m_images->GetFirstImage();
	  std::string sSeriesDescription("");
	  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x103E), sSeriesDescription, true );
	  std::transform(sSeriesDescription.begin(), sSeriesDescription.end(), sSeriesDescription.begin(), std::tolower);
		  
		subvolume->SetDescription( sSeriesDescription );
		subvolume->SetSubType( "default" );

		if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::CAR )
		  subvolume->AddProperty("phase", ExtractCardiacPhaseInfo(iSubVolIndex), "Cardiac");

    m_pSubVolumes.insert(std::pair<uint4, std::shared_ptr<CvnSubVolumeInfo>>(iSubVolIndex, subvolume));		
		
    // Set the callback parameters for a completed event
    m_parameters.EventType = E_COMPLETED;
    m_parameters.Completed = iNumOfImgInSeries;
    m_parameters.Remaining = 0;
 
    // Callback relay will call the parents threads callback
    CallbackRelay(m_parameters);

	} // try
  catch ( ... )
  {

    CallbackRelay(E_FAILED, "Failed to create sub volume");

    throw CvnException(LogRec("Failed to create sub volume", "Volume", "CreateSubVol"));
  } // catch

} // CreateSubVol()


/**
 *  Evaluate the output block volume for orthogonal image plane
 *
 *  @param pDcmImgBuffer the pointer to the dicom input sub volume image buffer
*/
void CvnConverter::EvaluateVol4OrthoImgs(  const std::shared_ptr<vxBlockVolume<uint2>> & pVolume, std::shared_ptr<uint2> &  pDcmImgBuffer )
{
	if( m_viDcmImageDim.m_x < 8 || m_viDcmImageDim.m_y < 8 )
	{
    throw CvnException(LogRec("The input image dimension is too small.", "CvnConverter", "EvaluateVol4OrthoImg"));
	} // if
	 	
	vxBlockVolumeIterator<uint2> it( * pVolume );
		
	register int4 x, y, z, i, j, k;

	k = GetDcmImgIndex(0, m_iZFormulaIndex) * abs(m_iZgap);

	/// Evaluate the volume from the dicom input images
	for( z=0, it.SetPosZ(0); z<m_viVolDim.m_z; z++, it.IncZ() )
	{
		j = k + GetDcmImgIndex(0, m_iYFormulaIndex) * abs(m_iYgap);

		for( y=0, it.SetPosY(0); y<m_viVolDim.m_y; y++, it.IncY() )
		{
			i = j + GetDcmImgIndex(0, m_iXFormulaIndex) * abs(m_iXgap);

			for( x=0, it.SetPosX(0); x<m_viVolDim.m_x; x++, it.IncX() )
			{ 
				it.SetVoxel( pDcmImgBuffer.get()[i] );

				i = i + m_iXgap;
			} // for x

			j = j + m_iYgap;
		} // for y

		k = k + m_iZgap;
	} // for z
} // EvaluateVol4OrthoImgs()


/**
 *   Calculate the dicom orthogonal image plane index from volume image plane index
 *
 *  @param iVolIndex the input volume image plane index
 *  @param iFormulaIndex the index that tell which calculation formula should apply
 *      0: use the same order index
 *      1: use the opposite order of the X
 *      2: use the opposite order of the Y
 *      3: use the opposite order of the Z
 *  @return the dicom image index
 */
int4 CvnConverter::GetDcmImgIndex( const int4 & iVolIndex, const int4 & iFormulaIndex )
{
	switch( iFormulaIndex )
	{
	  case 0:
			return iVolIndex;  /// the volume normal aligns to the dicom image normal
		case 1:
			return (m_iDcmW   - iVolIndex);
		case 2:
			return (m_iDcmH   - iVolIndex);
		case 3:
			return (m_iDcmNum - iVolIndex);
		default:
			return 0;
	} ///

} //GetDcmImgIndex()


/**
 *  Calculate parameters for coordinate conversion function
 */
void CvnConverter::CalculateParam4IndexFun()
{
	m_vfNormalSlice = Normal3D<float4>( m_vfNormalRow.Cross( m_vfNormalColumn ) );

	Vector3D<float4> vfGap( m_vvfDcmImagePosSub[1] - m_vvfDcmImagePosSub[0] );

	/// assume the dicom slice sample gap is consistent for all slices
	m_vfDcmPixelSpacing.m_z = (float4)abs( vfGap.GetLength() );
	if( m_vfDcmPixelSpacing.m_z == 0.0f )
	{
		throw CvnException(LogRec("Dicom image slice gap is zero", "CalculateParam4IndexFun", "CvnConverter"));
	}

	/// determine sub volume origin coordinate in the patient RCS
	if( vfGap.m_x >= 0 )
	{
		m_vfOrigin.m_x = m_vvfDcmImagePosSub[0].m_x;
	}
	else
	{
		m_vfOrigin.m_x = m_vvfDcmImagePosSub[0].m_x - float4( m_viDcmImageDim.m_x - 1 ) * m_vfDcmPixelSpacing.m_x;
	}

	if( vfGap.m_y >= 0 )
	{
		m_vfOrigin.m_y = m_vvfDcmImagePosSub[0].m_y;
	}
	else
	{
		m_vfOrigin.m_y = m_vvfDcmImagePosSub[0].m_y - float4( m_viDcmImageDim.m_y - 1 ) * m_vfDcmPixelSpacing.m_y;
	}

	if( vfGap.m_z >= 0 )
	{
		m_vfOrigin.m_z = m_vvfDcmImagePosSub[0].m_z;
	}
	else
	{
		m_vfOrigin.m_z = m_vvfDcmImagePosSub[0].m_z - float4( m_viDcmImageDim.m_z - 1 ) * m_vfDcmPixelSpacing.m_z;
	}

	bool bIsZOrderAligned( true ); 
 
	/// The dicom image slice normal may point to the opposite of the acending image number. 
	if( vfGap.Dot( m_vfNormalSlice ) < 0.0f ) 
	{
		bIsZOrderAligned = false;  /// It does not align to the dicom image number acending order
	}

	/// initialize the function constants
	m_iDcmW   = m_viDcmImageDim.m_x-1;
	m_iDcmH   = m_viDcmImageDim.m_y-1;
	m_iDcmNum = m_viDcmImageDim.m_z-1;

	const int4 iS( m_viDcmImageDim.m_x*m_viDcmImageDim.m_y );

	/// calculate formula indexes. There are 24 cases in total.
	if( m_vfNormalRow.X() == 1.0f )
	{
		m_iXgap = 1;
    m_iXFormulaIndex = 0;
		
		if( m_vfNormalColumn.Y() == 1.0f )
		{
			m_iYgap = m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}
			else 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z );
		}  ///1
		else if( m_vfNormalColumn.Y() == -1.0f )
		{
			m_iYgap = -m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 2;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}
			else 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z );
		}  ///2
		else if( m_vfNormalSlice.Y() == -1.0f )
		{
			m_iZgap = m_viDcmImageDim.m_x;
			m_iZFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}
			else 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y );
		}  ///3
		else if( m_vfNormalSlice.Y() == 1.0f )
		{
			m_iZgap = -m_viDcmImageDim.m_x;
			m_iZFormulaIndex = 2;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}
			else 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y );
		}  ///4
	}
	else if( m_vfNormalRow.X() == -1.0f )
	{
		m_iXgap = -1;
    m_iXFormulaIndex = 1;
		
		if( m_vfNormalColumn.Y() == 1.0f )
		{
			m_iYgap = m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}
			else 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z );
		}  ///5
		else if( m_vfNormalColumn.Y() == -1.0f )
		{
			m_iYgap = -m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 2;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}
			else 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z );
		}  ///6
		else if( m_vfNormalSlice.Y() == 1.0f )
		{
			m_iZgap = m_viDcmImageDim.m_x;
			m_iZFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}
			else 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y );
		}  ///7
		else if( m_vfNormalSlice.Y() == -1.0f )
		{
			m_iZgap = -m_viDcmImageDim.m_x;
			m_iZFormulaIndex = 2;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}
			else 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y );
		}  ///8
	} 
	//------------------------------------------------------------------------
	else if( m_vfNormalColumn.X() == 1.0f )
	{
		m_iXgap = m_viDcmImageDim.m_x;
		m_iXFormulaIndex = 0;

		if( m_vfNormalRow.Y() == 1.0f )
		{
			m_iYgap = 1;
			m_iYFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}
			else 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z );
		}  ///9
		else if( m_vfNormalRow.Y() == -1.0f )
		{
			m_iYgap = -1;
			m_iYFormulaIndex = 1;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}
			else 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z );
		}  ///10
		else if( m_vfNormalSlice.Y() == 1.0f )
		{
			m_iZgap = 1;
			m_iZFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}
			else 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x );
		}  ///11
		else if( m_vfNormalSlice.Y() == -1.0f )
		{
			m_iZgap = -1;
			m_iZFormulaIndex = 1;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}
			else 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x );
		}  ///12
	} 
	//------------------------------------------------------------------------
	else if( m_vfNormalColumn.X() == -1.0f )
	{
		m_iXgap = -m_viDcmImageDim.m_x;
		m_iXFormulaIndex = 2;

		if( m_vfNormalRow.Y() == 1.0f )
		{
			m_iYgap = 1;
			m_iYFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}
			else 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z );
		}  ///13
		else if( m_vfNormalRow.Y() == -1.0f )
		{
			m_iYgap = -1;
			m_iYFormulaIndex = 1;

			if( bIsZOrderAligned ) 
			{
				m_iZFormulaIndex = 3;
			  m_iZgap = -iS;
			}
			else 
			{
				m_iZFormulaIndex = 0;
			  m_iZgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_z     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_z );
		}  ///14
		else if( m_vfNormalSlice.Y() == 1.0f )
		{
			m_iZgap = -1;
			m_iZFormulaIndex = 1;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}
			else 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x );
		}  ///15
		else if( m_vfNormalSlice.Y() == -1.0f )
		{
			m_iZgap = 1;
			m_iZFormulaIndex = 0;

			if( bIsZOrderAligned ) 
			{
				m_iYFormulaIndex = 3;
			  m_iYgap = -iS;
			}
			else 
			{
				m_iYFormulaIndex = 0;
			  m_iYgap = iS;
			}  

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_y,     m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x );
		}  ///16
	} 
	//------------------------------------------------------------------------
	else if( m_vfNormalSlice.X() == 1.0f )
	{
		if( bIsZOrderAligned )
		{
		  m_iXFormulaIndex = 0;
		  m_iXgap = iS;
		}
		else
		{
		  m_iXFormulaIndex = 3;
		  m_iXgap = -iS;
		}

		if( m_vfNormalRow.Y() == 1.0f )
		{
			m_iYgap = 1;
			m_iZgap = m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 0;
      m_iZFormulaIndex = 0;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y );
		}  ///17
		else if( m_vfNormalRow.Y() == -1.0f )
		{
			m_iYgap = -1;
			m_iZgap = -m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 1;
      m_iZFormulaIndex = 2;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y );
		}  ///18
		else if( m_vfNormalColumn.Y() == 1.0f )
		{
			m_iYgap = m_viDcmImageDim.m_x;
			m_iZgap = -1;
			m_iYFormulaIndex = 0;
			m_iZFormulaIndex = 1;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x );
		}  ///19
		else if( m_vfNormalColumn.Y() == -1.0f )
		{
			m_iYgap = -m_viDcmImageDim.m_x;
			m_iZgap = 1;
			m_iYFormulaIndex = 2;
			m_iZFormulaIndex = 0;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x );
		}  ///20
	} 
	//------------------------------------------------------------------------
	else if( m_vfNormalSlice.X() == -1.0f )
	{
		if( bIsZOrderAligned )
		{
		  m_iXFormulaIndex = 3;
		  m_iXgap = -iS;
		}
		else
		{
		  m_iXFormulaIndex = 0;
		  m_iXgap = iS;
		}

		if( m_vfNormalRow.Y() == 1.0f )
		{
			m_iYgap = 1;
			m_iZgap = -m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 0;
			m_iZFormulaIndex = 2;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y );
		}  ///21
		else if( m_vfNormalRow.Y() == -1.0f )
		{
			m_iYgap = -1;
			m_iZgap = m_viDcmImageDim.m_x;
			m_iYFormulaIndex = 1;
			m_iZFormulaIndex = 0;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_x,     m_viDcmImageDim.m_y     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_x, m_vfDcmPixelSpacing.m_y );
		}  ///22
		else if( m_vfNormalColumn.Y() == 1.0f )
		{
			m_iYgap = m_viDcmImageDim.m_x;
			m_iZgap = 1;
			m_iYFormulaIndex = 0;
			m_iZFormulaIndex = 0;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x );
		}  ///23
		else if( m_vfNormalColumn.Y() == -1.0f )
		{
			m_iYgap = -m_viDcmImageDim.m_x;
			m_iZgap = -1;
			m_iYFormulaIndex = 2;
			m_iZFormulaIndex = 1;

			m_viVolDim        = Triple<int4>  ( m_viDcmImageDim.m_z,     m_viDcmImageDim.m_y,     m_viDcmImageDim.m_x     );
			m_vfVolVoxelUnits = Triple<float4>( m_vfDcmPixelSpacing.m_z, m_vfDcmPixelSpacing.m_y, m_vfDcmPixelSpacing.m_x );
		}  ///24		
	} 
	//------------------------------------------------------------------------
	else
	{
    throw CvnException(LogRec("Incorrect directioin normal for orthogonal image plane.", "CvnConverter", "CalculateParam4IndexFun"));
	}
	
} // CalculateParam4IndexFun()


/**
 *  Evaluate the output block volume for none-orthogonal image plane
 *
 *  @param pDcmImgBuffer the pointer to the dicom input sub volume image buffer
 */
void CvnConverter::EvaluateVol4NoneOrthoImgs( const std::shared_ptr<vxBlockVolume<uint2>> & pVolume,  std::shared_ptr<uint2> & pDcmImgBuffer )
{
	vxBlockVolumeIterator<uint2> it( * pVolume );
	
	const int4 iDcmX( m_viDcmImageDim.m_x-1 ), iDcmY    ( m_viDcmImageDim.m_y-1 );
	const int4 iDcmZ( m_viDcmImageDim.m_z-1 ), iDcmWidth( m_viDcmImageDim.m_x+1 );
	
	register int4   x, y, z, i, i1, i2, j1, j2, k1, k2, m1, m2;
	register float4 fwx1, fwx2, fwy1, fwy2, fwz1, fx, fy, fz, f;

	float4 fLowerMost = 999999.0f; /// variable for the smallest projection to the dicom slice normal 

	std::vector< float4 > vfDcmProjections;

	/// calculation projection of dicom slice reference points to the dicom slice normal
	for( i=0; i<m_viDcmImageDim.m_z; i++ )
	{
		/// projecion value
		f = m_vfNormalSlice.X() * (m_vvfDcmImagePosSub[i].m_x - m_vfOrigin.m_x) + 
			  m_vfNormalSlice.Y() * (m_vvfDcmImagePosSub[i].m_y - m_vfOrigin.m_y) + 
				m_vfNormalSlice.Z() * (m_vvfDcmImagePosSub[i].m_z - m_vfOrigin.m_z);

		vfDcmProjections.push_back( f );

		if( fLowerMost > f ) fLowerMost = f;
	} // for i

	/// temporary working vector for storing index mapping information
	std::vector<int4> viDcmSliceIndexes;
	for( i=0; i<m_viDcmImageDim.m_z; i++ ) viDcmSliceIndexes.push_back( 0 );

  for( i=0; i<m_viDcmImageDim.m_z; i++ )
	{ 
		j1 = int4( (vfDcmProjections[i] - fLowerMost) / m_vfDcmPixelSpacing.m_z + 0.5f );

		/// It assumes that the dicom image slice thickness are identical in the sub volume
		if( j1 < 0 || j1 >= m_viDcmImageDim.m_z )
		{ 
      throw CvnException(LogRec("Dicom image slice index out of bound", "CvnConverter", "EvaluateVol4NoneOrthoImg"));
		} // if

		viDcmSliceIndexes[ j1 ] = i;
	} // for i

	/// constants for speeding up calculation
	const float4 frx = m_vfNormalRow.X() / m_vfDcmPixelSpacing.m_x;
	const float4 fry = m_vfNormalRow.Y() / m_vfDcmPixelSpacing.m_x;
	const float4 frz = m_vfNormalRow.Z() / m_vfDcmPixelSpacing.m_x;
	const float4 fcz = m_vfNormalColumn.Z() / m_vfDcmPixelSpacing.m_y;
	const float4 fcy = m_vfNormalColumn.Y() / m_vfDcmPixelSpacing.m_y;
	const float4 fcx = m_vfNormalColumn.X() / m_vfDcmPixelSpacing.m_y;
  const int4   iOffset( m_viDcmImageDim.m_x * m_viDcmImageDim.m_y );

	fz = m_vfOrigin.m_z;
	/// interpolate the volume from the dicom input images
	for( z=0, it.SetPosZ(0); z<m_viVolDim.m_z; z++, it.IncZ() )
	{
		fy = m_vfOrigin.m_y;

		for( y=0, it.SetPosY(0); y<m_viVolDim.m_y; y++, it.IncY() )
		{
			fx = m_vfOrigin.m_x;

			for( x=0, it.SetPosX(0); x<m_viVolDim.m_x; x++, it.IncX() )
			{ 
				f = m_vfNormalSlice.X() * (fx - m_vfOrigin.m_x) + 
						m_vfNormalSlice.Y() * (fy - m_vfOrigin.m_y) + 
						m_vfNormalSlice.Z() * (fz - m_vfOrigin.m_z);

				i = int4( (f - fLowerMost) / m_vfDcmPixelSpacing.m_z + 0.5f );
				if( i < 0 || i > iDcmZ )
				{
					it.SetVoxel( 0 );
				} // if
				else
				{
				  k1 = viDcmSliceIndexes[ i ];

				  if( f >= vfDcmProjections[ k1 ] )
					{
						fwz1 = (f - vfDcmProjections[ k1 ]) / m_vfDcmPixelSpacing.m_z;
						
						if( i+1 > iDcmZ || fwz1 > 1.0f ) 
						{
							k2   = k1;
							fwz1 = 0.5f;
						}
						else
						{
						  k2 = viDcmSliceIndexes[ i+1 ];
						}
					} // if
					else
					{
						fwz1 = (vfDcmProjections[ k1 ] - f) / m_vfDcmPixelSpacing.m_z;

						if( i-1 < 0 || fwz1 > 1.0f )
						{
							k2   = k1;
							fwz1 = 0.5f;
						} // if
						else
						{
						  k2 = viDcmSliceIndexes[ i-1 ];
						}
					} // else
					
				  /// determine row indexes
					fwy1 = fcx * (fx-m_vvfDcmImagePosSub[k1].m_x) + fcy * (fy-m_vvfDcmImagePosSub[k1].m_y) + fcz * (fz-m_vvfDcmImagePosSub[k1].m_z) + 0.5f;
					j1   = int4( fwy1 );
					fwy1 = fwy1 - float4(j1);

					if( j1 < 0 || j1 > iDcmY )
					{
						it.SetVoxel(0);
					} // if
					else
					{ 
					  fwy2 = fcx * (fx-m_vvfDcmImagePosSub[k2].m_x) + fcy * (fy-m_vvfDcmImagePosSub[k2].m_y) + fcz * (fz-m_vvfDcmImagePosSub[k2].m_z) + 0.5f;
					  j2   = int4( fwy2 );
					  fwy2 = fwy2 - float4(j2);

						if( j2 < 0 || j2 > iDcmY )
						{
							fwz1 = 0.0f;
							j2   = j1;
						} // if

						/// determine column indexes
						fwx1 = frx * (fx-m_vvfDcmImagePosSub[k1].m_x) + fry * (fy-m_vvfDcmImagePosSub[k1].m_y) + frz * (fz-m_vvfDcmImagePosSub[k1].m_z) + 0.5f;
					  i1   = int4( fwx1 );
						fwx1 = fwx1 - float4(i1);

						if( i1 < 0 || i1 > iDcmX )
						{ 
							it.SetVoxel(0);
						}
						else
						{					
						  fwx2 = frx * (fx-m_vvfDcmImagePosSub[k2].m_x) + fry * (fy-m_vvfDcmImagePosSub[k2].m_y) + frz * (fz-m_vvfDcmImagePosSub[k2].m_z) + 0.5f;
					    i2   = int4( fwx2 );
						  fwx2 = fwx2 - float4(i2);

							if( i2 < 0 || i2 > iDcmX )
							{
								fwz1 = 0.0f;
								i2   = i1;
							} // if

              m1 = k1*iOffset + j1*m_viDcmImageDim.m_x + i1;
						  m2 = k2*iOffset + j2*m_viDcmImageDim.m_x + i2;

							/// Nearest linear interpolation is the robust solution when you don't know any more detail spatial information of the source.
							f = (1.0f-fwz1)*((1.0f-fwy1)*((1.0f-fwx1)*float4(pDcmImgBuffer.get()[ m1 ]) + fwx1*float4(pDcmImgBuffer.get()[ m1+1 ])) + 
								               fwy1*((1.0f-fwx1)*float4(pDcmImgBuffer.get()[ m1+m_viDcmImageDim.m_x ]) + fwx1*float4(pDcmImgBuffer.get()[ m1+iDcmWidth ]))) +
								  fwz1*((1.0f-fwy2)*((1.0f-fwx2)*float4(pDcmImgBuffer.get()[ m2 ]) + fwx2*float4(pDcmImgBuffer.get()[ m2+1 ])) + 
								               fwy2*((1.0f-fwx2)*float4(pDcmImgBuffer.get()[ m2+m_viDcmImageDim.m_x ]) + fwx2*float4(pDcmImgBuffer.get()[ m2+iDcmWidth ]))) + 0.5f;

							if( f < 0.0f )
								it.SetVoxel(0);
							else
							  it.SetVoxel( uint2(int4(f)) );

						} // else i
					} // else j
				} // else k

				fx = fx + m_vfVolVoxelUnits.m_x;

			} // for x

			fy = fy + m_vfVolVoxelUnits.m_y;

		} // for y

    fz = fz + m_vfVolVoxelUnits.m_z;

	} // for z

} // EvaluateVol4NoneOrthoImgs()


/**
 *   Calculate sub volume dimension parameters based on input dicom images
 */
void CvnConverter::CalculateSubVolParams()
{
	if( m_viDcmImageDim.m_x < 8 || m_viDcmImageDim.m_y < 8 )
	{
    throw CvnException(LogRec("Dicom image dimension is too small.", "CvnConverter", "CalculateSubVolParams"));
	} // if
	 
	m_vfNormalSlice = Normal3D<float4>( m_vfNormalRow.Cross( m_vfNormalColumn ) );

	Vector3D<float4> vfGap( m_vvfDcmImagePosSub[1] - m_vvfDcmImagePosSub[0] );

	/// assume the dicom slice sample gap is consistent for all slices
	m_vfDcmPixelSpacing.m_z = abs( vfGap.Dot( m_vfNormalSlice ) );
	if( m_vfDcmPixelSpacing.m_z == 0.0f )
	{
		throw CvnException(LogRec("Dicom image slice gap is zero", "CalculateSubVolParams", "CvnConverter"));
	}

	/// initialize the transform matrix from the dicom image CS to the patient RCS. 
	float4 vfMatrixDcm2Rcs[3][3];
	vfMatrixDcm2Rcs[0][0] = m_vfNormalRow.X()    * m_vfDcmPixelSpacing.m_x; 
	vfMatrixDcm2Rcs[1][0] = m_vfNormalColumn.X() * m_vfDcmPixelSpacing.m_y;
	vfMatrixDcm2Rcs[2][0] = 0.0f;
	vfMatrixDcm2Rcs[0][1] = m_vfNormalRow.Y()    * m_vfDcmPixelSpacing.m_x; 
	vfMatrixDcm2Rcs[1][1] = m_vfNormalColumn.Y() * m_vfDcmPixelSpacing.m_y;
	vfMatrixDcm2Rcs[2][1] = 0.0f;
	vfMatrixDcm2Rcs[0][2] = m_vfNormalRow.Z()    * m_vfDcmPixelSpacing.m_x; 
	vfMatrixDcm2Rcs[1][2] = m_vfNormalColumn.Z() * m_vfDcmPixelSpacing.m_y;
	vfMatrixDcm2Rcs[2][2] = 0.0f;

	/// diagnoal points of the patient RCS bounding box
	Vector3D<float4> vfOrigin  = m_vvfDcmImagePosSub[0];
	Vector3D<float4> vfTopMost = m_vvfDcmImagePosSub[0];

	register int4 i, j;
	const float4 fW( float4(m_viDcmImageDim.m_x-1) ), fH( float4(m_viDcmImageDim.m_y-1) );

	/// determine bounding box for the input dicom images
	for( i=0; i<m_viDcmImageDim.m_z; i++ )
	{
	  Vector3D<float4> vvfBoundingPoints[4];
	  vvfBoundingPoints[0] = m_vvfDcmImagePosSub[i];
	  vvfBoundingPoints[1] = Vector3D<float4>( fW * vfMatrixDcm2Rcs[0][0] + m_vvfDcmImagePosSub[i].m_x,
                                             fW * vfMatrixDcm2Rcs[0][1] + m_vvfDcmImagePosSub[i].m_y, 
		                                         fW * vfMatrixDcm2Rcs[0][2] + m_vvfDcmImagePosSub[i].m_z );		
	  vvfBoundingPoints[2] = Vector3D<float4>( fH * vfMatrixDcm2Rcs[1][0] + m_vvfDcmImagePosSub[i].m_x,
                                             fH * vfMatrixDcm2Rcs[1][1] + m_vvfDcmImagePosSub[i].m_y, 
		                                         fH * vfMatrixDcm2Rcs[1][2] + m_vvfDcmImagePosSub[i].m_z );	
	  vvfBoundingPoints[3] = Vector3D<float4>( 
		  fW * vfMatrixDcm2Rcs[0][0] + fH * vfMatrixDcm2Rcs[1][0] + m_vvfDcmImagePosSub[i].m_x,
      fW * vfMatrixDcm2Rcs[0][1] + fH * vfMatrixDcm2Rcs[1][1] + m_vvfDcmImagePosSub[i].m_y, 
		  fW * vfMatrixDcm2Rcs[0][2] + fH * vfMatrixDcm2Rcs[1][2] + m_vvfDcmImagePosSub[i].m_z );	

	  for( j=0; j<4; j++ )
	  {
		  if( vfOrigin.m_x > vvfBoundingPoints[j].m_x ) vfOrigin.m_x = vvfBoundingPoints[j].m_x;
		  if( vfOrigin.m_y > vvfBoundingPoints[j].m_y ) vfOrigin.m_y = vvfBoundingPoints[j].m_y;
		  if( vfOrigin.m_z > vvfBoundingPoints[j].m_z ) vfOrigin.m_z = vvfBoundingPoints[j].m_z;
		
		  if( vfTopMost.m_x < vvfBoundingPoints[j].m_x ) vfTopMost.m_x = vvfBoundingPoints[j].m_x;
		  if( vfTopMost.m_y < vvfBoundingPoints[j].m_y ) vfTopMost.m_y = vvfBoundingPoints[j].m_y;
		  if( vfTopMost.m_z < vvfBoundingPoints[j].m_z ) vfTopMost.m_z = vvfBoundingPoints[j].m_z;

	  } // for j
	} // for i

	m_vfOrigin = vfOrigin;

	/// determine volume voxel spacing along x-axis
	m_vfVolVoxelUnits.m_x = abs( m_vfDcmPixelSpacing.m_x * m_vfNormalRow.Dot( Normal3D<float4>(1.0f, 0.0f, 0.0f) ) );
	if( abs(m_vfNormalRow.X()) < abs(m_vfNormalColumn.X()) && abs(m_vfNormalSlice.X()) < abs(m_vfNormalColumn.X()) ) 
	  m_vfVolVoxelUnits.m_x = abs( m_vfDcmPixelSpacing.m_y * m_vfNormalColumn.Dot( Normal3D<float4>(1.0f, 0.0f, 0.0f) ) );
	if( abs(m_vfNormalColumn.X()) < abs(m_vfNormalSlice.X()) && abs(m_vfNormalRow.X()) < abs(m_vfNormalSlice.X()) ) 
	  m_vfVolVoxelUnits.m_x = abs( m_vfDcmPixelSpacing.m_z * m_vfNormalSlice.Dot( Normal3D<float4>(1.0f, 0.0f, 0.0f) ) );
	
	/// determine volume voxel spacing along y-axis
	m_vfVolVoxelUnits.m_y = abs( m_vfDcmPixelSpacing.m_x * m_vfNormalRow.Dot( Normal3D<float4>(0.0f, 1.0f, 0.0f) ) );
	if( abs(m_vfNormalRow.Y()) < abs(m_vfNormalColumn.Y()) && abs(m_vfNormalSlice.Y()) < abs(m_vfNormalColumn.Y()) ) 
	  m_vfVolVoxelUnits.m_y = abs( m_vfDcmPixelSpacing.m_y * m_vfNormalColumn.Dot( Normal3D<float4>(0.0f, 1.0f, 0.0f) ) );
	if( abs(m_vfNormalColumn.Y()) < abs(m_vfNormalSlice.Y()) && abs(m_vfNormalRow.Y()) < abs(m_vfNormalSlice.Y()) ) 
	  m_vfVolVoxelUnits.m_y = abs( m_vfDcmPixelSpacing.m_z * m_vfNormalSlice.Dot( Normal3D<float4>(0.0f, 1.0f, 0.0f) ) );

	/// determine volume voxel spacing along z-axis
	m_vfVolVoxelUnits.m_z = abs( m_vfDcmPixelSpacing.m_x * m_vfNormalRow.Dot( Normal3D<float4>(0.0f, 0.0f, 1.0f) ) );
	if( abs(m_vfNormalRow.Z()) < abs(m_vfNormalColumn.Z()) && abs(m_vfNormalSlice.Z()) < abs(m_vfNormalColumn.Z()) ) 
	  m_vfVolVoxelUnits.m_z = abs( m_vfDcmPixelSpacing.m_y * m_vfNormalColumn.Dot( Normal3D<float4>(0.0f, 0.0f, 1.0f) ) );
	if( abs(m_vfNormalColumn.Z()) < abs(m_vfNormalSlice.Z()) && abs(m_vfNormalRow.Z()) < abs(m_vfNormalSlice.Z()) ) 
	  m_vfVolVoxelUnits.m_z = abs( m_vfDcmPixelSpacing.m_z * m_vfNormalSlice.Dot( Normal3D<float4>(0.0f, 0.0f, 1.0f) ) );
	
	/// determine volume dimension
	m_viVolDim.m_x = 1 + int4(0.5f + (vfTopMost.m_x - m_vfOrigin.m_x)/m_vfVolVoxelUnits.m_x);
	m_viVolDim.m_y = 1 + int4(0.5f + (vfTopMost.m_y - m_vfOrigin.m_y)/m_vfVolVoxelUnits.m_y);
	m_viVolDim.m_z = 1 + int4(0.5f + (vfTopMost.m_z - m_vfOrigin.m_z)/m_vfVolVoxelUnits.m_z);

} // CalculateSubVolParams()


/**
 * Generates the sub volume file path using the supplied volume file path and index
 *
 * @param sVolumeFilePath         Volume file path
 * @param sSubVolumeIndex         Sub-volume index
 *
 * @return Returns the sub-volume path
 */
std::string CvnConverter::GenerateSubVolumeFilePath( const std::string & sVolumeFilePath, const int4 & iSubVolumeIndex)
{

  /// Make sure the volume path is supplied
  if ( sVolumeFilePath.empty() )
    throw CvnInvalidDataException(LogRec(util::Strings::Format("Invalid path specified %s",sVolumeFilePath.c_str()), "CnvConverter","GenerateSubVolumeFilePath"));


  std::vector<std::string> parts = util::IO::SplitPath(sVolumeFilePath);

  // If no path parts are returned then the specified volume file path is invalid
  if ( parts.empty() )
    throw CvnInvalidDataException(LogRec(util::Strings::Format("Invalid path specified %s",sVolumeFilePath.c_str()), "CnvConverter","GenerateSubVolumeFilePath"));

 
  std::string sDirectory(util::IO::Combine((parts[util::IO::DRIVE] + parts[util::IO::DIRECTORY]), CvnSubVolumeInfo::GetConversionId()));
 


  if ( _access(sDirectory.c_str(), 00) == -1 )
  {
    CvnLog::Debug(util::Strings::Format("Creating directory %s",sDirectory.c_str()), "CvnConverter", "Write");
    ::CreateDirectory ( sDirectory.c_str(), NULL );
  }

  std::string sFileName(util::Strings::Format("%s_%d%s",parts[util::IO::FILE].c_str(), iSubVolumeIndex, parts[util::IO::EXTENSION].c_str()));

  return util::IO::Combine(sDirectory, sFileName);

} // GenerateSubVolumeFilePath(const std::string & sVolumeFilePath, const int4 & iSubVolumeIndex)


/**
 * Write the all the volumes to the filepath specified.
 *
 *  The file name is expanded to be the <FILENAME>_<INDEX>.<EXTENTSION>
 *
 * @param sVolumeFilePath     Volume file path
 */
const std::vector<std::string> CvnConverter::Write( const std::string & sVolumeFilePath )
{

  std::vector<std::string> volumeFiles;
  
  if ( m_pSubVolumes.size() == 0 )
    throw CvnException(LogRec("No volumes have been created", "CvnConverter", "Write"));

	try
	{

    std::for_each( m_pSubVolumes.begin(), m_pSubVolumes.end(), [this, &volumeFiles, &sVolumeFilePath] ( std::pair<int4, std::shared_ptr<CvnSubVolumeInfo>> item ) 
    {
      item.second->SetFilePath( Write(sVolumeFilePath, item.first, item.second->GetVolume() ) );
      volumeFiles.push_back(item.second->GetFilePath());

    } );

    return volumeFiles;
	}
	catch( ... )
	{
		throw CvnException(LogRec("Fail to write volume to disk", "CvnConverter", "Write"));
	}

}   // Write(const std::string & sVolumeFile) 


/**
 * Write the volume to the file specified.
 *
 * @param sVolumeFilePath     Volume file path
 * @paran iSubVolumeINdex     Sub-volume index
 * @param pVolume             Volue to write out
 *
 * @return  Returns the volume files just written
 */
std::string CvnConverter::Write( const std::string & sVolumeFilePath, const int4 & iSubVolumeIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume )
{

  if ( pVolume == __nullptr )
    throw CvnInvalidDataException(LogRec("Cannot write null volume", "CvnConverter", "Write"));

 	try
	{
    std::string sVolumeFile(GenerateSubVolumeFilePath(sVolumeFilePath, iSubVolumeIndex));
    pVolume->Write(sVolumeFile);
    return sVolumeFile;
	}
	catch( ... )
	{
		throw CvnException(LogRec("Fail to write volume to disk", "CvnConverter", "Write"));
	}

}   // Write(const std::string & sVolumeFile) 


/**
 *   determine v3D application type of the dicom series
 */
void CvnConverter::DetermineAppType()
{ 
  std::shared_ptr<dcm::IDcmImage> pDcmImage(m_images->GetFirstImage());

	/// extract DICOM attribute values
	std::string sModality("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x0060), sModality, true );
	std::transform(sModality.begin(), sModality.end(), sModality.begin(), std::tolower);
	
	std::string sManufacturer("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x0070), sManufacturer, true );
	std::transform(sManufacturer.begin(), sManufacturer.end(), sManufacturer.begin(), std::tolower);

	std::string sStudyDescription("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x1030), sStudyDescription, true );
	std::transform(sStudyDescription.begin(), sStudyDescription.end(), sStudyDescription.begin(), std::tolower);

	std::string sSeriesDescription("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x103E), sSeriesDescription, true );
	std::transform(sSeriesDescription.begin(), sSeriesDescription.end(), sSeriesDescription.begin(), std::tolower);

	std::string sAcquisProtocolName("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1030), sAcquisProtocolName, true );
	std::transform(sAcquisProtocolName.begin(), sAcquisProtocolName.end(), sAcquisProtocolName.begin(), std::tolower);
	
	std::string sContrastBolus("");
	bool bContrastBolus(false);
	bContrastBolus = pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x0010), sContrastBolus, true );
	std::transform(sContrastBolus.begin(), sContrastBolus.end(), sContrastBolus.begin(), std::tolower);
		
	std::string sContrastRoute("");
	bool bContrastRoute(false);
	bContrastRoute = pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1040), sContrastRoute, true );
	std::transform(sContrastRoute.begin(), sContrastRoute.end(), sContrastRoute.begin(), std::tolower);
		
	std::string sContrastStartTime("");
	bool bContrastStartTime(false);
	bContrastStartTime = pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1042), sContrastStartTime, true );
	std::transform(sContrastStartTime.begin(), sContrastStartTime.end(), sContrastStartTime.begin(), std::tolower);

	const uint uEndStr = std::string::npos;  /// to robust to compiler and platform variations
	uint u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, u12;

	/// determine v3D application type
	if( sModality != "ct" )
	{
		if( sModality == "mr" )
		{
			const std::string sMRA("mra");
			const std::string sAngio("angio");

	    u1 = sStudyDescription.find(   sMRA,   0 );
	    u2 = sSeriesDescription.find(  sMRA,   0 );
	    u3 = sAcquisProtocolName.find( sMRA,   0 );
			u4 = sStudyDescription.find(   sAngio, 0 );
			u5 = sSeriesDescription.find(  sAngio, 0 );
			u6 = sAcquisProtocolName.find( sAngio, 0 );

	    if( u1 != uEndStr || u2 != uEndStr || u3 != uEndStr || u4 != uEndStr || u5 != uEndStr || u6 != uEndStr )
	    {
		    m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::VAS;
			  return;
		  } // if

		} // if
		else if( sModality == "XA" )
		{
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XA;
		} // else

		m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XP;

	} // none CT modalities
	else
	{ 
		/// check if it is virtual colonoscopy application first
	  const std::string sColon("colon");

	  u1 = sStudyDescription.find(   sColon, 0 );
	  u2 = sSeriesDescription.find(  sColon, 0 );
	  u3 = sAcquisProtocolName.find( sColon, 0 );

	  if( u1 != uEndStr || u2 != uEndStr || u3 != uEndStr )
	  {
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::VC;
			return;
		} // 

		const std::string sScore("scor");
		const std::string sCalcium("calcium");

		u1 = sStudyDescription.find(   sScore,   0 );
		u2 = sSeriesDescription.find(  sScore,   0 );
		u3 = sAcquisProtocolName.find( sScore,   0 );
		u4 = sStudyDescription.find(   sCalcium, 0 );
		u5 = sSeriesDescription.find(  sCalcium, 0 );
		u6 = sAcquisProtocolName.find( sCalcium, 0 );

	  if( !bContrastBolus && !bContrastRoute && !bContrastStartTime &&
			  (u1 != uEndStr || u2 != uEndStr || u3 != uEndStr || 
			   u4 != uEndStr || u5 != uEndStr || u6 != uEndStr) )
	  {
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::CS;
			return;
		} // 

		/// check if it is cardiac application
	  const std::string sCardia("cardia");
	  const std::string sCoronary("coronary");
		const std::string sCcta("ccta");

		u1 = sStudyDescription.find(sCardia,     0 );
		u2 = sSeriesDescription.find(sCardia,    0 );
		u3 = sAcquisProtocolName.find(sCardia,   0 );
		u4 = sStudyDescription.find(sCoronary,   0 );
		u5 = sSeriesDescription.find(sCoronary,  0 );
		u6 = sAcquisProtocolName.find(sCoronary, 0 );
		u7 = sStudyDescription.find(sCcta,       0 );
		u8 = sSeriesDescription.find(sCcta,      0 );
		u9 = sAcquisProtocolName.find(sCcta,     0 );

	  if( u1 != uEndStr || u2 != uEndStr || u3 != uEndStr || u4 != uEndStr || 
			  u5 != uEndStr || u6 != uEndStr || u7 != uEndStr || u8 != uEndStr || u9 != uEndStr )
	  {
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::CAR;
			return;
		} //  if

	  const std::string sCta("cta");
	  const std::string sAngio("angio");
		const std::string sAorta("aorta");
		const std::string sVascular("vascul");

		u1 = sStudyDescription.find(   sCta,     0 );
		u2 = sSeriesDescription.find(  sCta,     0 );
		u3 = sAcquisProtocolName.find( sCta,     0 );
		u4 = sStudyDescription.find(   sAngio,   0 );
		u5 = sSeriesDescription.find(  sAngio,   0 );
		u6 = sAcquisProtocolName.find( sAngio,   0 );
		u7 = sStudyDescription.find(   sAorta,   0 );
		u8 = sSeriesDescription.find(  sAorta,   0 );
		u9 = sAcquisProtocolName.find( sAorta,   0 );
		u10 = sStudyDescription.find(  sVascular,0 );
		u11 = sSeriesDescription.find( sVascular,0 );
		u12 = sAcquisProtocolName.find(sVascular,0 );

	  if( bContrastBolus || bContrastRoute || bContrastStartTime || u1 != uEndStr || 
			  u2 != uEndStr || u3 != uEndStr || u4 != uEndStr || u5 != uEndStr || 
				u6 != uEndStr || u7 != uEndStr || u8 != uEndStr || u9 != uEndStr ||
				u10 != uEndStr || u11 != uEndStr || u12 != uEndStr )
	  {
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::VAS;
			return;
		} //  if

		const std::string sEnterography("enterograph");
		const std::string sBrain("brain");
		const std::string sHead("head");
		const std::string sChest("chest");

		u1 = sStudyDescription.find(sEnterography, 0 );
		u2 = sSeriesDescription.find(sEnterography, 0 );
		u3 = sAcquisProtocolName.find(sEnterography, 0 );
		u4 = sStudyDescription.find(sBrain, 0 );
		u5 = sSeriesDescription.find(sBrain, 0 );
		u6 = sAcquisProtocolName.find(sBrain, 0 );
		u7 = sStudyDescription.find(sChest, 0 );
		u8 = sSeriesDescription.find(sChest, 0 );
		u9 = sAcquisProtocolName.find(sChest, 0 );
		u10 = sStudyDescription.find(sHead, 0 );
		u11 = sSeriesDescription.find(sHead, 0 );
		u12 = sAcquisProtocolName.find(sHead, 0 );

	  if( u1 != uEndStr || u2 != uEndStr || u3 != uEndStr || 
			  u4 != uEndStr || u5 != uEndStr || u6 != uEndStr || 
				u7 != uEndStr || u8 != uEndStr || u9 != uEndStr ||
				u10 != uEndStr || u11 != uEndStr || u12 != uEndStr )
	  {
		  m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XP;
			return;
		} //  if

	} // modality = CT

	if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::VC && m_vImgSeriesVolInfo.iNumOfSubVol > 1 )
		m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XP;

} // DetermineAppType()


/**
 *  extract cardiac phase percentage information
 * 
 *  @param iSubVolIndex
 *  @return string in percentage: if it is "-1", it means no info was provided
 */
std::string CvnConverter::ExtractCardiacPhaseInfo( const int4 & iSubVolIndex )
{
	const int4 iNumOfImgInSeries( m_images->Count() );

	m_images->Reset();
	m_images->MoveNext();
	std::shared_ptr<dcm::IDcmImage> pDcmImage;

	/// determine sub volume image number
	for( int4 i=0; i<iNumOfImgInSeries; i++ )
	{
		if( m_vvImageInfoCollection[i].iSubVolumeLabel == iSubVolIndex ) 
		{
			pDcmImage = m_images->Current();
	   
			if( pDcmImage == nullptr )
		    throw CvnException(LogRec("Dicom image collection was corrupted", "CvnConverter", "CreateSubVol"));

			i = 99999;  // exit the for loop
		}
		else
		{
			m_images->MoveNext();
		}
	} // for
	
	bool bGetPhaseNumber(false);

	std::string sPhase("");

  if( m_vImgSeriesVolInfo.iNumOfSubVol > 1 )
  {
		float4 f = (float4(iSubVolIndex) * (1.0f / float4(m_vImgSeriesVolInfo.iNumOfSubVol))) * 100.0f;

		sPhase = ToAscii( int4(f) ) + "%";
		return sPhase;
  } // if
  
	std::string sCarPhase("");
  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9241), sCarPhase, true );
  std::transform(sCarPhase.begin(), sCarPhase.end(), sCarPhase.begin(), std::tolower);

  if( sCarPhase.size() > 0 )
  {
	  return( sCarPhase );
  } // if

  std::string sSeriesDescription("");
  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x103E), sSeriesDescription, true );
  std::transform(sSeriesDescription.begin(), sSeriesDescription.end(), sSeriesDescription.begin(), std::tolower);

  uint u;
  const uint uEndStr = std::string::npos;

  if( sSeriesDescription.size() > 3 )
  {
	  u = sSeriesDescription.find( "%", 0 );

	  if( u != uEndStr )
	  {
		  if( sSeriesDescription[u-2] == '.' && u > 3 )
			  sPhase = sSeriesDescription.substr( u-4, 4 );
		  else
			  sPhase = sSeriesDescription.substr( u-2, 2 );

		  return( sPhase );
	  } // if

  } // if
  
	std::string sImageComments("");
  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0020, 0x4000), sImageComments, true );
  std::transform(sImageComments.begin(), sImageComments.end(), sImageComments.begin(), std::tolower);

  if( sImageComments.size() > 3 )
  {
	  u = sImageComments.find( "%", 0 );

	  if( u != uEndStr )
	  {
		  if( sImageComments[u-2] == '.' && u > 3 )
			  sPhase = sImageComments.substr( u-4, 4 );
		  else
			  sPhase = sImageComments.substr( u-3, 3 );

			return( sPhase );;
    } // if					
  } // if

  /// handle vendor specific private dicom attributes
  std::string sVendorName("");
  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x0070), sVendorName, true );
  std::transform(sVendorName.begin(), sVendorName.end(), sVendorName.begin(), std::tolower);
	
  if( sVendorName == "toshiba" )
  {
	  std::string sPhasePercent("");
    pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1004), sPhasePercent, true );
	  std::transform(sPhasePercent.begin(), sPhasePercent.end(), sPhasePercent.begin(), std::tolower);

	  std::string sRRmeanTime("");
	  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1003), sRRmeanTime, true );
	  std::transform(sRRmeanTime.begin(), sRRmeanTime.end(), sRRmeanTime.begin(), std::tolower);
	    
	  std::string sPhaseInTime("");
	  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1005), sPhaseInTime, true );
	  std::transform(sPhaseInTime.begin(), sPhaseInTime.end(), sPhaseInTime.begin(), std::tolower);

	  if( sPhasePercent.size() > 1 )
	  { /// phase percentage info is available explicitly
		  sPhase = sPhasePercent;
		  return( sPhasePercent );
	  }
    else
	  { /// both times are available for calculation
		  if( sRRmeanTime.size() > 2 && sPhaseInTime.size() > 2 )
		  {
			  u = sRRmeanTime.find( "ms", 0 );
			  std::string s1 = sRRmeanTime.substr(0, u);
			  u = sPhaseInTime.find( "ms", 0 );
			  std::string s2 = sPhaseInTime.substr(0, u);

			  if( s1.size() > 0 && s2.size() > 0 )
			  {
				  float4 f1 = float4(atoi(s1.c_str()));
				  float4 f2 = float4(atoi(s2.c_str()));

				  if( f1 > 0.0f )
				  {
					  sPhase = ToAscii( int4(f2/f1*100.0f) );
					  return( sPhase );
				  } // if
				} // if 
			} // if
		} // else

  } // if it is toshiba

  /// no phase percentage information was available
  if( !bGetPhaseNumber )
  {
	  int4 i = -1;
	  sPhase = ToAscii(i);
  } // if

	return( sPhase );
} //  ExtractCardiacPhaseInfo()


/**
 * Callback will pass through to the relay
 */
void CvnConverter::CallbackRelay(CvnEvents eEventType, const std::string & sMessage )
{
  m_parameters.EventType = eEventType;
  strncpy_s(m_parameters.Message, sizeof(m_parameters.Message), sMessage.c_str(), sMessage.size());

  // Callback relay will call the parents threads callback
  CallbackRelay(m_parameters);

  RtlZeroMemory(m_parameters.Message, sizeof(m_parameters.Message));

} //  Callback(CvnEvents eEventType, const std::string & sMessage = "");

