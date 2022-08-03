// $Id: v3D_AnimationActionPlaneOrthoData.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#include "v3D_AnimationActionPlaneOrthoData.h"
#include "vxUtils.h"

/**
 * Default consturctor.
 */
AnimationActionPlaneOrthoData::AnimationActionPlaneOrthoData() :
  m_pEnvironment( NULL ),
  m_fStartDistance( 0.0F ),
  m_fEndDistance( 0.0F ),
  m_fStepSize( 0.0F )
{
} // AnimationActionPlaneOrthoData()


/**
 * Destructor.
 */
AnimationActionPlaneOrthoData::~AnimationActionPlaneOrthoData()
{
} // ~AnimationActionPlaneOrthoData()


/** 
 * Gets the total number of frames for the animation.
 *
 * @return Total number of frames to render
 */
const int4 AnimationActionPlaneOrthoData::GetFramesToRender() const
{
  return static_cast< int4 >( (0.5F + Abs( m_fEndDistance - m_fStartDistance ) )  / m_fStepSize  + 1.0F );
} // GetFramesToRender()
//
//
//
///**
// * Called before the next frame is to be rendered.
// *
// * @param iFrameNumber  The current frame to be rendered.
// */
//void AnimationActionPlaneOrthoData::OnPreRenderFrame( const int4 iFrameNumber )
//{
//  // figure out the amount to offset the plane from the start distance
//  float4 fOffset = m_fStepSize * static_cast< float4 >( iFrameNumber );  
//  
//  if( m_fStartDistance > m_fEndDistance )
//    fOffset *= -1;
//  
//  // update the orthogonal plane's distance
//  GetEnvironment().GetPlaneOrthoPtr()->SetDistance( m_fStartDistance + fOffset );
//  GetEnvironment().GetPlaneOrthoPtr()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::vxEventPlaneEnum::PLANE_MODIFIED ) );  
//
//} //  OnPreRenderFrame()

// $Log: v3D_AnimationActionPlaneOrthoData.cpp,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/09/22 01:08:49  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_AnimationActionPlaneOrthoData.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id: v3D_AnimationActionPlaneOrthoData.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
