// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "ConversionModel.h"
#include "FileConverter.h"
#include "PipelineEventArgsFactory.h"


using namespace System::Xml;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Diagnostics;
using namespace Viatronix::Logging;
using namespace Viatronix::Conversion; 


/**
 * Constructor
 */
ConversionModel::ConversionModel() :
m_mpTask(nullptr),
m_id(Guid::NewGuid()),
m_mpConverter(gcnew FileConverter(0)),
m_mpArgs(gcnew Dictionary<String ^, String ^>()),
m_msJobId(String::Empty),
m_msApplication(String::Empty),
m_mpCompletedEventHandle(gcnew EventWaitHandle(false, System::Threading::EventResetMode::ManualReset )),
m_eSourceLevel(SourceLevels::Verbose)
{

  m_mpConverter->Started      += gcnew PipelineEventHandler(this, &ConversionModel::OnFileConverterStarted);
  m_mpConverter->Completed    += gcnew PipelineEventHandler(this, &ConversionModel::OnFileConverterCompleted);
  m_mpConverter->Failed       += gcnew PipelineEventHandler(this, &ConversionModel::OnFileConverterFailed);
  m_mpConverter->Updated      += gcnew PipelineEventHandler(this, &ConversionModel::OnFileConverterUpdated);
 
} // ConversionModel()


/**
 * Finalizer attempts to abort the task before deleting
 */
ConversionModel::~ConversionModel()
{
  Abort();

  this->!ConversionModel();

} // ~ConversionModel()


/**
 * Finalizer attempts to abort the task before deleting
 */
ConversionModel::!ConversionModel()
{
  ///
  //if ( m_mpTask != nullptr )
  //  delete m_mpTask;

} // ~ConversionModel()


#pragma region methods


/** 
 * Factory used to create the conversion model based on command line arguments
 *
 * @param mpArgs
 *
 * @return ConversionModel initialized based on parameters
 */
ConversionModel ^ ConversionModel::Create( Dictionary<String ^, String ^> ^ mpArgs )
{

  ConversionModel ^ mpModel = gcnew ConversionModel();

  mpModel->m_mpArgs = mpArgs;

  return mpModel;

}  // Create( )


/**
 * Creates the volume using the parameters
 */
void ConversionModel::CreateVolume()
{

  if ( m_mpTask == nullptr || m_mpTask->Status != System::Threading::Tasks::TaskStatus::Running )
  {
    m_mpTask = nullptr;

    // ===========================================
    // Make sure the volume file was specified
    // ===========================================
    if (!m_mpArgs->ContainsKey("destFilePath"))
    {
      Viatronix::Logging::Log::Error("Volume file was not specified", "ConversionModel", "CreateColume");
      throw gcnew System::IO::InvalidDataException("Volume file is not specified");
    } // END ... If the volume file was not specified


    /// Create the task to perform the conversion
    m_mpTask = gcnew System::Threading::Tasks::Task( gcnew Action(this, &ConversionModel::Run) );

    /// Start the task
    m_mpTask->Start();

  } // if

} // CreateVolume()


/**
 *  implement unit test on specific volume
 */
void ConversionModel::UnitTest()
{
	std::string sVolumeFilePath("");

  if ( m_mpArgs->ContainsKey("destFilePath") )
    sVolumeFilePath = ss( m_mpArgs["destFilePath"] );
  else
	  throw gcnew System::IO::InvalidDataException("Volume file path is not available for unit test.");

	m_mpConverter->UnitTest( sVolumeFilePath );

} // ConversionModel


/**
 * Aborts the current conversion
 */
void ConversionModel::Abort()
{
  m_mpConverter->Abort();
} // Abort()


/**
 * Runs the process
 */
void ConversionModel::Run()
{
  try
  {
 
    Viatronix::Logging::Log::Information("Conversion started", "ConversionModel", "Run");
    
    m_mpArgs->Add("job", m_msJobId);

    m_mpConverter->ClearFiles();

    m_mpConverter->RunSynchronously( m_mpArgs );
     
    Viatronix::Logging::Log::Information("Conversion completed ", "ConversionModel", "Run");

    OnCompleted(PipelineEventArgs::Create("conversion", "", "Volume file created"));

  }
  catch ( cvn::CvnException & ex )
  {

    String ^ mpMessage =  gcnew String(ex.GetTextMessage().c_str());

    Viatronix::Logging::Log::Error("Conversion failed ... " + mpMessage, "ConversionModel", "Run");

    /// We failed
    OnFailed(PipelineEventArgs::Create("conversion", "Creating", mpMessage));

  }
  catch (Exception ^ mpEx)
  {
    Viatronix::Logging::Log::Error("Conversion failed ... " + mpEx->Message, "ConversionModel", "Run");


    /// Failure handling
    OnFailed(PipelineEventArgs::Create("conversion", "Creating", mpEx->Message));
  }
  finally
  { 
    /// Set the handle indicating we are done.
    m_mpCompletedEventHandle->Set();
  }

} // Run(Object ^ mpObj)




#pragma endregion