// $Id: DatasetLoadParams.h,v 1.3 2006/05/12 20:29:20 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#pragma once

#include "FileExtensions.h"
#include "StudyLoad.h"

// namespace delcaration
namespace ReaderLib 
{

// class declaration
class DatasetLoadParams
{
// member functions
public:

  /// constructor
  DatasetLoadParams( DatasetOrientations::TypeEnum eOrientation );
  
  /// copy constructor
  DatasetLoadParams( const DatasetLoadParams & other );

  /// detstructor
  virtual ~DatasetLoadParams();

  /// gets the dataset prefix
  const std::string & GetPrefix() const { return m_sPrefix; }

  /// sets the dataset prefix
  void SetPrefix( const std::string & sPrefix ) { m_sPrefix = sPrefix; }

  /// gets the volume directory
  const std::string & GetVolumeDirectory() const { return m_sVolumeDirectory; }

  /// sets the volume directory
  void SetVolumeDirectory( const std::string & sVolumeDirectory ) { m_sVolumeDirectory = sVolumeDirectory; }

  /// gets the preprocessed directory
  const std::string & GetPreprocessedDirectory() const { return m_sPreprocessedDirectory; }

  /// sets the preprocessed directory
  void SetPreprocessedDirectory( const std::string & sPreprocessedDirectory ) { m_sPreprocessedDirectory = sPreprocessedDirectory; }

  /// gets the cad directory
  const std::string & GetCadDirectory() const { return m_sCadDirectory; }

  /// sets the cad directory
  void SetCadDirectory( const std::string & sCadDirectory ) { m_sCadDirectory = sCadDirectory; }

  /// gets the uid
  const std::string & GetUID() const { return m_sUID; }

  /// sets the uid
  void SetUID( const std::string & sUID ) { m_sUID = sUID; }

  /// returns the dataset orientation
  const DatasetOrientations::TypeEnum GetOrientation() const { return m_eOrientation; }

  /// assignment operator
  DatasetLoadParams & operator=( const DatasetLoadParams & rhs );

  /// helper function thats appends a directory and the prefix
  static std::string FormatPrefixPath( const std::string & sDirectory, const std::string & sPrefix );

  /// helper function tat generates the prefix for the dataset
  static std::string GeneratePrefix( const std::string & sVolumeDirectory, const std::string & sHint = std::string( "" ) );

  /// helper function tat generates the prefix for the dataset
  static std::string GeneratePrefix( const std::string & sVolumeDirectory, const vx::FileExtension & fileExt );

  /// finds the specified file and returns the filename minus the extension
  static bool FindFilename( const std::string & sDirectory, const std::string & sExtension, std::string & sFilename );
  
	/// set the dataset orientation enum
	void SetOrientation(const DatasetOrientations::TypeEnum & eOrientation) { m_eOrientation = eOrientation; }


// member variables
private:

  // dataset prefix
  std::string m_sPrefix;

  // volume directory
  std::string m_sVolumeDirectory;

  // volume directory
  std::string m_sPreprocessedDirectory;

  // cad directory
  std::string m_sCadDirectory;

  /// uid
  std::string m_sUID;

  // dataset orientation
  DatasetOrientations::TypeEnum m_eOrientation;

}; // class DatasetLoadParams

} // namespace ReaderLib 

// $Log: DatasetLoadParams.h,v $
// Revision 1.3  2006/05/12 20:29:20  jmeade
// Issue 4795: use proper prefix for cad xml file.
//
// Revision 1.2  2005/11/16 14:28:07  geconomos
// added the ability to hint at which volume ( .ecv or .vol ) to base the dataset prefix on
//
// Revision 1.1  2005/11/15 16:13:52  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/DatasetLoadParams.h,v 1.3 2006/05/12 20:29:20 jmeade Exp $
// $Id: DatasetLoadParams.h,v 1.3 2006/05/12 20:29:20 jmeade Exp $
