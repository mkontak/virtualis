// $Id: vxVesselUtilities.C,v 1.8 2008/04/08 13:28:32 cqian Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (gdavidson@viatronix.com)

#include "StdAfx.h"
#include "vxVesselUtilities.h"
#include "vxComponentArray.h"
#include "vxVessel.h"
#include "vxVolumeHeader.h"
#include "Point3D.h"
#include "vxRibbon.h"
#include "ValueRange.h"
#include "vxEnvironment.h"
#include "vxVesselSection.h"
#include "SingularValueDecomp.h"


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * gets the current vessel (or null)
 *
 * @param pComponents the component array
 *
 * @return the current vessel (or null)
 */
vxVessel * vxVesselUtilities::GetCurrentVessel( vxComponentArray * pComponents )
{

  vxComponent * pComponent = ( * pComponents )[ pComponents->GetSelectedComponentIndex() ];

  return dynamic_cast< vxVessel * >( pComponent->GetComponentDataPtr() );

} // GetCurrentVessel()


/**
 *	Get the reference center of the volume
 *  @param volHeader : volume header
 *  @return reference point
 */
Point3Df vxVesselUtilities::GetVolReferencePoint( const vxVolumeHeader & volHeader )
{
  Point3Df refercentPnt(0.0F, 0.0F, 0.0F);

  VarMap varmap( volHeader.GetVariableVolumeHeader() );  
  Point<float4> centerWorldPos;
  if ( varmap.GetHdrVar( "VolumeCenter", centerWorldPos ) )
  {
    const Vector3D<int4> vxDim( volHeader.GetVolDim() );
    const Triplef vxUnits( volHeader.GetVoxelUnits() );
    Point<float4> halfLen;
    halfLen.m_x = 0.5F*(vxDim.X()-1)*vxUnits.X();
    halfLen.m_y = 0.5F*(vxDim.Y()-1)*vxUnits.Y();
    halfLen.m_z = 0.5F*(vxDim.Z()-1)*vxUnits.Z();

    Point<float4> vRefPnt( centerWorldPos - halfLen );

    refercentPnt = vRefPnt;
  }

  return refercentPnt;

} // GetVolReferencePoint()


/**
 * Return the associated section at this position on the screen.
 *
 * @param iScreenY        the y position on the screen
 * @param pRibbon         pointer to the ribbon data structure
 * @param iViewportHeight the height of the screen
 *
 * @return                the interpolated section index, or < 0 if no section at screen position
 */
float4 vxVesselUtilities::GetVesselIndex( const int2 & iScreenY, vxRibbon * pRibbon, const int4 iViewportHeight )
{
  float4 fVesselSection( -1.0F );
  if ( pRibbon->GetScanlineData().size() <= 0 )
    return fVesselSection;

  float4 fScanline( Interpolate( float4( iViewportHeight - 1 ), float4( iScreenY ), 0.0F, 0.0F, float4( pRibbon->GetScanlineData().size() - 1 ) ) );

  if ( fScanline >= 0 && fScanline <= pRibbon->GetScanlineData().size()-1 )
    fVesselSection = pRibbon->GetScanlineData()[ fScanline ].m_fSectionIndex;

  return fVesselSection;

} // GetVesselIndex()


/**
 * Return the screen y scanline position given a section index.
 *
 * @param fSectionIndex   the section index
 * @param pRibbon         pointer to the ribbon data structure
 *
 * @return y scanline position
 */
float4 vxVesselUtilities::GetScanlinePosition( const float4 fSectionIndex, vxRibbon * pRibbon )
{
  float4 fScanline( -1.0F );
  ValueRange< int4 > iValidScanlines;
  const std::vector< vxRibbon::ScanlineData > & scanlines = pRibbon->GetScanlineData();

  ///TODO: WHY Scanline is less than 0
  if ( scanlines.size() <= 0 )
    return fScanline;

  for ( int i=0; i <scanlines.size()-1; ++i )
  {
    // keep track of the valid scanlines
    if ( scanlines[ i     ].m_fSectionIndex >= 0.0F ) iValidScanlines.AddValue( i     );
    if ( scanlines[ i + 1 ].m_fSectionIndex >= 0.0F ) iValidScanlines.AddValue( i + 1 );

    // see if we are within the range
    const float4 fSectionIndexThis( scanlines[ i     ].m_fSectionIndex );
    const float4 fSectionIndexNext( scanlines[ i + 1 ].m_fSectionIndex );

    // check for >= 0 section indexes
    if ( fSectionIndexThis >= 0.0F && fSectionIndexNext >= 0.0F )
    {
      if ( ( fSectionIndex >= fSectionIndexThis && fSectionIndex <= fSectionIndexNext ) ||
         ( fSectionIndex <= fSectionIndexThis && fSectionIndex >= fSectionIndexNext ) )
      {
        fScanline = Interpolate( fSectionIndexThis, fSectionIndex, fSectionIndexNext, float4( i ), float4( i + 1 ) );
      }
    } // section >= 0
  }

  // if we have not found it yet, then it's either off screen or at the very end of the valid data
  if ( fScanline == -1.0F )
  {
    // we haven't found it yet so determine start and end section indices
    ValueRange< float4 > fValidSectionIndexRange;
    fValidSectionIndexRange.AddValue( pRibbon->GetScanlineData()[ iValidScanlines.GetMinimum() ].m_fSectionIndex );
    fValidSectionIndexRange.AddValue( pRibbon->GetScanlineData()[ iValidScanlines.GetMaximum() ].m_fSectionIndex );

    const float4 fMinScanlineSection( scanlines[ iValidScanlines.GetMinimum() ].m_fSectionIndex  );
    const float4 fMaxScanlineSection( scanlines[ iValidScanlines.GetMaximum() ].m_fSectionIndex  );

    if ( fSectionIndex < fValidSectionIndexRange.GetMinimum() )
    {
      // choose the scanline with the minimum section index
      fScanline = ( fMinScanlineSection < fMaxScanlineSection ) ? iValidScanlines.GetMinimum() : iValidScanlines.GetMaximum();
    }
    if ( fSectionIndex > fValidSectionIndexRange.GetMaximum() )
    {
      // choose the scanline with the maximum section index
      const float4 fFirstSectionIndex( pRibbon->GetScanlineData()[ iValidScanlines.GetMinimum() ].m_fSectionIndex );
      const float4 fLastSectionIndex ( pRibbon->GetScanlineData()[ iValidScanlines.GetMaximum() ].m_fSectionIndex );
      fScanline = ( fFirstSectionIndex > fLastSectionIndex ) ? iValidScanlines.GetMinimum() : iValidScanlines.GetMaximum();
    }
  }

  return fScanline;
} // GetScanlinePosition()


/**
* Center the linked oblique view to this location along the vessel.
*
* @param position    the environment
* @param planeNormal the plane normal
* @param upDirection the up direction
*/
void vxVesselUtilities::RelocateNewObliqueView( vxShareableObject< vxCamera > * pCamera, vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique,
                                                const Point3D<float4> & position, const Normal<float4> & planeNormal, const Normal<float4> & upDirection )
{
  try
  {
    // get the original plane-camera distance
    float4 fOldDistance( 0.0F );
    if( pPlaneOblique->GetDistanceToIntersection( pCamera->GetEye(), pCamera->GetViewDirection(), fOldDistance ) == false )
      LogDbg( "ray does not intersect plane", "vxManipulatorVesselLuminal", "RelocateNewObliqueView" )

    // set the camera
    pCamera->SetEye( position - Vector<float4>( planeNormal ) * fOldDistance );
    pCamera->SetViewReference( position );
    pCamera->SetViewUp( upDirection );
    pCamera->Modified( vxModInfo( vxCamera, 0 ) );

    * pPlaneOblique = vxPlaneOblique<float4>( planeNormal, position );
    pPlaneOblique->Modified( vxModInfo( vxPlaneOblique<float4>, 0 ) );

  }
  catch ( ... )
  {
    LogDbg( "unhandled exception", "VascularUtilities", "RelocateNewObliqueView" );
  }

} // RelocateNewObliqueView()


/**
 * Utility function to determine the current scanline given a screen position.
 *
 * @return true if within the image
 */
bool vxVesselUtilities::GetScanline( float4 & fOutScanline, const float4 fScreenPosY, const vxWinRect & winRect, const uint4 uImageHeight )
{
  if ( fScreenPosY < 0 || fScreenPosY > winRect.GetViewportHeight() - 1 )
    return false;

  fOutScanline = Interpolate( 0.0F, fScreenPosY, float4( winRect.GetViewportHeight() - 1.0F ), 0.0F, ( uImageHeight - 1.0F ) );

  return true;
} // GetScanline()


/**
 * Orient the camera so that it lets the vessel rotate about any of four specific axes (x, y, z, v).
 *
 * @param environment the environment
 * @param uKeyId      the key that was pressed
 * @return            true if it did anything
 */
bool vxVesselUtilities::OrientCameraToVessel( vxEnvironment & environment, const uint1 uKeyId )
{
  vxRibbon * pRibbon = static_cast<vxRibbon *>(environment.GetElement( vxIDs::Ribbon ));
  if (pRibbon == NULL)
  {
    LogWrn( "Ribbon pointer is NULL", "vxManipulatorVesselCurvedProjected", "OrientCameraToVessel" );
    return false;
  }

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
  if ( pVessel == NULL )
  {
    //LogWrn( "Vessel pointer is NULL", "vxManipulatorVesselCurvedProjected", "OrientCameraToVessel" );
    return false;
  }

  vxCamera & camera = pRibbon->GetRefCamera();
  bool bUpdate( false );

  switch (uKeyId)
  {
  case 'x':
  case 'X': 
    bUpdate = true;
    camera.SetViewUp( Normal<float4>( 1.0f, 0.0f, 0.0f ) );
    camera.SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
    camera.SetViewReference( Point<float4>(0.0f, 0.0f, 1.0f) );
    pRibbon->SetOriModeOfMPR ( vxRibbon::X_AXIS ); 
    break;
  
  case 'y':
  case 'Y': 
    bUpdate = true;
    camera.SetViewUp( Normal<float4>( 0.0f, 1.0f, 0.0f ) );
    camera.SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
    camera.SetViewReference( Point<float4>(0.0f, 0.0f, 1.0f) );
    pRibbon->SetOriModeOfMPR( vxRibbon::Y_AXIS ); 
    break;
  
  case 'z':
  case 'Z': 
    bUpdate = true;
    camera.SetViewUp( Normal<float4>( 0.0f, 0.0f, 1.0f ) );
    camera.SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
    camera.SetViewReference( Point<float4>(0.0f, -1.0f, 0.0f) );
    pRibbon->SetOriModeOfMPR ( vxRibbon::Z_AXIS ); 
    break;

  case 'v':
  case 'V': 
  case 'd':
  case 'D': 
    { 
      bUpdate = true;
      pRibbon->SetOriModeOfMPR ( vxRibbon::VESSEL ); 
     /* try
      {
        float4 ** mCov;
        mCov = new float4 * [3];
        for( int4 i=0; i<3; i++ ) 
	      {
		      mCov[i] = new float4[3];
          mCov[i][0] = 0; 
          mCov[i][1] = 0; 
          mCov[i][2] = 0; 
        }
     
        for( int4 i=0; i<=pVessel->size()-1; i++ ) 
        {
          Point<float4> fP = pVessel->at(i).GetPosition();
          mCov[0][0] = mCov[0][0] + fP.m_x*fP.m_x;
          mCov[1][1] = mCov[1][1] + fP.m_y*fP.m_y;
          mCov[2][2] = mCov[2][2] + fP.m_z*fP.m_z;

          mCov[0][1] = mCov[0][1] + fP.m_x*fP.m_y;
          mCov[0][2] = mCov[0][2] + fP.m_x*fP.m_z;
          mCov[1][2] = mCov[1][2] + fP.m_y*fP.m_z;

          mCov[1][0] = mCov[0][1];
          mCov[2][0] = mCov[0][2];
          mCov[2][1] = mCov[1][2];
        }
      
        // initializing SVD class
	      SingularValueDecomp svd(4, 4);
	      // apply SVD procedure
	      svd.Solve( mCov, 3, 3 );
	      // get K-L matrix
	      svd.GetOrthoMatrixU( mCov );
        
        //the direction upwrads
        Vector<float4> upVect; 
        upVect.m_x = mCov[0][0];
        upVect.m_y = mCov[1][0];
        upVect.m_z = mCov[2][0];
       
        // find the right orientation up or down
        const Normal<float4> vXAxis( 1.0f, 0.0f, 0.0f );
        const Normal<float4> vYAxis( 0.0f, 1.0f, 0.0f );
        const Normal<float4> vZAxis( 0.0f, 0.0f, 1.0f );      
      
        const float4 fUpOnXProjection( upVect.Dot(vXAxis) );
        const float4 fUpOnYProjection( upVect.Dot(vYAxis) );
        const float4 fUpOnZProjection( upVect.Dot(vZAxis) );

        if ( fabs(fUpOnZProjection) > fabs(fUpOnYProjection) )
        {
          if ( fabs(fUpOnZProjection) > fabs(fUpOnXProjection) )  // most on Z axis
          {
            if ( fUpOnZProjection < 0.0F )
              upVect *= -1.0F;
          }
          else // most on X axis
          {
            if ( fUpOnXProjection < 0.0F )
              upVect *= -1.0F;
          }
        }
        else
        {
          if ( fabs(fUpOnYProjection) > fabs(fUpOnXProjection) )  // most on Y axis
          {
            if ( fUpOnYProjection < 0.0F )
              upVect *= -1.0F;
          }
          else //most on X axis
          {
            if ( fUpOnXProjection < 0.0F )
              upVect *= -1.0F;
          }
        }      

        //the scannin direction 
        Vector<float4> viewVect;
        viewVect.m_x = mCov[0][1];
        viewVect.m_y = mCov[1][1];
        viewVect.m_z = mCov[2][1];
        camera.SetViewUp( Normal<float4>(upVect) );
        camera.SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
        camera.SetViewReference( Point3Df(viewVect) );
      }
      catch( ...  ) 
      { 
    */
        /////////////////////////////////////////////////////////////////////////
        Point<float4> p0 = pVessel->at(0).GetPosition();
        Point<float4> p1 = pVessel->at( pVessel->size()-1 ).GetPosition();
        Vector<float4> upVect( p1 - p0 );
      
        // find the right orientation up or down
        const Normal<float4> vXAxis( 1.0f, 0.0f, 0.0f );
        const Normal<float4> vYAxis( 0.0f, 1.0f, 0.0f );
        const Normal<float4> vZAxis( 0.0f, 0.0f, 1.0f );      
      
        const float4 fUpOnXProjection( upVect.Dot(vXAxis) );
        const float4 fUpOnYProjection( upVect.Dot(vYAxis) );
        const float4 fUpOnZProjection( upVect.Dot(vZAxis) );

        if ( fabs(fUpOnZProjection) > fabs(fUpOnYProjection) )
        {
          if ( fabs(fUpOnZProjection) > fabs(fUpOnXProjection) )  // most on Z axis
          {
            if ( fUpOnZProjection < 0.0F )
              upVect *= -1.0F;
          }
          else // most on X axis
          {
            if ( fUpOnXProjection < 0.0F )
              upVect *= -1.0F;
          }
        }
        else
        {
          if ( fabs(fUpOnYProjection) > fabs(fUpOnXProjection) )  // most on Y axis
          {
            if ( fUpOnYProjection < 0.0F )
              upVect *= -1.0F;
          }
          else //most on X axis
          {
            if ( fUpOnXProjection < 0.0F )
              upVect *= -1.0F;
          }
        }      
      
        Vector<float4> viewVect;
        if ( upVect.GetLength() > 1.0f )
        { 
          if ( fabs(upVect.X()) > 1.0f || fabs(upVect.Y()) > 1.0f )
          {
            viewVect.m_x = upVect.Y();
            viewVect.m_y = -upVect.X();
            viewVect.m_z = 0.0f;
          }
          else
          {
            viewVect.m_x = upVect.Z();
            viewVect.m_y = 0.0;
            viewVect.m_z = -upVect.X();
          }
          camera.SetViewUp( Normal<float4>(upVect) );
          camera.SetEye( Point<float4>(0.0f, 0.0f, 0.0f) );
          camera.SetViewReference( Point3Df(viewVect) );
        }
      // }//catch
    }//case 
    break; 
    
    default:
    break;
  }

  if ( bUpdate )
  {
    environment.GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, 0 ) );
    environment.SetModified( vxIDs::Ribbon, true );
  }

  return bUpdate;
} // OrientCameraToVessel()


/**
 * Compute an interpolated contour
 *
 * @param outContour the output contour
 * @param contourA   the first contour
 * @param contourB   the second contour
 * @param fWeightA   the weight of contourA
 */
void vxVesselUtilities::GetInterpolatedContour( vxVesselSection::SectionContour & outContour,
                                                const vxVesselSection::SectionContour & contourA,
                                                const vxVesselSection::SectionContour & contourB,
                                                const ZeroOne & fWeightA )
{
  // check for bad case
  if ( contourA.size() != contourB.size() || contourA.size() == 0 || contourB.size() == 0 )
  {
    LogWrn( "contours are not of the same size, A="+ToAscii(contourA.size())+"; B="+ToAscii(contourB.size()), "VascularUtilities", "GetInterpolatedContour" );
    //outContour = contourA;
    return;
  }

  // interpolate
  outContour.resize( contourA.size() );
  vxVesselSection::SectionContour::const_iterator iterA( contourA.begin() );
  vxVesselSection::SectionContour::const_iterator iterB( contourB.begin() );
  vxVesselSection::SectionContour::iterator iterOut( outContour.begin() );
  for ( ; iterA != contourA.end(); ++iterA, ++iterB, ++iterOut )
  {
    * iterOut = Point3Df( Vector3Df( * iterA ) * fWeightA + Vector3Df( * iterB ) * ( 1.0F - fWeightA ) );
  }

} // GetInterpolatedContour()






#pragma region revision history

// $Log: vxVesselUtilities.C,v $
// Revision 1.8  2008/04/08 13:28:32  cqian
// remove the print command
//
// Revision 1.7  2008/04/03 22:40:17  gdavidson
// Changed the OrientationModeOfMPR enumeration
//
// Revision 1.6  2007/10/18 19:12:22  cqian
// MPR is changed
//
// Revision 1.5  2006/12/14 16:09:26  romy
// added scanline checks for VAS Crash
//
// Revision 1.4  2006/09/25 13:20:03  geconomos
// added "GetInterpolatedContour"
//
// Revision 1.3  2006/09/14 20:10:18  geconomos
// added OrientCameraToVessel
//
// Revision 1.2  2006/08/28 21:46:34  gdavidson
// Modified the parameters for RelocateNewObliqueView
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVesselUtilities.C,v 1.8 2008/04/08 13:28:32 cqian Exp $
// $Id: vxVesselUtilities.C,v 1.8 2008/04/08 13:28:32 cqian Exp $


#pragma endregion