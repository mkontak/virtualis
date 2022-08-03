// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "FileConverter.h"
#include "CvnUnitTest.h"
#include "PipelineEventArgsFactory.h"
#include "SubVolumeInfo.h"
#include "SubVolumeInfoCollection.h"

#pragma region usings

using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;
using namespace Viatronix::Logging;

USING_CONVERSION_NS
 

#pragma endregion

 /**
 * Constructor
 */
FileConverter::FileConverter() :
IConverter(),
m_pFileConverter( scoped_ptr<std::shared_ptr<cvn::CvnFileConverter>>(new std::shared_ptr<cvn::CvnFileConverter>(cvn::CvnFileConverter::Create(0)) ) )
{

  Initialize();

} // FileConverter()


/**
 * Constructor
 *
 *  @param iSiteId
 */
FileConverter::FileConverter( int iSiteId ) :
IConverter(),
m_pFileConverter( scoped_ptr<std::shared_ptr<cvn::CvnFileConverter>>(new std::shared_ptr<cvn::CvnFileConverter>(cvn::CvnFileConverter::Create(iSiteId)) ) )
{
  Initialize();

} // FileConverter()


/**
 * Initializes the FileConverter object
 */
void FileConverter::Initialize()
{
  m_mpCallbackDelegate = gcnew CallbackDelegate(this, &FileConverter::Callback);

  // Allocate an new handle for the callback delegate
  m_gcHandle = GCHandle::Alloc(m_mpCallbackDelegate);

  // Marshall the function pointer for unmanaged usage
  m_callbackPtr = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

} // Initialize


/**
 * Writes the volume file to the file path specified
 *
 * @param sVolumeFile     File path to write to
 */
void FileConverter::Write( String ^ sVolumeFile )
{

  (*m_pFileConverter)->Write( ss(sVolumeFile) );

} // Write(String ^ sVolumeFile)


/** 
 * Runs the conversion process in the current thread
 *
 * @param mpArgs        Arguments/Parameters
 */
void FileConverter::RunSynchronously( Dictionary<String ^, String ^> ^ mpArgs )
{
  // ==============================================
  // Make sure the thread has not already started
  // =============================================
  if (  (*m_pFileConverter)->IsRunning() )
    throw gcnew Exception("Conversion is alread running");

  try
  {
    // Process arguements
    ProcessArguments(mpArgs);

    // Set the default application is supplied on the command line
    (*m_pFileConverter)->SetDefaultApplication( ss(m_msDefaultApplication) );

    /// Make sure files were sepcified
    if ( (*m_pFileConverter)->GetFileCount() == 0 )
      throw gcnew Exception("No files speciifed");

    (*m_pFileConverter)->Create( nullptr, nullptr, ss(m_msVolumeFilePath) );

  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew Exception("Failed conversion : " + gcnew String(ex.GetTextMessage().c_str()) );
  }
 
} // RunSynchronously(array<String ^> ^ args)


/**
  * unit test
	*
	*  @param sVolumeFilePath
	*/
void FileConverter::UnitTest( const std::string & sVolumeFilePath )
{
	(*m_pFileConverter)->UnitTest( sVolumeFilePath );
} // UnitTest


/** 
 * Starts the store
 *
 *  @param mpArgs
 */
System::Threading::EventWaitHandle ^ FileConverter::Start( Dictionary<String ^, String ^> ^ mpArgs )
{
  try
  {
    // ==============================================
    // Make sure the thread has not wlaredy started
    // =============================================
    if ( ! (*m_pFileConverter)->IsRunning() )
    {
      ProcessArguments(mpArgs);

      /// Make sure files were sepcified
      if ( (*m_pFileConverter)->GetFileCount() == 0 )
        throw gcnew Exception("No files speciifed");

      m_mpCallbackDelegate = gcnew CallbackDelegate( this, &FileConverter::Callback );

      cvn::CVN_CALLBACK_FUNCTION callbackFunction = static_cast<cvn::CVN_CALLBACK_FUNCTION>(m_callbackPtr.ToPointer());
 
      std::function< void( cvn::PCVN_CALLBACK_PARAMETERS, LPVOID) > callback =  callbackFunction;
  
      // Starts the volume/file convrter thread
      (*m_pFileConverter)->Start(callback, nullptr, ss(m_msVolumeFilePath));

      // Wait till the thread actually starts running
      while( ! (*m_pFileConverter)->IsRunning() ) Sleep(250);

      // Sets the event wait handle to the thread handle
      m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr((*m_pFileConverter)->GetHandle()), false);

      return m_mpEventWaitHandle;

    } // END ... If the store has not already started
    else
      throw gcnew AlreadyStartedException("Converter already started");

  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew Exception("Failed to start conversion : " + gcnew String(ex.GetTextMessage().c_str()));
  }
  finally
  {
    // Free the global handle
    //gh.Free();
  }

} // Start()


/**
 * Aborts the thread
 */
void FileConverter::Abort()
{
  try
  {
    (*m_pFileConverter)->Abort();
  }
  catch( cvn::CvnException & ex )
  {
    throw gcnew ConversionException( gcnew String(ex.GetTextMessage().c_str()) );
  }
 
} // Stop()


/**
 * Adds the file to the list
 *
 * @param sFile  The parameters.
 */
void FileConverter::AddFile( String ^ sFile )
{
  try
  {
     (*m_pFileConverter)->AddFile(ss(sFile));
  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew Viatronix::Conversion::IOException("Failed to add file to list : " + gcnew String(ex.GetTextMessage().c_str()));
  }

 } // FileConverter::AddFile(String ^ sFile)


/**
 * Adds the file to the list
 *
 * @param fileList  The parameters.
 */
void FileConverter::AddFiles( array<String ^> ^ fileList )
{
  try
  {   
    for each ( String ^ sFile in fileList )
    {
      (*m_pFileConverter)->AddFile(ss(sFile));
    } // for
  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew IOException("Failed to add file to list : " + gcnew String(ex.GetTextMessage().c_str()));
  }

 } // FileConverter::AddFile(array<String ^> ^ fileList)


/**
 * Adds the file to the list
 *
 * @param sDirectory
 * @param sPattern
 * @param bRecursive
 */
void FileConverter::AddDirectory( String ^ sDirectory, String ^ sPattern, bool bRecursive )
{
  try
  {   
    (*m_pFileConverter)->AddDirectory(ss(sDirectory), ss(sPattern), bRecursive);
  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew Viatronix::Conversion::IOException("Failed to add directory to list : " + gcnew String(ex.GetTextMessage().c_str()));
  }

 } // FileConverter::AddFile(array<String ^> ^ fileList)


/**
 * Clears the file list
 */
void FileConverter::ClearFiles()
{
  try
  {   
    (*m_pFileConverter)->ClearFiles();
  }
  catch ( cvn::CvnException & ex )
  {
    throw gcnew Viatronix::Conversion::IOException("Failed to clear files from the list : " + gcnew String(ex.GetTextMessage().c_str()));
  }

 } // ClearFiles()


/**
 * Handles the callback from the server
 *
 *  @param pCallbackParameters
 *  @param lpCallbackData
 */
void FileConverter::Callback(cvn::PCVN_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {
    /// Create the args
    PipelineEventArgs ^ pArgs( PipelineEventArgsFactory::Create(IntPtr(pCallbackParameters)));
    pArgs->JodId = m_msJobId;

    // ================================================================
    // Determine the evenet to raise based on the evenet type retrned.
    // ================================================================
    switch(pCallbackParameters->EventType)
    {

      case cvn::CvnEvents::E_STARTED:
        OnStarted(pArgs);
        break;

      case cvn::CvnEvents::E_ABORTED:
        OnAborted(pArgs);
        break;

      case cvn::CvnEvents::E_COMPLETED:
        OnCompleted(pArgs);
        break;

      case cvn::CvnEvents::E_FAILED:
        OnFailed(pArgs);
        break;

      case cvn::CvnEvents::E_UPDATED:
        OnUpdated(pArgs);
        break;

      default:
        LogWrn("Received an invalid event type on callback, not processing", "FileConverter", "Callback");

    } // END ... Swicth() 

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->LogSource, "Exception detected during the handling of the callback : " + ex->Message, "Server","Callback");
    throw;
  }

} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);


/**
 * Process arguements
 *
 *  @param mpArgs
 */
void FileConverter::ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs)
{
  if ( mpArgs->Count > 0 )
  {
    String ^ msPattern("*.dcm");
    String ^ msSourceFilePath(String::Empty);
    String ^ msLogFilePath(String::Empty);
    Viatronix::Logging::MessageFormat logFormat(MessageFormat::Default);
    Viatronix::Logging::LogLevel logLevel(LogLevel::Information);

    Log::Debug("Procsssing arguement list", "FileConverter", "ProcessArguements");

    for each ( KeyValuePair<String ^, String ^> ^ mpItem in mpArgs )
    {      
      Log::Debug(String::Format("{0} - {1}", mpItem->Key, mpItem->Value), "FileConverter", "ProcessArguements");

      if ( mpItem->Key == "sourceFilePath")
      {
        msSourceFilePath = mpItem->Value;

        if (  ! System::IO::Directory::Exists(msSourceFilePath) )
        { /// if source file path does not exist, look for dicom file path as replacement
					String ^ msDicomFilePath(String::Empty);

					for each( KeyValuePair<String ^, String ^> ^ mpItem in mpArgs )
					{
						if( mpItem->Key == "dicomFilePath" )
						{
							msDicomFilePath = mpItem->Value;
						} // if
					} // for

          if(  ! System::IO::Directory::Exists(msDicomFilePath) )
          {
						throw gcnew System::IO::IOException(String::Format("Specified source file path \"{0}\" does not exists", msSourceFilePath));
					} // if
					else
					{
						msSourceFilePath = msDicomFilePath;
            Log::Warning("Replace source file path with dicom file path", "FileConverter", "ProcessArguements");
					} // else

        } // if

      } // if
      else if ( mpItem->Key == "pattern" )
        msPattern = mpItem->Value;
      else if ( mpItem->Key == "logFilePath")
        msLogFilePath = mpItem->Value;
      else if ( mpItem->Key == "logFormat" )
        logFormat = ( String::Compare(mpItem->Value,"xml",true) == 0 ? MessageFormat::Xml : MessageFormat::Default );
      else if ( mpItem->Key == "logLevel" )
      {
        Int32 level;
        if ( Int32::TryParse(mpItem->Value, level) )
          logLevel = (LogLevel)level;
      }
      else if ( mpItem->Key == "destFilePath" )
      {
        if ( System::IO::Path::GetExtension(mpItem->Value) == ".vol" )
          m_msVolumeFilePath = mpItem->Value;
        else
          m_msVolumeFilePath = mpItem->Value + ".vol";
      }
      else if  ( mpItem->Key == "site" )
      {
        Int32 iSiteId;
        Int32::TryParse(mpArgs["site"],iSiteId);
        this->SiteId = iSiteId;
      }

    } // for


    if ( ! String::IsNullOrEmpty(msLogFilePath) )
    {
      m_msLogSource =  Guid::NewGuid().ToString();
      Log::Create(m_msLogSource, msLogFilePath, logLevel, logFormat, false);
      (*m_pFileConverter)->SetLogSource(ss(m_msLogSource));
      //cvn::CvnLog::SetLogSource("conversion");
    } // if

    if ( ! String::IsNullOrEmpty(msSourceFilePath) )
      AddDirectory(msSourceFilePath, msPattern, false);
    else
      throw gcnew System::Exception("No source file path specified");

    if ( String::IsNullOrEmpty(m_msVolumeFilePath) )
      throw gcnew System::Exception("No destination file path specified");

  } // if

  IConverter::ProcessArguments(mpArgs);

} // ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs)


/**
 * Gets the results of the volume creation whcih should be a list of files
 */
IPipelineResults ^ FileConverter::GetResults()
{
  SubVolumeInfoCollection ^ mpCol = gcnew SubVolumeInfoCollection(m_msJobId, "conversion");

  std::map<uint4,std::shared_ptr<cvn::CvnSubVolumeInfo>> subvolumes = (*m_pFileConverter)->GetCvnSubVolumeInfo();

  std::map<uint4,std::shared_ptr<cvn::CvnSubVolumeInfo>>::iterator it;
  for( it = subvolumes.begin(); it != subvolumes.end(); it++ )
  {
    SubVolumeInfo ^ mpVolume(SubVolumeInfo::Create(IntPtr((void *)(new std::shared_ptr<cvn::CvnSubVolumeInfo>(it->second)))));

    mpCol->Add(mpVolume->Number, mpVolume);

  }

  return mpCol;
} // GetResults

