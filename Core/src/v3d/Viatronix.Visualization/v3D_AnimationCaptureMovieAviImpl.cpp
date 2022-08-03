// $Id: v3D_AnimationCaptureMovieAviImpl.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "stdafx.h"
#include "v3d_AnimationCaptureMovieAviImpl.h"
#include "logger.h"

// defines
#define FILE_REVISION "$Revision: 1.4 $"

/**
 * Default constructor.
 */
AnimationCaptureMovieAviImpl::AnimationCaptureMovieAviImpl() : 
  m_pAVIFile( NULL ), 
  m_pStream( NULL ), 
  m_pStreamCompressed( NULL ),
  m_bLossyCompression( false )
{
} // AnimationCaptureMovieAviImpl()


/**
 * Destructor
 */
AnimationCaptureMovieAviImpl::~AnimationCaptureMovieAviImpl()
{
  Free();
} // ~AnimationCaptureMovieAviImpl()


/**
 * Initializes with the specified arguments.
 *
 *
 * @param   iWidth      Width of frame in pixels.
 * @param   iHeight     Height of frame in pixels.
 * @param   iFps        Frames per second.
 * @param   sFilename   Output file name.
 *
 * @return  True if initialized successfuly; false otherwise.
 */
bool AnimationCaptureMovieAviImpl::Initialize( int4 iWidth,  int4 iHeight, int4 iFps, const std::string & sFilename )
{

  m_iSizeInBytes = iWidth * iHeight * 3;
  
  Free();

  // check the version of video for windows
  DWORD wVer = HIWORD( VideoForWindowsVersion() );
  if( wVer < 0x010a )
  {
    LogErr( "Version of Video for Windows too old.", "AnimationCaptureMovieAVIImpl", "Initialize" );
    return false;
  }

  // initialize AVI engine
  AVIFileInit();

  // open the movie file for writing
  HRESULT hr = AVIFileOpen( &m_pAVIFile, sFilename.c_str(), OF_WRITE | OF_CREATE,NULL );
  if( hr != AVIERR_OK )
  {
    LogErr( "AVI Engine failed to initialize.", "AnimationCaptureMovieAVIImpl", "Initialize" );
    return false;
  }

  // fill in the header for the video stream
  AVISTREAMINFO streamInfo; 
  memset( &streamInfo, 0, sizeof( streamInfo ) );
  streamInfo.fccType                = streamtypeVIDEO;
  streamInfo.fccHandler             = 0;
  streamInfo.dwScale                = 1;
  streamInfo.dwRate                 = iFps;
  streamInfo.dwSuggestedBufferSize  = m_iSizeInBytes;
  SetRect( &streamInfo.rcFrame, 0, 0, iWidth, iHeight );

  // create the stream
  hr = AVIFileCreateStream( m_pAVIFile, &m_pStream, &streamInfo );
  if( hr != AVIERR_OK )
  {
    LogErr( "AVI Stream creation failed.", "AnimationCaptureMovieAVIImpl", "Initialize" );
    return false;
  }

  // set the compression options
  AVICOMPRESSOPTIONS compressionOptions;  
  memset( &compressionOptions, 0, sizeof( compressionOptions) );
  compressionOptions.dwFlags    = AVICOMPRESSF_VALID;
  compressionOptions.fccHandler = mmioFOURCC('M','S','V','C');
  compressionOptions.fccType    = mmioFOURCC('v', 'i', 'd', 'i');
	compressionOptions.dwQuality  = 10000;
  if ( m_bLossyCompression )
  {
		compressionOptions.dwQuality =  7500;
  }

  // create a compressed stream using codec options
  hr = AVIMakeCompressedStream( &m_pStreamCompressed, m_pStream, &compressionOptions, NULL );
  if (hr != AVIERR_OK)
  {
    LogErr( "AVI Compressed Stream creation failed.", "AnimationCaptureMovieAVIImpl", "Initialize" );
    return false;
  }

  //  set the format of the stream at the specified position
  BITMAPINFOHEADER bih;
  memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
  bih.biSize      = sizeof( BITMAPINFOHEADER );
  bih.biWidth     = iWidth;
  bih.biHeight    = iHeight;
  bih.biPlanes    = 1;
  bih.biBitCount  = 24;
  bih.biSizeImage = m_iSizeInBytes;
  hr = AVIStreamSetFormat( m_pStreamCompressed, 0, &bih, sizeof( bih ) );
  if( hr != AVIERR_OK )
  {
    LogErr( "AVI Compressed Stream format setting failed.", "AnimationCaptureMovieAVIImpl", "Initialize" );
    return false;
  }

  return true;
} // Initialize()



/**
 * Adds a frame to the movie.
 *
 * @param   iFrameNumber    Current frame number.
 * @param   pData           Pixel data.
 */
void AnimationCaptureMovieAviImpl::AddFrame( int4 iFrameNumber, const uint1 * pData )
{
  // compress bitmap
  AVIStreamWrite( m_pStreamCompressed, iFrameNumber, 1, (void*)pData,					
                   m_iSizeInBytes, AVIIF_KEYFRAME, NULL, NULL );
} // AddFrame()


/**
 * Frees the avi resources associated with this object.
 */
void AnimationCaptureMovieAviImpl::Free()
{
  if( m_pStream !=  NULL )
  {
    AVIStreamRelease( m_pStream );
    m_pStream = NULL;
  } // if( m_pStream !=  NULL )

  if( m_pStreamCompressed != NULL)
  {
    AVIStreamRelease( m_pStreamCompressed );
    m_pStreamCompressed = NULL;
  } // if( m_pStreamCompressed != NULL)

  if( m_pAVIFile != NULL )
  {
    AVIFileRelease( m_pAVIFile );
    m_pAVIFile = NULL;
  } // if( m_pAVIFile != NULL )

  AVIFileExit();
} // Free()

// $Log: v3D_AnimationCaptureMovieAviImpl.cpp,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2004/06/15 15:06:04  frank
// enable compression
//
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovieAviImpl.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_AnimationCaptureMovieAviImpl.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
