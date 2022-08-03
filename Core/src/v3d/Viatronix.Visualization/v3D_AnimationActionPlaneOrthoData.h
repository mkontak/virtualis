// $Id: v3D_AnimationActionPlaneOrthoData.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#pragma once

// include declarations
#include "vxEnvironment.h"

// class declaration
class AnimationActionPlaneOrthoData 
{

// enums
public:
  
  /// events  
  enum EventAnimationPlaneOrtho
  {
    START_DISTANCE_MODIFIED,
    END_DISTANCE_MODIFIED,
    STEP_SIZE_MODIFIED,
  }; // enum vxEventRendererCpu3dDataEnum



// member functions
public:
  
  // default constructor 
  AnimationActionPlaneOrthoData();
  
  // destructor
  virtual ~AnimationActionPlaneOrthoData();
  
  ///// called before the next frame is to be rendered
  //virtual void OnPreRenderFrame( const int4 iFrameNumber );
  //
  /// gets the total number of frames for the animation
  virtual const int4 GetFramesToRender() const;
  
  /// gets the environment
  vxShareableObject< vxEnvironment > * GetEnvironment() const { return m_pEnvironment; }
  
  /// sets the environment
  void SetEnvionment( vxShareableObject< vxEnvironment > * pEnvironment ) { m_pEnvironment = pEnvironment; }
  
  /// gets the start distance
  float4 GetStartDistance() const { return m_fStartDistance; }
  
  /// sets the start distance
  void SetStartDistance( const float4 fStartDistance ) { m_fStartDistance = fStartDistance; }
  
  /// gets the end distance
  float4 GetEndDistance() const { return m_fEndDistance; }

  /// sets the end distance
  void SetEndDistance( const float4 fEndDistance ) { m_fEndDistance = fEndDistance; }

  /// gets the step size
  float4 GetStepSize() const { return m_fStepSize; }

  /// sets the step size
  void SetStepSize( const float4 fStepSize ) { m_fStepSize = fStepSize; }  

// member variables
private:

  /// animation environmen t
  vxShareableObject< vxEnvironment > * m_pEnvironment;
  
  /// start distance
  float4 m_fStartDistance;
  
  /// end distance
  float4 m_fEndDistance;
  
  /// step size
  float4 m_fStepSize;
  
  /// orientation of plane
  uint2 m_uOrientation;

}; // class AnimationActionPlaneOrthoData

// $Log: v3D_AnimationActionPlaneOrthoData.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/09/22 01:08:49  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_AnimationActionPlaneOrthoData.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id: v3D_AnimationActionPlaneOrthoData.h,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
