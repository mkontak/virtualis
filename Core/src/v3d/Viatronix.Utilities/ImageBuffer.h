

#pragma once

// Buffer Type for source image so that I do not need to reallocate
public __value enum BufferType { MANAGED = 0, UNMANAGED = 1 };

private __delegate unsigned char GetCharDelegate(int iIndex);
private __delegate void PutCharDelegate(int iIndex, unsigned char cByte);

OPEN_IMAGING_NS

public __gc class ImageBuffer : public System::IDisposable
{
public:

  /// Constructor
  ImageBuffer();

  /// Constructor
  ImageBuffer(void * pBuffer, int iBufferSize);

  /// Constructor
  ImageBuffer(unsigned char Buffer __gc [] , int iBufferSize);

  ///
  void Dispose(void);

  /// Reads a block of data
  void Read(unsigned char Buffer __gc [], int iOffset, int iBytes);

  /// Returns a character from the image
  unsigned char GetChar(void);

  /// Ungets a character (Does'nt actually put the character back)
  __forceinline void UnGetChar(unsigned char cByte) { --m_iPosition; }

  /// Returns a word value from the image
  unsigned short GetWord(void);

  /// Returns a character from the source inage buffer
  unsigned char Peek(int iOffset);

  /// Skips the specified amount of bytes
  void Skip(int iBytes);

  /// Reads a row of pixel data
  int ReadRow( unsigned short * pBuffer, unsigned int iNumberOfPixels, unsigned short iBitsPerPixel );

  /// Put character to thebuffer
  __forceinline void Put(unsigned char cByte) { Char[m_iPosition++] = cByte; }

  /// Put a word to the buffer
  //__forceinline void Put(unsigned short iWord) { Put((unsigned char)(iWord >> 8)); Put((unsigned char)(iWord & 0xff)); }

  /// Put a word to the buffer
  __forceinline void Put(unsigned short iWord) { Put((unsigned char)(iWord & 0xff)); Put((unsigned char)(iWord >> 8));  }

  /// Rewinds the source
  __forceinline void Rewind() { m_iPosition = 0; }

  /// Allocates memory
  void New(BufferType eType, int iSize);

  /// Releases any allocated memory
  void Delete();

private:

  ///
  void Dispose(bool bDisposing);

  /// Initialize data members
  void Init(void);

  /// Returns the chacacter ate the specified index from the unmanaged buffer
  __forceinline unsigned char GetCharUnmanaged(int iIndex) { return m_pUnmanagedBuffer[iIndex]; }

  /// Returns the chacacter ate the specified index from the managed buffer
  __forceinline unsigned char GetCharManaged(int iIndex) { return m_pManagedBuffer[iIndex]; }

  /// Sets the chacacter ate the specified index from the unmanaged buffer
  __forceinline void PutCharUnmanaged(int iIndex, unsigned char cByte) { m_pUnmanagedBuffer[iIndex] = cByte; }

  /// Sets the chacacter ate the specified index from the managed buffer
  __forceinline void PutCharManaged(int iIndex, unsigned char cByte) { m_pManagedBuffer[iIndex] = cByte; }

public:

  /// Returns the buffer length
  __property int get_Length() { return m_iLength; }

  /// Returns the poistion in the buffer
  __property int get_Position() { return m_iPosition; }

  ///
  __property unsigned char get_Char(int iIndex) { return m_pGetChar[(int)m_eType]->Invoke(iIndex); }

  ///
  __property void set_Char(int iIndex, unsigned char cByte) { m_pPutChar[(int)m_eType]->Invoke(iIndex, cByte); }

  /// Returns true if we are at the end of the SOURCE stream
  __property bool get_EndOfStream() { return (m_iPosition >= m_iLength); }


private:

  /// Get Char delegates used to get data from the proper buffer
  GetCharDelegate * m_pGetChar __gc [];

  /// Get Char delegates used to put data from the proper buffer
  PutCharDelegate * m_pPutChar __gc [];


  ///
  unsigned char m_pManagedBuffer __gc [];

  ///
  unsigned char * m_pUnmanagedBuffer;

  ///
  int m_iLength;

  BufferType m_eType;

  ///
  int m_iPosition;

  ///
  bool m_bAllocated;

  ///
  bool m_bDisposed;

};


CLOSE_IMAGING_NS