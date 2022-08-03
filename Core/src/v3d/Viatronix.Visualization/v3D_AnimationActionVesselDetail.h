// $Id: v3D_AnimationActionVesselDetail.h,v 1.3 2007/08/29 20:09:10 romy Exp $
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
public ref class AnimationActionVesselDetail : 
  public Viatronix::Animations::AnimationAction
{

// member functions
public:
  
  /// initializes a AnimationActionPlaneOrtho object with the specified environment
  AnimationActionVesselDetail( Viatronix::Visualization::Environment ^ mpEnvironment );
  
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

private:

	// member functions
	void SetObliqueView( const float4 fVesselSectionIndex );

private:

  /// start distance
  float4 m_fStartDistance;

  /// end distance
  float4 m_fEndDistance;

  /// step size
  float4 m_fStepSize;
  
	//Initial vessel index
	float4 m_fVesselSelectionIndex;
  
}; // class AnimationActionVesselDetail

CLOSE_ANIMATION_NS


// $Log: v3D_AnimationActionVesselDetail.h,v $
// Revision 1.3  2007/08/29 20:09:10  romy
// Code added to reset the view to the initial index after movie
//
// Revision 1.2  2007/05/22 14:10:16  romy
// Turns the Image information off during movie making
//
// Revision 1.1  2007/05/14 14:20:32  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionVesselDetail.h,v 1.3 2007/08/29 20:09:10 romy Exp $
// $Id: v3D_AnimationActionVesselDetail.h,v 1.3 2007/08/29 20:09:10 romy Exp $
