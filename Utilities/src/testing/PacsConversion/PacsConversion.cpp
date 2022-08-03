// ConversionTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "crypto\AESCrypt.h"
#include "crypto\CryptKey.h"
#include "Wmi.h"
#include "BitConverter.h"



/**
 * Handles the callback from the C-GET
 *
  * @param pCallbackParameters      Callback parameters
  * @param lpCallbackData           Callback data
 */
void Callback(cvn::PCVN_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  switch ( pCallbackParameters->EventType )
  {
  case cvn::E_STARTED:
    {
      printf("Conversion started\n");
    }
    break;
  case cvn::E_COMPLETED:
    {
      printf("Conversion completed\n");
    }
    break;

  case cvn::E_FAILED:
    {
      printf("Conversion failed : %s\n", pCallbackParameters->Message);
    }
    break;

  case cvn::E_ABORTED:
    {
      printf("Conversion aborted\n");
    }
    break;

  case cvn::E_PROGRESS:
    {
      switch ( pCallbackParameters->StageType )
      {
      case cvn::S_LOADING:
          printf("Loading : %s\n", pCallbackParameters->Message);
          break;
      case cvn::S_CREATION:
          printf("Creating : %s\n", pCallbackParameters->Message );
          break;
      case cvn::S_RETRIEVAL:
          printf("Reteiving image %d of %d\n", pCallbackParameters->Completed,  pCallbackParameters->Total );
          break;
      };
    }
    break;

  }
} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

/**
 * Prints out the usage of the command
 */
void Usage()
{
  printf("PacsConversion  [-e] [-study uid] [-series uid] [-instance uid] [-n patient name]\n");
  printf("                [i patient id] <-s server name/ip> <-p port> <-a aetitle> \n");
  printf("                <-sa servers aetitle> <-d destination aetitle> <-dp destination port>\n");
  printf("                [-w volume file]\n\n");

  printf("Starts an internal DICOM server using the destination aetitle and the destination port. Sends a DICOM C-MOVE\n");
  printf("argv[i] to the PACS server specfied by the -s, -p and -sa parameteres. This tell the PACS to send the images \n");
  printf("specified by the uid or patient parameters to the destination. The destination aetitle must be known by the server\n");
  printf("on the PACS.\n\n\\n");
  
  printf("Where:\n\n");

  printf("[-e]                     Forces the appication to wait for a ENTER key to be pressed at the end of the\n");
  printf("                         process. Helps detect errors if they occur. (Default is off)\n\n");

  printf("[-w volume file]         Indicates that the volume is to be written to the file specified\n\n");
 
  printf("[-study uid]             Specifies the study uid to retrieve.\n\n");

  printf("[-series uid]            Specifies the series uid to retrieve.\n\n");

  printf("[-instance uid]          Specifies the instnace uid to retrieve.\n\n");

  printf("[-i patient id]          Specifies the patient id to retrieve.\n\n");

  printf("[-n patient name]        Specifies the patient name to retrieve.\n\n");

  printf("<-s server name/ip>      Specifies the servers name or ip address.\n\n");
  
  printf("<-p port>                Specifies the port the server is listening on.\n\n");
  
  printf("<-sa servers aetitle>    Specifies the server aetitle.\n\n");
  
  printf("<-a aetitle>             Specifies the aetitle of this application.\n\n");
  
  printf("<-d destination aetitle> Specifies the server's aetitle that will be started for retreival and is\n");
  printf("                         destination aetitle send to the PACS to indicate where the images are to be\n");
  printf("                         pushed.\n\n");

  printf("<-dp destination port>   Specifies the server's port that will be started for the retreival.\n\n");

} // Usage()

/**
 * Prints the error message specified to the loga nd screen
 *
 * @param sMessage    Error message
 */
void PrintError(const std::string & sMessage)
{
  LogErr(sMessage, "PacsConversion", "WriteError");
  _tprintf(TEXT("%s\n"), sMessage.c_str());
}   // PrintError(const std::string & sMessage)


/**
 * Main entry point into the console application
 */
int _tmain(int argc, _TCHAR* argv[])
{
  
  crypto::AESCrypt::Encrypt("test");
    crypto::AESCrypt::Encrypt("test1");
  crypto::AESCrypt::Encrypt("test2");
  crypto::AESCrypt::Encrypt("test3");
  crypto::AESCrypt::Encrypt("test4");
  crypto::AESCrypt::Encrypt("test of a an actaully statementxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxmmmmmmmmmmmmmmmmmmmmmmmmmmmwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkllllllllllllllllllllllllllllllllllllsssssssssssssssssiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwy");


  // =========================
  // Check for HELP request
  // =========================
  if (argc == 2 && ( strcmp(argv[1],"/?") == 0 || strcmp(argv[1],"-?") == 0 ) )
  {
    Usage();
    getchar();
    return 0;

  } // END ... If help is requested

  /// Create the logger for the pacs conversion
  vxLogFactory::CreateLogger( "pacsConversion", "",  vxLogRecord::SL_VERBOSE, true);

  /// Create timer
  vxLogTimer timer;

  /// Parameters
  bool enterToContinue = false;
  std::string volumeFile;
  std::string server;
  std::string serverAETitle;
  int port = -1;
  int destinationPort = -1;
  std::string patientId("");
  std::string patientName("");
  std::string aetitle("");
  std::string studyUid("");
  std::string seriesUid("");
  std::string instanceUid("");
  std::string destination("");
  std::string sharedMemoryName("");


  try
  {

 
    for ( int i(1); i < argc ; ++i )
    {

      if ( strcmp(argv[i],"-study") == 0 )
      {
        if (++i < argc)
          studyUid = argv[i];
        else
        {
          PrintError("Missing parameter expected the study uid");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-series") == 0 )
      {
        if (++i < argc)
          seriesUid = argv[i];
        else
        {
          PrintError("Missing parameter expected the series uid");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-instance") == 0  )
      {
        if (++i < argc)
          instanceUid = argv[i];
        else
        {
          PrintError("Missing parameter expected the instance uid");
           return -1;
        }
      }
      else if ( strcmp(argv[i],"-d") == 0 )
      {
        if (++i < argc)
          destination = argv[i];
        else
        {
          PrintError("Missing parameter expected the destintaion aetitle");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-dp") == 0 )
      {
        if (++i < argc)
          destinationPort = atoi(argv[i]);
        else
        {
          PrintError("Missing parameter expected the destintaion port");
          return -1;
        }
      }

      else if ( strcmp(argv[i],"-a") == 0 )
      {
        if (++i < argc)
          aetitle= argv[i];
        else
        {
          PrintError("Missing parameter expected the aetitle");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-i") == 0 )
      {
        if (++i < argc)
          patientId = argv[i];
        else
        {
          PrintError("Missing parameter expected the patient id");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-n") == 0 )
      {
        if (++i < argc)
          patientName = argv[i];
        else
        {
          PrintError("Missing parameter expected the patient name");
          return -1;
        }
      }

      else if ( strcmp(argv[i],"-s") == 0 )
      {
        if (++i < argc)
          server = argv[i];
        else
        {
          PrintError("Missing parameter expected the PACS servers name or ip");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-sa") == 0 )
      {
        if (++i < argc)
          serverAETitle = argv[i];
        else
        {
          PrintError("Missing parameter expected the PACS servers aetitle");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-p") == 0 )
      {
        if (++i < argc)
          port = atoi(argv[i]);
        else
        {
          PrintError("Missing parameter expected the PACS servers port");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-w") == 0 )
      {
        if (++i < argc)
          volumeFile = argv[i];
        else
        {
          PrintError("Missing parameter expected the volume file path");
          return -1;
        }
      }
      else if ( strcmp(argv[i],"-sm") == 0 )
      {
        if (++i < argc)
          sharedMemoryName = argv[i];
        else
        {
          PrintError("Missing parameter expected the shared memory name");
          return -1;
        }
      }
 
      else if ( strcmp(argv[i],"-e") == 0 )
      {
        enterToContinue = true;
      } 
         
    }

    if (aetitle.empty())
    {
      PrintError("Missing aetitle");
      return -1;
    }

    if (server.empty())
    {
      PrintError("Missing PACS server name/ip");
      return -1;
    }

    if (serverAETitle.empty())
    {
      PrintError("Missing PACS server aetitle");
       return -1;
    }

    if ( port == -1)
    {
      PrintError("Missing PACS server port");
      return -1;
    }



    if (destination.empty())
    {
      PrintError("Missing destination ae title");
      return -1;
    }

    if ( destinationPort == -1)
    {
      PrintError("Missing destination port");
      return -1;
    }

       
    if (  patientId.empty() && patientName.empty() && 
          studyUid.empty() && seriesUid.empty() && instanceUid.empty()) 
    {
      PrintError("Missing search parameters");
      return -1;
    }

  

    /// Creates the PACS converter
	  cvn::CvnPacsConverter pacsConverter(0, aetitle);
  
    /// Set up the connection parameters to the PACS server
    dcm::DcmConnectionParameters connection(serverAETitle,server,port);

    //std::string studyUid("1.2.840.113619.2.55.1.1762897554.2239.1082374955.846.400");
    //std::string seriesUid("1.2.840.113619.2.55.1.1762897554.2239.1082374955.851.1");

    // ==========================================================
    // Set the parameters based on the command line arguements
    // ==========================================================
    if ( patientId.empty() && patientName.empty() )
      pacsConverter.SetRetrievalParameters(connection,dcm::DcmGetParameters::Create(destination, destinationPort, studyUid, seriesUid, instanceUid ));
    else
      pacsConverter.SetRetrievalParameters(connection,dcm::DcmGetParameters::Create(destination, destinationPort, patientId, patientName ));

    /// Start the converster
    pacsConverter.Start(&Callback, __nullptr);
  
    /// Wait for completion
    pacsConverter.Wait();

    /// Log the tital round trip time
    timer.Trace("Conversion test");

    if ( ! volumeFile.empty() )
    {
      printf("Writing the volume file %s", volumeFile.c_str());
      pacsConverter.Write(volumeFile);
    }


    
    //
    if ( ! sharedMemoryName.empty() )
    {

      std::shared_ptr<vxBlockVolume<uint2>> pVolume = pacsConverter.GetVolume();

      HANDLE hMapFile;
      LPCTSTR pBuf;

      const int iBufferSize(pVolume->GetMemoryUsage());



      hMapFile = CreateFileMapping(
                   INVALID_HANDLE_VALUE,    // use paging file
                   NULL,                    // default security 
                   PAGE_READWRITE,          // read/write access
                   0,                       // maximum object size (high-order DWORD) 
                   iBufferSize,                // maximum object size (low-order DWORD)  
                   sharedMemoryName.c_str());                 // name of mapping object
 
      if (hMapFile == NULL) 
      {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
        return 1;
      }

      pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                                    FILE_MAP_ALL_ACCESS, // read/write permission
                                    0,                   
                                    0,                   
                                    iBufferSize);           
 
       if (pBuf == NULL) 
       { 
          _tprintf(TEXT("Could not map view of file (%d).\n"), 
                 GetLastError()); 

           CloseHandle(hMapFile);

          return 1;
       }

   
       CopyMemory((PVOID)pBuf, (unsigned char *)pVolume.get(), iBufferSize);


       UnmapViewOfFile(pBuf);

       CloseHandle(hMapFile);



    }


    /// Dump the conversion done with the time
    printf("Conversion Done [%f seconds]\n", timer.GetElapsedTime());

  }
  catch ( cvn::CvnException & ex )
  {
    PrintError("Conversion failed : " + ex.GetTextMessage());
  }
  catch ( dcm::DcmException & ex ) 
  {
    PrintError("Conversion failed : " + ex.GetTextMessage());

  }
  catch ( ... )
  {
    PrintError("Conversion failed : unknown error");
  }

  // ===============================================
  // If we are waiting for the enter key do it here
  // ===============================================
  if ( enterToContinue )
  {
    printf("\nPress enter to continue ");
    getchar();
  }

} // tmain()

