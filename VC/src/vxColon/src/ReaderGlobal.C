// $Id: ReaderGlobal.C,v 1.21.2.2 2010/12/20 18:53:26 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.net)

#include "StdAfx.h"
#include "Volume.h"
#include "BlockVolume.h"
#include "LinearVolume.h"
#include "Thresholds.h"
#include "Timer.h"
#include "RegGrow26Iterator.h"
#include "File.h"
#include "FileExtensions.h"
#include "SystemInfo.h"
#include "LocationLock.h"
#include "StudyLoad.h"
#include "SeriesVersion.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "FilePtrFactory.h"

using namespace ReaderLib;


#define READER_GLOBAL_OBJECT_DEFINED_LOCAL
#include "ReaderGlobal.h"


Dataset ReaderGlobal::m_nullDataset;

#define FILE_REVISION "$Revision: 1.21.2.2 $"

/**
 * Constructor.
 */
ReaderGlobal::ReaderGlobal(): m_pCurrDataset(&m_supine), m_pSecondaryDataset(&m_prone), m_bAllFeatures(false),
  m_bMatchedFeatures(false), m_bDisplayCadFindings(false), m_bAlwaysShowCadArrows(false),
  m_bShowExternalCadFindings(false), m_bDisplaySRTroi(false), m_bDisplaySRTroi2D(false), m_bToolbarOnBottomForWideScreen(true)
{
  m_bReadOnly = false;
	m_bDisplay2DRuler = true;
	m_bDual3DEndoFly = false;
	m_bNormalizeSnapshotDim = true;
  Reset();
  m_eIsLoading = DatasetOrientations::eNONE;
} // ReaderGlobal()


/**
 * Reset all data members.
 */
void ReaderGlobal::Reset()
{
  m_supine.Reset();
  m_prone.Reset();
  m_pCurrDataset = & m_supine;
  m_pSecondaryDataset = & m_prone;
  m_bSPLocked = rdrGlobal.GetSettings()->GetBoolValue( VCSettings::ViewerXML(), VCSettings::DefaultLinkedDatasets() );
  m_vProneOffset = Triple<int4>(0, 0, 0);

  m_bAbortLoad = false;
  m_bLoadAborted = false;
  m_bDualDataDetected = false;
  m_bDualSegmentsLoaded = false;
  m_bDualDataLoaded = false;

  m_bBiopsyMode = false;
  m_bSpaceLeaping = true;
  m_bRaycastPaintVol = false;
  m_bAlignCrossSectional = true;
  m_bObliqueShadow = false;
  m_bOrthogonalShadow = false;
  m_bAxialShadow = false;
  m_bSagittalShadow = false;
  m_bCoronalShadow = false;
  m_bSegmentHalfPlaneShadow = false;
  m_bGridShadow = false;
  m_bReportCreated = true;
  m_bHomogenityCreated = false;
  m_bShowHomogenity = false;
	m_bUseULDvolume = rdrGlobal.GetSettings()->GetBoolValue( VCSettings::ViewerXML(), VCSettings::UseULDVolume() );
	m_bEnableDualDisplayTabs = false;
	if( rdrGlobal.GetSettings()->GetValue( VCSettings::ViewerXML(), VCSettings::EnableDualDisplayTabs() ) == "EnableDualDisplayTabs" )
		m_bEnableDualDisplayTabs = true;
	m_bDisplay2DRuler = rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Draw2DRuler());
	m_bNormalizeSnapshotDim = rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::NormalizeSnapshot());
	m_bDual3DEndoFly = false;

  m_iColorScheme = 0;

  // Clear the location lock (prior to erasing knowledge of the path)
  //LocationLock::ClearLock(vx::File(m_sVolumeTitlePath).GetParent());
  m_sVolumeTitlePath.erase();
  
  m_vRegMatchingFeatures.clear();
} // Reset()


/**
 * FreeAllLargeVolumesButPaint.
 */
void ReaderGlobal::FreeAllLargeVolumesButPaint()
{
  m_supine.FreeAllLargeVolumesButPaint();
  m_prone.FreeAllLargeVolumesButPaint();
} // FreeAllLargeVolumesButPaint()


/**
 * Destructor.
 */
ReaderGlobal::~ReaderGlobal()
{
} // ~ReaderGlobal()


/**
 * Save all persistent data.
 *
 * @param bSavePaintVolumeData   Whether or not to save paint volume data.
 * @return  true if read succeeds.
 */
bool ReaderGlobal::SaveData(bool bSavePaintVolumeData)
{
  if (m_bReadOnly == true)
  {
    return true;
  } // if writable

	if( !m_supine.SaveSessionData(bSavePaintVolumeData) || 
		  !m_prone.SaveSessionData(bSavePaintVolumeData) ) 
  {
	  return false;
	} // if

  return true;
} // SaveData


/**
 * Set current study orientation (supine or prone).
 *
 * @param eDesiredOrientation   to switch to.
 */
void ReaderGlobal::SetDatasetOrientation(const DatasetOrientations::TypeEnum eDesiredOrientation)
{
  m_supine.m_submarine.FullStop();
  m_prone.m_submarine.FullStop();

  switch (eDesiredOrientation)
  {
  case DatasetOrientations::eSWAP:
    // swap orientations
    if (m_pSecondaryDataset->IsLoadingComplete())
    {
      Swap(m_pCurrDataset, m_pSecondaryDataset);
    } // endif both data loaded
    break;

  case DatasetOrientations::eSUPINE:
    // Set to supine
    if (m_supine.IsLoadingComplete())
    {
      m_pCurrDataset = &m_supine;
      m_pSecondaryDataset = &m_prone;
    } // endif supine data loaded
    break;

  case DatasetOrientations::ePRONE:
    // set to prone
    if (m_prone.IsLoadingComplete())
    {
      m_pCurrDataset = &m_prone;
      m_pSecondaryDataset = &m_supine;
    } // endif prone data loaded
    break;

  default:
    // no more relevant orientation settings
    LogErr("Bad case.", "ReaderGlobal", "SetDatasetOrientation");
    break;
  } // switch on desired orientation
} // SetDatasetOrientation()


/**
 * Retrieve any pre-processing hazards.
 *
 * @param   filePrefix   file name for this patient.
 * @return  list of hazards from V2K.
 */
std::list<Hazards::HazardEnum> ReaderGlobal::GetHazards(const std::string & sFilePrefix) const
{
  Hazards::m_hazardList.clear();
  Hazards::Read(sFilePrefix);

  // remove duplicates
  std::list<Hazards::HazardEnum> uniqueCopy = Hazards::m_hazardList;
  uniqueCopy.unique();
  return uniqueCopy;
} // GetHazards()




//#include <gdiplus.h>



/**
 * Gets the class ID for a image codec.
 *
 * @param sFormat      Indicates which codec to retrieve.
 * @param codecCLSID   [OUT] Corresponding codec class ID.
 * @return bool        Success/failure.
 */
bool ReaderGlobal::GetCodecClsid(const WCHAR* sFormat, CLSID& codecCLSID)
{
  UINT  uNumEncoders = 0;       // number of image encoders
  UINT  uArraySize = 0;         // size of the image encoder array in bytes

  Gdiplus::GetImageEncodersSize(&uNumEncoders, &uArraySize);
  if (uArraySize == 0)
  {
    return false;
  }

  Gdiplus::ImageCodecInfo* pImageCodecInfo = new Gdiplus::ImageCodecInfo[uArraySize];
  if (pImageCodecInfo == NULL)
  {
    return false;
  }

  Gdiplus::GetImageEncoders(uNumEncoders, uArraySize, pImageCodecInfo);

  for (int4 j = 0; j < uNumEncoders; ++j)
  {
    if (wcscmp(pImageCodecInfo[j].MimeType, sFormat) == 0)
    {
      codecCLSID = pImageCodecInfo[j].Clsid;
      delete []pImageCodecInfo;
      return true;
    }
  }

  delete []pImageCodecInfo;
  return false;
}


/**
 * Writes a JPEG image file with the given bitmap.
 *
 * @param uWidth     Width of the bitmap.
 * @param uHeight    Height of the bitmap.
 * @param puPixels   Image pixels.
 * @param sFile      Output filename.
 */
void ReaderGlobal::WriteJPEGImage(const uint2 uWidth, const uint2 uHeight, uint1* puPixels, BSTR sFile)
{
  ULONG_PTR pToken;
  Gdiplus::GdiplusStartupInput gsi;
  Gdiplus::GdiplusStartup(&pToken, &gsi, NULL);

  // calculate full buffer size assuming word aligned boundary
  static const uint1 uBytesPerPixel( 3 );
  const int4 iWordPaddedWidth = (uWidth + uBytesPerPixel) & ~uBytesPerPixel;
  const uint4 uImageSize(iWordPaddedWidth * uHeight);

  BITMAPINFO bi;
  memset(&bi, 0, sizeof(bi));
  bi.bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth      = uWidth;
  bi.bmiHeader.biHeight     = uHeight;
  bi.bmiHeader.biSizeImage  = uImageSize;
  bi.bmiHeader.biPlanes     = 1;
  bi.bmiHeader.biBitCount   = 24;

  Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromBITMAPINFO( &bi, static_cast<void*>(puPixels));

  if (pImage != NULL)
  {
    CLSID imgCLSID;
    if (GetCodecClsid(L"image/jpeg", imgCLSID))
    {
      pImage->Save(sFile, &imgCLSID, NULL);
    }
  }

  Gdiplus::GdiplusShutdown(pToken);
}


/**
 *  Called to setup default zoom factor for overview rendering.
 *
 * @param vDim    reference to volume dimension
 * @param vUnits  reference to volume units.
 */
void ReaderGlobal::SetupZoomFactorInOverview(const Triple<uint4> & vDim, const Triple<float4> & vUnits)
{
  // reference to the setting of study 'Lewis: z-size=360, default=24'.
  const float4 fSettingOfStudyLewis = 24.0F;
  const float4 fSizeOfDimZOfStudyLewis = 360.0F;
  const float4 fFactor = 0.055F;
  float4 fSizeX = vUnits.m_x * vDim.m_x;
  float4 fSizeY = vUnits.m_y * vDim.m_y;
  float4 fSizeZ = vUnits.m_z * vDim.m_z;
  float4 fMaxSize = (fSizeZ > fSizeY) ? fSizeZ : fSizeY;
  if (fSizeX > fMaxSize) fMaxSize = fSizeX;
  float4 fDefaultZoomFactor = fSettingOfStudyLewis + (fMaxSize - fSizeOfDimZOfStudyLewis) * fFactor;

  m_supine.m_trackballStandard.SetDefaultZoomFactor(fDefaultZoomFactor);
  m_supine.m_trackballStandard.SetZoomFactor(fDefaultZoomFactor);
  m_supine.m_trackballVerify.SetDefaultZoomFactor(fDefaultZoomFactor);
  m_supine.m_trackballVerify.SetZoomFactor(fDefaultZoomFactor);

  m_prone.m_trackballStandard.SetDefaultZoomFactor(fDefaultZoomFactor);
  m_prone.m_trackballStandard.SetZoomFactor(fDefaultZoomFactor);
  m_prone.m_trackballVerify.SetDefaultZoomFactor(fDefaultZoomFactor);
  m_prone.m_trackballVerify.SetZoomFactor(fDefaultZoomFactor);
} // SetupZoomFactorInOverview()


/**
 * Are the two datasets registered?
 *
 * @return   True if the two datasets have been registered already.
 */
bool ReaderGlobal::IsRegistered()
{
  return
    IsDualDataLoaded() &&
    m_prone.IsJoined() &&
    m_supine.IsJoined();
} // IsRegistered()


/**
 * Loads a study
 *
 * @param params         Study load parameters
 * @param loadCallback   Indicates progress thru load steps to method caller
 * @return               Success
 */
bool ReaderGlobal::LoadStudy2( StudyLoadParams & params, StudyLoad::LoadCallback loadCallback )
{
  // Set IsLoading() to be true for the scope of this function
  ReaderGlobal::SetResetVar<DatasetOrientations::TypeEnum> srv(m_eIsLoading, DatasetOrientations::eNONE, DatasetOrientations::eNONE);

  // initialize the studies
  Reset();
  

  m_supine.SetOrientation(DatasetOrientations::eSUPINE); // this may change after it reads the data from disk
  m_supine.m_trackballStandard.Init(m_supine.MatchOrientation(DatasetOrientations::eSUPINE));
  m_supine.m_trackballVerify.Init(m_supine.MatchOrientation(DatasetOrientations::eSUPINE));
  m_supine.SetVolumePrefix( DatasetLoadParams::FormatPrefixPath( params.GetPrimaryDataset().GetVolumeDirectory(), params.GetPrimaryDataset().GetPrefix() ) );
  m_supine.SetPreprocessedPrefix( DatasetLoadParams::FormatPrefixPath( params.GetPrimaryDataset().GetPreprocessedDirectory(), DatasetLoadParams::GeneratePrefix( params.GetPrimaryDataset().GetPreprocessedDirectory() ) ) );

  std::string sCadFile;
  DatasetLoadParams::FindFilename(params.GetPrimaryDataset().GetCadDirectory(), "*.cad.xml", sCadFile);  
  m_supine.SetCadPrefix( DatasetLoadParams::FormatPrefixPath( params.GetPrimaryDataset().GetCadDirectory(), sCadFile ) );

  m_supine.SetSessionPrefix( DatasetLoadParams::FormatPrefixPath( params.GetSessionDirectory(), params.GetPrimaryDataset().GetPrefix() ) );
  LogDbg( "Primary Vol: " + m_supine.GetVolumePrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Primary Pre: " + m_supine.GetPreprocessedPrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Primary CAD: " + m_supine.GetCadPrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Primary Ssn: " + m_supine.GetSessionPrefix(), "ReaderGlobal", "LoadStudy2" );

  m_prone.SetOrientation(DatasetOrientations::ePRONE); // this may change after it reads the data from disk
  m_prone.m_trackballStandard.Init(m_prone.MatchOrientation(DatasetOrientations::ePRONE));
  m_prone.m_trackballVerify.Init(m_prone.MatchOrientation(DatasetOrientations::ePRONE));
  m_prone.SetVolumePrefix( DatasetLoadParams::FormatPrefixPath( params.GetSecondaryDataset().GetVolumeDirectory(), params.GetSecondaryDataset().GetPrefix() ) );
  m_prone.SetPreprocessedPrefix( DatasetLoadParams::FormatPrefixPath( params.GetSecondaryDataset().GetPreprocessedDirectory(), DatasetLoadParams::GeneratePrefix( params.GetSecondaryDataset().GetPreprocessedDirectory() ) ) );
  m_prone.SetCadPrefix( DatasetLoadParams::FormatPrefixPath( params.GetSecondaryDataset().GetCadDirectory(), DatasetLoadParams::GeneratePrefix(params.GetSecondaryDataset().GetCadDirectory(), vx::XMLExt ) ) );
  m_prone.SetSessionPrefix( DatasetLoadParams::FormatPrefixPath( params.GetSessionDirectory(), params.GetSecondaryDataset().GetPrefix() ) );
  LogDbg( "Secndry Vol: " + m_prone.GetVolumePrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Secndry Pre: " + m_prone.GetPreprocessedPrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Secndry CAD: " + m_prone.GetCadPrefix(), "ReaderGlobal", "LoadStudy2" );
  LogDbg( "Secndry Ssn: " + m_prone.GetSessionPrefix(), "ReaderGlobal", "LoadStudy2" );

  DatasetLoadParams & rDSparams = (params.GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID) ? params.GetPrimaryDataset() : params.GetSecondaryDataset();

  if( io::FilePtrFactory::Exists(rDSparams.GetPreprocessedDirectory() + "\\version.xml") )
  { 
    try
    {
      // set the series versions
      std::string sVersionNum;

      try
      {
        SeriesVersion sv = SeriesVersion::FromFile( rDSparams.GetPreprocessedDirectory() + "\\version.xml" );
        m_supine.SetSeriesVersion( sv );
        m_prone.SetSeriesVersion( sv );
        LogDbg( "Loading series version: " + ToAscii( atof( m_supine.GetSeriesVersion().GetVersion().c_str() ) ), "ReaderGlobal", "LoadStudy2" );

        sVersionNum = sv.GetVersion();
      }
      catch( ... )
      {
        LogErr("Failed to read the series version file", "ReaderGlobal", "LoadStudy2" );
        return false;
      }

      if( atof( sVersionNum.c_str() ) >= 5.1F )
      { 
        // see if both supine and prone studies exist
        uint4 uWhichToLoad = DatasetOrientations::eNONE;
        if( ( params.GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID )
          && (io::FilePtrFactory::Exists(m_supine.GetPreprocessedPrefix() + vx::CleansedDifferenceVolExt)) )
        {
          uWhichToLoad |= DatasetOrientations::eSUPINE;
        }
        if( ( params.GetFlags() & StudyLoadParams::SECONDARY_DATASET_VALID )
          && (io::FilePtrFactory::Exists(m_prone.GetPreprocessedPrefix() + vx::CleansedDifferenceVolExt)))
        {
          uWhichToLoad |= DatasetOrientations::ePRONE;
        }

        m_eIsLoading = DatasetOrientations::TypeEnum( uWhichToLoad );


        if( m_eIsLoading == DatasetOrientations::eNONE )
        {
          LogErr("No CDV file found","ReaderGlobal","LoadStudy2");
          throw ex::FileNotFoundException(LogRec("Study does not exist or could not be accessed.", "ReaderGlobal", "LoadStudy2"));
        }

        // set up the global pointers; supine is always first if it exists
        m_pCurrDataset = (m_eIsLoading & DatasetOrientations::eSUPINE) ? (& m_supine) : (& m_prone);
        m_pSecondaryDataset = (m_eIsLoading & DatasetOrientations::eSUPINE) ? (& m_prone)  : (& m_supine);

        // see if secondary data files exist
        m_bDualDataDetected = (m_eIsLoading == DatasetOrientations::eBOTH);
         
        Timer loadPatientTimer; loadPatientTimer.Reset();

        bool bSetZoomDefault(true);

        // Load the study's data
        try
        {
          m_supine.m_uLoadPercent = 0;
          m_prone.m_uLoadPercent  = 0;
          
          if( m_eIsLoading & DatasetOrientations::eSUPINE )
            m_supine.LoadStep3( Dataset::eLoadHeader, loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_prone.m_uLoadPercent, bSetZoomDefault );
          
          if( m_eIsLoading & DatasetOrientations::ePRONE )
            m_prone.LoadStep3( Dataset::eLoadHeader, loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_supine.m_uLoadPercent, bSetZoomDefault );
           

          if ((m_supine.m_vDim.m_z + m_prone.m_vDim.m_z) > 950 && SystemInfo::GetTotalPhysicalMemory() <= 1024*1024*1024)
          {
            loadCallback.NextStep(StudyLoad::eLARGESIZE_QUERY, 2, DatasetOrientations::eNONE);
            if( m_bAbortLoad )
              throw ex::AbortLoadException(LogRec("User choose to abort large Study", "ReaderGlobal", "LoadPatient"));
          }

          Timer loadTimer;

          uint4* pLoadStep = SystemInfo::GetTotalPhysicalMemory() > 1024*1024*1024? Dataset::vLoadStage2GB : Dataset::vLoadStage1GB;
          for (uint4 uStep = 1; uStep < Dataset::GetLoadStepsCount(); uStep++)
          {
            if (m_eIsLoading & DatasetOrientations::eSUPINE)
            {
              m_supine.LoadStep3(pLoadStep[uStep], loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_prone.m_uLoadPercent, bSetZoomDefault);
            } 
            
            if (m_eIsLoading & DatasetOrientations::ePRONE )
            {
              m_prone.LoadStep3(pLoadStep[uStep], loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_supine.m_uLoadPercent, bSetZoomDefault);
            } 

            LogFyi("Load step " + ToAscii(pLoadStep[uStep]) + ": " + loadTimer.AsString(), "ReaderGlobal", "LoadStudy");
            loadTimer.Reset();
          } // endfor all load steps
  
        } // try to load all data		
        // catch possible error conditions
        catch (ex::AbortLoadException)
        {
          m_bAbortLoad = false;
          Reset();
          m_bLoadAborted = true;
          return false;
        } // catch load abort
        catch (ex::VException & error)
        {
          LogRec(error.GetLogRecord().GetTextMessage(), "ReaderGlobal", "LoadPatient");
          throw;
        }
        catch (...)
        {
          throw ex::IOException(LogRec("error loading study", "ReaderGlobal", "LoadPatient"));
        }

        std::strstream loadString;
        loadString << "Loading :>)" << ((m_eIsLoading&(DatasetOrientations::eSUPINE|DatasetOrientations::ePRONE)) ? "Both" : "One") <<
          " datasets from " << m_supine.GetVolumePrefix().c_str() << " took " << loadPatientTimer.AsString().c_str() << std::ends;
        LogFyi(loadString.str(),"ReaderGlobal","LoadPatient");

        m_sVolumeTitlePath = m_supine.GetVolumePrefix();

        m_bDualDataLoaded = ((m_eIsLoading & DatasetOrientations::eBOTH) == DatasetOrientations::eBOTH);

        loadCallback.NextStep(StudyLoad::eLOAD_COMPLETE, 0, DatasetOrientations::eNONE);

        return (m_eIsLoading != DatasetOrientations::eNONE);
      }
    }
    catch ( ... )
    {
      LogErr("Failed to load the study", "ReaderGlobal", "LoadStudy2" );
      return false;
    }
  } // if new series version file exist
  else
  {
    LogDbg( "The processed series version file does not exist", "ReaderGlobal", "LoadStudy2" );

    // see if both supine and prone studies exist
    uint4 uWhichToLoad = DatasetOrientations::eNONE;
    if( ( params.GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID )
      && (io::FilePtrFactory::Exists(m_supine.GetVolumePrefix() + vx::ElecCleansedVolExt) || io::FilePtrFactory::Exists(m_supine.GetPreprocessedPrefix() + vx::ElecCleansedVolExt)))
    {
      uWhichToLoad |= DatasetOrientations::eSUPINE;
    }
    if( ( params.GetFlags() & StudyLoadParams::SECONDARY_DATASET_VALID )
      && (io::FilePtrFactory::Exists(m_prone.GetVolumePrefix() + vx::ElecCleansedVolExt) || io::FilePtrFactory::Exists(m_prone.GetPreprocessedPrefix() + vx::ElecCleansedVolExt)))
    {
      uWhichToLoad |= DatasetOrientations::ePRONE;
    }

    m_eIsLoading = DatasetOrientations::TypeEnum( uWhichToLoad );

    if( m_eIsLoading == DatasetOrientations::eNONE )
    {
      LogErr("No VOL or CDV file found","ReaderGlobal","LoadStudy");
      throw ex::FileNotFoundException(LogRec("Study does not exist or could not be accessed.", "ReaderGlobal", "LoadStudy"));
    }

    // set up the global pointers; supine is always first if it exists
    m_pCurrDataset = (m_eIsLoading & DatasetOrientations::eSUPINE) ? (& m_supine) : (& m_prone);
    m_pSecondaryDataset = (m_eIsLoading & DatasetOrientations::eSUPINE) ? (& m_prone)  : (& m_supine);

    // See if secondary data files exist
    m_bDualDataDetected = (m_eIsLoading == DatasetOrientations::eBOTH);
     
    Timer loadPatientTimer; loadPatientTimer.Reset();

    bool bSetZoomDefault(true);

    // Load the study's data
    try
    {
      m_supine.m_uLoadPercent = 0;
      m_prone.m_uLoadPercent = 0;
      
      if( m_eIsLoading & DatasetOrientations::eSUPINE )
        m_supine.LoadStep2( Dataset::eLoadHeader, loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_prone.m_uLoadPercent, bSetZoomDefault );
      
      if( m_eIsLoading & DatasetOrientations::ePRONE )
        m_prone.LoadStep2( Dataset::eLoadHeader, loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_supine.m_uLoadPercent, bSetZoomDefault );
       
      
      if ((m_supine.m_vDim.m_z + m_prone.m_vDim.m_z) > 950 && SystemInfo::GetTotalPhysicalMemory() <= 1024*1024*1024)
      {
        loadCallback.NextStep(StudyLoad::eLARGESIZE_QUERY, 2, DatasetOrientations::eNONE);
        if( m_bAbortLoad )
          throw ex::AbortLoadException(LogRec("User choose to abort large Study", "ReaderGlobal", "LoadPatient"));
      }

      Timer loadTimer;

      uint4* pLoadStep = SystemInfo::GetTotalPhysicalMemory() > 1024*1024*1024? Dataset::vLoadStage2GB : Dataset::vLoadStage1GB;
      for (uint4 uStep = 1; uStep < Dataset::GetLoadStepsCount(); uStep++)
      {
        if (m_eIsLoading & DatasetOrientations::eSUPINE)
        {
          m_supine.LoadStep2(pLoadStep[uStep], loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_prone.m_uLoadPercent, bSetZoomDefault);
        } 
        
        if (m_eIsLoading & DatasetOrientations::ePRONE )
        {
          m_prone.LoadStep2(pLoadStep[uStep], loadCallback, (m_eIsLoading == DatasetOrientations::eBOTH), m_supine.m_uLoadPercent, bSetZoomDefault);
        } 

        LogFyi("Load step " + ToAscii(pLoadStep[uStep]) + ": " + loadTimer.AsString(), "ReaderGlobal", "LoadStudy");
        loadTimer.Reset();
      } // endfor all load steps
    } // try to load all data

    // catch possible error conditions
    catch (ex::AbortLoadException)
    {
      m_bAbortLoad = false;
      Reset();
      m_bLoadAborted = true;
      return false;
    } // catch load abort
    catch (ex::VException & error)
    {
      LogRec(error.GetLogRecord().GetTextMessage(), "ReaderGlobal", "LoadPatient");
      throw;
    }
    catch (...)
    {
      throw ex::IOException(LogRec("error loading study", "ReaderGlobal", "LoadPatient"));
    }

    std::strstream loadString;
    loadString << "Loading :<(" << ((m_eIsLoading&(DatasetOrientations::eSUPINE|DatasetOrientations::ePRONE)) ? "Both" : "One") <<
      " datasets from " << m_supine.GetVolumePrefix().c_str() << " took " << loadPatientTimer.AsString().c_str() << std::ends;
    LogFyi(loadString.str(),"ReaderGlobal","LoadPatient");

    m_sVolumeTitlePath = m_supine.GetVolumePrefix();

    m_bDualDataLoaded = ((m_eIsLoading & DatasetOrientations::eBOTH) == DatasetOrientations::eBOTH);

    loadCallback.NextStep(StudyLoad::eLOAD_COMPLETE, 0, DatasetOrientations::eNONE);
  }

  return (m_eIsLoading != DatasetOrientations::eNONE);
} // LoadStudy2


// $Log: ReaderGlobal.C,v $
// Revision 1.21.2.2  2010/12/20 18:53:26  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.1  2007/06/05 23:36:16  jmeade
// don't output session file or marking volume for studies older than session version 3.0.
//
// Revision 1.21  2007/03/06 01:50:36  jmeade
// Issue 5445: fixed bug with locating cad file.
//
// Revision 1.20  2006/12/21 16:22:34  dongqing
// Add the switch for display SRT ROI on/off
//
// Revision 1.19  2006/10/04 20:54:54  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.18  2006/09/26 18:48:57  jmeade
// Ensure proper failure return in LoadStudy method.
//
// Revision 1.17  2006/09/13 15:22:04  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.16  2006/09/05 16:02:07  jmeade
// version specific loading for all versions greater than identified.
//
// Revision 1.15  2006/07/27 18:01:38  jmeade
// Issue 4891: allow load of secondary-dataset-only study sessions.
//
// Revision 1.14  2006/06/29 16:14:06  jmeade
// fixed "not all control paths return a value" warning; comments.
//
// Revision 1.13  2006/06/22 14:01:24  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.12  2006/06/07 19:21:19  frank
// checking version
//
// Revision 1.11  2006/05/12 20:30:22  jmeade
// Issue 4795: use proper prefix for cad xml file.
//
// Revision 1.10  2006/04/24 17:45:41  jmeade
// new study directory structure.
//
// Revision 1.9  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.8  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.7  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.6  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5  2005/11/04 18:08:06  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.4.2.2  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.4.2.1  2005/11/04 18:23:08  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.5  2005/11/04 18:08:06  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.4  2005/10/13 18:11:32  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.3  2005/10/12 22:38:37  jmeade
// option to always show cad findings arrows in 3D
//
// Revision 1.2  2005/10/04 17:33:11  jmeade
// display cad findings study-wide global variable.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.5  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.4  2005/08/22 13:40:36  vxconfig
// Fixed compile time error.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.41.2.2.4.4.2.1  2005/07/13 17:27:52  geconomos
// Added method to indicate if the two datsets have been registered
//
// Revision 3.41.2.2.4.4  2005/06/14 22:13:58  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.41.2.2.4.3.4.1  2005/06/14 17:57:16  jmeade
// debug load timer.
//
// Revision 3.41.2.2.4.3  2005/04/19 14:04:46  geconomos
// proper initialization of m_bSPLocked
//
// Revision 3.41.2.2.4.2  2005/04/13 18:23:37  jmeade
// Issue 4117: Identify which datasets are loading.
//
// Revision 3.41.2.2.4.1  2005/03/23 22:16:36  frank
// Issue #3983: Revamped the slice linking for all ortho directions
//
// Revision 3.41.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.41.2.1  2003/07/10 17:49:02  jmeade
// Comments.
//
// Revision 3.41  2003/02/05 18:32:50  kevin
// Make Registration Debug Global
//
// Revision 3.40  2002/11/24 17:50:51  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.39  2002/11/21 23:55:13  jmeade
// Code standard on m_trackball* variables.
//
// Revision 3.38  2002/10/31 20:03:18  kevin
// 1) Free up memroy on close to allow paint vol to be written to linear-vol file
// 2) Add load step so post-load init is always at the end even for both memory configurations
//
// Revision 3.37  2002/10/23 00:24:09  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 3.36  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.35  2002/09/29 21:44:15  kevin
// Re-Factor of SUpine/Prone registration ot place data structures
// in more meaningful places and remove so much copying of
// large data strucutres (there was a lot of creating local copies of
// refernces passed into toomany classes).
//
// Suzi and Ana's original registration is still presetn. In fact the registration
// still runs off their data structures. I jsut added new ones. From here on, I will
// have to break theres. So I wanted a version I could always go back
// to in case I broke anything.
//
// Revision 3.34  2002/09/27 19:07:05  jmeade
// Removed unnecessary function call, code clarify.
//
// Revision 3.33  2002/09/13 17:21:32  jmeade
// Function name change.
//
// Revision 3.32  2002/09/13 00:47:14  jmeade
// ColonSession for appropriate data; call SaveData() dataset function on study close.
//
// Revision 3.31  2002/09/11 19:16:34  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.30  2002/08/30 19:01:27  kevin
// Move S/P Registration debug output to C:\V-system
//
// Revision 3.29  2002/08/28 19:23:25  kevin
// Lots of updates to segmental viewing
//
// Revision 3.28  2002/08/08 16:28:50  jmeade
// Like girls moving furniture: Logger --> vxLogger.
//
// Revision 3.27  2002/07/18 19:56:29  jmeade
// Coding standards, clarified a variable name/meaning.
//
// Revision 3.26  2002/07/15 21:01:23  suzi
// Registration refinements.
//
// Revision 3.25  2002/07/15 15:49:51  jmeade
// Coding standards; variables localized to where appropriate.
//
// Revision 3.24  2002/07/03 03:23:41  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.23  2002/06/19 19:50:01  ana
// Bugs fixes in the new registration.
//
// Revision 3.22  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.21  2002/06/06 19:13:09  suzi
// Major refactoring job.
//
// Revision 3.20  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.19  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.18  2002/04/12 02:13:35  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.17  2002/03/28 18:46:44  jmeade
// Removed includes from Viatron project.
//
// Revision 3.16  2002/03/13 21:56:40  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.15  2002/02/05 22:13:36  binli
// Proper setting of default zoom factor in trackball according to the volume size.
//
// Revision 3.14.2.10  2002/06/19 19:06:27  jmeade
// Issue 2331:  Proper deletion of allocated memory.
//
// Revision 3.14.2.9  2002/06/06 17:55:35  kevin
// Fixed small bug in loading progress meter afer I rearanged 2GB loading
//
// Revision 3.14.2.8  2002/06/06 01:36:36  kevin
// Added check for 2GB of memeory. If this is present we can load the linear volumes
// first since they will nto swap out. This helps with Address space fragmentation
// since the large temporary "uncompres" buffers are allocated earlier. This is
// required now that we use a graphics card which takes 128MB of address space
// for it's on baord texture memory that is mapped into the processes address sapce.
//
// Revision 3.14.2.7  2002/05/31 03:04:13  jmeade
// Issue 2331:  Expose (declare as static) functions for snapshot.
//
// Revision 3.14.2.6  2002/04/02 16:59:36  jmeade
// Issue 2026: Allow open of single dataset from Study Info.
//
// Revision 3.14.2.5  2002/02/27 18:59:00  kevin
// Added user message Box for large studies (Note: had to pass
// mesage back to main thread to get the box displayed from
// the worker load thread)
//
// Revision 3.14.2.4  2002/02/27 15:29:45  binli
// issue 1699: refixed (refer Study.C Revision 3.13.2.4)
//
// Revision 3.14.2.3  2002/02/25 16:04:32  kevin
// Dramatically changed the patient loading mechanism. This is required to fix
// memory erros both in physicial memory and in adress space fragmentation.
// Moved the control of the loading order to Study class. Fields now only
// handles mutiple volumes in one load. ReaderGlobal now only calls Study
// in a loop which allows Supine and Prone loads to be interleaved (the
// part in Readglobal was simplified alot and the logic placed in study so that
// all the logic is in one place in case we want to play with the order of
// loading in the future)
//
// Also added pre-allocation of all the linear volumes before address is fragmented.
//
// Also added timing info into the LogFyi log file so that we can track the time it
// takes to laod even in the field.
//
// Revision 3.14.2.2  2002/02/11 15:14:37  binli
// issue 1699 in branch: setting default zoom factor setting in trackball according to volume size
//
// Revision 3.14.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.14  2002/01/22 22:55:48  jmeade
// Issue 1577: Remove max-time lock while loading (in case app is ambushed during load).
//
// Revision 3.13  2002/01/22 19:45:58  jmeade
// Don't erase the volume title path before clearing the location lock.
//
// Revision 3.12  2001/12/29 00:13:22  jmeade
// Merge from 1.1 branch.
//
// Revision 3.11  2001/12/21 22:50:31  jmeade
// Fixed out-of-bounds array access in WriteJPEG image.
//
// Revision 3.10  2001/12/04 15:51:01  binli
// changed function name in Trackball (by Jeff M)
//
// Revision 3.9  2001/11/19 13:48:38  kevin
// Some S/P Orientation Debug
//
// Revision 3.8  2001/11/19 13:17:52  kevin
// Fixed some of the problems with S/P orientation when segments are flipped
//
// Revision 3.7  2001/11/15 20:54:00  ana
// Took out some copies of the data used during registration. Didn't finish yet.
//
// Revision 3.6  2001/11/14 19:35:07  ana
// Each call to registration handler has to send the direction flag.
//
// Revision 3.5  2001/11/14 02:29:18  kevin
// More CLenaup of SUpine/Prone and added in orientation
// according to  submarine "lookahaed" algrotihm
//
// Revision 3.4  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.3  2001/11/09 04:50:08  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2  2001/10/29 14:13:42  kevin
// Added Jpeg Picture saving and tidied up avi/mpeg generation
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.2  2001/11/14 02:03:01  jmeade
// Issue 1320:  Lock study to disallow deletion while open and during transfer
//
// Revision 3.0.2.1  2001/10/30 23:27:33  jmeade
// Throw a file not found exception if the study files are not found.
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.5   Oct 03 2001 20:06:22   jmeade
// User load abort should throw AbortLoadException, not AbortOperationEx; spelling errors in exception comment
// 
//    Rev 2.4   Oct 03 2001 18:11:04   binli
// bug fix: error when open study from CD
// fix: readonly for all level custom.
// (.vrx file exists, open it, else, create new temporary file, but no save it)
// 
//    Rev 2.3   Oct 03 2001 11:54:50   jmeade
// Removed commented code
// 
//    Rev 2.2   Oct 03 2001 11:12:30   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar : added a flag variable.
// 
//    Rev 2.1   Oct 01 2001 16:10:10   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:40:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:04   ingmar
// Initial revision.
// Revision 1.234  2001/09/07 16:01:08  jmeade
// Bug fix:  Supine/prone load will fail only if *both* datasets fail to load
//
// Revision 1.233  2001/08/31 18:57:23  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.232  2001/08/24 20:03:20  frank
// Synchronized exceptions.
//
// Revision 1.231  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.230  2001/08/13 01:40:01  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.229  2001/07/26 20:29:49  binli
// ID 89: exception handling.
//
// Revision 1.228  2001/07/25 22:52:16  binli
// ID 89: new exception handling.
//
// Revision 1.227  2001/07/24 22:41:58  jmeade
// No need to reconstruct the error message (it's already constructed)
//
// Revision 1.226  2001/07/19 15:07:35  binli
// caught 'Abort' exception
//
// Revision 1.225  2001/07/18 21:25:42  binli
// no WarnLog info.
//
// Revision 1.224  2001/07/18 21:25:02  binli
// no WarnLog info.
//
// Revision 1.223  2001/07/18 20:53:42  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.222  2001/07/16 23:46:52  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.221  2001/07/13 19:52:37  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.220  2001/07/12 21:12:40  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.219  2001/06/29 23:01:18  jmeade
// no message boxes should be present other than in Viatron project
//
// Revision 1.218  2001/06/27 18:22:58  jmeade
// bool variable set only when LoadPatient() fn is active, for querying with IsLoading() fn
//
// Revision 1.217  2001/06/06 20:16:15  jmeade
// LoadPatient() throws exceptions (file not found so far)
//
// Revision 1.216  2001/05/25 20:28:57  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.211  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.210  2001/05/11 19:09:34  jmeade
// Coding conventions
//
// Revision 1.209  2001/05/09 18:34:47  liwei
// variable ReaderGlobal::m_bInteractingOverview deleted
// Unnecessary raycasting in translucent mode avoided.
//
// Revision 1.208  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.207  2001/05/02 21:39:43  jmeade
// corrections after mergeLostApril26 merge
//
// Revision 1.206  2001/05/02 18:07:05  dave
// re-named stream initialization classes
//
// Revision 1.205  2001/04/30 12:31:09  frank
// Added exception handling.
//
// Revision 1.203  2001/04/23 18:10:59  frank
// Logged streams to disk if no GUI is present.
//
// Revision 1.202  2001/04/19 11:56:05  frank
// Patched MRU bug.  Why does the MRU store a trailing "." now?
//
// Revision 1.201  2001/04/18 15:47:59  frank
// Output stream class has buggy destructor...removed console window.
//
// Revision 1.200  2001/04/13 13:29:02  frank
// Started using the streams interface.
//
// Revision 1.199  2001/04/13 00:48:55  jmeade
// change in Hazards for code standards
//
// Revision 1.198  2001/04/13 00:37:47  jmeade
// simplied IsSupineFilePrefix(...)  (substring length 1 is just character); file extensions
//
// Revision 1.197  2001/04/12 19:20:49  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.196  2001/04/12 15:58:20  dave
// added namespace vx for exceptions
//
// Revision 1.195  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.194  2001/04/11 17:09:15  frank
// Made streams work.
//
// Revision 1.193  2001/04/11 14:14:20  frank
// Removed debugging.
//
// Revision 1.192  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.191  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.190  2001/04/11 12:09:10  frank
// Worked on implementing the new output streams.
//
// Revision 1.189  2001/04/10 17:52:21  frank
// New coding conventions in Submarine class.
//
// Revision 1.188  2001/04/07 04:45:30  jmeade
// Bug: Interface reporting error on user abort.  Fix:  Better throw and catch
// of abort exception in ReaderGlobal, Study, Fields
//
// Revision 1.187  2001/04/06 13:53:23  frank
// Reflected changes in SegmentList class.
//
// Revision 1.186  2001/03/28 13:58:56  frank
// Reflected changes in SegmentList class.
//
// Revision 1.185  2001/03/26 17:34:32  frank
// Changed method of choosing whether to display console.
//
// Revision 1.184  2001/03/26 16:05:15  frank
// Reflected changes in Segment class.
//
// Revision 1.183  2001/03/21 15:01:26  frank
// More coding guideline compliance....
//
// Revision 1.182  2001/03/20 18:41:58  frank
// Worked on coding guideline compliance.
//
// Revision 1.181  2001/03/20 15:57:00  frank
// Updated to reflect new coding standards.
//
// Revision 1.180  2001/03/19 15:29:22  frank
// Removed friend class.
//
// Revision 1.179  2001/03/19 11:59:55  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.178  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.177  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.176  2001/03/16 14:38:02  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.175  2001/03/09 01:10:13  jmeade
// save *both* bookmark lists; check validity (IsVolumeLoaded) of study prior to switching current study
//
// Revision 1.174  2001/03/01 01:57:09  jmeade
// Update total segment length in each study twice?  I'll assume that's a typo bug
//
// Revision 1.173  2001/02/28 17:54:16  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.172  2001/02/28 16:39:07  geconomos
// Added read only flag
//
// Revision 1.171  2001/02/27 17:59:16  jmeade
// Missed regions list (m_patchList) a member of Study class
//
// Revision 1.170  2001/02/27 00:27:24  jmeade
// CPoint ==> Point2D<>
//
// Revision 1.169  2001/02/22 01:35:19  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.168  2001/02/13 12:58:31  frank
// Rendered the measurement line profile using the original volume data instead of the e-cleansed data.
//
// Revision 1.167  2001/02/09 14:41:04  kevin
// Make the maskVol in detect missed patches a block volume
//
// Revision 1.166  2001/02/09 13:10:19  frank
// Cleared prior hazards before reading.
//
// Revision 1.165  2001/02/09 12:40:06  frank
// Cleared measurement lists in Reset.
//
// Revision 1.164  2001/02/08 19:01:21  jeff
// Bug fix: no error returned when study doesn't exist: return false if no
// studies found, or if no studies were loaded
//
// Revision 1.163  2001/02/07 13:46:36  frank
// Merged the UpdateOffset and UpdateTotalLength functions.
//
// Revision 1.162  2001/02/06 21:43:54  jeff
// clarified segment jump operations
//
// Revision 1.161  2001/02/06 14:13:48  frank
// Revamped study load progress display.
//
// Revision 1.160  2001/02/05 19:30:05  frank
// Moved hazard strings into string table.
//
// Revision 1.159  2001/02/05 13:30:21  frank
// Made a single global null study.
//
// Revision 1.158  2001/02/02 13:49:40  frank
// Who changed the supine/prone file convention...?
//
// Revision 1.157  2001/02/02 13:08:13  frank
// Started navigation at the rectum.
//
// Revision 1.156  2001/02/01 17:50:33  frank
// Modified to use the newer hazard warning system.
//
// Revision 1.155  2001/02/01 16:34:16  geconomos
// Added accecors to get current segment from an offset
//
// Revision 1.154  2001/02/01 13:41:35  frank
// Clarified warning messages.
//
// Revision 1.153  2001/02/01 12:54:55  frank
// Went with standard template library strings.
//
// Revision 1.152  2001/02/01 00:53:14  jeff
// oops, I removed a very necessary line
//
// Revision 1.151  2001/01/31 22:21:39  geconomos
// Added GetCurrentColonSegment() and ColonSegment enum
//
// Revision 1.150  2001/01/31 22:18:55  geconomos
// Added GetCurrentColonSegment() and ColonSegment enum
//
// Revision 1.149  2001/01/31 19:59:44  frank
// Displays pre-processing warnings during study load.
//
// Revision 1.148  2001/01/31 18:15:31  jeff
// Removed MFC dependencies in missed patch list calculation
//
// Revision 1.147  2001/01/31 14:09:45  frank
// Initialized the view position to the end of both primary and secondary studies.
//
// Revision 1.146  2001/01/29 20:02:21  frank
// Simplified code.
//
// Revision 1.145  2001/01/29 18:19:03  frank
// Changed to reflect Study class changes.
//
// Revision 1.144  2001/01/29 16:27:05  frank
// Increased opacity of profiles.
//
// Revision 1.143  2001/01/29 02:45:34  frank
// Added histogram rendering.  Improved profile visualization.
//
// Revision 1.142  2001/01/27 02:02:01  kevin
// 1) Fixed 90% missed Patch error
// 2) Moved GetPotenetil to fields from Submarine
// as part of this fix.
//
// Revision 1.141  2001/01/26 19:59:00  binli
// defect 000188: default Overview position: added bool variable m_bResetTrackball
//
// Revision 1.140  2001/01/26 12:10:01  frank
// Somebody broke the build??
//
// Revision 1.139  2001/01/26 03:22:06  jeff
// oops, must throw dataCorruption on error
//
// Revision 1.138  2001/01/26 02:46:14  jeff
// changed enum
//
// Revision 1.137  2001/01/26 02:03:01  jeff
// Overhauled ReaderGlobal::Load() to always load supine first (if present)
//
// Revision 1.136  2001/01/25 17:41:25  frank
// Indicated volume load completion.
//
// Revision 1.135  2001/01/25 16:04:13  jeff
// stop flying when orientation is switched
//
// Revision 1.134  2001/01/25 15:32:01  frank
// Added DualSegmentAvailable flag.
//
// Revision 1.133  2001/01/25 00:04:39  jeff
// m_bDualDataAvailable ==> m_bDualDataDetected;
// hey Frank, quit moving m_bDualDataDetected = true, I need it set ASAP! :)
//
// Revision 1.132  2001/01/24 19:18:19  liwei
// Software volume rendering for overview
//
// Revision 1.131  2001/01/24 16:41:41  frank
// Upgraded study load functions for supine/prone.
//
// Revision 1.130  2001/01/24 15:55:19  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.129  2001/01/24 14:00:51  frank
// Rendered tick marks on measurement line profile view.
//
// Revision 1.128  2001/01/24 01:00:38  jeff
// must set m_bDualDataAvailable to true!!!!
//
// Revision 1.127  2001/01/23 20:46:39  binli
// changed the arrow color in sliceviewer.
//
// Revision 1.126  2001/01/23 19:47:20  frank
// Exposed information about the availability of both supine and prone datasets.
//
// Revision 1.125  2001/01/23 19:35:49  frank
// Removed excess warning boxes.
//
// Revision 1.124  2001/01/23 19:29:14  binli
// improved the visibility of arrow.
//
// Revision 1.123  2001/01/23 17:44:12  jeff
// enums should be capitalized; SetStudyOrientation() - more generalized than SwapStudy()
//
// Revision 1.122  2001/01/23 15:49:51  frank
// Removed question about loading dual because of memory.
// I hope you have lots of GB...
//
// Revision 1.121  2001/01/22 19:11:19  frank
// Moved the profile line out of the way of the measurement line.
//
// Revision 1.120  2001/01/22 12:52:42  frank
// Fixed supine-prone data similarity.
//
// Revision 1.119  2001/01/20 18:19:07  kevin
// Fixed the hanging when detecting missed regions
// when there are none.
//
// Revision 1.118  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.117  2001/01/18 17:45:42  binli
// removed 'trackball' defined in the class
//
// Revision 1.116  2001/01/12 22:38:45  frank
// Horizontal and vertical profiles.
//
// Revision 1.115  2001/01/12 21:48:26  frank
// Swapped ends when appropriate.
//
// Revision 1.114  2001/01/12 19:53:16  frank
// Moved code from NavigationView
//
// Revision 1.113  2001/01/11 23:46:20  kevin
// Added navigation based missed patch viewing
//
// Revision 1.112  2001/01/11 15:34:21  frank
// Cleaned up load logic.
//
// Revision 1.111  2001/01/11 15:29:33  frank
// Moved static member elsewhere.
//
// Revision 1.110  2001/01/11 13:08:44  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.109  2001/01/09 18:23:27  frank
// Animated centerline.
//
// Revision 1.108  2001/01/09 15:20:47  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.107  2001/01/09 13:28:35  frank
// Added automatic dual dataset recognition.
//
// Revision 1.106  2001/01/08 18:42:59  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.105  2001/01/08 02:42:14  kevin
// Added alternative method to align missed patch viewing and auto-choosing of best alternative
//
// Revision 1.104  2001/01/05 20:13:49  frank
// Added Supine/Prone swap function.
//
// Revision 1.103  2001/01/05 19:27:39  frank
// Moved marking functions into Study class.
//
// Revision 1.102  2001/01/05 18:48:42  frank
// Improved readability of Load code.
//
// Revision 1.101  2000/12/29 14:46:00  binli
// reset trackball on study reset
//
// Revision 1.100  2000/12/28 19:50:04  kevin
// Added new algo to compute viewpoint and dir for missed regions
//
// Revision 1.99  2000/12/22 14:55:52  frank
// Removed spurious warnings
//
// Revision 1.98  2000/12/19 14:49:40  frank
// Added "loading..." message during initialization of painting
//
// Revision 1.97  2000/12/19 02:30:34  kevin
// Added Grid Shadow
//
// Revision 1.96  2000/12/18 23:38:39  kevin
// Removed 2D unwrap-painting and clenaed up it's memory and flags.
// (NOTE; This seemed to be causing a weird memory crash where
// there was memory, but the call to new was creating an out-of-mem
// exception and there was plenty of memory left. Since this code never
// caused problems before, I am concerned that this is really some
// other problem rearing it's ugly head)
//
// Revision 1.95  2000/12/12 18:01:14  frank
// Removed spurious warning upon loading secondary volume header
//
// Revision 1.94  2000/11/25 03:04:57  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.93  2000/11/24 20:48:52  kevin
// Fixed bug on jump before missed patches found
//
// Revision 1.92  2000/11/22 19:46:01  frank
// Moved study completion to after marking surfaces
//
// Revision 1.91  2000/11/22 18:45:35  liwei
// Lighting of 3D arrow tuned
//
// Revision 1.90  2000/11/22 19:17:09  kevin
// Made auto-detect missed regions work only when over 90% of the colon has been visualized
//
// Revision 1.89  2000/11/22 16:55:50  frank
// Updated progress meter with all data fields
//
// Revision 1.88  2000/11/22 14:20:05  frank
// Nibbled off the ends of the skeleton when they are too close to the boundary
//
// Revision 1.87  2000/11/22 12:37:20  frank
// Added study load aborting
//
// Revision 1.86  2000/11/21 22:58:02  liwei
// 3D arrow is blended to slices if it is facing inside-out
//
// Revision 1.85  2000/11/21 19:56:40  frank
// Cleaned up code
//
// Revision 1.84  2000/11/21 15:39:05  frank
// Fixed overview synchronization problem
//
// Revision 1.83  2000/11/20 22:24:38  liwei
// Disable GL_BLEND before draw arrow. Otherwise it is blended in HybridView
//
// Revision 1.82  2000/11/20 18:35:24  frank
// Matrix fun
//
// Revision 1.81  2000/11/20 19:05:34  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.80  2000/11/20 14:35:54  frank
// Added function to quickly select among different segments
//
// Revision 1.79  2000/11/20 03:36:45  kevin
// Switched to "paint volume" schema.
//
// Revision 1.78  2000/11/19 23:47:05  kevin
// Changed RGBvol to PaintVol to reflect new meaning
//
// Revision 1.77  2000/11/18 18:36:55  kevin
// Removed #if for sld counting and painting (didn't work)
//
// Revision 1.76  2000/11/17 21:18:52  kevin
// FIxed some errors from changing the rendersize to be non-power-of-two
//
// Revision 1.75  2000/11/17 19:13:20  frank
// Improved arrow rendering
//
// Revision 1.74  2000/11/17 18:26:50  frank
// Cleaned up
//
// Revision 1.73  2000/11/17 16:51:26  binli
// Made sure if the second study (segments and volume head at present) can't be opened,
// the second window in VerifyView would not be blank. ---- temporal version.
//
// Revision 1.72  2000/11/17 14:32:18  frank
// Fixed arrow direction
//
// Revision 1.71  2000/11/16 22:41:33  binli
// Simply registation of Supine  and Prone.
//
// Revision 1.70  2000/11/16 20:23:58  frank
// Improved arrow shape and rendering...still wrong direction
//
// Revision 1.69  2000/11/16 19:48:00  jeff
// SystemInfo class now stores number of processors
//
// Revision 1.68  2000/11/16 19:26:28  frank
// Added arrow color
//
// Revision 1.67  2000/11/16 18:49:48  frank
// Added RenderArrow function
//
// Revision 1.66  2000/11/16 18:15:00  kevin
// Made surface counting 6-connected and dissalawed a few voxels on border of the volume from counting
//
// Revision 1.65  2000/11/16 15:46:34  frank
// Recomputed tangents and lengths after smoothing
//
// Revision 1.64  2000/11/16 15:28:38  jeff
// LoadCallback class to keep track of study load state information
// Number of processors data member in ReaderGlobal, used to determine
//  whether to use threads for rendering
//
// Revision 1.63  2000/11/16 11:24:07  frank
// Clarified debug messages
//
// Revision 1.62  2000/11/15 21:51:56  liwei
// The errors in scaling vli volume corrected
// Axial slices loaded earlier so that vli volume can be created earlier
//
// Revision 1.61  2000/11/15 19:13:45  liwei
// Add features to read transfer funciton from file
//
// Revision 1.60  2000/11/15 14:03:36  frank
// Moved some functions to inline file
//
// Revision 1.59  2000/11/14 23:53:04  liwei
// Add support for VolumePro
//
// Revision 1.58  2000/11/14 20:32:33  jeff
// Reset m_mmReport in ReaderGlobal::Reset() method
//
// Revision 1.57  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.56  2000/11/14 18:35:37  binli
// added * assignment function
//
// Revision 1.55  2000/11/14 17:25:02  frank
// Fixed problems in skeleton smoothing
//
// Revision 1.54  2000/11/14 04:38:33  kevin
// Changed marking and painting to utilize bits within the ins vol
//
// Revision 1.53  2000/11/13 20:01:08  frank
// Barlett filtered smoothing algorithm
//
// Revision 1.52  2000/11/13 19:25:01  binli
// moved some (Supine/Prone related) stuffs from 'ReaderGlobal' to 'Study'.
//
// Revision 1.51  2000/11/13 18:27:35  frank
// Added constraints to skeleton smoothing.
//
// Revision 1.50  2000/11/13 12:12:31  frank
// Added skeleton smoothing function
//
// Revision 1.49  2000/11/10 22:35:52  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.48  2000/11/10 19:43:11  frank
// Read in secondary volume header information
//
// Revision 1.47  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.46  2000/11/09 17:41:59  frank
// Added reading for the secondary study
//
// Revision 1.45  2000/11/09 14:53:05  frank
// Increased robustness
//
// Revision 1.44  2000/11/09 02:55:04  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.43  2000/11/08 21:15:32  jeff
// Put volume header info into reader global (so as to be accessible elsewhere)
//
// Revision 1.42  2000/11/08 20:05:58  frank
// Added m_secondary to complement primary m_study
//
// Revision 1.41  2000/11/08 19:45:50  frank
// Added m_supine and m_prone to Study and resolved upon Load
//
// Revision 1.40  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.39  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.38  2000/11/06 21:26:27  liwei
// Minor changes for 2D painting
//
// Revision 1.37  2000/11/06 20:03:48  jeff
// Pushed reading of slice data to end of the Load() method
//
// Revision 1.36  2000/11/06 16:18:47  liwei
// 2D painting is displayed with hardware
//
// Revision 1.35  2000/11/03 21:42:51  liwei
// m
//
// Revision 1.34  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
// Revision 1.33  2000/11/03 19:34:24  frank
// Removed dead code
//
// Revision 1.32  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.31  2000/11/02 19:45:10  kevin
// Somehow (e.g. CVS MERGE SUCKS) the u and v coordinates
// of unwrapped image creation got switched back to the version
// from a few weeks ago.
//
// Revision 1.30  2000/11/02 17:43:04  kevin
// Changed Recursive region grow to Iterator
//
// Revision 1.29  2000/11/01 22:41:21  jeff
// Have to notify the calling interface properly once a step has completed
//
// Revision 1.28  2000/10/31 23:51:14  jeff
// Reset() methods, encapsulating all the initializing done in constructor,
// now also called prior to load
//
// Revision 1.27  2000/10/31 14:37:40  frank
// Merged in new segment structure
//
// Revision 1.26  2000/10/29 19:42:03  kevin
// Added X-Sec alignment functionality
//
// Revision 1.25  2000/10/29 17:42:47  kevin
// FIxed up visible surface determination with bug fixes
// and additoin of missed region detection
//
// Revision 1.24  2000/10/27 01:13:32  kevin
// Lots of changes to Marking (and cleaned up volume measurement a lttile bit)
//
// Revision 1.23  2000/10/25 17:41:45  jeff
// Save(), GetCurrentDistanceFromRectum() functions
//
// Revision 1.22.2.1  2000/10/26 13:28:19  frank
// Added new segment structure
//
// Revision 1.22  2000/10/20 17:14:20  kevin
// Added debug for load timing (and changed the organization
// of some of the different projection methods)
//
// Revision 1.21  2000/10/12 21:05:04  kevin
// Added Axial, Sagittal and Coronal Flags. Also added direcitonal
// flag to semgnet jumping
//
// Revision 1.20  2000/10/12 02:46:30  kevin
// Added start at Cecum end (as weel as start at Rectum end)
//
// Revision 1.19  2000/10/11 15:02:56  kevin
// Changes for rdrGloabl current seg/skel pointers and added next segment prev segment
//
// Revision 1.18  2000/10/09 23:41:02  kevin
// Changes for volume header read
//
// Revision 1.17  2000/10/05 00:01:27  jeff
// Added an info read step (in Load() fn), for volume header read
//
// Revision 1.16  2000/10/04 14:30:18  kevin
// Added  rdrGlobal units and dim for fast
// reading for study load speed
//
// Also rearranged reads for new VolumeHeader
//
// Revision 1.15  2000/10/03 00:55:27  kevin
// Updated Angle and Volume Measurement and Changed
// the preload order (temp fix for volume header parameters)
//
// Revision 1.14  2000/10/02 23:02:55  jeff
// Moved bookmark list to ReaderGlobal (instance)
// Added a Reset() (re-initialize) method
//
// Revision 1.13  2000/10/02 21:56:59  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.12  2000/09/30 20:35:22  jeff
// Notifications to main application during study load process
//
// Revision 1.11  2000/09/27 16:18:05  frank
// Moved segment data structures to v2k
//
// Revision 1.10  2000/09/21 19:40:53  kevin
// Changes to unwrapped display
//
// Revision 1.9  2000/09/21 08:16:28  ingmar
// removed specific header again
//
// Revision 1.8  2000/09/19 16:37:09  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
//
// Revision 1.7  2000/09/19 12:53:33  frank
// Added memory allocation check
//
// Revision 1.6  2000/09/18 15:04:54  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.5  2000/09/18 12:17:07  ingmar
// code beautify and removal of obsolete SpecificHeaderPointer
//
// Revision 1.4  2000/09/13 15:17:55  binli
// fitted to the Viatronix C++ coding conventions and standards
//
// Revision 1.3  2000/09/13 14:55:05  kevin
// Got Volume Measurement to work
//
// Revision 1.2  2000/09/11 18:09:06  kevin
// updated volume measurement hooks
//
//
// 63    9/09/00 1:55p Kevin
// Added angular measurement and hooks for volume measurement
//
// 62    9/07/00 4:02p Frank
// Added new colon unwrapping features:
// -green composited overlay
// -continuous examination confidence (compared to binary)
// -percent examined
//
// 61    9/06/00 3:10p Frank
// Added data structure to store examined 2D unwrapped region
//
// 60    9/06/00 11:12a Kevin
// Fixed marking of unraveled image
//
// 59    9/02/00 4:21p Kevin
// Heaps of changes to Unwrap colon function
//
// 58    9/02/00 12:34p Kevin
// Added some differnt methods for unwrapping
//
// 57    9/01/00 4:45p Kevin
// Updates for colon unwrapping
//
// 56    9/01/00 4:55p Frank
// Worked on unwrapping the colon wall
//
// 55    9/01/00 10:37a Frank
// Initial work on unwrapping the colon wall
//
// 54    8/31/00 4:55p Kevin
// FIxed m_treshold due to change in Thresholds class
//
// 53    8/31/00 4:19p Frank
// Added spherical projection visualization mode
//
// 52    8/29/00 1:25p Frank
// Added corrupt dataset detection
//
// 51    8/28/00 11:40a Antonio
// updated header
//
//*****************  Version 50  *****************
//User: Jay             Date:  8/22/00  Time:  4:42p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Thresholds removed from Thresholder to Volume
//ReaderGlobal creates an instance of Thresholds
//
//*****************  Version 49  *****************
//User: Jeff            Date:  8/22/00  Time: 10:32a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  The m_flying flag is kept in Submarine member (m_submarine)
//
//*****************  Version 48  *****************
//User: Kevin           Date:  8/21/00  Time:  5:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added scaleing (window/level-like functionality) of the translucent
//transfer function
//
//*****************  Version 47  *****************
//User: Kevin           Date:  8/21/00  Time:  3:41p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added segment length checking
//
//*****************  Version 46  *****************
//User: Ingmar          Date:  8/20/00  Time:  8:25p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  removed hdr prefix.
//
//*****************  Version 45  *****************
//User: Frank           Date:  8/18/00  Time: 11:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colored orthogonal slice viewing in navigation view
//
//*****************  Version 44  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  cleanup also
//
//*****************  Version 43  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved tri strips to the segmnet data structure so that the correlate
//with the skeleton indexes for picking
//
//*****************  Version 42  *****************
//User: Frank           Date:  8/17/00  Time: 11:54a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added navigation test mode
//
//*****************  Version 41  *****************
//User: Kevin           Date:  8/16/00  Time:  1:55p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added read of threshold value from .thr file and adjust to 8 bit
//value
//
//*****************  Version 40  *****************
//User: Jeff            Date:  8/15/00  Time:  7:25p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved OverviewTrackBall (class and instance) to ReaderGlobal so that
//overview position/orientation can persist throughout application
//
//*****************  Version 39  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added RGB marked volume writing for Revli viewing
//
//*****************  Version 38  *****************
//User: Kevin           Date:  8/13/00  Time:  4:50p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added memory usage debugs (commented out though)
//
//*****************  Version 37  *****************
//User: Kevin           Date:  8/13/00  Time:  4:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Visible Rendering Marking
//
//*****************  Version 36  *****************
//User: Frank           Date:  8/11/00  Time:  4:26p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a plane projection function
//
//*****************  Version 35  *****************
//User: Kevin           Date:  8/09/00  Time:  4:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added update of preipheral views only when not flying live
//
//*****************  Version 34  *****************
//User: Kevin           Date:  8/09/00  Time:  3:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  += 10 gone
//
//*****************  Version 33  *****************
//User: Kevin           Date:  8/09/00  Time:  3:16p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  m_colonWallThreshold += 10 ?????????????
//
//*****************  Version 32  *****************
//User: Kevin           Date:  8/08/00  Time:  9:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed RearView rendering hooks
//
//*****************  Version 31  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 30  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 29  *****************
//User: Kevin           Date:  8/04/00  Time:  4:43p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed aborts to warns
//Moved orthogonal slice reading to here
//
//*****************  Version 28  *****************
//User: Kevin           Date:  8/02/00  Time: 10:52a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colors
//
//*****************  Version 27  *****************
//User: Kevin           Date:  8/01/00  Time: 10:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added num segments to inf file
//
//*****************  Version 26  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 25  *****************
//User: Binli           Date:  8/01/00  Time:  2:21p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 24  *****************
//User: Kevin           Date:  8/01/00  Time: 11:16a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Debug for set sec ses images
//
//*****************  Version 23  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 22  *****************
//User: Kevin           Date:  7/31/00  Time:  1:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed axial images to be dfb for debug
//
//*****************  Version 21  *****************
//User: Jeff            Date:  7/28/00  Time:  5:53p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added functions for serializing bookmark list and for adding
//bookmarks thru GUI
//
//*****************  Version 20  *****************
//User: Binli           Date:  7/28/00  Time:  1:04p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 19  *****************
//User: Jeff            Date:  7/27/00  Time:  6:12p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Nightly checkin (added a class variable to note the path of the
//current opened volume)
//
//*****************  Version 18  *****************
//User: Kevin           Date:  7/26/00  Time:  6:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a simple skeleton smooth function
//
//*****************  Version 17  *****************
//User: Kevin           Date:  7/26/00  Time:  6:32p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed the Skeleton scaleing
//
//*****************  Version 16  *****************
//User: Kevin           Date:  7/26/00  Time:  2:01p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Got the new skeleton format to draw
//
//*****************  Version 15  *****************
//User: Frank           Date:  7/26/00  Time:  9:52a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added debug output for examining the skeleton
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/26/00  Time:  9:40a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wored OldVolume variables in
//anticipation of new Volume class
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/25/00  Time: 10:40a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Scaled the skeleton to fit the volume
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 11  *****************
//User: Kevin           Date:  7/24/00  Time:  8:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added translucent rendering as a "magic window" with context in
//surface rendering
//
//*****************  Version 10  *****************
//User: Kevin           Date:  7/20/00  Time: 10:17p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Moving Lights and ifxed some distance measuring stuff
//
//*****************  Version 9  *****************
//User: Frank           Date:  7/20/00  Time: 12:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added utility function
//
//*****************  Version 8  *****************
//User: Kevin           Date:  7/19/00  Time:  3:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added global Field-Of-View for rear and forward view
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 6  *****************
//User: Jeff            Date:  7/18/00  Time:  1:35p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/18/00  Time:  1:01p
//Checked in $/v1k/src/ReaderLib
//Comment:
//
//
//*****************  Version 4  *****************
//User: Jeff            Date:  7/18/00  Time:  1:00p
//Branched at version 4
//Comment:
//
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/14/00  Time:  2:19p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Changed RGBA of floats to RGB of char and moved
//so it doesn't always grab large memory untill needed
//
//*****************  Version 2  *****************
//User: Kevin           Date:  7/13/00  Time:  9:36a
//Checked in $/NewViatron1000/ReaderLib
//Comment:
//  new boolean toggle fields for rendering debug
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Global.cpp
//Comment:
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/ReaderGlobal.C,v 1.21.2.2 2010/12/20 18:53:26 dongqing Exp $
// $Id: ReaderGlobal.C,v 1.21.2.2 2010/12/20 18:53:26 dongqing Exp $
