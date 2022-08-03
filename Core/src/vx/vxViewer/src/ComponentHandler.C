// $Id: ComponentHandler.C,v 1.6 2006/03/29 20:01:25 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)


/**
 * Class that encapsulates the Component Handler. This class provides
 * methods to handle mask tag and region tag volumes associated with
 * components.
 */


// includes
#include "stdafx.h"
#include "ComponentHandler.h"
#include "Point.h"
#include "vxBlockVolume.h"
#include "Timer.h"
#include "vxComponentUtils.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.6 $"


// static member initializations
std::vector<PixelRGBA<uint1> > ComponentColors::m_vDefaultColors;


/**
 * Default constructor for ComponentHandler class.
 */
ComponentHandler::ComponentHandler()
{
  try
  {
    m_defaultSegLblVol.GetHeader().SetVolDim(2,2,2);
    m_pSegLblVol  = & m_defaultSegLblVol;
    m_maskTagVol.GetHeader().SetVolDim(2,2,2);
    m_maskTagVol.GetHeader().SetVoxelUnits(Triplef(1,1,1));

    for (uint2 i(0); i<MAXN_COMP; i++)
    {
      m_vRegTagVols[i].resize(0, NULL);
    }

    m_pvpComponents = &m_vpDefaultComponents;
  }
  catch ( ... )
  {
    LogDbg( "unhandled exception", "ComponentHandler", "ComponentHandler" );
  }
} // end default constructor


/**
 * Destructor for ComponentHandler class.
 */
ComponentHandler::~ComponentHandler()
{
  try
  {
    // Delete component regions:
    std::vector< vxBlockVolume<bool> * >::iterator iterRegTagVol;
    for (int4 i(0); i<MAXN_COMP; i++)
    {
      // Remove allocated region tag volumes:
      for (iterRegTagVol = m_vRegTagVols[i].begin(); iterRegTagVol != m_vRegTagVols[i].end(); ++iterRegTagVol)
      {
        if (*iterRegTagVol != NULL)
        {
          delete *iterRegTagVol;
          *iterRegTagVol = NULL;
        }
      }
    } // end for all components
  }
  catch ( ... )
  {
    LogDbg( "unhandled exception", "ComponentHandler", "~ComponentHandler" );
  }

} // end destructor


/**
 * Public method to initialize component handler.
 */
void ComponentHandler::Initialize()
{
  try
  {
    m_segVols.CondenseLabelsInSegVolume();
    m_pvpComponents->Compact();
    CreateRegionTagVols();
    UpdateMaskTagVol(*m_pSegLblVol);
  }
  catch ( ... )
  {
    LogDbg( "unhandled exception", "ComponentHandler", "Initialize" );
  }
} // Initialize()


/**
 * Public method to be used to update component handling.
 */
void ComponentHandler::OnUserInputUpdate()
{
  try
  {
    UpdateMaskTagVol(*m_pSegLblVol);
  }
  catch (...)
  {
    throw ex::VException(LogRec("OnUserInputUpdate failed.", "ComponentHandler", "OnUserInputUpdate"));    
  }
} // OnUserInputUpdate()


/**
 * Private method to create region tags based on the segmentation label volume.
 */
void ComponentHandler::CreateRegionTagVols()
{
  try
  {
    Timer timer;

    // Delete existing region tag volumes:
    std::vector<vxBlockVolume<bool> *>::iterator iterRegTagVols;
    for (int4 i(0); i<MAXN_COMP; i++)
    {
      for (iterRegTagVols=m_vRegTagVols[i].begin(); iterRegTagVols!=m_vRegTagVols[i].end(); iterRegTagVols++)
      {
        if (*iterRegTagVols != NULL)
        {
          delete *iterRegTagVols;
          *iterRegTagVols = NULL;
        }
      }
      m_vRegTagVols[i].resize(0,NULL);
    } // end for all components

    std::vector<vxBlockVolumeIterator<bool> *> vRegTagVols[MAXN_COMP];

    // Allocate new region tags:
    for (int4 j(0); j<GetComponentCount(); j++)
    {
      vxBlockVolume<bool> * pRegTagVol = new vxBlockVolume<bool>(m_regTagVolHdr);
      if (pRegTagVol == NULL)
      {
        throw ex::VException(LogRec("Could not allocate Region Tag Volume.", "ComponentHandler", "DeleteComp"));    
      }
      m_vRegTagVols[j].push_back(pRegTagVol);
      vRegTagVols[j].push_back(new vxBlockVolumeIterator<bool>(*pRegTagVol));
    } // end for all components

    // Set region tags from(segmentation label volume):
    vxBlockVolumeIterator<uint2> iterSegLabel(*m_pSegLblVol);
    for (iterSegLabel.FirstNonBackgroundBlockZYX(); iterSegLabel.IsNotAtEnd(); iterSegLabel.NextNonBackgroundBlockZYX())
    {
      if (iterSegLabel.GetCurrentBlock().IsHomogenous())
      {
        const uint2 uIndexHistory(m_pvpComponents->GetHistoryFromLabel(iterSegLabel.GetVoxel()));
        (*vRegTagVols[uIndexHistory].at(0)).SetPos(iterSegLabel);
        (*vRegTagVols[uIndexHistory].at(0)).SetClearBlock(true);
        (*vRegTagVols[uIndexHistory].at(0)).ClearCurrentBlock();
      }
      else // non homogeneous blocks
      {
        for (iterSegLabel.FirstNonBackgroundZYXinsideBlock(); iterSegLabel.IsNotAtEndOfBlock(); iterSegLabel.NextNonBackgroundZYXinsideBlock())
        {
          const uint2 uIndexHistory(m_pvpComponents->GetHistoryFromLabel(iterSegLabel.GetVoxel()));
          if (uIndexHistory < MAXN_COMP)
          {
            (*vRegTagVols[uIndexHistory].at(0)).SetVoxel(true, iterSegLabel);
          }
        }
      }
    } // for all non background blocks

    // delete pointers to iterators
    for (int4 k(0); k<GetComponentCount(); k++)
    {
      for (std::vector<vxBlockVolumeIterator<bool> *>::iterator iter = vRegTagVols[k].begin(); iter != vRegTagVols[k].end(); iter++)
      {
        if (*iter != NULL)
        {
          delete *iter;
          *iter = NULL;
        }
      }
    } // for all components

    LogDbg(string("CreateRegionTagVols took ") + ToAscii(timer.ElapsedTime()), "ComponentHandler", "CreateRegionTagVols"); timer.Reset();
  } // try
  catch (...)
  {
    throw ex::VException(LogRec("CreateRegionTagVols failed.", "ComponentHandler", "CreateRegionTagVols"));    
  }
} // CreateRegionTagVols()


/**
 * Private method to update mask tag volume based on component's status
 *
 * @param rSegLblVol reference to a segmentation volume.
 */
void ComponentHandler::UpdateMaskTagVol(const vxBlockVolume<uint2> & segLblVol)
{
  try
  {
    // Loop through all of Label & Tag Volume:
    Timer timer;
    m_maskTagVol.Clear();
    m_maskTagVol.GetHeader().SetVolDim(segLblVol.GetHeader().GetVolDim());
    vxBlockVolumeIterator<bool> iterMaskTag(m_maskTagVol);
    vxBlockVolumeIterator<uint2> iterSegLabel((vxBlockVolume<uint2> &)segLblVol);
    for (iterSegLabel.FirstNonBackgroundBlockZYX(); iterSegLabel.IsNotAtEnd(); iterSegLabel.NextNonBackgroundBlockZYX())
    {
      if (iterSegLabel.GetCurrentBlock().IsHomogenous())
      {
        iterMaskTag.SetPos(iterSegLabel);
        iterMaskTag.SetClearBlock((bool) m_pvpComponents->ArrayByLabel()[iterSegLabel.GetVoxel()]->IsLocked());
        iterMaskTag.ClearCurrentBlock();
      }
      else
      {
        for (iterSegLabel.FirstNonBackgroundZYXinsideBlock(); iterSegLabel.IsNotAtEndOfBlock(); iterSegLabel.NextNonBackgroundZYXinsideBlock())
        {
          iterMaskTag.SetPos(iterSegLabel);
          iterMaskTag.SetVoxel((bool) m_pvpComponents->ArrayByLabel()[iterSegLabel.GetVoxel()]->IsLocked());
        }
      }
    } // end for all non background blocks

    LogDbg(string("UpdateMaskTagVol took ") + ToAscii(timer.ElapsedTime()), "ComponentHandler", "UpdateMaskTagVol"); timer.Reset();
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("UpdateMaskTagVol failed.", "ComponentHandler", "UpdateMaskTagVol"));    
  }
} // end UpdateMaskTagVol()


/**
 * Public method to update mask tag volume based on component's status.
 * @param history index of component
 */
void ComponentHandler::UpdateCompMask(const uint2 & uIndexHistory)
{
  try
  {
    Timer timer;

    // Loop through all of Label & Tag Volume:
    vxBlockVolumeIterator<bool> iterMaskTag(m_maskTagVol);
    std::vector<vxBlockVolume<bool> *>::iterator iterRegTagVols;

    if (uIndexHistory > 0) // anything but NONE
    {
      bool bLock((*m_pvpComponents)[uIndexHistory]->IsLocked());
      for (iterRegTagVols = m_vRegTagVols[uIndexHistory].begin(); iterRegTagVols != m_vRegTagVols[uIndexHistory].end(); iterRegTagVols++)
      {
        vxBlockVolumeIterator<bool> iterRegTag(**iterRegTagVols);
        for (iterRegTag.FirstNonBackgroundBlockZYX(); iterRegTag.IsNotAtEnd(); iterRegTag.NextNonBackgroundBlockZYX())
        {
          if (iterRegTag.GetCurrentBlock().IsHomogenous())
          {
            iterMaskTag.SetPos(iterRegTag);
            iterMaskTag.SetClearBlock(bLock);
            iterMaskTag.ClearCurrentBlock();
          }
          else
          {
            for (iterRegTag.FirstNonBackgroundZYXinsideBlock(); iterRegTag.IsNotAtEndOfBlock(); iterRegTag.NextNonBackgroundZYXinsideBlock())
            {
              iterMaskTag.SetPos(iterRegTag);
              iterMaskTag.SetVoxel(bLock);
            }
          }
        }
      } // end for all region tag vols of component
    } // end if index not representing NONE
    else // it is actually NONE
    {
      vxBlockVolumeIterator<uint2> iterSegLabel(*m_pSegLblVol);
      bool bLock((*m_pvpComponents)[0]->IsLocked());
      for (iterSegLabel.SetPos(0,0,0); iterSegLabel.IsNotAtEnd(); iterSegLabel.NextBlockZYX())
      {
        if (iterSegLabel.GetCurrentBlock().IsHomogenous() && (iterSegLabel.GetVoxel() == 0))
        {
          iterMaskTag.SetPos(iterSegLabel);
          iterMaskTag.SetClearBlock(bLock);
          iterMaskTag.ClearCurrentBlock();
        }
        else // non homogeneous block
        {
          for (; iterSegLabel.IsNotAtEndOfBlock(); iterSegLabel.NextZYXinsideBlock())
          {
            if (iterSegLabel.GetVoxel() == 0)
            {
              iterMaskTag.SetVoxel(bLock, iterSegLabel);
            }
          }
        }
      } // end for iterate over label volume blocks
    } // else  is actually NONE

    LogDbg(string("UpdateCompMask took ")+ToAscii(timer.ElapsedTime()), "ComponentHandler", "UpdateCompMask"); timer.Reset();
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("UpdateCompMask failed.", "ComponentHandler", "UpdateCompMask"));    
  }
} // end UpdateCompMask()


/**
 * Public method to update the tag volume(s) associated with a compoenent.
 */
void ComponentHandler::UpdateRegionTags()
{
  try
  {
    int iCount(0);

    std::vector<uint2> vCollapsedComponents;

    Timer timer;
    vxBlockVolumeIterator<uint2> iterSegLabel(*m_pSegLblVol);
    vxBlockVolumeIterator<bool> iterMaskTag(m_maskTagVol);
    vector<vxBlockVolume<bool> *>::iterator iterRegTagVols;
    for (int4 iIndexHistory(1); iIndexHistory<GetComponentCount(); iIndexHistory++) // complete vector of component pointers
    {
      if ((*m_pvpComponents)[iIndexHistory]->IsLocked() == false)
      {
        bool bVoxelsFound(false);
        for (iterRegTagVols = m_vRegTagVols[iIndexHistory].begin(); iterRegTagVols != m_vRegTagVols[iIndexHistory].end(); iterRegTagVols++)
        {
          vxBlockVolumeIterator<bool> iterRegTag(**iterRegTagVols);
          for (iterRegTag.FirstNonBackgroundBlockZYX(); iterRegTag.IsNotAtEnd(); iterRegTag.NextNonBackgroundBlockZYX())
          {
            for (iterRegTag.FirstNonBackgroundZYXinsideBlock(); iterRegTag.IsNotAtEndOfBlock(); iterRegTag.NextNonBackgroundZYXinsideBlock())
            {
              iterSegLabel.SetPos(iterRegTag.GetPos());
              if (iterSegLabel.GetVoxel() != (*m_pvpComponents)[iIndexHistory]->GetLabel())
              {
                iterRegTag.SetVoxel(false);
                ++iCount;
              }
              else if (!bVoxelsFound)
              {
                bVoxelsFound = true;
              }
            }
          } // end for all non background blocks
        } // end for (all region tags for this component)

        UpdateCompMask(iIndexHistory);
        if (bVoxelsFound == false)
        {
          vCollapsedComponents.push_back(iIndexHistory);
        }
      } // end if component unlocked
    } // end for (all components)

    // now remove all collapsed components
    for (int i(0); i<vCollapsedComponents.size(); ++i)
    {
      DeleteComp(vCollapsedComponents[i]);
    }

    // Update of NONE
    if ((*m_pvpComponents)[0]->IsLocked())
    {
      vxBlockVolumeIterator<bool> iterDelTag(*(m_segVols.GetDeltaSegTagVol()));
      for (iterDelTag.FirstNonBackgroundBlockZYX(); iterDelTag.IsNotAtEnd(); iterDelTag.NextNonBackgroundBlockZYX())
      {
        iterSegLabel.SetPos(iterDelTag);
        if (iterDelTag.GetCurrentBlock().IsHomogenous() && (iterSegLabel.GetVoxel() == 0))
        {
          iterMaskTag.SetPos(iterDelTag);
          iterMaskTag.SetClearBlock(true);
          iterMaskTag.ClearCurrentBlock();
        }
        else // non homogeneous NONE blocks
        {
          for (iterDelTag.FirstNonBackgroundZYXinsideBlock(); iterDelTag.IsNotAtEndOfBlock(); iterDelTag.NextNonBackgroundZYXinsideBlock())
          {
            iterSegLabel.SetPos(iterDelTag);
            if (iterSegLabel.GetVoxel() == 0)
            {
              iterMaskTag.SetVoxel(true,iterDelTag);
            }
          }
        }
      } // end for non background blocks
    } // end if NONE locked

    bool bShowTimingsInLog(true);
    if (bShowTimingsInLog) LogDbg(string("UpdateRegionTags took ")+ToAscii(timer.ElapsedTime()), "ComponentHandler", "UpdateRegionTags");timer.Reset();
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("UpdateRegionTags failed.", "ComponentHandler", "UpdateRegionTags"));    
  }
} // end UpdateRegionTags()


/**
 * Public method to modify mask tag volume for a region based on input value
 *
 * @param pRegTagVol pointer to region tag volume.
 * @param bVal       value of tag to be set.
 */
void ComponentHandler::ModifyMaskTagVol(vxBlockVolume<bool> * pRegTagVol, 
                                        bool bVal)
{
  try
  {
    // Loop through all of Tag Volume:
    Timer timer;
    vxBlockVolumeIterator<bool> iterRegTag(*pRegTagVol);
    vxBlockVolumeIterator<bool> iterMaskTag(m_maskTagVol);

    iterMaskTag.SetClearBlock(bVal);

    for (iterRegTag.FirstNonBackgroundBlockZYX(); iterRegTag.IsNotAtEnd(); iterRegTag.NextNonBackgroundBlockZYX())
    {

      // try to set the entire block at once
      if (iterRegTag.GetCurrentBlock().IsHomogenous() && iterRegTag.GetVoxel() == true)
      {
        iterMaskTag.SetPos(iterRegTag);
        iterMaskTag.ClearCurrentBlock();
      }
      else
      {
        for (iterRegTag.FirstNonBackgroundZYXinsideBlock();iterRegTag.IsNotAtEndOfBlock(); iterRegTag.NextNonBackgroundZYXinsideBlock())
        {
          iterMaskTag.SetPos(iterRegTag.GetPos());
          iterMaskTag.SetVoxel(bVal);
        }
      }    
    } // end for (all non-zero tags)

    m_maskTagVol.Compact();
    LogDbg(string("ModifyMaskTagVol took ")+ToAscii(timer.ElapsedTime()), "ComponentHandler", "ModifyMaskTagVol"); timer.Reset();
  }
  catch (...)
  {
    throw ex::VException(LogRec("UpdateMaskTagVol failed.", "ComponentHandler", "ModifyMaskTagVol"));    
  }
} // end ModifyMaskTagVol()


/**
 * AddCompReg
 * Public method to add a component region
 *
 * @param a component label
 * @param eRegGrowAddOpt  region add option(new or append)
 * @return Pointer to region tag volume(block volume of bools)
 */
vxBlockVolume<bool> * ComponentHandler::AddCompReg(uint2 & uIndexLabel, 
                                                   const SegParams::RegionAddOptionEnum eRegAddOpt)
{
  try
  {
    // Determine component label (new or append)
    //////////////////////////////////////////////
    Timer timer;
    uIndexLabel = 0;
    uint2 uIndexHistory(0);

    if (eRegAddOpt == SegParams::APPEND_REGIONS)
    {
      uIndexHistory = GetSelectedComponentIndex();
      uIndexLabel   = (*m_pvpComponents)[uIndexHistory]->GetLabel();
    } // end if (region add option is append)

    // Do we need to create new component
    if (uIndexHistory == 0)
    {
      // Find available color:
      PixelRGBA<uint1> newCompColor = ComponentColors::GetUniqueColor( m_pvpComponents );

      // name, add, and lock it
      string sNewCompName(vxComponentUtils::GetUniqueComponentName(*m_pvpComponents, "Component"));
      if ( m_pvpComponents->GetCount() + 1 >= MAXN_COMP )
      {
        throw ex::OutOfMemoryException( LogRec( "Maximum number of components reached.", "ComponentHandler", "AddCompReg" ) );
      }
      uIndexHistory = m_pvpComponents->AddComponent(sNewCompName, newCompColor);
      uIndexLabel   = (*m_pvpComponents)[uIndexHistory]->GetLabel();
      (*m_pvpComponents)[uIndexHistory]->SetLocked(true);
    } // end if NONE

    // Allocate associated Region Tag Volume for component
    ////////////////////////////////////////////////////////
    vxBlockVolume<bool> * pRegTagVol(NULL);

    if (0<uIndexHistory && uIndexHistory<MAXN_COMP)
    {
      pRegTagVol = new vxBlockVolume<bool>(m_regTagVolHdr);
      if (pRegTagVol == NULL)
      {
        throw ex::VException(LogRec("Could not allocate Region Tag Volume.", "ComponentHandler", "AddCompReg"));    
      }
      m_vRegTagVols[uIndexHistory].push_back(pRegTagVol);
    } // end if valid index

    LogDbg(string("AddCompReg took ")+ToAscii(timer.ElapsedTime()), "ComponentHandler", "AddCompReg"); timer.Reset();

    return(pRegTagVol);
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("AddCompReg failed.", "ComponentHandler", "AddCompReg"));    
  }
} // end AddCompReg()


/**
 * Public method that deletes a component
 *
 * @param uCompIndex component index.
 */
void ComponentHandler::DeleteComp(const uint2 uIndexHistory)
{
  try
  {
    if (0<uIndexHistory && uIndexHistory<m_pvpComponents->GetCount())
    {
      // Reset the delta tag volume of segmentation:
      m_segVols.InitDeltaSegTagVolume();

      // Remove region associated with component:
      RemCompReg(uIndexHistory);
    }
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("DeleteComp " + ToAscii(uIndexHistory) + "failed.", "ComponentHandler", "DeleteComp"));    
  }
} // end DeleteComp()


/**
 * Public method to clear all components
 */
void ComponentHandler::DeleteAllComps()
{
  try
  {
    // Reset the delta tag volume of segmentation:
    m_segVols.InitDeltaSegTagVolume();

    // Clear label volume:
    m_segVols.RemoveAllLabelsFromSegVolume();

    // Clear mask volume:
    m_maskTagVol.Clear();

    // Clear all region tags:
    while (m_pvpComponents->GetCount() > 1)
    {
      uint2 uIndexHistory(m_pvpComponents->GetCount()-1);
      std::vector< vxBlockVolume<bool> * >::iterator iterRegTagVols;
      for (iterRegTagVols = m_vRegTagVols[uIndexHistory].begin(); iterRegTagVols != m_vRegTagVols[uIndexHistory].end(); iterRegTagVols++)
      {
        if (*iterRegTagVols != NULL)
        {
          delete *iterRegTagVols;
          *iterRegTagVols = NULL; 
        }
      }
      m_vRegTagVols[uIndexHistory].clear();
      m_pvpComponents->RemoveComponent(uIndexHistory);
    } // end while components around
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("DeleteAllComps failed.", "ComponentHandler", "DeleteAllComps"));    
  }
} // end DeleteAllComps()


/**
 * Public method to remove region(s) comprising a component.
 *
 * @param uCompIndex component index.
 */
void ComponentHandler::RemCompReg(const uint2 & uIndexHistory)
{
  try
  {
    // Delete region tag volume associated with component:
    std::vector<vxBlockVolume<bool> *>::iterator iterRegTagVols;
    for (iterRegTagVols = m_vRegTagVols[uIndexHistory].begin(); iterRegTagVols != m_vRegTagVols[uIndexHistory].end(); iterRegTagVols++)
    {
      if (*iterRegTagVols != NULL)
      {
        m_segVols.RemoveLabelFromSegVolume(**iterRegTagVols);
        ModifyMaskTagVol(*iterRegTagVols, false);
        delete *iterRegTagVols; *iterRegTagVols = NULL;
      }
    } // end for all region tags volumes

    // Shift down all remaining region pointers within array:
    uint2 uCompCnt(m_pvpComponents->GetCount());
    for (int4 i(uIndexHistory); i<uCompCnt-1; i++)
    {
      m_vRegTagVols[i].assign(m_vRegTagVols[i+1].begin(), m_vRegTagVols[i+1].end());
    }

    // Remove last element
    m_vRegTagVols[uCompCnt-1].erase(m_vRegTagVols[uCompCnt-1].begin(),m_vRegTagVols[uCompCnt-1].end());

    if (uIndexHistory < GetComponentCount())
    {
      m_pvpComponents->RemoveComponent(uIndexHistory);
    }
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("RemCompReg failed.", "ComponentHandler", "RemCompReg"));    
  }
} // RemCompReg()


/**
 * Public method to test if component is segmented
 *
 * @param uLabelIndex label index of component.
 * @return Result of test(true/false).
 */
bool ComponentHandler::IsComponentSegmented(const uint4 uLabelIndex)
{
  bool bTest(false);
  try
  {
    if (m_pSegLblVol != NULL)
    {
      vxBlockVolumeIterator<uint2> iterLabel(*m_pSegLblVol);
      for (iterLabel.FirstNonBackgroundBlockZYX(); iterLabel.IsNotAtEnd(); iterLabel.NextNonBackgroundBlockZYX())
      {
        for (iterLabel.FirstNonBackgroundZYXinsideBlock(); iterLabel.IsNotAtEndOfBlock(); iterLabel.NextNonBackgroundZYXinsideBlock())
        {
          if (iterLabel.GetVoxel() == (*m_pvpComponents)[uLabelIndex]->GetLabel())
          {
            bTest = true;
            break;
          }
        }
      }
    } // end if label volume not NULL
  }
  catch ( ... )
  {
    LogDbg( "unhandled exception ComponentHandler::IsComponentSegmented()", "ComponentHandler", "IsComponentSegmented" );
  }

  return(bTest);
} // end IsComponentSegmented()


/**
 * Public method to set default colors for new components.
 */
void ComponentColors::SetDefaultColors()
{
  try
  {
    m_vDefaultColors.resize(MAXN_COMP);
    for (uint4 k(0); k<MAXN_COMP; ++k)
    {
      m_vDefaultColors[k] = PixelRGBA<uint1>::GetColorPalettePixel(k%256);
    }
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("SetDefaultColors failed.", "ComponentHandler", "SetDefaultColors"));
  }
} // SetDefaultColors()


/**
 * get a color that is not already used
 *
 * @param pComponentArray the component array
 *
 * @return a unique color (if any possible)
 */
PixelRGBA<uint1> ComponentColors::GetUniqueColor( vxComponentArray * pComponentArray )
{

  if ( m_vDefaultColors.size() == 0 )
  {
    SetDefaultColors();
  }

  PixelRGBA<uint1> newCompColor( 255, 0, 0, 0 );

  for ( std::vector< PixelRGBA<uint1> >::iterator vecDefaultColors_iter = m_vDefaultColors.begin();
        vecDefaultColors_iter != m_vDefaultColors.end();
        vecDefaultColors_iter++ )
  {

    uint1 uRed  ( vecDefaultColors_iter->R() );
    uint1 uGreen( vecDefaultColors_iter->G() );
    uint1 uBlue ( vecDefaultColors_iter->B() );
    uint1 uAlpha( vecDefaultColors_iter->A() );
    uint4 uTestCnt( 0 );

    for ( int4 n(0); n<pComponentArray->GetCount(); n++ )
    {
      if ( ( * pComponentArray )[ n ] != NULL )
      {
        const PixelRGBA<uint1> & compColor = ( * pComponentArray )[ n ]->GetColor();
        if ( uRed   != compColor.R() ||
             uGreen != compColor.G() ||
             uBlue  != compColor.B() ||
             uAlpha != compColor.A() )
        {
          ++uTestCnt;
        } 
      }
    } // end for all components
    if ( uTestCnt == pComponentArray->GetCount() )
    {
      newCompColor.SetRGBA( uRed, uGreen, uBlue, uAlpha );
      break;
    } 
  } // end for all default colors

  return newCompColor;

} // GetUniqueColor()


#undef FILE_REVISION


// Revision History:
// $Log: ComponentHandler.C,v $
// Revision 1.6  2006/03/29 20:01:25  geconomos
// removed inclusion of vxBlockVolumeBinaryHistogram.h
//
// Revision 1.5  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.4  2006/02/17 15:07:24  frank
// split out component colors from the component handler
//
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.2  2005/02/15 17:51:34  frank
// Issue #3941: Increase the number of possible components
//
// Revision 1.1.1.1.6.1  2005/02/15 17:13:58  frank
// added exception when adding too many components
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.82  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.81  2003/06/07 15:35:47  michael
// reformatting
//
// Revision 1.80  2003/05/17 20:29:04  michael
// minor change
//
// Revision 1.79  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.78  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.77  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.76  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.75  2003/04/07 20:28:05  michael
// fixed issue #3041
//
// Revision 1.74  2003/04/07 15:10:59  michael
// cosmetics
//
// Revision 1.73  2003/01/22 21:51:59  ingmar
// dimension and units are now Triples
//
// Revision 1.72  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.71  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.70  2002/12/19 21:30:21  ingmar
// back to timings in log
//
// Revision 1.69  2002/12/12 17:07:38  ingmar
// removed binary histogram 04 ( 3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.68  2002/11/27 23:04:16  michael
// cosmetics
//
// Revision 1.67  2002/11/26 13:42:42  michael
// renamed method (GetHistoryFromLabel())
//
// Revision 1.66  2002/11/15 23:07:15  ingmar
// removed timings from log
//
// Revision 1.65  2002/11/15 14:52:41  michael
// after deletion, set pointer back to NULL (clean code!!!)
//
// Revision 1.64  2002/11/14 16:30:10  michael
// removed redundant code
//
// Revision 1.63  2002/11/13 00:11:46  michael
// account for changes in vxComponent and vxComponentArray
//
// Revision 1.62  2002/09/13 13:50:30  jaddonisio
// Lock background.
//
// Revision 1.61  2002/09/11 18:17:05  jaddonisio
// Removed some debug code.
//
// Revision 1.60  2002/09/11 15:50:21  jaddonisio
// i. Call 'CondenseLabelsInSegVol' function in initialization.
// ii. Make 'CreateRegionTags' a little bit faster.
// iii. Clean-up.
//
// Revision 1.59  2002/09/05 22:48:11  jaddonisio
// New vxComponent.
//
// Revision 1.58  2002/08/20 15:10:26  jaddonisio
// New function to calculate mask based on block04(used for new region grow iterator).
//
// Revision 1.57  2002/07/30 17:16:56  michael
// Fixed call on NULL pointer
//
// Revision 1.56  2002/07/25 20:35:30  jaddonisio
// Check for homogeneous blocks when updating component mask.
//
// Revision 1.55  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.54  2002/07/11 16:37:37  ingmar
// added timers and exceptions
//
// Revision 1.53  2002/07/09 17:58:57  jaddonisio
//  i. Initialize region tag pointers to NULL in constructor
// ii. resize after deleting exisitng regions in 'CreateRegionTagVols'.
//
// Revision 1.52  2002/07/08 23:34:49  ingmar
// used the homogeneous flag to significantly improve the speed of UpdateMaskTagVol()  (in our example from 4.8s to 0.1s)
//
// Revision 1.51  2002/07/02 23:54:52  soeren
// i. Made MaskVolume & deltaVolume compact.
// ii. Created 'RemoveAllLabelsFromSegVol'
//
// Revision 1.50.2.2  2002/07/11 14:58:25  jaddonisio
// Check homegenous status for MaskVolume.
//
// Revision 1.50.2.1  2002/07/02 18:28:16  michael
// i. Initialize region tag pointers to NULL in constructor
// ii. resize after deleting exisitng regions in 'CreateRegionTagVols'.
//
// - jaddonisio
//
// Revision 1.50  2002/06/13 19:11:45  frank
// Optimized sculpt
//
// Revision 1.49  2002/06/11 22:11:04  manju
// Removed a small bug.
//
// Revision 1.48  2002/06/11 21:23:38  ingmar
// the color palette is now computed through PixelRGBA<uint1>::GetColorPalletPixel()
//
// Revision 1.47  2002/06/11 21:01:40  jaddonisio
// Take out last change(found a problem, oops).
//
// Revision 1.46  2002/06/11 20:49:28  jaddonisio
// Create region tags when initialize component handler.
//
// Revision 1.45  2002/06/10 18:26:17  jaddonisio
// New updates for component mask.
//
// Revision 1.44  2002/06/06 19:14:52  manju
// Added method Initialize.
//
// Revision 1.43  2002/06/03 21:31:07  jaddonisio
// Coding standard changes.
//
// Revision 1.42  2002/06/03 15:40:11  manju
// Issue 2334: Modified naming of a new component.
//
// Revision 1.41  2002/05/28 15:37:12  jaddonisio
// New method to test if component is segmented.
//
// Revision 1.40  2002/05/21 21:07:21  jaddonisio
// Replaced SetVoxel with call without iterator(temporarily).
//
// Revision 1.39  2002/05/15 15:07:55  soeren
// added more exceptions
//
// Revision 1.38  2002/05/15 15:04:26  soeren
// added more exceptions
//
// Revision 1.37  2002/05/09 16:13:18  jaddonisio
// Corrected end check for loop inside block.
//
// Revision 1.36  2002/05/07 19:01:07  jaddonisio
// Corrected iterations through block for update on user input.
//
// Revision 1.35  2002/05/07 18:53:57  soeren
// added timers and optimized UpdateMaskTagVol
//
// Revision 1.34  2002/05/03 22:25:47  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.33  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.32  2002/05/01 20:00:07  jaddonisio
// m_pMaskTagVol is old volume.
//
// Revision 1.31  2002/05/01 19:50:54  jaddonisio
// VolumeSet/AnyVolume no longer used.
//
// Revision 1.30  2002/05/01 18:55:53  jenny
// initilized blk volume to NULL
//
// Revision 1.29  2002/05/01 13:57:41  jaddonisio
// Removed AnyVolume references.
//
// Revision 1.28  2002/04/30 15:32:00  soeren
// added firstnonbackgroundzyxinsideblock
//
// Revision 1.27  2002/04/30 14:49:16  soeren
// changed all for loop frok nextnonbackground to nextnonbackgroundblock + nextnonbackgroundinsideblock
//
// Revision 1.26  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.25  2002/04/20 19:46:33  jaddonisio
// Corrected error, not be able to delete last component.
//
// Revision 1.24  2002/04/18 18:48:36  jaddonisio
// Don't delete an invalid label.
//
// Revision 1.23  2002/04/18 16:21:03  jaddonisio
// i. New methods to set and get properties of components.
// ii. Contains an instance of the SegmentationVolumes class.
//
// Revision 1.22  2002/04/12 19:19:02  manju
// Proper deletion of m_pMasktagVol.
//
// Revision 1.21  2002/03/08 22:38:15  dmitry
// Changed default volume sizes to 2x2x2.
//
// Revision 1.20  2002/01/17 15:17:51  jaddonisio
// Fix index from going out of bounds.
//
// Revision 1.19  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.18  2001/12/07 00:08:27  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.17  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.16  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.15  2001/11/30 23:50:44  jaddonisio
// i. Removed seed points member variable.
// ii. Removed 'AddRegToSegVolume' member function.
// iii. Removed 'DelRegToSegVolume' member function.
//
// Revision 1.14  2001/11/21 16:57:35  jaddonisio
// For efficiency, creating new mask tag volume instead of updating each voxels in the block volume.
//
// Revision 1.13  2001/11/19 01:04:01  jaddonisio
// Adjust array when component is deleted.
//
// Revision 1.12  2001/11/18 00:44:10  jaddonisio
// New private method to clear deleted region from internal mask tag volume.
//
// Revision 1.11  2001/11/16 19:59:55  jaddonisio
// Removed dead code.
//
// Revision 1.10  2001/11/16 00:26:33  jaddonisio
// New vector of RegionTagVolumes.
//
// Revision 1.9  2001/11/11 05:28:49  jaddonisio
// New method to remove from seg. volume.
//
// Revision 1.8  2001/11/11 02:20:28  jaddonisio
// Record seed points of components.
//
// Revision 1.7  2001/11/08 19:30:18  jaddonisio
// i. New method to set default colors.
// ii. Fixed append region logic.
//
// Revision 1.6  2001/11/08 14:38:28  jaddonisio
// Commented out new color coding in constructor.
//
// Revision 1.5  2001/11/08 02:58:13  ingmar
// now initializes 256 different colors for up to 256 components
//
// Revision 1.4  2001/11/05 16:51:59  jaddonisio
// Temporarily setting Lock on component.
//
// Revision 1.3  2001/11/02 15:33:16  soeren
// removed stl warnings
//
// Revision 1.2  2001/01/13 09:27:16  jaddonisio
// First integration of Interactive Segmentation into v3D Explorer.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/ComponentHandler.C,v 1.6 2006/03/29 20:01:25 geconomos Exp $
// $Id: ComponentHandler.C,v 1.6 2006/03/29 20:01:25 geconomos Exp $
