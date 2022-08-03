// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmStore.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "DcmStore.h"
#include "DcmClient.h"
#include "DcmEnums.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"
#include "DcmImageFactory.h"
#include "DcmHost.h"
#include "DcmUtility.h"

// usings
using namespace dcm;


/**
 * constructor
 *
 * @param sAETitle      AE Title of the store client
 */
DcmStore::DcmStore(const std::string & sAETitle) :
DcmClient(sAETitle),
DcmThread("Starting C-STORE command")
{
 
  // Non Blocking allows for a network disconnect to return
  m_blockingMode = DCM_NONBLOCKING;

  m_iRemaining = 0;
  m_iCompleted = 0;

  m_callbackParameters.CommandType  = C_STORE;


  m_abstractSyntaxes.push_back( UID_ComputedRadiographyImageStorage  );
  m_abstractSyntaxes.push_back( UID_DigitalXRayImageStorageForPresentation   );
  m_abstractSyntaxes.push_back( UID_DigitalXRayImageStorageForProcessing  );
  m_abstractSyntaxes.push_back( UID_DigitalMammographyXRayImageStorageForPresentation  );
  m_abstractSyntaxes.push_back( UID_DigitalMammographyXRayImageStorageForProcessing  );
  m_abstractSyntaxes.push_back( UID_DigitalIntraOralXRayImageStorageForPresentation  );
  m_abstractSyntaxes.push_back( UID_DigitalIntraOralXRayImageStorageForProcessing  );
  m_abstractSyntaxes.push_back( UID_CTImageStorage  );
  m_abstractSyntaxes.push_back( UID_EnhancedCTImageStorage  );
  m_abstractSyntaxes.push_back( UID_UltrasoundMultiframeImageStorage  );
  m_abstractSyntaxes.push_back( UID_MRImageStorage   );
  m_abstractSyntaxes.push_back( UID_EnhancedMRImageStorage  );
  m_abstractSyntaxes.push_back( UID_MRSpectroscopyStorage  );
  m_abstractSyntaxes.push_back( UID_EnhancedMRColorImageStorage  );
  m_abstractSyntaxes.push_back( UID_UltrasoundImageStorage  );
  m_abstractSyntaxes.push_back( UID_EnhancedUSVolumeStorage  );
  m_abstractSyntaxes.push_back( UID_SecondaryCaptureImageStorage  );
  m_abstractSyntaxes.push_back( UID_MultiframeSingleBitSecondaryCaptureImageStorage  );
  m_abstractSyntaxes.push_back( UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage  );
  m_abstractSyntaxes.push_back( UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage  );
  m_abstractSyntaxes.push_back( UID_MultiframeTrueColorSecondaryCaptureImageStorage  );
  m_abstractSyntaxes.push_back( UID_TwelveLeadECGWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_GeneralECGWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_AmbulatoryECGWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_HemodynamicWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_CardiacElectrophysiologyWaveformStorage   );
  m_abstractSyntaxes.push_back( UID_BasicVoiceAudioWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_GeneralAudioWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_ArterialPulseWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_RespiratoryWaveformStorage  );
  m_abstractSyntaxes.push_back( UID_GrayscaleSoftcopyPresentationStateStorage   );
  m_abstractSyntaxes.push_back( UID_ColorSoftcopyPresentationStateStorage   );
  m_abstractSyntaxes.push_back( UID_PseudoColorSoftcopyPresentationStateStorage   );
  m_abstractSyntaxes.push_back( UID_BlendingSoftcopyPresentationStateStorage  );
  m_abstractSyntaxes.push_back( UID_XAXRFGrayscaleSoftcopyPresentationStateStorage  );
  m_abstractSyntaxes.push_back( UID_XRayAngiographicImageStorage  );
  m_abstractSyntaxes.push_back( UID_EnhancedXAImageStorage  );
  m_abstractSyntaxes.push_back( UID_XRayRadiofluoroscopicImageStorage   );
  m_abstractSyntaxes.push_back( UID_EnhancedXRFImageStorage  );
  m_abstractSyntaxes.push_back( UID_XRay3DAngiographicImageStorage  );
  m_abstractSyntaxes.push_back( UID_XRay3DCraniofacialImageStorage  );
  m_abstractSyntaxes.push_back( UID_BreastTomosynthesisImageStorage  );
  m_abstractSyntaxes.push_back( UID_NuclearMedicineImageStorage  );
  m_abstractSyntaxes.push_back( UID_RawDataStorage  );
  m_abstractSyntaxes.push_back( UID_SpatialRegistrationStorage  );
  m_abstractSyntaxes.push_back( UID_SpatialFiducialsStorage  );
  m_abstractSyntaxes.push_back( UID_DeformableSpatialRegistrationStorage   );
  m_abstractSyntaxes.push_back( UID_SegmentationStorage   );
  m_abstractSyntaxes.push_back( UID_SurfaceSegmentationStorage   );
  m_abstractSyntaxes.push_back( UID_RealWorldValueMappingStorage   );
  m_abstractSyntaxes.push_back( UID_RETIRED_VLImageStorage  );
  m_abstractSyntaxes.push_back( UID_VLEndoscopicImageStorage  );
  m_abstractSyntaxes.push_back( UID_VideoEndoscopicImageStorage   );
  m_abstractSyntaxes.push_back( UID_VLMicroscopicImageStorage  );
  m_abstractSyntaxes.push_back( UID_VideoMicroscopicImageStorage   );
  m_abstractSyntaxes.push_back( UID_VLSlideCoordinatesMicroscopicImageStorage   );
  m_abstractSyntaxes.push_back( UID_VLPhotographicImageStorage  );
  m_abstractSyntaxes.push_back( UID_VideoPhotographicImageStorage  );
  m_abstractSyntaxes.push_back( UID_OphthalmicPhotography8BitImageStorage   );
  m_abstractSyntaxes.push_back( UID_OphthalmicPhotography16BitImageStorage   );
  m_abstractSyntaxes.push_back( UID_StereometricRelationshipStorage  );
  m_abstractSyntaxes.push_back( UID_OphthalmicTomographyImageStorage   );
  m_abstractSyntaxes.push_back( UID_VLWholeSlideMicroscopyImageStorage  );
  m_abstractSyntaxes.push_back( UID_LensometryMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_AutorefractionMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_KeratometryMeasurementsStorage   );
  m_abstractSyntaxes.push_back( UID_SubjectiveRefractionMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_VisualAcuityMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_SpectaclePrescriptionReportStorage  );
  m_abstractSyntaxes.push_back( UID_OphthalmicAxialMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_IntraocularLensCalculationsStorage  );
  m_abstractSyntaxes.push_back( UID_MacularGridThicknessAndVolumeReportStorage  );
  m_abstractSyntaxes.push_back( UID_OphthalmicVisualFieldStaticPerimetryMeasurementsStorage  );
  m_abstractSyntaxes.push_back( UID_BasicTextSRStorage  );
  m_abstractSyntaxes.push_back( UID_EnhancedSRStorage  );
  m_abstractSyntaxes.push_back( UID_ComprehensiveSRStorage  );
  m_abstractSyntaxes.push_back( UID_ProcedureLogStorage  );
  m_abstractSyntaxes.push_back( UID_MammographyCADSRStorage  );
  m_abstractSyntaxes.push_back( UID_KeyObjectSelectionDocumentStorage   );
  m_abstractSyntaxes.push_back( UID_ChestCADSRStorage  );
  m_abstractSyntaxes.push_back( UID_XRayRadiationDoseSRStorage  );
  m_abstractSyntaxes.push_back( UID_ColonCADSRStorage   );
  m_abstractSyntaxes.push_back( UID_ImplantationPlanSRDocumentStorage   );
  m_abstractSyntaxes.push_back( UID_EncapsulatedPDFStorage   );
  m_abstractSyntaxes.push_back( UID_EncapsulatedCDAStorage   );
  m_abstractSyntaxes.push_back( UID_PositronEmissionTomographyImageStorage   );
  m_abstractSyntaxes.push_back( UID_EnhancedPETImageStorage  );
  m_abstractSyntaxes.push_back( UID_BasicStructuredDisplayStorage   );
  m_abstractSyntaxes.push_back( UID_RTImageStorage  );
  m_abstractSyntaxes.push_back( UID_RTDoseStorage  );
  m_abstractSyntaxes.push_back( UID_RTStructureSetStorage  );
  m_abstractSyntaxes.push_back( UID_RTBeamsTreatmentRecordStorage  );
  m_abstractSyntaxes.push_back( UID_RTPlanStorage   );
  m_abstractSyntaxes.push_back( UID_RTBrachyTreatmentRecordStorage   );
  m_abstractSyntaxes.push_back( UID_RTTreatmentSummaryRecordStorage   );
  m_abstractSyntaxes.push_back( UID_RTIonPlanStorage  );
  m_abstractSyntaxes.push_back( UID_RTIonBeamsTreatmentRecordStorage  );
  m_abstractSyntaxes.push_back( UID_GenericImplantTemplateStorage  );
  m_abstractSyntaxes.push_back( UID_ImplantAssemblyTemplateStorage  );
  m_abstractSyntaxes.push_back( UID_ImplantTemplateGroupStorage  );
  m_abstractSyntaxes.push_back( UID_JPEG2000LosslessOnlyTransferSyntax  );
  m_abstractSyntaxes.push_back( UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax  );
  m_abstractSyntaxes.push_back( UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax  );
  m_abstractSyntaxes.push_back( UID_JPEG2000TransferSyntax  );





  
} // DcmStore


/**
 * Destructor
 */
DcmStore::~DcmStore()
{
  m_images.clear();
  m_files.clear();
} // ~DcmStore()



#pragma region create methods


/**
 * Creates the find client object
 *
 * @param sAETitleOrXml     AE Title of the client or an xml initializer
 *
 * @return DcmStore object
 */
std::shared_ptr<DcmStore> DcmStore::Create(const std::string & sAETitleOrXml)
{
  
  try
  {

    std::string sAETitle( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? "" : sAETitleOrXml ) ) );
    std::string sXml( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? sAETitleOrXml : "" ) ) );
         
    /// Allocate memory
    std::shared_ptr<DcmStore> pDcmStore(new DcmStore(sAETitle));

    /// Make sure no memory error occurred
    if ( pDcmStore== __nullptr )
      throw DcmOutOfMemoryException(LogRec("Could not allocate DcmStore", "DcmStore", "Create"));

 
    // ======================================================================================================
    // Initializes the DcmClient inherited class. Cannot override a static method so I do it this way 
    // so that the code is only written once.
    // ====================================================================================================
    if ( ! sXml.empty() ) 
      pDcmStore->DcmClient::Initialize(sXml);

    // Returns the DcmFinc object
    return pDcmStore;

  }
  catch ( DcmException & ex )
  {

    DcmLog::Error(util::Strings::Format("Failed to create find : %s",ex.GetTextMessage().c_str()), "DcmClient", "Create");

    /// This is most likely a configuration issue
    throw;

  }
  catch ( ... )
  {

    DcmLog::Error("Failed to create find, check configuration", "DcmClient", "Create");

     /// This is most likely a configuration issue
    throw DcmConfigurationException(LogRec("Failed to create find, check configuration", "DcmStore", "Create")); 

  } // END ... catch ( ... )

} // Create(const std::string & sXml)



/**
 * Creates the DcmStore object using the xml std::string
 *
 * @param sAETitle      AE Title
 * @param pHost         Server host
 *
 * @return DcmMove object
 */
std::shared_ptr<DcmStore> DcmStore::Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer )
{
 
  // Validates the host 
  pServer->Validate(dcm::DcmServiceTypes::ST_STORE);

  /// Allocate memory
  std::shared_ptr<DcmStore> pDcmStore(Create(sAETitle));


  std::shared_ptr<dcm::DcmHostAttribute> pAttribute;

  // Sets the host
  pDcmStore->SetServer( pServer );

  // Returns the find object
  return pDcmStore; 
  
} // Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pHost )


#pragma endregion


/**
 * Add specified file
 *
 *  @param sFile
 */
void DcmStore::AddFile(const std::string & sFile)
{
  if ( ! IsRunning() )
  {
    if ( _access(sFile.c_str(), 4) != 0 )
      throw DcmIOException(LogRec(util::Strings::Format("Cannot read file specified %s",sFile.c_str()), "DcmStore", "AddFile"), m_sLogSource);

    m_files.push_back(sFile);
  }
} // AddFile(const std::string & file)


/**
 * Add specified directory
 *
 *  @param sDirectory
 *  @param sPattern
 *  @param recursive
 */
void DcmStore::AddDirectory(const std::string & sDirectory, const std::string & sPattern, bool recursive)
{

  if ( _access(sDirectory.c_str(), 4) != 0 )
    throw DcmIOException(LogRec(util::Strings::Format("Cannot read directory specified %s",sDirectory.c_str()), "DcmStore", "AddDirectory"), m_sLogSource);

  std::string sDicomFiles = sDirectory + "\\" + ( sPattern.empty() ? "*.dcm" : sPattern );
  WIN32_FIND_DATA fileData;

  std::vector<std::string> directories;

  HANDLE handle = ::FindFirstFile(sDicomFiles.c_str(), &fileData);

  
  if ( handle != INVALID_HANDLE_VALUE )
  {

    do 
    {
      std::string sFilePath(util::Strings::Format("%s\\%s",sDirectory.c_str(), fileData.cFileName));

      if ( fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        AddFile(sFilePath);
      else
        directories.push_back(std::string(sFilePath));
     
    } while ( ::FindNextFile(handle, &fileData ) );

    FindClose(handle);

    if ( recursive )
    {

      for ( std::vector<std::string>::iterator it = directories.begin(); it != directories.end(); it++ )
      {
          AddDirectory((*it),sPattern,recursive); 
      }
    } // if   
  } // if
 
} // AddFile(const std::string & file)


/**
 * Clears all files from the list
 */
void DcmStore::ClearFiles()
{
  if ( ! IsRunning() )
    m_files.clear();

} // ClearFiles()


/**
 * Adds image to list
 *
 * @param pDcmImage     Image to be stored
 */
void DcmStore::AddImage(std::shared_ptr<DcmImage> pDcmImage)
{
  if ( ! IsRunning() )
    m_images.push_back(pDcmImage);
} // AddImage(DcmImage * image)

/**
 * Clears all images from list
 */
void DcmStore::ClearImages()
{
  if ( ! IsRunning() )
    m_images.clear();
} // ClearImages()



/**
 * Sends a single image to the specified sever. will connect if not connected. Each image sent successfully will be deleted.
 * from the list
 */
void DcmStore::Send()
{

  // ===============================================================
  // If there are images to store then we need to connect and store
  // ===============================================================
  if (  IsThereImagesToStore()  )
  {

    // Connect to the SCP  ( Only connects it not already connected )
    Connect();

    // Log informative message that eth store is being performed
    DcmLog::Information(m_sLogSource, util::Strings::Format("Sending a single image to %s", GetServer()->GetServer().c_str()), "DcmStore", "Run");

    std::shared_ptr<DcmImage> pDcmImage(DcmImageFactory::Create());

    // ========================================
    // Only loop if the file list is not empty
    // ========================================
    if ( ! m_files.empty() )
    {

      /// Get the last element
      std::string file = m_files.back();

      /// Remove the last element
      m_files.pop_back();

      /// Log debug message about file open
      DcmLog::Debug(m_sLogSource, util::Strings::Format("Attempting to read %s", file.c_str()), "DcmStore", "Run");

      /// Only store image if the load was successful
      if ( pDcmImage->Load(file, false) )
        DcmClient::Store(pDcmImage);
 
    } // END ... If the file list is not empty

    // =========================================
    // Only loop if the image list is not empty
    // ==========================================
    if ( ! m_images.empty() )
    {

      /// Get the last element
      std::shared_ptr<DcmImage> pDcmImage = m_images.back();      
      
      // Remove the last element
      m_images.pop_back();
     
      /// Store image
      DcmClient::Store(pDcmImage);

    } // END ... If the image list is not empty


    // =================================================================
    // If there are no more images to send then release and disconnect
    // ================================================================
    if ( ! IsThereImagesToStore() )
    {
      // Releases the association 
      ReleaseAssociation();

      // Disconnect from the server
      Disconnect();

    } // END ... If there are no more images

  } // if

} // Send()


/**
 * Stores the specified files and images to the server specified
 */
void DcmStore::RunSynchronously()
{
  m_bAbort = false;

  try
  {

    // ==================================================================
    // Set up parameters that are returned during the callback phase
    // =================================================================
    SetConnectionInformation(m_callbackParameters.Connection);
    

    // ===============================================================
    // If there are images to store then we need to connect and store
    // ===============================================================
    if (  IsThereImagesToStore()  )
    {
 
      // Connect to the SCP (We try this first just in case we cannot connect there is no point in doing anything else )
      Connect();

      // Create a DICOM image object for loading files
      std::shared_ptr<DcmImage> pDcmImage(DcmImageFactory::Create());

      /// Get the starting time
      DWORD start = GetTickCount();  

      // Get the image count
      const int iImageCount( GetImageCount() );

      // Set the callback counters
      m_callbackParameters.Command.Store.Completed = 0;
      m_callbackParameters.Command.Store.Remaining = iImageCount;
      
 
      // Log informative message that eth store is being performed
      DcmLog::Information(m_sLogSource, util::Strings::Format("Starting C-STORE operation (Count: %d)", iImageCount), "DcmStore", "Run");

      /// Indicate that we have started
      Callback(E_STARTED);

      // ========================================
      // Only loop if the file list is not empty
      // ========================================
      if ( ! m_files.empty() )
      {

        //===========================
        // Loop through all the files
        //==========================
        std::for_each( m_files.begin(), m_files.end(), [ this, &pDcmImage ] ( std::string file ) 
        {
   
          // Check the abort flag each iteration
          if (m_bAbort)
            throw DcmAbortException(LogRec("Aborting", "DcmStore", "Run"));

          /// Log debug message about file open
          DcmLog::Debug(m_sLogSource, util::Strings::Format("Attempting to read %s", file.c_str()), "DcmStore", "Run");

 
          //========================================
          // Check for an error in reading the file
          //========================================
          if ( ! pDcmImage->Load(file.c_str(), false) ) 
          {

            DcmLog::Warning(m_sLogSource, util::Strings::Format("Bad dicom file %s", file.c_str()), "DcmStore", "Run");

            std::string sMessage(util::Strings::Format("Invalid dicom file %s",file));

            m_callbackParameters.Message = sMessage.c_str();
 
  
          } // END ... If the files read failed
          else
          {

            /// Store image
            this->DcmClient::Store(pDcmImage);
 
            /// Update parameters 
            m_callbackParameters.Message = file.c_str();
 
          }

          // Update counters
          m_callbackParameters.Command.Store.Completed++;
          m_callbackParameters.Command.Store.Remaining--;

          /// Progress event
          Callback(E_PROGRESS);



        } ); // END ... For each file

 
        DcmLog::Timing(util::Strings::Format("Sending %d files took %f seconds", (int)m_files.size(), ( ((float)GetTickCount() - (float)start) / 1000.0F )), "DcmStore", "Run");

      } // END ... If the file list is not empty

      // =========================================
      // Only loop if the image list is not empty
      // ==========================================
      if ( ! m_images.empty() )
      {

        //==============================
        // Loop through all the images
        //=============================
        std::for_each( m_images.begin(), m_images.end(), [this, &pDcmImage ] ( std::shared_ptr<DcmImage> image ) 
        {

         // Check the abort flag each iteration
          if ( m_bAbort )
            throw DcmAbortException(LogRec("Aborting","DcmStore","Run"));

          /// Store image
          this->DcmClient::Store(image);
 
          /// Update the callback parameters
          m_callbackParameters.Message = pDcmImage->GetInstanceUid().c_str();
          m_callbackParameters.Command.Store.Completed++;
          m_callbackParameters.Command.Store.Remaining--;

          /// Progress event
          Callback(E_PROGRESS);

        } );

        DcmLog::Timing(util::Strings::Format("Sending %d images took %f seconds", (int)m_images.size(), ( ((float)GetTickCount() - (float)start) / 1000.0F )), "DcmStore", "Run");

      } // END ... If the image list is not empty

      
      // Log informative message that eth store is being performed
      DcmLog::Information(m_sLogSource, util::Strings::Format("Completed C-STORE operation (Count: %d)", GetImageCount()), "DcmStore", "Run");

      DcmLog::Timing(util::Strings::Format("Sending %d files/images took %f seconds", iImageCount, ( ((float)GetTickCount() - (float)start) / 1000.0F )), "DcmStore", "Run");

      // Releases the association
      ReleaseAssociation();

      // Indicate that we have completed
      m_callbackParameters.Command.Store.Completed += m_callbackParameters.Command.Store.Remaining;
      m_callbackParameters.Command.Store.Remaining = 0;
      Callback(E_COMPLETED);

      // Disconnect from the server
      Disconnect();

    } // END ... If there are images to store
 
  }
  catch ( DcmAbortException &  )
  {
    DcmLog::Error(m_sLogSource, "Store operation was aborted" , "DcmStore", "Run" );

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    Callback(E_ABORTED);

    throw;

  }
  catch ( DcmException & ex )
  {
    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    std::string sMessage(util::Strings::Format("Failed to store files : %s", ex.GetTextMessage().c_str()));

    m_callbackParameters.Message = sMessage.c_str();

    Callback(E_FAILED);

    /// Throw exception but get the last error code from the system
    throw;

  }
  catch ( ...  )
  {

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();


    m_callbackParameters.Message = util::Strings::Format("Failed to store files : unknown error [%d]", ::GetLastError()).c_str();

    Callback(E_FAILED);

    /// Throw exception but get the last error code from the system
    throw DcmException(LogRec(m_callbackParameters.Message, "DcmStore", "Run"), m_sLogSource);

  } // catch

} // Run()


/**
 * Aborts the store
 */
void DcmStore::Abort()
{
  ///
  DcmLog::Debug(m_sLogSource, "Aborting C-STORE", "DcmStore", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  DcmThread::Abort();

} // Abort()