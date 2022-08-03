// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CommandLineParameters.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "Stdafx.h"
#include "Host.h"
#include "HostTable.h"
#include "Utility.h"
#include "CommandLineParameters.h"

USING_DICOM_NS;

/**
 * Constructor
 */
CommandLineParameters::CommandLineParameters() :
m_mpHost(gcnew Host()),
m_msAEtitle("V3D_SCU"),
m_iNetworkTimeout(30),
m_mpTransferSyntaxes(gcnew Generic::List<String ^>()),
m_mpAbstractSyntaxes(gcnew Generic::List<String ^>()),
m_mpParameters(gcnew Generic::Dictionary<String ^, String ^>()),
m_bVerbose(false),
m_msProgram(String::Empty),
m_msDescription(String::Empty),
m_blockingMode(BlockingModes::NONBLOCKING),
m_bWait(false)
{

} // COmmandLineParameters()


/**
 * Creates the object from the xml supplied
 *
 * @param mpNode      Xml Node
 *
 * @return CommandLineParameters object
 */
CommandLineParameters ^ CommandLineParameters::Create(String ^ msProgram, String ^ msDescription, XmlNode ^ mpNode)
{

  CommandLineParameters ^ mpParameters = gcnew CommandLineParameters();

  if ( mpParameters == nullptr )
    throw gcnew System::OutOfMemoryException("Failed to allocated CommandLineParameters object");

  mpParameters->m_msProgram = msProgram;
  mpParameters->m_msDescription = msDescription;
  mpParameters->Initialize(mpNode);
      
  return mpParameters;
} // Create(XmlNode ^ mpNode) 

/**
 * Initializes the object using the xml
 *
 * @param mpNode      Xml node
 */
void CommandLineParameters::Initialize(XmlNode ^ mpNode)
{

  XmlNode ^ mpServerNode = mpNode->SelectSingleNode("server");
  if ( mpServerNode != nullptr )
    m_mpHost = Host::Create(mpServerNode);

  Utility::TryGetAttribute<String ^>( mpNode, "aetitle",  m_msAEtitle);
  Utility::TryGetAttribute<unsigned int>( mpNode, "networkTimeout",  m_iNetworkTimeout);

  String ^ msBlockingMode;
  Utility::TryGetAttribute<String ^>(mpNode, "blockingMode", msBlockingMode);
  m_blockingMode = ( msBlockingMode == "BLOCKING" ? BlockingModes::BLOCKING : ( msBlockingMode == "NONBLOCKING" ? BlockingModes::NONBLOCKING : m_blockingMode ) );

  m_mpTransferSyntaxes->AddRange( Utility::GetSectionElements(mpNode, "transferSyntaxes") );
  m_mpAbstractSyntaxes->AddRange( Utility::GetSectionElements(mpNode, "abstraxtSyntaxes") );

      
} // Initialize(XmlNode ^ mpNode)

/**
 * Updates the parameter with values in the arg list
 *
 * @param mpArgs      Arguments
 *
 * @return Index after all arguments
 */
int CommandLineParameters::Update(array<String ^> ^ mpArgs)
{
  array<Char> ^ comma = { ',' };

  int index = -1;

  // ==============================
  // If no args then dump the help
  // =============================
  if (mpArgs->Length == 0 || mpArgs[0] == "-?" || mpArgs[0] == "/?" )
  {
    Help();
    return index;
  } // END ... If no arguments

  /// Command
  String ^ msCommand;

  // =================================
  // Loop through all commands
  // =================================
  for (index = 0; index < mpArgs->Length  ; ++index)
  {

    // Argument
    String ^ mpArg = mpArgs[index];

    // =======================
    // If we have a command
    // =======================
    if ( mpArg[0] == '-' )
    {

      if ( ! String::IsNullOrEmpty(msCommand) )
        throw gcnew InvalidDataException(String::Format("Missing command value [{0}]", msCommand));

      m_mpParameters->Add(mpArg, String::Empty);

      // =====================
      // Handle the switches
      // ======================
      switch ( mpArg[1] )
      {
      case 'v':         // Verbose
        m_bVerbose = true;
        break;
      case 'w':         // Wait for input
        m_bWait = true;
        break;
      default:
        msCommand = mpArg;
        break;
      };

      continue;
    } // END 
    else if ( String::IsNullOrEmpty(msCommand) )
      break;




    //========================
    // Switch on the command 
    //=========================
    if ( msCommand->Length == 2 )
    {

      switch (msCommand[1])
      {
        case 'a':   // AE Title
          m_msAEtitle = mpArg;
          break;

        case 'c':   // Server AE Title
          m_mpHost->AETitle = mpArg;
          break;

        case 'n':   // Network Timeout
          m_iNetworkTimeout = Int32::Parse(mpArg);
          break;

        case 'p':   // Port
          m_mpHost->Port = UInt32::Parse(mpArg);
          break;

        case 's':  // Server
          m_mpHost->Server = mpArg;
          break;

        case 'h':  // Host
          m_mpHost = Viatronix::Dicom::HostTable::CurrentHostTable->GetHostByName(mpArg);
          break;

        case 'b':  // Block Mode
          m_blockingMode = ( mpArg == "BLOCKING" ? BlockingModes::BLOCKING : ( mpArg == "NONBLOCKING" ? BlockingModes::NONBLOCKING : m_blockingMode ) );
          break;

        case 't':         // Transfer syntaxes

          {
 
            m_mpTransferSyntaxes->Clear();
         
            array<String ^> ^ mpSyntaxes = mpArg->Split(comma);

            // ================================
            // Loop through all the syntaxes
            // ===============================
            for each (String ^ msSyntax in mpSyntaxes)
            {

              String ^ op   = String::Empty;
              String ^ uid  = msSyntax;
              if (msSyntax[0] == '-')
              {
                op = "-";
                uid = msSyntax->Substring(1);
              }

              // ===================================================
              // If the list item contains a '.' assume its a UID
              // ===================================================
              if ( uid->Contains(".") )
                m_mpTransferSyntaxes->Add(op + uid);
              else if (uid == "implicit" || uid == "little-endian-implict")
                m_mpTransferSyntaxes->Add(op + "1.2.840.10008.1.2");
              else if (uid == "explicit" || uid == "lttle-endian-explicit")
                m_mpTransferSyntaxes->Add(op + "1.2.840.10008.1.2.1");
              else if (uid == "big-endian-implicit")
                m_mpTransferSyntaxes->Add(op + "1.2.840.10008.1.2.2");
              else 
                throw gcnew InvalidDataException("Invalid transfer syntax");

            } // END ... FOr each syntax

            // =============================================
            // Make sure we have value transfer syntaxes
            // =============================================
            if ( m_mpTransferSyntaxes->Count == 0 )
              throw gcnew InvalidDataException("Invalid transfer syntaxes");

          }
          break;

        case 'x':         // Abstract syntaxes

          {
 
            m_mpAbstractSyntaxes->Clear();
            array<String ^> ^ mpSyntaxes = mpArg->Split(comma);

            // ================================
            // Loop through all the syntaxes
            // ===============================
            for each (String ^ msSyntax in mpSyntaxes)
            {

              String ^ op   = String::Empty;
              String ^ uid  = msSyntax;
              if (msSyntax[0] == '-')
              {
                op = "-";
                uid = msSyntax->Substring(1);
              }

              // ===================================================
              // If the list item contains a '.' assume its a UID
              // ===================================================
              if ( uid->Contains(".") )
                m_mpAbstractSyntaxes->Add(op + uid);
              else 
                throw gcnew InvalidDataException("Invalid abstract syntax");

            } // END ... FOr each syntax

            // =============================================
            // Make sure we have value transfer syntaxes
            // =============================================
            if ( m_mpAbstractSyntaxes->Count == 0 )
              throw gcnew InvalidDataException("Invalid abstract syntaxes");

          }
          break;


        
        default:
          break; //throw gcnew InvalidOperationException("Invalid command " + mpArg);

 
      };  // END ... Switch on the command

    } // END .. If the command is of the form -X where X is the identifier
        
    m_mpParameters[msCommand] = mpArg;
    msCommand = String::Empty;

  }  // END ... FOr each argument

    return index;

  } // Update( string [] args )


/**
 * Validates the data
 */
void CommandLineParameters::Validate()
{

  if ( String::IsNullOrEmpty(m_msAEtitle) )
    throw gcnew InvalidDataException("Missing AETitle");

  try
  {
    m_mpHost->Validate();
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew InvalidDataException(gcnew String(ex.GetTextMessage().c_str()));
  }

} // Validate()

/**
 * Dumps the Help to the console
 */
void CommandLineParameters::Help()
{



  Console::WriteLine("");
  Console::WriteLine("Program:     " + m_msProgram);
  Console::WriteLine("");
  Console::WriteLine("Description: ");
  Console::WriteLine("");

  array<Char> ^ space = { ' ' };
  array<String ^> ^ words = m_msDescription->Split(space);
  int length = 0;

  for each ( String ^ msWord in words )
  {
    if ( length >= 80 )
    {
      length = 0;
      Console::WriteLine("");
    }
    else
    {
      length = length + (msWord->Length + 1);
      Console::Write(msWord + " ");
    }
  }
  Console::WriteLine("");
  Console::WriteLine("");
  
  Console::WriteLine("Parameters:");
  Console::WriteLine("");
  Console::WriteLine("[-a AE-Title]     AE Title of the application (Default is V3D_SCU).");
  Console::WriteLine("");
  Console::WriteLine("[-s Server]       Host or IP Address of the DICOM server.");
  Console::WriteLine("");
  Console::WriteLine("[-c AE-Title]     AE Title of the DICOM server.");
  Console::WriteLine("");
  Console::WriteLine("[-h Hostname]     Name of host in the Host.xml file.");
  Console::WriteLine("");
  Console::WriteLine("[-p Port]         DICOM server port to connect to.");
  Console::WriteLine("");
  Console::WriteLine("[-v]              Verbose logging.");
  Console::WriteLine("");
  Console::WriteLine("[-w]              Flag to indicate that the application needs to wait for a user to continue.");
  Console::WriteLine("");
  Console::WriteLine("[-n Timeout]      Network timeout. Used if there is high latency in your network, ");
  Console::WriteLine("                  increase the timeout on the TCP connection so you don't get ");
  Console::WriteLine("                  connection errors.");
  Console::WriteLine("");
  Console::WriteLine("[-b Mode]         Mode is either BLOCKING or NONBLOCKING. This is used for the connection and");
  Console::WriteLine("                  overrides the default value of NONBLOCKING and the attribute of from the Host");
  Console::WriteLine("                  table if present");
  Console::WriteLine("");
  Console::WriteLine("[-t list]         Transfer syntax list. This is a commas delimited list of transfer");
  Console::WriteLine("                  syntaxes that you want implemented on the connection. Having a '-'");
  Console::WriteLine("                  pre-pended to any of these uids will remove them instead of add ");
  Console::WriteLine("                  them. Below is a list of names that can be used instead of the ");
  Console::WriteLine("                  UID. Any other transfer syntaxes not on the list you will need to");
  Console::WriteLine("                  use the UID.");
  Console::WriteLine("");
  Console::WriteLine("                  Uid                   Names");
  Console::WriteLine("                  1.2.840.1008.1.2      little-endian-implicit, implicit");
  Console::WriteLine("                  1.2.840.1008.1.2.1    little-endian-explicit, explicit");
  Console::WriteLine("                  1.2.840.1008.1.2.2    big-endian-implicit");
  Console::WriteLine("");
  Console::WriteLine("                  Example:");
  Console::WriteLine("                           implicit,-explicit    ");
  Console::WriteLine("");
  Console::WriteLine("                           Turns off little-endian-explicit and turns on ");
  Console::WriteLine("                           litte-endian-implicit");
  Console::WriteLine("");
  Console::WriteLine("                           1.2.840.1008.1.2,-1.2.840.1008.1.2.1    ");
  Console::WriteLine("");
  Console::WriteLine("                           Does the same as above only using the actual uid's");
  Console::WriteLine("");
  Console::WriteLine("[-a list]         Abstract syntax list. This is a commas delimited list of abstract");
  Console::WriteLine("                  syntaxes that you want implemented on the connection. Having a '-'");
  Console::WriteLine("                  pre-pended to any of these uids will remove them instead of add ");
  Console::WriteLine("                  them. Below is a list of names that can be used instead of the ");
  Console::WriteLine("                  UID. Any other transfer syntaxes not on the list you will need to");
  Console::WriteLine("                  use the UID.");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("                  Example:");
  Console::WriteLine("                           1.2.840.10008.5.1.4.1.1.4,-1.2.840.10008.5.1.4.1.1.2    ");
  Console::WriteLine("");
  Console::WriteLine("                           Turns off CT image types and turns on ");
  Console::WriteLine("                           MR image types");
  Console::WriteLine("");

 

} // Help
