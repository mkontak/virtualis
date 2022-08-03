// $Id: v3D_AnimationActionOblique.h,v 1.5 2007/08/30 18:35:05 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

#pragma once

// includes
#include "v3D_AnimationAction.h"

// namespaces
OPEN_ANIMATION_NS

// class declaration
public ref class AnimationActionOblique : 
  public Viatronix::Animations::AnimationAction
{

// member functions
public:
  
  /// initializes a AnimationActionPlaneOrtho object with the specified environment
  AnimationActionOblique( Viatronix::Visualization::Environment ^ mpEnvironment );
  
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
	
	///Animates the forward
  property bool AnimateForward
	{
		bool get () { return m_bAnimateForward; }
		void set ( bool bAnimateForward ) { m_bAnimateForward = bAnimateForward; }
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
	
	///Sets the plane distance
	void SetPlaneDistance( float4  fDistanceNew );

	///Gets the new Plane Distance
	float4 GetNewPlaneDistance();


private:

	//If the animation is forward.
	bool m_bAnimateForward;

  /// start distance
  float4 m_fInitialDistance;

  /// end distance
  float4 m_fEndDistance;

  /// step size
  float4 m_fStepSize;
  
	///Keep the image information flag 
	bool m_bImageInformation;

	///Keep the current fontsize
	int2 m_iFontSize;
  
	//the number of frames to process
	int m_iFrames;

	//Flag indicates about the animation status
	bool m_bAnimating;
 
}; // class AnimationActionVesselDetail

CLOSE_ANIMATION_NS


// $Log: v3D_AnimationActionOblique.h,v $
// Revision 1.5  2007/08/30 18:35:05  romy
// modified the backward and forward steps calculatiions
//
// Revision 1.4  2007/08/30 17:10:56  romy
// exposes properties
//
// Revision 1.3  2007/08/29 21:24:36  romy
// fixed the wrong slice number indication
//
// Revision 1.2  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.1  2007/07/02 20:52:09  romy
// oblique movie added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionOblique.h,v 1.5 2007/08/30 18:35:05 romy Exp $
// $Id: v3D_AnimationActionOblique.h,v 1.5 2007/08/30 18:35:05 romy Exp $
