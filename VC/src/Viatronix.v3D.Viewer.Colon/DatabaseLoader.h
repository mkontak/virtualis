// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
//File: DatabaseLoader.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "FileSystemLoader.h"
#include <functional>

class DatabaseLoader : public FileSystemLoader
{
public:

  /// constructor
  DatabaseLoader( const StudyLoadParamsEx & params, ReaderLib::StudyLoad::LoadCallback callback );

  /// destructor
  virtual ~DatabaseLoader();

  /// load operation
  virtual void Load();

private:

  /// copy constructor
  DatabaseLoader( const DatabaseLoader & );

  /// assignment operator
  DatabaseLoader & operator=( const DatabaseLoader & );
};

