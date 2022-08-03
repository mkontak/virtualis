// $Id: v3D_AnimationActionPlaneOrtho.h,v 1.5 2007/07/17 19:37:15 romy Exp $
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
#include "v3D_AnimationAction.h"

// namespaces
OPEN_ANIMATION_NS

// class declaration
public ref class AnimationActionPlaneOrtho : 
  public Viatronix::Animations::AnimationAction
{

// member functions
public:
  
  /// initializes a AnimationActionPlaneOrtho object with the specified environment
  AnimationActionPlaneOrtho( Viatronix::Visualization::Environment ^ mpEnvironment );
  
  /// called when the environment is to be initialized
  virtual void OnInitializeEnvironment() override;

  /// called when the environments's state is to be restored
  virtual void OnRestoreEnvironment() override;
  
  // called after the last frame is rendered
  virtual void OnEndAnimation( bool bPreview ) override;

  /// called when the before frame is rendered
  virtual void OnPreRenderFrame( bool bPreview, int iFrameNumber ) override;

  /// gets the total number of frames for the animation
  virtual int GetFramesToRender() override;


public:
  /// gets the start distance
  property float StartDistance
  {
    float get() { return m_fStartDistance; }
    void set( float fStartDistance ) { m_fStartDistance = fStartDistance;  }
  }

  /// gets the end distance
  property float EndDistance
  {
    float get() { return m_fEndDistance; }
    void set( float fEndDistance ) { m_fEndDistance = fEndDistance;  }
  }
  
  /// gets the step size
  property float StepSize
  {
    float get() { return m_fStepSize; }
    void set( float fStepSize ) { m_fStepSize = fStepSize; }
  }

// member functions
private:

  /// start distance
  float4 m_fStartDistance;

  /// end distance
  float4 m_fEndDistance;

  /// step size
  float4 m_fStepSize;
  
  /// indicates if the patient info was displaying on initialization
  bool m_bShowPatientInfo;

  /// indicates if the image info was displaying on initialization
  bool m_bShowImageInfo;
  
}; // class AnimationActionPlaneOrtho

CLOSE_ANIMATION_NS

// $Log: v3D_AnimationActionPlaneOrtho.h,v $
// Revision 1.5  2007/07/17 19:37:15  romy
// fixed the ortho movie creation issue
//
// Revision 1.4  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.3  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2004/05/04 17:47:13  geconomos
// Changed Viewer parameter to type Environment.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/09/26 10:54:30  geconomos
// Removed unneeded native data class.
//
// Revision 1.2  2003/09/22 01:09:20  geconomos
// Added unmanaged data class.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionPlaneOrtho.h,v 1.5 2007/07/17 19:37:15 romy Exp $
// $Id: v3D_AnimationActionPlaneOrtho.h,v 1.5 2007/07/17 19:37:15 romy Exp $
