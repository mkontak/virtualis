// $Id: vxManipulatorSampleRegion.C,v 1.6 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (mailto:jaddonisio@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorSampleRegion.h"
#include "vxEnvironment.h"
#include "vxViewerTypes.h"
#include "vxUtils.h"
#include "Quadrilateral.h"
#include "vxIntensityConverter.h"


// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor
 * @param environment the environment
 */
vxManipulatorSampleRegion::vxManipulatorSampleRegion(vxEnvironment & environment)
: vxManipulator(environment)
//*******************************************************************
{
} // vxManipulatorSampleRegion()


/**
 * Public method to process mouse move event
 * @param position mouse position
 * @param uKeyboardState state of the mouse
 * @return handled?
 */
bool vxManipulatorSampleRegion::OnMouseMove(const Point2D<int2> & position, 
                                            const uint4 uKeyboardState)
//*******************************************************************
{
  SetMousePosition(position);
  PerformSampling();

  return false;
} // OnMouseMove()


/**
 * Perform sampling after a change in parameters
 * @param mousePosition the mouse position
 */
void vxManipulatorSampleRegion::PerformSampling()
//*******************************************************************
{
  if (GetEnvironment().GetInteractiveSegmentation() != NULL)
  {
    if (GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod() == SegParams::SAMPLE_INTENSITY_RANGE)
    {
      // get the world coordinates for the mouse position. 
      Point3Df mouseWorldCoord;
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(GetMousePosition(), mouseWorldCoord) == false)
        return;

      // convert to volume coordinates
      const Triplef & units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      Point<float4> mouseVolumeIndexCoord(mouseWorldCoord[0]/units[0], mouseWorldCoord[1]/units[1], mouseWorldCoord[2]/units[2]);
      Point<uint2> mouseVolumeIndexCoordRounded(mouseVolumeIndexCoord.m_x + 0.5F,
                                                mouseVolumeIndexCoord.m_y + 0.5F,
                                                mouseVolumeIndexCoord.m_z + 0.5F);

      RegionSampler & regionSampler(GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler());
      regionSampler.SetSampleRadius(GetEnvironment().GetInteractiveSegmentation()->GetRegionSampleRadius());

      if (dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType()))
      {
        // sample in 3D
        regionSampler.SampleIntensityRangeSphere(mouseVolumeIndexCoordRounded, GetEnvironment().GetIntensityVolume());

        // set thresholds
        //GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityLow (regionSampler.GetThresholdRange().first );
        //GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityHigh(regionSampler.GetThresholdRange().second);
        vxVolumeHeader header(GetEnvironment().GetIntensityVolume()->GetHeader());  
        int iMin(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().first,  header));
        int iMax(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().second, header));

        // set thresholds
        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityLow ( iMin );
        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityHigh( iMax );



        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE));
        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_INTENSITY_RANGE));
      }
      else if (dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()))
      {
        // sample on 2D ortho slice
        uint4 uAxisIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
        RegionSampler::SampleRestrictionEnum eSamplePlane[] = { RegionSampler::YZ_PLANE,
                                                                RegionSampler::XZ_PLANE,
                                                                RegionSampler::XY_PLANE,
                                                                RegionSampler::SPHERICAL };
        regionSampler.SampleIntensityRangeOrtho(mouseVolumeIndexCoord, GetEnvironment().GetIntensityVolume(), eSamplePlane[ uAxisIndex ]);

        vxVolumeHeader header(GetEnvironment().GetIntensityVolume()->GetHeader());  
        int iMin(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().first,  header));
        int iMax(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().second, header));

        // set thresholds
        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityLow ( iMin );
        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityHigh( iMax );

        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE));
        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_INTENSITY_RANGE));
      }
      else if (dynamic_cast<vxViewerTypeMPROblique *>(GetEnvironment().GetViewerType()))
      {
        // scale the quadrilateral to be the correct world size
        Quadrilateral quad(mouseWorldCoord,
                           GetEnvironment().GetCamera()->GetViewUp(),
                           GetEnvironment().GetCamera()->GetViewRight(),
                           GetEnvironment().GetInteractiveSegmentation()->GetRegionSampleRadius());

        // sample on 2D oblique slice
        regionSampler.SampleIntensityRangeOblique(quad, GetEnvironment().GetIntensityVolume());

        // set thresholds
        vxVolumeHeader header(GetEnvironment().GetIntensityVolume()->GetHeader());  
        int iMin(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().first,  header));
        int iMax(vxIntensityConverter::RawToModality(regionSampler.GetThresholdRange().second, header));

        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityLow ( iMin);
        GetEnvironment().GetInteractiveSegmentation()->SetRegionIntensityHigh( iMax);
        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE));
        GetEnvironment().GetInteractiveSegmentation()->Modified(vxModIndInfo(InteractiveSegmentation, 0, InteractiveSegmentation::EVENT_INTENSITY_RANGE));
      }
      else
      {
        throw ex::IllegalArgumentException(LogRec("Bad sampling space", "vxManipulatorSampleRegion", "PerformSampling"));
      }
    } // SAMPLE_RANGE method?
  } // end (InteractiveSegmentationPtr != NULL)
} // PerformSampling()


/**
 * Public method to process mouse wheel event
 * @param position       mouse position
 * @param iDelta         mouse wheel delta
 * @param uKeyboardState keyboard state
 * @return handled?
 */
bool vxManipulatorSampleRegion::OnMouseWheel(const Point2D<int2> & position, 
                                             const int2 iDelta, 
                                             const uint4 uKeyboardState)
//*******************************************************************
{
  // Unless we can ensure that this manipulator is in the manipulator queue
  // after the plane manipulator, the plane will not be set correctly. An
  // alternative approach would be to have a controller do this ...

  //SetMousePosition(position);
  //PerformSampling();

  return false;
} // OnMouseWheel()


/**
 * Public method to process 'key pressed' event
 * @param key id
 * @param keyboard state
 * @param handled?
 */
bool vxManipulatorSampleRegion::OnKeyPressed(const uint1 uKeyId, 
                                             const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (GetEnvironment().GetInteractiveSegmentation() != NULL)
  {
    if (GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod() == SegParams::SAMPLE_INTENSITY_RANGE)
    {
      float4 fRadius(1);
      switch (uKeyId)
      {
      case vxInput::KEY_EQUALS:
      case vxInput::KEY_PLUS_KEYPAD:
      case vxInput::KEY_MINUS:
      case vxInput::KEY_MINUS_KEYPAD:
          {
            // maybe we need a non linear mapping at some point ...
            Triplef units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
            float fMin(Min(units[0], units[1], units[2]));
            float fIncrement(fMin);
            const bool bIncrement(uKeyId == vxInput::KEY_EQUALS || uKeyId == vxInput::KEY_PLUS_KEYPAD);
            fIncrement *= (bIncrement ? 1 : -1);
            fRadius = Bound((float4)(fMin), GetEnvironment().GetInteractiveSegmentation()->GetRegionSampleRadius() + fIncrement, (float4)(50 * fMin));
            GetEnvironment().GetInteractiveSegmentation()->SetRegionSampleRadius(fRadius);
            PerformSampling();
            bIfHandled = true;
          }
          break;
        default:
          break;
      }
    } // end if (RegionGrowMethod == SAMPLE_INTENSITY_RANGE)
  } // end if (InteractiveSegmentationPtr != NULL)

  return bIfHandled;
} // OnKeyPressed()


/**
 * Public method to process mouse leave event
 */
void vxManipulatorSampleRegion::OnMouseLeave()
//*******************************************************************
{
  if (GetEnvironment().GetInteractiveSegmentation() != NULL)
  {
    if (GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod() == SegParams::SAMPLE_INTENSITY_RANGE)
    {
      GetEnvironment().GetInteractiveSegmentation()->SetSeedPoint(Point<uint2>(0xFFFF, 0xFFFF, 0xFFFF));
    }
  }
} // OnMouseLeave()


#undef FILE_REVISION


// $Log: vxManipulatorSampleRegion.C,v $
// Revision 1.6  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.5.6.1  2007/04/04 21:46:33  romy
// Change the Modality to Raw before you set them. Issue#5555
//
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.2  2005/08/02 17:07:28  michael
// Fixed PVCS tracker issue #4324 (intensity sample range not working)
//
// Revision 1.3.2.1  2005/07/05 17:54:15  frank
// spelling
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/08 15:34:51  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.2  2004/04/06 16:25:49  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/01/23 16:24:41  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.3  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.2  2003/12/22 16:21:42  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis. Also changed the increase/decrease
// of the circle size using '+' and '-' instead of 'A' and 'Z'.
//
// Revision 1.1  2003/12/19 13:50:05  michael
// Renaming: vxManipulatorRegionSampler -> vxManipulatorSampleRegion
//
// Revision 1.14  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.13  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.12  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.11  2003/04/29 14:12:57  frank
// Moved projection functionality to the viewer type
//
// Revision 1.10  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.9  2003/03/19 19:34:07  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.8  2003/03/18 21:22:22  geconomos
// Added EVENT_INTENSITY_RANGE to modified queue when sampling.
//
// Revision 1.7  2003/01/22 21:58:20  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.6  2001/01/09 16:59:04  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.5  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.4  2002/11/13 16:09:34  geconomos
// Set SeedPoint to outside the volume in OnMouseLeave()
//
// Revision 1.3  2002/08/20 22:43:20  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.2  2002/06/26 14:41:04  jaddonisio
// Set rendering mode to VIEW_CHANGING to make 3D faster.
//
// Revision 1.1  2002/06/21 18:02:11  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorSampleRegion.C,v 1.6 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorSampleRegion.C,v 1.6 2007/06/28 19:33:55 jmeade Exp $
