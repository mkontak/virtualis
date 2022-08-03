// $Id: v3D_AnimationActionRotate3D.cpp,v 1.21 2007/07/16 20:01:14 romy Exp $
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
#include "v3D_AnimationActionRotate3D.h"
#include "v3D_Dataset.h"
#include "vxUtils.h"
#include "v3D_Momentum.h"


// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS
using namespace System::Xml;


// defines
#define FILE_REVISION "$Revision: 1.21 $"


/**
 * default constructor
 */ 
AnimationActionRotate3D::AnimationActionRotate3D() :
AnimationAction(),
m_iNumberFrames( 30 ),
m_eRotationAxis( Viatronix::Animations::RotationAxis::y ),
m_iDegreesRotation( 360 )
{ 
} // AnimationActionRotate3D()


/**
 * Initializes a AnimationActionRotate3D object with the specified environment
 *
 * @param   mpEnvironment    Environment object to animate.
 */ 
AnimationActionRotate3D::AnimationActionRotate3D( Viatronix::Visualization::Environment ^ mpEnvironment ) : 
AnimationAction( mpEnvironment ), 
m_iNumberFrames( 30 ), 
m_eRotationAxis( Viatronix::Animations::RotationAxis::y ),
m_iDegreesRotation( 360 )
{ 
	
} // AnimationActionRotate3D()


/** 
 * Gets the total number of frames for the animation.
 *
 * @return Total number of frames to render
 */
int AnimationActionRotate3D::GetFramesToRender()
{
  return NumberFrames;
} // get_FramesToRender()


/**
 * Called when the environment is to be initialized.
 */
void AnimationActionRotate3D::OnInitializeEnvironment()
{
  // turn off momentum
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  this->Environment->Momentum->Enabled = false;    
} // OnInitializeEnvironment()


/**
 * Called when the environment's state is to be restored.
 */
void AnimationActionRotate3D::OnRestoreEnvironment()
{
} // OnRestoreEnvironment()


/**
 * Called before the first frame is rendered.
 */
void AnimationActionRotate3D::OnBeginAnimation(bool bPreview)
{
  if (!bPreview)
  {
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  } // if ( bPreview )

  AnimationAction::OnBeginAnimation( bPreview );
} // BeginAnimation()


/**
 * Called after the last frame is rendered.
 */
void AnimationActionRotate3D::OnEndAnimation( bool bPreview )
{
  if (!bPreview)
  {
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  } // if ( !bPreview )

  if ( m_bCycleDatasets )
  {
    m_iDatasetIndex = 0;
    Dataset ^ mpDataset = m_mpDatasetList[ m_iDatasetIndex ];
    this->Environment->Volume = mpDataset->IntensityVolume;
    this->Environment->LabelVolume = mpDataset->LabelVolume;
  }
  
  AnimationAction::OnEndAnimation( bPreview );
} // EndAnimation()


/**
 * Called before the next frame is to be rendered.
 *
 * @param iFrameNumber  The current frame to be rendered.
 */
void AnimationActionRotate3D::OnPreRenderFrame( bool bPreview, int iFrameNumber )
{ 

  if ( m_bCycleDatasets && m_mpDatasetList->Count == 0 )
    throw gcnew System::ArgumentException( "No dataset list supplied for animation" );

  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );

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
  
  // update camera
  float4 fAngle = m_iDegreesRotation / 180.0F * 3.1415F / m_iNumberFrames;

  if ( m_eRotationAxis == Viatronix::Animations::RotationAxis::x )
    pEnvironment->GetCamera()->Rotate( pEnvironment->GetCamera()->GetViewRight(), fAngle );

  else if ( m_eRotationAxis == Viatronix::Animations::RotationAxis::y )
    pEnvironment->GetCamera()->Rotate(pEnvironment->GetCamera()->GetViewUp(), fAngle );

  else    
    pEnvironment->GetCamera()->Rotate(pEnvironment->GetCamera()->GetViewDirection(), fAngle );

  pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} //  OnPreRenderFrame()


/**
 * intialize further from xml node
 *
 * @param mpParent the parent xml node
 */
void AnimationActionRotate3D::ConfigureFromXml( System::Xml::XmlNode ^ mpParent )
{

  // configure the axis of rotation
  XmlNode ^ mpAxisOfRotationNode = mpParent->SelectSingleNode( "AxisOfRotation" );
  if ( mpAxisOfRotationNode != nullptr )
  {
    System::String ^ mpAxis = mpAxisOfRotationNode->InnerText;
    if ( mpAxis->CompareTo( "x" ) == 0 )
    {
      this->RotationAxis = Viatronix::Animations::RotationAxis::x;
    }
    else if ( mpAxis->CompareTo( "y" ) == 0 )
    {
      this->RotationAxis = Viatronix::Animations::RotationAxis::y;
    }
    else if ( mpAxis->CompareTo( "z" ) == 0 )
    {
      this->RotationAxis = Viatronix::Animations::RotationAxis::z;
    }
  }
  
  // configure the degrees per image
  XmlNode ^ mpDegreesRotationNode = mpParent->SelectSingleNode( "DegreesRotation" );
  if ( mpDegreesRotationNode != nullptr )
  {
    int4 iDegrees = System::Int32::Parse( mpDegreesRotationNode->InnerText );
    if ( iDegrees <= 0 )
    {
      throw ex::IllegalArgumentException( LogRec( "degrees rotation must be a positive integer, was " + ToAscii( iDegrees ), "AnimationActionRotate3D", "ConfigureFromXml" ) );
    }
    this->DegreesRotation = iDegrees;
  }

} // ConfigureFromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_AnimationActionRotate3D.cpp,v $
// Revision 1.21  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.20  2007/01/16 17:12:04  gdavidson
// Added Cine
//
// Revision 1.19  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.18  2006/10/03 20:13:55  geconomos
// fixed size rendering support added
//
// Revision 1.17  2006/08/01 15:14:58  gdavidson
// Changed ArrayList to List<>
//
// Revision 1.16  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.15  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.14  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.13.6.2  2005/07/08 16:45:43  frank
// permitted less than 360 degrees rotation
//
// Revision 1.13.6.1  2005/07/08 15:49:19  frank
// added 4D animation
//
// Revision 1.13  2005/01/17 15:36:14  geconomos
// leaving the display of patient and image info to it's default setting
//
// Revision 1.12  2004/12/16 16:18:34  frank
// formatting
//
// Revision 1.11  2004/11/17 22:31:59  michael
// using SetFixedImageSize() and no 2nd class environment citizen ProgressiveRefinement
//
// Revision 1.10  2004/11/04 01:36:02  michael
// Added setting the fixed image size rendering as well as setting the camera
// modified for the very first time since otherwise the vxViewer will determine
// that there is nothing to do and skip the image generation --> previous resolution
// image will be taken. Need to revisit this after RSNA 2004
//
// Revision 1.9  2004/10/18 14:50:29  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.8  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.7  2004/05/11 19:44:21  frank
// changed the number of images so that there is no repetition of the initial image at the end of the sequence - this makes the animation smooth while looping
//
// Revision 1.6  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.5  2004/05/05 13:19:43  frank
// changed interface to animation action
//
// Revision 1.4  2004/05/04 17:47:13  geconomos
// Changed Viewer parameter to type Environment.
//
// Revision 1.3  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.2  2004/03/30 18:16:56  frank
// split out the progressive refinement flag from the renderer CPU 3d data so that it can be applied to all renderers in an environment at once
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:47  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/17 01:09:25  geconomos
// Issue 3747: Disabled momentum\cine before displaying movie tool.
//
// Revision 1.3  2003/09/26 11:40:48  geconomos
// Added calls to bass class on the OnXXX methods
//
// Revision 1.2  2003/09/09 00:52:51  geconomos
// Defaulted rotation orientation to horizontal.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionRotate3D.cpp,v 1.21 2007/07/16 20:01:14 romy Exp $
// $Id: v3D_AnimationActionRotate3D.cpp,v 1.21 2007/07/16 20:01:14 romy Exp $
