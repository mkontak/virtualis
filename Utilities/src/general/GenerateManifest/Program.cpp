// includes
#include "StdAfx.h"
#include "Program.h"
#include "FileVersionReader.h"

// namespaces
USING_MANIFEST_NS
using namespace System;
using namespace System::IO;

/**
 * Constructor.
 */
Program::Program() : m_bHelpMode(false)
{
} // Program()


/**
 * The main entry point.
 *
 * @param mpArgs  The arguments to take into account.
 */
void Program::Start(cli::array<System::String ^> ^ mpArgs)
{
  m_mpInputDirectory = "C:\\Program Files\\Viatronix";
  m_mpOutputFile = Path::Combine("C:\\Temp\\", Guid::NewGuid().ToString() + ".xml");

  Console::WriteLine();
  Console::WriteLine("Viatronix Generate Manifest");
  Console::WriteLine("Copyright © 2012 Viatronix Inc.");
  Console::WriteLine("===============================");
  Console::WriteLine();

  HandleArguments(mpArgs);

  if(!m_bHelpMode)
  {
    Console::WriteLine(String::Format("Source Directory: {0}", m_mpInputDirectory));
    Console::WriteLine(String::Format("Destination File: {0}", m_mpOutputFile));

    FileVersionReader ^ mpReader = gcnew FileVersionReader();

    mpReader->ReadDirectory(m_mpInputDirectory);
    mpReader->WriteManifest(m_mpInputDirectory, m_mpOutputFile);
  }
} // Start( mpArgs )


/**
 * Handles the arguments provided.
 *
 * @param mpArgs The arguments to take into account.
 */
void Program::HandleArguments(cli::array<String ^> ^ mpArgs)
{
  if(mpArgs->Length > 1)
  {
    // Iterate through the arguments.
    for(int i = 0; i < mpArgs->Length; ++i)
    {
      if((mpArgs[i] == "/i" || mpArgs[i] == "-i" || mpArgs[i] == "--input") && (mpArgs->Length > i + 1))
      {
        m_mpInputDirectory = mpArgs[i + 1];
      }
      else if((mpArgs[i] == "/o" || mpArgs[i] == "-o" || mpArgs[i] == "--output") && (mpArgs->Length > i + 1))
      {
        m_mpOutputFile = mpArgs[i + 1];
      }
    }
  }
  else
  {
    if(mpArgs->Length > 0)
    {
      if(mpArgs[0] == "/?" || mpArgs[0] == "-h" || mpArgs[0] == "--help")
      {
        DisplayHelp();
      }
    }
  }
} // HandleArguments( mpArgs )


/**
 * Displays the help text to inform the user about the usage of the program.
 */
void Program::DisplayHelp()
{
  m_bHelpMode = true;

  Console::WriteLine("GenerateManifest [/i | -i | --input][/o | -o | --output][/? | -h | --help]");
  Console::WriteLine("/i | -i | --input   Sets the input directory.  By default, \"C:\\Program Files\\Viatronix\" is used.");
  Console::WriteLine("/o | -o | --output  Sets the output file.  By default, \"C:\\Temp\\{GUID}.xml\" is used");
  Console::WriteLine("/? | -h | --help    Displays this message and returns immediately.");

 } // DisplayHelp()