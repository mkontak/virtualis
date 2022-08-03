// $Id: SegmentationVolumes.C,v 1.2 2006/02/27 15:23:42 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class implementation for SegmentationVolumes. This class encapsulates the
 * updates and modifications, associated with segmentation, on a (label) volume.
 */


// includes
#include "stdafx.h"
#include "SegmentationVolumes.h"
#include "Vector3D.h"
#include "Timer.h"
#include "vxComponentUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


// statics
vxBlockVolume<uint2> SegmentationVolumes::m_defaultSegLblVol; // Default segmentation label volume.
vxBlockVolume<bool>  SegmentationVolumes::m_delSegTagVol;     // Delta tag volume(tracks effected voxels of each operation).


/**
 * Default constructor for SegmentationVolumes class.
 */
SegmentationVolumes::SegmentationVolumes()
:m_volDim(0,0,0)
//*******************************************************************
{
  m_defaultSegLblVol.GetHeader().SetVolDim(Triple<int4>(2, 2, 2));
  m_pSegLabelVol = & m_defaultSegLblVol;
} // SegmentationVolumes()


/**
 * Public method to set a pointer to a segmentation volume.
 * @param pLabelVolume pointer to a label volume.
 */
void SegmentationVolumes::SetSegLabelVol(vxBlockVolume<uint2> * pLabelVolume)
//*******************************************************************
{
  m_pSegLabelVol = pLabelVolume;
  m_volDim       = m_pSegLabelVol->GetHeader().GetVolDim();
  m_delSegTagVol.GetHeader().SetVolDim(m_pSegLabelVol->GetHeader().GetVolDim());
} // SetSegLabelVol()


/**
 * Private method that initializes the delta tag volume for segmentation.
 */
void SegmentationVolumes::InitDeltaSegTagVolume()
//*******************************************************************
{
  m_delSegTagVol.Clear();
} // InitDeltaSegTagVolume()


/**
 * Public method that updates the segmentation volume.
 * @param pTagVol pointer to tag volume.
 * @param uLabel  Value of label.
 */
void SegmentationVolumes::UpdateSegVolume(vxBlockVolume<bool> * pTagVol, 
                                          const uint2 & uLabel)
//*******************************************************************
{
  // Apply results to segmentation volume
  /////////////////////////////////////////
  Timer timer;

  if (m_pSegLabelVol != NULL && pTagVol != NULL)
  {
    vxBlockVolumeIterator<bool>  iterTag(*pTagVol);
    vxBlockVolumeIterator<uint2> iterLabel(*m_pSegLabelVol);
    vxBlockVolumeIterator<bool>  iterDeltaTag(m_delSegTagVol);

    iterLabel.SetClearBlock(uLabel);
    iterDeltaTag.SetClearBlock(true);
    
    for (iterTag.FirstNonBackgroundBlockZYX(); iterTag.IsNotAtEnd(); iterTag.NextNonBackgroundBlockZYX())
    {
      // try to set the entire block at once
      if (iterTag.GetCurrentBlock().IsHomogenous() && iterTag.GetVoxel() == true)
      {
        iterLabel.SetPos(iterTag);
        iterLabel.ClearCurrentBlock();
        iterDeltaTag.SetPos(iterTag);
        iterDeltaTag.ClearCurrentBlock();
      }
      else
      {
        // have to set each voxel individually
        for (iterTag.FirstNonBackgroundZYXinsideBlock() ; iterTag.IsNotAtEndOfBlock(); iterTag.NextNonBackgroundZYXinsideBlock())
        {
          iterLabel.SetPos(iterTag);
          iterLabel.SetVoxel(uLabel);
          iterDeltaTag.SetPos(iterTag);
          iterDeltaTag.SetVoxel(true);
        }
      }      
    } // end for all tag non-zero voxels 
  } // end if (m_pSegLabelVol != NULL && pTagVol != NULL)

  // just in case pTagVol was not compact...
  m_pSegLabelVol->Compact();
  m_delSegTagVol.Compact();

  LogDbg(std::string("UpdateSegVolume took ")+ToAscii(timer.ElapsedTime()) + " on label " + ToAscii( uLabel ), "SegmentationVolumes", "UpdateSegVolume");
} // UpdateSegVolume()


/**
 * Public method that removes a label from the segmentation volume.
 * @param uLabel Value of label.
 */
void SegmentationVolumes::RemoveLabelFromSegVolume(const uint2 & uLabelIndex)
//*******************************************************************
{
  // Remove results from segmentation volume
  ////////////////////////////////////////////
  Timer timer;

  if (m_pSegLabelVol != NULL)
  {
    vxBlockVolumeIterator<uint2> iterSegLabelVol(*m_pSegLabelVol);
    vxBlockVolumeIterator<bool>  iterDelSegTagVol(m_delSegTagVol);
    iterSegLabelVol.SetClearBlock(0);
    for (iterSegLabelVol.FirstNonBackgroundBlockZYX() ; iterSegLabelVol.IsNotAtEnd() ; iterSegLabelVol.NextNonBackgroundBlockZYX())
    {
      if (iterSegLabelVol.GetCurrentBlock().IsHomogenous())
      {
        const uint2 uVoxel(iterSegLabelVol.GetVoxel());
        if (uVoxel == uLabelIndex)
        {
          iterSegLabelVol.ClearCurrentBlock();
        }
        iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
        iterDelSegTagVol.SetClearBlock(true);
        iterDelSegTagVol.ClearCurrentBlock();
      }
      else
      {
        for (iterSegLabelVol.FirstNonBackgroundZYXinsideBlock() ; iterSegLabelVol.IsNotAtEndOfBlock() ; iterSegLabelVol.NextNonBackgroundZYXinsideBlock())
        {        
          const uint2 uVoxel(iterSegLabelVol.GetVoxel());
          if (uVoxel == uLabelIndex)
          {
            iterSegLabelVol.SetVoxel(0); // remove label(i.e., set to background, 0).
          }
          iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
          iterDelSegTagVol.SetVoxel(true);
        }
      }
    } // end for all non-zero label voxels
  } // end if (m_pSegLabelVol != NULL)

  m_pSegLabelVol->Compact();
  m_delSegTagVol.Compact();

  LogDbg(std::string("RemoveLabelFromSegVolume took ")+ToAscii(timer.ElapsedTime()), "SegmentationVolumes", "RemoveLabelFromSegVolume");
} // RemoveLabelFromSegVolume()


/**
 * Public method that removes a label from the segmentation volume
 * @param uLabel Value of label
 */
void SegmentationVolumes::RemoveLabelFromSegVolume(vxBlockVolume<bool> & rRegTagVol)
//*******************************************************************
{
  // Remove results from segmentation volume
  ////////////////////////////////////////////
  Timer timer;

  if (m_pSegLabelVol != NULL)
  {
    vxBlockVolumeIterator<uint2> iterSegLabelVol(*m_pSegLabelVol);
    vxBlockVolumeIterator<bool>  iterRegTagVol(rRegTagVol);
    vxBlockVolumeIterator<bool>  iterDelSegTagVol(m_delSegTagVol);
    iterSegLabelVol.SetClearBlock(0);
    iterDelSegTagVol.SetClearBlock(0);

    for (iterRegTagVol.FirstNonBackgroundBlockZYX() ; iterRegTagVol.IsNotAtEnd() ; iterRegTagVol.NextNonBackgroundBlockZYX())
    {
      // grow a component (auto segment), unlock component, grow 
      // same auto component again --> one block remains (bug!!!)
      // This was fixed by Michael's change to the homogeneous bit
      // mechanism in the vxBlockVolume ...
      ///////////////////////////////////////////////////////////
      if (iterRegTagVol.GetCurrentBlock().IsHomogenous())
      {
        iterSegLabelVol.SetPos(iterRegTagVol);
        iterSegLabelVol.ClearCurrentBlock();
        iterDelSegTagVol.SetPos(iterRegTagVol);
        iterDelSegTagVol.ClearCurrentBlock();
      }
      else
      {
        for (iterRegTagVol.FirstNonBackgroundZYXinsideBlock() ; iterRegTagVol.IsNotAtEndOfBlock() ; iterRegTagVol.NextNonBackgroundZYXinsideBlock())
        {
          iterSegLabelVol.SetVoxel(0, iterRegTagVol);
          iterDelSegTagVol.SetVoxel(true, iterRegTagVol);
        }
      }
    } // end for all non-zero label voxels
  } // end if (m_pSegLabelVol != NULL)

  m_pSegLabelVol->Compact();
  m_delSegTagVol.Compact();

  LogDbg(std::string("RemoveLabelFromSegVolume took ")+ToAscii(timer.ElapsedTime()), "SegmentationVolumes", "RemoveLabelFromSegVolume");
} // RemoveLabelFromSegVolume()


/**
 * Public method that removes a label from the segmentation volume.
 */
void SegmentationVolumes::RemoveAllLabelsFromSegVolume()
//*******************************************************************
{
  // Remove results from segmentation volume
  ////////////////////////////////////////////
  Timer timer;

  if (m_pSegLabelVol != NULL)
  {
    vxBlockVolumeIterator<uint2> iterSegLabelVol(*m_pSegLabelVol);
    vxBlockVolumeIterator<bool>  iterDelSegTagVol(m_delSegTagVol);
    
    for (iterSegLabelVol.FirstNonBackgroundBlockZYX() ; iterSegLabelVol.IsNotAtEnd() ; iterSegLabelVol.NextNonBackgroundBlockZYX())
    {
      if(iterSegLabelVol.GetCurrentBlock().IsHomogenous())
      {
        iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
        iterDelSegTagVol.SetClearBlock(true);
        iterDelSegTagVol.ClearCurrentBlock();
      }
      else
      {
        for (iterSegLabelVol.FirstNonBackgroundZYXinsideBlock() ; iterSegLabelVol.IsNotAtEndOfBlock() ; iterSegLabelVol.NextNonBackgroundZYXinsideBlock())
        {         
          iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
          iterDelSegTagVol.SetVoxel(true);
        }
      }
    } // end for all non-zero label voxels
  } // end if (m_pSegLabelVol != NULL)
  m_pSegLabelVol->Clear();

  LogDbg(std::string("RemoveAllLabelsFromSegVolume took ")+ToAscii(timer.ElapsedTime()), "SegmentationVolumes", "RemoveAllLabelsFromSegVolume");
} // RemoveLabelFromSegVolume()


/**
 * Public method that condenses labels such that the sequence of
 * labels increment by one(e.g., 0, 1, 2,...).
 */
void SegmentationVolumes::CondenseLabelsInSegVolume()
//*******************************************************************
{
  Timer timer;

  if (m_pSegLabelVol != NULL)
  {
    std::map<uint2, uint2> labelMap;
    labelMap = vxComponentUtils::CreateMapToCompactVolume(*m_pSegLabelVol);

    vxBlockVolumeIterator<uint2> iterSegLabelVol(*m_pSegLabelVol);
    vxBlockVolumeIterator<bool>  iterDelSegTagVol(m_delSegTagVol);

    // Set labels to new values that are strictly monotonic increasing by 1:
    for (iterSegLabelVol.FirstNonBackgroundBlockZYX() ; iterSegLabelVol.IsNotAtEnd() ; iterSegLabelVol.NextNonBackgroundBlockZYX())
    {
      if (iterSegLabelVol.GetCurrentBlock().IsHomogenous())
      {
        uint2 uNewLabel(labelMap[iterSegLabelVol.GetVoxel()]) ;
        iterSegLabelVol.SetClearBlock(uNewLabel);
        iterSegLabelVol.ClearCurrentBlock();
        iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
        iterDelSegTagVol.SetClearBlock(true);
        iterDelSegTagVol.ClearCurrentBlock();
      }
      else
      {
        for (iterSegLabelVol.FirstNonBackgroundZYXinsideBlock() ; iterSegLabelVol.IsNotAtEndOfBlock() ; iterSegLabelVol.NextNonBackgroundZYXinsideBlock())
        { 
          uint2 uNewLabel(labelMap[iterSegLabelVol.GetVoxel()]) ;
          iterSegLabelVol.SetVoxel(uNewLabel);
          iterDelSegTagVol.SetPos(iterSegLabelVol.GetPos());
          iterDelSegTagVol.SetVoxel(true);
        }
      }
    } // end for all non-zero label voxels
  } // end if (m_pSegLabelVol != NULL)

  LogDbg(std::string("CondenseLabelsInSegVolume took ")+ToAscii(timer.ElapsedTime()), "SegmentationVolumes", "CondenseLabelsInSegVolume");
} // RemoveLabelFromSegVolume()


#undef FILE_REVISION


// Revision History:
// $Log: SegmentationVolumes.C,v $
// Revision 1.2  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.41  2003/07/09 13:25:51  michael
// moved 1-liners to header file and brought code back in that was once commented
// out due to a bug in the vxBlockVolume homogeneous flag.
//
// Revision 1.40  2003/07/02 22:01:21  michael
// took out destructor from source file, put single lined code in header file, and
// added comment for outcommented code that needs to be revisited later on.
//
// Revision 1.39  2003/06/19 20:20:06  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.38  2003/06/07 15:36:38  michael
// reformatting
//
// Revision 1.37  2003/04/28 17:01:32  michael
// more code reviews
//
// Revision 1.36  2003/04/24 13:35:01  frank
// Code review
//
// Revision 1.35  2003/04/16 12:49:33  frank
// Code review
//
// Revision 1.34  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.33  2003/04/09 21:39:41  michael
// added a utility function to vxUtils to compact the label indices
//
// Revision 1.32  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.31  2003/04/08 20:55:08  michael
// cosmetics
//
// Revision 1.30  2003/04/07 20:20:31  michael
// clean-up
//
// Revision 1.29  2003/01/22 21:49:44  ingmar
// dimension and units are now Triples
//
// Revision 1.28  2002/12/19 21:30:22  ingmar
// back to timings in log
//
// Revision 1.27  2002/11/15 23:08:26  ingmar
// removed timings from log
//
// Revision 1.26  2002/09/11 15:46:20  jaddonisio
// New 'CondenseLabelsInSegVol' function.
//
// Revision 1.25  2002/09/05 22:44:02  jaddonisio
// New vxComponent(don't shift labels).
//
// Revision 1.24  2002/07/25 20:15:16  jaddonisio
// Removed multiple Compact() calls.
//
// Revision 1.23  2002/07/23 20:05:01  dmitry
// Setting up the default delta volume in the constructor removed.
//
// Revision 1.22  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.21  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.20  2002/07/08 22:56:37  ingmar
// fixed remove component function
//
// Revision 1.19  2002/07/02 23:55:03  soeren
// i. Made MaskVolume & deltaVolume compact.
// ii. Created 'RemoveAllLabelsFromSegVol'
//
// Revision 1.18  2002/07/02 01:24:25  soeren
// RemoveLabelFromSegVolume makes use of Homogenous flag now...
//
// Revision 1.17.2.2  2002/07/08 23:02:37  jaddonisio
// Set label to background - fix bug when deleting component.
//
// Revision 1.17.2.1  2002/07/02 20:29:31  jaddonisio
// Check homogenous flag when removing labels from volume.
//
// Revision 1.17  2002/06/26 17:13:04  soeren
// changed  iterLabel.SetPos(iterTag.GetPos()); to  iterLabel.SetPos(iterTag); and  iterDeltaTag.SetPos(iterTag.GetPos()); to  iterDeltaTag.SetPos(iterTag); Why not use it, if it's faster.
//
// Revision 1.16  2002/06/13 19:11:18  frank
// Optimized sculpt
//
// Revision 1.15  2002/06/03 21:19:35  jaddonisio
// Coding standard changes.
//
// Revision 1.14  2002/05/21 21:11:02  jaddonisio
// Replaced SetVoxel with call without iterator(temporarily).
//
// Revision 1.13  2002/05/09 16:14:18  jaddonisio
// Use iterator in SetVoxel.
//
// Revision 1.12  2002/05/07 18:56:27  jaddonisio
// Spacing.
//
// Revision 1.11  2002/05/07 16:03:55  ingmar
// added timings
//
// Revision 1.10  2002/05/06 15:56:39  jaddonisio
// More clean-up.
//
// Revision 1.9  2002/05/04 16:59:02  soeren
// removed logelapsedtime
//
// Revision 1.8  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.7  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.6  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.5  2002/04/30 15:32:00  soeren
// added firstnonbackgroundzyxinsideblock
//
// Revision 1.4  2002/04/30 14:49:16  soeren
// changed all for loop frok nextnonbackground to nextnonbackgroundblock + nextnonbackgroundinsideblock
//
// Revision 1.3  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.2  2002/04/18 16:17:45  jaddonisio
// Made delta tag volume static.
//
// Revision 1.1  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/SegmentationVolumes.C,v 1.2 2006/02/27 15:23:42 frank Exp $
// $Id: SegmentationVolumes.C,v 1.2 2006/02/27 15:23:42 frank Exp $
