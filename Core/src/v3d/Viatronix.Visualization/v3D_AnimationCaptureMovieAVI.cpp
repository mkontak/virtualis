// $Id: v3D_AnimationCaptureMovieAVI.cpp,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// includes
#include "stdafx.h"
#include "v3D_AnimationCaptureMovieAvi.h"
#include "v3D_AnimationCaptureMovieAviImpl.h"
#include "Logger.h"

// namespaces
USING_ANIMATION_NS

// defines
#define FILE_REVISION "$Revision: 1.4 $"

/**
 * Initializes a AnimationCaptureMovieAvi with default values.
 */
AnimationCaptureMovieAvi::AnimationCaptureMovieAvi() 
{
  m_pImpl = new AnimationCaptureMovieAviImpl();
} // AnimationCaptureMovieAvi()


/**
 * destructor.
 */
AnimationCaptureMovieAvi::~AnimationCaptureMovieAvi()
{
  this->!AnimationCaptureMovieAvi();
} // ~AnimationCaptureMovieAvi()


/**
 * Finalizer.
 */
AnimationCaptureMovieAvi::!AnimationCaptureMovieAvi()
{
  Close();
} // !AnimationCaptureMovieAvi()


/**
 * Closes the capture 
 */
void AnimationCaptureMovieAvi::Close()
{
  if( m_pImpl != NULL )
  {
    delete m_pImpl;
    m_pImpl = NULL;
  } // if( m_pImpl != NULL )
} // Close()


/**
 * Initializes the capture object.
 *
 * @return true if initialized successfuly; false otherwise 
 */
bool AnimationCaptureMovieAvi::Initialize()
{
  return m_pImpl->Initialize( this->Width, this->Height, this->FramesPerSecond, ss( this->Filename ) );
} // Initialize()


/**
 *  Adds a frame to the movie.
 *
 * @param   data    Pointer to the first byte of the image data.
 */
void AnimationCaptureMovieAvi::AddFrame( System::IntPtr data )
{
  m_pImpl->AddFrame(( this->CurrentFrame )++, reinterpret_cast< uint1 * >( data.ToPointer() ) );
} // AddFrame()


/**
 * further configure from xml
 *
 * @param mpParent the parent xml node
 */
void AnimationCaptureMovieAvi::ConfigureFromXml( System::Xml::XmlNode ^ mpParent )
{

  // call the base class first
  AnimationCaptureMovie::ConfigureFromXml( mpParent );

  // configure the compression
  System::Xml::XmlNode ^ mpCompressionNode = mpParent->SelectSingleNode( "Compression" );
  if ( mpCompressionNode != nullptr )
  {
    System::String ^ mpTrueFalse( mpCompressionNode->InnerText );
    if ( mpTrueFalse->CompareTo( "true" ) == 0 )
    {
      LogDbg( "Lossy compression is enabled - compressed video to be created", "AnimationCaptureMovieAvi", "ConfigureFromXml" );
      m_pImpl->SetLossyCompression( true );
    }
    else if ( mpTrueFalse->CompareTo( "false" ) == 0 )
    {
      LogDbg( "Lossy compression is disabled - uncompressed video to be created", "AnimationCaptureMovieAvi", "ConfigureFromXml" );
      m_pImpl->SetLossyCompression( false );
    }
    else
    {
      throw ex::IllegalArgumentException( LogRec( "value of compression must be either true or false, was " + ss( mpCompressionNode->InnerText ), "AnimationCaptureMovieAvi", "ConfigureFromXml" ) );
    }
  }

} // ConfigureFromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_AnimationCaptureMovieAVI.cpp,v $
// Revision 1.4  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2004/10/29 20:18:21  frank
// formatting
//
// Revision 1.2  2004/06/15 14:54:16  frank
// added lossy compression xml interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovieAVI.cpp,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
// $Id: v3D_AnimationCaptureMovieAVI.cpp,v 1.4 2006/10/04 18:29:03 gdavidson Exp $