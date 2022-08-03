// $Id: MorphOp.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates Morphological Operation image processing.
 */


// includes
#include "stdafx.h"
#include "Timer.h"
#include "MorphOp.h"
#include "SegmentationVolumes.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxDeciderVoxelValue.h"
#include "vxIntensityConverter.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * default constructor
 */
MorphOp::MorphOp()
//*******************************************************************
{
  m_pSegLblVol    = NULL;
  m_pSegVols      = NULL;
  m_pMaskTagVol   = NULL;
  m_pvpRegTagVols = NULL;
  m_uThrPhi       = 0; 1; 2;
  m_uThrChi       = 0; 1; 2;
} // default constructor


/**
 * Public method that runs the MorphOp class
 * @param vpRegTagVol vector of region tag volume pointers
 * @param pSegLblVol  pointer to a segmentation label volume
 * @param pMaskTagVol pointer to a mask tag volume
 * @param uCompLabel  label of component
 * @param uOpType     operation type
 * @param uIterN      number of iterations
 */
void MorphOp::Run(const vector<vxBlockVolume<bool> *> & vpRegTagVol, 
                  vxBlockVolume<bool> * pMaskTagVol, 
                  uint2 uCompLabel, 
                  uint2 uOpType, 
                  vxBlockVolume<uint2> * pIntensityVol, 
                  uint4 uIterN,
                  uint2 uThresholdLow, 
                  uint2 uThresholdHigh)
//*******************************************************************
{
  m_uCompLabel    = uCompLabel;
  m_pSegLblVol    = m_pSegVols->GetSegLabelVol();
  m_pvpRegTagVols = &vpRegTagVol;
  m_pMaskTagVol   = pMaskTagVol;
  m_pIntensityVol = pIntensityVol;

  m_morphTagVol.GetHeader().SetVolDim(m_pSegLblVol->GetHeader().GetVolDim());
  m_morphTagVol.GetHeader().SetVoxelUnits(m_pSegLblVol->GetHeader().GetVoxelUnits());

  if (m_pSegLblVol != NULL && m_pMaskTagVol != NULL)
  {
    switch (uOpType)
    {
    case ERODE_OP:
      {
        for (int4 i(0); i<uIterN ; i++)
        {
          Erosion(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                  vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
      }
      break; // end ERODE_OP
    case ERODE_WITHIN_RANGE_OP:
      {
        for (int4 i(0); i<uIterN ; i++)
        {
          Erosion(uThresholdLow, uThresholdHigh);
        }
      }
      break; // end ERODE_WITHIN_RANGE_OP
    case DILATE_OP:
      {
        for (int4 i(0); i<uIterN ; i++)
        {
          Dilation(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                   vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
      }
      break; // end DILATE_OP
    case DILATE_WITHIN_RANGE_OP:
      {
        for (int4 i(0); i<uIterN ; i++)
        {
          Dilation(uThresholdLow, uThresholdHigh);
        }
      }
      break; // end DILATE_WITHIN_RANGE_OP
// currently not used 
/*
    case CLOSE_OP:
      {
        for (int4 i(0); i<uIterN ; i++)
        {
          Dilation(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                   vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
        for (int4 j(0); j<uIterN ; j++)
        {
          Erosion(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                  vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
      }
      break; // CLOSE_OP
    case OPEN_OP:
      {
        for (int4 i(0); i < uIterN ; i++)
        {
          Erosion(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                  vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
        for (int4 j(0); j < uIterN ; j++)
        {
          Dilation(vxIntensityConverter::GetMinValue(*pIntensityVol), 
                   vxIntensityConverter::GetMaxValue(*pIntensityVol));
        }
      }
      break; // end OPEN_OP
*/
    default:
      break;
    } // end switch operation type
  } // end if volumes not NULL

  return;
} // Run()


/**
 * Private method for erosion
 * @param uThresholdLow lower threshold
 * @param uThresholdHigh upper threshold
 */
void MorphOp::Erosion(uint2 uThresholdLow, uint2 uThresholdHigh)
//*******************************************************************
{
  try
  {
    Timer timer, timer2;

    m_uNumberOfVoxels = 0;
    vxDeciderVoxelValueRange<uint2> deciderRange(uThresholdLow, uThresholdHigh);

    // morph tag block volume
    m_morphTagVol.Clear();
    vxBlockVolumeIterator<bool> iterMorphTag(m_morphTagVol);
    std::vector<vxBlockVolume<bool> *>::const_iterator iterRegTagVols;

    for (iterRegTagVols = m_pvpRegTagVols->begin(); iterRegTagVols != m_pvpRegTagVols->end(); iterRegTagVols++)
    {
      vxBlockVolumeIterator<bool>  iterRegTag(**iterRegTagVols);
      vxBlockVolumeIterator<uint2> iterIntensity(*m_pIntensityVol);
      vxBlockVolumeIterator<uint2> iterLabel(*m_pSegLblVol);
      vxBlockVolumeIterator<bool>  iterMaskTag(*m_pMaskTagVol);
      vxBlockVolumeIteratorNeighbors<uint2> iterNeighbour(*m_pSegLblVol);

      // process volume
      vxBlockVolume<bool> processVolNew;
      processVolNew.GetHeader().SetVolDim(m_pSegLblVol->GetHeader().GetVolDim());
      processVolNew.GetHeader().SetVoxelUnits(m_pSegLblVol->GetHeader().GetVoxelUnits());

      vxBlockVolumeIterator<bool> iterProcTagVol(processVolNew);

      // iterate through region
      timer2.Reset();
      for (iterRegTag.FirstNonBackgroundBlockZYX(); iterRegTag.IsNotAtEnd(); iterRegTag.NextNonBackgroundBlockZYX())
      {
        if (iterRegTag.GetCurrentBlock().IsHomogenous()) // Traverse around block boundaries
        {
			BlockFaceTraversal(iterRegTag, iterNeighbour, iterProcTagVol, &MorphOp::ExamErosionCandidate);
        }
        else // traverse around each voxel in block
        {
          for (iterRegTag.FirstNonBackgroundZYXinsideBlock(); iterRegTag.IsNotAtEndOfBlock(); iterRegTag.NextNonBackgroundZYXinsideBlock())
          {
            ExamErosionCandidate(iterRegTag, iterNeighbour, iterProcTagVol);
          }
        }
      } // end for iteration through all non-background blocks

      LogDbg("Time for process volume is: " + ToAscii(timer2.ElapsedTime()), "MorphOp", "Erosion");
      timer2.Reset();

      // iterate through processing volume
      for (iterProcTagVol.FirstNonBackgroundBlockZYX(); iterProcTagVol.IsNotAtEnd(); iterProcTagVol.NextNonBackgroundBlockZYX())
      {
        for (iterProcTagVol.FirstNonBackgroundZYXinsideBlock(); iterProcTagVol.IsNotAtEndOfBlock(); iterProcTagVol.NextNonBackgroundZYXinsideBlock())
        {
          // is not checking whether this component is locked!!!
          if (deciderRange.IsPartOfRegion(iterIntensity.GetVoxel(iterProcTagVol)))
          {
            m_uNumberOfVoxels++;
            iterMorphTag.SetVoxel(true, iterProcTagVol);
          }
        } // end for iteration through non-background within block
      } // end for iteration through all non-background blocks

      LogDbg("Time for checking locked & is in region: " + ToAscii(timer2.ElapsedTime()), "MorphOp", "Erosion");

      // apply morphing to region tag
      ApplyMorphToTag((*iterRegTagVols), false);

      // apply morphing to mask tag
      ApplyMorphToTag(m_pMaskTagVol, false);
    } // end for all region tag volumes

    // apply morphing to segmentation
    UpdateSegVolume(&m_morphTagVol, 0);

    LogDbg(string("Erosion took ")+ToAscii(timer.ElapsedTime()), "MorphOp", "Erosion"); timer.Reset();
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("Erosion failed.", "MorphOp", "Erosion"));    
  }

  return;
} // Erosion()


/**
 * Private method for dilation
 * @param uThresholdLow lower threshold
 * @param uThresholdHigh upper threshold
 */
void MorphOp::Dilation(uint2 uThresholdLow, 
                       uint2 uThresholdHigh)
//*******************************************************************
{
  try
  {
    Timer timer, timer2;

    m_uNumberOfVoxels = 0;
    vxDeciderVoxelValueRange<uint2> deciderRange(uThresholdLow, uThresholdHigh);

    // morph tag block volume
    m_morphTagVol.Clear();
    vxBlockVolumeIterator<bool> iterMorphTag(m_morphTagVol);
    std::vector<vxBlockVolume<bool> *>::const_iterator iterRegTagVols;

    for (iterRegTagVols = m_pvpRegTagVols->begin(); iterRegTagVols != m_pvpRegTagVols->end(); iterRegTagVols++)
    {
      vxBlockVolumeIterator<bool>  iterRegTag(**iterRegTagVols);
      vxBlockVolumeIterator<uint2> iterIntensity(*m_pIntensityVol);
      vxBlockVolumeIterator<uint2> iterLabel(*m_pSegLblVol);
      vxBlockVolumeIterator<bool>  iterMaskTag(*m_pMaskTagVol);
      vxBlockVolumeIteratorNeighbors<uint2> iterNeighbour(*m_pSegLblVol);

      // process volume
      vxBlockVolume<bool> processVolNew;
      processVolNew.GetHeader().SetVolDim(m_pSegLblVol->GetHeader().GetVolDim());
      processVolNew.GetHeader().SetVoxelUnits(m_pSegLblVol->GetHeader().GetVoxelUnits());

      vxBlockVolumeIterator<bool> iterProcTagVol(processVolNew);

      // iterate through region
      timer2.Reset();
      for (iterRegTag.FirstNonBackgroundBlockZYX(); iterRegTag.IsNotAtEnd(); iterRegTag.NextNonBackgroundBlockZYX())
      {
        if (iterRegTag.GetCurrentBlock().IsHomogenous()) // Traverse around block boundaries
        {
			BlockFaceTraversal(iterRegTag, iterNeighbour, iterProcTagVol, &MorphOp::ExamDilationCandidate);
        }
        else // traverse around each voxel in block
        {
          for (iterRegTag.FirstNonBackgroundZYXinsideBlock(); iterRegTag.IsNotAtEndOfBlock(); iterRegTag.NextNonBackgroundZYXinsideBlock())
          {
            ExamDilationCandidate(iterRegTag, iterNeighbour, iterProcTagVol);
          }
        }
      } // end for iteration through all non-background blocks

      LogDbg("Time for process volume is: " + ToAscii(timer2.ElapsedTime()), "MorphOp", "Dilation");
      timer2.Reset();

      // iterate through processing volume
      for (iterProcTagVol.FirstNonBackgroundBlockZYX(); iterProcTagVol.IsNotAtEnd(); iterProcTagVol.NextNonBackgroundBlockZYX())
      {
        for (iterProcTagVol.FirstNonBackgroundZYXinsideBlock(); iterProcTagVol.IsNotAtEndOfBlock(); iterProcTagVol.NextNonBackgroundZYXinsideBlock())
        {
          // checking whether the new candidate belongs not to a locked component and to the region
          if (iterMaskTag.GetVoxel(iterProcTagVol) == false && deciderRange.IsPartOfRegion(iterIntensity.GetVoxel(iterProcTagVol)))
          {
            m_uNumberOfVoxels++;
            iterMorphTag.SetVoxel(true, iterProcTagVol);
          }
        } // end for iteration through non-background within block
      } // end for iteration through all non-background blocks

      LogDbg("Time for checking locked & is in region: " + ToAscii(timer2.ElapsedTime()), "MorphOp", "Dilation");

      // apply morphing to region tag
      ApplyMorphToTag((*iterRegTagVols), true);

      // apply morphing to mask tag
      ApplyMorphToTag(m_pMaskTagVol, true);
    } // end for all region tag volumes

    // apply morphing to segmentation
    UpdateSegVolume(&m_morphTagVol, m_uCompLabel);

    LogDbg(string("Dilation took ")+ToAscii(timer.ElapsedTime()), "MorphOp", "Dilation"); timer.Reset();
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("Dilation failed.", "MorphOp", "Dilation"));    
  }

  return;
} // Dilation()


/**
 * Private method that traverses around a neighborhood.
 * @param iterRegTag region tag iterator
 * @param iterNeighbour neighborhood iterator
 * @param iterProcTagVol processing tag(resulting tags that will be applied) iterator
 */
void MorphOp::ExamErosionCandidate(vxBlockVolumeIterator<bool> & iterRegTag, 
                                   vxBlockVolumeIteratorNeighbors<uint2> & iterNeighbour, 
                                   vxBlockVolumeIterator<bool> & iterMorphTag)
//*******************************************************************
{
  // set counter to zero
  uint4 iOffCount(0);

  // set neighborhood at point in region
  const Vector3D<int4> centerPos(iterRegTag.GetPos());

  // loop around neighborhood and remove if at least one neighbour is outside
  for (iterNeighbour.CenterAt(centerPos); !iterNeighbour.IsAtEnd(); ++iterNeighbour)
  {
    if (iterNeighbour.IsInsideVolume())
    {
      if (iterNeighbour.GetVoxel() != m_uCompLabel)
      {
        iterMorphTag.SetVoxel(true, iterRegTag);
        break; // go to next point in region.
      }
    }
    else // we need to erode back in on a volume face!!!
    {
      iterMorphTag.SetVoxel(true, iterRegTag);
      break; // go to next point in region
    }
  } // end for loop around neighborhood

  return;
} // ExamErosionCandidate()


/**
 * Private method that traverses around a neighborhood
 * @param iterRegTag region tag iterator
 * @param iterNeighbour neighborhood iterator
 * @param iterProcTagVol processing tag(resulting tags that will be applied) iterator
 */
void MorphOp::ExamDilationCandidate(vxBlockVolumeIterator<bool> & iterRegTag, 
                                    vxBlockVolumeIteratorNeighbors<uint2> & iterNeighbour, 
                                    vxBlockVolumeIterator<bool> & iterProcTagVol)
//*******************************************************************
{
  // set neighborhood at point in region
  Vector3D<int4> centerPos(iterRegTag.GetPos());

  // loop around neighborhood to find voxels that should become part of region
  for (iterNeighbour.CenterAt(centerPos); !iterNeighbour.IsAtEnd(); ++iterNeighbour)
  {
    if (iterNeighbour.GetVoxel() != m_uCompLabel)
    {
      if (iterNeighbour.IsInsideVolume())
      {
        iterProcTagVol.SetPos(iterNeighbour.GetPos());
        iterProcTagVol.SetVoxel(true);
      }
    }
  } // end for loop around neighborhood
} // ExamDilationCandidate()


/**
 * Private method that traverses around the block faces. 
 * This only works because it is a six connected neighbourhood. 
 * For other neighbourhood iterators, we might need more than 
 * just the faces!!! TO_DO TODO
 * @param iterRegTag region tag iterator
 * @param iterNeighbour neighborhood iterator
 * @param iterProcTagVol processing tag(resulting tags that will be applied) iterator
 */
void MorphOp::BlockFaceTraversal(vxBlockVolumeIterator<bool> &           iterRegTag,
                                 vxBlockVolumeIteratorNeighbors<uint2> & iterNeighbour,
                                 vxBlockVolumeIterator<bool> &           iterProcTagVol,
                                 MorphOp::NtraversalFunctionPtrType      pNtraversal)
//*******************************************************************
{
  uint4 i(0), j(0);  
  const Vector3D<int4> position(iterRegTag.GetPos());

  // the optimizations based on block faces to only work for a 6-connected
  // neighbourhood. What would rather be needed is a way of getting information
  // about which of the 25 neighbouring blocks could potentially be visited but
  // that would then be tricky to implement. Therefore, only the 6-connected
  // neighbourhood is implemented and optimized.
  ///////////////////////////////////////////////////////////////////////////////
  const Vector3D<int4> volSize(iterRegTag.GetVolume().GetHeader().GetVolDim());
  bool bIsInnerBlock(!(position.m_x == 0 || position.m_y == 0 || position.m_z == 0 || position.m_x+16 >= volSize.m_x || position.m_y+16 >= volSize.m_y || position.m_z+16 >= volSize.m_z));
  bool bOptimizationPossible(bIsInnerBlock && iterNeighbour.GetNeighbourhoodType() == vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
  // --------------- X == 0 Plane ---------------
  for (iterRegTag.SetPos(position), i=0; i<16; iterRegTag.IncY(), i+=2)
  {
/*
    // optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.DecX();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break;  // skip adjacent homogenous block
      iterRegTag.IncX();
    }
*/
    for (j=0; j<16; iterRegTag.IncZ(), j++) 
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol); 
    }
    iterRegTag.IncY();
    for (j=0; j<16; j++) 
    {
      iterRegTag.DecZ();
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol); 
    }
  } // end for x==0 plane

  // --------------- Y == 0 Plane ---------------
  for (iterRegTag.SetPos(position), i=0; i<16; iterRegTag.IncX(), i+=2)
  {
/* 
// optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.DecY();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break;  // skip adjacent homogenous block
      iterRegTag.IncY();
    }
*/
    for (j=0; j<16; iterRegTag.IncZ(), j++)
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
    iterRegTag.IncX();
    for (j=0; j<16; j++)
    {
      iterRegTag.DecZ(), 
        (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
  } // end ofr y==0 plane

  // --------------- Z == 0 Plane ---------------
  for (iterRegTag.SetPos(position), i=0; i<16; iterRegTag.IncX(), i+=2)
  {
/* 
// optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.DecZ();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break; // skip adjacent homogenous block
      iterRegTag.IncZ();
    }
*/
    for (j=0; j<16; iterRegTag.IncY(), j++)
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
    iterRegTag.IncX();
    for (j=0; j<16; j++)
    {
      iterRegTag.DecY();
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
  } // end fo z==0 plane

  // --------------- X == 16 Plane ---------------
  for (iterRegTag.SetPos(position+Vector3D<int4>(15,0,0)), i=0; i<16; iterRegTag.IncY(), i+=2)
  {
/* 
// optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.IncX();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break; // skip adjacent homogenous block
      iterRegTag.DecX();
    }
*/
    for (j=0; j<16; iterRegTag.IncZ(), j++)
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
    iterRegTag.IncY();
    for (j=0; j<16; j++)
    {
      iterRegTag.DecZ();
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
  } // end for x==16 plane

  // --------------- Y == 16 Plane ---------------
  for (iterRegTag.SetPos(position+Vector3D<int4>(0,15,0)), i=0; i<16; iterRegTag.IncX(), i+=2)
  {
/* 
// optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.IncY();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break; // skip adjacent homogenous block
      iterRegTag.DecY();
    }
*/
    for (j=0; j<16; iterRegTag.IncZ(), j++)
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
    iterRegTag.IncX();
    for (j=0; j<16; j++)
    {
      iterRegTag.DecZ();
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
  } // end for y==16 plane

  // --------------- Z == 16 Plane ---------------
  for (iterRegTag.SetPos(position+Vector3D<int4>(0,0,15)), i=0; i<16; iterRegTag.IncX(), i+=2)
  {
/* 
// optimize for six connected neighbourhood only (but that's what is currently used anyway)
    if (i == 0)
    {
      iterRegTag.IncZ();
      if (bOptimizationPossible == true && iterRegTag.GetCurrentBlock().IsHomogenous())
        break; // skip adjacent homogenous block
      iterRegTag.DecZ();
    }
*/
    for (j=0; j<16; iterRegTag.IncY(), j++)
    {
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
    iterRegTag.IncX();
    for (j=0; j<16; j++)
    {
      iterRegTag.DecY();
      (this->*pNtraversal)(iterRegTag, iterNeighbour, iterProcTagVol);
    }
  } // end for z==16 plane

  // reset iterator within current block
  iterRegTag.SetPos(position);

  return;
} // BlockFaceTraversal()


/**
 * Private method that updates the segmentation volume
 * @param pTagVol pointer to tag volume
 * @param uCompLabel componet label
 */
void MorphOp::UpdateSegVolume(vxBlockVolume<bool> * pTagVol, 
                              uint2 uCompLabel)
//*******************************************************************
{
  if (m_pSegVols != NULL)
  {
    m_pSegVols->UpdateSegVolume(pTagVol,uCompLabel);
  }
  return;
} // UpdateSegVolume()


/**
 * Private method that applies the morphological results to a tag volume
 * @param pTagVolIn  pointer to tag volume
 * @param uCompLabel componet label
 */
void MorphOp::ApplyMorphToTag(vxBlockVolume<bool> * pTagVolIn, 
                              bool bTag)
//*******************************************************************
{
  Timer timer;

  if (pTagVolIn == NULL)
  {
    throw ex::NullPointerException(LogRec("Tag volume pointer is null", "MorphOp", "ApplyMorphToTag"));
  }

  // apply results to tag volume
  ////////////////////////////////
  vxBlockVolumeIterator<bool> tagVolInIter(*pTagVolIn);
  vxBlockVolumeIterator<bool> morphTagIter(m_morphTagVol);

  for (morphTagIter.FirstNonBackgroundBlockZYX(); morphTagIter.IsNotAtEnd(); morphTagIter.NextNonBackgroundBlockZYX())
  {
    for (morphTagIter.FirstNonBackgroundZYXinsideBlock(); morphTagIter.IsNotAtEndOfBlock(); morphTagIter.NextNonBackgroundZYXinsideBlock())
    {
      tagVolInIter.SetPos(morphTagIter);
      tagVolInIter.SetVoxel(bTag);
    }
  }

  pTagVolIn->Compact();
  LogDbg(string("ApplyMorphToTag took ")+ToAscii(timer.ElapsedTime()), "MorphOp", "ApplyMorphToTag");

  return;
} // end ApplyMorphToTag


#undef FILE_REVISION


// Revision History:
// $Log: MorphOp.C,v $
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.43  2003/07/08 14:41:41  michael
// renamed functions to be descriptive
//
// Revision 1.42  2003/07/03 17:49:26  michael
// added functionality to grow and shrink within range
//
// Revision 1.41  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.40  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.39  2003/06/09 16:17:34  michael
// cosmetics
//
// Revision 1.38  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.37  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.36  2003/04/28 17:01:31  michael
// more code reviews
//
// Revision 1.35  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.34  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.33  2002/12/13 19:21:36  ingmar
// restricted "grow" operation (Dilate) to only include voxels in the active intensity range as long as it is beeing displayed
//
// Revision 1.32  2002/07/25 20:36:59  jaddonisio
// Skip traversal inside homogeneous blocks during morph operations.
//
// Revision 1.31  2002/06/05 14:58:46  jaddonisio
// Some spacing changes.
//
// Revision 1.30  2002/06/03 21:30:24  jaddonisio
// Coding standard changes.
//
// Revision 1.29  2002/05/21 21:06:35  jaddonisio
// Replaced SetVoxel with call without iterator(temporarily).
//
// Revision 1.28  2002/05/16 04:08:47  soeren
// added more exceptions
//
// Revision 1.27  2002/05/07 19:42:47  jaddonisio
// Using GetVoxel with iterator input.
//
// Revision 1.26  2002/05/07 18:56:50  jaddonisio
// Don't allow dilation into locked regions.
//
// Revision 1.25  2002/05/07 16:04:15  ingmar
// added timings & fixed erosion
//
// Revision 1.24  2002/05/07 15:28:28  ingmar
// added timings
//
// Revision 1.23  2002/05/03 21:11:17  soeren
// fixed firstnon.. functions
//
// Revision 1.22  2002/05/03 19:06:21  jaddonisio
// Changed processVol to the new block volume.
//
// Revision 1.21  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.20  2002/05/02 22:02:05  jaddonisio
// Commented out old block volume and associated functions(will cleanup soon).
//
// Revision 1.19  2002/05/01 19:49:20  jaddonisio
// Fixed label volume update. Also, removed threshold for morphing.
//
// Revision 1.18  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.17  2002/04/30 15:32:00  soeren
// added firstnonbackgroundzyxinsideblock
//
// Revision 1.16  2002/04/30 14:49:16  soeren
// changed all for loop frok nextnonbackground to nextnonbackgroundblock + nextnonbackgroundinsideblock
//
// Revision 1.15  2002/04/30 14:42:33  jaddonisio
// Dilate with new volume.
//
// Revision 1.14  2002/04/29 19:20:00  jaddonisio
// Moved seg. vol. update within each region morphing.
//
// Revision 1.13  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.12  2002/04/18 16:24:33  jaddonisio
// i. Try Phi =1 & Chi = 1 for morphing - doesn't have problem with termination of morphing like Phi > 1 & Chi > 1.
// ii. Contains an instance of the SegmentationVolumes class.
//
// Revision 1.11  2002/04/15 15:33:31  jaddonisio
// New class: SegmentationVolumes.
//
// Revision 1.10  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.9  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.8  2001/12/06 19:58:59  jaddonisio
// Mods to comply with coding standard
//
// Revision 1.7  2001/11/30 23:46:32  jaddonisio
// Allignment of code.
//
// Revision 1.6  2001/11/21 20:05:52  jaddonisio
// New morphology based on vector or region tag volumes(removed seed point methods).
//
// Revision 1.5  2001/11/18 00:39:31  jaddonisio
// Fixes problem at volume boundaries during erosion.
//
// Revision 1.4  2001/11/11 05:28:22  jaddonisio
// Implemented dilation.
//
// Revision 1.3  2001/11/11 02:27:16  jaddonisio
// Implemented erosion.
//
// Revision 1.2  2001/11/10 03:58:15  jaddonisio
// Modified enums. Also, disable 'Erosion' temporarily(until I can get it to work).
//
// Revision 1.1  2001/11/09 21:55:03  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/MorphOp.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
// $Id: MorphOp.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
