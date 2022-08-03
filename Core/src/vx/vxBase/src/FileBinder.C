// $Id: FileBinder.C,v 1.4 2008/04/03 19:50:32 gdavidson Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "FileBinder.h"

#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 */
FileBinder::FileBinder()
{
} // FileBinder()


/**
 * Destructor
 */
FileBinder::~FileBinder()
{
  Close();
} // ~FileBinder()


/**
 * Creates a binder file.
 *
 * @param   sFilename   filename of binder file
 */
void FileBinder::Create( const std::string & sFilename )
{
  USES_CONVERSION;

  DWORD dwFlags = STGM_CREATE | STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
  
  HRESULT hr;
  if( FAILED( hr = StgCreateDocfile( CA2W( sFilename.c_str() ), dwFlags, 0, &m_spRootStg )))
    throw ex::IOException( LogRec( "Unable able to create file binder. StgCreateDocfile returned " + ToAscii( hr ), "FileBinder", "Create" ));
} // Create( const std::string & sFilename )


/**
 * Opens an existing binder file.
 *
 * @param   sFilename   filename of binder file
 */
void FileBinder::Open( const std::string & sFilename )
{
  USES_CONVERSION;

  DWORD dwFlags =   STGM_DIRECT | STGM_READ | STGM_SHARE_DENY_WRITE;
  
  HRESULT hr = S_OK;
  if( FAILED( hr = StgOpenStorage( CA2W( sFilename.c_str() ), NULL, dwFlags, NULL,  0, &m_spRootStg )))
    throw ex::IOException( LogRec( "Unable able to open file binder. StgOpenStorage returned " + ToAscii( hr ), "FileBinder", "Open" ));
} // Open( const std::string & sFilename )


/**
 * Closes a binder file.
 */
void FileBinder::Close()
{
  // release underlying root storage
  m_spRootStg = NULL;
} // Close()



/**
 * Extracts the contents of the file binder to the specified directory ( NOTE: destination directory assumed to exist )
 *
 * @param   sDestiation   destination directory
 */
void FileBinder::Extract( const std::string & sDestination )
{
  ExtractStorageToDirectory( m_spRootStg, sDestination );
} // Extract( const std::string & sDestination )


/**
 *  Adds the specified directory and all of it's contents to the root of the binder file. The filesystem hierarchy is preserved.
 *
 * @param   sDirectory      directory to add
 */
void FileBinder::AddDirectory( const std::string & sDirectory )
{
  CreateStorageFromDirectory( m_spRootStg, sDirectory, false );
} // AddDirectory( const std::string & sDirectory )


/**
 *  Adds the specified directory and all of it's contents to the root of the binder file. The filesystem hierarchy is preserved.
 *
 * @param   sDirectory      directory to add
 */
void FileBinder::AddChildrenOfDirectory( const std::string & sDirectory )
{
  CreateStorageFromDirectory( m_spRootStg, sDirectory, true );
} // AddChildrenOfDirectory( const std::string & sDirectory )



/**
 *  Adds the specified directory and all of it's contents to the root of the binder file. The filesystem hierarchy is preserved.
 *
 * @param   sDirectory      directory to add
 */
void FileBinder::AddDirectory(  const std::string & sLogicalDirectory, const std::string & sDirectory )
{
  AddObjectToLogicalDirectory( m_spRootStg, sLogicalDirectory, sDirectory, false );
} // AddDirectory( const std::string & sDirectory )


/**
 * Adds the specified file to the root of the file binder.
 *
 * @param   sFilename     file to add
 */
void FileBinder::AddFile( const std::string & sFilename )
{
  CreateStreamFromFile( m_spRootStg,  sFilename );
} // AddFile( const std::string & sFilename )


/**
 * Adds the specified file to the root of the file binder under the logical directory
 *
 * @param   sFilename     file to add
 */
void FileBinder::AddFile( const std::string & sLogicalDirectory, const std::string & sFilename )
{
  AddObjectToLogicalDirectory( m_spRootStg, sLogicalDirectory, sFilename, true );
} // AddFile( const std::string & sFilename )


/**
 * Creates a hierachical storage sturcture based on the supplied directories.
 *
 * @param   spParentStorage     parent storage
 * @param   sLogicalDirectory   logical sturcture to add
 * @param   sObjectPath         path to file/directory to add to logical structure
 * @param   bIsFile             true if object is a file; otherwise a directory is assumed
 */
void FileBinder::AddObjectToLogicalDirectory( const CComPtr< IStorage > & spParentStorage, const std::string & sLogicalDirectory, const std::string & sObjectPath, bool bIsFile   )
{
  char sDir[ _MAX_DIR ];
  strcpy( sDir, sLogicalDirectory.c_str() );

  std::vector< CComPtr< IStorage > > storages;

  CComPtr< IStorage > spStorageP = spParentStorage;
  CComPtr< IStorage > spStorageC;

  char * sToken = strtok( sDir, "\\" );
  while( sToken != NULL )
  {
    CComBSTR bsDirectoryName( sToken );

    if( FindChildStorage( spStorageP, bsDirectoryName, spStorageC ) )
    {
      storages.push_back( spStorageC );
      spStorageP = spStorageC;
    }
    else
    {
      CreateStorage( spStorageP, sToken, spStorageC );
      storages.push_back( spStorageC );
      spStorageP = spStorageC;
    }
    spStorageC = NULL;

    sToken = strtok( NULL, "\\" );
  }

  CComPtr< IStorage > spObjectStorage = storages[ storages.size() -1 ];
  if( bIsFile )
    CreateStreamFromFile( spObjectStorage,  sObjectPath );
  else
    CreateStorageFromDirectory( spObjectStorage, sObjectPath, false );
} // AddObjectToLogicalDirectory( const CComPtr< IStorage > & spParentStorage, const std::string & sLogicalDirectory, const std::string & sObjectPath, bool bIsFile   )


/**
 * Creates a storage within the specified parent storage
 *
 * @param   spParentStorage     parent storage
 * @param   bsDirectoryName     directory name
 * @param   spStorage           upon return contains the newly create storage
 */
void FileBinder::CreateStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & bsDirectoryName, CComPtr< IStorage > & spStorage )
{
  // create the name for the IStorage
  CComBSTR bsName;
  GetUniqueName( bsName );

  HRESULT hr = spParentStorage->CreateStorage( bsName, STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &spStorage );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able to create IStorage instance. CreateStream returned " + ToAscii( hr ), "FileBinder", "CreateStorage" ) );
 
  // create the "NAME" stream
  CComPtr< IStream > spStream;
  hr = spStorage->CreateStream( L"NAME", STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &spStream );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able to create IStream instance. CreateStream returned " + ToAscii( hr ), "FileBinder", "CreateStorage" ) );
  
  hr = const_cast< CComBSTR & >( bsDirectoryName ).WriteToStream( spStream );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able directory name to BSTR stream " + ToAscii( hr ), "FileBinder", "CreateStorage" ) );

  spStream == NULL;
} //CreateStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & bsDirectoryName, CComPtr< IStorage > & spStorage )


/**
 * Creates an IStorage instance for the specified directory.
 *
 * NOTE: Each substorage create will contain a stream called "NAME" which contains the filesystem name of the directory
 *
 * @param   spParentStorage     parent torage that will contained newly created storage
 * @param   sDirectory          directory to create storage for
 */
void FileBinder::CreateStorageFromDirectory( CComPtr< IStorage > & spParentStorage, const std::string & sDirectory, bool bChildrenOnly )
{
  CComBSTR bsDirectoryName;
  GetDirectoryName( sDirectory, bsDirectoryName );
 
  CComPtr< IStorage > spStorage;
  if( !bChildrenOnly )
  {
    if( !FindChildStorage( spParentStorage, bsDirectoryName, spStorage ) )
      CreateStorage( spParentStorage, bsDirectoryName, spStorage );  
  }
  else
  {
    spStorage = spParentStorage;
  }
 
  // iterate over all items contained in the directory
  char s[_MAX_PATH ];
  sprintf( s, "%s\\*", sDirectory.c_str() );
  WIN32_FIND_DATA ffd;
  HANDLE hFind = FindFirstFile( s, &ffd );
  if( hFind == INVALID_HANDLE_VALUE )
    throw ex::IOException( LogRec( "Unable able to create enum files " + ToAscii( GetLastError() ), "FileBinder", "CreateStorageFromDirectory" ) );

  do
  {
    // check that it isn't the default junk windows returns
    if( strcmp( ffd.cFileName, "." ) != 0 && strcmp( ffd.cFileName, ".." ) != 0 )
    {
      char sFullFilePath[ _MAX_PATH ];
      sprintf( sFullFilePath, "%s\\%s", sDirectory.c_str(), ffd.cFileName );

      if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        CreateStorageFromDirectory( spStorage, sFullFilePath, false );
      else
        CreateStreamFromFile( spStorage,  sFullFilePath );
    }
  } while( FindNextFile( hFind, &ffd ) != 0 );

  // close search handle
  FindClose( hFind );
} // CreateStorageFromDirectory( CComPtr< IStorage > & spParentStorage, const std::string & sDirectory )


/**
 * Extracts the specified IStorage and all it's contents to the specified directory
 *
 * @spParentStorage   storage to extract
 * @sDirectory        fileystem directory to extract to
 */
void FileBinder::ExtractStorageToDirectory( CComPtr< IStorage > & spParentStorage, const std::string & sDirectory )
{
  USES_CONVERSION;

  // grab an enumerator to get sub storages and streams
  CComPtr< IEnumSTATSTG > spEnum;
  HRESULT hr = spParentStorage->EnumElements( 0, NULL, 0, &spEnum );
  if( FAILED( hr ) )
      throw ex::IOException( LogRec( "Unable able to create IEnumSTATSTG instance. EnumElements returned " + ToAscii( hr ), "FileBinder", "ExtractStorageToDirectory" ) );


  // enumerate over children
  STATSTG statstg;
  hr = spEnum->Next( 1, &statstg, 0 );
  while( hr == S_OK )
  {

    if ( statstg.pwcsName != __nullptr )
    {


      if( STGTY_STORAGE == statstg.type )
      {        
        // open child storage
        CComPtr< IStorage > spStorage;
        spParentStorage->OpenStorage( statstg.pwcsName, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, NULL, 0, &spStorage );

        CComBSTR bsDirecoryName;
        GetDirectoryNameForStorage( spStorage, bsDirecoryName );


        // format to get the full path
        char s[ _MAX_PATH ];
        wsprintf( s, "%s\\%s", sDirectory.c_str(), (LPSTR)CW2AEX<_MAX_DIR>( bsDirecoryName ) );
        if( CreateDirectory( s, NULL ) == FALSE )
        {
          int4 err = GetLastError();
        }

        // recursively extract sub-storage
        ExtractStorageToDirectory( spStorage, s );
      }
      else
      {
        // igonore the "NAME" stream
        CComBSTR bs(statstg.pwcsName);
        if( bs != L"NAME" )
        {
          // open stream containing file to extract
          CComPtr< IStream > spStream;
        
          HRESULT hr = spParentStorage->OpenStream( statstg.pwcsName, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &spStream );  
          if ( FAILED(hr) )
              throw ex::IOException( LogRec( "Unable to open stream " + ToAscii( hr ), "FileBinder", "ExtractStorageToDirectory" ) );

          ExtractStreamToFile( spStream, sDirectory );      
        }

      } //  else

      // cleanup
      CoTaskMemFree( statstg.pwcsName );
      statstg.pwcsName = __nullptr;

    } /// END ... If the name is NULL

    // next item please
    hr = spEnum->Next( 1, &statstg, 0 );

  } // while( hr == S_OK )

} // ExtractStorageToDirectory( CComPtr< IStorage > & spParentStorage, const std::string & sDirectory )


/**
 * Creates an IStream instance for the specified file
 * 
 * NOTE: the stream contains two items: (1) filesystem name (2) file data
 *
 * @param   spStorage   parent storage
 * @param   sFilename   file to store in stream
 */
void FileBinder::CreateStreamFromFile( CComPtr< IStorage > & spStorage, const std::string & sFilename )
{
  HRESULT hr = S_OK;

  // create the name for the IStream
  CComBSTR bsStreamName;
  GetUniqueName( bsStreamName );
  
  // attempt to create an IStream 
  CComPtr< IStream > spStream;
  hr = spStorage->CreateStream( bsStreamName, STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &spStream );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able to create IStream instance. CreateStream returned " + ToAscii( hr ), "FileBinder", "CreateStreamFromFile" ) );

  // get the filename plus extension from the full file path
  CComBSTR bsFilename;
  GetFileName( sFilename, bsFilename );

  // write the filename to the stream
  hr = bsFilename.WriteToStream( spStream );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able to write filename to IStream instance. WriteToStream returned " + ToAscii( hr ), "FileBinder", "CreateStreamFromFile" ) );

  // open the source file for copying
  HANDLE hFile = CreateFile(  sFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if( hFile == INVALID_HANDLE_VALUE )
    throw ex::IOException( LogRec( "Unable open source file for copying (" + ToAscii( GetLastError() ) + ")", "FileBinder", "CreateStreamFromFile" ) );

  // copy the file from disk into the IStream
  const DWORD dwBytesToRead = 1024;
  uint1 buffer[ dwBytesToRead ];
  while( true )
  {
    DWORD dwBytesRead = 0;
    
    if ( ReadFile( hFile, buffer, dwBytesToRead, &dwBytesRead, NULL ) == FALSE )
      throw ex::IOException( LogRec( "Failed to read file " + sFilename + " (" + ToAscii( GetLastError() ) + ")", "FileBinder", "CreateStreamFromFile" ) );

    spStream->Write( buffer, dwBytesRead, NULL );
    if( dwBytesRead < dwBytesToRead )
      break;
  }
  CloseHandle( hFile );
} // CreateStreamFromFile( CComPtr< IStorage > & spStorage, const std::string & sFilename )


/**
 * extracts file contained in the stream to the specified directory
 *
 * NOTE: the stream contains two items: (1) filesystem name (2) file data
 *
 * @param   spStream      stream containing file
 * @param   sDirectory    directory to extract file to
 */
void FileBinder::ExtractStreamToFile( CComPtr< IStream > & spStream, const std::string & sDirectory )
{
  USES_CONVERSION;

  // read in the filesystem name from the IStream
  CComBSTR bsFilename;
  HRESULT hr = bsFilename.ReadFromStream( spStream );

  if ( FAILED(hr ) )
    throw ex::IOException( LogRec( "Unable cread fromn stream (" + ToAscii( GetLastError() ) + ")", "FileBinder", "ExtractStreamToFile" ) );

  // format destination full path
  char sFilePath[ _MAX_PATH ];
  sprintf( sFilePath, "%s\\%s", sDirectory.c_str(), (LPSTR)CW2AEX<_MAX_DIR>( bsFilename ) );

  // create the source file for copying
  HANDLE hFile = CreateFile(  sFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if( hFile == INVALID_HANDLE_VALUE )
    throw ex::IOException( LogRec( "Unable create destination file for extracting. GetLastError return " + ToAscii( GetLastError() ), "FileBinder", "ExtractStreamToFile" ) );

  // copy file from IStream to disk
  const ULONG uBytesToRead = 1024;
  uint1 buffer[ uBytesToRead ];
  ULONG uBytesRead = 0;
  do
  {
    DWORD dwBytesWritten = 0;
    spStream->Read( buffer, uBytesToRead, &uBytesRead );
    if( uBytesRead > 0 )
      WriteFile( hFile, buffer, uBytesRead, &dwBytesWritten, NULL );
  } while( uBytesToRead == uBytesRead );
  
  CloseHandle( hFile );
} // ExtractStreamToFile( CComPtr< IStream > & spStream, const std::string & sDirectory )


/**
 * Gets the storages directory name
 *
 * @param     spStoarge     storage to retrieve directory name for
 * @param     bsName        upon return contains the storage name
 */
void FileBinder::GetDirectoryNameForStorage( CComPtr< IStorage > & spStorage, CComBSTR & bsName )
{
  // open "NAME" stream for filesystem directory
  CComPtr< IStream > spStream;
  HRESULT hr = spStorage->OpenStream( L"NAME", NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &spStream );  
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable able to create IStream instance fort stream NAME " + ToAscii( hr ), "FileBinder", "GetStorageDirectoryName" ) );

  // read in the filesystem directory name from the disk
  hr = bsName.ReadFromStream( spStream );
  if( FAILED( hr ) )
    throw ex::IOException( LogRec( "Unable to deserialize NAME stream " + ToAscii( hr ), "FileBinder", "GetStorageDirectoryName" ) );

} // GetDirectoryNameForStorage( CComPtr< IStorage > & spStorage, CComBSTR & bsName  )


/**
 * Finds the child storage with the matching directory name
 *
 * @param     spParentStorage     storage containg sub-storage to search
 * @param     bsDirectoryName     directory name to search for
 * @param     spStorage           upon return contains IStorage to matching child storage
 * @return    true if storage was found
 */
bool FileBinder::FindChildStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & bsDirectoryName, CComPtr< IStorage > & spStorage )
{
  bool bFound = false;

  // grab an enumerator to get sub storages and streams
  CComPtr< IEnumSTATSTG > spEnum;
  HRESULT hr = spParentStorage->EnumElements( 0, NULL, 0, &spEnum );

  // enumerate over children
  STATSTG statstg;
  hr = spEnum->Next( 1, &statstg, 0 );
  while( hr == S_OK )
  {
    if( STGTY_STORAGE == statstg.type )
    {        
      // open child storage
      CComPtr< IStorage > spChildStorage;
      spParentStorage->OpenStorage( statstg.pwcsName, NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, 0, &spChildStorage );

      CComBSTR bsChildDirectoryName;
      GetDirectoryNameForStorage( spChildStorage, bsChildDirectoryName );
      if( bsDirectoryName == bsChildDirectoryName )
      {
        spStorage = spChildStorage;
        bFound = true;
        break;
      }     
    }

    CoTaskMemFree( statstg.pwcsName );
    statstg.pwcsName = NULL;

    hr = spEnum->Next( 1, &statstg, 0 );
  }
  return bFound;
} // FindChildStorage( const CComPtr< IStorage > & spParentStorage, const CComBSTR & sDirectoryName, CComPtr< IStorage > & spStorage )


/**
 * Gets the filename( including extension ) from the specified full file path
 *
 * @param   sFilePath   full file path
 * @param   bsFileName  upon return contains the filename plus the extension
 */
void FileBinder::GetFileName( const std::string & sFilePath, CComBSTR & bsFileName )
{
   char sFileName[ _MAX_FNAME ], sFileExt[ _MAX_EXT ];
  _splitpath( sFilePath.c_str(), NULL, NULL, sFileName, sFileExt );

  char sFile[ _MAX_PATH ];
  wsprintf( sFile, "%s%s", sFileName, sFileExt );

  bsFileName = sFile;
} // GetFileName( const std::string & sFilePath, CComBSTR & bsFileName )


/**
 * Gets the directory name from the specified path.
 *
 * @param   sDirectoryPath    full directory path
 * @param   bsDirectoryName   upon return contains the filesystem directory name
 */
void FileBinder::GetDirectoryName( const std::string & sDirectoryPath, CComBSTR & bsDirectoryName )
{
  char sDirectory[ _MAX_DIR ];
  strcpy( sDirectory, sDirectoryPath.c_str() );

  char * t = strtok( sDirectory, "\\" );
  char * u = t;
  while( (t = strtok( NULL, "\\" )) != NULL )
    u = t;
  if( u != NULL )
    bsDirectoryName = u;
} // GetDirectoryName( const std::string & sDirectoryPath, CComBSTR & bsDirectoryName )


/**
 * Gets a unique name for the storages and stream. You guessed it's a GUID!
 *
 * @param   bsName    upon return contains the unique filename
 */
void FileBinder::GetUniqueName( CComBSTR & bsName )
{
  GUID guid;
  CoCreateGuid( &guid );

  TCHAR szGuid[250];
  wsprintf(
    szGuid, 
    _T("%04X%02X%02X%02X%02X%02X%02X%02X%02X%02X"),
    guid.Data1, 
    guid.Data2, 
    guid.Data3,
    guid.Data4[0], 
    guid.Data4[1],
    guid.Data4[2], 
    guid.Data4[3], 
    guid.Data4[4], 
    guid.Data4[5], 
    guid.Data4[7]
  );
  bsName = szGuid;
} // GetUniqueName( CComBSTR & bsName )


// $Log: FileBinder.C,v $
// Revision 1.4  2008/04/03 19:50:32  gdavidson
// Added AddChildrenOfDirectory method
//
// Revision 1.3  2008/03/04 16:40:38  gdavidson
// Fixed bug with open File handle
//
// Revision 1.2  2008/01/23 18:42:51  geconomos
// added ability to add files and directories to a specified directory sturcture
//
// Revision 1.1  2008/01/16 21:57:33  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/FileBinder.C,v 1.4 2008/04/03 19:50:32 gdavidson Exp $
// $Id: FileBinder.C,v 1.4 2008/04/03 19:50:32 gdavidson Exp $
