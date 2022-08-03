// $Id: vxManipulatorResearch.C,v 1.5 2006/04/24 17:53:42 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorResearch.h"
#include "vxEnvironment.h"
#include "ValueRange.h"
#include "vxUtils.h"
#include "vxRayProfile.h"
#include "vxViewerTypes.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeRegionGrowIterator.h"
#include "vxComponentUtils.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * default constructor
 * @param environment
 */
vxManipulatorResearch::vxManipulatorResearch(vxEnvironment & environment):
vxManipulator(environment)
{
  m_lastSelectedPoint = Point<float4>(-1,-1,-1);
} // Constructor()


/**
 * Event handler if key is pressed
 * @param uKeyId
 * @param uKeyboardState
 */ 
bool vxManipulatorResearch::OnKeyPressed(const uint1 uKeyId, 
                                         const uint4 uKeyboardState)
{
  bool bIfHandled(false);
  const std::string sFilename("C:/label.vol");
  bool fUpdate(false);

  switch (uKeyId)
  {
  case vxInput::KEY_L:
    if (uKeyboardState & STATE_SHIFT)
      LoadLabelVolume(sFilename);
    else
      WriteLabelVolume(sFilename);
    break;
  default:
    // just log out a debug message giving the key and state
    LogDbg(ToAscii(uint4(uKeyId)) + string(" ") + ToAscii(uint4(uKeyboardState)),"vxManipulatorResearch", "OnKeyPressed");
    break;
  }

  return bIfHandled;
} // OnKeyPressed()



/**
 * Event handler if key is pressed
 * @param position
 * @param eButtonId
 * @param uKeyboardState
 */ 
bool vxManipulatorResearch::OnButtonDown(const Point2D<int2> & position, 
                                         const ButtonEnum eButtonId, 
                                         const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  return bIfHandled;
} // OnButtonDown()



/**
 * Event handler if key is pressed
 * @param position
 * @param eButtonId
 * @param uKeyboardState
 */ 
bool vxManipulatorResearch::OnButtonUp(const Point2D<int2> & mousePosition, 
                                       const ButtonEnum eButtonId, 
                                       const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  if ( ! ( uKeyboardState & STATE_CTRL ) )
    return false;

  // get the click point in world and volume coordinates
  Point3Df worldPosition( 0, 0, 0 );
  if ( GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPosition ) == false )
    return false;
  const Triplef & units = GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits();
  Vector3Di volumePosition = Vector3Di( Triplei( Triple<float4>( worldPosition ).GetDividedAnIso( units ) ) );

  // prepare iterators for the region grow
  vxBlockVolume<bool> maskVolume( GetEnvironment().GetLabelVolume()->GetHeader() );
  vxBlockVolumeIterator<bool> iterMask( maskVolume );
  vxBlockVolumeIterator<uint2> iterIntensity( * GetEnvironment().GetIntensityVolume() );
  vxBlockVolumeIteratorNeighbors<uint2> iterIntensityNeighbors( * GetEnvironment().GetIntensityVolume(), volumePosition, vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
  std::queue< Vector3Di > posQueue;
  iterIntensity.SetPos( volumePosition );
  const uint2 uThreshold = 1350; // TODO use the vxIntensityConvertor

  // make sure we clicked above the threshold
  if ( iterIntensity.GetVoxel() < uThreshold )
    return false;

  // initialize the region grow conditions
  float4 fMaxDistance = 35;
  posQueue.push( volumePosition );
  iterMask.SetPos( volumePosition );
  iterMask.SetVoxel( true );
  while ( ! posQueue.empty() )
  {

    // get the front of the queue
    Vector3Di position = posQueue.front();
    posQueue.pop();

    // loop over the six neightbors
    for ( iterIntensityNeighbors.CenterAt( position ); ! iterIntensityNeighbors.IsAtEnd(); ++iterIntensityNeighbors )
    {

      // check distance and threshold
      iterMask.SetPos( iterIntensityNeighbors.GetPos() );
      const Point3Df currentWorldPosition = Point3Df( iterIntensityNeighbors.GetPos().GetScaledAnIso( units ) );
      float4 fDistance = ( currentWorldPosition - worldPosition ).GetLength();
      if ( iterIntensityNeighbors.GetVoxel() > uThreshold &&
           fDistance < fMaxDistance &&
           iterMask.GetVoxel() == false )
      {

        // add to queue
        iterMask.SetVoxel( true );
        posQueue.push( iterIntensityNeighbors.GetPos() );
      }

    }

  }

  // make the component
  vxComponentUtils::CreateNewComponent( maskVolume, "Left Ventricle", GetEnvironment(), true );

  return bIfHandled;
} // OnButtonUp()


/**
 * Event handler if key is pressed
 * @param position
 * @param uKeyboardState
 */ 
bool vxManipulatorResearch::OnMouseMove(const Point2D<int2> & mousePosition, 
                                        const uint4 uKeyboardState)
{
  bool bHandled(false);

  // sample the ray underneath the cursor
  vxViewerType3D * pViewerType3D = dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() );
  if ( pViewerType3D != NULL )
  {
    // add to environment
    if( GetEnvironment().GetElement( vxIDs::RayProfileData ) == NULL )
      GetEnvironment().AddElement( vxIDs::RayProfileData, reinterpret_cast<vxElement *>( new vxRayProfile ) );

    // save the data to environment
    vxShareableObject< vxRayProfile > * pProfileData( dynamic_cast< vxShareableObject< vxRayProfile > * >( GetEnvironment().GetElement( vxIDs::RayProfileData ) ) );
    std::vector<float4> & profile = pProfileData->GetRayOpacityProfile();
    profile.clear();
    Point<float4> worldPos;
    bool bResult = pViewerType3D->GetVesselLumenWorldFromMouse(mousePosition, worldPos, profile);
    pProfileData->Modified( vxModInfoID( vxElement, vxIDs::RayProfileData, 0 ) );
  }

  return bHandled;
} // OnMouseMove()


/**
 * Event handler if mouse wheel is scrolled
 * @param position
 * @param iDelta
 * @param uKeyboardState
 */
bool vxManipulatorResearch::OnMouseWheel(const Point2D<int2> & mousePosition, 
                                         const int2 iDelta, 
                                         const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  return bIfHandled;
} // OnMouseWheel()


/**
 * Load a label volume
 * @param sFilename file name
 */
void vxManipulatorResearch::LoadLabelVolume(const std::string & sFilename)
{
  // load from disk
  vxBlockVolume<uint2> inputLabelVol(sFilename);

  // allocate new volume if none yet
  if (GetEnvironment().GetLabelVolume() == NULL)
  {
    vxBlockVolume<uint2> * pNewVolume = new vxBlockVolume<uint2>(GetEnvironment().GetIntensityVolume()->GetHeader());
    vxShareableObject<vxBlockVolume<uint2> > * pObj = new vxShareableObject<vxBlockVolume<uint2> >(*pNewVolume);
    GetEnvironment().SetLabelVolume(pObj);
  }
  vxBlockVolume<uint2> * pLabelVolume = GetEnvironment().GetLabelVolume();
  if (pLabelVolume->GetHeader().GetVolDim() != inputLabelVol.GetHeader().GetVolDim())
  {
    throw ex::InvalidDataException(LogRec("Dimensions don't match!", "vxManipulatorResearch", "LoadLabelVolume"));
  }

  // search for highest label
  ValueRange<uint2> labelRange;
  vxBlockVolumeIterator<uint2> inputVolIter(inputLabelVol);
  for (inputVolIter.FirstNonBackgroundBlockZYX(); inputVolIter.IsNotAtEnd(); inputVolIter.NextNonBackgroundBlockZYX())
  {
    for(; inputVolIter.IsNotAtEndOfBlock(); inputVolIter.NextNonBackgroundZYXinsideBlock())
    {
      labelRange.AddValue(inputVolIter.GetVoxel());
    }
  }

  // copy data into label volume
  // loop over labels
  for (uint4 uLabels(0); uLabels<labelRange.GetMaximum(); uLabels++)
  {
    // create a new component
    vxBlockVolume<bool> * pMskTagVol(GetEnvironment().GetInteractiveSegmentation()->GetCompHdlrPtr()->GetMaskTagVolume());
    vxBlockVolumeIterator<bool> iterMaskVol(*pMskTagVol);
    vxBlockVolume<bool> * pRegTagVol;
    uint2 uComponentLabel(0);
    pRegTagVol = GetEnvironment().GetInteractiveSegmentation()->GetCompHdlrPtr()->AddCompReg(uComponentLabel, SegParams::NEW_REGIONS);
    vxBlockVolumeIterator<bool> iterRegionVol(*pRegTagVol);

    // loop over voxels in label
    for (inputVolIter.FirstNonBackgroundBlockZYX(); inputVolIter.IsNotAtEnd(); inputVolIter.NextNonBackgroundBlockZYX())
    {
      for(; inputVolIter.IsNotAtEndOfBlock(); inputVolIter.NextNonBackgroundZYXinsideBlock())
      {
        if (inputVolIter.GetVoxel() == uLabels+1)
        {
          iterRegionVol.SetPos(inputVolIter);
          iterRegionVol.SetVoxel(true);
        }
      }
    }

    // update the label volume
    GetEnvironment().GetInteractiveSegmentation()->GetSegVolsPtr()->UpdateSegVolume(pRegTagVol, uComponentLabel);
    
    // update the mask volume(to lock component)
    GetEnvironment().GetInteractiveSegmentation()->GetCompHdlrPtr()->ModifyMaskTagVol(pRegTagVol, true);
    
    // let everyone know that the volume has changed
    GetEnvironment().GetLabelVolume()->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));
    GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED));
  }
} // LoadLabelVolume()


/**
 * Write the label volume
 * @param sFilename file name
 */
void vxManipulatorResearch::WriteLabelVolume(const std::string & sFilename)
{
  if (GetEnvironment().GetLabelVolume() != NULL)
  {
    GetEnvironment().GetLabelVolume()->WriteAsLinVolVer1_0(sFilename);
  }
} // WriteLabelVolume()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorResearch.C,v $
// Revision 1.5  2006/04/24 17:53:42  frank
// working on cardiac left ventricle segmentation
//
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/08/05 13:06:35  frank
// added modified for ray profile
//
// Revision 1.2  2004/04/06 16:25:49  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.19  2004/01/09 17:21:44  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.18  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.17  2003/06/04 18:23:18  michael
// added namespace declaration
//
// Revision 1.16  2003/04/08 14:45:21  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.15  2003/03/31 18:57:40  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.14  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.13  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.12  2003/01/03 12:56:45  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// Revision 1.11  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.10  2002/11/10 23:06:58  michael
// removed includes of texture server and update calls on textures
//
// Revision 1.9  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.8  2002/08/20 22:44:07  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.7  2002/08/14 16:07:41  wenli
// initial version - Vascular
//
// Revision 1.6  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.5  2002/06/26 22:52:09  frank
// Improved speed of loading label.
//
// Revision 1.4  2002/06/26 20:15:11  frank
// Added two click vessel segmentation interface.
//
// Revision 1.3  2002/06/26 17:32:41  frank
// Implemented reading and writing label volume.
//
// Revision 1.2  2002/06/21 19:17:55  frank
// cleaned out
//
// Revision 1.1  2002/06/21 19:03:20  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorResearch.C,v 1.5 2006/04/24 17:53:42 frank Exp $
// $Id: vxManipulatorResearch.C,v 1.5 2006/04/24 17:53:42 frank Exp $