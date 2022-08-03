// $Id: FileBinder.h,v 1.3 2008/04/03 19:50:32 gdavidson Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declarations
#pragma once


// include declarations
#include <atlbase.h>


/**
 * Provides a class for storing a fileystem hierarchy in a single file.
 */
class VX_BASE_DLL FileBinder
{
// member functions
public:
  
  /// constructor
  FileBinder();
  
  /// destructor
  virtual ~FileBinder();

  /// creates the binder file
  void Create( const std::string & sFilename );

  /// opens an existing binder file
  void Open( const std::string & sFilename );

  /// closes the file binder object
  void Close();

  /// extracts the contents of the file binder to the specified directory ( NOTE: destination directory assumed to exist )
  void Extract( const std::string & sDestination );

  /// adds the specified file to the binder
  void AddFile( const std::string & sFilename );

  /// adds the specified file to the binder under the logical directory 
  void AddFile( const std::string & sLogicalDirectory, const std::string & sFilename );

  /// adds the directory and all of it's contents to the binder
  void AddDirectory( const std::string & sDirectory );

  /// adds the directory and all of it's contents to the binder under the logical directory 
  void AddDirectory( const std::string & sLogicalDirectory, const std::string & sDirectory );

  /// adds all the children (subdirectories and files) to this binder
  void AddChildrenOfDirectory( const std::string & sDirectory );

private:

  /// gets the storages directory name
  void GetDirectoryNameForStorage( CComPtr< IStorage > & spStorage, CComBSTR & sDirectoryName );

  /// gets if a storage exists for the specified directory
  bool FindChildStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & sDirectoryName, CComPtr< IStorage > & spStorage );


  void CreateStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & bsName, CComPtr< IStorage > & spStorage );

  /// creates an IStorage instance for the specified directory
  void CreateStorageFromDirectory( CComPtr< IStorage > & spStorage, const std::string & sDirectory, bool bChildrenOnly );
  
  /// creates an IStream instance for the specified file
  void CreateStreamFromFile( CComPtr< IStorage > & spStorage, const std::string & sFilename );

  /// creates a hierachical storage sturcture based on the supplied directories
  void AddObjectToLogicalDirectory( const CComPtr< IStorage > & spParentStorage, const std::string & sLogicalDirectory, const std::string & sFilePath, bool bIsFile );

  /// extracts the specified IStorage and all it's contents to the specified directory
  void ExtractStorageToDirectory( CComPtr< IStorage > & spParentStorage, const std::string & sDirectory );
  
  /// extracts file contained in the stream to the specified directory
  void ExtractStreamToFile( CComPtr< IStream > & spStream, const std::string & sDirectory );

  /// gets a unique name for the storages and stream
  void GetUniqueName( CComBSTR & bsName );

  /// gets the filename( including extension ) from the specified full file path
  void GetFileName( const std::string & sFilePath, CComBSTR & bsFilename );

  /// gets the directory name from the specified path
  void GetDirectoryName( const std::string & sDirectoryPath, CComBSTR & bsDirectoryName );

// member variables
private:

  // root IStorage interface
  CComPtr<IStorage> m_spRootStg;

}; // class FileBinder


// $Log: FileBinder.h,v $
// Revision 1.3  2008/04/03 19:50:32  gdavidson
// Added AddChildrenOfDirectory method
//
// Revision 1.2  2008/01/23 18:42:51  geconomos
// added ability to add files and directories to a specified directory sturcture
//
// Revision 1.1  2008/01/16 21:57:33  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/FileBinder.h,v 1.3 2008/04/03 19:50:32 gdavidson Exp $
// $Id: FileBinder.h,v 1.3 2008/04/03 19:50:32 gdavidson Exp $
