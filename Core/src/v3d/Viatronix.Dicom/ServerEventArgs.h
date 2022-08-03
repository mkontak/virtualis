// $Id:Events.h,v 1.11 2009/11/03 14:10:54 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once


#include "BaseEventArgs.h"

// Namespaces
using namespace System;

OPEN_DICOM_NS

/// Forward declarations
ref class QueryParameters;


/**
 * Implements the Server (SCP) event args that are created
 * from the callback's.
 */
[Serializable]
public ref class ServerEventArgs : public BaseEventArgs
{

#pragma region constructor 

private:

   /// Constructor
 ServerEventArgs();
 

#pragma endregion

public:

  /// Creates the event args from the callback parameters
  static ServerEventArgs ^ Create(IntPtr dcmCallbackParametersPtr, IntPtr lpCallbackDataPtr );



  /// Generate the XML record to the database insert
  String ^ ToXml();


#pragma region properties

public:



  /// Dicom file 
  property String ^ FilePath
  {
    String ^ get() { return m_msFilePath; }
    void set(String ^ msValue) { m_msFilePath = msValue; }
  } 

  /// Thumbnail Path 
  property String ^ ThumbnailPath
  {
    String ^ get() { return m_msThumbnailPath; }
    void set(String ^ msValue) { m_msThumbnailPath = msValue; }
  } 



  property String ^ Results
  {
    String ^ get() { return m_sResults; }
    void set(String ^ msValue) { m_sResults = msValue; }
  } 

  property Viatronix::Dicom::QueryParameters ^ QueryParameters
  {
    Viatronix::Dicom::QueryParameters ^ get() { return m_mpQueryParameters; }
  } 

  /// Gets the dicom header
  property String ^ Header
  {
    String ^ get() { return m_msHeader; }
  } // Header


#pragma endregion


#pragma region fields

private:


  /// Version
  String ^ m_mpVersion;

  /// File path to write a dicom file to
  String ^ m_msFilePath;

  /// File path to write a thumbnail to
  String ^ m_msThumbnailPath;


  /// Results based on query or move
  String ^ m_sResults;

  /// Dicom header ( xml string )
  String ^ m_msHeader;

  /// Query Parameters
  Viatronix::Dicom::QueryParameters ^ m_mpQueryParameters;


#pragma endregion

};  // class EventArgs

/// Thread Event Handler Delegate
public delegate void ServerEventHandler(System::Object ^ mpSender, ServerEventArgs ^ e );

CLOSE_DICOM_NS
