// $Id: vxManipulatorSculpt.C,v 1.20 2007/01/11 15:29:08 romy Exp $
//
// Copyright ©2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorSculpt.h"
#include "vxManipulatorUtils.h"
#include "vxBlockVolumeIterator.h"
#include "vxUtils.h"
#include "vxComponentUtils.h"
#include "vxContour.h"
#include "vxMouseCursor.h"
#include "Timer.h"
#include "Point2D.h"
#include "Vector3D.h"
#include "vxViewerTypes.h"
#include "vxMessageBox.h"
#include "vxUndoActionSegmentation.h"


// defines
#define FILE_REVISION "$Revision: 1.20 $"


// static member initialization
const bool vxManipulatorSculpt::m_bShowTimingsInLog( true );


/**
 * Constructor
 *
 * @param environment the environment
 */
vxManipulatorSculpt::vxManipulatorSculpt( vxEnvironment & environment )
: vxManipulator( environment ),
m_bFullDepth( true ),
m_bQueryUser( false ),
m_maskImage( Triple<uint4>( 10, 10, 0 ), false )
{
} // vxManipulatorSculpt()


/**
 * Destructor
 */
vxManipulatorSculpt::~vxManipulatorSculpt()
{
  Reset();
} // vxManipulatorSculpt()


/**
 * reset everything to the initial state
 */
void vxManipulatorSculpt::Reset()
{
  m_bQueryUser = false;
  ClearLasso();
} // Reset()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorSculpt::OnSetCursor()
{
  vxMouseCursor::SetCursor( m_bQueryUser ? "CURSOR_SCULPT_CUT" : "CURSOR_SCULPT" );
  return true;
} // OnSetCursor()


/**
 * Event handler if the mouse button is pressed
 *
 * @param position  mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState state of the mouse
 *
 * @return true if handled
 */ 
bool vxManipulatorSculpt::OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  bool bIfHandled( false );

  try
  {
    if ( eButtonId == BUTTON_LEFT )
    {

      if ( m_bQueryUser )
        return bIfHandled;

      bIfHandled = true;

      GetEnvironment().GetSculptContour()->GetContourPtVector().clear();

      // store the point as the starting point of the contour.
      Point3Df normalizedMouseCoord( float4( position.m_x ) / ( GetEnvironment().GetWinRect().GetViewportWidth() - 1 ),
                                     float4( position.m_y ) / ( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ), 0.0F );
      GetEnvironment().GetSculptContour()->AddPoint( normalizedMouseCoord );
      GetEnvironment().GetSculptContour()->Modified( vxModIndInfo( vxContour, 0, 0 ) );

    } // if left button
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnButtonDown failed.", "vxManipulatorSculpt", "OnButtonDown" ) );
  }

  return bIfHandled;

} // OnButtonDown()


/**
 * Event handler if the mouse is moved
 *
 * @param position       mouse position
 * @param uKeyboardState mouse state
 *
 * @return true if handled
 */ 
bool vxManipulatorSculpt::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  bool bIfHandled(false);

  try
  {
    // if applicable, display warning that this operation cannot be restricted in oblique views
    vxManipulatorUtils::WarnShiftOblique(GetEnvironment(), uKeyboardState);

    if ( m_bQueryUser )
      return false;

    // return if list is empty.
    if ( GetEnvironment().GetSculptContour()->GetContourPtVector().size() == 0 )
      return bIfHandled;

    // clamp to the window size
    Point3Df mouseCoord( Bound( 0.0F, float4( position.m_x ), float4( GetEnvironment().GetWinRect().GetViewportWidth () - 1 ) ),
                         Bound( 0.0F, float4( position.m_y ), float4( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ) ), 0.0F  );

    // normalize
    Point3Df normalizedMouseCoord( mouseCoord.m_x / ( GetEnvironment().GetWinRect().GetViewportWidth () - 1 ), 
                                   mouseCoord.m_y / ( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ), 0.0F );

    // add the point to the contour.
    GetEnvironment().GetSculptContour()->AddPoint( normalizedMouseCoord );

    // Refresh all the views so as to incorporate the changes occurred in the move event.
    GetEnvironment().GetSculptContour()->Modified( vxModIndInfo( vxContour, 0, 0 ) );

  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnMouseMove failed.", "vxManipulatorSculpt", "OnMouseMove" ) );
  }

  return bIfHandled;

} // OnMouseMove()


/**
 * Event handler if the mouse is released
 *
 * @param position       mouse position
 * @param eButtonId      mouse button
 * @param uKeyboardState mouse state
 *
 * @return true if handled
 */ 
bool vxManipulatorSculpt::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{

  bool bHandled( false );

  try
  {
    if ( eButtonId == BUTTON_LEFT )
    {
      //keep this value incase you need to set this back for append to Background condition.
      SegParams::RegionAddOptionEnum regionGrowSelection = 
                  (SegParams::RegionAddOptionEnum)GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt();

      // if we are querying the user for the part to cut
      if ( m_bQueryUser )
      {
        // full depth when ctrl is down
        m_bFullDepth = ( uKeyboardState & vxInput::STATE_SHIFT ) == 0;

        m_bQueryUser = false;
        
     
        if ( GetEnvironment().GetSculptContour()->GetContourPtVector().size() > 0 )
        {
          // check the segmentation mode and query the user if append mode is slected
          // to make sure they really want to append - assuming they do not
          if ( GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::APPEND_REGIONS )
          {
            // make sure they aren't trying to append to none
            bool bAppendingToNone( GetEnvironment().GetComponents()->GetSelectedComponentIndex() == 0 );
            if ( bAppendingToNone )
            {
              std::string sComponentName( GetEnvironment().GetComponents()->ArrayByLabel()[ 0 ]->GetName() );
              vxMessageBox::ShowInfo( "Appending to " + sComponentName + " is not possible. A new component will be created instead" );
              GetEnvironment().GetInteractiveSegmentation()->SetRegionGrowAddOpt( SegParams::NEW_REGIONS );
              GetEnvironment().GetInteractiveSegmentation()->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE ) );
            }
          }

          CutLasso( GetEnvironment().GetSculptContour(), position );
          ClearLasso();
        }
      } // if querying user for the part to cut
      else
      {
        // if the annotations list is not empty
        if ( GetEnvironment().GetSculptContour()->GetContourPtVector().size() > 0 )
        {

          // close the contour
          GetEnvironment().GetSculptContour()->AddPoint( GetEnvironment().GetSculptContour()->GetContourPtVector().front() );
          m_bQueryUser = true;

        }
        GetEnvironment().GetSculptContour()->Modified( vxModIndInfo( vxContour, 0, 0 ) );

        vxMouseCursor::SetCursor( "CURSOR_MOVE" );

      } // if initial drawing phase of manipulation

      //Set the old regionGrow option
      GetEnvironment().GetInteractiveSegmentation()->SetRegionGrowAddOpt( regionGrowSelection );
    } // if left button
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnButtonUp failed.", "vxManipulatorSculpt", "OnButtonUp" ) );
  }


  return bHandled;

} // OnButtonUp()


/**
 * cut the lasso
 *
 * @param lasso the lasso annotation object
 * @param position mouse position
 */
void vxManipulatorSculpt::CutLasso( vxContour * pContour, Point2D<int2> position )
{
  try
  {

    // ensure that OpenGL is properly set up, and not just using the setup from the most-recently displayed view
    GetEnvironment().GetViewerType()->SetupView();

    vxMouseCursor waitCursor( "CURSOR_WAIT" );

    // convert the normalized mouse coordinates back to current mouse coordinates
    for ( std::list< Point3Df >::iterator iter( pContour->GetContourPtVector().begin() );
          iter != pContour->GetContourPtVector().end();
          ++iter )
    {
      Point3Df & pos = * iter;
      pos.m_x *= GetEnvironment().GetWinRect().GetViewportWidth() - 1;
      pos.m_y *= GetEnvironment().GetWinRect().GetViewportHeight() - 1;
    }

    // scan convert the polygon
    Triple<uint4> fullImageDim( GetEnvironment().GetWinRect().GetViewportWidth(),
                                GetEnvironment().GetWinRect().GetViewportHeight(), 0 );
    m_maskImage = Image<bool>( fullImageDim, false );
    pContour->FillPolygon( m_maskImage );

    // clamp the pick point to the window size
    Point2D<float4> mouseScreenPos( position.m_x, position.m_y );
    std::pair<uint2, uint2> viewportSize( GetEnvironment().GetWinRect().GetViewportSize() );
    mouseScreenPos.m_x = Bound( 0.0F, mouseScreenPos.m_x, float4( viewportSize. first - 1 ) );
    mouseScreenPos.m_y = Bound( 0.0F, mouseScreenPos.m_y, float4( viewportSize.second - 1 ) );
    Point3Df normalizedMouseCoord( float4( position.m_x ) / ( GetEnvironment().GetWinRect().GetViewportWidth() - 1 ),
                                   float4( position.m_y ) / ( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ), 0.0F );

    // choose keep inside or out depending on the "cut position"
    m_bKeepInside = ! m_maskImage.GetPixel( mouseScreenPos );

    // run through the voxels and project them to the image and see if we should remove them
    SculptVolume( m_maskImage );

  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "unhandled exception", "vxManipulatorSculpt", "CutLasso" ) );
  }

} // CutLasso()


/**
 * Clear out the lasso
 */
void vxManipulatorSculpt::ClearLasso()
{
  GetEnvironment().GetSculptContour()->GetContourPtVector().clear();
  GetEnvironment().GetSculptContour()->Modified( vxModIndInfo( vxContour, 0, 0 ) );
} // ClearLasso()


/**
 * Multiply two OpenGL projection matrices very quickly
 *
 * @param pfProduct the product matrix
 * @param pMatrixA  the first matrix
 * @param pMatrixB  the second matrix
*/
void vxManipulatorSculpt::MatrixMul( float4 * pfProduct, const GLdouble * pMatrixA, const GLdouble * pMatrixB )
{
  // alias the matrix values for visual clarity
  const GLdouble * a = pMatrixA;
  const GLdouble * b = pMatrixB;

  // multiply into a temp array
  float4 temp[16];

  temp[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
  temp[ 4] = a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7];
  temp[ 8] = a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11];
  temp[12] = a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15];

  temp[ 1] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
  temp[ 5] = a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7];
  temp[ 9] = a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11];
  temp[13] = a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15];

  temp[ 2] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
  temp[ 6] = a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7];
  temp[10] = a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11];
  temp[14] = a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15];

  temp[ 3] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
  temp[ 7] = a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7];
  temp[11] = a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11];
  temp[15] = a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15];

  // copy into product array
  memcpy( ( void * ) pfProduct, ( void * ) temp, 16 * sizeof( float4 ) );

} // MatrixMul()


/**
 * Project a point in 3D to the screen given the combined (modelview and projection)
 * matrix and the viewport.
 *
 * @param fObjX     the x world coordinate of the object
 * @param fObjY     the y world coordinate of the object
 * @param fObjZ     the z world coordinate of the object
 * @param mCombined the combined projection matrices
 * @param vViewport the viewport locations
 * @param iWinX     the output window x coordinate
 * @param iWinY     the output window y coordinate
 */
void vxManipulatorSculpt::ProjectToScreen( const float4 fObjX, const float4 fObjY, const float4 fObjZ,
                                           const float4 mCombined[16], const int4 vViewport[4],
                                           int4 & iWinX, int4 & iWinY )
{
  // Compute the homgenized coordinate 'w' first so we can check it for 0 before the divide.

  // Matrix(3,0)*x + Matrix(3,1)*y + Matrix(3,2)*z + Matrix(3,3)
  float4 fNormalizedDevCoordW( mCombined[3]*fObjX + mCombined[7]*fObjY + mCombined[11]*fObjZ + mCombined[15] );

  if ( fNormalizedDevCoordW != 0.0 )
  {
    // Matrix(0,0)*x + Matrix(0,1)*y + Matrix(0,2)*z + Matrix(0,3)
    float4 fNormalizedDevCoordX( ( mCombined[0]*fObjX + mCombined[4]*fObjY + mCombined[8]*fObjZ + mCombined[12] ) / fNormalizedDevCoordW );

    // Matrix(1,0)*x + Matrix(1,1)*y + Matrix(1,2)*z + Matrix(1,3)
    float4 fNormalizedDevCoordY( ( mCombined[1]*fObjX + mCombined[5]*fObjY + mCombined[9]*fObjZ + mCombined[13] ) / fNormalizedDevCoordW );

    // The range of Normalized Device Coordinates is (-1::1) So this is (1+NDC)*viewport/2, not NDC*viewport
    iWinX = vViewport[0] + ( 1.0 + fNormalizedDevCoordX ) * vViewport[2] / 2.0;
    iWinY = vViewport[1] + ( 1.0 + fNormalizedDevCoordY ) * vViewport[3] / 2.0;
  }
  else
  {
    // Homogenous coordinate is 0. Set the screen point to lower left corner
    iWinX = vViewport[0];
    iWinY = vViewport[1];
  } // endif valid NDC

} // ProjectToScreen()


/**
 * Process a subcube of the dataset deciding which voxels belong inside and outside
 *
 * @param iterVolume      the intensity volume iterator
 * @param iterLabel       the label volume iterator
 * @param uStartZ         min z block boundary
 * @param uStartY         min y block boundary
 * @param uStartX         min x block boundary
 * @param mCombinedMatrix the combined OpenGL projection and model matrices
 * @param vViewport       the OpenGL viewport
 * @param volUnits        the volume units
 * @param maskImage       the mask image that tells whether to keep or discard voxels that land there
 * @param bKeepInside     whether to keep 
 * @param visibleBounds   the visible bounds of the screen
 * @param iRemoveAllCount the count of the number of blocks removed en masse
 * @param iLeaveAllCount  the count of the number of blocks left alone
 * @param iSlowCount      the count of the number of blocks processed voxel-by-voxel
 * @param bAnyRemoved     if any of the voxels have been removed by processing
 */
void vxManipulatorSculpt::ProcessCube( vxBlockVolumeIterator<bool> & iterVolume, 
                                       vxBlockVolumeIterator<uint2> & iterLabel,  bool * pbLocked, 
                                       const uint4 uStartZ, const uint4 uStartY, const uint4 uStartX,
                                       const uint4 uStopZ , const uint4 uStopY , const uint4 uStopX,
                                       const float4 mCombinedMatrix[16],
                                       const int4 vViewport[4],
                                       Triple<float4> volUnits,
                                       Image<bool> & maskImage,
                                       bool bKeepInside,
                                       Box<uint4> & visibleBounds, 
                                       int4 & iRemoveAllCount, int4 & iLeaveAllCount, int4 & iSlowCount,
                                       bool & bAnyRemoved )
{
  // early return (for speed) if the cube is outside the visible bounds
  if ( uStartX > visibleBounds.GetMaxX() || uStopX < visibleBounds.GetMinX() ||
       uStartY > visibleBounds.GetMaxY() || uStopY < visibleBounds.GetMinY() ||
       uStartZ > visibleBounds.GetMaxZ() || uStopZ < visibleBounds.GetMinZ() )
  {
    return;
  }

  // see if its fully within the visible bounds
  bool bRequireClipping( true );
  if ( uStartX >= visibleBounds.GetMinX() && uStopX <= visibleBounds.GetMaxX() &&
       uStartY >= visibleBounds.GetMinY() && uStopY <= visibleBounds.GetMaxY() &&
       uStartZ >= visibleBounds.GetMinZ() && uStopZ <= visibleBounds.GetMaxZ() )
  {
    bRequireClipping = false;
  }

  // prepare to project to the screen
  int4 iWinCoordsX(0);
  int4 iWinCoordsY(0);
  int4 iWinCoordsXMax(0);
  int4 iWinCoordsYMax(0);
  int4 iWinCoordsXMin(0);
  int4 iWinCoordsYMin(0);

  // This is an optimization: 
  // Project corner vertices, determine screen space bounding box
  // of the eight projected points and scan through this box. If
  // box is fully outside or inside, then the decision is easy. :-)
  ///////////////////////////////////////////////////////////////////
  ProjectToScreen((uStartX*volUnits.m_x), (uStartY*volUnits.m_y), (uStartZ*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  iWinCoordsXMax = iWinCoordsXMin = iWinCoordsX;
  iWinCoordsYMax = iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen(((uStopX-1)*volUnits.m_x), (uStartY*volUnits.m_y), (uStartZ*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen((uStartX*volUnits.m_x), ((uStopY-1)*volUnits.m_y), (uStartZ*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen(((uStopX-1)*volUnits.m_x), ((uStopY-1)*volUnits.m_y), (uStartZ*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen((uStartX*volUnits.m_x), (uStartY*volUnits.m_y), ((uStopZ-1)*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen(((uStopX-1)*volUnits.m_x), (uStartY*volUnits.m_y), ((uStopZ-1)*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen((uStartX*volUnits.m_x), ((uStopY-1)*volUnits.m_y), ((uStopZ-1)*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;
  ProjectToScreen(((uStopX-1)*volUnits.m_x), ((uStopY-1)*volUnits.m_y), ((uStopZ-1)*volUnits.m_z),mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY);
  if (iWinCoordsX > iWinCoordsXMax) iWinCoordsXMax = iWinCoordsX;
  if (iWinCoordsY > iWinCoordsYMax) iWinCoordsYMax = iWinCoordsY;
  if (iWinCoordsX < iWinCoordsXMin) iWinCoordsXMin = iWinCoordsX;
  if (iWinCoordsY < iWinCoordsYMin) iWinCoordsYMin = iWinCoordsY;

  // check if the entire block is outside the image
  bool bAllOutsideImage( false );
  if ( iWinCoordsXMax < 0 || iWinCoordsXMin > vViewport[2]-1 || iWinCoordsYMax < 0 || iWinCoordsYMin > vViewport[3]-1 )
    bAllOutsideImage = true;

  if (iWinCoordsXMax <  0           ) iWinCoordsXMax = 0;
  if (iWinCoordsXMax >= vViewport[2]) iWinCoordsXMax = vViewport[2]-1;
  if (iWinCoordsYMax <  0           ) iWinCoordsYMax = 0;
  if (iWinCoordsYMax >= vViewport[3]) iWinCoordsYMax = vViewport[3]-1;
  if (iWinCoordsXMin <  0           ) iWinCoordsXMin = 0;
  if (iWinCoordsXMin >= vViewport[2]) iWinCoordsXMin = vViewport[2]-1;
  if (iWinCoordsYMin <  0           ) iWinCoordsYMin = 0;
  if (iWinCoordsYMin >= vViewport[3]) iWinCoordsYMin = vViewport[3]-1;

  bool bRemoveAll = true;
  bool bLeaveAll  = true;

  if (bAllOutsideImage)
  {
    if (bKeepInside)
    {
      bRemoveAll = true;
      bLeaveAll = false;
    }
    else
    {
      bLeaveAll = true;
      bRemoveAll = false;
    }
  }
  else
  {
    // at least partly inside image
    for (uint4 y(iWinCoordsYMin); y<=iWinCoordsYMax; y++)
    {
      for (uint4 x(iWinCoordsXMin); x<=iWinCoordsXMax; x++)
      {
        if (bKeepInside == true)
        {
          if (maskImage.GetPixelRaw(Point2D<uint2>(x, vViewport[3] - y - 1)) == false)
            bLeaveAll  = false;
          else
            bRemoveAll = false;
        }
        else
        {
          if (maskImage.GetPixelRaw(Point2D<uint2>(x, vViewport[3] - y - 1)) == true)
            bLeaveAll  = false;
          else
            bRemoveAll = false; 
        }
      }
    }
  } // if all outside image

  if ( bRequireClipping )
  {
    bRemoveAll = false;
    bLeaveAll = false;
  }

  // initialize iterator
  iterVolume.SetPos( uStartX, uStartY, uStartZ );

  //iterVolume
  iterLabel.SetPos( iterVolume.GetPos() );

  // apply a test to see if the mask prevents removing all
  if ( iterLabel.GetCurrentBlock().IsHomogenous() == false || pbLocked[ iterLabel.GetVoxel() ]  )
  {
    bRemoveAll = false;
  }
  if ( iterLabel.GetCurrentBlock().IsHomogenous() == true && pbLocked[ iterLabel.GetVoxel() ] )
  {
    bLeaveAll = true;
  }

  if ( bRemoveAll == true )
  {
    iterVolume.ClearCurrentBlock();
    bAnyRemoved = true;
    ++iRemoveAllCount;
  }
  else if ( bLeaveAll == false )
  {
    if ( bKeepInside == true )
    {
      for ( ;
            iterVolume.IsNotAtEndOfBlock() && iterLabel.IsNotAtEndOfBlock();
            iterVolume.NextZYXinsideBlock(), iterLabel.NextZYXinsideBlock() )
      {
        const int4 iPosVolumeX( iterVolume.GetPosX() );
        const int4 iPosVolumeY( iterVolume.GetPosY() );
        const int4 iPosVolumeZ( iterVolume.GetPosZ() );

        // see if the voxel is within the visible bounds
        if ( iPosVolumeX >= visibleBounds.GetMinX() && iPosVolumeX <= visibleBounds.GetMaxX() &&
             iPosVolumeY >= visibleBounds.GetMinY() && iPosVolumeY <= visibleBounds.GetMaxY() &&
             iPosVolumeZ >= visibleBounds.GetMinZ() && iPosVolumeZ <= visibleBounds.GetMaxZ() )
        {
          const float4 fPosX( iPosVolumeX * volUnits.m_x );
          const float4 fPosY( iPosVolumeY * volUnits.m_y );
          const float4 fPosZ( iPosVolumeZ * volUnits.m_z );
          ProjectToScreen( fPosX, fPosY, fPosZ, mCombinedMatrix, vViewport, iWinCoordsX, iWinCoordsY );

          if ( pbLocked[ iterLabel.GetVoxel()] == false )
          {
            if ( iWinCoordsX < 0 || iWinCoordsX >= vViewport[2] || iWinCoordsY < 0 || iWinCoordsY >= vViewport[3] ||
                 maskImage.GetPixelRaw( Point2D<uint2>( iWinCoordsX, vViewport[3] - iWinCoordsY - 1) ) == false )
            {
              iterVolume.SetVoxel( true );
              bAnyRemoved = true;
            }
          }
        }
      }
    }
    else
    {
      for ( ;
            iterVolume.IsNotAtEndOfBlock() && iterLabel.IsNotAtEndOfBlock();
            iterVolume.NextZYXinsideBlock(),  iterLabel.NextZYXinsideBlock() )
      {
        const int4 iPosVolumeX( iterVolume.GetPosX() );
        const int4 iPosVolumeY( iterVolume.GetPosY() );
        const int4 iPosVolumeZ( iterVolume.GetPosZ() );

        // see if the voxel is within the visible bounds
        if ( iPosVolumeX >= visibleBounds.GetMinX() && iPosVolumeX <= visibleBounds.GetMaxX() &&
             iPosVolumeY >= visibleBounds.GetMinY() && iPosVolumeY <= visibleBounds.GetMaxY() &&
             iPosVolumeZ >= visibleBounds.GetMinZ() && iPosVolumeZ <= visibleBounds.GetMaxZ() )
        {
          const float4 fPosX( iPosVolumeX * volUnits.m_x );
          const float4 fPosY( iPosVolumeY * volUnits.m_y );
          const float4 fPosZ( iPosVolumeZ * volUnits.m_z );
          ProjectToScreen( fPosX, fPosY, fPosZ, mCombinedMatrix, vViewport,iWinCoordsX, iWinCoordsY );

          if ( pbLocked[ iterLabel.GetVoxel()] == false )
          {
            if ( iWinCoordsX >= 0 && iWinCoordsX < vViewport[2] && iWinCoordsY >= 0 && iWinCoordsY < vViewport[3] &&
                 maskImage.GetPixelRaw( Point2D<uint2>( iWinCoordsX, vViewport[3] - iWinCoordsY - 1 ) ) )
            {
              iterVolume.SetVoxel( true );
              bAnyRemoved = true;
            }
          }
        }
      }

    }
    ++iSlowCount;
  }
  else
  {
    ++iLeaveAllCount;
  }

} // ProcessCube()


/**
 * Perform the actual sculpting
 *
 * @param maskImage the mask image
 */
void vxManipulatorSculpt::SculptVolume( Image<bool> & maskImage )
{
  // save the current state for undo purposes
  vxUndoActionSegmentation * pUndo = new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(), GetEnvironment().GetComponents() );

  try
  {
    // set it up as it is originally
    const vxWinRect & winRect = GetEnvironment().GetWinRect(); 

    // set mouse cursor
    vxMouseCursor cursor( "CURSOR_WAIT" );

    // initialize timer
    Timer timer;

    // get the current OpenGL matrices
    GLdouble mModelMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mModelMatrix );
    GLdouble mProjMatrix[16];
    glGetDoublev( GL_PROJECTION_MATRIX, mProjMatrix );
    const int4 vViewport[4] = { 0, 0, winRect.GetViewportSize().first, winRect.GetViewportSize().second };
    Triple<float4> volUnits( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );

    // write timing into log
    LogDbg( "SculptVolume - Setup OpenGL took: " + ToAscii( timer.ElapsedTime() ), "vxManipulatorSculpt", "SculptVolume" );
    timer.Reset();

    // get the current slab thickness to limit sculpting to the visible voxels
    Box<uint4> visibleBounds( vxUtils::GetActiveBoundingBox( ! m_bFullDepth, GetEnvironment().GetIntensityVolume(),
        GetEnvironment().GetViewerType(), GetEnvironment().GetPlaneOrtho(), GetEnvironment().GetRenderingMode(),
        GetEnvironment().GetOffset(), GetEnvironment().GetMaximumOffset(), GetEnvironment().GetCropBox() ) );

    //Get the Label volume iterator
    vxBlockVolumeIterator<uint2> iterLabel (* GetEnvironment().GetLabelVolume() );

    //pre-compute the component lock information
    bool * pbLocked = new bool[ GetEnvironment().GetComponents()->ArrayByLabel().size() ]; 
    for ( int i=0; i< GetEnvironment().GetComponents()->ArrayByLabel().size(); i++ )
      pbLocked[ i ] = GetEnvironment().GetComponents()->ArrayByLabel()[ i ]->IsLocked();

    // create a new component volume
    vxBlockVolume<bool> newComponentVolume( GetEnvironment().GetLabelVolume()->GetHeader() );
    vxBlockVolumeIterator<bool> iterRegionVol( newComponentVolume );
    iterRegionVol.SetClearBlock( true );

    // write timing into log
    LogDbg( "SculptVolume - Setup volume took: " + ToAscii( timer.ElapsedTime() ), "vxManipulatorSculpt", "SculptVolume" );
    timer.Reset();

    // do the projection
    float4 mCombinedMatrix[16];
    MatrixMul( mCombinedMatrix, mProjMatrix, mModelMatrix );
    vxBlockVolume<uint2> * pVolume = GetEnvironment().GetIntensityVolume();

    // alias the size of the volume
    const uint4 uSizeX( pVolume->GetHeader().GetVolDim().X() );
    const uint4 uSizeY( pVolume->GetHeader().GetVolDim().Y() );
    const uint4 uSizeZ( pVolume->GetHeader().GetVolDim().Z() );

    // initialize the performance counters
    int4 iRemoveAllCount( 0 );
    int4 iLeaveAllCount( 0 );
    int4 iSlowCount( 0 );
    bool bAnyRemoved( false );

    // loop over blocks and process
    const int4 iBlockSize( 16 );
    for ( uint4 z(0); z<uSizeZ; z+=iBlockSize )
    {
      for ( uint4 y(0); y<uSizeY; y+=iBlockSize )
      {
        for ( uint4 x(0); x<uSizeX; x+=iBlockSize )
        {
          ProcessCube( iterRegionVol, iterLabel, pbLocked, z, y, x,
                       min(z+iBlockSize,uSizeZ), min(y+iBlockSize,uSizeY), min(x+iBlockSize,uSizeX),
                       mCombinedMatrix, vViewport, volUnits, maskImage, m_bKeepInside, visibleBounds,
                       iRemoveAllCount, iLeaveAllCount, iSlowCount, bAnyRemoved );
        }
      }
    }

    // write timing into log
    if ( m_bShowTimingsInLog )
      LogDbg( "SculptVolume - Update Voxels took: " + timer.AsString() + " ra=" + ToAscii( iRemoveAllCount ) + " la=" + ToAscii( iLeaveAllCount ) + " s=" + ToAscii( iSlowCount ), "vxManipulatorSculpt", "SculptVolume" );

    timer.Reset();

    if ( m_bShowTimingsInLog )
      LogDbg( "SculptVolume - Update binary histogram took: " + timer.AsString(), "vxManipulatorSculpt", "SculptVolume" );

    // if any voxels were selected then allow change, otherwise notify user to try again
    if ( bAnyRemoved )
    {
      const uint2 uLabel = vxComponentUtils::CreateNewComponent( newComponentVolume,
        vxComponentUtils::GetUniqueComponentName( * GetEnvironment().GetComponents(), "Sculpt" ), GetEnvironment(), false );

      uint2 uHistoryIndex( GetEnvironment().GetComponents()->GetHistoryFromLabel( uLabel ) );
      const bool bNewRegion( GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::NEW_REGIONS );
      if ( bNewRegion )
      {
        // the component density and label visibility off if a new region
        ( * GetEnvironment().GetComponents() )[ uHistoryIndex ]->SetIntensityVisible( false );
        ( * GetEnvironment().GetComponents() )[ uHistoryIndex ]->SetLabelVisible( false );
        ( * GetEnvironment().GetComponents() )[ uHistoryIndex ]->SetNameEditable( true );
      }

      // let everyone know that the volume has changed
      GetEnvironment().GetLabelVolume()->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
      GetEnvironment().GetComponents()->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED ) );

      // save the undo information - relinquish control of memory too
      vxEnvironment::GetUndoActions().PushAction( pUndo );
      pUndo = NULL;

      // notify the feedback so that manipulator could be removed
      GetEnvironment().GetInputFeedback().Add( vxFeedback( typeid( * this ), vxFeedback::DONE ) );
    }
    else
    {

      std::string sMessage( "The area had no selectable voxels.\n" );
      sMessage += "Please unlock any locked components, adjust your thresholds and try again.";

      vxMessageBox::ShowError( sMessage );

    }

    // write timing into log
    if ( m_bShowTimingsInLog )
      LogDbg( "done with sculpting operation", "vxManipulatorSculpt", "SculptVolume" );

  } // try
  catch ( ... )
  {
    Reset();
    throw ex::VException( LogRec( "SculptVolume failed.", "vxManipulatorSculpt", "SculptVolume" ) );
  }

  // clear undo information
  if ( pUndo != NULL )
  {
    delete pUndo;
    pUndo = NULL;
  }

} // SculptVolume()


/**
 * Event handler if a key is pressed
 *
 * @param uKeyId         the key pressed
 * @param uKeyboardState key state
 *
 * @return true if handled
 */ 
bool vxManipulatorSculpt::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bIfHandled( false );

  try
  {
    switch ( uKeyId )
    {

    case vxInput::KEY_ESCAPE:
      Reset();
      OnSetCursor();
      break;

    case vxInput::KEY_SHIFT:
      // if applicable, display warning that this operation cannot be restricted in oblique views
      vxManipulatorUtils::WarnShiftOblique(GetEnvironment(), uKeyboardState);
      break;

    default:
      break;

    } // switch key
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnKeyPressed failed.", "vxManipulatorSculpt", "OnKeyPressed" ) );
  }

  return bIfHandled;

} // OnKeyPressed()


#undef FILE_REVISION


// $Log: vxManipulatorSculpt.C,v $
// Revision 1.20  2007/01/11 15:29:08  romy
// Sub issue of #5273 fixed. Reseting the user selction option if the background component append option is true.
//
// Revision 1.19  2006/07/06 12:58:04  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.18  2006/06/30 15:11:55  romy
// fixed the Lock component check
//
// Revision 1.17  2006/04/24 14:15:21  romy
// removed the 'locked voxel mask' from sculpt operation
//
// Revision 1.16  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.15  2006/03/02 15:48:08  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.14  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.13  2006/02/17 15:13:33  frank
// working on undoing all segmentation actions
//
// Revision 1.12  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.11  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.10  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.9.2.3  2005/08/26 13:46:21  frank
// cleaned up some confusing message boxes for sculpting with append
//
// Revision 1.9.2.2  2005/08/26 12:30:58  frank
// Issue #4401: Fixed messages referring to the NONE component
//
// Revision 1.9.2.1  2005/07/25 20:10:41  frank
// Issue #4304: Fixed problem when sculpting or contouring locked voxels
//
// Revision 1.9  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.8  2001/01/03 19:30:08  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.7  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.6.1  2005/02/15 19:50:04  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.6  2004/04/06 16:25:49  frank
// unified environment access naming
//
// Revision 1.5  2004/04/06 13:31:23  frank
// warned the user during sculpts
//
// Revision 1.4  2004/04/05 15:25:33  frank
// fixed sculpting bug when sculpting at bottom of image
//
// Revision 1.3  2004/03/31 17:45:47  frank
// unused include files
//
// Revision 1.2  2004/03/12 19:51:22  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.105  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.104  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.103  2004/01/14 16:56:05  frank
// cleared data when the manipulator is destructed
//
// Revision 1.102  2004/01/13 13:15:14  frank
// Fixed so that the sculpting and segmentation works properly on views derived from the oblique viewer type
//
// Revision 1.101  2004/01/12 14:19:56  frank
// fixed problem with manipulating on oblique with shift held down
// also made it possible to sculpt without rotating the oblique plane
//
// Revision 1.100  2003/12/16 20:20:30  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.99  2003/11/20 18:27:00  frank
// Fixed new component creation procedure
//
// Revision 1.98  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.97  2003/11/05 01:18:46  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.96  2003/07/07 13:58:53  geconomos
// Using new mouse cursors.
//
// Revision 1.95  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.94  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.93  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.92  2003/04/28 16:59:25  michael
// fixed issue #3116 and 3065
//
// Revision 1.91  2003/04/09 15:05:31  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.90  2003/04/08 17:33:56  michael
// fixed setting of maximum component index number
//
// Revision 1.89  2003/04/08 17:28:28  michael
// took out redundant updates of the binary histogram in sculpting
//
// Revision 1.88  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.87  2003/04/07 18:08:03  frank
// Refactored code to improve reusability and maintainability.
//
// Revision 1.86  2003/03/31 18:57:40  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.85  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.84  2003/03/24 14:12:36  frank
// Fixed creation of multiple aborted lassos
//
// Revision 1.83  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.82  2003/03/19 19:42:42  frank
// Removed debug
//
// Revision 1.81  2003/03/19 19:34:07  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.80  2003/03/13 16:30:13  frank
// spelling
//
// Revision 1.79  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.78  2003/02/18 16:40:54  frank
// Moved full-depth sculpting on CTRL to final click
//
// Revision 1.77  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.76  2003/02/05 23:08:28  frank
// Changed sculpting so you choose inside/outside after drawing.
//
// Revision 1.75.2.1  2003/02/12 22:51:02  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.75  2003/01/22 21:58:45  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.74  2002/12/20 16:14:55  frank
// Added x-ray projection
//
// Revision 1.73  2002/12/19 21:30:22  ingmar
// back to timings in log
//
// Revision 1.72  2002/12/12 18:35:22  ingmar
// made sure that locked componentsd are handles in block increments (not only voxel increments) whenever possible
//
// Revision 1.71  2002/11/27 22:44:29  michael
// added timing information
//
// Revision 1.70  2002/11/26 13:40:04  michael
// renamed method (GetHistoryFromLabel())
//
// Revision 1.69  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.68  2002/11/18 16:47:03  ingmar
// added update of the label vol binary histogram after completed sculpt operation
//
// Revision 1.67  2002/11/15 21:06:10  michael
// changed view types
//
// Revision 1.66  2002/11/13 00:11:46  michael
// account for changes in vxComponent and vxComponentArray
//
// Revision 1.65  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.64  2002/11/10 23:06:58  michael
// removed includes of texture server and update calls on textures
//
// Revision 1.63  2002/11/08 23:24:15  michael
// cosmetics
//
// Revision 1.62  2002/10/04 14:06:25  frank
// Added full depth sculpting on ctrl key.
//
// Revision 1.61  2002/09/13 13:48:02  jaddonisio
// Use index instead of label to set component parameters.
//
// Revision 1.60  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.59  2002/08/20 22:44:43  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.58  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.57  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.56.2.2  2002/07/11 22:37:05  michael
// took out the MakeCurrent() since this is ensured by V3D_Viewer and vxViewer
//
// Revision 1.56.2.1  2002/07/08 15:22:15  frank
// Restricted the sculpting to the visible voxels.
//
// Revision 1.56  2002/06/25 15:51:18  dmitry
// Moved mouse cursor up.
//
// Revision 1.55  2002/06/21 21:59:05  michael
// switch to view_changing while sculpting and changing window/level
// ... to render faster
//
// Revision 1.54  2002/06/20 20:49:56  frank
// Fixing up block sculpting on boundary.
//
// Revision 1.53  2002/06/20 19:02:48  michael
// fixed wrongly passed modified parameter
//
// Revision 1.52  2002/06/20 02:18:36  dmitry
// Using MouseCursor from the Environment.
//
// Revision 1.51  2002/06/19 15:13:42  jaddonisio
// Fixed logic in sculpt to respect locked components. (Frank)
//
// Revision 1.50  2002/06/14 20:14:03  geconomos
// Changde feedback typeids to be classes from pointer to classes.
//
// Revision 1.49  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.48  2002/06/13 23:22:36  michael
// added feedback information from the manipulators so that they can be removed
// from VB once operation is completed ...
//
// Revision 1.47  2002/06/11 04:28:08  frank
// Fixing sculpt problems.
//
// Revision 1.46  2002/05/23 16:01:56  michael
// removed comments from try/catch t bring it back in
//
// Revision 1.45  2002/05/23 15:59:54  michael
// fixed out of bounce problem by adding the point AFTER bounding it :-)
//
// Revision 1.44  2002/05/23 15:23:38  frank
// Clamped to window size.
//
// Revision 1.43  2002/05/20 22:54:46  frank
// Checked for case when zoom puts some blocks fully outside the image.
//
// Revision 1.42  2002/05/20 12:12:32  ingmar
// unique names for function UpdateTileTextures() -> UpdateTileTexturesLabels() and UpdateTileTexturesIntensities()
//
// Revision 1.41  2002/05/17 20:42:36  frank
// Didn't sculpt into locked voxels.
//
// Revision 1.40  2002/05/17 15:08:09  dmitry
// Removed setting up opengl view volume.
//
// Revision 1.39  2002/05/16 04:29:13  michael
// Sicne texmap server is multi-threaded, scultping and segmentation need to
// make sure that textures are already there before changing label volume.
// This is done by calling vxServerTexMap2d::EnsureTextureGenerationIsCompleted()
//
// Revision 1.38  2002/05/15 15:31:57  soeren
// added more exceptions
//
// Revision 1.37  2002/05/15 15:25:55  soeren
// added more exceptions
//
// Revision 1.36  2002/05/14 17:18:40  michael
// update component name only if new component is selected, else use current one
//
// Revision 1.35  2002/05/10 21:02:04  frank
// Changed the name of the sculpting component.
//
// Revision 1.34  2002/05/10 14:16:32  jaddonisio
// Modify mask tag volume so that scuplted region is locked.
//
// Revision 1.33  2002/05/04 17:00:28  soeren
// changed from logfyi to logdbg
//
// Revision 1.32  2002/05/04 11:47:41  michael
// cosmetics
//
// Revision 1.31  2002/05/03 18:25:17  soeren
// removed setclearcolor
//
// Revision 1.30  2002/05/03 18:19:47  soeren
// fixed clearcurrentblock
//
// Revision 1.29  2002/05/03 15:47:59  frank
// Fixed blocky outline, but still slow.
//
// Revision 1.28  2002/05/02 23:24:12  michael
// fixed update to component table (COMPONENT_ADDED)
//
// Revision 1.27  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.26  2002/05/02 19:21:42  frank
// typo
//
// Revision 1.25  2002/05/02 19:10:16  frank
// Changed sculpting to add a new component.
//
// Revision 1.24  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.23  2002/05/01 00:01:55  michael
// Adjusted the Modify calls to use the Modified on the object instead of the one in the environment
//
// Revision 1.22  2002/04/30 21:32:49  jenny
// vxBlockVolume stuff
//
// Revision 1.21  2002/04/29 16:35:15  dmitry
// Wait Cursor added.
//
// Revision 1.20  2002/04/26 19:43:26  frank
// Temporarily disabled new component access.
//
// Revision 1.19  2002/04/26 16:50:01  frank
// Found new component index.
//
// Revision 1.18  2002/04/26 15:09:59  michael
// added delta volume which should be use for texture update ...
// ... once component index for sculpting is correctly included
//
// Revision 1.17  2002/04/26 13:40:52  michael
// cosmetics
//
// Revision 1.16  2002/04/26 02:38:05  frank
// Adding comments.
//
// Revision 1.15  2002/04/25 17:55:29  dmitry
// Cursors added.
//
// Revision 1.14  2002/04/25 16:59:04  michael
// update intensity textures on sculpting (added update method for vxBlockVolume)
//
// Revision 1.13  2002/04/24 14:04:30  michael
// cosmetics
//
// Revision 1.12  2002/04/24 13:14:38  michael
// slight performance improvement on sculpting
//
// Revision 1.11  2002/04/24 03:06:30  michael
// accelerated sculpting by properly using the BlockVolume iterator ... :-)
//
// Revision 1.10  2002/04/24 01:10:02  kevin
// Second Checkin for speeding up scuplting uses Michaels algorithm of projecting corners of a cube.
// It turns out that this is much slower using the iterators with the new block volumes than the old
// linear volume with pointers. This should be considered in the future.
//
// Revision 1.9  2002/04/24 01:06:32  kevin
// First Checkin for Speeding up scuplting. This contains ifdefs with all the steps
//
// Revision 1.8  2002/04/20 18:26:00  michael
// structure
//
// Revision 1.7  2002/04/20 18:00:51  michael
// Replaced include by forward declaration
//
// Revision 1.6  2002/04/19 21:35:38  frank
// Added outside sculpting.
//
// Revision 1.5  2002/04/19 10:41:32  frank
// Basic sculpting works on 2D.
//
// Revision 1.4  2002/04/18 23:51:37  frank
// Removed dead code.
//
// Revision 1.3  2002/04/18 23:48:16  frank
// Got sculpting working in 3D.
//
// Revision 1.2  2002/04/18 23:06:03  frank
// Changed it to use screen pixels instead of world coordinates.
//
// Revision 1.1  2002/04/18 18:42:03  frank
// Initial version.
//
// $Id: vxManipulatorSculpt.C,v 1.20 2007/01/11 15:29:08 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorSculpt.C,v 1.20 2007/01/11 15:29:08 romy Exp $
