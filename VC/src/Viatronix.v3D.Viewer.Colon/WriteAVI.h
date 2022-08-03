// $Id: WriteAVI.h,v 1.3 2006/01/31 14:29:16 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Kevin Kreeger  kevin@viatronix.net


// includes
#include <vfw.h>
#include "typedef.h"


// defines
#define TEXT_HEIGHT 20
#define AVIIF_KEYFRAME  0x00000010L // this frame is a key frame.
#define BUFSIZE 260


class WriteAVI
{
public:
  
  /// Constructor.
  WriteAVI(LPCTSTR lpszFileName, int xdim, int ydim,int4 iFrameRate);
  
  /// Destructor.
  virtual ~WriteAVI();

  /// Adds <iNumFrames> of the given bitmap.
  bool AddFrame(const CBitmap & bmp, const uint4 iNumFrames = 1);
  
  /// Adds <iNumFrames> of the given RGB bitmap data.  Alters <pdata> by flipping B and R values.
  bool AddFrame(void * pdata, const uint4 iNumFrames = 1);
  
  /// Whether errors occurred while adding frames to the movie.
  virtual bool IsOK() const {return m_bOK;};
  
  /// Closes and saves the movie (to the file given in the constructor).
  void Close();

private:
  
  /// X dimension.
  int m_uXDim;
  
  /// Y Dimension.
  int m_uYDim;
  
  /// Current number of frames in movie.
  int m_uNumFrames;

  /// Pointer to AVI structure.
  PAVIFILE m_pAVIFile;
  
  /// Pointer to AVI stream.
  PAVISTREAM m_pAVIStream;
  
  /// Pointer to compressed AVI stream.
  PAVISTREAM m_pAVIStreamCompressed;
  
  /// Whether errors occurred while adding frames to the movie.
  bool m_bOK;

}; // class WriteAVI


// $Log: WriteAVI.h,v $
// Revision 1.3  2006/01/31 14:29:16  frank
// code review
//
// Revision 1.2  2005/10/28 13:28:28  geconomos
// code cleanup
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.6  2002/01/09 03:47:03  jmeade
// Standards.
//
// Revision 1.5  2001/12/19 20:51:43  jmeade
// Tabs to spaces.
//
// Revision 1.4  2001/12/12 20:17:25  jmeade
// Allow specification of number of frames in AddFrame(); Comments, standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WriteAVI.h,v 1.3 2006/01/31 14:29:16 frank Exp $
// $Id: WriteAVI.h,v 1.3 2006/01/31 14:29:16 frank Exp $
