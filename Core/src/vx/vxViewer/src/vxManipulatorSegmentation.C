// $Id: vxManipulatorSegmentation.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
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
#include "vxManipulatorSegmentation.h"
#include "vxManipulatorUtils.h"
#include "vxUtils.h"
#include "vxComponentUtils.h"
#include "vxMouseCursor.h"
#include "Timer.h"
#include "vxMessageBox.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"
#include "vxUndoActionSegmentation.h"
#include "vxIntensityConverter.h"


// defines
#define FILE_REVISION "$Revision: 1.16 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorSegmentation::vxManipulatorSegmentation(vxEnvironment & environment)
:vxManipulator(environment)
{
} // constructor


/**
 * OnButtonDown
 * Public method to process button down event.
 * @param mouse position
 * @param mouse button enum
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorSegmentation::OnButtonUp(const Point2D<int2> & mouseCoord, 
                                           const ButtonEnum eButtonId,
                                           const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  if (eButtonId == BUTTON_LEFT)
  { 

    // prepare to save the current state for undo purposes
    vxUndoActionSegmentation * pUndo = NULL;

    try
    {

      // make sure we are in a known viewer type
      const bool bKnownViewerType( dynamic_cast< vxViewerTypeMPROrtho   * >( GetEnvironment().GetViewerType() ) != NULL ||
                                   dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL ||
                                   dynamic_cast< vxViewerType3D         * >( GetEnvironment().GetViewerType() ) != NULL );
      if ( bKnownViewerType == false )
      {
        LogDbg( "Attempted to segment in an unknown viewer type", "vxManipulatorSegmentation", "OnButtonUp" );
        return false;
      }

      // get the world coordinates for the mouse position. 
      Point<float4> worldCoord;
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoord, worldCoord) == false)
      {
        LogDbg( "Illegal seed!!!" , "vxManipulatorSegmentation", "OnButtonUp" );
        return false;
      }

      //Display a warning message if the selected point is outside the image.
      Triple<float4> volWorldDim( (GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()));
      Box<float4> roi(0,0,0, volWorldDim[0], volWorldDim[1], volWorldDim[2]);
      
      if ( !roi.IsInside( (Triplef)worldCoord ) ) 
      {
        vxMessageBox::ShowInfo( "The selected point is outside the image. Please select a valid point." );
        return false;
      }

      pUndo = new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(), GetEnvironment().GetComponents() );
      vxMouseCursor cursor("CURSOR_WAIT");
      Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
      uint2 x(0.5F + worldCoord[0]/units[0]);
      uint2 y(0.5F + worldCoord[1]/units[1]);
      uint2 z(0.5F + worldCoord[2]/units[2]);
      Point<uint2> mouseVolumeIndexCoord(0.5F + worldCoord[0]/units[0], 
                                         0.5F + worldCoord[1]/units[1],
                                         0.5F + worldCoord[2]/units[2]);

      bool bAppendingToNone( dynamic_cast< vxComponentArray * >( GetEnvironment().GetComponents() )->GetSelectedComponentIndex() == 0 &&
                             GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::APPEND_REGIONS );
      if ( bAppendingToNone )
      {
        std::string sComponentName( GetEnvironment().GetComponents()->ArrayByLabel()[ 0 ]->GetName() );
        vxMessageBox::ShowInfo( "Appending to " + sComponentName + " is not possible. A new component will be created instead" );
      }

      // initialize timer
      Timer timer;

      // set up the active subset of the volume that is allowed to change
      Box<int4> validSubset( vxUtils::GetActiveBoundingBox( ( uKeyboardState & vxInput::STATE_SHIFT ), GetEnvironment().GetIntensityVolume(),
        GetEnvironment().GetViewerType(), GetEnvironment().GetPlaneOrtho(), GetEnvironment().GetRenderingMode(), GetEnvironment().GetOffset(),
        GetEnvironment().GetMaximumOffset(), GetEnvironment().GetCropBox() ) );
      GetEnvironment().GetInteractiveSegmentation()->SetValidSubset( Box<int4>( Point3Di( validSubset.GetMinPoint() ),
                                                                                 Point3Di( validSubset.GetMaxPoint() ) ) );

      // run region grow
      GetEnvironment().GetInteractiveSegmentation()->SetSeedPoint(mouseVolumeIndexCoord);

      // region threshold range
      int4 iThresholdLow(0);
      int4 iThresholdHigh(0);
      if ( GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod()== SegParams::SAMPLE_INTENSITY_RANGE )
      {
        std::pair< int4, int4 > uRange( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler().GetThresholdRange() );
        iThresholdLow  = uRange.first;
        iThresholdHigh = uRange.second;
      }
      else
      {
        //Use the RAW value 

      	iThresholdLow  = vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow(),
                          GetEnvironment().GetIntensityVolume()->GetHeader() );

  	    iThresholdHigh = vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh(), 
                          GetEnvironment().GetIntensityVolume()->GetHeader() );

      } // end if RegionGrowMethod is SampleIntensityRange

      // get the lock mask
      vxBlockVolume<bool> lockVolume( GetEnvironment().GetLabelVolume()->GetHeader() );
      vxComponentUtils::CalculateLockMask( lockVolume, * GetEnvironment().GetLabelVolume(), * GetEnvironment().GetComponents() );

      // get the valid subset of the volume
      Box<uint4> validSubsetUint4( vxUtils::GetActiveBoundingBox( uKeyboardState & vxInput::STATE_SHIFT, GetEnvironment().GetIntensityVolume(),
        GetEnvironment().GetViewerType(), GetEnvironment().GetPlaneOrtho(), GetEnvironment().GetRenderingMode(), GetEnvironment().GetOffset(),
        GetEnvironment().GetMaximumOffset(), GetEnvironment().GetCropBox() ) );
      Box<int4> validSubsetInt4( Point3Di( validSubsetUint4.GetMinPoint() ), Point3Di( validSubsetUint4.GetMaxPoint() ) );

      // set and run region grow
      vxBlockVolume<bool> regionVolume( GetEnvironment().GetLabelVolume()->GetHeader() );
      GetEnvironment().GetInteractiveSegmentation()->GetRegionGrow().Set( RegionGrow::REGION_THRESHOLD_RANGE, iThresholdLow, iThresholdHigh );
      GetEnvironment().GetInteractiveSegmentation()->GetRegionGrow().Run(
        mouseVolumeIndexCoord, GetEnvironment().GetIntensityVolume(),
        & lockVolume, & regionVolume, validSubsetInt4 );

      // get number of voxels in region grow
      uint4 uNumOfRegVoxels(0);
      GetEnvironment().GetInteractiveSegmentation()->GetRegionGrow().Get( RegionGrow::NUMBER_OF_REGION_VOXELS, & uNumOfRegVoxels );
      
      // write timing into logger
      LogDbg("Segmentation - Overall Run() took: "+ToAscii(timer.ElapsedTime()), "vxManipulatorSegmentation", "OnButtonDown");

      // check for the outcome of segmentation
      if ( uNumOfRegVoxels > 0 )
      {

        // make a new component
        const std::string sName = vxComponentUtils::GetUniqueComponentName( * GetEnvironment().GetComponents(), GetEnvironment().GetInteractiveSegmentation()->GetNextComponentName() );
        const uint2 uLabel = vxComponentUtils::CreateNewComponent( regionVolume, sName, GetEnvironment(), false );

        // set modified on shareable objects:
        GetEnvironment().GetLabelVolume()->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));

        if ( GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::NEW_REGIONS || bAppendingToNone == true )
        {

          // set the component color
          const uint2 uHistoryIndex = GetEnvironment().GetComponents()->GetHistoryFromLabel( uLabel );
          ( * GetEnvironment().GetComponents() )[ uHistoryIndex ]->SetColor( GetEnvironment().GetInteractiveSegmentation()->GetNextComponentColor() );

          GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED));
        }
        else
        {
          GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED));
        }

        // save the undo information - relinquish control of memory too
        vxEnvironment::GetUndoActions().PushAction( pUndo );
        pUndo = NULL;

      }
      else // segmentation failed
      {
        //if (GetEnvironment().GetInteractiveSegmentation()->GetStatus() == InteractiveSegmentation::FAILED_TO_SEGMENT)
        //{
          // this message messes up the input context so the user will just have to contend with 
          vxMessageBox::ShowInfo("Find Region: Failed to segment, check segmentation parameters for proper settings.");
        //}        
      } // if segmentation is successful
    }
    catch ( ... )
    {
      throw ex::VException(LogRec("OnButtonDown failed.","vxManipulatorSegmentation", "OnButtonDown"));
    }

    // clear undo information
    if ( pUndo != NULL )
    {
      delete pUndo;
      pUndo = NULL;
    }

  } // if left button.

  return bIfHandled;

} // OnButtonDown()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorSegmentation::OnSetCursor()
{

  const bool bKnownViewerType( dynamic_cast< vxViewerTypeMPROrtho   * >( GetEnvironment().GetViewerType() ) != NULL ||
                               dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL ||
                               dynamic_cast< vxViewerType3D         * >( GetEnvironment().GetViewerType() ) != NULL );

  vxMouseCursor::SetCursor( bKnownViewerType ? "CURSOR_REGION_GROW" : "CURSOR_NO" );
  return true;

} // OnSetCursor()


/**
 * Event handler if a key is pressed.
 *
 * @param uKeyId the key pressed.
 * @param uKeyboardState key state.
 * @return handled?
 */ 
bool vxManipulatorSegmentation::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled(false);

  switch(uKeyId)
  {

  case vxInput::KEY_SHIFT:
    // if applicable, display warning that this operation cannot be restricted in oblique views
    vxManipulatorUtils::WarnShiftOblique(GetEnvironment(), uKeyboardState);
    break;

  default:
    break;

  } // switch

  return bHandled;
} // OnKeyPressed()


/**
 * Event handler if the mouse is moved
 *
 * @param position       mouse position
 * @param uKeyboardState mouse state
 *
 * @return true if handled
 */ 
bool vxManipulatorSegmentation::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  // if applicable, display warning that this operation cannot be restricted in oblique views
  vxManipulatorUtils::WarnShiftOblique(GetEnvironment(), uKeyboardState);

  return false;
} // OnMouseMove()


#undef FILE_REVISION


// $Log: vxManipulatorSegmentation.C,v $
// Revision 1.16  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.15.2.3  2007/04/25 19:48:31  romy
// converted decider type to int4. Issue#5611
//
// Revision 1.15.2.2  2007/04/05 17:12:37  romy
// displays a message box if the user clicks a point outside the image for segmentation.
//
// Revision 1.15.2.1  2007/04/04 21:54:11  romy
// Issue#5555
//
// Revision 1.15  2007/02/13 22:32:04  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.14  2007/02/13 19:29:16  romy
// Fixed issue #  5382. Previously the values where using without converting them in to RAW values.
//
// Revision 1.13  2006/07/06 12:58:10  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.12  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.11  2006/03/02 15:47:58  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.10  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.9  2006/02/17 15:13:16  frank
// working on undoing all segmentation actions
//
// Revision 1.8  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.2  2005/08/26 12:31:27  frank
// Issue #4401: Fixed messages referring to the NONE component
//
// Revision 1.5.2.1  2005/07/11 18:11:46  frank
// cleaned up naming and color of components
//
// Revision 1.5  2001/01/03 19:30:08  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.4  2004/04/06 16:25:59  frank
// unified environment access naming
//
// Revision 1.3  2004/03/31 17:45:47  frank
// unused include files
//
// Revision 1.2  2004/03/12 18:04:13  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.31  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.30  2004/02/12 12:24:44  frank
// removed debug
//
// Revision 1.29.2.1  2004/02/12 12:25:56  frank
// removed debug
//
// Revision 1.29  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.28  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.27  2004/01/15 13:35:34  frank
// Issue #3644: Prevented segmentation on the curved MPR view
//
// Revision 1.26  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.25  2004/01/13 13:15:14  frank
// Fixed so that the sculpting and segmentation works properly on views derived from the oblique viewer type
//
// Revision 1.24  2004/01/12 14:18:36  frank
// fixed problem with manipulating on oblique with shift held down
//
// Revision 1.23  2003/12/22 12:54:08  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.22  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.21  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.20  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.19  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.18  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.17  2003/04/28 16:59:25  michael
// fixed issue #3116 and 3065
//
// Revision 1.16  2003/04/15 21:10:43  geconomos
// Issue# 3046: MessageBox causing focus problems.
//
// Revision 1.15  2003/04/11 18:00:07  frank
// Refactored 2D/3D selection code so that the same algorithm is used regardless of 2D or 3D and the mode (x-ray, MIP, thin slab, normal)
//
// Revision 1.14  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.13  2003/04/07 15:10:59  michael
// cosmetics
//
// Revision 1.12  2003/04/01 12:12:10  michael
// addressed issue #2934
//
// Revision 1.11  2003/03/31 18:57:40  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.10  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.9  2003/03/26 04:06:23  michael
// fixed status to not be failed if the append is not to NONE
// (subsequent bug of fixing issue #2972)
//
// Revision 1.8  2003/03/26 03:31:03  michael
// formatting
//
// Revision 1.7  2003/03/13 16:30:13  frank
// spelling
//
// Revision 1.6  2002/12/20 14:16:16  frank
// Removed failed segmentation message that messes up focus
//
// Revision 1.5  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.4  2002/11/27 22:44:24  michael
// added timing information
//
// Revision 1.3  2002/11/25 21:06:19  michael
// cosmetics
//
// Revision 1.2  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.1  2002/11/20 18:34:05  frank
// Renamed from vxManipulatorInterSeg
//
// Revision 1.43  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.42  2002/11/08 16:04:39  ingmar
// removed obsolete code
//
// Revision 1.41  2002/11/07 20:16:45  ingmar
// code beautify
//
// Revision 1.40  2002/10/14 20:05:04  geconomos
// Changed vxMessageBox to plain ole' window message box (temporary)
//
// Revision 1.39  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.38  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.37  2002/08/20 22:36:42  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.36  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.35  2002/07/11 16:36:49  ingmar
// set number of components for label volume
//
// Revision 1.34  2002/07/08 22:57:58  ingmar
// added SetMaxComponentIndex function
//
// Revision 1.33.2.1  2002/07/18 15:12:01  jaddonisio
// Reaaplying cursor after it gets lost upon return from thresd->wait().
//
// Revision 1.33  2002/06/20 19:41:34  michael
// fixed missing update
//
// Revision 1.32  2002/06/20 02:18:47  dmitry
// Using MouseCursor from the Environment.
//
// Revision 1.31  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.30  2002/06/03 21:19:04  jaddonisio
// Coding standard changes.
//
// Revision 1.29  2002/05/22 16:02:46  jaddonisio
// Set modified on component as EVENT_ADDED only if new region. This
// will correct the selected item in the component pane.
//
// Revision 1.28  2002/05/20 12:12:44  ingmar
// unique names for function UpdateTileTextures() -> UpdateTileTexturesLabels() and UpdateTileTexturesIntensities()
//
// Revision 1.27  2002/05/16 04:29:13  michael
// Sicne texmap server is multi-threaded, scultping and segmentation need to
// make sure that textures are already there before changing label volume.
// This is done by calling vxServerTexMap2d::EnsureTextureGenerationIsCompleted()
//
// Revision 1.26  2002/05/15 21:20:31  jaddonisio
// i. Check new status from segmentation
// ii. Only update textures if necessary
// iii. pop-up message box if segmentation unsuccessful
//
// Revision 1.25  2002/05/15 14:53:14  soeren
// added more exceptions
//
// Revision 1.24  2002/05/09 16:14:52  jaddonisio
// Log error on exception.
//
// Revision 1.23  2002/05/06 16:14:06  dmitry
// Placed try catch around segmentation code and cleaned up cursor in catch.
//
// Revision 1.22  2002/05/04 17:00:28  soeren
// changed from logfyi to logdbg
//
// Revision 1.21  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.20  2002/05/02 15:52:10  michael
// completed transition from MouseCursor to vxMouseCursor
//
// Revision 1.19  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.18  2002/05/01 19:43:39  jenny
// getting rid of volumeset
//
// Revision 1.17  2002/04/29 16:35:15  dmitry
// Wait Cursor added.
//
// Revision 1.16  2002/04/27 23:15:26  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.15  2002/04/24 14:57:40  jenny
// vxBlockVolume stuff
//
// Revision 1.14  2002/04/23 20:50:46  dmitry
// Cursors implemented.
//
// Revision 1.13  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.12  2002/04/23 15:33:47  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.11  2002/04/18 16:14:40  jaddonisio
// No need to set pointer to component object.
//
// Revision 1.10  2002/04/17 18:17:34  frank
// Cleaned up header & footer.
//
// Revision 1.9  2002/04/09 16:13:54  jaddonisio
// Removed setting of region grow method to sample range.
//
// Revision 1.8  2002/04/09 14:20:43  michael
// fixed update mechanism (components)
//
// Revision 1.7  2002/04/08 22:15:01  michael
// added timing stuff and some code optimization
//
// Revision 1.6  2002/04/06 00:10:48  jaddonisio
// i .Made GL context part of 'GetMouseCoord'.
// ii. Removed setting of rendering mode.
//
// Revision 1.5  2002/04/04 00:52:58  jaddonisio
// Implementation for button down - region grow.
//
// Revision 1.4  2002/04/01 16:36:19  dmitry
// fixed typo
//
// Revision 1.3  2002/04/01 16:32:42  dmitry
// Changed to use vxRenderingSettings.
//
// Revision 1.2  2002/03/29 22:32:54  jaddonisio
// Moved worl coordinate function ouside to base class.
// Also, corrected usage of volume index coordinates.
//
// Revision 1.1  2002/03/29 01:42:54  jaddonisio
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorSegmentation.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorSegmentation.C,v 1.16 2007/06/28 19:33:55 jmeade Exp $
