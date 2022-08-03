// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Loader.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ReaderGlobal.h"
#include "StudyLoadParamsEx.h"

class Loader
{
public:

  /// constructor
  Loader( const StudyLoadParamsEx & params, ReaderLib::StudyLoad::LoadCallback callback );

  /// destructor
  virtual ~Loader() = 0;

public:

  /// load operation
  virtual void Load() = 0;

  /// abort operation
  virtual void Abort() = 0;

  /// returns the load parameters
  inline const StudyLoadParamsEx & GetParams() const { return m_params; }

  /// returns the load parameters
  inline StudyLoadParamsEx & GetParams() { return m_params; }

protected:

  /// returns the callback
  inline ReaderLib::StudyLoad::LoadCallback & GetCallback() { return m_callback; }

private:

  /// private copy constructor
  Loader( const Loader & );

  /// private assignment operator
  Loader & operator=( const Loader & );

private:

  /// callback operation
  ReaderLib::StudyLoad::LoadCallback m_callback;

  /// study load params
  StudyLoadParamsEx m_params;

};

