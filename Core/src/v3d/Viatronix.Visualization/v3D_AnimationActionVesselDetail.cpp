// $Id: v3D_AnimationActionVesselDetail.cpp,v 1.8 2007/09/18 17:24:17 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )


// includes
#include "stdafx.h"
#include "v3D_AnimationActionVesselDetail.h"
#include "vxUtils.h"
#include "v3D_Cine.h"
#include "vxVessel.h"
#include "vxVesselUtilities.h"



// namespaces
USING_ANIMATION_NS

/**
* Initializes a AnimationActionVesselDetail object with the sepcified environment.
*
* @param   mpEnvironment    Environment object to animate.
*/ 
AnimationActionVesselDetail::AnimationActionVesselDetail( Viatronix::Visualization::Environment ^ mpEnvironment ) : 
AnimationAction( mpEnvironment ),
m_fStepSize( 1.0F ),
m_fVesselSelectionIndex( 1.0F)
{
} // AnimationActionVesselDetail()


/**
* Called when the environment is to be initialized.
*/
void AnimationActionVesselDetail::OnInitializeEnvironment()
{
	__super::OnInitializeEnvironment();

	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	this->Environment->Cine->CineMode = Viatronix::Visualization::CineModes::Idle;

	//Store the initial index
	m_fVesselSelectionIndex = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( pEnvironment->GetElement( vxIDs::CurrentVesselSection ) )->GetValue();

} // OnInitializeEnvironment()


/**
* Called when the environments's state is to be restored.
*/
void AnimationActionVesselDetail::OnRestoreEnvironment()
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );

	//reset the initial view
	dynamic_cast< vxShareableObject< vxElementFloat4 > * >( pEnvironment->GetElement( vxIDs::CurrentVesselSection ))->SetValue( m_fVesselSelectionIndex );
	SetObliqueView( m_fVesselSelectionIndex );

} // OnRestoreEnvironment()


/**
* Called after the last frame is rendered.
*/
void AnimationActionVesselDetail::OnEndAnimation( bool bPreview )
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	AnimationAction::OnEndAnimation( bPreview );

	//reset the initial view
	dynamic_cast< vxShareableObject< vxElementFloat4 > * >( pEnvironment->GetElement( vxIDs::CurrentVesselSection ))->SetValue( m_fVesselSelectionIndex );
	SetObliqueView( m_fVesselSelectionIndex );

} // EndAnimation()


/** 
* Gets the total number of frames for the animation.
*
* @return Total number of frames to render
*/
int AnimationActionVesselDetail::GetFramesToRender()
{
	// make sure we have a vessel
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	vxVessel * pSelectedVessel = vxVesselUtilities::GetCurrentVessel( pEnvironment->GetComponents() );
	if( pSelectedVessel == NULL )
		return 0;

	return static_cast< int >( pSelectedVessel->size());

} // GetFramesToRender()


/**
* Called before the next frame is to be rendered.
*
* @param iFrameNumber  The current frame to be rendered.
*/
void AnimationActionVesselDetail::OnPreRenderFrame( bool bPreview, int iFrameNumber )
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( pEnvironment->GetComponents() );
	if ( pVessel == NULL )
		return;

	SetObliqueView( iFrameNumber );

} //  OnPreRenderFrame()


/*
 * Sets the oblique View
 */
void AnimationActionVesselDetail::SetObliqueView( const float4 fVesselSectionIndex )
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( pEnvironment->GetComponents() );
	if ( pVessel == NULL )
		return;

	Point<float4> position( pVessel->GetResampledPosition( fVesselSectionIndex ) );
	Normal<float4> tangent( pVessel->GetResampledTangent( fVesselSectionIndex) );
	Normal<float4> upDirection( pVessel->GetResampledUpDirection( fVesselSectionIndex ) );
	Normal<float4> leftDirection( tangent.Cross( upDirection ) );

	Normal<float4> vYAxis(0.0f,-1.0f,0.0f);
	float4 fYOnUp( vYAxis.Dot(upDirection) );
	float4 fYOnLeft( vYAxis.Dot(leftDirection) );
	Vector<float4> vUpdatedUpDirection( fYOnUp*upDirection + fYOnLeft*leftDirection );
	if( vUpdatedUpDirection.GetLength() == 0 )
		vUpdatedUpDirection =  static_cast< Vector3Df >( upDirection );

	Normal<float4> vZAxis( 0.0F, 0.0F, 1.0F );
	Normal<float4> vView( tangent.Dot(vZAxis) < 0.0F ? -tangent : tangent );

	vxVesselUtilities::RelocateNewObliqueView( pEnvironment->GetCamera(), pEnvironment->GetPlaneOblique(), position, vView, Normal3Df( vUpdatedUpDirection ) );
	dynamic_cast< vxShareableObject< vxElementFloat4 > * >( pEnvironment->GetElement( vxIDs::CurrentVesselSection ) )->SetValue( fVesselSectionIndex );

}//SetObliqueView( float4 fVesselSectionIndex )



// $Log: v3D_AnimationActionVesselDetail.cpp,v $
// Revision 1.8  2007/09/18 17:24:17  romy
// adjusted the wrong frame number
//
// Revision 1.7  2007/08/30 15:58:47  romy
// Added code to consider the Start Position to calculate total Frames.
//
// Revision 1.6  2007/08/29 20:09:10  romy
// Code added to reset the view to the initial index after movie
//
// Revision 1.5  2007/08/21 20:11:30  romy
// Leave the image information options untouched
//
// Revision 1.4  2007/08/10 17:55:21  romy
// added vessel check before rendering
//
// Revision 1.3  2007/06/04 20:29:24  romy
// fov modified to 8mm
//
// Revision 1.2  2007/05/22 14:10:16  romy
// Turns the Image information off during movie making
//
// Revision 1.1  2007/05/14 14:20:32  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionVesselDetail.cpp,v 1.8 2007/09/18 17:24:17 romy Exp $
// $Id: v3D_AnimationActionVesselDetail.cpp,v 1.8 2007/09/18 17:24:17 romy Exp $
