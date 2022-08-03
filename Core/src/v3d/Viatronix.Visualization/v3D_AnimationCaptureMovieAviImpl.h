// $Id: v3D_AnimationCaptureMovieAviImpl.h,v 1.2 2004/06/15 14:53:44 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragmas
#pragma once
#pragma comment ( lib, "vfw32.lib")

// includes
#include <windows.h>
#include <vfw.h>
#include "typedef.h"

// class delaration
class AnimationCaptureMovieAviImpl
{
// member functions
public:
  
  /// default consturctor
  AnimationCaptureMovieAviImpl();
  
  /// destructor
  virtual ~AnimationCaptureMovieAviImpl();
  
  /// initializes with the specified arguments.
  bool Initialize( int4 iWidth,  int4 iHeight, int iFps, const std::string & sFilename );

  /// adds a frame to the movie
  void AddFrame( int4 iFrameNumber, const uint1 * pData );

  /// get the state of lossy compression
  const bool GetLossyCompression() const { return m_bLossyCompression; }

  /// set the state of lossy compression
  void SetLossyCompression( const bool bLossyCompression ) { m_bLossyCompression = bLossyCompression; }

private:

  /// frees the avi resources associated with this object
  void Free();
  
// member variables
private:

  /// file interface pointer
  PAVIFILE m_pAVIFile;

  /// stream interface
  PAVISTREAM m_pStream;		

  /// compressed video stream
  PAVISTREAM m_pStreamCompressed; 
  
  /// size of bytes of image
  int m_iSizeInBytes;

  /// if lossy compression is enabled
  bool m_bLossyCompression;

}; // class AnimationCaptureMovieAviImpl


// $Log: v3D_AnimationCaptureMovieAviImpl.h,v $
// Revision 1.2  2004/06/15 14:53:44  frank
// added lossy compression
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovieAviImpl.h,v 1.2 2004/06/15 14:53:44 frank Exp $
// $Id: v3D_AnimationCaptureMovieAviImpl.h,v 1.2 2004/06/15 14:53:44 frank Exp $
