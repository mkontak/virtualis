// $Id: Jpeg.h,v 1.3 2004/06/29 18:01:08 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pagmas
#pragma once

#include "ImageBuffer.h"

// Tpedefs
typedef short ComponentType;    // the type of image components
typedef ComponentType *MCU;     // MCU - array of samples
typedef unsigned char byte;

// Defines
#define CLAMP(x,y,z)  ( y < x ? x : ( y > z ? z : y ) )

// Namespaces
OPEN_IMAGING_NS

// Forward Declarations
public __gc class HuffmanTable;

// Delegates used to allows for flexibility and less conditionals in the critacal paths
private __delegate void PutRowDelegate(MCU * pRow);

// Compression Type
public __value enum CompressionType { LOSSLESS, LOSSLESS_LS };

// Buffer Type for source image so that I do not need to reallocate
public __value enum BufferType { MANAGED = 0, UNMANAGED = 1 };

#ifdef ERROR
#undef ERROR
#endif

// JPEG Marker ID's
public __value enum JpegMarker 
{ 
  STARTER = 0xff,

  SOF0    = 0xc0,
  SOF1    = 0xc1,
  SOF2    = 0xc2,
  SOF3    = 0xc3,              // Lossless compression
  SOF5    = 0xc5,
  SOF6    = 0xc6,
  SOF7    = 0xc7,

  JPG     = 0xc8,
  SOF9    = 0xc9,
  SOF10   = 0xca,
  SOF11   = 0xcb,
  SOF13   = 0xcd,
  SOF14   = 0xce,
  SOF15   = 0xcf,

  DHT     = 0xc4,
  DAC     = 0xcc,

  RST0    = 0xd0,
  RST1    = 0xd1,
  RST2    = 0xd2,
  RST3    = 0xd3,
  RST4    = 0xd4,
  RST5    = 0xd5,
  RST6    = 0xd6,
  RST7    = 0xd7,

  SOI     = 0xd8,
  EOI     = 0xd9,
  SOS     = 0xda,
  DQT     = 0xdb,
  DNL     = 0xdc,
  DRI     = 0xdd,
  DHP     = 0xde,
  EXP     = 0xdf,

  APP0    = 0xe0,
  APP15   = 0xef,

  JPG0    = 0xf0,
  JPG13   = 0xfd,
  COM     = 0xfe,                     // Comment, may be ignored

  // New for JPEG-LS (14495-1:1997)
  SOF55   = 0xf7,                   // Lossless-LS
  LSE     = 0xf8,

  TEM     = 0x01,                     // usually causes a decoding error, may be ignored 

  ERROR   = 0x100

};  // enum JpegMarker



/// LSE Id enumerator
public __value enum LseId { L1   = 0x01, L2   = 0x02, L3   = 0x03, L4   = 0x04 };

/// Image Enum used to detarmine the SOURCE and DEST image buffers
private __value enum Image { SOURCE, DEST };

/// Process Type (Used on Lossless-LS)
private __value enum ProcessType { COMPRESS, DECOMPRESS };

/// Mode of operation
public __value enum Mode { REGULAR, RUN };

/// Predictor Location
private __value enum PredictorLocation { UPPER, LEFT, DIAGNAL };

/**
 * Implements a JPEG image for compression and decompression
 */
public __gc class Jpeg : public System::IDisposable
{

protected:

  // JPEG Marker Flags
  __value enum JpegMarkerFlag 
  {  
    SOF  = 0x01,  
    DHT  = 0x02,
    SOI  = 0x04,
    SOS  = 0x08,
    DQT  = 0x10,
    APP  = 0x20,
    DRI  = 0x40,
    EOI  = 0x80
  };  // enum JpegMarkerFlag

public:

  /// Constructor
  Jpeg(Viatronix::Imaging::ImageBuffer * pSource, short iDepth);

  /// Constructor
  Jpeg(Viatronix::Imaging::ImageBuffer * pSource);

  /// Destructor
  virtual ~Jpeg();

  /// Compress image
  int Compress(CompressionType eType, Viatronix::Imaging::ImageBuffer * pDestination);

  /// Decompresses image
  int Decompress(Viatronix::Imaging::ImageBuffer * pDestination);

  /// Dispose of object
  void Dispose();

private:


  ///
  void Compress(CompressionType eType);

  /// Compresses image lossless ls
  void CompressLosslessLs();

  /// Compress image lossless
  void CompressLossless();

  /// Compress image lossless
  void CompressLossless2();

  /// Decompresses image
  void Decompress();

  /// Decompresses lossless images (SOF3)
  void DecompressLossless();

  /// Decompresses lossless-LS (JPEG 2000)
  void DecompressLS();

  /// Dispose of object
  void Dispose(bool bDisposing);

  /// Bit Count
  int GetBitCount(int iValue);

public:

  /// Returns the source image
  __property ImageBuffer * get_SourceImage() { return m_pSource; }

  /// Returns the destination image
  __property ImageBuffer * get_DestinationImage() { return m_pDestination; }

  /// Returns the source image size
  //__property int get_SourceImageSize() { return m_iImageSize[(int)Image::SOURCE]; }

  /// Returns the destination image szie
  //__property int get_DestinationImageSize() { return m_iImageSize[(int)Image::DEST]; }



  /// Returns the number of rows
  __property int get_Rows() { return m_iRows; }

  /// Sets the number of rows
  __property void set_Rows(int iRows) { m_iRows = iRows; }

  /// Returns the number of Columns
  __property int get_Columns() { return m_iColumns; }

  /// Sets the columns
  __property void set_Columns(int iColumns) { m_iColumns = iColumns; }

  /// Returns the reset
  __property unsigned short get_Reset() { return m_iRESET; }

  /// Sets the reset
  __property void set_Reset(unsigned short iReset) { m_iRESET = iReset; }

  /// Returns threshold 1
  __property unsigned short get_Threshold1() { return m_iThreshold[0]; }

  /// Sets threshold 1
  __property void set_Threshold1(unsigned short iThreshold) { m_iThreshold[0] = iThreshold; }

   /// Returns threshold 2
  __property unsigned short get_Threshold2() { return m_iThreshold[1]; }

  /// Sets threshold 2
  __property void set_Threshold2(unsigned short iThreshold) { m_iThreshold[1] = iThreshold; }

  /// Returns threshold 3
  __property unsigned short get_Threshold3() { return m_iThreshold[2]; }

  /// Sets threshold 3
  __property void set_Threshold3(unsigned short iThreshold) { m_iThreshold[2] = iThreshold; }

  /// Returns the bits per pixel
  __property unsigned short get_BitsPerPixel() { return m_iBitsPerPixel; }

  /// Sets the bits per pixel
  __property void set_BitsPerPixel(unsigned short iBitsPerPixel) { m_iBitsPerPixel = iBitsPerPixel; }

  /// Returns the use markers flag
  __property bool get_UseMarkers() { return m_bUseMarkers; }

  /// Sets the use markers flag
  __property void set_UseMarkers(bool bUse) { m_bUseMarkers = bUse; }

  /// Returns the mode
  __property Viatronix::Imaging::Mode get_Mode() { return m_eMode; }

  /// Sets the mode
  __property void set_Mode(Viatronix::Imaging::Mode eMode) { m_eMode = eMode; }


  /// Resturns the Depth
  __property int get_Depth() { return m_iDepth; }

  /// Sets the image depth
  __property void set_Depth(int iDepth);

private:

 /*
  * The following structure stores basic information about one component.
  */
  __value struct ComponentInformation 
  {
     ///////////////////////////////////////////////////////////////////////////////////
     // These values are fixed over the whole image. They are read from the SOF marker.
    ////////////////////////////////////////////////////////////////////////////////////
    short ComponentId;                      // identifier for this component (0..255) 
    short ComponentIndex;                   // its index in SOF or cPtr->compInfo[]   

    ///////////////////////////////////////////////////////////////////////
    // Downsampling is not normally used in lossless JPEG, although
    // it is permitted by the JPEG standard (DIS). We set all sampling 
    // factors to 1 in this program.
    //////////////////////////////////////////////////////////////////////
    short HorizontalSampleFactor;           // horizontal sampling factor
    short VerticalSampleFactor;             // vertical sampling factor  

    //////////////////////////////////////////////////////////////////
    // Huffman table selector (0..3). The value may vary
    // between scans. It is read from the SOS marker.
    /////////////////////////////////////////////////////////////////
    short DcTableNumber;

  };  // struct ComponentInformation

  /**
   * Node
   */
  __gc class Node : public System::IComparable
  {
  public:
    Node(int iValue, int iCount) : m_pRoot(NULL), m_pLeft(NULL), m_pRight(NULL), m_iValue(iValue), m_iCount(iCount), m_iCodeSize(0) {}
    Node(Node * pLeft, Node * pRight, int iCount) : m_pRoot(NULL), m_pLeft(pLeft), m_pRight(pRight), m_iValue(0), m_iCount(iCount), m_iCodeSize(0)  { m_pRight->Root = this; m_pLeft->Root = this;  }

    virtual ~Node() {}

    inline int CompareTo(System::Object * pObject) 
    {
      Node * pNode = dynamic_cast<Node *>(pObject);

      if ( m_iCount == pNode->Count )
        return 0;
      else if ( m_iCount < pNode->Count )
        return -1;
      else 
        return 1;
    }

    __property Node * get_Root() { return m_pRoot; }
    __property void set_Root(Node * pRoot) { m_pRoot = pRoot; }

    __property Node * get_Left() { return m_pLeft; }
    __property void set_Left(Node * pLeft) { m_pLeft = pLeft; }

    __property Node * get_Right() { return m_pRight; }
    __property void set_Right(Node * pRight) { m_pRight = pRight; }

    __property int get_Count() { return m_iCount; }
    __property void set_Count(int iCount) { m_iCount = iCount; }

    __property int get_Value() { return m_iValue; }
    __property void set_Value(int iValue) { m_iValue = iValue; }

    __property int get_CodeSize() { return m_iCodeSize; }
    __property void set_CodeSize(int iCodeSize) { m_iCodeSize = iCodeSize; }

    __property int get_Bits() { return m_iBits; }
    __property void set_Bits(int iBits) { m_iBits = iBits; }

  private:
    Node * m_pRoot;
    Node * m_pLeft;
    Node * m_pRight;
    int m_iCodeSize;
    int m_iCount;
    int m_iValue;
    int m_iBits;
  }; // class Node

  /// Initialize the data members to their defaults
  void Init();

  /// Reads the JPEG marker
  bool ReadMarker(unsigned short & iMarker);

  /// Reads the SOI header
  inline void ReadSOI() { m_iRestartInterval = 0; }

   /// Reads the EOI header
  inline void ReadEOI() { }

  /// Reads the DRI header
  inline void ReadDRI();

  /// Read SOF header
  void ReadSOF();

  /// Read SOS header
  void ReadSOS();

  /// Read LSE1 header
  void ReadLSE1();

  /// ReadDHT
  void ReadDHT();

  /// Reads the DeQuantization Table (DQT) header
  void ReadDQT();

  /// Reads the comment
  void ReadCOM();

  /// Reads the APP
  void ReadAPP();


  /// Put a marker to the destination image
  inline void Put(JpegMarker eMarker) { m_pDestination->Put((unsigned short)eMarker); }
  
  /// Put a marker to the destination image
  inline void Put(LseId eId) { m_pDestination->Put((unsigned char)eId); }

  /// Puts SIO marker
  inline void PutSOI() { Put(JpegMarker::SOI); }

  /// Puts the SOF55 header
  void PutSOF();

  /// Puts the SOS header
  void PutSOS();

  /// Puts the LSE header
  void PutLSE1();

  /// Put the EOI marker
  inline void PutEOI() { Put(JpegMarker::EOI); }

  /// Put the DHT marker
  void PutDHT();

  /// Puts a row into the dest image
  //void PutRow(unsigned short * pBuffer, unsigned int iNumberOfPixels, unsigned short iBitsPerPixel );

  /// Decodes the mapped error values with the golomb parameters (A.5.3)
  unsigned int DecodeMappedErrvalWithGolomb(unsigned short k, unsigned short iGolumbLimit);

  /// Encodes the mapped error values with the golomb parameters (A.5.3)
  void EncodeMappedErrvalWithGolomb(unsigned short k, unsigned short iGolumbLimit, unsigned int  value);


  /// Determine the golomb variable (A.5.1)
  unsigned short DetermineGolombParameter(unsigned int n, unsigned int a);

  ///
  void Quantize(int & Errval);

  ///
  void Dequantize(int & Errval);

  /// 
  void ZigZagReorder(int * pCoefficients);


  ///
  void CodecRunEndSample(unsigned short &Ix,int Ra,int Rb, ProcessType eProcessType );

  /// Initialzie all counter arrays
  void InitializeCounters();

  /// Process image
  void ProcessImage(ProcessType eProcessType);

  /// Maps the error value to a non-negative value (A.5.2)
  unsigned int ErrorMapping(unsigned short iGolombParameter, int iQ, unsigned short iErrorValue);

  /// Updates the current prediction (A.6.1)
  void UpdateCurrentPredictionError(int iQ, unsigned short iErrorValue);

  /// Bias computation (A.6.2)
  void BiasComputation(int iQ);

  /// Returns the run count(Run scanning - A.7.1.1) 
  int RunScanning( unsigned short * pRow, int & iCol, unsigned short iRa);

  /// UpdateLocalContext Row
  void UpdateLocalContext(int iRow, int iCol, unsigned short * pPrevRow, unsigned short * pCurrentRow, unsigned short & iRa, unsigned short & iRb );

  /// Compute the Rx value
  int ComputeRx(int iPx, int iErrorValue);

  /// Modulo Reduction of the Erro values Prediction (A.4.5)
  int ModuloErrorPredictionReduction(int iErrorValue);

  /// Huffman decode
  int HuffmanDecode(HuffmanTable * pHuffmanTable);

  /// Bounds the value (int)
  __forceinline int Bound(int iLow, int iValue, int iHigh) { return ( iValue < iLow ? iLow : ( iValue > iHigh ? iHigh : iValue ) ); }

  /// Bounds the value (unsigned int)
  __forceinline unsigned int Bound(unsigned int iLow, unsigned int iValue, unsigned int iHigh) { return ( iValue < iLow ? iLow : ( iValue > iHigh ? iHigh : iValue ) ); }

  /// Desscale
  __forceinline int Descale( int iX, int iN) { return ((iX + ((long)1 << (iN - 1))) >> iN); }

  /// Initialize Quantization Tables
  void InitializeQuantizationTables();

  /// Returns the predictor
  int Predict( int iRow, int iCol, int iCurrentComponent, MCU * pCurrentRow, MCU * pPreviousRow );

  /// Returns the quick predictor
  int QuickPredict(int iCol, int iCurrentComponent, MCU * pCurrentRow, MCU * pPreviousRow );

  /// Huffman decoder initialize 
  void HuffmanDecoderInitialize(void);

  /// Processes a restart RST
  void ProcessRestart(void);

  /// Initializes the MCU information
  void InitializeMcu(void);

  /// Fills the bit buffer to extract
  void FillBitBuffer(int iBits);

  /// Decodes the first row in the buffer
  void DecodeFirstRow (MCU * pCurrentRow);

  /// Decodes the image
  void DecodeImage();

  /// Swaps the MCU pointers
  __forceinline void SwapMcu(MCU ** pMcu1, MCU ** pMcu2 ) { MCU * pTmp(*pMcu1); *pMcu1 = *pMcu2; *pMcu2 = pTmp; }

  /// Returns the value of the bits requested without advancing the bit buffer index
  __forceinline int PeekBits(int iBits)  { if ( m_iAvailableBits < iBits) FillBitBuffer(iBits); return ( m_iBitBuffer >> ( m_iAvailableBits - iBits)) & m_iBitMask[iBits]; }

  /// Returns the value of the next 8 bits requested without advancing the bit buffer index
  __forceinline int PeekBits8() { return PeekBits(8); }

  /// Advances the bit buffers index
  __forceinline void FlushBits(int iBits) { m_iAvailableBits -= iBits; }

  /// Returns the value of the bits requested and advances the bit buffer index
  __forceinline int GetBits(int iBits) {if ( m_iAvailableBits < iBits) FillBitBuffer(iBits); return ( m_iBitBuffer >> ( m_iAvailableBits -= iBits)) & m_iBitMask[iBits]; }

  /// Returns the value of the next available bit 
  __forceinline int GetBit() { if ( m_iAvailableBits <= 0 ) FillBitBuffer(1); return ((m_iBitBuffer >> (--m_iAvailableBits)) & 1); }                                                 \

  /// Puts a row of 16 bit values into the destination image buffer
  void PutRow16(MCU * pRow);

  /// Puts a rows of 8 bit values into the destination image buffer
  void PutRow8(MCU * pRow);

  /// Puts a row of 24 bit values into the destination image buffer
  void PutRow24(MCU * pRow);

  /// Puts a Row into the destintaion buffer
  __forceinline void PutRow(MCU * pRow) { m_pPutRow[m_iDepthIndex]->Invoke(pRow); }

  /// Returns a character from the source inage buffer
  __forceinline unsigned char GetChar(void) { return m_pSource->GetChar(); }

  /// Returns a character from the source inage buffer
  __forceinline unsigned char Peek(int iOffset) { return m_pSource->Peek(iOffset); }

  /// Puts bit to destination image
  void PutBit(unsigned int iBit);

  /// Put character to destination
  __forceinline void Put(unsigned char cByte) { m_pDestination->Put(cByte); }

  /// Flush remaining bits to the dest image
  void Flush();

private:

  /// Minimum allowed value for C[0...364]
  static int m_iMIN_C = -128;

  /// Maximum alloed value for C[0...364]
  static int m_iMAX_C = 127;

  static int m_iMinPrecisionBits = 2;
  static int m_iMaxPrecisionBits = 16;
  static int m_iMinPrecisionValue = 2;
  static int m_iMaxPrecisionValue = 65535;

  static int m_iMinBitsInBuffer = (( 8 * sizeof(long) ) - 7 );

  /// Put Row delegates
  PutRowDelegate * m_pPutRow __gc [];


  /// Depth (8, 12, 24) in bits of the image
  int m_iDepth;

  /// Depth index for delgate chosing ( (m_iDepth / 8) - 1 )
  int m_iDepthIndex;

  /// Range
  unsigned short m_iRANGE;

  /// Number of rows in image (SOF)
  int m_iRows;

  /// Number of Columns in the image (SOF)
  int m_iColumns;

  /// Bits per pixel 
  unsigned short m_iBitsPerPixel;

  /// Restart Interval (DRI)
  int m_iRestartInterval;

  /// Restart Rows
  int m_iRestartRows;

  /// Restart in Rows
  int m_iRestartInRows;

  /// Restart Rows to go (remaining)
  int m_iRestartRowsToGo; 

  /// Next restart number
  int m_iNextRestartNumber;

  /// Precision
  unsigned short m_iPrecision;
 
  /// Thresholds (Used in Lossless-LS)
  unsigned short m_iThreshold __gc [];

  /// Reset 
  unsigned short m_iRESET;

  /// Max Value
  unsigned int m_iMAXVAL;

  /// Near or PSV (NEAR: 0 = Lossy, 1 = Lossless)
  int m_iNEAR;

  /// Components in SOF
  ComponentInformation m_ComponentInfo __nogc [4];

  /// Number of components in SOF
  int m_iNumberOfComponents;

  /// SOS
  ComponentInformation m_ScanComponentInfo __nogc [4];

  /// SOS
  int m_iComponentsInScan;

  /// Point Transform Parameter
  int m_iPointTransformParameter;

  ///  Horizontal sampling
  int m_iYSampling;

  /// Vertical Sampling
  int m_iXSampling;

  /// Quantization table Index for sampling
  int m_iQuantizationSamplingIndex;

  /// Quantization table Index for cbcr
  int m_iQuantizationCbCrIndex;

  /// Flag to indicate the use of markers
  bool m_bUseMarkers;

  /// Run Index
  int m_iRunIndex;

  /// Quantized Bits Per Pixel (qbpp)
  unsigned short m_iQuantizedBitsPerPixel;

  /// Golumb Limit (LIMIT)
  unsigned short m_iLIMIT;

  /// Available bits in the BIT BUFFER
  short m_iAvailableBits;

  int m_iReadBitByteOffset;

  int m_iPutBitByteOffset;

  int m_iPutBitCount;

  unsigned char m_PutBitByte;

  /// Marker Flags
  int m_iMarkerFlags;

  /// Mode of operation
  Viatronix::Imaging::Mode m_eMode;

  /// Number of contexts ( 2 will be added when allocating )
  static const int m_iContexts = 365;

  /// Context Type Counters                  - N[0...366]
  int m_iN __gc [];

  /// Accumulated Prdiction Magnitude         - A[0...366]
  unsigned int m_iA __gc [];

  /// Auxillary Counters for bias correction  - B[0...364] 
  int m_iB __gc [];

  /// Bias Correction Counter                 - C[0...364]
  int m_iC __gc []; 

  /// Negative Predication Error              - Nn[365-366]
  int m_iNn __gc []; 

  /// Huffman Tables
  HuffmanTable * m_HuffmanTable __gc [];

  /// Huffman Node Table
  int m_iHuffmanCount __nogc [256];

  System::Collections::ArrayList * m_pHuffmanTable;

  /// Root of binary huffman tree
  Node * m_pRoot;

  /// Dequantization Table
  int m_DequantizationTable __nogc [5] __nogc[128];

  /// Order run length codes
  static const unsigned short J __nogc [32] = 
  {	0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,10,11,12,13,14,15 };

  /// Length of run length codes (ie. 1<<J[n])
  static const unsigned short J_rm __nogc [32] = 
  {	1u<<0,1u<<0,1u<<0,1u<<0,
	  1u<<1,1u<<1,1u<<1,1u<<1,
	  1u<<2,1u<<2,1u<<2,1u<<2,
	  1u<<3,1u<<3,1u<<3,1u<<3,
	  1u<<4,1u<<4,
	  1u<<5,1u<<5,
	  1u<<6,1u<<6,
	  1u<<7,1u<<7,
	  1u<<8,
	  1u<<9,
	  1u<<10,
	  1u<<11,
	  1u<<12,
	  1u<<13,
	  1u<<14,
	  1u<<15
  };

  // BitMask[n] is mask for n rightmost bits
  static int m_iBitMask __nogc [] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 
                                      0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };


  /// Flag to indicate that the object has been disposed
  bool m_bDisposed;
 

  /// The MCU table that buffers the source image
  MCU * m_pMcuTable; 
  
  /// Number of MCU's in the table
  int m_iNumberofMcu;   

  /// 
  MCU * m_pMcuRow1;
  MCU * m_pMcuRow2; 

  /// Buffer size
  static const int m_iBufferSize = 1024 * 4 * sizeof(ComponentType);

  char __nogc * m_pBuffer1;
  char __nogc * m_pBuffer2;

  /// MCUmembership[i] indexes the i'th component of MCU into the curCompInfo array.
  short m_iMcuMembership __gc [];

  ///
  long m_iBitBuffer;

  ///
  bool m_bSourceAllocated;

  ///
  Viatronix::Imaging::ImageBuffer * m_pSource;

  ///
  Viatronix::Imaging::ImageBuffer * m_pDestination;

};  // class Jpeg

CLOSE_IMAGING_NS

// $Log: Jpeg.h,v $
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
// Revision 1.4  2003/11/14 20:04:31  mkontak
// Fixes
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Jpeg.h,v 1.3 2004/06/29 18:01:08 mkontak Exp $
// $Id: Jpeg.h,v 1.3 2004/06/29 18:01:08 mkontak Exp $

