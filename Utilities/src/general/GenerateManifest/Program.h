#pragma once

// namespaces
OPEN_MANIFEST_NS
using namespace System;

/**
 * Contains the main entry point.
 */
ref class Program
{
// construction
public:

  /// constructor.
  Program(void);

// methods
internal:

  /// The start point.
  void Start(cli::array<String ^> ^ mpArgs);

private:

  /// Handles the arguments.
  void HandleArguments(cli::array<String ^> ^ mpArgs);

  /// Displays the help text to inform the user about the usage of the program.
  void DisplayHelp();

// fields
private:

  /// The input directory.
  String ^ m_mpInputDirectory;
  
  /// The output file.
  String ^ m_mpOutputFile;

  /// Denotes the help mode.
  bool m_bHelpMode;

}; // class Program

// namespaces
CLOSE_MANIFEST_NS