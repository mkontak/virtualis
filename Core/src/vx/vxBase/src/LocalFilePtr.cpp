// $Id: LocalFilePtr.C,v 1.5 2006/07/06 13:27:45 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren soeren@viatronix.net


// Includes
#include "stdafx.h"
#include "LocalFilePtr.h"
#include "vxTask.h"


using namespace io;


/**
 * Constructor
 *
 * @param sFilename the filename
 * @param sMode     the file mode
 */
LocalFilePtr::LocalFilePtr( const std::string & sFilePath, const uint4 uCacheSize ) :
m_hFile(INVALID_HANDLE_VALUE),
FilePtr(sFilePath, uCacheSize)
{

  /// Set the handle
  m_sHandle = vxLogUtility::GenerateGuid();

} // LocalFilePtr()



/**
 * Destructor
 */
LocalFilePtr::~LocalFilePtr()
{
  Close();
} // ~LocalFilePtr()


/**
 * Creates the Local file ptr object
 *
 * @param sFilePath
 *
 * @return LocalFilePtr
 */
std::shared_ptr<LocalFilePtr> LocalFilePtr::Create(const std::string & sFilePath, const uint4 uCacheSzie)
{
  std::shared_ptr<LocalFilePtr> pFilePtr = std::shared_ptr<LocalFilePtr>( new LocalFilePtr(sFilePath, uCacheSzie) );

  if ( pFilePtr == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate LocalFilePtr object", "LocalFilePtr", "Create"));

  return pFilePtr;

} // Create()

/**
 * Opens a file using fopen
 *
 * @param iModeFlags    Mode flags
 * @param iCreateFlags  Create flags
 */
void LocalFilePtr::Open( const int4 iModeFlags, const int4 iCreateFlags )
{

  if ( ! IsOpen() )
  {
    LogFyi(util::Strings::Format("Opening - %s",m_sFilePath.c_str()), "LocalFilePtr","Open");


    if ( (m_hFile = CreateFile( m_sFilePath.c_str(), (DWORD)iModeFlags, FILE_SHARE_READ, NULL, (DWORD)iCreateFlags, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL)) == INVALID_HANDLE_VALUE )
      throw ex::FileNotFoundException( LogRec(util::Strings::Format("Failed to open file %s (%d)",m_sFilePath.c_str(), ::GetLastError()), "LocalFilePtr", "Open" ) );

    // We only need to read ahead if we are reading
    if ( ( iModeFlags & GENERIC_READ) == GENERIC_READ  )
      ReadAhead();
  }
  else
    LogWrn(util::Strings::Format("File %s is already open", m_sFilePath.c_str()), "LocalFilPtr", "Open");
} // Open()


/**
 * Closes the file
 *
 * @return success
 */
void LocalFilePtr::Close()
{
  if ( IsOpen() )
  {

    LogFyi(util::Strings::Format("Closing - %s",m_sFilePath.c_str()), "LocalFilePtr","Close");

    /// Wait for any outstanding tasks
    m_task->Wait();

    if ( ! CloseHandle(m_hFile) )
      LogWrn(util::Strings::Format("Close of file %s failed (%d)",m_sFilePath.c_str(), GetLastError()),"LocalFilePtr","Close");

    m_hFile = INVALID_HANDLE_VALUE;
  }

  // Close the base (Clears the cache)
  FilePtr::Close();

 } // Close()


/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 LocalFilePtr::Read( void * pBuffer, const uint4 uBytesToRead )
{


  // Read from file buffer
  uint4 uBytesRead(0);


  // ===========================
  // Check if the file is open
  // ===========================
  if ( IsOpen() )
  {

    // Wait for any read ahead's
    m_task->Wait();

    LogDbg(util::Strings::Format("Read (%d bytes) - %s",uBytesToRead,m_sFilePath.c_str()), "LocalFilePtr","Read");
   
    // =====================================================================================
    // If caching is enabled then read from the cache first before going out to the file
    // ====================================================================================
    if ( IsCachingEnabled() )
    {

      // Read what I can from the buffer
      uBytesRead = ReadCache(pBuffer, uBytesToRead);

#ifdef _DEBUG
      // Uncomment this statement to gain additional debug information
      LogDbg(util::Strings::Format("%s: %d byte(s) read from buffer",m_sFilename.c_str(), uBytesRead), "LocalFilePtr","Read");
#endif 

      // calculate the remaining bytes needed after the reading from the cache
      const uint4 uRemainingBytes( uBytesToRead - uBytesRead );


      // ===========================================================================================
      // If there are remaining bytes to read and we are not at the End of File for the file then
      // issue a read.
      // ===========================================================================================
      if ( uRemainingBytes > 0 && ! m_bEof )
      {
      
#ifdef _DEBUG
        // Uncomment this statement to gain additional debug information
        LogDbg(util::Strings::Format("%s: Attempting to read %d byte(s) from file",m_sFilename.c_str(), uRemainingBytes), "LocalFilePtr","Read");
#endif

        uBytesRead += ReadFile(((char *)pBuffer + uBytesRead), uRemainingBytes );

#ifdef _DEBUG
        // Uncomment this statement to gain additional debug information
        LogDbg(util::Strings::Format("%s: %d byte(s) read",m_sFilename.c_str(), uBytesRead ), "LocalFilePtr","Read");
#endif

      }


      ReadAhead();
   
  
    }
    else
    {
      uBytesRead = ReadFile(pBuffer, uBytesToRead);

#ifdef _DEBUG
      // Uncomment this statement to gain additional debug information
      LogDbg(util::Strings::Format("%s: %d bytes read",m_sFilename.c_str(), uBytesRead), "LocalFilePtr","Read");
#endif

    }
 
  }
  else
    LogWrn(util::Strings::Format("Cannot read from %s, file is not open",m_sFilePath.c_str()), "LocalFilePtr","Read")

  return uBytesRead;

} // Read()


/**
 * Reads data from the file
 *
 * @param pBuffer      the buffer into which to read
 * @param uBytesToRead the total number of bytes to read
 */
uint4 LocalFilePtr::ReadFile( void * pBuffer, const uint4 uBytesToRead )
{

  /// Actual number of bytes read from the file
  DWORD dwBytesRead(uBytesToRead);


  /// Read from the file
  if ( ! ::ReadFile(m_hFile, pBuffer, (DWORD)uBytesToRead, &dwBytesRead, NULL ) )
    throw ex::IOException(LogRec(util::Strings::Format("Read failed (Error: %d)",GetLastError() ), "LocalFilePtr", "ReadFile" ));



  /// Total bytes read
  m_uTotalBytesRead += dwBytesRead;
  m_uFilePosition += dwBytesRead;

  // ==================================================================================================
  // Only issue warning if we read less bytes then expected. We chack for EOF if less bytes were read.
  // ===================================================================================================
  if ( dwBytesRead < uBytesToRead )
  {
     m_bEof = true;
    
     LogWrn(util::Strings::Format("Read failed, Expected to read %d bytes(s) but only read %d (Error: %d)", uBytesToRead, dwBytesRead, GetLastError() ), "LocalFilePtr", "ReadFile" );
  }


  // Return the number of bytes read
  return  (uint4)dwBytesRead;


} // ReadFile()


/** 
 * Writes data to the file
 *
 * @param pBuffer the output buffer
 * @uBytesToWrite the size of the output buffer
 */
void LocalFilePtr::Write( const void * pBuffer, const uint4 uBytesToWrite )
{


  if ( IsOpen() )
  {

    DWORD dwBytesWritten(0);
 
    LogDbg(util::Strings::Format("Write (%d bytes) - %s",uBytesToWrite,m_sFilePath.c_str()),"LocalFilePtr","Write");

    // Wait for any read ahead's
    m_task->Wait();

 
    /// Write data
    if ( ! ::WriteFile(m_hFile, pBuffer, (DWORD)uBytesToWrite, &dwBytesWritten, NULL) )
      throw ex::IOException( LogRec( util::Strings::Format( "Write failed (Error: 0x%08x) ",GetLastError() ), "LocalFilePtr", "Write" ) );


    // Keep track of the total bytes written
    m_uTotalBytesWritten += dwBytesWritten;


  }
  else
   LogWrn(util::Strings::Format("Cannot write to %s, file is not open",m_sFilePath.c_str()), "LocalFilePtr","Write")


} // Write()


/**
 * Flush the file to disk
 */
void LocalFilePtr::Flush()
{

  // ====================================
  // Can only flush if the file is open
  // =====================================
  if ( IsOpen() )
  {
    LogDbg(util::Strings::Format("Flush - %s",m_sFilePath.c_str()),"LocalFilePtr","Flush");

    /// Wait for any read ahead's
    m_task->Wait();

    if ( ! FlushFileBuffers(m_hFile) )
      throw ex::IOException( LogRec( util::Strings::Format( "Flush of file buffers failed (Error: 0x%08x) ",GetLastError() ), "LocalFilePtr", "Flush" ) );
   
  }
} // Flush()


/**
 * Seek the file pointer ahead from the current position
 *
 * @param uNumBytes the number of bytes to seek
 *
 * @return True if we are at EOF
 */
bool LocalFilePtr::SeekCurrent( const __int64 uNumBytes )
{

  // ======================================
  // Can only SEEK if the file is open
  // ======================================
  if ( IsOpen() )
  {

    LogDbg(util::Strings::Format("Seek (%d bytes) - %s",uNumBytes, m_sFilePath.c_str()),"LocalFilePtr","Seek");

    // Wait for any read ahead's
    m_task->Wait();


 
    // ============================================================================================================
    // If the caching is enabled and the number of bytes to seek is greater then the remaining bytes left in 
    // the cache we:
    //  1. throw away the bytes remaining in cache
    //  2. Clear the cache buffer
    //  3. Seek the file for the remaining bytes
    // ============================================================================================================
    if ( IsCachingEnabled() )
    {

      if (  uNumBytes > m_uCacheBytesRemaining) 
      {
        const uint4 seekBytes(uNumBytes - m_uCacheBytesRemaining);

        ClearCache();

        LARGE_INTEGER li;

        li.QuadPart = uNumBytes;

        li.LowPart = SetFilePointer (m_hFile, li.LowPart, &li.HighPart, FILE_CURRENT );
  

        // Only seek through file if not in buffer
        if (  li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
          throw ex::IOException( LogRec( util::Strings::Format( "Seek of (%I64d) bytes from current failed (Error: 0x%08x) ", uNumBytes, GetLastError() ), "LocalFilePtr", "SeekCurrent" ) );

      }
      else
      {
        m_uCacheBytesRead += uNumBytes;
        m_uCacheBytesRemaining -= uNumBytes;
      }

      // Attempt to read ahead but only necessary if remaining is 0;
      ReadAhead();
 

    }
    else 
    {
        LARGE_INTEGER li;

        li.QuadPart = uNumBytes;

        li.LowPart = SetFilePointer (m_hFile, li.LowPart, &li.HighPart, FILE_CURRENT );
  

        // Only seek through file if not in buffer
        if (  li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
          throw ex::IOException( LogRec( util::Strings::Format( "Seek of (%I64d) bytes from current failed (Error: 0x%08x) ", uNumBytes, GetLastError() ), "LocalFilePtr", "SeekCurrent" ) );
    }

    
    /// Number of bytes seeked
    m_uFilePosition += uNumBytes;

    // Set the file EOF is we are at the en
    m_bEof = IsEof();


  }
  else
    LogWrn(util::Strings::Format("Cannot perform seek in %s, file is not open",m_sFilePath.c_str()), "LocalFilePtr","Seek")

  return  FilePtr::IsEof();

} // SeekCurrent()


/**
 * Returns true iff the read/write last attempted was outside the file. It does not necessarily return true
 * even if the file pointer currently points outside the file.
 *
 * @return true if end of file
 */
const bool LocalFilePtr::IsEof()
{

  // ==========================================
  // Can only check EOF if the file is open
  // ==========================================
  if ( IsOpen()  )
  {

    // ==================================================
    // If the object is not at EOF then check further
    // ==================================================
    if ( ! FilePtr::IsEof()  ) 
    {
      LogDbg(util::Strings::Format("EOF - %s",m_sFilePath.c_str()), "LocalFilePtr","IsEof");

      if ( ! m_bEof )
      {

        // Wait for any read ahead's
        m_task->Wait();

 
        char buffer[1];
        DWORD dwBytesRead(0);

        // Attempt to read one byte from the file. If an error occurs then throw exception
        if ( ! ::ReadFile(m_hFile, buffer, 1, &dwBytesRead, NULL )  )
          throw ex::IOException( LogRec( util::Strings::Format( "EOF failed (Error: 0x%08x) ", GetLastError() ), "LocalFilePtr", "IsEof" ) );


        // =============================================================================================
        // If no bytes weere read then we are at the EOF. Otherwise we need to reset the file pointer
        // =============================================================================================
        if ( dwBytesRead == 0 )
          m_bEof = true;
        else
        {
          if ( SetFilePointer(m_hFile, -1, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER )
            throw ex::IOException( LogRec( util::Strings::Format( "EOF failed (Error: 0x%08x) ", GetLastError() ), "LocalFilePtr", "IsEof" ) );
        }
      } // END ... If not at the file eof


 
    } // END ... If the object EOF is not set

  }
  else
    LogWrn(util::Strings::Format("Cannot check EOF in %s, file is not open",m_sFilePath.c_str()), "LocalFilePtr","IsEof")


  return  FilePtr::IsEof()  ;

} // IsEof()


/**
 * Determines if the file pointer currently points to the last character of the file. The feof() function only 
 * returned true if the pointer is beyond the last character.
 *
 * @return true if the file pointer currently points outside the file
 */	
const bool LocalFilePtr::IsEofSmart()
{
  
  // ==========================================
  // Can only check EOF if the file is open
  // ==========================================
  if ( IsOpen()  )
  {

    // ==================================================
    // If the object is not at EOF then check further
    // ==================================================
    if ( ! FilePtr::IsEof() ) 
    {

      LogDbg(util::Strings::Format("Smart-EOF - %s",m_sFilePath.c_str()), "LocalFilePtr","IsEofSmart");


      if ( ! m_bEof )
      {

        // Wait for any read ahead's
        m_task->Wait();


        char buffer[2];
        DWORD dwBytesRead(0);

        // Attempt to read 2 bytes from the file. If an error occurs then throw exception
        if ( ! ::ReadFile(m_hFile, buffer, 2, &dwBytesRead, NULL )  )
          throw ex::IOException( LogRec( util::Strings::Format( "EOF failed (Error: 0x%08x) ", GetLastError() ), "LocalFilePtr", "IsEof" ) );


        // =============================================================================================
        // If no bytes weere read then we are at the EOF. Otherwise we need to reset the file pointer
        // =============================================================================================
        if ( dwBytesRead <= 1)
          m_bEof = true;
        else
        {
          if ( SetFilePointer(m_hFile, -2, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER )
            throw ex::IOException( LogRec( util::Strings::Format( "EOF failed (Error: 0x%08x) ", GetLastError() ), "LocalFilePtr", "IsEof" ) );
        }
      } // END ... If not at the file eof

    } // END ... If not EOF 

  }
  else
    LogWrn(util::Strings::Format("Cannot check EOF in %s, file is not open",m_sFilePath.c_str()), "LocalFilePtr","IsEofSmart")

  return FilePtr::IsEof();
} // IsEofSmart()


/**
 * Gets the size of the file
 *
 * @return the size of the file
 */	
const uint4  LocalFilePtr::Size()
{

  LARGE_INTEGER iFileSize;

  // ===========================================
  // Can only get the files size if it is open
  // ===========================================
  if ( IsOpen() )
  {

    LogDbg(util::Strings::Format("Size - %s",m_sFilePath.c_str()), "LocalFilePtr", "Size");
 
     // Wait for any read ahead's
     m_task->Wait();

     // Get the files size
     if ( ! ::GetFileSizeEx(m_hFile, &iFileSize ) )
        throw ex::IOException( LogRec( util::Strings::Format( "Getting the file size failed (Error: 0x%08x) ", GetLastError() ), "LocalFilePtr", "IsEof" ) );

  }

  return (uint4)iFileSize.LowPart;


} // Size()


/**
 * Determines if the file exists 
 *
 * @param   sFilePath     File to be checked
 *
 * @return  True if the file exists, false if not
 */
const bool LocalFilePtr::Exists( const std::string & sFilePath)
{
  LogDbg(util::Strings::Format("Exists - %s", sFilePath.c_str()), "LocalFilePtr","Exists");

  int error;
  bool bExists ( ! sFilePath.empty() ? (( error = _access(sFilePath.c_str(), 0)) == 0 ) : false );

  if ( ! bExists )
   LogWrn(util::Strings::Format("File %s does not exist (%d)", sFilePath.c_str(), error), "LocalFilePtr","Exists");


  // Determine if the file exists
  return bExists; 
}


/**
 * Delets the file specified 
 *
 * @param   sFilePath     File to be checked
 *
 * @return  True if the file was deleted, false if not
 */
const bool LocalFilePtr::Delete( const std::string & sFilePath)
{

  LogDbg(util::Strings::Format("Delete - %s", sFilePath.c_str()), "LocalFilePtr","Delete");

  bool bDeleted ( ! sFilePath.empty() ?  ::DeleteFile(sFilePath.c_str()) : false );


  if ( ! bDeleted )
   LogWrn(util::Strings::Format("File %s was not deleted (%d)", sFilePath.c_str(), ::GetLastError()), "LocalFilePtr","Exists");

  // Determine if the file exists
  return bDeleted;
}

/**
 * Is it a UNC path
 * @return true if the path contains UNC path specifiers
 */
const bool LocalFilePtr::ContainsUNCPath() const
{
  const std::string sFirstTwo(m_sFilePath.substr(0,2));

  return ((sFirstTwo == "//") || (sFirstTwo == "\\\\"));
} // ContainsUNCPath


/**
 * Returns the UNC system name
 *
 */
const std::string LocalFilePtr::GetUNCSystemName() const
{
  std::string sUNCSystemName;

  if ( ContainsUNCPath() ) 
  {
    std::string::size_type idx(m_sFilePath.substr(2,m_sFilePath.size() - 2).find("\\"));
    if ( idx != std::string::npos )
    {
      sUNCSystemName = m_sFilePath.substr(2,idx);
    }
    else
    {
      idx = m_sFilePath.substr(2, m_sFilePath.size() - 2).find("/");
      if ( idx != std::string::npos )
      {
        sUNCSystemName = m_sFilePath.substr(2,idx);      
      }
    }
  }
  
  return toupper(sUNCSystemName);
} // GetUNCSystemName()


