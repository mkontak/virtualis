// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StudyLoadParamsEx.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "StudyLoadParams.h"

/**
 * Study Load Parameters Extension that extends the current StudyLoadParams
 */
class StudyLoadParamsEx : public ReaderLib::StudyLoadParams
{
public:

  /// study load flags
  enum LoaderStyleEnum
  { 
    LOADER_STYLE_LEGACY, 
    LOADER_STYLE_CONSOLE,
    LOADER_STYLE_DATABASE,
    LOADER_STYLE_PACS,
  };

public:

  /// constructor
  StudyLoadParamsEx();
  
  /// destructor
  virtual ~StudyLoadParamsEx();

  /// copy constructor
  StudyLoadParamsEx( const StudyLoadParamsEx & other );

  /// assignment operator
  StudyLoadParamsEx & operator=( const StudyLoadParamsEx & rhs );

public:

  /// gets the uids
  const std::string & GetUIDs() const { return m_sUIDs; }

  /// sets the uids
  void SetUIDs( const std::string & sUIDs ) { m_sUIDs = sUIDs; }

  /// gets the session uid
  const std::string & GetSessionUID() const { return m_sSessionUID; }

  /// sets the session uid
  void SetSessionUID( const std::string & sSessionUID ) { m_sSessionUID = sSessionUID; }

  /// get loader style
  LoaderStyleEnum GetLoaderStyle() const { return m_eLoaderStyle; }

  /// set the loader style
  void SetLoaderStyle( LoaderStyleEnum eLoaderStyle ) { m_eLoaderStyle = eLoaderStyle; }

private:
  
  /// private assignment operator for base class
  StudyLoadParamsEx & operator=( const StudyLoadParams & rhs );

private:

  /// collection of uids
  std::string m_sUIDs;

  /// session uid
  std::string m_sSessionUID;

  /// loader style
  LoaderStyleEnum m_eLoaderStyle;

};

