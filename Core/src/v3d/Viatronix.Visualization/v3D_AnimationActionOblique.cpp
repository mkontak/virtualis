// $Id: v3D_AnimationActionOblique.cpp,v 1.10 2008/01/18 20:26:49 geconomos Exp $
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
#include "v3D_AnimationActionOblique.h"
#include "vxUtils.h"
#include "vxMathUtils.h"
#include "v3D_Cine.h"
#include "v3D_Dataset.h"
#include "v3D_ModifiedQueue.h"

#define FILE_REVISION "$Revision: 1.10 $"

// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS


/**
* Initializes a AnimationActionVesselDetail object with the sepcified environment.
*
* @param   mpEnvironment    Environment object to animate.
*/ 
AnimationActionOblique::AnimationActionOblique( Viatronix::Visualization::Environment ^ mpEnvironment ) : 
AnimationAction( mpEnvironment ),
m_fStepSize( 1.0F ),
m_iFrames( 0),
m_bAnimating( false )
{
} // AnimationActionVesselDetail()


/**
* Called when the environment is to be initialized.
*/
void AnimationActionOblique::OnInitializeEnvironment()
{
	__super::OnInitializeEnvironment();

	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	this->Environment->Cine->CineMode = Viatronix::Visualization::CineModes::Idle;

	m_fInitialDistance =  pEnvironment->GetPlaneOblique()->GetDistance();
	m_iFrames = 0;
	m_bAnimating = false;

	GetFramesToRender();

} // OnInitializeEnvironment()


/**
* Called when the environments's state is to be restored.
*/
void AnimationActionOblique::OnRestoreEnvironment()
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	const float4 dist = m_fInitialDistance;
	pEnvironment->GetPlaneOblique()->SetDistance( dist );
	pEnvironment->GetPlaneOblique()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );
	
	m_bAnimating = false;

} // OnRestoreEnvironment()


/**
* Called after the last frame is rendered.
*/
void AnimationActionOblique::OnEndAnimation( bool bPreview )
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	AnimationAction::OnEndAnimation( bPreview );

	const float4 dist = m_fInitialDistance;
	pEnvironment->GetPlaneOblique()->SetDistance( dist );
  pEnvironment->GetPlaneOblique()->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );

  Viatronix::Visualization::ModifiedQueue::Flush();

	m_bAnimating = false;
	
} // EndAnimation()




/** 
* Gets the total number of frames for the animation.
*
* @return Total number of frames to render
*/
int AnimationActionOblique::GetFramesToRender()
{
	// If the animation is in progress return the already calculated values. else calculate everytime
	if ( m_bAnimating )
		return m_iFrames;
	
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	const Triplef & units = pEnvironment->GetIntensityVolume()->GetHeader().GetVoxelUnits();

	float4 fUnitDistance(Min(Min(units[0], units[1]), units[2]));
	Triple<float4> worldDim = (pEnvironment->GetIntensityVolume()->GetHeader().GetWorldDim());
	Box<float4> box(0,0,0,worldDim.X(), worldDim.Y(), worldDim.Z() );
	vxPlaneOblique<float4> * pPlaneOblique(pEnvironment->GetPlaneOblique());
	float4 fMin(vxMathUtils::GetMinDistance(box, *pPlaneOblique));
	float4 fMax(vxMathUtils::GetMaxDistance(box, *pPlaneOblique));

	//Get the frames
	if ( fMax * fMin <= 0.0F)
	{
		if (fabs( pEnvironment->GetPlaneOblique()->GetNormal().GetMaxAbsComponent()) != 1.0F)
		{
			if( m_bAnimateForward )
				m_iFrames = (1 + (int4)(fabs(fMax)/fUnitDistance))/m_fStepSize;
			else
				m_iFrames = (1 + (int4)(fabs(fMin)/fUnitDistance))/m_fStepSize;
		}
		else
		{
			const float fEpsilon(0.001F*fUnitDistance);
			if( m_bAnimateForward )
				m_iFrames = (1 + (int4)((fabs(fMax)+fEpsilon)/fUnitDistance) )/m_fStepSize;
			else
				m_iFrames = (1 + (int4)((fabs(fMin+fEpsilon))/fUnitDistance))/m_fStepSize;
		}
	}

	//if ( m_bCycleDatasets )
	//	m_iFrames *= m_mpDatasetList->Count;

	return m_iFrames;
} // GetFramesToRender()



/**
* Called before the next frame is to be rendered.
*
* @param iFrameNumber  The current frame to be rendered.
*/
void AnimationActionOblique::OnPreRenderFrame( bool bPreview, int iFrameNumber )
{
	try
	{
		m_bAnimating = true;
		
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

		float4 fDistanceNew = GetNewPlaneDistance();
		SetPlaneDistance(fDistanceNew);

	}
	catch ( ... )
	{
		m_bAnimating = false;
		LogErr( "An error occurred in OnPreRenderFrame", "AnimationActionOblique", "OnPreRenderFrame" );
	}

} //  OnPreRenderFrame()


/**
* Gets the next plane disatnce based on the stepsize
*
* @return float4 - the new plane distance
*/
float4 AnimationActionOblique::GetNewPlaneDistance()
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );

	vxPlaneOblique<float4> * pPlane = dynamic_cast<vxPlaneOblique<float4> *>( pEnvironment->GetPlaneOblique());
	if ( pPlane != NULL )
	{
		Triplei dim = pEnvironment->GetIntensityVolume()->GetHeader().GetVolDim();
		const Triplef & units = pEnvironment->GetIntensityVolume()->GetHeader().GetVoxelUnits();
		Normal<float4> normal(pPlane->GetNormal());
		Box<float4> boxVolume(0, 0, 0, (dim[0]-1)*units[0], (dim[1]-1)*units[1], (dim[2]-1)*units[2]);
		float4 fDistance(pPlane->GetDistance());
		float4 fDistanceMin(vxMathUtils::GetMinDistance(boxVolume, *pPlane));
		float4 fDistanceMax(vxMathUtils::GetMaxDistance(boxVolume, *pPlane));
		float4 fDistanceUnit(Min(Min(units[0], units[1]), units[2]));
		float4 fDistanceMinMax(fDistanceMax-fDistanceMin);

		int4 iSign(1);
		if ( m_bAnimateForward )
			iSign = -1;

		float4 fStepDelta(iSign*fDistanceUnit * 1);
		float4 fDistanceNew(fDistance + fStepDelta);
	
		return fDistanceNew;
	}
	return 0.0F;
}//GetNewPlaneDistance()




/**
* Sets the plane disatnce 
*
* @param fDistanceNew - the new plane distance to be set
*/
void AnimationActionOblique::SetPlaneDistance( float4  fDistanceNew )
{
	vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( this->Environment->GetEnvironmentPtr() );
	vxPlaneOblique<float4> * pPlane = dynamic_cast<vxPlaneOblique<float4> *>( pEnvironment->GetPlaneOblique());
	if ( pPlane != NULL )
	{
		pPlane->SetDistance(fDistanceNew);
		pEnvironment->GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
	}
}//SetPlaneDistance( float4  fDistanceNew )



// $Log: v3D_AnimationActionOblique.cpp,v $
// Revision 1.10  2008/01/18 20:26:49  geconomos
// Issue# 5867: oblique image not resetting properly after movie creation
//
// Revision 1.9  2007/08/30 19:08:05  romy
// fixed the beating issue
//
// Revision 1.8  2007/08/30 18:35:05  romy
// modified the backward and forward steps calculatiions
//
// Revision 1.7  2007/08/29 21:24:36  romy
// fixed the wrong slice number indication
//
// Revision 1.6  2007/07/17 19:37:15  romy
// fixed the ortho movie creation issue
//
// Revision 1.5  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.4  2007/07/06 15:46:53  romy
// clean up time
//
// Revision 1.3  2007/07/06 15:44:13  romy
// clean up time
//
// Revision 1.2  2007/07/03 17:58:02  romy
// added code to fix the mismatch in number of slices
//
// Revision 1.1  2007/07/02 20:52:09  romy
// oblique movie added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionOblique.cpp,v 1.10 2008/01/18 20:26:49 geconomos Exp $
// $Id: v3D_AnimationActionOblique.cpp,v 1.10 2008/01/18 20:26:49 geconomos Exp $
