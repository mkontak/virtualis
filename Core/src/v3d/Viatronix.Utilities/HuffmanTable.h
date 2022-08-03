// $Id: HuffmanTable.h,v 1.1 2004/03/02 20:02:52 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


// Pragmas
#pragma once

// Namespaces
OPEN_IMAGING_NS

/*
 * One of the following structures is created for each huffman coding
 * table.  We use the same structure for encoding and decoding, so there
 * may be some extra fields for encoding that aren't used in the decoding
 * and vice-versa.
 */
public __gc class HuffmanTable : public System::IDisposable
{
public:
  /// Constructor
  HuffmanTable() { Init(); }

  /// Destructor
  virtual ~HuffmanTable();

  ///
  void Fix(void);

  /// Extends the huffman data value
  __forceinline static void Extend(int & iData, int iDecodeValue) { if ( iData < m_iExtendTest[iDecodeValue] ) { iData += m_iExtendOffset[iDecodeValue]; } }                                         \

  ///
  void Dispose();

  /// Dumps the huffman table to the specified file
  void Dump(System::String * sFileName);

  /// Clear all work values (Except the BITS and HUFFMAN VALUES) 
  void Clear();

  /// Clear all values (Including the BITS and HUFFMAN VALUES)
  void ClearAll();

private:


  ///
  void Dispose(bool bDisposing);

  /// Initialize data memebers
  void Init();

  ///
  System::String * FormatString(System::String * sValue, int iSize );

public:

  /// Returns the indexed value
  __property int get_Value(int iIndex) { return m_iValue[iIndex]; }

  /// Sets the indexed value
  __property void set_Value(int iIndex, int iBits ) { m_iValue[iIndex] = iBits; }

  /// Returns the indexed number of bits
  __property int get_NumberOfBits(int iIndex) { return m_iNumberOfBits[iIndex]; }

  /// Sets the indexed number of bits
  __property void set_NumberOfBits(int iIndex, int iBits ) { m_iNumberOfBits[iIndex] = iBits; }

  /// Returns the indexed bits 
  __property unsigned char get_Bits(int iIndex) { return m_Bits[iIndex]; }

  /// Sets the indexed bits
  __property void set_Bits(int iIndex, unsigned char bit ) { m_Bits[iIndex] = bit; }

  /// Returns the indexed huffman value
  __property unsigned char get_HuffmanValue(int iIndex) { return m_HuffmanValue[iIndex]; }

  /// Sets the indexed huffman value
  __property void set_HuffmanValue(int iIndex, unsigned char huffmanValue ) { m_HuffmanValue[iIndex] = huffmanValue; }

  /// Returns the max code for the index specified
  __property int get_MaxCode(int iIndex) { return m_iMaxCode[iIndex]; }

  /// Sets the nax code for the index specified
  __property void set_MaxCode(int iIndex, int iMaxCode ) { m_iMaxCode[iIndex] = iMaxCode; }

  /// Reeturns the min code for the index specified
  __property int get_MinCode(int iIndex) { return m_iMinCode[iIndex]; }

  /// Sets the min code for the index specified
  __property void set_MinCode(int iIndex, int iMinCode ) { m_iMinCode[iIndex] = iMinCode; }

  /// Returns the value pointer for the index specified
  __property short get_ValuePtr(int iIndex) { return m_iValuePtr[iIndex]; }

  /// Sets the value pointer for the index specified
  __property void set_ValuePtr(int iIndex, short iValuePtr ) { m_iValuePtr[iIndex] = iValuePtr; }

  /// Returns the huffman code for the index specified
  __property unsigned short get_HuffmanCode(int iIndex) { return m_HuffmanValue[iIndex]; }

  /// Sets the huffman code for the index specified
  __property void set_HuffmanCode(int iIndex, unsigned short huffmanCode ) { m_HuffmanCode[iIndex] = huffmanCode; }

  /// Returns the length
  __property int get_Length(int iIndex);

private:


  /// Entry n is 2**(n-1) 
  static int m_iExtendTest __gc [] =  { 0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                                        0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000};

  /// Entry n is (-1 << n) + 1 
  static int m_iExtendOffset __gc [] = {  0, 
                                          ((-1) <<  1) + 1, ((-1) <<  2) + 1, 
                                          ((-1) <<  3) + 1, ((-1) <<  4) + 1,
                                          ((-1) <<  5) + 1, ((-1) <<  6) + 1, 
                                          ((-1) <<  7) + 1, ((-1) <<  8) + 1,
                                          ((-1) <<  9) + 1, ((-1) << 10) + 1, 
                                          ((-1) << 11) + 1, ((-1) << 12) + 1,
                                          ((-1) << 13) + 1, ((-1) << 14) + 1, 
                                          ((-1) << 15) + 1};


  /// Bits array read from The DHT segment
  unsigned char m_Bits __gc [];

  /// Huffman value array read from the DHT segment
  unsigned char m_HuffmanValue __gc [];

  /// Huffman code array read from the DHT segment
  unsigned short m_HuffmanCode __gc [];

  // This field is used only during compression.  It's initialized
  // ALSE when the table is created, and set TRUE when it's been
  // output to the file.
  bool m_bSentTable;

  // The remaining fields are computed from the above to allow more
  // efficient coding and decoding.  These fields should be considered
  // private to the Huffman compression & decompression modules.
  unsigned char m_Ehufco __gc [];

  ///
  char m_Ehufsi __gc [];

  ///
  int m_iMinCode __gc [];

  ///
  int m_iMaxCode __gc [];

  ///
  short m_iValuePtr __gc [];

  ///
  int m_iNumberOfBits __gc [];

  ///
  int m_iValue __gc[];


  ///
  static unsigned int m_iBitMask __gc [] = {0xffffffff, 0x7fffffff, 0x3fffffff, 0x1fffffff,
                                            0x0fffffff, 0x07ffffff, 0x03ffffff, 0x01ffffff,
                                            0x00ffffff, 0x007fffff, 0x003fffff, 0x001fffff,
                                            0x000fffff, 0x0007ffff, 0x0003ffff, 0x0001ffff,
                                            0x0000ffff, 0x00007fff, 0x00003fff, 0x00001fff,
                                            0x00000fff, 0x000007ff, 0x000003ff, 0x000001ff,
                                            0x000000ff, 0x0000007f, 0x0000003f, 0x0000001f,
                                            0x0000000f, 0x00000007, 0x00000003, 0x00000001  };

  /// Dispoed flag
  bool m_bDisposed;

}; /// class HuffmanTable

CLOSE_IMAGING_NS


// $Log: HuffmanTable.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/HuffmanTable.h,v 1.1 2004/03/02 20:02:52 geconomos Exp $
// $Id: HuffmanTable.h,v 1.1 2004/03/02 20:02:52 geconomos Exp $

