// $Id: vxControllerVesselDetail.C,v 1.15 2007/11/23 19:37:20 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxControllerVesselDetail.h"
#include "vxIDs.h"
#include "vxEnvironment.h"
#include "vxVessel.h"
#include "vxElementFloat4.h"
#include "vxVesselUtilities.h"
#include "vxViewerTypeVesselDetailMPROblique.h"
#include "vxViewerTypeVesselDetailMPROrtho.h"
#include "vxViewerTypeVesselDetail3D.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxManipulatorVessel.h"
#include "vxManipulatorVesselDetailMPROblique.h"


// defines
#define FILE_REVISION "$Revision: 1.15 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxControllerVesselDetail::vxControllerVesselDetail( vxEnvironment & environment ): vxController( environment ),
  m_pLastSelectedVessel( NULL ),
  m_bDraggingdEndoCameraForward( false ),
  m_fPreviousVesselSectionIndex( -1.0F )
{
} // vxControllerVesselDetail()


/**
 * Determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerVesselDetail::IsProcessingNecessary()
{
  // make sure we have a vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL )
    return false;

  // check if the vessel has changed or the viewer type has
  const bool bVesselChanged = pVessel != m_pLastSelectedVessel;
  const bool bVesselIndexChanged = GetEnvironment().IsModified( vxIDs::CurrentVesselSection );
  const bool bViewerTypeChanged = m_pLastViewerType != GetEnvironment().GetViewerType();

  // cache last viewer type and vessel
  m_pLastViewerType = GetEnvironment().GetViewerType();
  m_pLastSelectedVessel = pVessel;

  return bVesselChanged || bVesselIndexChanged || bViewerTypeChanged;
} // IsProcessingNecessary()


/**
 * Synchrozies the detail view to the current vessel section index.
 */
void vxControllerVesselDetail::Process()
{
  // determine if the selected vessel has changed
  // and change the camera if necessary
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL  )
    return;

  // check for a valid section index
  const float4 fVesselSectionIndex = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) )->GetValue();
  if ( fVesselSectionIndex >= 0.0F && fVesselSectionIndex <= pVessel->size() - 1 )
  {

    ///////////////////////////////
    // axial

    if(  dynamic_cast< vxViewerTypeVesselDetailMPROrtho * >( GetEnvironment().GetViewerType() ) )
    {
      // get the position along the centerline
      Point<float4> position( pVessel->GetResampledPosition( fVesselSectionIndex ) );

      // set the ortho plane location
      uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
      GetEnvironment().GetPlaneOrtho()->SetDistance( position[ uOrientation ] );
      GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );

    }


    ///////////////////////////////
    // oblique

    else if( dynamic_cast< vxViewerTypeVesselDetailMPROblique * >( GetEnvironment().GetViewerType() ) )
    {
      // handle the oblique view
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

			///Check if the user had modified the Zoom Level, If so use the custom one.
			if ( !vxManipulatorVesselDetailMPROblique::IsZoomFactorModified() )
			{
				//TODO: Reset the Eye to fix the Oblique blackout issue. 
				//Fix this after RSNA2007
				GetEnvironment().GetCamera()->SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
				GetEnvironment().GetCamera()->SetFovDegrees( 4 );
  		}

      vxVesselUtilities::RelocateNewObliqueView( GetEnvironment().GetCamera(), GetEnvironment().GetPlaneOblique(), position, vView, Normal3Df( vUpdatedUpDirection ) );
    }

  
    ///////////////////////////////
    // endoluminal

    else if( dynamic_cast< vxViewerTypeVesselDetailEndoluminal * >( GetEnvironment().GetViewerType() ) )
    {
      // handle the endoluminal view
      m_bDraggingdEndoCameraForward = ( m_fPreviousVesselSectionIndex == fVesselSectionIndex ) ? m_bDraggingdEndoCameraForward : m_fPreviousVesselSectionIndex < fVesselSectionIndex;
      
      // set the camera
			//commented out this code due to the issue of endoluminal fly goes out of Volume, Romy
      //Point<float4> refercentPnt( vxManipulatorVessel::GetDetailVolumeOffset( GetEnvironment() ) );
      const Point3Df position( pVessel->GetResampledPosition( fVesselSectionIndex ));// -Vector3Df( refercentPnt ) );
      Normal3Df viewDirection( pVessel->GetResampledTangent( fVesselSectionIndex ) );
      if ( ! m_bDraggingdEndoCameraForward ) viewDirection = - viewDirection;
      Normal3Df upDirection( pVessel->GetResampledUpDirection( fVesselSectionIndex ) );

      GetEnvironment().GetCamera()->SetEye( position );
      GetEnvironment().GetCamera()->SetViewDirection( viewDirection );
      GetEnvironment().GetCamera()->SetViewUp( upDirection );
			GetEnvironment().GetCamera()->SetFovDegrees( 60.0f );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
            
      m_fPreviousVesselSectionIndex = fVesselSectionIndex;

			//TODO: Reset the Eye to fix the Oblique blackout issue. 
			//Fix this after RSNA2007
			vxManipulatorVesselDetailMPROblique::ResetZoomFactor();
    }
  


    ///////////////////////////////
    // 3d detail

    else if( dynamic_cast< vxViewerTypeVesselDetail3D * >( GetEnvironment().GetViewerType() ) )
    {
      // get the center of the crop box
      Point<float4> cuttingBoxCenter = pVessel->GetResampledPosition( fVesselSectionIndex );

      const float4 fVesselDiameterMM = Max( 1.0F, pVessel->at( int4( fVesselSectionIndex ) ).GetMeanDiameter() );
      const float4 fBoxRadiusMM( 3 * fVesselDiameterMM );
      const Vector<float4> vBoxRadiusMM( fBoxRadiusMM, fBoxRadiusMM, fBoxRadiusMM );
      Point<float4> minPoint( cuttingBoxCenter - vBoxRadiusMM );
      Point<float4> maxPoint( cuttingBoxCenter + vBoxRadiusMM );
      Box<float4> cutBox( minPoint, maxPoint );

      // set the crop box
      GetEnvironment().GetCropBox()->SetBox( cutBox );
      GetEnvironment().GetCropBox()->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );       

      // Set the camera
      vxShareableObject< vxCamera > * pDetailCamera = GetEnvironment().GetCamera();
      Vector3Df viewDir = Vector3Df( pDetailCamera->GetViewDirection() );
      if ( viewDir.GetLength() == 0.0F )
        viewDir = Vector3Df( 1, 0, 0 );
      pDetailCamera->SetViewReference( cutBox.GetMidPoint() );
      pDetailCamera->SetEye( cutBox.GetMidPoint() - viewDir * 2.5 * cutBox.GetDimX() ); 
      vxUtils::CameraViewAll( * pDetailCamera, cutBox );
      pDetailCamera->Modified( vxModInfo( vxCamera,vxUtils::CAMERA_MODIFIED ) );
    }
  } // if valid
} // Process()


// $Log: vxControllerVesselDetail.C,v $
// Revision 1.15  2007/11/23 19:37:20  romy
// reset the Eye when it switches from Endo to Oblique
//
// Revision 1.14  2007/11/23 19:31:07  romy
// reset the Eye when it switches from Endo to Oblique
//
// Revision 1.13  2007/11/19 14:28:32  romy
// endoluminal position calculation changed due to the issue of endoluminal fly goes out of Volume
//
// Revision 1.12  2007/11/19 13:55:43  romy
// added SetFov to check the Endoluminal View
//
// Revision 1.11  2007/07/31 15:24:39  romy
// temporary fix added to keep the FOV to 4 for VesselDetailObliqueView
//
// Revision 1.10  2007/07/30 19:23:55  romy
// Uses the Custon FOV is user has selected one.
//
// Revision 1.9  2007/06/04 20:28:57  romy
// fov modified to 8mm
//
// Revision 1.8  2007/05/10 19:48:22  romy
// Field of view changed to 4cm, for Vessel Details Oblique plane
//
// Revision 1.7  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.6  2006/11/22 21:48:54  gdavidson
// Check if the ViewerType has changed.
//
// Revision 1.5  2006/11/17 21:32:11  geconomos
// added cheks to see if processing is necessary
//
// Revision 1.4  2006/09/25 20:27:13  geconomos
// minor cleanup
//
// Revision 1.3  2006/09/25 20:08:13  geconomos
// added support for 3d detail and endoluminal viewer types
//
// Revision 1.2  2006/09/25 13:25:55  geconomos
// using ViewerType instead of DetailViewMode to determine detail viewer type
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerVesselDetail.C,v 1.15 2007/11/23 19:37:20 romy Exp $
// $Id: vxControllerVesselDetail.C,v 1.15 2007/11/23 19:37:20 romy Exp $
