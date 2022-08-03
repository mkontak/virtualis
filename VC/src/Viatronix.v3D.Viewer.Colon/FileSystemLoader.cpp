// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FileSystemLoader.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "FileSystemLoader.h"
#include "File.h"
#include "SystemInfo.h"
#include <functional>
#include "ApplicationSettings.h"
#include <ppl.h>
#include "VCSettings.h"
#include "FilePtrFactory.h"


using namespace ReaderLib;


/**
 * constructor
 *
 * @param         params            study load parameters
 * @param         callback          load callback
 */
FileSystemLoader::FileSystemLoader( const StudyLoadParamsEx & params, StudyLoad::LoadCallback callback ) : Loader( params, callback ),
  //m_bIsMultiThreaded( VCSettings::GetInstance().GetBoolValue( "multithreaded-load" )) // temporarily disabled multi-threaded loading, requires testing with RAID 1 setup
  m_bIsMultiThreaded( false )
{
} // FileSystemLoader


/**
 * destructor
 */
FileSystemLoader::~FileSystemLoader()
{
} // ~FileSystemLoader()


/**
 * load operation
 */
void FileSystemLoader::Load()
{
  // Set IsLoading() to be true for the scope of this function
  rdrGlobal.SetStudyLoading( DatasetOrientations::eNONE );
  SCOPE_EXIT { rdrGlobal.SetStudyLoading( DatasetOrientations::eNONE ); };

  // initialize the studies
  Initialize();
  
  // initialize datasets
  InitializeDataset( rdrGlobal.m_supine, GetParams().GetPrimaryDataset() );
  InitializeDataset( rdrGlobal.m_prone, GetParams().GetSecondaryDataset() );
  
  // create the load function
  FileSystemLoader::LoadFunctionPtr loadFunction;
  CreateLoadFunction( loadFunction );

  // see if both supine and prone studies exist
  uint4 uWhichToLoad = DatasetOrientations::eNONE;
  if(( GetParams().GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID ) && ( io::FilePtrFactory::Exists( rdrGlobal.m_supine.GetPreprocessedPrefix() + vx::CleansedDifferenceVolExt)) )
    uWhichToLoad |= DatasetOrientations::eSUPINE;

  if(( GetParams().GetFlags() & StudyLoadParams::SECONDARY_DATASET_VALID ) && (  io::FilePtrFactory::Exists( rdrGlobal.m_prone.GetPreprocessedPrefix() + vx::CleansedDifferenceVolExt) ))
    uWhichToLoad |= DatasetOrientations::ePRONE;

  rdrGlobal.SetStudyLoading( DatasetOrientations::TypeEnum( uWhichToLoad ));
        
  if( rdrGlobal.IsStudyLoading() == DatasetOrientations::eNONE )
  {
    LogErr("No VOL file found","FileSystemLoader","Load");
    throw ex::FileNotFoundException( LogRec( "Study does not exist or could not be accessed.", "FileSystemLoader", "Load" ));
  }

  // set up the global pointers; supine is always first if it exists
  rdrGlobal.m_pCurrDataset = rdrGlobal.IsDatasetLoading( DatasetOrientations::eSUPINE ) ? ( &rdrGlobal.m_supine ) : ( &rdrGlobal.m_prone );
  rdrGlobal.m_pSecondaryDataset = rdrGlobal.IsDatasetLoading( DatasetOrientations::eSUPINE ) ? ( &rdrGlobal.m_prone )  : ( &rdrGlobal.m_supine );

  // see if secondary data files exist
  rdrGlobal.m_bDualDataDetected = rdrGlobal.IsDatasetLoading( DatasetOrientations::eBOTH );

  std::vector< Dataset * > datasets;
  if( rdrGlobal.IsDatasetLoading( rdrGlobal.m_pCurrDataset->GetOrientation() ))
    datasets.push_back( rdrGlobal.m_pCurrDataset );
  if( rdrGlobal.IsDatasetLoading( rdrGlobal.m_pSecondaryDataset->GetOrientation() ))
    datasets.push_back( rdrGlobal.m_pSecondaryDataset );

  if( datasets.empty() )
    throw ex::LoadFailedException( LogRec( "No datasets specified", "FileSystemLoader", "Load" ));
      
  Timer loadPatientTimer; 
  loadPatientTimer.Reset();

  try
  {
    std::string sName;
    if( m_bIsMultiThreaded )
    {
      LoadParallel( loadFunction, datasets );
      sName = "Parallel";
    }
    else
    {
      LoadSequential( loadFunction, datasets );
      sName = "Sequential";
    }

    std::string sLoadPatientTime( loadPatientTimer.AsString() );
    std::strstream loadString;
    loadString << "Loading :>)" << ( rdrGlobal.IsDatasetLoading( DatasetOrientations::eBOTH ) ? "Both" : "One") <<
      " datasets from " << rdrGlobal.m_supine.GetVolumePrefix().c_str() << " took " << sLoadPatientTime << std::ends;
    LogFyi( loadString.str(), "FileSystemLoader", "Load" );
    LogFyi( sName + " Load operation took " + sLoadPatientTime, "FileSystemLoader", "Load" );

    rdrGlobal.m_sVolumeTitlePath = rdrGlobal.m_supine.GetVolumePrefix();
    rdrGlobal.m_bDualDataLoaded = rdrGlobal.IsDatasetLoading( DatasetOrientations::eBOTH );

    GetCallback().NextStep( StudyLoad::eLOAD_COMPLETE, 0, DatasetOrientations::eNONE );
  } 
  catch( ex::AbortLoadException & )
  {
    rdrGlobal.m_bAbortLoad = false;
    rdrGlobal.Reset();
    rdrGlobal.m_bLoadAborted = true;
    throw;
  } 
  catch( ex::VException & )
  {
    throw;
  }
  catch (...)
  {
    throw ex::LoadFailedException( LogRec( "Failed to load the study", "FileSystemLoader", "Load" ));
  }
} // Load()


/** 
 * abort operation
 */
void FileSystemLoader::Abort()
{
  rdrGlobal.AbortLoad();
} // Abort()


/**
 * sequential load operation
 *
 * @param       loadFunction                    dataset load function
 * @param       datasets                        datasets
 */
void FileSystemLoader::LoadSequential( LoadFunctionPtr & loadFunction, DatasetVector & datasets )
{
  bool bSetZoomDefault(false);

  // load header
  Dataset * pOther( rdrGlobal.m_pSecondaryDataset );
  std::for_each( datasets.begin(), datasets.end(), [&]( Dataset * pDataset )
  {
    loadFunction( *pDataset, Dataset::eLoadHeader, GetCallback(), rdrGlobal.IsDatasetLoading( DatasetOrientations::eBOTH ), pOther->m_uLoadPercent );
    pOther = pDataset;
  });

  rdrGlobal.SetupZoomFactorInOverview( datasets[0]->m_vDim, datasets[0]->m_vUnits );

  if(( rdrGlobal.m_supine.m_vDim.m_z + rdrGlobal.m_prone.m_vDim.m_z ) > 950 && SystemInfo::GetTotalPhysicalMemory() <= 1024*1024*1024 )
  {
    GetCallback().NextStep( StudyLoad::eLARGESIZE_QUERY, 2, DatasetOrientations::eNONE );
    if( rdrGlobal.m_bAbortLoad )
      throw ex::AbortLoadException( LogRec("User choose to abort large Study", "FileSystemLoader", "Load" ));
  }

  // load additional steps
  Timer loadTimer;
  uint4* pLoadStep = SystemInfo::GetTotalPhysicalMemory() > 1024*1024*1024? Dataset::vLoadStage2GB : Dataset::vLoadStage1GB;
  for (uint4 uStep = 1; uStep < Dataset::GetLoadStepsCount(); uStep++)
  {
    pOther = rdrGlobal.m_pSecondaryDataset;
    std::for_each( datasets.begin(), datasets.end(), [&]( Dataset * pDataset )
    {
      loadFunction( *pDataset, pLoadStep[uStep], GetCallback(), rdrGlobal.IsDatasetLoading( DatasetOrientations::eBOTH ), pOther->m_uLoadPercent );
      pOther = pDataset;
    });

    LogFyi( "Load step " + ToAscii(pLoadStep[uStep]) + ": " + loadTimer.AsString(), "FileSystemLoader", "Load" );
    loadTimer.Reset();
  } // endfor all load steps
} // LoadSequential( loadFunction, datasets )


/**
 * multi-threaded load operation
 *
 * @param       loadFunction                    dataset load function
 * @param       datasets                        datasets
 */
void FileSystemLoader::LoadParallel( LoadFunctionPtr & loadFunction, DatasetVector & datasets )
{
  StudyLoad::LoadCallback emptyCallback( EmptyLoadNotify );
  Concurrency::task_group group;

  try
  {
    // load header
    std::for_each( datasets.begin() + 1, datasets.end(), [&]( Dataset * pDataset )
    {
      group.run( [&, pDataset] { loadFunction( *pDataset, Dataset::eLoadHeader, emptyCallback, false, 0 ); });
    });

    loadFunction( *datasets[0], Dataset::eLoadHeader, GetCallback(), false, 0 );
    group.wait();
    rdrGlobal.SetupZoomFactorInOverview( datasets[0]->m_vDim, datasets[0]->m_vUnits );

    // check for cancellation
    if( rdrGlobal.m_bLoadAborted )
      throw ex::AbortLoadException( LogRec( "Load aborted", "FileSystemLoader", "LoadParallel" ));

    if(( rdrGlobal.m_supine.m_vDim.m_z + rdrGlobal.m_prone.m_vDim.m_z ) > 950 && SystemInfo::GetTotalPhysicalMemory() <= 1024*1024*1024 )
    {
      GetCallback().NextStep( StudyLoad::eLARGESIZE_QUERY, 2, DatasetOrientations::eNONE );
      if( rdrGlobal.m_bAbortLoad )
        throw ex::AbortLoadException( LogRec("User choose to abort large Study", "FileSystemLoader", "LoadParallel" ));
    }

    // load additional steps
    Timer loadTimer;
    uint4* pLoadStep = SystemInfo::GetTotalPhysicalMemory() > 1024*1024*1024? Dataset::vLoadStage2GB : Dataset::vLoadStage1GB;
    for (uint4 uStep = 1; uStep < Dataset::GetLoadStepsCount(); uStep++)
    {
      std::for_each( datasets.begin() + 1, datasets.end(), [&]( Dataset * pDataset )
      {
        group.run( [&, pDataset] { loadFunction( *pDataset, pLoadStep[uStep], emptyCallback, false, 0 ); });
      });

      loadFunction( *datasets[0], pLoadStep[uStep], GetCallback(), false, 0 );
      group.wait();

      LogFyi( "Load step " + ToAscii(pLoadStep[uStep]) + ": " + loadTimer.AsString(), "FileSystemLoader", "LoadParallel" );
      loadTimer.Reset();
    } // endfor all load steps
  }
  catch( ex::VException & )
  {
    try
    {
      rdrGlobal.AbortLoad();

      // wait for other thread to stop
      group.wait();
    }
    catch( ... )
    {
      // already have an exception which is terminating the load
    }
  }
} // LoadParallel( loadFunction, datasets )


/** 
 * Initialization of load operation
 */
void FileSystemLoader::Initialize()
{
  rdrGlobal.Reset();

  ApplicationSettings::UserSettings().TryGet<float>( APP_SETTING_FOV, VolumeRender::m_fFovDegrees );
} // Initialize()


/**
 * initializes the dataset
 *
 * @param         dataset             Dataset instance
 * @param         params              load parameters
 */
void FileSystemLoader::InitializeDataset( Dataset & dataset, const DatasetLoadParams & params )
{
  DatasetOrientations::TypeEnum eOrientation = ( params.GetOrientation() == DatasetOrientations::ePRIMARY ) ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE;

  dataset.SetUID( params.GetUID() );
  dataset.SetOrientation( eOrientation ); // this may change after it reads the data from disk
  dataset.m_trackballStandard.Init( dataset.MatchOrientation( eOrientation ));
  dataset.m_trackballVerify.Init( dataset.MatchOrientation( eOrientation ));
  dataset.SetVolumePrefix( DatasetLoadParams::FormatPrefixPath( params.GetVolumeDirectory(), params.GetPrefix() ));
  dataset.SetPreprocessedPrefix( DatasetLoadParams::FormatPrefixPath( params.GetPreprocessedDirectory(), DatasetLoadParams::GeneratePrefix( params.GetPreprocessedDirectory() )));

  std::string sCadFile;
  DatasetLoadParams::FindFilename(params.GetCadDirectory(), "*.cad.xml", sCadFile);  
  dataset.SetCadPrefix( DatasetLoadParams::FormatPrefixPath( params.GetCadDirectory(), sCadFile ) );

  dataset.SetSessionPrefix( DatasetLoadParams::FormatPrefixPath( GetParams().GetSessionDirectory(), params.GetPrefix() ));
  dataset.SetSessionUID( GetParams().GetSessionUID() );

  std::string sName = ( params.GetOrientation() == DatasetOrientations::ePRIMARY ) ? "Primary" : "Secondary";
  LogDbg( sName + " Vol: " + dataset.GetVolumePrefix(), "FileSystemLoader", "InitializeDataset" );
  LogDbg( sName + " Pre: " + dataset.GetPreprocessedPrefix(), "FileSystemLoader", "InitializeDataset" );
  LogDbg( sName + " CAD: " + dataset.GetCadPrefix(), "FileSystemLoader", "InitializeDataset" );
  LogDbg( sName + " Ssn: " + dataset.GetSessionPrefix(), "FileSystemLoader", "InitializeDataset" );

  dataset.m_uLoadPercent = 0;
} // InitializeDataset( dataset, params )


/**
 * creates the dataset load operation
 *
 * @param         functionPtr                 dataset load operation
 */
void FileSystemLoader::CreateLoadFunction( LoadFunctionPtr & functionPtr )
{
  DatasetLoadParams & rDSparams = ( GetParams().GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID) ? GetParams().GetPrimaryDataset() : GetParams().GetSecondaryDataset();


  if( io::FilePtrFactory::Exists(rDSparams.GetPreprocessedDirectory() + "\\version.xml") )
  { 
    try
    {


      SeriesVersion seriesVersion = SeriesVersion::FromFile( rDSparams.GetPreprocessedDirectory() + "\\version.xml" );
      
      rdrGlobal.m_supine.SetSeriesVersion( seriesVersion );
      rdrGlobal.m_prone.SetSeriesVersion( seriesVersion );
      LogDbg( "Loading series version: " + ToAscii( atof( rdrGlobal.m_supine.GetSeriesVersion().GetVersion().c_str() ) ), "ReaderGlobal", "LoadStudy2" );

      if( atof( seriesVersion.GetVersion().c_str() ) < 5.1F )
        throw ex::LoadFailedException( LogRec( "Invalid series version", "FileSystemLoader", "Load" ));

      // latest load function
      functionPtr = [&]( Dataset & dataset, const uint4 uStage, StudyLoad::LoadCallback & callback, const bool bLoadBoth, const uint4 uOtherLoadPercent ) 
                        { 
                          bool bSetupZoomFactor( false );
                          dataset.LoadStep3( uStage, callback, bLoadBoth, uOtherLoadPercent, bSetupZoomFactor ); 
                          
                          // check for cancellation
                          if( rdrGlobal.m_bLoadAborted )
                            throw ex::AbortLoadException( LogRec( "Load aborted", "FileSystemLoader", "Load" ));
                        };      
    }
    catch( ex::VException & )
    {
      throw;
    }
    catch( ... )
    {
      throw ex::FileNotFoundException( LogRec( "Failed to read the series version file", "FileSystemLoader", "Load" ));
    }
  }
  else
  {
    // legacy load fuction
    functionPtr = [&]( Dataset & dataset, const uint4 uStage, StudyLoad::LoadCallback & callback, const bool bLoadBoth, const uint4 uOtherLoadPercent ) 
                  { 
                    bool bSetupZoomFactor( false );
                    dataset.LoadStep2( uStage, callback, bLoadBoth, uOtherLoadPercent, bSetupZoomFactor ); 
                    
                    // check for cancellation
                    if( rdrGlobal.m_bLoadAborted )
                      throw ex::AbortLoadException( LogRec( "Load aborted", "FileSystemLoader", "Load" ));
                  };
  }
} // CreateLoadFunction( functionPtr )


/**
 * empty callback
 *
 *  @param pData
 *  @param ePrevStep
 *  @param eNextStep
 *  @param fPercentComplete
 *  @param eNewOrientation
 *  @return 1
 */
int4 FileSystemLoader::EmptyLoadNotify( void* pData, const ReaderLib::StudyLoad::StepsEnum ePrevStep,
                                        const ReaderLib::StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                                        const ReaderLib::DatasetOrientations::TypeEnum eNewOrientation )
{
  return 1;
} //EmptyLoadNotify(