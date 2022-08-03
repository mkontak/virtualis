// $Id: v3D_AnimationCaptureMovieAVI.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
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
#include "v3D_AnimationCaptureMovie.h"


// foward declarations
class AnimationCaptureMovieAviImpl;

// namespaces
OPEN_ANIMATION_NS


// class declaration
public ref class AnimationCaptureMovieAvi : public AnimationCaptureMovie
{
public:
  
  // initializes a AnimationCaptureMovieAvi with default values.
  AnimationCaptureMovieAvi();
  
  /// destructor
  virtual ~AnimationCaptureMovieAvi();

  /// finalizer
  !AnimationCaptureMovieAvi();
  
  /// initializes the capture object
  virtual bool Initialize() override;
  
  /// adds a frame to the movie
  virtual void AddFrame( System::IntPtr data ) override;
  
  // closes the capture 
  virtual void Close() override;
  
  /// further configure from xml
  virtual void ConfigureFromXml( System::Xml::XmlNode ^ mpParent ) override;

// member variables
private:

  // unmanged implementation
  AnimationCaptureMovieAviImpl * m_pImpl;

}; // class AnimationCaptureMovieAvi

CLOSE_ANIMATION_NS


// $Log: v3D_AnimationCaptureMovieAVI.h,v $
// Revision 1.3  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovieAVI.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_AnimationCaptureMovieAVI.h,v 1.3 2006/10/04 18:29:04 gdavidson Exp $

