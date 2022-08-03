// $Id: Jpeg.cpp,v 1.9.2.1 2011/05/18 01:56:58 mkontak Exp $
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
#include "Jpeg.h"
#include "HuffmanTable.h"
#include "ImageBuffer.h"


// Namespaces
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace Viatronix::Logging;

USING_IMAGING_NS


/**
 * Constructor
 *
 * @param pSource           Image to be compressed or decompressed
 * @param iDepth            Depth of image pixels in bits (8, 16, 24)
 */
Jpeg::Jpeg(Viatronix::Imaging::ImageBuffer * pSource, short iDepth)
{
  Init();

  m_pSource = pSource;

  Depth = iDepth;

} // Jpeg()

/**
 * Constructor
 *
 * @param pSource           Image to be compressed or decompressed
 */
Jpeg::Jpeg(Viatronix::Imaging::ImageBuffer * pSource)
{

  Init();

  m_pSource = pSource;


} // Jpeg()



/**
 * Destructor
 */
Jpeg::~Jpeg()
{
  Dispose(false);
} // ~Jpeg()

/**
 * Dispose
 */
void Jpeg::Dispose()
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
void Jpeg::Dispose(bool bDisposing)
{


  /////////////////////////////////////////////////
  // Make sure the object is not already disposed
  ////////////////////////////////////////////////
  if ( ! m_bDisposed )
  {

    Log::Debug("Disposing Jpeg", "Jpeg", "Dispose");

    ///////////////////////////////////////
    // Are we being disposed by the GC
    //////////////////////////////////////
    if ( bDisposing )
    {

    } // END ... If we are being disposed of
   
    delete [] m_HuffmanTable;
    delete [] m_iThreshold;
    delete [] m_pMcuRow1;
    delete [] m_pMcuRow2;
    delete [] m_pPutRow;
    delete m_pBuffer1;
    //delete [] m_pHuffmanNodes;

  } // END ... If the object is not already disposed of

  m_bDisposed = true;

} // Dispose()

/**
 * Initialize the data members to their default values
 */
void Jpeg::Init()
{
  
  m_pRoot = NULL;

  m_pPutRow = new PutRowDelegate * [3];
  m_pPutRow[0] = new PutRowDelegate(this, &Jpeg::PutRow8);
  m_pPutRow[1] = new PutRowDelegate(this, &Jpeg::PutRow16);
  m_pPutRow[2] = new PutRowDelegate(this, &Jpeg::PutRow24);
  m_pBuffer1   = new char __nogc [m_iBufferSize];
  m_pBuffer2   = new char __nogc [m_iBufferSize];

  m_pMcuRow1 = new MCU __nogc [1024];
  m_pMcuRow2 = new MCU __nogc [1024];

  m_iMcuMembership = new short __gc [10];

  m_HuffmanTable = new HuffmanTable * __gc [4];
  m_HuffmanTable[0] = (HuffmanTable *)0;
  m_HuffmanTable[1] = (HuffmanTable *)0;
  m_HuffmanTable[2] = (HuffmanTable *)0;
  m_HuffmanTable[3] = (HuffmanTable *)0;



  InitializeQuantizationTables();

  m_iColumns = 0;
  m_iRows = 0;
  
  m_iRestartInterval = 0;
  m_iNumberOfComponents = 0;
  m_iComponentsInScan = 0;

  m_iThreshold = new unsigned short  __gc [3];
  m_iThreshold[0] = 0;
  m_iThreshold[1] = 0;
  m_iThreshold[2] = 0;

  m_iRESET = 0;
  m_iMAXVAL = 0;
  m_iNEAR = 1;
  m_iPointTransformParameter = 0;
  m_iDepthIndex = 0;
  m_iDepth = 0;
  m_iPrecision = 0;

} // Init()


/**
 * Initialize Quantization Tables
 */
void Jpeg::InitializeQuantizationTables()
{

  m_DequantizationTable[0][0] = 8;
  m_DequantizationTable[0][1] = 72;
  m_DequantizationTable[0][2] = 72;
  m_DequantizationTable[0][3] = 72;
  m_DequantizationTable[0][4] = 72;
  m_DequantizationTable[0][5] = 72;
  m_DequantizationTable[0][6] = 72;
  m_DequantizationTable[0][7] = 72;
  m_DequantizationTable[0][8] = 72;
  m_DequantizationTable[0][9] = 72;
  m_DequantizationTable[0][10] = 78;
  m_DequantizationTable[0][11] = 74;
  m_DequantizationTable[0][12] = 76;
  m_DequantizationTable[0][13] = 74;
  m_DequantizationTable[0][14] = 78;
  m_DequantizationTable[0][15] = 89;
  m_DequantizationTable[0][16] = 81;
  m_DequantizationTable[0][17] = 84;
  m_DequantizationTable[0][18] = 84;
  m_DequantizationTable[0][19] = 81;
  m_DequantizationTable[0][20] = 89;
  m_DequantizationTable[0][21] = 106;
  m_DequantizationTable[0][22] = 93;
  m_DequantizationTable[0][23] = 94;
  m_DequantizationTable[0][24] = 99;
  m_DequantizationTable[0][25] = 94;
  m_DequantizationTable[0][26] = 93;
  m_DequantizationTable[0][27] = 106;
  m_DequantizationTable[0][28] = 129;
  m_DequantizationTable[0][29] = 111;
  m_DequantizationTable[0][30] = 108;
  m_DequantizationTable[0][31] = 116;
  m_DequantizationTable[0][32] = 116;
  m_DequantizationTable[0][33] = 108;
  m_DequantizationTable[0][34] = 111;
  m_DequantizationTable[0][35] = 129;
  m_DequantizationTable[0][36] = 135;
  m_DequantizationTable[0][37] = 128;
  m_DequantizationTable[0][38] = 136;
  m_DequantizationTable[0][39] = 145;
  m_DequantizationTable[0][40] = 136;
  m_DequantizationTable[0][41] = 128;
  m_DequantizationTable[0][42] = 135;
  m_DequantizationTable[0][43] = 155;
  m_DequantizationTable[0][44] = 160;
  m_DequantizationTable[0][45] = 177;
  m_DequantizationTable[0][46] = 177;
  m_DequantizationTable[0][47] = 160;
  m_DequantizationTable[0][48] = 155;
  m_DequantizationTable[0][49] = 193;
  m_DequantizationTable[0][50] = 213;
  m_DequantizationTable[0][51] = 228;
  m_DequantizationTable[0][52] = 213;
  m_DequantizationTable[0][53] = 193;
  m_DequantizationTable[0][54] = 255;
  m_DequantizationTable[0][55] = 255;
  m_DequantizationTable[0][56] = 255;
  m_DequantizationTable[0][57] = 255;
  m_DequantizationTable[0][58] = 255;
  m_DequantizationTable[0][59] = 255;
  m_DequantizationTable[0][60] = 255;
  m_DequantizationTable[0][61] = 255;
  m_DequantizationTable[0][62] = 255;
  m_DequantizationTable[0][63] = 255;

  m_DequantizationTable[1][0] = 8;
  m_DequantizationTable[1][1] = 36;
  m_DequantizationTable[1][2] = 36;
  m_DequantizationTable[1][3] = 36;
  m_DequantizationTable[1][4] = 36;
  m_DequantizationTable[1][5] = 36;
  m_DequantizationTable[1][6] = 36;
  m_DequantizationTable[1][7] = 36;
  m_DequantizationTable[1][8] = 36;
  m_DequantizationTable[1][9] = 36;
  m_DequantizationTable[1][10] = 39;
  m_DequantizationTable[1][11] = 37;
  m_DequantizationTable[1][12] = 38;
  m_DequantizationTable[1][13] = 37;
  m_DequantizationTable[1][14] = 39;
  m_DequantizationTable[1][15] = 45;
  m_DequantizationTable[1][16] = 41;
  m_DequantizationTable[1][17] = 42;
  m_DequantizationTable[1][18] = 42;
  m_DequantizationTable[1][19] = 41;
  m_DequantizationTable[1][20] = 45;
  m_DequantizationTable[1][21] = 53;
  m_DequantizationTable[1][22] = 47;
  m_DequantizationTable[1][23] = 47;
  m_DequantizationTable[1][24] = 50;
  m_DequantizationTable[1][25] = 47;
  m_DequantizationTable[1][26] = 47;
  m_DequantizationTable[1][27] = 53;
  m_DequantizationTable[1][28] = 65;
  m_DequantizationTable[1][29] = 56;
  m_DequantizationTable[1][30] = 54;
  m_DequantizationTable[1][31] = 59;
  m_DequantizationTable[1][32] = 59;
  m_DequantizationTable[1][33] = 54;
  m_DequantizationTable[1][34] = 56;
  m_DequantizationTable[1][35] = 65;
  m_DequantizationTable[1][36] = 68;
  m_DequantizationTable[1][37] = 64;
  m_DequantizationTable[1][38] = 69;
  m_DequantizationTable[1][39] = 73;
  m_DequantizationTable[1][40] = 69;
  m_DequantizationTable[1][41] = 64;
  m_DequantizationTable[1][42] = 68;
  m_DequantizationTable[1][43] = 78;
  m_DequantizationTable[1][44] = 81;
  m_DequantizationTable[1][45] = 89;
  m_DequantizationTable[1][46] = 89;
  m_DequantizationTable[1][47] = 81;
  m_DequantizationTable[1][48] = 78;
  m_DequantizationTable[1][49] = 98;
  m_DequantizationTable[1][50] = 108;
  m_DequantizationTable[1][51] = 115;
  m_DequantizationTable[1][52] = 108;
  m_DequantizationTable[1][53] = 98;
  m_DequantizationTable[1][54] = 130;
  m_DequantizationTable[1][55] = 144;
  m_DequantizationTable[1][56] = 144;
  m_DequantizationTable[1][57] = 130;
  m_DequantizationTable[1][58] = 178;
  m_DequantizationTable[1][59] = 190;
  m_DequantizationTable[1][60] = 178;
  m_DequantizationTable[1][61] = 243;
  m_DequantizationTable[1][62] = 243;
  m_DequantizationTable[1][63] = 255;

  m_DequantizationTable[2][0] = 8;
  m_DequantizationTable[2][1] = 10;
  m_DequantizationTable[2][2] = 10;
  m_DequantizationTable[2][3] = 10;
  m_DequantizationTable[2][4] = 10;
  m_DequantizationTable[2][5] = 10;
  m_DequantizationTable[2][6] = 10;
  m_DequantizationTable[2][7] = 10;
  m_DequantizationTable[2][8] = 10;
  m_DequantizationTable[2][9] = 10;
  m_DequantizationTable[2][10] = 11;
  m_DequantizationTable[2][11] = 10;
  m_DequantizationTable[2][12] = 11;
  m_DequantizationTable[2][13] = 10;
  m_DequantizationTable[2][14] = 11;
  m_DequantizationTable[2][15] = 13;
  m_DequantizationTable[2][16] = 11;
  m_DequantizationTable[2][17] = 12;
  m_DequantizationTable[2][18] = 12;
  m_DequantizationTable[2][19] = 11;
  m_DequantizationTable[2][20] = 13;
  m_DequantizationTable[2][21] = 15;
  m_DequantizationTable[2][22] = 13;
  m_DequantizationTable[2][23] = 13;
  m_DequantizationTable[2][24] = 14;
  m_DequantizationTable[2][25] = 13;
  m_DequantizationTable[2][26] = 13;
  m_DequantizationTable[2][27] = 15;
  m_DequantizationTable[2][28] = 18;
  m_DequantizationTable[2][29] = 16;
  m_DequantizationTable[2][30] = 15;
  m_DequantizationTable[2][30] = 16;
  m_DequantizationTable[2][32] = 16;
  m_DequantizationTable[2][33] = 15;
  m_DequantizationTable[2][34] = 16;
  m_DequantizationTable[2][35] = 18;
  m_DequantizationTable[2][36] = 19;
  m_DequantizationTable[2][37] = 18;
  m_DequantizationTable[2][38] = 19;
  m_DequantizationTable[2][39] = 21;
  m_DequantizationTable[2][40] = 19;
  m_DequantizationTable[2][41] = 18;
  m_DequantizationTable[2][42] = 19;
  m_DequantizationTable[2][43] = 22;
  m_DequantizationTable[2][44] = 23;
  m_DequantizationTable[2][45] = 25;
  m_DequantizationTable[2][46] = 25;
  m_DequantizationTable[2][47] = 23;
  m_DequantizationTable[2][48] = 22;
  m_DequantizationTable[2][49] = 27;
  m_DequantizationTable[2][50] = 30;
  m_DequantizationTable[2][51] = 32;
  m_DequantizationTable[2][52] = 30;
  m_DequantizationTable[2][53] = 27;
  m_DequantizationTable[2][54] = 36;
  m_DequantizationTable[2][55] = 40;
  m_DequantizationTable[2][56] = 40;
  m_DequantizationTable[2][57] = 36;
  m_DequantizationTable[2][58] = 50;
  m_DequantizationTable[2][59] = 53;
  m_DequantizationTable[2][60] = 50;
  m_DequantizationTable[2][61] = 68;
  m_DequantizationTable[2][62] = 68;
  m_DequantizationTable[2][63] = 91;

  m_DequantizationTable[3][0] = 8;
  m_DequantizationTable[3][1] = 7;
  m_DequantizationTable[3][2] = 7;
  m_DequantizationTable[3][3] = 7;
  m_DequantizationTable[3][4] = 7;
  m_DequantizationTable[3][5] = 7;
  m_DequantizationTable[3][6] = 7;
  m_DequantizationTable[3][7] = 7;
  m_DequantizationTable[3][8] = 7;
  m_DequantizationTable[3][9] = 7;
  m_DequantizationTable[3][10] = 8;
  m_DequantizationTable[3][11] = 7;
  m_DequantizationTable[3][12] = 8;
  m_DequantizationTable[3][13] = 7;
  m_DequantizationTable[3][14] = 8;
  m_DequantizationTable[3][15] = 9;
  m_DequantizationTable[3][16] = 8;
  m_DequantizationTable[3][17] = 8;
  m_DequantizationTable[3][18] = 8;
  m_DequantizationTable[3][19] = 8;
  m_DequantizationTable[3][20] = 9;
  m_DequantizationTable[3][21] = 11;
  m_DequantizationTable[3][22] = 9;
  m_DequantizationTable[3][23] = 9;
  m_DequantizationTable[3][24] = 10;
  m_DequantizationTable[3][25] = 9;
  m_DequantizationTable[3][26] = 9;
  m_DequantizationTable[3][27] = 11;
  m_DequantizationTable[3][28] = 13;
  m_DequantizationTable[3][29] = 11;
  m_DequantizationTable[3][30] = 11;
  m_DequantizationTable[3][31] = 12;
  m_DequantizationTable[3][32] = 12;
  m_DequantizationTable[3][33] = 11;
  m_DequantizationTable[3][34] = 11;
  m_DequantizationTable[3][35] = 13;
  m_DequantizationTable[3][36] = 14;
  m_DequantizationTable[3][37] = 13;
  m_DequantizationTable[3][38] = 14;
  m_DequantizationTable[3][39] = 15;
  m_DequantizationTable[3][40] = 14;
  m_DequantizationTable[3][41] = 13;
  m_DequantizationTable[3][42] = 14;
  m_DequantizationTable[3][43] = 16;
  m_DequantizationTable[3][44] = 16;
  m_DequantizationTable[3][45] = 18;
  m_DequantizationTable[3][46] = 18;
  m_DequantizationTable[3][47] = 16;
  m_DequantizationTable[3][48] = 16;
  m_DequantizationTable[3][49] = 20;
  m_DequantizationTable[3][50] = 22;
  m_DequantizationTable[3][51] = 23;
  m_DequantizationTable[3][52] = 22;
  m_DequantizationTable[3][53] = 20;
  m_DequantizationTable[3][54] = 26;
  m_DequantizationTable[3][55] = 29;
  m_DequantizationTable[3][56] = 29;
  m_DequantizationTable[3][57] = 26;
  m_DequantizationTable[3][58] = 36;
  m_DequantizationTable[3][59] = 38;
  m_DequantizationTable[3][60] = 36;
  m_DequantizationTable[3][61] = 49;
  m_DequantizationTable[3][62] = 49;
  m_DequantizationTable[3][63] = 65;


  m_DequantizationTable[4][0] = 4;
  m_DequantizationTable[4][1] = 4;
  m_DequantizationTable[4][2] = 4;
  m_DequantizationTable[4][3] = 4;
  m_DequantizationTable[4][4] = 4;
  m_DequantizationTable[4][5] = 4;
  m_DequantizationTable[4][6] = 4;
  m_DequantizationTable[4][7] = 4;
  m_DequantizationTable[4][8] = 4;
  m_DequantizationTable[4][9] = 4;
  m_DequantizationTable[4][10] = 4;
  m_DequantizationTable[4][11] = 4;
  m_DequantizationTable[4][12] = 4;
  m_DequantizationTable[4][13] = 4;
  m_DequantizationTable[4][14] = 4;
  m_DequantizationTable[4][15] = 5;
  m_DequantizationTable[4][16] = 5;
  m_DequantizationTable[4][17] = 5;
  m_DequantizationTable[4][18] = 5;
  m_DequantizationTable[4][19] = 5;
  m_DequantizationTable[4][20] = 5;
  m_DequantizationTable[4][21] = 6;
  m_DequantizationTable[4][22] = 5;
  m_DequantizationTable[4][23] = 5;
  m_DequantizationTable[4][24] = 6;
  m_DequantizationTable[4][25] = 5;
  m_DequantizationTable[4][26] = 5;
  m_DequantizationTable[4][27] = 6;
  m_DequantizationTable[4][28] = 7;
  m_DequantizationTable[4][29] = 6;
  m_DequantizationTable[4][30] = 6;
  m_DequantizationTable[4][31] = 6;
  m_DequantizationTable[4][32] = 6;
  m_DequantizationTable[4][33] = 6;
  m_DequantizationTable[4][34] = 6;
  m_DequantizationTable[4][35] = 7;
  m_DequantizationTable[4][36] = 8;
  m_DequantizationTable[4][37] = 7;
  m_DequantizationTable[4][38] = 8;
  m_DequantizationTable[4][39] = 8;
  m_DequantizationTable[4][40] = 8;
  m_DequantizationTable[4][41] = 7;
  m_DequantizationTable[4][42] = 8;
  m_DequantizationTable[4][43] = 9;
  m_DequantizationTable[4][44] = 9;
  m_DequantizationTable[4][45] = 10;
  m_DequantizationTable[4][46] = 10;
  m_DequantizationTable[4][47] = 9;
  m_DequantizationTable[4][48] = 9;
  m_DequantizationTable[4][49] = 11;
  m_DequantizationTable[4][50] = 12;
  m_DequantizationTable[4][51] = 13;
  m_DequantizationTable[4][52] = 12;
  m_DequantizationTable[4][53] = 11;
  m_DequantizationTable[4][54] = 14;
  m_DequantizationTable[4][55] = 16;
  m_DequantizationTable[4][56] = 16;
  m_DequantizationTable[4][57] = 14;
  m_DequantizationTable[4][58] = 20;
  m_DequantizationTable[4][59] = 21;
  m_DequantizationTable[4][60] = 20;
  m_DequantizationTable[4][61] = 27;
  m_DequantizationTable[4][62] = 27;
  m_DequantizationTable[4][63] = 36;


} // InitializationQuantizationTables()









/**
 * Reads the JPEG marker
 */
bool Jpeg::ReadMarker(unsigned short & iMarker)
{
  iMarker = m_pSource->GetWord();

	return (( iMarker & 0xff80 ) != 0);
}


/**
 * Reads the SOF header
 */
void Jpeg::ReadSOF()
{


  // Read segment length
  unsigned short iLength = m_pSource->GetWord();

  // Read precision
  m_iPrecision = (unsigned short)GetChar();
  Depth = m_iPrecision;

  ///////////////////////////
  // Validate the precision
  ////////////////////////////
  if ( m_iPrecision < m_iMinPrecisionBits || m_iPrecision > m_iMaxPrecisionBits )
  {
    throw new IOException(String::Format("Unsupported precision [{0}]",__box(m_iPrecision)));
  }

  //////////////////////
  // Set the MAX VAL
  /////////////////////
  if ( m_iMAXVAL == 0 )
  {
    m_iMAXVAL = (1ul << m_iPrecision ) - 1;
  }

  // Reads Rows
  m_iRows = (int)m_pSource->GetWord();

  // Read Colimns
  m_iColumns = (int)m_pSource->GetWord();

  // Read the number of pixel components
  m_iNumberOfComponents = (int)GetChar();

  //////////////////////////////////////////////////////////////////////
  // We don't support files in which the image height is initially
  // specified as 0 and is later redefined by DNL.  As long as we
  // have to check that, might as well have a general sanity check.
  /////////////////////////////////////////////////////////////////////
  if ( m_iRows <= 0 && m_iColumns <= 0 && m_iNumberOfComponents  > 0 )
  {
    throw new IOException("Empty JPEG image (DNL not supported)");
  }

  if ( iLength != (m_iNumberOfComponents * 3 + 8 ) ) 
  {
    throw new IOException("Invalid SOF length");
  }
  for ( int i(0); i < m_iNumberOfComponents; i++ )
  {

    m_ComponentInfo[i].ComponentIndex = i;
    m_ComponentInfo[i].ComponentId = GetChar();

    byte cByte(GetChar());

    m_ComponentInfo[i].HorizontalSampleFactor = (cByte >> 4) & 15;
    m_ComponentInfo[i].VerticalSampleFactor = (cByte) & 15;

    m_pSource->Skip(1);

  } // END ... For each component

          
} // ReadSOF


/**
 * Reads the Start Of Scan (SOS) segment
 */
void Jpeg::ReadSOS()
{
	unsigned short iLength;

  unsigned char cByte;

  iLength = m_pSource->GetWord();

  // Read the number of pixel components
  m_iComponentsInScan = (int)GetChar();

  
  iLength -= 3;

  if ( iLength != (m_iComponentsInScan * 2 + 3) || m_iComponentsInScan < 1 || m_iComponentsInScan > 4) 
  {
    throw new IOException("Invalid SOS"); 
  }

  for ( int i(0); i < m_iComponentsInScan; i++) 
  {

    byte cId = GetChar();
    byte cByte = GetChar();
    iLength -= 2;

	int j(0);

    for ( ; j < m_iNumberOfComponents && cId != m_ComponentInfo[j].ComponentId; j++ );

    if ( j >= m_iNumberOfComponents ) 
    {
      throw new IOException("Invalid component number in SOS");
    }

    m_ScanComponentInfo[i].ComponentId = cId;
    m_ScanComponentInfo[i].ComponentIndex = m_ComponentInfo[j].ComponentIndex;
    m_ScanComponentInfo[i].VerticalSampleFactor = m_ComponentInfo[j].VerticalSampleFactor;
    m_ScanComponentInfo[i].HorizontalSampleFactor = m_ComponentInfo[j].HorizontalSampleFactor;
    m_ScanComponentInfo[i].DcTableNumber = (cByte >> 4) & 15;

  } // END ... for each component

  // Prediction Selection Value (PSV) or NEAR value from Lossless-LS
  m_iNEAR = (unsigned short)GetChar();

  // Skip
  m_pSource->Skip(1);

  // Point Transform Parameter
  cByte = GetChar();

  m_iPointTransformParameter = cByte & 0x0F; 

} // ReadSOS

/**
 * Reads the comment
 */
void Jpeg::ReadCOM()
{
  unsigned short iLength = m_pSource->GetWord();

  m_pSource->Skip(iLength - 2);

} // ReadCOM()

/**
 * Reads the DRI segment
 */
void Jpeg::ReadDRI()
{
  if ( m_pSource->GetWord() != 4 )
  {
    throw new IOException("Invalid DRI, bad length");
  }

  m_iRestartInterval = m_pSource->GetWord();

} // ReadDRI()

/**
 *  Reads dequantization table(s).
 */
void Jpeg::ReadDQT()
{
	unsigned short  iLength = m_pSource->GetWord();
  unsigned char   cByte;

  m_pSource->Skip(2);
  int iCount = 2;
  unsigned int iQtNumber;

  do {

    cByte = GetChar();

    if ( (iQtNumber = (cByte & 0xf)) > 1)
    {
      throw new IOException("Illegal dequantization table number");
    }
    
    if ( cByte & 0xf0 )
    {
      throw new IOException("Only 8 bit dequantization table precision supported");
    }

    cByte = GetChar();
    iCount++;
    
    unsigned int iQTNumber(0);

    for ( int i(0); i < 64; i++, iCount++)
    {
      m_DequantizationTable[m_iQuantizationSamplingIndex][(iQTNumber << 6) | i] = GetChar();
    }

    int NeedsToBeAddressed_Causes_71_CompilerError_George;
    //ZigZagReorder( (m_DequantizationTable[m_iQuantizationSamplingIndex] + (iQTNumber << 6)) );

   } while ( iCount < iLength);
   
} // ReadDQT()

/**
 * Read the APP0 Header
 *
 * Layout:
 *
 * APP0 Marker        WORD                      0xFFE0
 * Segment Length     WORD                      Length of block including length counter
 * APP0 ID            ARRAY[0...4] of CHAR      "JFIF" and 0x00
 * Version            WORD                      0x0102
 * Units              BYTE                      0-Default, 1-Pixel/in., 2-Pixel/cm.
 * X Denisty          WORD                      Horizontal Pixel ratio ( 1 is default )
 * Y Density          WORD                      Vertical Pixel ration ( 1 is deafult )
 * X Thumb            BYTE                      Width of thumbnail
 * Y Thumb            BYTE                      Hight of thumbnail
 * Thumbnail          3*ThumbSize               Thumnail image is 24bit RGB
 *
 */
void Jpeg::ReadAPP()
{
  int iLength = m_pSource->GetWord();


  if ( iLength < 16 )
  {
    throw new IOException("Invalid or corrupt JPEG, APP0 segment length is invalid");
  }


  m_pSource->Skip(iLength - 2);

  unsigned char Buffer __gc [] = new unsigned char __gc [5];

  m_pSource->Read(Buffer,0,5);

  if ( Buffer[0] != 'J' || Buffer[1] != 'F' || Buffer[2] != 'I' || Buffer[3] != 'F' )
  {
    throw new IOException("Invalid of corrupt JPEG, APP0 ID is invalid");
  }

  unsigned short iDensity;

  iDensity = m_pSource->GetWord();           // X Density
  iDensity = m_pSource->GetWord();           // Y Density

  const int iX = (int)GetChar();

  const int iY = (int)GetChar();


 
  ///////////////////////////////
  // Skip the thumnail image
  ///////////////////////////////
  const int iSize = iX * iY * 3;
  for ( int i(0); i < iSize; GetChar(), i++ );

} // ReadAPP0()

/**
 * Reads the Defined Huffman Table (DHT) segment
 *
 * Layout:
 *
 * DHT Marker         WORD                      0xFFC4
 * Segment Length     WORD                      Length of block including length counter
 * Table Identifier   BYTE                      XXXX XXXX
 *                                              ||||    `--- Table ID (0,1 Baseline)
 *                                              ````-------- Table Class: 0 - DC Table, 1 - AC Table                                       
 * Counts             BYTE [0...16]             Number of codes of each length (Bits Array)
 * Symbols            BYTE [0...16]             Huffman Value Array     
 *
 */
void Jpeg::ReadDHT()
{

  int iLength = m_pSource->GetWord() - 2;


  while ( iLength > 0 ) 
  {
    int iIndex = GetChar();

    if ( iIndex < 0 || iIndex >= 4 ) 
    {
      throw new IOException(String::Format("Invalid DHT bad index [{0}]", __box(iIndex)));
    }

    /* AC table definition */
    if ( iIndex & 0x10 ) 
    { 
      Log::Warning("Huffman table for lossless JPEG is not defined.", "Jpeg", "ReadDHT");
    } 
    else 
    {                /* DC table definition */

      if ( m_HuffmanTable[iIndex] == (HuffmanTable *)0 )
      {
        m_HuffmanTable[iIndex] = new HuffmanTable();
        if ( m_HuffmanTable[iIndex] == (HuffmanTable *)0 )
        {
          throw new System::OutOfMemoryException("Could not allocate a HUFFMAN TABLE");
        }
      }

    }

    int iCount = 0;
    for ( int i(1); i <= 16; i++ ) 
    {
      m_HuffmanTable[iIndex]->Bits[i] = GetChar();
      iCount += (int)m_HuffmanTable[iIndex]->Bits[i];
    }

    if ( iCount > 256 ) 
    {
      throw new IOException("Invalid DHT counts");
    }

    for ( int i(0); i < iCount; i++ )
    {

      m_HuffmanTable[iIndex]->HuffmanValue[i] = GetChar();

      iLength -= 1 + 16 + iCount;
    }

   }
}


/**
 * Reads the LSE ID=1 header
 */
void Jpeg::ReadLSE1()
{
	unsigned short iLength;
	unsigned char Id;

  iLength = m_pSource->GetWord();

  Id = GetChar();

  m_iMAXVAL = m_pSource->GetWord();

  unsigned short iThreshold(0);

  m_iThreshold[0] = m_pSource->GetWord();
  m_iThreshold[0] = m_pSource->GetWord();
  m_iThreshold[0] = m_pSource->GetWord();

  m_iRESET = m_pSource->GetWord();

  /////////////////////////////
  // Validate some entries
  ///////////////////////////
  if ( iLength <= 2 || Id != (unsigned char)LseId::L1 || iLength != 13 )
  {
    throw new System::IO::IOException("Corrupt or invalid JPEG, invalid LSE header");
  } // END .... If invalid
          
} // ReadLSE()

/**
 *  Reorders coefficients following the zigzag scan path.
 */
void Jpeg::ZigZagReorder(int * pCoefficients)
{
   int Tmp[64];
   int Scan[64] = {  0,  1,  5,  6, 14, 15, 27, 28,
                     2,  4,  7, 13, 16, 26, 29, 42,
                     3,  8, 12, 17, 25, 30, 41, 43,
                     9, 11, 18, 24, 31, 40, 44, 53,
                    10, 19, 23, 32, 39, 45, 52, 54,
                    20, 22, 33, 38, 46, 51, 55, 60,
                    21, 34, 37, 47, 50, 56, 59, 61,
                    35, 36, 48, 49, 57, 58, 62, 63 };
    
   register int i(0);

   for ( ; i < 64; Tmp[i] = pCoefficients[Scan[i]], i++ );
   for ( i = 0; i < 64; pCoefficients[i] = Tmp[i], i++ );

} // ZigZagReorder()



/**
 * Puts the bit specified out to the Destination image
 * 
 * @param iBit      Bit to write out
 */
void Jpeg::PutBit(unsigned int iBit)
{

	m_PutBitByte = m_PutBitByte<<1;

	if ( iBit != 0 ) 
  {
    m_PutBitByte |= 1;
  }

	if ( ++m_iPutBitCount >= 8 ) 
  {
		++m_iPutBitByteOffset;
    
    m_pDestination->Put(m_PutBitByte);

		// need to stuff with a following zero bit to distinguish from JPEG marker
		m_iPutBitCount = ( m_PutBitByte == 0xff ) ? 1 : 0;
		m_PutBitByte = 0;
	}

} // PutBit()


/**
 * Flushes the remaining bit to the destination image
 */
void Jpeg::Flush()
{
	
	m_PutBitByte = m_PutBitByte << ( 8 - m_iPutBitCount );
  m_pDestination->Put(m_PutBitByte);
	m_PutBitByte = 0;
	m_iPutBitCount = 0;

} // Flush()




/*
void Jpeg::PutRow(unsigned short * pBuffer, unsigned int iNumberOfPixels, unsigned short iBitsPerPixel )
{
	if ( iBitsPerPixel <= 8 ) 
  {
    while ( ( iNumberOfPixels-- > 0 ) && m_pDestination->Position < m_iDestImageSize )
    {
      m_pDestination->Put((unsigned char)(*pBuffer++));
    }
     
	}
	else 
  {
		while ( ( iNumberOfPixels-- > 0 ) && m_pDestination->Position < m_iDestImageSize ) 
    {
      Put(*pBuffer++);
    }
	}

}  // PutRow
*/

/**
 * Reads the Defined Huffman Table (DHT) segment
 *
 * Layout:
 *
 * DHT Marker         WORD                      0xFFC4
 * Segment Length     WORD                      Length of block including length counter
 * Table Identifier   BYTE                      XXXX XXXX
 *                                              ||||    `--- Table ID (0,1 Baseline)
 *                                              ````-------- Table Class: 0 - DC Table, 1 - AC Table                                       
 * Counts             BYTE [0...16]             Number of codes of each length (Bits Array)
 * Symbols            BYTE [0...16]             Huffman Value Array     
 *
 */
void Jpeg::PutDHT()
{

  // Marker
  Put(JpegMarker::DHT);

  // Put Length
  int iLength = 16;

  int iIndex = 0;

  // DC Table
  Put((unsigned char)iIndex);


  while ( iLength > 0 ) 
  {

    int iCount = 0;
    for ( int i(1); i <= 16; i++ ) 
    {
      Put((unsigned char)m_HuffmanTable[iIndex]->Bits[i]);
      iCount += (int)m_HuffmanTable[iIndex]->Bits[i];
    }

    if ( iCount > 256 ) 
    {
      throw new IOException("Invalid DHT counts");
    }

    for ( int i(0); i < iCount; i++ )
    {

      Put((unsigned char )m_HuffmanTable[iIndex]->HuffmanValue[i]);
      iLength -= 1 + 16 + iCount;
    }

  }
}


/**
 * Puts the SOF segment into the destination image
 */
void Jpeg::PutSOF()
{

  // Marker
  Put(JpegMarker::SOF55);

  // Put the length (inclusive of self)
  m_pDestination->Put((unsigned short)11);			

	m_pDestination->Put((unsigned char)m_iPrecision);		// sample precision
	m_pDestination->Put((unsigned short)m_iRows);			  // Y - number of lines
	m_pDestination->Put((unsigned short)m_iColumns);		// X - number of samples per line

	m_pDestination->Put((unsigned char)1);	            // one component per frame only
	m_pDestination->Put((unsigned char)1);		          // component identifier is 1
	m_pDestination->Put((unsigned char)0x11);	          // no horizontal or vertical sampling factor
	m_pDestination->Put((unsigned char)0);		          // no quantization table used in JPEG-LS

} // PutSOF55

/**
 * Puts the SOS header
 */
void Jpeg::PutSOS()
{

  // Marker
  Put(JpegMarker::SOS);

	m_pDestination->Put((unsigned short)8);		                    // length (inclusive of self)

	m_pDestination->Put((unsigned char)m_iComponentsInScan);	    // one component per scan only

  for ( int i(0); i < m_iComponentsInScan; i++ )
  {
    Put((byte)m_ScanComponentInfo[i].ComponentId);
    Put((byte)(m_ScanComponentInfo[i].DcTableNumber << 4));
  }

	m_pDestination->Put((unsigned char)m_iNEAR);                  // in place of start of spectral selection
	m_pDestination->Put((unsigned char)0);			                  // ILV - interleave mode is 0 (none)
	m_pDestination->Put((unsigned char)0);			                  // not used in JPEG-LS
  
} // PutSOS()

/**
 * Puts the LSE header
 */
void Jpeg::PutLSE1()
{
  // Marker
  Put(JpegMarker::LSE);


  m_pDestination->Put((unsigned short)13);

  Put(LseId::L1);
	m_pDestination->Put((unsigned short)m_iMAXVAL);
	m_pDestination->Put((unsigned short)m_iThreshold[0]);
	m_pDestination->Put((unsigned short)m_iThreshold[1]);
	m_pDestination->Put((unsigned short)m_iThreshold[2]);
	m_pDestination->Put((unsigned short)m_iRESET);

} // PutLSE1()


/**
 * 
 * @param k   
 * @param iGolumbLimit
 * @param iValue
 */
void Jpeg::EncodeMappedErrvalWithGolomb(unsigned short k, unsigned short iGolumbLimit, unsigned int  iValue)
{

	// A.5.3 Mapped-error encoding

	unsigned int iUnarycode( iValue >> k );					// Most significant bits go into unary code
	unsigned short iLimit = iGolumbLimit - m_iQuantizedBitsPerPixel - 1;


  ////////////////////////////////////////////////
  // If the unary code is less then the limit
  ///////////////////////////////////////////////
	if ( iUnarycode < iLimit) 
  {
		while (iUnarycode--) PutBit(0);			// Append unary representation of remaining most significant bits
		PutBit(1);					                // Flag the end of the unary code
		unsigned short iBits = k;						                // Append least significant k bits
		while ( iBits-- ) 
    { 
      PutBit( ( iValue >> iBits ) & 1 ); 
    } 	// msb bit is written first & use the decremented bits as shift
	}
	else 
  {
		while (iLimit--) 
    {
      PutBit(0);			// Append limit 0 bits
    }

		PutBit(1);					// Flag the end of the unary code
		iValue -= 1;
    for ( int iQbpp(m_iQuantizedBitsPerPixel); iQbpp > 0; --iQbpp)
    { 
      PutBit( ( iValue >> iQbpp ) & 1 ); 
    } 	// write whole value (always of length qbpp)
	}
} // EncodeMappedErrvalWithGolomb


/**
 *
 * @param k   
 * @param iGolumbLimit
 */
unsigned int  Jpeg::DecodeMappedErrvalWithGolomb(unsigned short k,unsigned short iGolumbLimit )
{

  unsigned int iValue(0);
  unsigned int iUnarycode(0);

	// Read unary representation of remaining most significant bits

  // stops after bit is 1 (having read and discared trailing 1 bit)
  for ( ; (GetBit() != 0 ); ++iUnarycode );

	unsigned int iOffset(0);
	unsigned short iBitsStored(0);
	const unsigned short iLimit(iGolumbLimit - m_iQuantizedBitsPerPixel - 1);

  ///////////////////////////////////////
  // use it to form most significant bits
  ///////////////////////////////////////
	if (iUnarycode < iLimit) 
  {		
		iValue       = iUnarycode;		// will later get shifted into ms bits
		iBitsStored = k;
		iOffset     = 0;
	}
	else 
  {
		iValue      = 0;			// no contribution from unary code ... whole value is next
		iBitsStored = m_iQuantizedBitsPerPixel;
		iOffset    = 1;
	}

	// Read least significant k bits
	while ( iBitsStored-- > 0 ) 
  {
    iValue = ( iValue << 1 ) | GetBit();	// msb bit is read first
  }


	iValue += iOffset;				// correct for limited case 

  return iValue;
}


/**
 * Determines the Golomb codeing variable.
 *
 * FCD 14495 ... A.5.1
 *
 * @param iN      Where iN is m_iN[iQ]
 * @param iA      Where iA is m_iA[iQ]
 *
 * @return Returns the golomb variable to be used in the Golomb code function LG(k,LIMIT)
 */
unsigned short Jpeg::DetermineGolombParameter(unsigned int iN, unsigned int iA)
{
  // Number of occurrences vs accumulated error magnitude

	unsigned short k(0);
	for ( ; ( iN << k ) < iA; ++k );
	return k;
} // DetermineGolombParameter()




/**
 * Quantize the error value
 */
void Jpeg::Quantize(int & Errval)
{

	if ( m_iNEAR > 0 ) 
  {
		if (Errval > 0)
    {
			Errval = ( Errval + m_iNEAR) / ( 2 * m_iNEAR + 1 );
    }
		else
    {
			Errval = ( Errval - m_iNEAR ) / ( 2 * m_iNEAR + 1 );		// in A.4.4 it is actually -(m_iNEAR-Errval)/(2*m_iNEAR+1)
    }
	}

} // Quantize()

/**
 * Dequantizes the error value
 */
void Jpeg::Dequantize(int & Errval)
{
	if ( m_iNEAR > 0 ) 
  {
    Errval = Errval * ( 2 * m_iNEAR + 1 );
  }

} // Dequantize()


/**
 *
 */
void Jpeg::CodecRunEndSample(unsigned short &Ix,int iRa,int iRb, ProcessType eProcessType )
{

  const unsigned short iRItype( (iRa == iRb || Math::Abs( iRa - iRb ) <= m_iNEAR) ? 1 : 0 );

	const short iSIGN(( iRItype == 0 && iRa > iRb) ? -1 : 1);

	int iPx = (iRItype == 1 ? iRa : iRb);


	const unsigned int iA = (iRItype == 1 ? (m_iA[366] + (m_iN[366] >> 1)) : m_iA[365]);

	unsigned short iQ = 365 + iRItype;

	unsigned short k = DetermineGolombParameter(m_iN[iQ], iA);

	int  Errval;
	int  updateErrval;
	unsigned int EMErrval;

  const int iGolombLimit(m_iLIMIT - J[m_iRunIndex] - 1);

  ////////////////////////////////
  // If we are decompressing
  ///////////////////////////////
  if ( eProcessType = ProcessType::DECOMPRESS ) 
  {

		EMErrval = DecodeMappedErrvalWithGolomb( k, iGolombLimit );	

		unsigned int tEMErrval = EMErrval + iRItype;		// use local copy to leave original for parameter update later

    //
    const int iRemaining(tEMErrval % 2);

		if (tEMErrval == 0) 
    {
			Errval = 0;
		}
		else if (k == 0) 
    {

			if ( 2 * m_iNn[iQ-365] < m_iN[iQ]) 
      {
				if ( iRemaining == 0) 
        {
					Errval = -int(tEMErrval >> 1);		// "map = 0"	2 becomes -1, 4 becomes -2, 6 becomes -3
				}
				else 
        {
					Errval = (tEMErrval + 1) >> 1;		// "map = 1"	1 becomes 1, 3 becomes 2, 5 becomes 3
				}
			}
      // 2*Nn[Q-365] >= N[Q]
			else  
      {	
				if ( iRemaining == 0) 
        {
					Errval = tEMErrval >> 1;			// "map = 0"	2 becomes 1, 4 becomes 2, 6 becomes 3
				}
				else 
        {
					Errval = -int((tEMErrval + 1) >> 1);	// "map = 1"	1 becomes -1, 3 becomes -2, 5 becomes -3
				}
			}
		}
		else 
    {
			if ( iRemaining == 0 ) 
      {
				Errval = tEMErrval >> 1;				// "map = 0"	2 becomes  1, 4 becomes  2, 6 becomes 3
			}
			else 
      {
				Errval = -int((tEMErrval + 1) >> 1);		// "map = 1"	1 becomes -1, 3 becomes -2, 5 becomes -3
			}
		}


		updateErrval=Errval;

		Dequantize(Errval);


		if ( iSIGN < 0 ) Errval = -Errval;		// if "context type" was negative

		// Apply inverse point transform and mapping table when implemented
		Ix = (unsigned short)ComputeRx(iPx, Errval);

	}
	else 
  {

		Errval = int(Ix) - iPx;           

    ////////////////////////////////////
    // if "context type" was negative
    ///////////////////////////////////
		if (iSIGN < 0) Errval = -Errval;		


		// Figure out sign to later correct Errval (Figure A.19) ...

    /////////////////////////////////////////////////////////////////////////////////
    // For near-lossless, quantize Errval and derive reconstructed value (A.4.4)
    /////////////////////////////////////////////////////////////////////////////////
		if ( m_iNEAR > 0 ) 
    {	

      // Quantize error value
			Quantize(Errval);


      ////////////////////////////////////////////////////////////////////
			// Replace with the reconstructed value the decoder will have
			// (obviously if in lossless mode there will be no difference)
      ///////////////////////////////////////////////////////////////////
			int iRx = iPx + iSIGN * Errval * (2 * m_iNEAR + 1);
			
      // CLAMP
      iRx = CLAMP((int)0,iRx,(int)m_iMAXVAL);
					
			Ix = (unsigned short)iRx;

		} // END ... If near-lossless

		// Modulo reduction of the prediction error (A.4.5)
    ModuloErrorPredictionReduction(Errval);

    updateErrval = Errval;

		// Golomb stuff is outside decompress/compress decision since same

		// Map error to non-negative ...

		// short map = ((k == 0 && Errval > 0 && 2*Nn[Q-365] < N[Q]) || (Errval < 0 && (2*Nn[Q-365] >= N[Q] || k != 0)) ? 1 : 0;
		// EMErrval = 2*Abs(Errval) - RItype - map;


		if ( k == 0 ) 
    {
      const int iNegCount( 2 * m_iNn[iQ - 365] );

			if (Errval > 0) 
      {
				if ( iNegCount < m_iN[iQ] ) 
        {
					EMErrval = 2 * Errval - 1;		// "map = 1"	1 becomes 1, 2 becomes 3, 3 becomes 5
				}
        // 2*Nn[Q-365] >= N[Q]
				else 
        {	
					EMErrval = 2 * Errval;			// "map = 0"	1 becomes 2, 2 becomes 4, 3 becomes 6
				}
			}
			else if (Errval < 0) 
      {
				if ( iNegCount < m_iN[iQ] ) 
        {
					EMErrval = -2 * Errval;			// "map = 0"	-1 becomes 2, -2 becomes 4, -3 becomes 6
				}
				else 
        {	// 2*Nn[Q-365] >= N[Q]
					EMErrval = -2 * Errval - 1;		// "map = 1"	-1 becomes 1, -2 becomes 3, -3 becomes 5
				}
			}
			else 
      { // Errval == 0
				EMErrval = 0;					// "map = 0"	0 stays 0
			}
		}
		else 
    {	// k != 0
			if (Errval > 0) 
      {
				EMErrval = 2 * Errval;				// "map = 0"	1 becomes 2, 2 becomes 4, 3 becomes 6
			}
			else if (Errval < 0) 
      {
				EMErrval = -2 * Errval - 1;			// "map = 1"	-1 becomes 1, -2 becomes 3, -3 becomes 5
			}
			else 
      { // Errval == 0
				EMErrval = 0;					// "map = 0"	0 stays 0
			}
		}

		EMErrval -= iRItype;

   
		EncodeMappedErrvalWithGolomb(k, iGolombLimit, EMErrval);

	}

	// Update parameters ...

	if (updateErrval < 0) 
  {
    ++m_iNn[iQ - 365];
  }

	m_iA[iQ] += (EMErrval + 1 - iRItype) >> 1;

	if ( m_iN[iQ] == m_iRESET) 
  {
		m_iA[iQ]        = m_iA[iQ] >> 1;
		m_iN[iQ]        = m_iN[iQ] >> 1;
		m_iNn[iQ - 365] = m_iNn[iQ - 365] >> 1;
	}

	++m_iN[iQ];

} // CodecRunEndSample()


/**
 * Decompresses into the supplies image buffer
 *
 * @param pBuffer            Destination image buffer
 * @param iBufferSize       Destination buffer in bytes
 *
 * @return Returns the actual number of bytes used in the buffer.
 */
int Jpeg::Decompress(ImageBuffer * pDestination)
{

  m_pDestination = pDestination;

  Decompress();

  return m_pDestination->Position;
} // Decompress()


/**
 * Decompresses into the supplies image buffer
 *
 * @param pBuffer            Destination image buffer
 * @param iBufferSize        Destination buffer in bytes
 *
 * @return Returns the actual number of bytes used in the buffer.
 */
int Jpeg::Compress(CompressionType eType, ImageBuffer * pDestination)
{

  m_pDestination = pDestination;


  Compress(eType);


  return m_pDestination->Position;

} // Decompress()


/**
 * Compresses image
 */
void Jpeg::Compress(CompressionType eType)
{
  switch ( eType )
  {
  case CompressionType::LOSSLESS:
    CompressLossless2();
    break;
  case CompressionType::LOSSLESS_LS:
    CompressLosslessLs();
    break;
  default:
    throw new System::InvalidOperationException("Invalid compression type");
  } // END ... switch on compression type
} // Compress()

/**
 * Compresses image
 */
void Jpeg::CompressLosslessLs()
{

  m_iPrecision = m_iBitsPerPixel;

  if ( ! m_bUseMarkers )
  {

    m_iMAXVAL = (1ul << m_iPrecision ) - 1;
    
    if ( m_iRESET == 0 ) m_iRESET = 64;

    // Basic Initial Thresholds
    const int iBasicThreshold [] = { 3, 7, 21 };
    const int iOffset [] = { 2, 3, 4 };
    const int iNear [] = { (3 * m_iNEAR), (5 * m_iNEAR), (7 * m_iNEAR) };

    if ( m_iMAXVAL > 128 )
    {

      const unsigned int iFactor((unsigned int)System::Math::Floor((double)(256 / (m_iMAXVAL + 1))));


      /////////////////////////////
      // Set all the thresholds
      //////////////////////////////
      for ( int i(0); i < 3; i++ )
      {
        if ( m_iThreshold[i] == 0 )
        {
          m_iThreshold[i] = Bound( (unsigned int)(m_iNEAR + 1), (unsigned int)( iFactor * ( iBasicThreshold[i] - iOffset[i] ) + iOffset[i] + iNear[i] ), m_iMAXVAL);
        }

      } // END ... For each threshold

    }
    else
    {
      const unsigned int iFactor((unsigned int)System::Math::Floor((double)(System::Math::Min((int)m_iMAXVAL, (int)4095) + 128 / 256)));

      /////////////////////////////
      // Set all the thresholds
      //////////////////////////////
      for ( int i(0); i < 3; i++ )
      {
        if ( m_iThreshold[i] == 0 )
        {
          m_iThreshold[i] = Bound((unsigned int)(m_iNEAR + 1), (unsigned int)System::Math::Max((double)iOffset[i], (double)(iBasicThreshold[i] / iFactor + iNear[i])), m_iMAXVAL);
        }

      } // END ... For each threshold

    }

  } // END ... If we are not using markers


  ProcessImage(ProcessType::COMPRESS);

} // Compress()

/** 
 * Returns the number of set bits in the short value
 *
 * @param iValue    Value to count the bits in
 */
int Jpeg::GetBitCount(int iValue)
{
  int iBits(0);

  /////////////////////////////////////////////////
  // Count all the bits set in the value supplied
  /////////////////////////////////////////////////
  while ( iValue != 0 )
  {
    ++iBits;
    iValue >>= 1;
  }  // END ... while there are still bits in the value 

  //if ( iBits > 15 )
  //{
  //  throw new System::Exception("Bad DCT Coefficiant value");
  //}

  return iBits;

} // GetBitCount()

/**
 * Compresses image lossless
 */
void Jpeg::CompressLossless()
{
  //////////////////////////////////////////////////////////////
  // If we are using markers write them out to the dest image
  /////////////////////////////////////////////////////////////
  if ( m_bUseMarkers )
  {
    PutSOI();
    PutSOF();
  } // END ... If we are comprsssing and using markers


  // Create the huffman table
  m_pHuffmanTable = new System::Collections::ArrayList();

  // Create the huffman tree
  ArrayList * pHuffmanTree = new ArrayList();



  int iBits __gc [] = new int __gc [33];
  int iFrequency __gc [] = new int __gc [257];
  int iOthers __gc [] = new int __gc [257];
  int iCodeSize __gc [] = new int __gc [257];

  // Initialize the bit counter array
  for ( register int i(0); i < 17; iBits[i] = 0, i++ );

  // Initialize the Codesize and Others array.
  for ( register int i(0); i < 257; iCodeSize[i] = 0, iOthers[i] = -1, i++ );

  //
  iFrequency[256] = 1;
  
  // Initial predictor
  int iPredictor( 1 << ( 16 - m_iPointTransformParameter - 1 ) );

  const int iSize(m_pSource->Length / 2);  


  /////////////////////////////////////////////////////////////////////////////
  // Loop through the source image setting the bit frequency of each shot value
  /////////////////////////////////////////////////////////////////////////////
  for ( register int i(0); i < iSize; i++ )
  {
     short iCh(m_pSource->GetWord());
     
     int iValue(System::Math::Abs(iCh - iPredictor));

     // Set the predictor to the previous value
     iPredictor = iCh;

     iFrequency[GetBitCount(iValue)]++;
    
  } // END ... For each short value in the source image calculate the bit frequency
 
  // Rewind the source
  m_pSource->Rewind();

  
  ///////////////////////////////////////
  // Build the initial huffman tree
  //////////////////////////////////////
  for ( register int i(0); i < 256; i++ )
  {

    ////////////////////////////////////////////////////////
    // If the freqeuncy is greater then 0 then add a node
    ////////////////////////////////////////////////////////
    if ( iFrequency[i] > 0 )
    {
      Node * pNode = new Node(i, m_iHuffmanCount[i]);
      m_pHuffmanTable->Add(pNode);
      pHuffmanTree->Add(pNode);
    }
  } // END ... for each character



  /////////////////////////////
  // Build the binary tree
  ////////////////////////////
  while ( pHuffmanTree->Count > 1 )
  {
    pHuffmanTree->Sort();

    Node * pNode1 = dynamic_cast<Node *>(pHuffmanTree->Item[0]);
    Node * pNode2 = dynamic_cast<Node *>(pHuffmanTree->Item[1]);

    pHuffmanTree->RemoveAt(0);
    pHuffmanTree->RemoveAt(0);

    Node * pRoot = new Node(pNode2, pNode1, pNode1->Count + pNode2->Count);
    pRoot->Value = pNode1->Value;

    pHuffmanTree->Add(pRoot);

    int iV1 = pNode1->Value;
    int iV2 = pNode2->Value;

    while ( true ) 
    {
      iCodeSize[iV1]++;

      if ( iOthers[iV1] == -1 )
      {
        break;
      }
      else
      {
        iV1 = iOthers[iV1];
      }
    }

    iOthers[iV1] = iV2;
    

    while ( true )
    {
      iCodeSize[iV2]++;

      if ( iOthers[iV2] == -1 )
      {
        break;
      }
      else
      {
        iV2 = iOthers[iV2];
      }
    }

  } // END ... while we do not have a root


  for ( register int j(0); j < 257; j++ )
  {
    if ( iCodeSize[j] != 0 )
    {
      iBits[iCodeSize[j]]++;
    }

  }

  for ( int i(32); i > 0 ; )
  {
    if ( iBits[i] > 0 )
    {

      int j(i - 2);
      for ( ; iBits[j] <= 0; j--);

      iBits[i]    = iBits[i] - 1;
      iBits[i-1]  = iBits[i-1] + 1;
      iBits[j+1]  = iBits[j+1] + 2;
      iBits[j]    = iBits[j] - 1;
    }
    else
    {
      i--;
      if ( i == 16 )
      {
        for ( ; i > 0 && iBits[i] == 0; --i);
        iBits[i]--;
        break;
      }

    }
  }

  int iHuffVal __gc [] = new int __gc [257];

  for ( int i = 1, j = 0, k = 0; i <= 32; )
  {
    if ( iCodeSize[j] == i )
    {
      iHuffVal[k] = j;
      k++;
    }

    if ( ++j > 255 )
    {
      j = 0;
      i++;
    }

  }

  /*
  int iTest = 0;

  int iBitCount __gc [] = new int __gc [16];

  for ( register int i(0); i < m_pHuffmanTable->Count; i++ )
  {
    Node * pLeafNode = dynamic_cast<Node *>(m_pHuffmanTable->Item[i]);

    int iCode(0);
    int iBits(0);
    Node * pNode = pLeafNode;

    for ( register int j(0); pNode->Root != NULL; pNode = pNode->Root, j++ )
    {
      if ( pNode->Root->Left == pNode )
      {
        iCode = (1 << j) | iCode;
        iBits = j + 1;
      }
    }

    pLeafNode->Code = iCode;
    pLeafNode->Bits = iBits;
   
    iBitCount[iBits]++;

    Debug::WriteLine(String::Format("0x{0:X2}   0x{1:X8}   {2}",__box(pLeafNode->Index), __box(pLeafNode->Code), String::Format("{0:00}  {1}",__box(pLeafNode->Bits), __box(pLeafNode->Count))));

  }
*/


} // CompressLossless()


/**
* Compresses image lossless
*/
void Jpeg::CompressLossless2()
{

  if ( m_HuffmanTable[0] == (HuffmanTable *)0 )
  {
    m_HuffmanTable[0] = new HuffmanTable();
    if ( m_HuffmanTable[0] == (HuffmanTable *)0 )
    {
      throw new System::OutOfMemoryException("Could not allocate a HUFFMAN TABLE");
    }
  }
  else
  {
    m_HuffmanTable[0]->ClearAll();
  }

  //////////////////////////////////////////////////////////////
  // If we are using markers write them out to the dest image
  /////////////////////////////////////////////////////////////
  if ( m_bUseMarkers )
  {
    PutSOI();
    PutSOF();
  } // END ... If we are comprsssing and using markers


  // Create the huffman table
  m_pHuffmanTable = new System::Collections::ArrayList();

  // Create the huffman tree
  ArrayList * pHuffmanTree = new ArrayList();



  int iBits __gc [] = new int __gc [33];
  int iFrequency __gc [] = new int __gc [257];
  int iOthers __gc [] = new int __gc [257];
  int iCodeSize __gc [] = new int __gc [257];

  // Initialize the bit counter array
  for ( register int i(0); i < 17; iBits[i] = 0, i++ );

  // Initialize the Codesize and Others array.
  for ( register int i(0); i < 257; iCodeSize[i] = 0, iOthers[i] = -1, i++ );

  //
  iFrequency[256] = 1;

  // Initial predictor
  int iPredictor( 1 << ( 16 - m_iPointTransformParameter - 1 ) );

  const int iSize(m_pSource->Length / 2);  


  /////////////////////////////////////////////////////////////////////////////
  // Loop through the source image setting the bit frequency of each shot value
  /////////////////////////////////////////////////////////////////////////////
  for ( register int i(0); i < iSize; i++ )
  {
    short iCh(m_pSource->GetWord());

    int iValue(System::Math::Abs(iCh - iPredictor));

    // Set the predictor to the previous value
    iPredictor = iCh;

    iFrequency[GetBitCount(iValue)]++;

  } // END ... For each short value in the source image calculate the bit frequency

  // Rewind the source
  m_pSource->Rewind();

  int iV1;
  int iV2;
  long lValue;

  //////////////////////////////////////////////////////////////////////////
  // Huffman's basic algorithm to assign optimal code lengths to symbols
  //////////////////////////////////////////////////////////////////////////
  for ( ; ; ) 
  {

    iV1 = -1;
    lValue = 1000000000L;

    ///////////////////////////////////////////////////////////////////
    // Find the smallest nonzero Frequency, set c1 = to its symbol 
    // In case of ties, take the larger symbol number 
    ///////////////////////////////////////////////////////////////////
    for ( register int i(0); i <= 256; i++ ) 
    {
      if ( iFrequency[i] > 0 && iFrequency[i] <= lValue ) 
      {
        lValue = iFrequency[i];
        iV1 = i;
      }
    } // END ... For each frequency value 

    iV2 = -1;
    lValue = 1000000000L;

    //////////////////////////////////////////////////////////////////////////
    // Find the next smallest nonzero Frequency, set c2 = its symbol 
    // In case of ties, take the larger symbol number 
    //////////////////////////////////////////////////////////////////////////
    for ( register int i(0); i <= 256; i++) 
    {
      if ( iFrequency[i] > 0 && iFrequency[i] <= lValue && i != iV1 ) 
      {
        lValue = iFrequency[i];
        iV2 = i;
      }
    } // END ... For each frequency value 

    // Done if we've merged everything into one Frequency 
    if ( iV2 < 0 )
      break;

    //////////////////////////////////////
    // Else merge the two counts/trees 
    //////////////////////////////////////
    iFrequency[iV1] += iFrequency[iV2];
    iFrequency[iV2] = 0;


    // Increment the codesize of everything in c1's tree branch 
    iCodeSize[iV1]++;

    // 
    while ( iOthers[iV1] != -1 ) 
    {
      iV1 = iOthers[iV1];
      iCodeSize[iV1]++;
    } // END ... While 

    // chain c2 onto c1's tree branch 
    iOthers[iV1] = iV2;	    

    // Increment the codesize of everything in c2's tree branch 
    iCodeSize[iV2]++;

    //
    while ( iOthers[iV2] != -1 ) 
    {
      iV2 = iOthers[iV2];
      iCodeSize[iV2]++;
    } // END ... While


  } // END ... For ever

  /////////////////////////////////////////////////////////
  // Now count the number of symbols of each code length
  /////////////////////////////////////////////////////////
  for ( register int j(0); j <= 256; j++ ) 
  {

    ///////////////////////////////////
    // if there is a code of this size
    ///////////////////////////////////
    if ( iCodeSize[j] > 0 ) 
    {
      ///////////////////////////////////////////////////////////
      // The JPEG standard seems to think that this can't happen
      ///////////////////////////////////////////////////////////
      if ( iCodeSize[j] > 32 )
      {
        throw new System::OverflowException("CLEN Overflow");
      }

      m_HuffmanTable[0]->Bits[iCodeSize[j]]++;

      //iBits[iCodeSize[j]]++;
    } // END ... If the code size is set 

  } // END ... For each code size

  ////////////////////////////////////////////////////////////////////////////////
  // JPEG doesn't allow symbols with code lengths over 16 bits, so if the pure
  // Huffman procedure assigned any such lengths, we must adjust the coding.
  // Here is what the JPEG spec says about how this next bit works:
  // Since symbols are paired for the longest Huffman code, the symbols are
  // removed from this length category two at a time.  The prefix for the pair
  // (which is one bit shorter) is allocated to one of the pair; then,
  // skipping the BITS entry for that prefix length, a code word from the next
  // shortest nonzero BITS entry is converted into a prefix for two code words
  // one bit longer.
  /////////////////////////////////////////////////////////////////////////////////
  /*
  for ( int i(32); i > 16; i-- ) 
  {
    while ( iBits[i] > 0 ) 
    {

      for ( int j(i - 2); iBits[j] == 0; --j);

      iBits[i] -= 2;		  // remove two symbols 
      iBits[i-1]++;		    // one goes in this length 
      iBits[j+1] += 2;		// two new symbols in this length 
      iBits[j]--;		      // symbol of this length is now a prefix 
    }
  }
  */


  //////////////////
  // C-3 in specs
  //////////////////
  for  ( int i(32); ; )
  {

    if ( iBits[i] > 0 )
    {

      int j(i + 1);
      for ( ; iBits[j] <= 0; --j);

      m_HuffmanTable[0]->Bits[i] -= 2;
      m_HuffmanTable[0]->Bits[i-1]++;
      m_HuffmanTable[0]->Bits[j+1] += 2;
      m_HuffmanTable[0]->Bits[j]--;

      //iBits[i] -= 2;		  // remove two symbols 
      //iBits[i-1]++;		    // one goes in this length 
      //iBits[j+1] += 2;		// two new symbols in this length 
      //iBits[j]--;		      // symbol of this length is now a prefix 

    }
    else
    {
      --i;

      if ( i == 16 ) 
      {
        for ( ; m_HuffmanTable[0]->Bits[i] == 0; --i  );
        --m_HuffmanTable[0]->Bits[i];
        break;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Remove the count for the pseudo-symbol 256 from the largest codelength
  ////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////
  // find largest codelength still in use 
  /////////////////////////////////////////
  //while ( iBits[i--] == 0 );

  //iBits[i]--;


 
  ////////////////////////////////////////////////////////////////////////////
  // Return a list of the symbols sorted by code length 
  // It's not real clear to me why we don't need to consider the codelength
  // changes made above, but the JPEG spec seems to think this works.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned char huffmanValue __nogc [257];
  int p(0);

  for ( int i(1); i <= 32; i++) 
  {
    for ( register int j(0); j <= 255; j++ ) 
    {
      if (iCodeSize[j] == i ) 
      {
        m_HuffmanTable[0]->HuffmanValue[p] = (unsigned char) j;
        huffmanValue[p] = (unsigned char) j;
        p++;
      }
    }
  } // END


  


} // CompressLossless2()


/**
 * Initializes all context conters
 */
void Jpeg::InitializeCounters()
{
  /// Number of contexts ( 2 will be added when allocating )
  const int iContexts = m_iContexts + 2;

  /// Context Type Counters
  m_iN  = new int __gc [iContexts];

  /// Accumulated Prediction Magnitude 
  m_iA = new unsigned int __gc [iContexts];

  /// Auxillary Counters for bias correction
  m_iB = new int __gc [iContexts];

  /// Bias Correction Counter
  m_iC = new int __gc [iContexts]; 

  /// Negative Predication Error
  m_iNn = new int __gc [iContexts]; 


  const int iAccumulatedPredicationMagnitudeDefault = (int)System::Math::Max((double)2.0, System::Math::Floor( (m_iRANGE + ( 11u << 5 ) ) / ( 11u << 6 ) ) );

  ///////////////////////////////////
  // Initialaze all the counters
  /////////////////////////////////
  for ( int i(0); i < m_iContexts; i++ )
  {
    m_iN[i] = 1;
    m_iA[i] = iAccumulatedPredicationMagnitudeDefault;
    m_iB[i] = 0;
    m_iC[i] = 0;
  } // END ... for each counter

  m_iNn[365-365] = m_iNn[366-365] = 0;

} // InitialzieCounters()


/***
 * Process images
 */
void Jpeg::ProcessImage(ProcessType eProcessType)
{


  const int iNearAdj(2 * m_iNEAR);

  m_iRANGE = (int)System::Math::Floor( (m_iMAXVAL + iNearAdj ) / iNearAdj + 1 ) + 1;

  m_iBitsPerPixel = (unsigned short)System::Math::Max((double)2.0, System::Math::Ceiling(System::Math::Log(m_iMAXVAL + 1)));

  m_iQuantizedBitsPerPixel = (unsigned short)System::Math::Ceiling((double)m_iRANGE);

  m_iLIMIT = 2 * ( m_iQuantizedBitsPerPixel + System::Math::Max((int)8, (int)m_iBitsPerPixel) );
   
  
  Log::Debug(String::Format("Near:                      {0}", __box(m_iNEAR)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Rows:                      {0}", __box(m_iRows)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Columns:                   {0}", __box(m_iColumns)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Precision:                 {0}", __box(m_iPrecision)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Max Value:                 {0}", __box(m_iMAXVAL)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Reset:                     {0}", __box(m_iRESET)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Threshold 1:               {0}", __box(m_iThreshold[0])), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Threshold 2:               {0}", __box(m_iThreshold[1])), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Threshold 3:               {0}", __box(m_iThreshold[2])), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Range:                     {0}", __box(m_iRANGE)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Bits Per Pixel:            {0}", __box(m_iBitsPerPixel)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Quantized Bits Per Pixel:  {0}", __box(m_iQuantizedBitsPerPixel)), "Jpeg", "ProcessImage");
  Log::Debug(String::Format("Limit:                     {0}", __box(m_iLIMIT)), "Jpeg", "ProcessImage");

  //////////////////////////////////////////////////////////////
  // If we are using markers write them out to the dest image
  /////////////////////////////////////////////////////////////
  if ( eProcessType == ProcessType::COMPRESS && m_bUseMarkers )
  {
    PutSOI();
    PutSOF();
    PutLSE1();
    PutSOS();
  } // END ... If we are comprsssing and using markers


  InitializeCounters();


  unsigned int m_iRunIndex = 0;

  unsigned short * pRowA = new unsigned short[m_iColumns];
  unsigned short * pRowB = new unsigned short[m_iColumns];


  unsigned short * pThisRow(pRowA);
  unsigned short * pPrevRow(pRowB);

  for ( int iRow(0); iRow < m_iRows; iRow++ )
  {

    unsigned int iRead(m_pSource->ReadRow(pRowA, m_iColumns, m_iBitsPerPixel ));

    unsigned short iPrevRa0;

    for ( int iCol(0); iCol < m_iColumns; iCol++ )
    {

      int iRx(0);

      unsigned short iRa(0);
      unsigned short iRb(0);
      unsigned short iRc(0);
      unsigned short iRd(0);

      if ( iRow > 0 )
      {
        iRb = pPrevRow[iCol];
        iRc = ( iCol > 0 ? pPrevRow[iCol - 1] : iPrevRa0 );
        iRa = ( iCol > 0 ? pThisRow[iCol - 1] : ( iPrevRa0 = iRb ) );
        iRd = ( (iCol + 1) < m_iColumns ? pPrevRow[iCol + 1] : iRb ); 
      }
      else
      {
        iRb = 0;
        iRc = 0;
        iRa = (iCol > 0 ? pThisRow[iCol - 1] : (iPrevRa0 = 0 ));
        iRd = 0;
      }

      //////////////////////////////////////////////////////////////////////////////////////////////////////////
      // We want the reconstructed values, which are the same in lossless mode, but if m_iNEAR != 0 take care
      // to write back reconstructed values into the row buffers in previous patterns
      //////////////////////////////////////////////////////////////////////////////////////////////////////////

      // Compute the local gradient
      int iD1 = (int)(iRd - iRb);
      int iD2 = (int)(iRb - iRc);
      int iD3 = (int)(iRc - iRa);

      if ( Math::Abs(iD1) <= (int)m_iNEAR && Math::Abs(iD2) <= (int)m_iNEAR && Math::Abs(iD3) <= (int)m_iNEAR && m_eMode == Viatronix::Imaging::Mode::RUN )
      {
        // Run mode
        if ( eProcessType == ProcessType::DECOMPRESS ) 
        {


		      while ( true ) 
          {

            unsigned int iR = GetBit();

			      if ( iR == 1 ) 
            {
				      // Fill image with 2^J[m_iRunIndex] samples of Ra or till EOL
				      int rm = J_rm[m_iRunIndex];

				      while ( rm-- && iCol < m_iColumns ) 
              {
					      pThisRow[iCol] = iRa;
					      ++iCol;
				      }

				      // This will match when exact count coincides with end of row ...
				      if ( rm == -1 && m_iRunIndex < 31 ) 
              {
					      ++m_iRunIndex;
				      }

				      if ( iCol >= m_iColumns ) 
              {
					      break;
				      }
			      }
			      else 
            {
	
              unsigned int iFill = (unsigned int)GetBits(J[m_iRunIndex]);

              
				      // msb bit is read first
				      //while ( iBits-- > 0 ) 
              //{
					    //  iFill = (iFill << 1) | ReadBit();
				      //}


				      // Fill with nfill values of Ra
				      while ( iFill-- ) 
              {
					      pThisRow[iCol] = iRa;
					      ++iCol;
				      }

				      // Decode the run interruption sample ...

				      // First update local context for interrupting sample, since weren't kept updated during run
              UpdateLocalContext(iRow, iCol, pPrevRow, pThisRow, iRa, iRb);

              CodecRunEndSample(pThisRow[iCol],iRa,iRb,eProcessType);

				      if ( m_iRunIndex > 0 ) 
              {
					      --m_iRunIndex;	// NB. Do this AFTER J[m_iRunIndex] used in the limited length Golomb coding
				      }

				      break;
			      }
		      }
	      } // END ... If we are DECOMPRESSING
	      else 
        {
		      // Scan to determine length of run (A.7.1.1) ...
          int iRunCount = RunScanning(pThisRow, iCol, iRa);

          // Encode length of run (A.7.1.2) ...

          ////////////////////////////////////////////////
          // Encoding of run segments of length rm
          ///////////////////////////////////////////////
          for ( unsigned short rm(J_rm[m_iRunIndex]); iRunCount >= rm; rm = J_rm[m_iRunIndex] ) 
          {
			      PutBit(1);
			      iRunCount -= rm;
			      //Assert(RUNcnt >= 0);	// is why int not unsigned
			      if ( m_iRunIndex < 31 ) 
            {	// ie. value ranges from 0..31
				      ++m_iRunIndex;
			      }
		      } // END ... While run segments are >= 

          ////////////////////////////////////////////////////////
          // Encoding of run segments of length less them rm
          ///////////////////////////////////////////////////////
		      if (iCol < m_iColumns) 
          {	

			      PutBit(0);

			      // Append least significant J[m_iRunIndex] bits
			      unsigned short iBits = J[m_iRunIndex];
			      unsigned int iValue(iRunCount);

            // msb bit is written first
			      while ( iBits-- ) 
            {
				      PutBit(( iValue >> iBits ) & 1);  // use the decremented bits as shift
			      }

			      // Encode run interruption sample (A.7.2) ...

			      // First update local context for interrupting sample, since weren't kept updated during run
            UpdateLocalContext(iRow, iCol, pPrevRow, pThisRow, iRa, iRb);

			      CodecRunEndSample(pThisRow[iCol],iRa,iRb,eProcessType);

			      if ( m_iRunIndex > 0 ) 
            {
				      --m_iRunIndex;	// NB. Do this AFTER J[m_iRunIndex] used in the limited length Golomb coding
			      }

		      } // END ... If we are not at the end of the row
		      else 
          {						
            if ( iRunCount > 0) 
            {
				      PutBit(1);		// Append an extra 1
			      }
		      } // END ... Else we were at the end of the row

	      } // END ... Else we are COMPRESSING
      }
      else
      {
        // Regular mode

        // Gradient quantization ... (A.3.3)

        short iQ1(0);
        short iQ2(0);
        short iQ3(0);

        if      (iD1 <= -m_iThreshold[2])     iQ1=-4;
        else if (iD1 <= -m_iThreshold[1])     iQ1=-3;
        else if (iD1 <= -m_iThreshold[0])     iQ1=-2;
        else if (iD1 <  -m_iNEAR)             iQ1=-1;
        else if (iD1 <=  m_iNEAR)             iQ1= 0;
        else if (iD1 <   m_iThreshold[0])     iQ1= 1;
        else if (iD1 <   m_iThreshold[1])     iQ1= 2;
        else if (iD1 <   m_iThreshold[2])     iQ1= 3;
        else                                  iQ1= 4;

        if      (iD2 <= -m_iThreshold[2])     iQ2=-4;
        else if (iD2 <= -m_iThreshold[1])     iQ2=-3;
        else if (iD2 <= -m_iThreshold[0])     iQ2=-2;
        else if (iD2 <  -m_iNEAR)             iQ2=-1;
        else if (iD2 <=  m_iNEAR)             iQ2= 0;
        else if (iD2 <   m_iThreshold[0])     iQ2= 1;
        else if (iD2 <   m_iThreshold[1])     iQ2= 2;
        else if (iD2 <   m_iThreshold[2])     iQ2= 3;
        else                                  iQ2= 4;

        if      (iD3 <= -m_iThreshold[2])     iQ3=-4;
        else if (iD3 <= -m_iThreshold[1])     iQ3=-3;
        else if (iD3 <= -m_iThreshold[0])     iQ3=-2;
        else if (iD3 <  -m_iNEAR)             iQ3=-1;
        else if (iD3 <=  m_iNEAR)             iQ3= 0; 

        else if (iD3 <   m_iThreshold[0])     iQ3= 1;
        else if (iD3 <   m_iThreshold[1])     iQ3= 2;
        else if (iD3 <   m_iThreshold[2])     iQ3= 3;
        else                                  iQ3= 4;


        // Context merging and determination of iSIGN ... (A.3.4)

        short iSIGN;

        // "If the 1st non-zero component of vector (Q1,Q2,Q3) is negative" ...

        if ( iQ1 < 0 || (iQ1 == 0 && iQ2 < 0) || (iQ1 == 0 && iQ2 == 0 && iQ3 < 0) ) 
        {
          iQ1 = -iQ1;
          iQ2 = -iQ2;
          iQ3 = -iQ3;
          iSIGN = -1;	// signifies -ve
        }
        else 
        {
          iSIGN = 1;		// signifies +ve
        }


        // The derivation of Q is not specified in the standard :(

        // Let's try this approach ....

        // Q1 can be 0 to 4 only
        // Q1 1 to 4 and Q2 -4 to 4 and Q3 -4 to 4	= 4*9*9 = 324
        // Q1 0 and Q2 1 to 4 only and Q3 -4 to 4	= 1*4*9 = 36
        // Q1 0 and Q2 0 and Q3 0 to 4			= 1*1*5 = 5
        // total of 365
        // and 0,0,0 (Q == 360) only occurs for run mode or regular mode with sample interleaved

        unsigned short iQ;

        if (iQ1 == 0) 
        {
          if (iQ2 == 0) 
          {
            iQ = 360 + iQ3;		// fills 360..364
          }
          else 
          {	// Q2 is 1 to 4
            iQ = 324 + (iQ2 - 1) * 9 + (iQ3 + 4);	// fills 324..359
          }
        }
        else 
        {		// Q1 is 1 to 4
          iQ = (iQ1 - 1) * 81 + (iQ2 + 4) * 9 + (iQ3 + 4);	// fills 0..323
        }

        //Assert(Q<nContexts);	// Just in case

        // Figure A.5 Edge detecting predictor ...

        int iPx;	// Predicted value - not unsigned short to allow overrange before clamping

        if      (iRc >= Math::Max(iRa,iRb))	iPx = Math::Min(iRa,iRb);
        else if (iRc <= Math::Min(iRa,iRb))	iPx = Math::Max(iRa,iRb);
        else				                      iPx = (int)(iRa + iRb - iRc);


        // Figure A.6 Prediction correction and clamping ...

        iPx = iPx + ((iSIGN > 0) ? m_iC[iQ] : -m_iC[iQ]);



        iPx = CLAMP((int)0,iPx,(int)m_iMAXVAL);


        // Figure A.10 Prediction error Golomb encoding and decoding...

        unsigned short k = DetermineGolombParameter(m_iN[iQ],m_iA[iQ]);

        unsigned int MErrval;
        int Errval;
        int updateErrval;

        if ( eProcessType == ProcessType::DECOMPRESS )
        {

          // Decode Golomb mapped error from input...
          MErrval = DecodeMappedErrvalWithGolomb(k,m_iLIMIT);


          // Unmap error from non-negative (inverse of A.5.2 Figure A.11) ...
          Errval = ErrorMapping(k, iQ, MErrval);

          updateErrval=Errval;			// NB. Before dequantization and sign correction

          Dequantize(Errval);


          if (iSIGN < 0) Errval = -Errval;		// if "context type" was negative


          iRx = iPx + Errval;

          // modulo(RANGE*(2*m_iNEAR+1)) as per F.1 Item 14

          // (NB. Is this really the reverse of the encoding procedure ???)

          if (iRx < -m_iNEAR)
            iRx += m_iRANGE * ( 2 * m_iNEAR + 1 );
          else if ( iRx > ( (int)m_iMAXVAL + m_iNEAR ) )
            iRx -= m_iRANGE * ( 2 * m_iNEAR + 1 );

          iRx = CLAMP((int)0,iRx,(int)m_iMAXVAL);
       
          // Apply inverse point transform and mapping table when implemented

          pThisRow[iCol]=(unsigned short)iRx;
        }
        else 
        {	

          int iIx = pThisRow[iCol];	// Input value - not unsigned short to allow overrange before clamping

          Errval = iIx - iPx;		// watch this for bad unsigned->signed conversion :(


          if ( iSIGN < 0 ) Errval = -Errval;	// if "context type" was negative


          // For near-lossless, quantize Errval and derive reconstructed value (A.4.4)
          if ( m_iNEAR > 0 ) 
          {	

            Quantize(Errval);

            //////////////////////////////////////////////////////////////////
            // Replace with the reconstructed value with the decoder 
            // (obviously if in lossless mode there will be no difference)
            /////////////////////////////////////////////////////////////////
            iRx = iPx + iSIGN * Errval * ( 2 * m_iNEAR + 1 );
            iRx = CLAMP((int)0,iRx, (int)m_iMAXVAL);

            pThisRow[iCol]=(unsigned short)iRx;
          } // END .... If near-lossless

		      // Modulo reduction of the prediction error (A.4.5)
          ModuloErrorPredictionReduction(Errval);


          updateErrval=Errval;			// NB. After sign correction but before mapping

          // Prediction error encoding (A.5)

          // Golomb k parameter determined already outside decompress/compress test

          // Map Errval to non-negative (A.5.2) ...
          MErrval = ErrorMapping(k, iQ, Errval);

          EncodeMappedErrvalWithGolomb(k,m_iLIMIT,MErrval);
        }

        // Update variables (A.6) ...


        // A.6.1 Use the signed error after modulo reduction (figure A.12 note). which is updateErrval
        UpdateCurrentPredictionError(iQ, updateErrval);

        // A.6.2 Context dependent bias cancellation ...
        BiasComputation(iQ);

      }
    }

    if ( ProcessType::DECOMPRESS )
    {
      // TODO:
      //PutRow(pThisRow,m_iColumns, m_iBitsPerPixel);
    }

		unsigned short *tmpRow = pThisRow;
		pThisRow = pPrevRow;
		pPrevRow = tmpRow;

	}
	

} // ProcessImage()


/**
 * Maps the predication error value to a non-negative vlaue.
 *
 * FCD 14495 ... A.5.2
 *
 * The perdiction error, Errval shall be mapped to a non-negative value. For lossless coding the 
 * the mapping procedure checks the Golomb Parameter (k) and according to its value performs a 'regular
 * mapping' (k != 0), or a 'special mapping' (k == 0) and B[Q] less then or equal to -N[Q]/2, which
 * is equivalent to encoding -(Errval + 1) with the regualr mapping. For near-lossless coding, the mapping 
 * is independent of the Golomb parameters (k).
 *
 * @param iGolombParameter        The golomb parameter 
 * @param iQ                      Index into N amd B
 * @param iErrorValue             Current error value
 *
 * @return Returns the mapped error value.
 */
unsigned int Jpeg::ErrorMapping(unsigned short iGolombParameter, int iQ, unsigned short iErrorValue)
{

  unsigned int iMappedErrorValue(iErrorValue);
  
  if ( m_iNEAR == 0 && iGolombParameter == 0 &&  2 * m_iB[iQ] <= -m_iN[iQ] ) 
  {
    if ( iErrorValue >= 0 )
      iMappedErrorValue =  2 * iErrorValue + 1;	    //  0 becomes 1,  1 becomes 3,  2 becomes 5
    else
      iMappedErrorValue = -2 * (iErrorValue + 1);	  // -1 becomes 0, -2 becomes 2, -3 becomes 4
  } // END ... If lossless
  else 
  {
    if ( iErrorValue >= 0 )
      iMappedErrorValue =  2 * iErrorValue;		      //  0 becomes 0,  1 becomes 2,  2 becomes 4
    else
      iMappedErrorValue = -2 * iErrorValue - 1;	    // -1 becomes 1, -2 becomes 3
  } // END ... Else near-lossless

  return iMappedErrorValue;

} // ErrorMapping()

/**
 * Updates the current prediction error 
 *
 * FCD 14495 ... A.6.1
 *
 * The values A[Q], B[Q] and N[Q} are updated according to the current predication error value.
 *
 * @param iQ            current A, B and N index
 * @param iErrorValue   Updated error value
 *
 */
void Jpeg::UpdateCurrentPredictionError(int iQ, unsigned short iErrorValue)
{
  m_iB[iQ] = m_iB[iQ] + iErrorValue * ( 2 * m_iNEAR + 1 );
  m_iA[iQ] = m_iA[iQ] + iErrorValue;

  if ( m_iN[iQ] == m_iRESET ) 
  {
    m_iA[iQ] = m_iA[iQ] >> 1;
    m_iB[iQ] = m_iB[iQ] >> 1;
    m_iN[iQ] = m_iN[iQ] >> 1;
  }

  ++m_iN[iQ];

} // UpdateCurrentPredicationError()


/**
 * Bias computation
 *
 * FCD 14495 ... A.6.2
 *
 * The prediction coreection value C[Q] shall be updated as the end of the encoding of sample x. Two variables 
 * are needed for this procedure: B[Q} and N[Q}. The varaiable N[Q] is the number of occurences of context Q 
 * since the initialization. The bias variable B[Q] allows an update of the value of C[Q] by at most one unit 
 * every iteration. The variables are clamped to the limit their range of possible values. 
 *
 * @param iQ        Index int C, N, and B
 *
 * @return void.
 */
void Jpeg::BiasComputation(int iQ)
{

  if ( m_iB[iQ] <= -m_iN[iQ] ) 
  {
    m_iB[iQ] += m_iN[iQ];
    if ( m_iC[iQ] > m_iMIN_C ) 
    {
      --m_iC[iQ];
    }

    if ( m_iB[iQ] <= -m_iN[iQ] ) 
    {
      m_iB[iQ] = -m_iN[iQ] + 1;
    }
  }
  else if ( m_iB[iQ] > 0 ) 
  {
    m_iB[iQ] -= m_iN[iQ];
    if ( m_iC[iQ] < m_iMAX_C ) 
    {
      ++m_iC[iQ];
    }

    if ( m_iB[iQ] > 0 ) 
    {
      m_iB[iQ] = 0;
    }

  }

} // BiasComputation()



int Jpeg::RunScanning( unsigned short * pRow, int & iCol, unsigned short iRa)
{

  int iRunValue(iRa);
  int iRunCount(0);

  while ( iCol < m_iColumns && ( pRow[iCol] == iRunValue || 
         ( m_iNEAR > 0 && Math::Abs((int)pRow[iCol] - iRunValue) <= m_iNEAR) ) ) 
  {

    ++iRunCount;
	  if ( m_iNEAR > 0 )
    {
      pRow[iCol] = iRunValue;	// Replace with "reconstructed value"
    }

	  ++iCol;
  } // END ... while 

  return iRunCount;
} // RunScanning()


/**
 * Updates the LocalContext
 */
void Jpeg::UpdateLocalContext(int iRow, int iCol, unsigned short * pPrevRow, unsigned short * pCurrentRow, unsigned short & iRa, unsigned short & iRb )
{
  if ( iRow > 0 ) 
  {
	  iRb = pPrevRow[iCol];
	  iRa = (iCol > 0) ? pCurrentRow[iCol - 1] : iRb;
  }
  else 
  {
	  iRb = 0;
	  iRa = (iCol > 0) ? pCurrentRow[iCol - 1] : 0;
  }

} // UpdateRow()

/**
 * Computes the Rx
 */
int Jpeg::ComputeRx(int iPx, int iErrorValue)
{
  int iRx = iPx + iErrorValue;
  int iNear(m_iNEAR);
  int iAdjustment(iNear * 2 + 1);

	// modulo(m_iRANGE*(2*NEAR+1)) as per F.1 Item 14

	// (NB. Is this really the reverse of the encoding procedure ???)

	if ( iRx < -iNear )
  {
		iRx += m_iRANGE * iAdjustment;
  }
	else if ( iRx > (int)m_iMAXVAL + iNear)
  {
		iRx -= m_iRANGE * iAdjustment;
  }

	iRx = CLAMP((int)0,iRx,(int)m_iMAXVAL);

  return iRx;

} // ComputeRx()

/**
 * Modulo reduction of the prediction error
 *
 * FCD 14495 ... A.4.5
 *
 * The error shall be reduced by the range relevant for coding ( -[RANGE/2] ... [RANGE/2]-1).
 *
 * @param iErrorValue         Error value to reduce
 *
 * @return Returns the reduces error prediction value
 */
int Jpeg::ModuloErrorPredictionReduction(int iErrorValue)
{

  int iReducedValue(iErrorValue);

  if ( iReducedValue < 0 )
  {
    iReducedValue = iReducedValue + m_iRANGE;
  }

  if ( iReducedValue >= (m_iRANGE + 1) / 2 )	
  {
    iReducedValue = iReducedValue - m_iRANGE;
  }

  return iReducedValue;


} //  ModuloErrorPredictionReduction()


/**
 * Decompresses a JPEG image
 */
void Jpeg::Decompress()
{
  

  Log::Debug("Perfoming Decompression on the source image", "Jpeg", "Decompress");

  unsigned char byteRead(0);
  bool bSearchForEOI(false);

  /// Minimum marker flags required
  const int iMinFlags = (int)JpegMarkerFlag::SOI | (int)JpegMarkerFlag::SOF; 

  while ( ! m_pSource->EndOfStream )
  {

    /// Stores the SOF marker
    short iSofMarker(0);

    bool bIgnore(true);
    int iMarkerFlags(0);

    //////////////////////////////////////////////////////////////////////
    // Scans the jpg data for markers, until the SOS marker is found.
    /////////////////////////////////////////////////////////////////////
    for ( int i(0); ! m_pSource->EndOfStream /*&& i < 2000*/ && (iMarkerFlags & JpegMarkerFlag::SOS) != JpegMarkerFlag::SOS ; i++ )
    {

      // Read byte
      byteRead = GetChar();

      if ( byteRead == JpegMarker::STARTER )
      {

        // Read marker
        byteRead = GetChar();

        
        switch ( byteRead )
        {

        case JpegMarker::SOF3:
          iSofMarker = (short)byteRead;
          iMarkerFlags |= JpegMarkerFlag::SOF;
          //Log::Debug(S"Reading SOF");
          ReadSOF();
          break;

        case JpegMarker::SOF55:
          iSofMarker = (short)byteRead;
          iMarkerFlags |= JpegMarkerFlag::SOF;
          //Log::Debug(S"Reading SOF");
          ReadSOF();
          break;

        case JpegMarker::DHT:
          iMarkerFlags |= JpegMarkerFlag::DHT;
          //Log::Debug(S"Reading DHT");
          ReadDHT();
          break;

        case JpegMarker::SOI:
          bIgnore = false;
          iMarkerFlags |= JpegMarkerFlag::SOI;
          //Log::Debug(S"Reading SOI");
          ReadSOI();
          break;

        case JpegMarker::EOI:
          bSearchForEOI = false;
          //iMarkerFlags |= JpegMarkerFlag::EOI;
          //Log::Debug(S"Reading EOI");
          ReadEOI();
          break;

        case JpegMarker::SOS:
          iMarkerFlags |= JpegMarkerFlag::SOS;
          //Log::Debug(S"Reading SOS");
          ReadSOS();
          break;

        case JpegMarker::DQT:
          iMarkerFlags |= JpegMarkerFlag::DQT;
          //Log::Debug(S"Reading DQT");
          ReadDQT();
          break;

        case JpegMarker::DRI:
          iMarkerFlags |= JpegMarkerFlag::DRI;
          //Log::Debug(S"Reading DRI");
          ReadDRI();
          break;

        case JpegMarker::APP0:
          iMarkerFlags |= JpegMarkerFlag::APP;
          //Log::Debug(S"Reading APP");
          ReadAPP();
          break;

        case JpegMarker::COM:
          //int4 MARK_CHECK_THIS_OUT;
          //iMarkerFlags |= JpegMarkerFlag::COM;
          //Log::Debug(S"Reading COM");
          ReadCOM();
          break;

        case JpegMarker::TEM:
          //Log::Debug(S"Reading TEM");
          // Ignore
          break;

        default:

          if ( ! bSearchForEOI && ( ! bIgnore && byteRead != 0x00 ) )
          {
            throw new IOException(String::Format("Invalid marker found 0x{0:X2}", __box((int)byteRead)) );
          }
                 
        } // END ... Switch on marker

      } // END ... If a marker starter was found

    } // END ... for 2000 bytes look for proper markers 


    // If we are at the end of the stream we are done
    if ( m_pSource->EndOfStream ) break;

    //////////////////////////////////////////////
    // Check for the minimum required segments
    /////////////////////////////////////////////
    if ( (iMarkerFlags & iMinFlags) != iMinFlags )
    {
      throw new IOException("Invalid or corrupt JPEG, missing segments");
    } // END ... If we don not have the proper segment
    

    ///////////////////////////////////////
    // Perform the proper decompression
    ///////////////////////////////////////
    switch ( (JpegMarker)iSofMarker )
    {
    case JpegMarker::SOF3:                  // Lossless decompression
      DecompressLossless();
      break;

    case JpegMarker::SOF55:                 // Lossless-LS decompression
      DecompressLS();
      break;

    default:
      throw new System::Exception("Unsupported decompression");
      break;
    } // END ... switch
   

    // We are searching for the EOI again
    bSearchForEOI = true;

  } // END .... While we are not at the EndOfStream

 

} // Decompress()


/**
 * Decompresses lossless images (SOF3)
 */
void Jpeg::DecompressLossless()
{

  //Log::Debug(S"Performing a lossless decompression");

  //////////////////////////////////
  // Check sampling factor validity
  //////////////////////////////////
  for ( int i(0); i < m_iComponentsInScan; i++ )
  {
    if ( m_ScanComponentInfo[i].HorizontalSampleFactor != 1 || m_ScanComponentInfo[i].VerticalSampleFactor != 1 )
    {
      throw new System::Exception("Down sampling is not supported");
    }
  } // END ...for each component in SCAN



  //Log::Debug(S"Initializing the MCU");

  // Initializes the MCU information
  InitializeMcu();

  //Log::Debug(S"Initializing the Huffman Decoder");

  HuffmanDecoderInitialize();

  //Log::Debug(S"Decoding the image");

  DecodeImage();

} // DecompressLossless()

/**
 * Decompresses lossless image (SOF55)
 *
 * JPEG 2000 standard.
 */
void Jpeg::DecompressLS()
{

  // The process image will perform both the compression and decompression
  ProcessImage(ProcessType::DECOMPRESS);

  
  Flush();

} // DecompressLS()



/**
 * Calculate the predictor for pixel[row][col][iCurrentComponent],
 * i.e. pCurrentRow[col][iCurrentComponent]. It handles the all special 
 * cases at image edges, such as first row and first column
 * of a scan.
 *
 */
int Jpeg::Predict( int iRow, int iCol, int iCurrentComponent, MCU * pCurrentRow, MCU * pPreviousRow ) 
{    

  int iPredictor(0);

  register int iLeft, iUpper, iDiagnol, iLeftCol;                                
                                                                          
  iLeftCol = iCol - 1;                                                        
  if ( iRow == 0 ) 
  {                                                         
                                                                          
      ////////////////////////////////////////////////////////////////                                                               
      // The predictor of first pixel is (1<<(Pr-Pt-1), and the          
      // predictors for rest of first row are left neighbors.            
      ///////////////////////////////////////////////////////////////                                                                
      if ( iCol == 0 ) 
      {                                                      
        iPredictor = ( 1 << ( m_iPrecision - m_iPointTransformParameter - 1 ) );                                    
      }                                                                  
      else {                                                             
        iPredictor = pCurrentRow[iLeftCol][iCurrentComponent];                       
      }
  }                                                                     
  else 
  {                                                                
                                                                          
    ///////////////////////////////////////////////////////////////////////                                                              
    // The predictors of first column are upper neighbors.             
    // All other preditors are calculated according to psv.            
    //////////////////////////////////////////////////////////////////////                                                                
    iUpper = pPreviousRow[iCol][iCurrentComponent];                                    
    if ( iCol == 0 )
    {
      iPredictor = iUpper;                                             
    }
    else 
    {                                                             
        
      iLeft    = pCurrentRow[iLeftCol][iCurrentComponent];                               
      iDiagnol = pPreviousRow[iLeftCol][iCurrentComponent];                              
      switch ( m_iNEAR ) 
      {                                                  
      case 0:                                                      
        iPredictor = 0;                                      
        break;                                               
      case 1:                                                      
        iPredictor = iLeft;                                   
        break;                                               
      case 2:                                                      
        iPredictor = iUpper;                                  
        break;                                               
      case 3:                                                      
        iPredictor = iDiagnol;                                   
        break;                                               
      case 4:                                                      
        iPredictor = iLeft + iUpper - iDiagnol;                        
        break;                                               
      case 5:                                                      
        iPredictor = iLeft + ( ( iUpper - iDiagnol ) >> 1 );                 
        break;                                               
      case 6:                                                      
        iPredictor = iUpper + ( ( iLeft - iDiagnol ) >> 1 );                 
        break;                                               
      case 7:                                                      
        iPredictor = ( iLeft + iUpper ) >> 1;                        
        break;                                               
      default:
        throw new System::Exception("Undefined PSV");
        iPredictor = 0;                                      
      } // END ... swicth                                                              
    }                                                                 
  }   

  return iPredictor;
}


/**
 * Calculate the predictor for sample pCurrentRow[col][iCurrentComponent].
 * It does not handle the special cases at image edges, such 
 * as first row and first column of a scan. We put the special 
 * case checkings outside so that the computations in main
 * loop can be simpler. This has enhenced the performance
 * significantly.
 *
 * @param iCol                Current column
 * @param iCurrentComponent   Current component
 * @param pCurrentRow         Current row
 * @param pPreviousRow        Previous Row
 *
 * @return Returns the Prediction value
 */
int Jpeg::QuickPredict(int iCol, int iCurrentComponent, MCU * pCurrentRow, MCU * pPreviousRow )
{

  int iPredictor(0);

  register int iLeft, iUpper, iDiagnol, iLeftCol;

  //////////////////////////////////////////////////////
  // All predictor are calculated according to psv.
  //////////////////////////////////////////////////////
  switch ( m_iNEAR ) 
  {
  case 0:
    iPredictor = 0;
    break;
  case 1:         // Left
    iPredictor  = pCurrentRow[iCol - 1][iCurrentComponent];
    break;
  case 2:         // Upper
    iPredictor  = pPreviousRow[iCol][iCurrentComponent];
    break;
  case 3:         // Diagnol
    iPredictor  = pPreviousRow[iCol - 1][iCurrentComponent];
    break;
  case 4:
    iLeftCol    = iCol - 1;
    iUpper      = pPreviousRow[iCol][iCurrentComponent];
    iLeft       = pCurrentRow[iLeftCol][iCurrentComponent];
    iDiagnol    = pPreviousRow[iLeftCol][iCurrentComponent];
    iPredictor  = iLeft + iUpper - iDiagnol;
    break;
  case 5:
    iLeftCol    = iCol - 1;
    iUpper      = pPreviousRow[iCol][iCurrentComponent];
    iLeft       = pCurrentRow[iLeftCol][iCurrentComponent];
    iDiagnol    = pPreviousRow[iLeftCol][iCurrentComponent];
    iPredictor  = iLeft + ( ( iUpper - iDiagnol ) >> 1 );
    break;
  case 6:
    iLeftCol    = iCol  - 1;
    iUpper      = pPreviousRow[iCol][iCurrentComponent];
    iLeft       = pCurrentRow[iLeftCol][iCurrentComponent];
    iDiagnol    = pPreviousRow[iLeftCol][iCurrentComponent];
    iPredictor  = iUpper+((iLeft-iDiagnol)>>1);
    break;
  case 7:
    iLeftCol    = iCol-1;
    iUpper      = pPreviousRow[iCol][iCurrentComponent];
    iLeft       = pCurrentRow[iLeftCol][iCurrentComponent];
    iPredictor  = ( iLeft + iUpper ) >> 1;
    break;
  default:
    Log::Warning("Undefined PSV", "Jpeg" , "QuickPredict");
    iPredictor = 0;
  }

  return iPredictor;
}


/**
 * Initialize for a Huffman-compressed scan. This is invoked after reading the SOS marker.
 */
void Jpeg::HuffmanDecoderInitialize()
{

  for ( int i(0); i < m_iComponentsInScan ; i++ ) 
  {

      
    //  Make sure requested tables are present
    if ( m_HuffmanTable[m_ScanComponentInfo[i].DcTableNumber] == NULL ) 
    { 
      throw new System::Exception("Error: Use of undefined Huffman table");
    }

    ////////////////////////////////////////////////////////////////////////////
    // Compute derived values for Huffman tables. We may do this more than once 
    // for same table, but it's not a big deal
    ///////////////////////////////////////////////////////////////////////////
    m_HuffmanTable[m_ScanComponentInfo[i].DcTableNumber]->Fix();

    //m_HuffmanTable[m_ScanComponentInfo[i].DcTableNumber]->Dump("C:\\V-System\\Log\\HuffmanTable.txt");
  }

  // Initialize Restart Stuff
  m_iRestartInRows = (m_iRestartInterval / m_iColumns );
  m_iRestartRowsToGo = m_iRestartInRows;
  m_iNextRestartNumber = 0;
  m_iAvailableBits = 0;

} // HuffmanDecoderInitialize()

/**
 * Check for a restart marker & resynchronize decoder.
 */
void Jpeg::ProcessRestart()
{

  int c, nbytes;
  
  // Throw away any unused bits remaining in bit buffer
  nbytes = m_iAvailableBits / 8;
  m_iAvailableBits = 0;

  ////////////////////////////////
  // Scan for next JPEG marker
  ////////////////////////////////
  do 
  {

    /////////////////////////
    // skip any non-FF bytes
    /////////////////////////
    do 
    { 
      nbytes++;
      c = (int)GetChar();
    } while ( c != JpegMarker::STARTER );
      
    ////////////////////////////
    // skip any duplicate 0xFF
    ////////////////////////////
    do 
    {  
      // we don't increment nbytes here since extra FFs are legal
      c = (int)GetChar ();
    } while (c == JpegMarker::STARTER );

  } while ( c == 0 ); /* repeat if it was a stuffed FF/00 */

  if ( c != (JpegMarker::RST0 + m_iNextRestartNumber) ) 
  {
    throw new IOException("Corrupt JPEG data, invalid reset marker");
  }
  
  // Update restart state
  m_iRestartRowsToGo    = m_iRestartInRows;
  m_iNextRestartNumber  = (m_iNextRestartNumber + 1) & 7;

} // ProcessRestart()



/**
 * Initializes the MCU information
 */
void Jpeg::InitializeMcu()
{
  if ( m_iComponentsInScan == 1 )
  {
    m_iMcuMembership[0] = 0;
  }
  else
  {
    if ( m_iComponentsInScan > 4 )
    {
      throw new System::Exception("Too many components for interleaved scan");
    }

    for ( int i(0); i < m_iComponentsInScan; i++ )
    {
      m_iMcuMembership[i] = i;
    }
  }

  /// Calculate the MCU size
  const int iMcuSize(m_iComponentsInScan * sizeof(ComponentType));

  for ( register int i(0); i < m_iBufferSize; m_pBuffer1[i] = 0, m_pBuffer2[i] = 0, i++);

  for ( int i(0); i < m_iRows; i++ )
  {
    m_pMcuRow1[i] = (MCU)(m_pBuffer1 + i * iMcuSize);
    m_pMcuRow2[i] = (MCU)(m_pBuffer2 + i * iMcuSize);
  }
} // InitializeMcu()


/**
 * Decode the first raster line of samples at the start of the scan and at the beginning of each restart interval.
 * This includes modifying the component value so the real value, not the difference is returned.
 *
 * @param pCurrentRow         Current row being processed
 */
void Jpeg::DecodeFirstRow (MCU * pCurrentRow)
{

  const short iShiftOffset( 1 << ( m_iPrecision - m_iPointTransformParameter - 1 ) );

  ///////////////////////////////////////////////////////////////////
  // The start of the scan or at the beginning of restart interval.
  ///////////////////////////////////////////////////////////////////
  for ( int i(0); i < m_iComponentsInScan; i++ ) 
  {
    int iData(0);
    int iDecodeValue = HuffmanDecode(m_HuffmanTable[m_ComponentInfo[m_iMcuMembership[i]].DcTableNumber]);
    if ( iDecodeValue > 0 )
    {
      iData = GetBits(iDecodeValue);
      HuffmanTable::Extend(iData,iDecodeValue);
    }

    // Add the predictor to the difference.
    pCurrentRow[0][i] = iData + iShiftOffset;

  } // END ... for each scan component

    
  //////////////////////////////////
  // The rest of the first row
  /////////////////////////////////
  for ( register int iCol(1); iCol < m_iColumns; iCol++ ) 
  {

    for ( int i(0); i < m_iComponentsInScan; i++ ) 
    {
      int iData(0);
      int iDecodeValue = HuffmanDecode(m_HuffmanTable[m_ComponentInfo[m_iMcuMembership[i]].DcTableNumber]);
      if ( iDecodeValue > 0 )
      {
        iData = GetBits(iDecodeValue);
        HuffmanTable::Extend(iData,iDecodeValue);
      }

      /// Add the predictor to the difference.
      pCurrentRow[iCol][i] = iData + pCurrentRow[iCol - 1][i];
      
    } // END ... for each component in the SCAN

  } // END ... 

  if ( m_iRestartInRows) 
  {
    m_iRestartRowsToGo--;
  }

} // DecodeFirstRow()


/**
 * Taken from Figure F.16: extract next coded symbol from the source.  
 *
 * @param pHuffmanTable         Reference to the huffman table
 */
int Jpeg::HuffmanDecode(HuffmanTable * pHuffmanTable)
{
  int iDecodeValue(0);
                          
  ///////////////////////////////////////////////////////////////////////                                                                  
  //  If the huffman code is less than 8 bits, we can use the fast     
  //  table lookup to get its value.  It's more than 8 bits about      
  //  3-4% of the time.                                                
  ///////////////////////////////////////////////////////////////////////
  int iCode = PeekBits(8);
  const int iNumberOfBits = pHuffmanTable->NumberOfBits[iCode];

  if (  iNumberOfBits > 0 ) 
  {                                          
    FlushBits(iNumberOfBits);                                
    iDecodeValue = pHuffmanTable->Value[iCode];                                           
  }                                                                   
  else 
  {
    FlushBits(8);

    int i(8);
    for (  ; iCode > pHuffmanTable->MaxCode[i]; i++ ) 
    {
      int iBit = GetBit();
      iCode = (iCode << 1) | iBit;
    }                                                               
                                                                      
    // With garbage input we may reach the sentinel value l = 17.   
    if ( i > 16 ) 
    {
      Log::Warning("Corrupt JPEG data: bad Huffman code", "Jpeg" , "HuffmanDecode");
      //throw new Exception("Corrupt JPEG data: bad Huffman code");
      iDecodeValue = 0;
    } 
    else 
    {
      const int iIndex = pHuffmanTable->ValuePtr[i] + ((int)(iCode - pHuffmanTable->MinCode[i]));
      iDecodeValue = pHuffmanTable->HuffmanValue[iIndex];                      
    }                                                               
 
  }   

  return iDecodeValue;


} // HuffmanDecode()


/**
 * Fills the bit buffer bucket
 *
 * @param iBits       Number of bites required
 */
void Jpeg::FillBitBuffer( int iBits) 
{ 

  while ( m_iAvailableBits < m_iMinBitsInBuffer ) 
  {
    byte cByte = GetChar();
    
    ///////////////////////////////////////////////////////
    // If it's 0xFF, check and discard stuffed zero byte          
    //////////////////////////////////////////////////////
    if ( cByte == 0xFF ) 
    {                                                
      byte cByte2 = GetChar();

      if ( cByte2 != 0x00 ) 
      {                                              
        m_pSource->UnGetChar(cByte2);                                     
        m_pSource->UnGetChar(cByte);                                      
        cByte = 0;
      }                                                           
     } // END ... If 0xFF                                                 
        
     // OK, load c into BIT BUFFER 
     m_iBitBuffer = (m_iBitBuffer << 8) | cByte;
     m_iAvailableBits += 8;                                                  

  } // END ... 

} // FillBitBuffer()


/**
 * Puts a row of 24 bit pixels into the destination buffer
 *
 * @param pRow        Row to put into destination buffer
 */
void Jpeg::PutRow24(MCU * pRow)
{
  /////////////////////////
  // Loop through the row
  //////////////////////////
  for ( int iCol(0); iCol < m_iColumns; iCol++ )
  {
    m_pDestination->Put((unsigned char) pRow[iCol][0]);
    m_pDestination->Put((unsigned char) pRow[iCol][1]);
    m_pDestination->Put((unsigned char) pRow[iCol][2]);
  } // END ... For each column

} // PutRow12()

/**
 * Puts a row of 16 bit pixels into the destination buffer
 *
 * @param pRow        Row to put into destination buffer
 */                                 
void Jpeg::PutRow16(MCU * pRow)
{
  /////////////////////////
  // Loop through the row
  //////////////////////////
  for ( int iCol(0); iCol < m_iColumns; iCol++)
  {     
    m_pDestination->Put((unsigned short)pRow[iCol][0]);
  } // END ... For each column
} // PutRow16()     

/**
 * Puts a row of 8 bit pixels into the destination buffer
 *
 * @param pRow        Row to put into destination buffer
 */                                 
void Jpeg::PutRow8(MCU * pRow)
{  
  /////////////////////////
  // Loop through the row
  //////////////////////////
  for ( int iCol(0); iCol < m_iColumns; iCol++)
  {              
    m_pDestination->Put((unsigned char) pRow[iCol][0]);
  } // END ... for each column 
} // PutRow8()
                                                                           

/**
 * Decode the input stream. This includes modifying the component value so the real value, not the
 * difference is returned.
 */
void Jpeg::DecodeImage()
{

  MCU * pCurrentRow(m_pMcuRow1);
  MCU * pPreviousRow(m_pMcuRow2);

  //Log::Debug(S"Decoding the first row");

  //////////////////////////////////////////////////////////////////////////
  // Decode the first iRow of image. Output the ro and turn this row into 
  // a previous row for later predictor calculation.
  /////////////////////////////////////////////////////////////////////////         
  DecodeFirstRow(pCurrentRow);
  
  //Log::Debug(S"Writing the Decoded row to the destination buffer");

  PutRow(pCurrentRow);

  SwapMcu(&pPreviousRow, &pCurrentRow);

  ///////////////////////////////////////////////////////////////////////////
  // optimal case : 8 bit image, one iColor component, no restartInRows 
  ///////////////////////////////////////////////////////////////////////////
  if (  m_iDepth == 8 && m_iComponentsInScan == 1 && m_iRestartInRows == 0 ) 
  {

    int iCurrentComponent(0);
      
    int iComponentIndex(m_iMcuMembership[iCurrentComponent]);

    HuffmanTable * pHuffmanTable(m_HuffmanTable[m_ComponentInfo[iComponentIndex].DcTableNumber]);

    //if ( pHuffmanTable == NULL )
    //{
    //  throw new System::Exception("No huffman table found");
    //}

    for ( int iRow(1); iRow < m_iRows; iRow++ ) 
    {

      ////////////////////////////////////////////////////////
      // Upper neighbor is predictor for the first column
      // Section F.2.2.1: decode the difference 
      ////////////////////////////////////////////////////////
      int iData(0);
      int iDecodeValue(HuffmanDecode(pHuffmanTable));
      if ( iDecodeValue > 0 ) 
      { 
        iData = GetBits(iDecodeValue);
        HuffmanTable::Extend(iData, iDecodeValue);
      }

      Put((byte)(iData + m_pDestination->Char[m_pDestination->Position - m_iColumns]));

      if ( m_iNEAR == 1 ) 
      {

        /////////////////////////////////
        // Loop through all the columns
        /////////////////////////////////
        for ( register int iCol(1); iCol < m_iColumns; iCol++ ) 
        {
          int iData(0);
          int iDecodeValue = HuffmanDecode(pHuffmanTable);
          if ( iDecodeValue > 0 ) 
          { 
            iData = GetBits(iDecodeValue);
            HuffmanTable::Extend(iData, iDecodeValue);
          } // END ... If there is a Decode Value

          Put((byte)(iData + m_pDestination->Char[m_pDestination->Position - 1]));

        } // END ... for each column

      } /// END ... If the PSV is 0
      else 
      {

        // 
        for ( register int iCol(1); iCol < m_iColumns; iCol++ )  
        {
          int iData(0);
          int iDecodeValue = HuffmanDecode(pHuffmanTable);
          if ( iDecodeValue > 0 ) 
          { 
            iData = GetBits(iDecodeValue);
            HuffmanTable::Extend(iData, iDecodeValue);
          }

          int iPredictor(0);
          int iUpper(0);
          int iLeft(0);
          int iDiagnol(0);

          // Predict : All predictors are calculated according to PSV
          switch ( m_iNEAR ) 
          {
          case 0: 
            iPredictor = 0;
            break;
          case 1:           // Left
            iPredictor = m_pDestination->Char[m_pDestination->Position - 1];         
            break;
          case 2:           // Upper 
            iPredictor = m_pDestination->Char[m_pDestination->Position - m_iColumns];         
            break;
          case 3:           // Diagnol
            iPredictor = m_pDestination->Char[m_pDestination->Position - m_iColumns - 1];     
            break;
          case 4: 
            iUpper     = m_pDestination->Char[m_pDestination->Position - m_iColumns];
            iLeft      = m_pDestination->Char[m_pDestination->Position - 1];
            iDiagnol   = m_pDestination->Char[m_pDestination->Position - m_iColumns - 1];
            iPredictor = iLeft + iUpper - iDiagnol;         
            break;
          case 5: 
            iUpper     = m_pDestination->Char[m_pDestination->Position - m_iColumns];
            iLeft      = m_pDestination->Char[m_pDestination->Position - 1];
            iDiagnol   = m_pDestination->Char[m_pDestination->Position - m_iColumns - 1];
            iPredictor = iLeft+((iUpper-iDiagnol)>>1);          
            break;
          case 6: 
            iUpper     = m_pDestination->Char[m_pDestination->Position - m_iColumns];
            iLeft      = m_pDestination->Char[m_pDestination->Position - 1];
            iDiagnol   = m_pDestination->Char[m_pDestination->Position - m_iColumns - 1];
            iPredictor = iUpper+((iLeft-iDiagnol)>>1);      
            break;
          case 7: 
            iUpper     = m_pDestination->Char[m_pDestination->Position - m_iColumns];
            iLeft      = m_pDestination->Char[m_pDestination->Position - 1];
            iPredictor = (iLeft+iUpper)>>1;                 
            break;
          default : 
            iPredictor = 0;
          }  // END ... switch on PSV

          m_pDestination->Put((unsigned char)(iData + iPredictor));
        } // END ... for each column

      } // END ... Else if 
          
    }  // END ... for each row

  }  // END ... If this is a fast case
  else 
  { 
    //////////////////////////////////////////////////////////////////////
    // normal case with 16 bits or Color (24 bit / 3 planes) or ...
    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    // Loop through all the rows skipping the first since it was already processed
    /////////////////////////////////////////////////////////////////////////////////
    for( register int iRow(1); iRow < m_iRows; iRow++ ) 
    {

      ////////////////////////////////////////////////////////////////////
      // Account for restart interval, process restart marker if needed.
      ///////////////////////////////////////////////////////////////////
      if ( m_iRestartInRows) 
      {

        if ( m_iRestartRowsToGo == 0 ) 
        {

          // Processes a restart
          ProcessRestart();
   
          ////////////////////////////////////////////
          // Reset predictors at restart.
          ////////////////////////////////////////////
          DecodeFirstRow(pCurrentRow);

          PutRow(pCurrentRow);

          SwapMcu(&pPreviousRow, &pCurrentRow);

          continue;
        } // END ... If 

        m_iRestartRowsToGo--;

      }  // END ... If there are restarte

      /////////////////////////////////////////////////////////////
      // The upper neighbors are predictors for the first column
      /////////////////////////////////////////////////////////////
      for ( int i(0); i < m_iComponentsInScan; i++ ) 
      {

        int iData(0);
        int iDecodeValue = HuffmanDecode(m_HuffmanTable[m_ComponentInfo[m_iMcuMembership[i]].DcTableNumber]);
        if ( iDecodeValue > 0 ) 
        { 
          iData = GetBits(iDecodeValue);
          HuffmanTable::Extend(iData, iDecodeValue);
        }

        pCurrentRow[0][i] = iData + pPreviousRow[0][i];

      } // END ... for each component in scan
        
      ///////////////////////////////////////
      // Loop through the remaining columns
      ///////////////////////////////////////
      for ( register int iCol(1); iCol < m_iColumns; iCol++ ) 
      {

        ////////////////////////////////
        // Loop through each component
        ////////////////////////////////
        for ( int i(0); i < m_iComponentsInScan; i++ ) 
        {

          int iData(0);
          int iDecodeValue = HuffmanDecode(m_HuffmanTable[m_ComponentInfo[m_iMcuMembership[i]].DcTableNumber]);
          if ( iDecodeValue > 0 ) 
          { 
            iData = GetBits(iDecodeValue);
            HuffmanTable::Extend(iData, iDecodeValue);
          }

          pCurrentRow[iCol][i] = iData + QuickPredict(iCol, i, pCurrentRow, pPreviousRow);

        } // END ... for each component in scan

      } // END ... For each column 

      // Stuff the ROW into the destination buffer
      PutRow(pCurrentRow);

      // Swap the rows
      SwapMcu(&pPreviousRow, &pCurrentRow);
      
    } // END ... for each row

  } // END ... Else if not fast case

} // DecodeImage


/**
 * Sets the depth
 */
void Jpeg::set_Depth(int iDepth)
{
  ///////////////////////
  // Validate the depth
  //////////////////////
  if ( iDepth != 8 && iDepth != 12 && iDepth != 24 && iDepth != 16 )
  {
    throw new System::Exception(String::Format("Unsupported Image depth [{0}]",__box(iDepth)));
  } // END ... Validate image depth

  m_iDepth = iDepth;

  m_iDepthIndex = System::Math::Ceiling((float)m_iDepth / 8.0F) - 1;

} // set_Depth()

// $Log: Jpeg.cpp,v $
// Revision 1.9.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9  2006/06/01 19:05:39  mkontak
// VS 2005 conversion
//
// Revision 1.8  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.7  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.6  2005/07/01 12:39:08  vxconfig
// Added new Logger.
//
// Revision 1.5  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.4  2004/08/13 19:20:57  mkontak
// Fixed 12 bit decompression
//
// Revision 1.3.2.1  2004/08/13 19:22:33  mkontak
// Fixed 12 bit decompression
//
// Revision 1.4  2004/08/13 19:20:57  mkontak
// Fixed 12 bit decompression
//
// Revision 1.3  2004/06/29 18:01:08  mkontak
// Fixed decompress issue intrduced when moving from 7.0 to 7.1
//
// Revision 1.2  2004/03/05 17:04:54  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1  2004/03/02 20:02:52  geconomos
// Merged Viatronix.v3D.Logger, Viatronix.v3D.Imaging, Viatronix.v3D.Threading and Viatronix.v3D.Licensing into Viatronix.Utilities.
//
// Revision 1.1.1.1  2004/03/02 03:58:37  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/11/14 20:04:31  mkontak
// Fixes
//
// Revision 1.4  2003/10/07 12:24:22  mkontak
// Fix for uncompression
//
// Revision 1.3  2003/07/15 14:50:54  mkontak
// Fixes
//
// Revision 1.2  2003/07/01 20:22:16  mkontak
// Fix for 16 bot decompression
//
// Revision 1.1  2003/06/26 20:27:22  mkontak
// Initial check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Jpeg.cpp,v 1.9.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: Jpeg.cpp,v 1.9.2.1 2011/05/18 01:56:58 mkontak Exp $

