// $Id: v3D_AnimationActionPlaneOrtho.cpp,v 1.14 2007/08/30 19:08:05 romy Exp $
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
#include "v3D_AnimationActionPlaneOrtho.h"
#include "vxUtils.h"
#include "v3D_Cine.h"
#include "v3D_Dataset.h"
#include "v3D_VisualizationGraph.h"
#include "v3D_Viewer.h"


#define FILE_REVISION "$Revision: 1.14 $"

// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS


/**
 * Initializes a AnimationActionPlaneOrtho object with the sepcified environment.
 *
 * @param   mpEnvironment    Environment object to animate.
 */ 
AnimationActionPlaneOrtho::AnimationActionPlaneOrtho( Viatronix::Visualization::Environment ^ mpEnvironment ) : 
  AnimationAction( mpEnvironment ),
  m_fStartDistance( 0.0F ),
  m_fEndDistance( 0.0F ),
  m_fStepSize( 0.0F ),
  m_bShowPatientInfo( false ),
  m_bShowImageInfo( false )
{
} // AnimationActionPlaneOrtho()
 
 
/**
 * Called when the environment is to be initialized.
 */
void AnimationActionPlaneOrtho::OnInitializeEnvironment()
{
  __super::OnInitializeEnvironment();

  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );

  m_bShowPatientInfo = pEnvironment->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION );
  m_bShowImageInfo = pEnvironment->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION );
  this->Environment->Cine->CineMode = Viatronix::Visualization::CineModes::Idle;

} // OnInitializeEnvironment()


/**
 * Called when the environments's state is to be restored.
 */
void AnimationActionPlaneOrtho::OnRestoreEnvironment()
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
} // OnRestoreEnvironment()


/**
 * Called after the last frame is rendered.
 */
void AnimationActionPlaneOrtho::OnEndAnimation( bool bPreview )
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );

  pEnvironment->GetPlaneOrtho()->SetDistance( m_fStartDistance + 0.0F );
  pEnvironment->GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );  

  AnimationAction::OnEndAnimation( bPreview );

} // EndAnimation()


/** 
 * Gets the total number of frames for the animation.
 *
 * @return Total number of frames to render
 */
int AnimationActionPlaneOrtho::GetFramesToRender()
{
  float fOffset = m_fEndDistance > m_fStartDistance ? 0.0F : 1.0F;
	int iTotalFrames =  static_cast< int4 >( ( 0.5F + Abs( m_fEndDistance - m_fStartDistance ) ) / m_fStepSize + fOffset );
  return iTotalFrames;
} // GetFramesToRender()


/**
 * Called before the next frame is to be rendered.
 *
 * @param iFrameNumber  The current frame to be rendered.
 */
void AnimationActionPlaneOrtho::OnPreRenderFrame( bool bPreview, int iFrameNumber )
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	
  // figure out the amount to offset the plane from the start distance
	int iFrame  = iFrameNumber;
	float4 fOffset = m_fStepSize * static_cast< float4 >( iFrame );
  if( m_fStartDistance > m_fEndDistance )
    fOffset *= -1;

	if ( iFrameNumber == 0 )
  {
    // need to set something modified otherwise nothing will happen!!!
    pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    if ( m_bCycleDatasets )
      m_iDatasetIndex = m_mpDatasetList->Count - 1;
    return;
  }

  // cycle through the datasets
  if ( m_bCycleDatasets )
  {
    m_iDatasetIndex = ( m_iDatasetIndex + 1 ) % m_mpDatasetList->Count;
    Dataset ^ mpDataset = m_mpDatasetList[ m_iDatasetIndex ];
    this->Environment->Volume = mpDataset->IntensityVolume;
    this->Environment->LabelVolume = mpDataset->LabelVolume;
  }
  
  pEnvironment->GetPlaneOrtho()->SetDistance( m_fStartDistance + fOffset );
  pEnvironment->GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );
	pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
	
} //  OnPreRenderFrame()



// $Log: v3D_AnimationActionPlaneOrtho.cpp,v $
// Revision 1.14  2007/08/30 19:08:05  romy
// fixed the beating issue
//
// Revision 1.13  2007/08/30 17:15:11  romy
// sets the Direction fix
//
// Revision 1.12  2007/08/29 21:24:36  romy
// fixed the wrong slice number indication
//
// Revision 1.11  2007/07/17 19:37:15  romy
// fixed the ortho movie creation issue
//
// Revision 1.10  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.9  2007/01/16 17:12:17  gdavidson
// Added Cine
//
// Revision 1.8  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.7  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.6  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/01/21 13:20:13  frank
// fixed update of number of images text
//
// Revision 1.5  2005/01/17 15:36:14  geconomos
// leaving the display of patient and image info to it's default setting
//
// Revision 1.4  2004/05/05 13:19:26  frank
// changed interface to animation action
//
// Revision 1.3  2004/05/04 17:47:13  geconomos
// Changed Viewer parameter to type Environment.
//
// Revision 1.2  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/03/02 01:10:47  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.4.2.1  2004/02/17 01:09:25  geconomos
// Issue 3747: Disabled momentum\cine before displaying movie tool.
//
// Revision 1.4  2003/09/26 11:40:48  geconomos
// Added calls to bass class on the OnXXX methods
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionPlaneOrtho.cpp,v 1.14 2007/08/30 19:08:05 romy Exp $
// $Id: v3D_AnimationActionPlaneOrtho.cpp,v 1.14 2007/08/30 19:08:05 romy Exp $
