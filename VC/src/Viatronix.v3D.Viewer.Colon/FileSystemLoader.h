// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FileSystemLoader.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "Loader.h"
#include <functional>

/**
 * Implements the loading of the dataset from the file system such as the stand alone launching
 */
class FileSystemLoader : public Loader
{
public:

  /// constructor
  FileSystemLoader( const StudyLoadParamsEx & params, ReaderLib::StudyLoad::LoadCallback callback );

  /// destructor
  virtual ~FileSystemLoader();

  /// load operation
  virtual void Load();

  /// abort operation
  virtual void Abort();

protected:
	/// macro
  typedef std::vector< ReaderLib::Dataset * > DatasetVector;
  typedef std::function< void( ReaderLib::Dataset &, const uint4, ReaderLib::StudyLoad::LoadCallback &, const bool, const uint4 )> LoadFunctionPtr;

  /// single threaded sequential load
  virtual void LoadSequential( LoadFunctionPtr & loadFunction, DatasetVector & datasets );

  /// multi-threaded load
  virtual void LoadParallel( LoadFunctionPtr & loadFunction, DatasetVector & datasets );

  /// initialization for loader
  virtual void Initialize();

  /// intialize dataset
  virtual void InitializeDataset( ReaderLib::Dataset & dataset, const ReaderLib::DatasetLoadParams & params );

private:

  /// create load function
  void CreateLoadFunction( LoadFunctionPtr & functionPtr );

  /// Background thread study load function (does nothing)
  static int4 EmptyLoadNotify( void* pData, const ReaderLib::StudyLoad::StepsEnum ePrevStep,
                               const ReaderLib::StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                               const ReaderLib::DatasetOrientations::TypeEnum eNewOrientation );

private:

  /// private copy constructor
  FileSystemLoader( const FileSystemLoader & );

  /// private assignment operator
  FileSystemLoader & operator=( const FileSystemLoader & );

private:

  /// using multi-threaded load?
  bool m_bIsMultiThreaded;

};

