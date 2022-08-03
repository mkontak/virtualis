#include "stdafx.h"
#include "FilePtr.h"
#include "vxTask.h"


using namespace io;


/**
 * Constructor
 */
FilePtr::FilePtr(const std::string & sFilePath, const uint4 uCacheSize) :
m_sFilePath(sFilePath),
m_sFilename(util::IO::GetFilename(sFilePath)),
m_task(std::make_shared<Task>()),
m_uTotalBytesRead(0),
m_uFilePosition(0),
m_uTotalBytesWritten(0),
m_bEof(false),
m_uCacheSize(uCacheSize),
m_uCacheBytesRead(0),
m_cache(__nullptr),
m_uCacheBytesRemaining(0)
{ 

  if ( m_uCacheSize > 0 )
  {

    LogDbg(util::Strings::Format("Allocating  file cache of %d byte(s)", m_uCacheSize), "FilePtr","FilePtr");

    m_cache = std::shared_ptr<char>( new  char[m_uCacheSize],  [] ( char * value) {  delete [] value; } ); 

    if ( m_cache == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate internal cache buffer","FilePtr","FilePtr"));
  }

}

/**
 * Destructor
 */
FilePtr:: ~FilePtr()
{ 

  m_cache = __nullptr;
  m_task = __nullptr;
}


/**
 * Loads the cache
 *
 * @param bRefresh      Refresh flag use to clear and reload
 */
void FilePtr::ReadAhead(bool bRefresh)
{

  // ==========================================
  // If no caching is enabled then so nothing
  // ===========================================
  if ( m_bEof || ! IsCachingEnabled() )
    return;

  // =======================
  // Refreshes the cache
  // =======================
  if ( bRefresh ) 
    ClearCache();


  // ==========================================================================================
  // Only need to read ahead if there is no more data in the cache
  // ==========================================================================================
  if ( m_uCacheBytesRemaining == 0 )
  {

    // ===========================
    // Run the read-ahead task
    // ==========================
    m_task->Run( Concurrency::task<void>( [this] () 
    {
       

      try
      {


        /// Read the buffer size into the cache
        const uint4 uBytesRead(ReadFile(this->m_cache.get(), this->m_uCacheSize));


        this->m_uCacheBytesRead = 0;
        this->m_uCacheBytesRemaining = uBytesRead;

        // Debug
        LogDbg(util::Strings::Format("Filling cache ( %d, %d, %d )", this->m_uCacheSize, this->m_uCacheBytesRead, this->m_uCacheBytesRemaining), "FilePtr", "ReadAhead");
      }
      catch (ex::VException & ex)
      {
        throw std::exception(util::Strings::Format("Failed to read ahead %s : %s", m_sFilePath.c_str(), ex.GetLogRecord().GetTextMessage().c_str()).c_str());
      }
      catch (...)
      {
        throw std::exception(util::Strings::Format("Failed to read ahead %s [%d]", m_sFilePath.c_str(), GetLastError()).c_str());
      }

    
    }  ) ); // END ... Start task

  }
 
} // ReadAhead()

/**
 * Reads from the file buffer
 *
 * @param pBuffer         Buffer to load into
 * @param uBytesToRead    Byte to read
 *
 * @return Bytes read
 */
uint4 FilePtr::ReadCache(void * pBuffer, const uint4 uBytesToRead)
{



  uint4 uBytesRead(0);

  // ===========================================================
  // Can only read from the cache if there are bytes remaining
  // ===========================================================
  if ( m_uCacheSize > 0 && m_uCacheBytesRemaining > 0 )
  {
#ifdef _DEBUG
    LogDbg(util::Strings::Format("Cache ( %d, %d, %d )", m_uCacheSize, m_uCacheBytesRead, m_uCacheBytesRemaining ),"FilePtr","ReadCache");
#endif


    // Calculate the bytes that can be read.
    uBytesRead = ( m_uCacheBytesRemaining >= uBytesToRead ? uBytesToRead : m_uCacheBytesRemaining );

#ifdef _DEBUG
    LogDbg(util::Strings::Format("Attempting to read %d byte(s) from the cache", uBytesRead ),"FilePtr","ReadCache");
#endif

    // Copy the memory
    memcpy(pBuffer, (m_cache.get() + m_uCacheBytesRead), uBytesRead);


    // Make pointer adjustments
    m_uCacheBytesRead  += uBytesRead;
    m_uCacheBytesRemaining -= uBytesRead;

  }

  // returns the bytes read
  return uBytesRead;

} // Read(void * pBuffer, const uint4 uBytesToRead)


