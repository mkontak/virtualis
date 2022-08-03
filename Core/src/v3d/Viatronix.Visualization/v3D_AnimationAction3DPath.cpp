// $Id: v3D_AnimationAction3DPath.cpp,v 1.15 2007/01/16 17:12:32 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


// includes
#include "stdafx.h"
#include "v3D_AnimationAction3DPath.h"
#include "vxUtils.h"
#include "vxElementFloat4.h"
#include "vxVessel.h"
#include "v3D_Momentum.h"



// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS
using namespace System::Xml;


// defines
#define FILE_REVISION "$Revision: 1.15 $"


/**
 * default constructor
 */ 
AnimationAction3DPath::AnimationAction3DPath() :
AnimationAction(),
m_iNumberFrames( 32 ),
m_bShowPatientInfo( false ),
m_bShowImageInfo( false ),
m_bForward( true )
{ 
} // AnimationAction3DPath()


/**
 * Initializes a AnimationAction3DPath object with the specified environment
 *
 * @param   mpEnvironment    Environment object to animate.
 */ 
AnimationAction3DPath::AnimationAction3DPath( Viatronix::Visualization::Environment ^ mpEnvironment ) : 
 AnimationAction( mpEnvironment ), 
 m_iNumberFrames( 32 ), 
 m_bShowPatientInfo( false ), 
 m_bShowImageInfo( false ),
 m_bForward( true )
{ 
} // AnimationAction3DPath()


/**
 * Called when the environment is to be initialized.
 */
void AnimationAction3DPath::OnInitializeEnvironment()
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  m_bShowPatientInfo = pEnvironment->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION );
  m_bShowImageInfo = pEnvironment->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION );
  
  //pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION, false );
  //pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION, false );
  
  // turn off momentum
  this->Environment->Momentum->Enabled = false; 
    
} // OnInitializeEnvironment()


/**
 * Called when the environment's state is to be restored.
 */
void AnimationAction3DPath::OnRestoreEnvironment()
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  //pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION, m_bShowPatientInfo );
  //pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION, m_bShowImageInfo );
} // OnRestoreEnvironment()


/**
 * Called before the first frame is rendered.
 */
void AnimationAction3DPath::OnBeginAnimation( bool bPreview )
{
  if ( ! bPreview )
  {
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  } // if( bPreview )

  AnimationAction::OnBeginAnimation( bPreview );

} // BeginAnimation()


/**
 * Called after the last frame is rendered.
 */
void AnimationAction3DPath::OnEndAnimation( bool bPreview )
{
  if ( ! bPreview )
  {
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
  } // if( !bPreview )
  
  AnimationAction::OnEndAnimation( bPreview );

} // EndAnimation()


/**
 * Called before the next frame is to be rendered.
 *
 * @param iFrameNumber  The current frame to be rendered.
 */
void AnimationAction3DPath::OnPreRenderFrame( bool bPreview, int iFrameNumber )
{ 

  vxEnvironment * pEnvironment( ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() ) );

  // why is this here???
  if ( iFrameNumber == 0 )
  {
    // need to set something modified otherwise nothing will happen!!!
    pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    return;
  }

  // handle backwards flight
  if ( m_bForward == false ) iFrameNumber = m_iNumberFrames - 1 - iFrameNumber;
  
  // get the vessel
  vxVessel * pVessel( dynamic_cast< vxVessel * >( pEnvironment->GetElement( vxIDs::Path ) ) );
  if ( pVessel == NULL )
  {
    LogWrn( "Vessel array pointer is NULL", "AnimationAction3DPath", "OnPreRenderFrame" );
    return;
  }
  if ( pVessel->size() == 0 )
  {
    LogWrn( "Vessel is empty", "AnimationAction3DPath", "OnPreRenderFrame" );
    return;
  }

  const float4 fMaximumMM( pVessel->ResampleAccumulatedLengthMM( pVessel->size() -1 ) );
  const float4 fDesiredMM( Interpolate( 0.0F, float4( iFrameNumber ), float4( m_iNumberFrames - 1 ), 0.0F, fMaximumMM ) );
  float4 fSectionIndex( pVessel->GetFractionalIndex( fDesiredMM ) );

  // clamp to valid range
  fSectionIndex = Bound( 0.0F, fSectionIndex, float4( pVessel->size()-1 ) );

  // set the camera location - need to interpolate these
  const Point<float4> position( pVessel->GetResampledPosition( fSectionIndex ) );
  Normal<float4> tangent( pVessel->GetResampledTangent( fSectionIndex ) );
  const Normal<float4> upDirection( pVessel->GetResampledUpDirection( fSectionIndex ) );

  // handle backwards flight
  if ( m_bForward == false ) tangent = -tangent;

  pEnvironment->GetCamera()->SetEye( position );
  pEnvironment->GetCamera()->SetViewDirection( tangent );
  pEnvironment->GetCamera()->SetViewUp( upDirection );
  pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  LogDbg( "image: " + ToAscii( iFrameNumber ) + " index: " + ToAscii( fSectionIndex ) + " pos: " + pEnvironment->GetCamera()->GetEye().ToString(), "", "" );

  pEnvironment->GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} //  OnPreRenderFrame()


/**
 * intialize further from xml node
 *
 * @param mpParent the parent xml node
 */
void AnimationAction3DPath::ConfigureFromXml( System::Xml::XmlNode ^ mpParent )
{

  // configure the number of frames
  XmlNode ^ mpNumberFramesNode = mpParent->SelectSingleNode( "NumberOfFrames" );
  if ( mpNumberFramesNode != nullptr )
  {
    int iNumberFrames = System::Int32::Parse( mpNumberFramesNode->InnerText );
    if ( iNumberFrames < 0 )
    {
      throw ex::IllegalArgumentException( LogRec( "number of frames must be a positive integer, was " + ToAscii( iNumberFrames ), "AnimationAction3DPath", "ConfigureFromXml" ) );
    }
    this->NumberFrames = iNumberFrames;
  }
  
  // configure the forward/backward direction
  XmlNode ^ mpForwardNode = mpParent->SelectSingleNode( "Forward" );
  if ( mpForwardNode != nullptr )
  {
    this->ForwardFlight = System::Boolean::Parse( mpForwardNode->InnerText );
  }

} // ConfigureFromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_AnimationAction3DPath.cpp,v $
// Revision 1.15  2007/01/16 17:12:32  gdavidson
// Added Momentum
//
// Revision 1.14  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.13  2006/10/03 20:13:55  geconomos
// fixed size rendering support added
//
// Revision 1.12  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.11  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.10  2005/01/17 15:36:14  geconomos
// leaving the display of patient and image info to it's default setting
//
// Revision 1.9  2004/12/08 13:10:48  frank
// fixed flight (cursor 3D no longer needed to move camera)
//
// Revision 1.8  2004/11/17 22:31:59  michael
// using SetFixedImageSize() and no 2nd class environment citizen ProgressiveRefinement
//
// Revision 1.7  2004/10/29 20:18:07  frank
// formatting
//
// Revision 1.6  2004/10/18 14:50:18  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.5  2004/08/30 23:59:08  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.4  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.3  2004/08/12 20:34:18  frank
// added backwards flight
//
// Revision 1.2  2004/08/09 12:21:23  frank
// added forward/backward flight
//
// Revision 1.1  2004/06/15 15:11:56  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationAction3DPath.cpp,v 1.15 2007/01/16 17:12:32 gdavidson Exp $
// $Id: v3D_AnimationAction3DPath.cpp,v 1.15 2007/01/16 17:12:32 gdavidson Exp $
