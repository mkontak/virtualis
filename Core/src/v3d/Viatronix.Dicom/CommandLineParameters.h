// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ComandLineParameters.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once



/// Usings
using namespace System;
using namespace System::Collections;
using namespace System::Xml;

OPEN_DICOM_NS

public ref class CommandLineParameters
{

protected:

  CommandLineParameters();
  
public:

  /// Creates the command line parameters sing the xml from a config file
  static CommandLineParameters ^ Create(String ^ msCommand, String ^ msDescription, XmlNode ^ mpNode);

  /// Update the parameters with the command line options
  virtual int Update(array<String ^> ^ mpArgs);

  /// Dumps the help to the console
  virtual void Help();

  /// Try and get the parameter value
  generic<typename T> 
  bool TryGetParameter(String ^ msName, T % value)
  {
    bool bFound(m_mpParameters->ContainsKey(msName));

    if ( bFound )
      value = (T)System::Convert::ChangeType(m_mpParameters[msName], T::typeid); 

    return bFound;

  } // TryGetParameter()


  /// Validates the data
  void Validate();

protected:

  /// Initializes the object using the xml node
  void Initialize(XmlNode ^ mpNode);

#pragma region properties

public:

  // Gets/Sets the Wait flag
  property bool Wait
  {
    bool get() { return m_bWait; }
    void set(bool value) { m_bWait = value; }
  } // Wait

  /// Gets the verbose flag
  property bool Verbose
  {
    bool get() { return m_bVerbose; }
    void set(bool value) { m_bVerbose = value; }
  } // Verbose


  /// <summary>
  /// Sets/Gets the AE Title
  /// </summary>
  property String ^ AETitle
  {
    String ^ get() { return m_msAEtitle; }
    void set(String ^ value)  { m_msAEtitle = value; }
  } // AETitle

  /// Gets the Server 
  property Viatronix::Dicom::Host ^ Server
  {
    Viatronix::Dicom::Host ^ get() { return m_mpHost; }
  } // Host

  /// Sets/Gets the network timeout
  property unsigned int NetworkTimeout
  {
    unsigned int get() { return m_iNetworkTimeout; }
    void set(unsigned int value) { m_iNetworkTimeout = value; }
  } // NetworkTimeout

  property BlockingModes BlockingMode
  {
    BlockingModes get() { return m_blockingMode; }
    void set ( BlockingModes value ) { m_blockingMode = value; }
  }

 
#pragma endregion

#pragma region fields

protected:

  /// Command parameter pairs
  Generic::Dictionary<String ^, String ^> ^ m_mpParameters;

  // Program name
  String ^ m_msProgram;

  /// Description
  String ^ m_msDescription;

private:


  // Blocking mode
  BlockingModes m_blockingMode;

  // Server Host
  Viatronix::Dicom::Host ^ m_mpHost;

  /// AE Title of this application
  String ^ m_msAEtitle;

  /// Network time out
  unsigned int m_iNetworkTimeout;

  /// Transfer syntaxes
  Generic::List<String ^> ^ m_mpTransferSyntaxes;

  /// Abstract syntaxes
  Generic::List<String ^> ^ m_mpAbstractSyntaxes;

  /// Verbose logging
  bool m_bVerbose;

  /// Flag to indicate if we are to wait for enter at the end
  bool m_bWait;

#pragma endregion

};  // class 

CLOSE_DICOM_NS