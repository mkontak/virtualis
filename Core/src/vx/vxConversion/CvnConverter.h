// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing  dongqing@viatronix.net

#pragma once

#include "CvnCallback.h"
#include "DcmCallback.h"
#include "vxBlockVolume.h"
#include "Normal3D.h"
#include "CvnEnums.h"
#include "CvnLog.h"


namespace cvn
{

  /** For non-enhanced dicom image 
   *  data structure to contain the dicom image unique information to other slices
   */
  typedef struct ImageUniqeInfo 
  {
	  /// unique dicom attributes
	  int4           iImageNumber;
	  Triple<float4> vfImagePosition;
	  int4           iTemporalPosIdentifier;
		int4           iAcquisiNum;

	  /// sub-volume relationship index
	  int4 iSubVolumeLabel;

  } ImageUniqueInfo;


  /** For enhanced dicom image
   *  data structure to contain the dicom frame unique information to other slices
   */
  typedef struct FrameUniqeInfo 
  {
	  /// unique dicom attributes
	  int4           iStackId;
	  Triple<float4> vfImagePosition;
	  int4           iInStackIndex;
		int4           iTemporalIndex;

	  /// sub-volume relationship index
	  int4 iSubVolumeLabel;

  } FrameUniqueInfo;



  /**
	 *  Dicom image series volume information
	 */
	typedef struct SeriesVolInfo
	{
		/// number of sub volumes
		int4 iNumOfSubVol;

		/// application type, 
		ApplicationTypes sV3DAppType;

		/// flag for functional temporal volume
		bool bIsFunTempoVol;

	} SeriesVolInfo;


  // Forward declarations
  class CVN_DLL CvnSubVolumeInfo;

  /**
   * Implements a generic converter class. It converts a dicom image series into v3D volumes.
   * See CvnFileConverter and CvnPacsConverter for a more specific implementation.
   */
  class CVN_DLL CvnConverter 
  {

#pragma region constructors/destructors
    
  protected:

     /// Constructor
    CvnConverter( int iSiteId = 0 );

  public:

    /// Destructor
    virtual ~CvnConverter();

#pragma endregion


#pragma region methods


#pragma region Create methods

  public:

    /// Creates all volumes for the dataset and returns the list of volumes in memory
    virtual const std::map<uint4,std::shared_ptr<CvnSubVolumeInfo>>  & Create( std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction , LPVOID callbackData, const std::string & sVolumeFilePath = "" ) = 0 ;


#pragma endregion

    virtual HANDLE Start(  std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction = nullptr, LPVOID callbackData = nullptr, const std::string & sVolumeFilePath = "" ) = 0 ;


  public:

    /// Indicates that the volumes can be written
    const bool CanWrite() const 
    { return m_pSubVolumes.size() > 0; }

    /// Writes the volume file
    const std::vector<std::string> Write( const std::string & sVolumeFilePath );


  protected:

    // =====================================================================================================
    // Writes the volume file using the volume file path (Splits the path and generates the file name) 
    //  <ROOT><DIRECTORY><FILENAME>_<INDEX>.<EXTENSTION>
    // =====================================================================================================
    static std::string Write( const std::string & sVolumeFilePath, const int4 & iSubVolumeIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume );

    /// Generates the volume file path using the supplied volume path and sub volume index 
    static std::string GenerateSubVolumeFilePath(  const std::string & sVolumeFilePath, const int4 & iSubVolumeIndex );

    //// Create the volumes from input memory object of dicom image collections
    void Create( const dcm::DcmImageCollection & dcmImages );

   /// Callback based on event (Passes through to the Relay)
   void CallbackRelay( CvnEvents eEventType, const std::string & sMessage = "" );

    /// Relays the callback up
    virtual void CallbackRelay( const CVN_CALLBACK_PARAMETERS & parameters ) {}

	  /// create the sub volumes with conventional dicom, i.e non-enhanced dicom
	  void CreateSubVol( const int4 & iSubVolIndex );
	  /// create the sub volumes with enhanced dicom
		void CreateSubVol_enh( const int4 & iSubVolIndex );

		/// determine v3D application type of the dicom image series
		void DetermineAppType();
		void DetermineAppType_enh();


  private:

    /// Gets the pixel bias for the iSubVolIndex sub-volume
    void DetermineBias( const int4 & iSubVolIndex, std::shared_ptr<short> & pDcmImgBuffer );  
    void DetermineBias_enh( const int4 & iSubVolIndex, std::shared_ptr<short> & pDcmImgBuffer );  
	
    /// Initialize the header for the iSubVolIndex sub-volume
    void InitializeHeader( const int4 & iSubVolIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume );
    void InitializeHeader_enh( const int4 & iSubVolIndex, const std::shared_ptr<vxBlockVolume<uint2>> & pVolume  );

    /// scale the dicom image pixel value to the physical meaningful one for the iSubVolIndex sub volume
	  void ScaleDcmImgPixel( const int4 & iSubVolIndex, std::shared_ptr<short> &  pDcmImgBuffer, std::shared_ptr<uint2> &  puDcmImgBuffer );
	  void ScaleDcmImgPixel_enh( const int4 & iSubVolIndex, std::shared_ptr<short> &  pDcmImgBuffer, std::shared_ptr<uint2> &  puDcmImgBuffer );

	  /// Releases the member volume object in the memory
    void ReleaseSubVolumes();

	  /// Evaluate the output sub volume for orthogonal image plane
	  void EvaluateVol4OrthoImgs( const std::shared_ptr<vxBlockVolume<uint2>> & pVolume, std::shared_ptr<uint2> & pDcmImgBuffer );

	  /// Evaluate the output sub volume for non-orthogonal image plane
	  void EvaluateVol4NoneOrthoImgs( const std::shared_ptr<vxBlockVolume<uint2>> & pVolume,  std::shared_ptr<uint2> & pDcmImgBuffer );

	  /// calculate parameters for coordinate index conversion function. This is just for orthogonal image plane.
	  void CalculateParam4IndexFun();

	  /// calculate sub volume dimension parameters based on input dicom images. This is just for non-orthogonal image plane
	  void CalculateSubVolParams();

	  /// Orthogonal image plane coordiante index conversion utility. Applicable to all X, Y, and Z.
	  int4 GetDcmImgIndex( const int4 & iVolIndex, const int4 & iFormulaIndex );

    /// extract cardiac phase percentage information
    std::string ExtractCardiacPhaseInfo( const int4 & iSubVolIndex );
    std::string ExtractCardiacPhaseInfo_enh( const int4 & iSubVolIndex );


#pragma endregion

#pragma region properties

  public:

    /// Sets the default application type
    void SetDefaultApplication( const std::string & sDefaultApplication )
    { m_sDefaultApplication = sDefaultApplication; }

    /// Gets the default application
    const std::string & GetDefaultApplication() const
    { return m_sDefaultApplication; }

    /// Sets the log source
    void SetLogSource( const std::string & sLogSource ) 
    { CvnLog::SetLogSource(sLogSource); }

    /// Get the site ID
    const int GetSiteId() const
    { return m_iSiteId; }

    /// Sets the site id
    void SetSiteId( int iSiteId ) 
    { m_iSiteId = iSiteId; }
    
    /// Gets the volume files
		const std::map<uint4, std::shared_ptr<CvnSubVolumeInfo> > & GetCvnSubVolumeInfo() const
    { return m_pSubVolumes; }

    /// Get the volume file path
		const std::string & GetColumeFilePath() const
    { return m_sVolumeFilePath; }

    /// Sets the volume file path
		void SetVolumeFilePath( const std::string & sValue ) 
    { m_sVolumeFilePath = sValue; }

		/// get series sub volume infomation
		const SeriesVolInfo & GetSeriesInfo() const
    { return m_vImgSeriesVolInfo; }


#pragma endregion

#pragma region fields

  private:

    /// Bias used for pixel data of the vx block volume
    int4 m_iBias;

    /// Parameters 
    CVN_CALLBACK_PARAMETERS m_parameters;

    /// Site ID
    int m_iSiteId;

    /// Default application
    std::string m_sDefaultApplication;

  protected:

    /// Sub Volumes
    std::map<uint4, std::shared_ptr<CvnSubVolumeInfo>> m_pSubVolumes;

    /// The input DICOM series images
    std::shared_ptr<dcm::DcmImageCollection> m_images;

		/// The key information containter for DICOM images
	  std::vector< ImageUniqeInfo > m_vvImageInfoCollection;
	  std::vector< FrameUniqeInfo > m_vvFrameInfoCollection;

	  /// The destination volume path 
	  std::string m_sVolumeFilePath;

    /// Abort flag
    bool m_bAbort;

		/// flag to the dicom image type of byte
		bool m_bIsDcmByteStored;

		/// flag to the temporal identifier availability
		bool m_bTemporalInfoAvailable;

		/// flag to the multiple acquisition number shown
		bool m_bIsMultiAcquisition;

		/// Container for the series sub volume informaiton
		SeriesVolInfo m_vImgSeriesVolInfo;

		/// minimum image number for creating an volume
		int4 m_iMinImgNumber4Vol;

		/// container for number of frame in sub-volume
		std::vector< int4 > m_vNumOfFrameInSubVols;

    /// Series Uid
    std::string m_sSeriesUid;

		/// sub-volume description
		std::string m_sSubVolumeDescription;

		/// sub-volume application sub-type
		std::string m_sSubVolSubType;


  private:

    /// Patient Name
    std::string m_sPatientName;

	  /// the direction cosine in the patient RCS for the row of the DICOM image
	  Normal3D<float4> m_vfNormalRow;

	  /// the direction cosine in the patient RCS for the column of the DICOM image
	  Normal3D<float4> m_vfNormalColumn;

	  /// the direction cosine in the patient RCS for the direction that is perpendicular to the DICOM image plane
	  Normal3D<float4> m_vfNormalSlice;

	  /// dicom image pixel spacing
	  Triple<float4> m_vfDcmPixelSpacing;

	  /// dicom image position container for sub volume rather than all images inside the dicom series
	  std::vector< Vector3D<float4> > m_vvfDcmImagePosSub;

	  /// dicom image dimension
	  Triple<int4> m_viDcmImageDim;

	  /// voxel spacing in the patient RCS. It is the volume voxel units.
	  Triple<float4> m_vfVolVoxelUnits;

	  /// volume origin in the patient RCS
	  Vector3D<float4> m_vfOrigin;

	  /// volume dimension in the patient RCS
	  Triple<int4> m_viVolDim;

	  /// coordinate conversion function constants
    /// Revoked CalculateParameters4ConversionFunction() to initialize the following parameters
	  /// The constructor does not initialize them.
	  int4 m_iDcmW;
    int4 m_iDcmH;
    int4 m_iDcmNum;
	  int4 m_iXFormulaIndex;
	  int4 m_iYFormulaIndex;
	  int4 m_iZFormulaIndex;
	  int4 m_iXgap;
	  int4 m_iYgap;
	  int4 m_iZgap;

		/// range between maximum and minimum voxel value
		int4 m_iMaxValueRange;


		#pragma endregion

  };  // class CvnConverter

};  // namespace cvn