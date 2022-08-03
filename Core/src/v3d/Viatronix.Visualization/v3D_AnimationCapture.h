// $Id: v3D_AnimationCapture.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
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

// namespaces
OPEN_ANIMATION_NS

// class declaration
public ref class AnimationCapture
{
// member functions
public:
  
  /// initializes a AnimationCaptureMovieAvi with default values.
  AnimationCapture();
  
  /// destructor
  virtual ~AnimationCapture();

  /// finalizer
  !AnimationCapture();
  
  /// called to initialized the capture object
  virtual bool Initialize() { return false; }
  
  /// adds a frame to the movie
  virtual void AddFrame( System::IntPtr data ) {}
  
  /// closes the capture
  virtual void Close() {}
  
}; // class vxAnimationCapture

CLOSE_ANIMATION_NS

// $Log: v3D_AnimationCapture.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCapture.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
// $Id: v3D_AnimationCapture.h,v 1.4 2006/10/04 18:29:03 gdavidson Exp $
