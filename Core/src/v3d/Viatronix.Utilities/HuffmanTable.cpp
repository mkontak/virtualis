// $Id: HuffmanTable.cpp,v 1.4.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]



// Includes
#include "stdafx.h"
#include "HuffmanTable.h"


// Namespaces
using namespace System;
using namespace System::IO;
using namespace Viatronix::Logging;

USING_IMAGING_NS



/** 
 * Destructor
 */
HuffmanTable::~HuffmanTable()
{
  Dispose(false);
} // ~HuffmanTable

/**
 * Dispose
 */
void HuffmanTable::Dispose()
{
  Dispose(true);
  System::GC::SuppressFinalize(this);
} // Dispose()

/**
 * Disposes of the object and cleans up any resources.
 *
 * @param bDisposing      Flag to indicate if the object is being disposed of by the GC or not.
 *
 * @return void.
 */
void HuffmanTable::Dispose(bool bDisposing)
{


  /////////////////////////////////////////////////
  // Make sure the object is not already disposed
  ////////////////////////////////////////////////
  if ( ! m_bDisposed )
  {

    Log::Debug("Disposing HuffmanTable", "HuffmaneTable", "Dispose");

    ///////////////////////////////////////
    // Are we being disposed by the GC
    //////////////////////////////////////
    if ( bDisposing )
    {

    } // END ... If we are being disposed of

    delete [] m_Bits;
    delete [] m_HuffmanValue;
    delete [] m_Ehufco;
    delete [] m_Ehufsi;
    delete [] m_iMinCode;
    delete [] m_iMaxCode;
    delete [] m_iValuePtr;
    delete [] m_iNumberOfBits;
    delete [] m_iValue;
    delete [] m_HuffmanCode;


  } // END ... If the object is not already disposed of

  m_bDisposed = true;

} // Dispose()


/**
 * Initialize data members
 */
void HuffmanTable::Init()
{

  m_bDisposed = false;
  m_bSentTable = false;

  m_Bits = new unsigned char __gc [33];
  m_HuffmanValue = new unsigned char __gc [256];
  m_Ehufco = new unsigned char __gc [256];
  m_Ehufsi = new char __gc [256];
  m_iMinCode = new int __gc [17];
  m_iMaxCode = new int __gc [18];
  m_iValuePtr = new short __gc [17];
  m_iNumberOfBits = new int __gc [256];
  m_iValue = new int __gc[256];
  m_HuffmanCode = new unsigned short __gc [257];

  ClearAll();

  //for ( register i(0); i < 256; i++ )
  //{
  //  if ( i < 33 )
  //  {
  //    m_Bits[i] = 0;
  //  }

  //  m_HuffmanValue[i] = 0;
  //}

  //Clear();

} // Init();


/** 
 * Clears all the values
 */
void HuffmanTable::ClearAll()
{

  for ( int i(0); i < 256; i++ )
  {
    if ( i < 33 )
    {
      m_Bits[i] = 0;
    }

    m_HuffmanValue[i] = 0;
  }

  Clear();

} // ClearAll()

/**
 * Clears all values ( Except the BITS and HUFFMAN VALUES)
 */
void HuffmanTable::Clear()
{
  for ( int i(0); i < 256; i++ )
  {
    if ( i < 17 )
    {
      m_iMinCode[i] = 0;
      m_iMaxCode[i] = 0;
      m_iValuePtr[i] = 0;
    }
    
    //if ( i < 33 )
    //{
    //  m_Bits[i] = 0;
    //}

    m_Ehufco[i] = 0;
    m_Ehufsi[i] = 0;
    m_iNumberOfBits[i] = 0;
    m_iValue[i] = 0;
    m_HuffmanCode[i] = 0;
  }

} // Clear()

/**
 * Dump the huffman table to the file
 */
void HuffmanTable::Dump(String * sFileName )
{

  FileStream * pFS = new FileStream(sFileName, FileMode::Create, FileAccess::Write, FileShare::None);
  StreamWriter * pWriter = new StreamWriter(pFS);

  String * sLine = L"         BIT    HUFF    HUFF                      VALUE   MIN     MAX     # OF         ";
  pWriter->WriteLine(sLine);

  sLine = L"INDEX   COUNT   VALUE   CODE    EHUFCO   EHUFSI    PTR    CODE    CODE    BITS    VALUE";
  pWriter->WriteLine(sLine);

  sLine = L"-----   -----   -----   -----   ------   ------   -----   -----   -----   -----   -----";
  pWriter->WriteLine(sLine);

  String * sFields[] = new String * [11];

  for ( int i(0); i < 256; i++ )
  {

    sFields[0]  = FormatString(Convert::ToString(i),5);
    sFields[1]  = ( i < 16 ? FormatString(Convert::ToString((int)m_Bits[i]), 5) : new String("     "));
    sFields[2]  = FormatString(Convert::ToString((int)m_HuffmanValue[i]), 5);
    sFields[3]  = FormatString(Convert::ToString((int)m_HuffmanCode[i]), 5);
    sFields[4]  = FormatString(Convert::ToString((int)m_Ehufco[i]), 6);
    sFields[5]  = FormatString(Convert::ToString((int)m_Ehufsi[i]), 6);
    sFields[6]  = ( i < 16 ? FormatString(Convert::ToString((int)m_iValuePtr[i]), 5) : new String("     "));
    sFields[7]  = ( i < 16 ? FormatString(Convert::ToString((int)m_iMinCode[i]), 5) : new String("     "));
    sFields[8]  = ( i < 17 ? FormatString(Convert::ToString((int)m_iMaxCode[i]), 5) : new String("     "));
    sFields[9]  = FormatString(Convert::ToString((int)m_iNumberOfBits[i]), 5);
    sFields[10] = FormatString(Convert::ToString((int)m_iValue[i]), 5);
    sLine = String::Format("{0}   {1}   {2}   {3}   {4}   {5}   {6}   {7}   {8}   {9}   {10}", sFields);

    pWriter->WriteLine(sLine);

  }


  pWriter->Close();

} // Dump()

String * HuffmanTable::FormatString(String * sValue, int iSize )
{
  String * sResult = sValue;
  String * sSpaces = L"          ";
  int iDiff = iSize - sValue->Length;

  if ( iDiff > 0 )
  {
    sResult = String::Concat(sSpaces->Substring(0,iDiff),sValue); 
  }

  return sResult;
} // FormatString

/**
 * 
 */
void HuffmanTable::Fix()
{


  char HuffmanSize __gc [] = new char __gc [257];
  int value, ll, ul;

  Clear();

  int p(0);

  ///////////////////////////////////////////////////////////////////////
  // Figure C.1: make table of Huffman code length for each symbol
  // Note that this is in code-length order.
  /////////////////////////////////////////////////////////////////////
  for ( int i(1); i <= 16; i++) 
  {
    for ( int j(1); j <= (int)m_Bits[i]; j++ )
    {
      HuffmanSize[p++] = (char)i;
    }
  }

  HuffmanSize[p] = 0;
  int iLastp(p);


  /////////////////////////////////////////////////
  // Figure C.2: generate the codes themselves
  // Note that this is in code-length order.
  ////////////////////////////////////////////////
  unsigned short iCode(0);

  for ( int iSize = HuffmanSize[0], p = 0; HuffmanSize[p] > 0; iSize++ ) 
  {
    while ( ((int)HuffmanSize[p]) == iSize ) 
    {
      m_HuffmanCode[p++] = iCode;
      iCode++;
    }
    iCode <<= 1;
  } // END ...

  //////////////////////////////////////////////////////////////////////
  // Figure C.3: generate encoding tables
  // These are code and size indexed by symbol value
  // Set any codeless symbols to have code length 0; this allows
  // EmitBits to detect any attempt to emit such symbols.
  /////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < iLastp; i++) 
  {
    m_Ehufco[m_HuffmanValue[i]] = (unsigned char)m_HuffmanCode[i];
    m_Ehufsi[m_HuffmanValue[i]] = HuffmanSize[i];
  }

  /////////////////////////////////////////////
  // Figure F.15: generate decoding tables
  /////////////////////////////////////////////
  for ( int j = 1, k = 0; j <= 16; j++) 
  {
    if ( m_Bits[j] > 0 ) 
    {
      m_iValuePtr[j]  = k;
      m_iMinCode[j]   = m_HuffmanCode[k];
      k += m_Bits[j];
      m_iMaxCode[j]   = m_HuffmanCode[k - 1];
    } 
    else 
    {
      m_iMaxCode[j] = -1;
    }
  }

  ////////////////////////////////////////////////////////
  // We put in this value to ensure HuffDecode terminates.
  ////////////////////////////////////////////////////////
  m_iMaxCode[17] = 0xFFFFFL;

  ///////////////////////////////////////////////////////////////////////////////
  // Build the numbits, value lookup tables.
  // These table allow us to gather 8 bits from the bits stream,
  // and immediately lookup the size and value of the huffman codes.
  // If size is zero, it means that more than 8 bits are in the huffman
  // code (this happens about 3-4% of the time).
  ////////////////////////////////////////////////////////////////////////////
  for ( int p(0); p < iLastp; p++ ) 
  {
    int iSize = HuffmanSize[p];
    if ( iSize <= 8 ) 
    {
      value = m_HuffmanValue[p];
      iCode = m_HuffmanCode[p];
      ll = iCode << (8 - iSize);
      if ( iSize < 8 ) 
      {
        ul = ll | m_iBitMask[24 + iSize];
      } 
      else 
      {
        ul = ll;
      }

      for ( int i(ll); i <= ul; i++ ) 
      {
        m_iNumberOfBits[i] = iSize;
        m_iValue[i] = value;
      }
    }
  }


}


/** 
 * Returns the length for the table index specified
 *
 * @param iIndex  
 *
 */
int HuffmanTable::get_Length(int iIndex)
{

  int iLength = 16;


  int iCount = 0;
  for ( int i(1); i <= 16; i++ ) 
  {
    iCount += (int)m_Bits[i];
  }

  if ( iCount > 256 ) 
  {
    throw new IOException("Invalid Huffman Table Counts");
  }

  for ( int i(0); i < iCount; i++ )
  {
    iLength += 1 + 16 + iCount;
  }

  return iLength;

} // get_Length();

// $Log: HuffmanTable.cpp,v $
// Revision 1.4.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.3  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.2  2005/07/01 12:39:08  vxconfig
// Added new Logger.
//
// Revision 1.1  2004/03/02 20:02:52  geconomos
// Merged Viatronix.v3D.Logger, Viatronix.v3D.Imaging, Viatronix.v3D.Threading and Viatronix.v3D.Licensing into Viatronix.Utilities.
//
// Revision 1.1.1.1  2004/03/02 03:58:37  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/11/14 20:04:31  mkontak
// Fixes
//
// Revision 1.2  2003/07/01 20:22:16  mkontak
// Fix for 16 bot decompression
//
// Revision 1.1  2003/06/26 20:27:22  mkontak
// Initial check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/HuffmanTable.cpp,v 1.4.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: HuffmanTable.cpp,v 1.4.2.1 2011/05/18 01:56:58 mkontak Exp $


