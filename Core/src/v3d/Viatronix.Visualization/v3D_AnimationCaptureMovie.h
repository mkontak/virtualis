// $Id: v3D_AnimationCaptureMovie.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#pragma once

// includes
#include "v3D_AnimationCapture.h"

OPEN_ANIMATION_NS

// class declaration
public ref class AnimationCaptureMovie : public AnimationCapture
{
// member functions
public:
  
  /// initializes a AnimationCaptureMovieAvi with default values.
  AnimationCaptureMovie();
  
  /// destructor
  virtual ~AnimationCaptureMovie();

  /// finalizer
  !AnimationCaptureMovie();
  
  /// gets the width of a frame
  property int Width
  {
    int get()  { return m_iWidth; }
    void set( int iWidth ){ m_iWidth = iWidth; }
  }
  
  /// gets the height of a frame
  property int Height
  {
    int get() { return m_iHeight; }
    void set( int iHeight ){ m_iHeight = iHeight; }
  }
  
  /// gets the filename
  property System::String ^ Filename
  {
    System::String ^ get() { return m_mpFilename; }
    void set( System::String ^ mpFilename ) { m_mpFilename = mpFilename; }
  }

  /// gets the frames per second
  property int FramesPerSecond
  {
    int get() { return m_iFramesPerSecond; }
    void set( int iFramesPerSecond ) { m_iFramesPerSecond = iFramesPerSecond; }
  }
  
  /// gets the current frame
  property int CurrentFrame
  {
    int get() { return m_iCurrentFrame; }
    void set( int iFrame ) { m_iCurrentFrame = iFrame; } 
  }

  /// create from xml
  static AnimationCaptureMovie ^ FromXml( System::Xml::XmlNode ^ mpParent );

  /// further configure from xml
  virtual void ConfigureFromXml( System::Xml::XmlNode ^ mpParent );

// member variables
private:

  /// width of frame
  int m_iWidth;

  /// height of frame
  int m_iHeight; 

  /// output filename of the movie file
  System::String ^ m_mpFilename;

  /// frames per second
  int m_iFramesPerSecond;
  
  // current frame number
  int m_iCurrentFrame;

}; // class vxAnimationCaptureMovie

CLOSE_ANIMATION_NS

// $Log: v3D_AnimationCaptureMovie.h,v $
// Revision 1.4  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2004/06/16 19:40:01  frank
// added movie previews
//
// Revision 1.2  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovie.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
// $Id: v3D_AnimationCaptureMovie.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
