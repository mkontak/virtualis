// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen,   dongqing@viatronix.net

#include "stdafx.h"
#include "Timer.h"
#include "CvnEnums.h"
#include "CvnLog.h"
#include "CvnConverter.h"
#include "CvnExceptions.h"
#include "CvnExceptions.h"
#include "vxBlockVolumeIterator.h"
#include "CvnSubVolumeInfo.h"
#include <algorithm>


using namespace cvn;


/**
 * Determine the pixel bias
 *
 *  @param iSubVolIndex the sub volume index
 *  @param pDcmImgBuffer the pointer to the dicom image data content
 */
void CvnConverter::DetermineBias_enh( const int4 & iSubVolIndex, std::shared_ptr<short> & pDcmImgBuffer )
{
  std::string sManufacturer = m_images->GetManufacturer();
  std::string sModality     = m_images->GetModality();
  float4 fRescaleIntercept  = m_images->GetRescaleIntercept();

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

  register int4 i, j, k, iMin, iHUmin, iPixelPaddingValue;

  m_iBias = SHRT_MAX;

  // Gets the frame based on the stack id 
  dcm::DcmStackMap stacks = m_images->GetStacks();   

  // frame image pointer
  std::shared_ptr<dcm::IDcmImage> pFrameImage;

	if( sModality == "CT" )
	{  // CT HU value has special meaning for tissue type. It shall be treated uniquely.
		  
		i = 0;
		j = 0;

		// obtain sub-volume specific information
    std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
    {
      /// Get the indexes defined for the stack (This is the defined index from the dicom header)
      std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		  item.second->Reset();

      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
				if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
				{
		      pFrameImage = item.second->Current();
		      if( pFrameImage == nullptr )
		      {
		        throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		      } //if

			    k = pFrameImage->GetPixelPaddingValue();
			    iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
          fRescaleSlope      = pFrameImage->GetRescaleSlope();
          fRescaleIntercept  = pFrameImage->GetRescaleIntercept(); 

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
				}  // frame is in the current sub-volume

				i++;
      } // end while loop through all images in the collection

    } ); // for

  } // if

  else  /// non-CT modality
  {
    register int4 iMax(SHRT_MIN);

		i = 0;
		j = 0;

		// obtain sub-volume specific information
    std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
    {
      /// Get the indexes defined for the stack (This is the defined index from the dicom header)
      std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		  item.second->Reset();

      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
				if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
				{
		      pFrameImage = item.second->Current();
		      if( pFrameImage == nullptr )
		      {
		        throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		      } //if

			    k = pFrameImage->GetPixelPaddingValue();
			    iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
          fRescaleSlope      = pFrameImage->GetRescaleSlope();
          fRescaleIntercept  = pFrameImage->GetRescaleIntercept(); 

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
				}  // frame is in the current sub-volume

				i++;
      } // end while loop through all images in the collection
		} );  // end for

	}  // else if not CT modality
  
  /// bias is an offset value that is greater than 0.
  m_iBias = (m_iBias < 0 ? abs(m_iBias) : 0 );

} // DetermineBias_enh()


/**
 *  Initialize block volume variables in the header for the iStackIndex sub volume.
 *
 * @param iSubVolIndex    Sub-volume index
 * @patam pVolume         Volume whose header is to be initialized
 */
void CvnConverter::InitializeHeader_enh( const int4 & iSubVolIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume )
{
  std::shared_ptr<dcm::DcmElement> element;

  // Set Dimensions/Units and Type
  pVolume->GetHeader().SetVolDim       ( m_viVolDim           );
  pVolume->GetHeader().SetVoxelUnits   ( m_vfVolVoxelUnits    );
  pVolume->GetHeader().SetVoxelTypeEnum( vxVolumeHeader::UINT2);

	try
  {
    // Gets the frame based on the stack id 
    dcm::DcmStackMap stacks = m_images->GetStacks();   

    // frame image pointer
    std::shared_ptr<dcm::IDcmImage> pFrameImage;
		  
		register int4 i(0);

		// obtain sub-volume specific information
    std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
    {
      /// Get the indexes defined for the stack (This is the defined index from the dicom header)
      std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		  item.second->Reset();

      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
				if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
				{
		      pFrameImage = item.second->Current();
		      if( pFrameImage == nullptr )
		      {
		        throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		      } //if

					stacks.end();
					break;
				} // if it is in the current sub-volume

				i++;
			} // end while()
		} ); // end for stacks

    // patient weight
    std::string sPatientWeight( "" );
  
    if ( pFrameImage->GetElements()->TryGetElement(DCM_PatientWeight, element, true) )
      sPatientWeight = element->GetValue<std::string>();

    // patient sex
    std::string sPatientSex( "" );

    if ( pFrameImage->GetElements()->TryGetElement(DCM_PatientSex, element, true) )
      sPatientSex = element->GetValue<std::string>();


    std::string sPatientAgeUnits( "Y" );
    std::string sPatientAge("" );

    if ( pFrameImage->GetElements()->TryGetElement(DCM_PatientAge, element, true) )
      sPatientAge = element->GetValue<std::string>();

    if( sPatientAge.size() > 0 )
    {
      int ch = (int)sPatientAge.back();
      sPatientAge.pop_back();

      if ( ch >= 0 && ch <= 9  )
        sPatientAgeUnits = ("" + toupper(ch));
    } // if
   
    // patient birth date
    std::string sPatientBirthDate("");
    pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0010, 0x0030), sPatientBirthDate, true );



    // patient History
    std::string sPatientHistory("" );
  
    if ( pFrameImage->GetElements()->TryGetElement(DCM_AdditionalPatientHistory, element, true) )
      sPatientHistory = util::Strings::Replace(util::Strings::Replace(element->GetValue<std::string>(),'\n',' '),'\r',' ');

    // operator Name
    std::string sOperatorName("");
  
    if ( pFrameImage->GetElements()->TryGetElement(DCM_OperatorsName, element, true) )
      sOperatorName = element->GetValue<std::string>();

    // referring phys.
    std::string sPhysicianName( pFrameImage->GetReferringPhysician().ToString() );
  
    // Series Instance UID
    std::string sSeriesInstanceUID( pFrameImage->GetSeriesInstanceUid() );

    // Study Instance UID
    std::string sStudyInstanceUID( pFrameImage->GetStudyInstanceUid() );

    // Institution name
    std::string sInstitutionName( pFrameImage->GetInstitutionName() );

    // Manufacturer
    std::string sManufacturer( pFrameImage->GetManufacturer() );

    // Manufacture Model
    std::string sManufacturerModel( pFrameImage->GetManufacturerModel() );

    // Patient Position
    std::string sPatientPosition( pFrameImage->GetPatientPosition() );
  
    // slice Thickness
    float fSliceThickness( 0.0F );

    if ( pFrameImage->GetElements()->TryGetElement(DCM_SliceThickness, element, true) )
      fSliceThickness = element->GetValue<float>();


    float fSliceSpacing( m_vfDcmPixelSpacing.m_z ); 
		  
    float fPitch = ( fSliceThickness != 0 ? fSliceSpacing / fSliceThickness : 0.0F );

    // space Between slices
    std::string sPitch( util::Strings::Format("%f",fPitch) );
  
    // KVP
    std::string sKVP( "" );
  
    if ( pFrameImage->GetElements()->TryGetElement(DCM_KVP, element, true) )
      sKVP = element->GetValue<std::string>();

    // mA
    std::string sMA( "" );
 
    if ( pFrameImage->GetElements()->TryGetElement(DCM_XRayTubeCurrent, element, true) )
      sMA = element->GetValue<std::string>();

    // Reconstruction diameter
    std::string sReconstructionDiameter("" );

     if ( pFrameImage->GetElements()->TryGetElement(DCM_ReconstructionDiameter, element, true) )
      sReconstructionDiameter = element->GetValue<std::string>();

    std::string sFOV( sReconstructionDiameter.size() > 2 ? sReconstructionDiameter.substr(0,2) : "0" );

    // tilt
    std::string sTilt( "");

    if ( pFrameImage->GetElements()->TryGetElement(DCM_GantryDetectorTilt, element, true) )
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

    std::string sStudyDate( pFrameImage->GetStudyDate().ToString("yyyy-MM-dd") );
      
    dcm::DcmDate studyDate = pFrameImage->GetStudyDate();
    dcm::DcmTime studyTime = pFrameImage->GetStudyTime();

    // Study/Scan date
    vx::Date scanDate(studyDate.GetYear(), (uint1)studyDate.GetMonth(), (uint1)studyDate.GetDay(), (uint1)studyTime.GetHour(), (uint1)studyTime.GetMinutes(), studyTime.GetSeconds() );

    // Patient name
    m_sPatientName = pFrameImage->GetPatientName().ToString();
  
    // Patient ID
    std::string sPatientID( pFrameImage->GetPatientId() ); 
  
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
    std::string sHeader(pFrameImage->GetElements()->GetSerializedHeader());

    // Set General Header Variables
    varMapHeader.SetHdrVar("version",          iVersion,                        "Header version");
    varMapHeader.SetHdrVar("siteID",           m_iSiteId,                        "Site ID");
    varMapHeader.SetHdrVar("studyDate",        pFrameImage->GetStudyDate().ToString(),  "Study Date");
    varMapHeader.SetHdrVar("studyTime",        pFrameImage->GetStudyTime().ToString(),  "Study Time");
    varMapHeader.SetHdrVar("seriesDate",       pFrameImage->GetSeriesDate().ToString(), "Series Date");
    varMapHeader.SetHdrVar("seriesTime",       pFrameImage->GetSeriesTime().ToString(), "Series Time");
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
		i = static_cast<int4>(m_vImgSeriesVolInfo.sV3DAppType);
    varMapHeader.SetHdrVar("studyType",        i,                               "Study Type"); 
    varMapHeader.SetHdrVar("zOrderReversed",   0,                               "Slices are in reverse order");
    varMapHeader.SetHdrVar("manufacturer",     sManufacturer,                   "Device manufacturer");
    varMapHeader.SetHdrVar("bias",             m_iBias,                         "Viatronix bias value");

		/// vxVolumeHeader 5.0 contents
    varMapHeader.SetHdrVar("volumeOrigin",     m_vfOrigin,                      "RCS coordinate of volume origin");
		varMapHeader.SetHdrVar("subVolIndex",      iSubVolIndex,                    "Index of the sub volume");
		varMapHeader.SetHdrVar("numOfSubvol",      m_vNumOfFrameInSubVols.size(),"Total number of subvolume in series");
		varMapHeader.SetHdrVar("maxValueRange",    m_iMaxValueRange,                "Maximum voxel value range");

		if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::CAR )
      varMapHeader.SetHdrVar("cardiaPhase", ExtractCardiacPhaseInfo_enh(iSubVolIndex), "Cardiac Phase Percentage");

		/// serilized dicom header
    varMapHeader.SetHdrVar("dicomHeader",      util::Strings::Remove(sHeader,'\n'),          "Dicom Header Serialized");

    pVolume->GetHeader().SetVariableVolumeHeader(varMapHeader);

  }
  catch ( ... )
  {
    throw CvnException(LogRec("Failed to initialize the header, unspecified exception", "CvnConverter", "InitializeHeader"));
  }

} // InitializeHeader_enh()


/**
 *  Scale the dicom image pixel values. It may clamp CT HU values for rendering.
 *
 *  @param iSubVolIndex the sub volume index
 *  @param pDcmImgBuffer the pointer to the dicom image data
 *  @prarm puDcmImgBuffer the pointer to the unsigned short volume
 */
void CvnConverter::ScaleDcmImgPixel_enh( const int4 & iSubVolIndex, std::shared_ptr<short> &  pDcmImgBuffer, std::shared_ptr<uint2> & puDcmImgBuffer )
{
  // Gets the frame based on the stack id 
  dcm::DcmStackMap stacks = m_images->GetStacks();   

  // frame image pointer
  std::shared_ptr<dcm::IDcmImage> pFrameImage;

  std::string sModality = m_images->GetModality();

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

  register int4 i(0), j(0), k, iVoxelRaw;

	// obtain sub-volume specific information
  std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
  {
    /// Get the indexes defined for the stack (This is the defined index from the dicom header)
    std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

	  item.second->Reset();

    /// collecting unique image infomation for all slices
    while( item.second->MoveNext() )
    {
		  if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
			{
		    pFrameImage = item.second->Current();
		    if( pFrameImage == nullptr )
		    {
		      throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		    } //if

			  k = pFrameImage->GetPixelPaddingValue();
			  iPixelPaddingValue = (k == INT_MAX ? SHRT_MIN : k);
        fRescaleSlope      = pFrameImage->GetRescaleSlope();
        fRescaleIntercept  = pFrameImage->GetRescaleIntercept();

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

			    } // for k
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
          } // for k
			  } // else

			}  // frame is in the current sub-volume

			i++;
    } // end while loop through all images in the collection
	} );  // end for

} // ScaleDcmImgPixel_enh


/**
 *  create the sub volume for enhanced dicom series
 *
 *  @param iSubVolIndex the index for the current sub-volume
 */
void CvnConverter::CreateSubVol_enh( const int4 & iSubVolIndex )
{ 
	CvnLog::Debug("Start processing the " + ToAscii(iSubVolIndex+1) + "-th sub-volume", "CreateSubVol_enh", "CvnConverter_enh");

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
    m_parameters.Remaining = m_vNumOfFrameInSubVols[iSubVolIndex];
    m_parameters.Total     = m_vNumOfFrameInSubVols[iSubVolIndex];
 
    CallbackRelay(m_parameters);

    // Gets the frame based on the stack id 
    dcm::DcmStackMap stacks = m_images->GetStacks();   

    // frame image pointer
    std::shared_ptr<dcm::IDcmImage> pFrameImage;

		i = 0;
    // obtain sub-volume specific information
    std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
    {
      /// Get the indexes defined for the stack (This is the defined index from the dicom header)
      std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		  item.second->Reset();

      /// collecting unique image infomation for all slices
      while( item.second->MoveNext() )
      {
				if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
				{
		      pFrameImage = item.second->Current();
		      if( pFrameImage == nullptr )
		      {
		        throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		      } //if

		      /// Extract sub volume dimension information. 
		      /// Assume that all images have the identical dimensional parameter. So, do it for only one slice.
          m_vfNormalRow    = Normal3D<float4>( (float4)pFrameImage->GetImageOrientation().GetRowX(), 
				       (float4)pFrameImage->GetImageOrientation().GetRowY(), (float4)pFrameImage->GetImageOrientation().GetRowZ() );

			    m_vfNormalColumn = Normal3D<float4>( (float4)pFrameImage->GetImageOrientation().GetColumnX(), 
				    (float4) pFrameImage->GetImageOrientation().GetColumnY(), (float4)pFrameImage->GetImageOrientation().GetColumnZ() );
	    
			    m_viDcmImageDim.m_x = pFrameImage->GetColumns();
	        m_viDcmImageDim.m_y = pFrameImage->GetRows();

					std::string s("");
          pFrameImage->TryGetValue< std::string >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0030), s, true );
					uint4 n = s.find_first_of('\\');
					std::string sx = s.substr(0, n);
					std::string sy = s.substr(n+1, s.size()-1);
					
			    m_vfDcmPixelSpacing.m_x = (float4)atof(sx.c_str());
			    m_vfDcmPixelSpacing.m_y = (float4)atof(sy.c_str());

	        int4 iBitsStored(0);
	        /// check if the data storage in 12 bit 
	        if( ! pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0101), iBitsStored, true ) )
	        {
            throw CvnException(LogRec("Miss bits stored information.", "CvnFileConverter", "ExtractSubVolInfo"));	
	        } // if
	        else if( iBitsStored < 9 )
	        {
		        int4 iBitsAllocated(0);
		        if( pFrameImage->TryGetValue< int4 >( dcm::DcmUtility::CreateDcmTag(0x0028, 0x0100), iBitsAllocated, true ) && (iBitsAllocated == 8) )
		        {
			        m_bIsDcmByteStored = true;
		        }
		        else
		        {
              throw CvnException(LogRec("Image bits stored less than 8 is not supported.", "CvnFileConverter", "ExtractSubVolInfo"));	
		        }
	        } // else
	        else if( iBitsStored > 16 )
	        {
            throw CvnException(LogRec("Bits store was greater than 12", "CvnFileConverter", "ExtractSubVolInfo"));	
	        } // if
				
					/// terminate both while and stack loops
					stacks.end();
					
					break;
				}  // frame is in the current sub-volume

				i++;
      } // end while loop through all images in the collection

    } ); // for

	        
		/// set the dcm image number for the sub volume
		m_viDcmImageDim.m_z = m_vNumOfFrameInSubVols[iSubVolIndex];

		/// assume image in the series has identical image dimension
	  const int4 iDcmImgPixelNum( m_viDcmImageDim.m_x * m_viDcmImageDim.m_y );
	  const int4 iDcmVolSize    ( iDcmImgPixelNum * m_vNumOfFrameInSubVols[iSubVolIndex] );

		/// allocate memory for the sub volume dicom images
    pDcmVolData = std::shared_ptr<short>( new  short[iDcmVolSize],  [] ( short * value) {  delete [] value; } ); 
	  if( pDcmVolData == nullptr )
      throw CvnOutOfMemoryException(LogRec("Failed to allocate sub-volume", "CvnConverter", "CreateSubVol_enh"));

	  /// clear the sub volume image position container
	  if( m_vvfDcmImagePosSub.size() > 0 ) m_vvfDcmImagePosSub.clear();

		i = 0;
		int4 iOffset(0);

    /// get the sub-volume dicom image data
    std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
    {
      /// Get the indexes defined for the stack (This is the defined index from the dicom header)
      std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		  item.second->Reset();

      /// loop for all frame images
      while( item.second->MoveNext() )
      {
				if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
				{
		      pFrameImage = item.second->Current();
		      if( pFrameImage == nullptr )
		      {
		        throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		      } //if

			    m_vvfDcmImagePosSub.push_back( Vector3D<float4>(m_vvFrameInfoCollection[i].vfImagePosition.m_x, 
					                                                m_vvFrameInfoCollection[i].vfImagePosition.m_y, 
																												  m_vvFrameInfoCollection[i].vfImagePosition.m_z) );

				  if( m_bIsDcmByteStored )
				  { /// The dicom image pixel value is in byte
			      uint1 * puTmp = pFrameImage->GetPixelData()->GetData<uint1>();
			
			      for( j=0; j<iDcmImgPixelNum; j++, iOffset++)
			      {
				      pDcmVolData.get()[iOffset] = static_cast<short>(int4(puTmp[j]));				
			      } // for j
				  }
				  else
				  { /// the dicom image pixel value is in short
			      short * psTmp = pFrameImage->GetPixelData()->GetData<short>();
			
			      for( j=0; j<iDcmImgPixelNum; j++, iOffset++)
			      {
				      pDcmVolData.get()[iOffset] = psTmp[j];				
			      } // for j
				  } //else 

				}  // frame is in the current sub-volume

				i++;
      } // end while loop through all images in the collection

    } ); // for

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

		DetermineBias_enh( iSubVolIndex, pDcmVolData );
		
		/// we need an uint2 buffer to store modality value images
    puDcmVolData = std::shared_ptr<uint2>( new  uint2[iDcmVolSize],  [] ( uint2 * value) {  delete [] value; } ); 
	  if( puDcmVolData == nullptr )
	    throw CvnOutOfMemoryException(LogRec("Failed to allocate unsigned modality buffer", "CvnConverter", "CreateSubVol"));


		/// Scale the dicom image to modality value images. If the modality value range 
		/// is greater than USHRT_MAX, the pixel value will be clampped. 
		ScaleDcmImgPixel_enh( iSubVolIndex, pDcmVolData, puDcmVolData );

    // Creates the block volume and loads it into the shared_ptr
    std::shared_ptr<vxBlockVolume<uint2>> pVolume(std::make_shared<vxBlockVolume<uint2>>());

		// ======================================
    // Make sure the volume memory was allocated OK
    // =====================================
    if ( pVolume == __nullptr )
      throw CvnOutOfMemoryException(LogRec("Failed to allocate vxBlockVolume", "CvnConverter", "CreateSubVol"));

		InitializeHeader_enh( iSubVolIndex, pVolume ); 

	  /// Check the image plane orientation
		if( bIsOrthoPlane )
	  {
		  /// The image plane is orthogonal. 
		  EvaluateVol4OrthoImgs( pVolume, puDcmVolData );
	  } // if
	  else
	  {
		  /// none-orthogonal imaging plane
		  EvaluateVol4NoneOrthoImgs( pVolume, puDcmVolData );
	  } // else
		
    /// Make sure that we do not have more then 32 sub-volumes
		if( iSubVolIndex > 32 )
       throw CvnException(LogRec("More than 32 sub volume is unrealistic", "CvnConverter", "CreateSubVol"));
		
   // =================================================================================================================
    // Only add the volume into the list iff the file path is not specified. If the filepath is specified then
    // we are generating each volume as we go and writing them to disk.
    // =================================================================================================================
    std::shared_ptr<CvnSubVolumeInfo> subvolume( m_sVolumeFilePath.empty() ? 
                                                 CvnSubVolumeInfo::Create( iSubVolIndex, m_vNumOfFrameInSubVols[iSubVolIndex], pVolume) :
                                                 CvnSubVolumeInfo::Create( iSubVolIndex, m_vNumOfFrameInSubVols[iSubVolIndex], Write( m_sVolumeFilePath, iSubVolIndex, pVolume) ) );

    subvolume->SetApplicationType(m_vImgSeriesVolInfo.sV3DAppType);
		subvolume->SetImageCount(m_vNumOfFrameInSubVols[iSubVolIndex]);
    subvolume->IsFunctionalTemporalVolume(m_vImgSeriesVolInfo.bIsFunTempoVol);
    subvolume->SetSeriesUid(m_images->GetSeriesUid());

		std::shared_ptr<dcm::IDcmImage> pDcmImage = m_images->GetFirstImage();
	  std::string sSeriesDescription("");
	  pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x103E), sSeriesDescription, true );
	  std::transform(sSeriesDescription.begin(), sSeriesDescription.end(), sSeriesDescription.begin(), std::tolower);
		  
		subvolume->SetDescription( sSeriesDescription );
		subvolume->SetSubType( "default" );

		if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::CAR )
		  subvolume->AddProperty("phase", ExtractCardiacPhaseInfo_enh(iSubVolIndex), "Cardiac");

		m_pSubVolumes.insert(std::pair<uint4, std::shared_ptr<CvnSubVolumeInfo>>(iSubVolIndex, subvolume));

	
    // Set the callback parameters for a completed event
    m_parameters.EventType = E_COMPLETED;
    m_parameters.Completed = m_vNumOfFrameInSubVols[iSubVolIndex];
    m_parameters.Remaining = 0;
 
    // Callback relay will call the parents threads callback
    CallbackRelay(m_parameters);

	} // try
  catch ( ... )
  {

    CallbackRelay(E_FAILED, "Failed to create sub volume");

    throw CvnException(LogRec("Failed to create sub volume", "Volume", "CreateSubVol"));
  } // catch

} // CreateSubVols_enh()



/**
 *   determine v3D application type of the dicom series
 */
void CvnConverter::DetermineAppType_enh()
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
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1040), sContrastBolus, true );
	std::transform(sContrastBolus.begin(), sContrastBolus.end(), sContrastBolus.begin(), std::tolower);
		
	std::string sContrastRoute("");
	bool bContrastRoute(false);
	bContrastRoute = pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1040), sContrastRoute, true );
	std::transform(sContrastRoute.begin(), sContrastRoute.end(), sContrastRoute.begin(), std::tolower);
		
	std::string sContrastStartTime("");
	bool bContrastStartTime(false);
	bContrastStartTime = pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0018, 0x1042), sContrastStartTime, true );
	std::transform(sContrastStartTime.begin(), sContrastStartTime.end(), sContrastStartTime.begin(), std::tolower);

	std::string sImageComments("");
	pDcmImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0020, 0x4000), sImageComments, true );
	std::transform(sImageComments.begin(), sImageComments.end(), sImageComments.begin(), std::tolower);

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
	} // else

	/// per current CTC guideline, each body position scan must be a single series.
	if( m_vImgSeriesVolInfo.sV3DAppType == ApplicationTypes::VC && m_vNumOfFrameInSubVols.size() > 1 )
		m_vImgSeriesVolInfo.sV3DAppType = ApplicationTypes::XP;

} // DetermineAppType_enh()


/**
 *  extract cardiac phase percentage information
 * 
 *  @param iSubVolIndex
 *  @return string in percentage: if it is "-1", it means no info was provided
 */
std::string CvnConverter::ExtractCardiacPhaseInfo_enh( const int4 & iSubVolIndex )
{
  std::shared_ptr<dcm::DcmElement> element;

	// Gets the frame based on the stack id 
  dcm::DcmStackMap stacks = m_images->GetStacks();   

  // frame image pointer
  std::shared_ptr<dcm::IDcmImage> pFrameImage;
		  
	register int4 i(0);

	// obtain sub-volume specific information
  std::for_each( stacks.begin(), stacks.end(), [&] ( dcm::DcmStackMapItem item )
  {
    /// Get the indexes defined for the stack (This is the defined index from the dicom header)
    std::shared_ptr<dcm::DcmIndexList> indexes = item.second->GetIndexes();

		item.second->Reset();

    /// get first frame image for the subvolume
    while( item.second->MoveNext() )
    {
		  if( m_vvFrameInfoCollection[i].iSubVolumeLabel == iSubVolIndex )
			{
		    pFrameImage = item.second->Current();
		      
				if( pFrameImage == nullptr )
		    {
		      throw CvnException(LogRec("Dicom image collection was corrupted", "CvnFileConverter", "ExtractSubVolInfo"));	
		    } //if

				stacks.end();
				break;
			} // if it is in the current sub-volume

			i++;
		} // end while()

	} ); // end for stacks

	bool bGetPhaseNumber(false);

	std::string sPhase("");

	if( m_vNumOfFrameInSubVols.size() > 1 )
	{
		float4 f = (float4(iSubVolIndex) * (1.0f / float4(m_vNumOfFrameInSubVols.size()))) * 100.0f;

		sPhase = ToAscii( int4(f) ) + "%";

		return sPhase;
	}

	std::string sCarPhase("");
  pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0020, 0x9241), sCarPhase, true );
  std::transform(sCarPhase.begin(), sCarPhase.end(), sCarPhase.begin(), std::tolower);

  if( sCarPhase.size() > 0 )
  {
	  return( sCarPhase );
  } // if

	std::string sImageComments("");
  pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0020, 0x4000), sImageComments, true );
  std::transform(sImageComments.begin(), sImageComments.end(), sImageComments.begin(), std::tolower);

	const int uEndStr( std::string::npos);
	int u;

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
  pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x0008, 0x0070), sVendorName, true );
  std::transform(sVendorName.begin(), sVendorName.end(), sVendorName.begin(), std::tolower);
	
  if( sVendorName == "toshiba" )
  {
	  std::string sPhasePercent("");
    pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1004), sPhasePercent, true );
	  std::transform(sPhasePercent.begin(), sPhasePercent.end(), sPhasePercent.begin(), std::tolower);

	  std::string sRRmeanTime("");
	  pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1003), sRRmeanTime, true );
	  std::transform(sRRmeanTime.begin(), sRRmeanTime.end(), sRRmeanTime.begin(), std::tolower);
	    
	  std::string sPhaseInTime("");
	  pFrameImage->TryGetValue<std::string>( dcm::DcmUtility::CreateDcmTag(0x7005, 0x1005), sPhaseInTime, true );
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
} //  ExtractCardiacPhaseInfo_enh()
