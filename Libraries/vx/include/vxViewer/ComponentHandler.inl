// $Id: ComponentHandler.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)


// includes
#include "SegmentationVolumes.h"
#include "vxBlockVolume.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Get selected component index
 * @return index
 */
inline uint2 ComponentHandler::GetSelectedComponentIndex()
//*******************************************************************
{ 
  return m_pvpComponents->GetSelectedComponentIndex();
}


/**
 * Set selected component index
 * @param index
 */
inline void ComponentHandler::SetSelectedComponentIndex(const uint2 & uIndex)
//*******************************************************************
{ 
  m_pvpComponents->SetSelectedComponentIndex(uIndex);
}


/**
 * Public method to get component count
 * @return number of components
 */
inline uint2 ComponentHandler::GetComponentCount() const
//*******************************************************************
{
  return(Bound((uint2)0, (uint2)m_pvpComponents->GetCount(), (uint2)MAXN_COMP));
} // GetComponentCount()


/**
 * Get method for mask tag volume
 * @return Pointer to a mask tag volume
 */
inline vxBlockVolume<bool> * ComponentHandler::GetMaskTagVolume()
//*******************************************************************
{
  return(&m_maskTagVol);
} // GetMaskTagVol()


/**
 * Public method to set name of a component.
 * @param value of label index
 * @param name as string
 */
inline void ComponentHandler::SetName(const uint2 & uLabelIndex, 
                                      const std::string & sName)
//*******************************************************************
{
  if (BoundIndex(uLabelIndex) == uLabelIndex)
  {
    (*m_pvpComponents)[uLabelIndex]->SetName(sName);
  }
} // SetName()


/**
 * Public method to get name of a component
 * @param value of label index
 * @return name as string
 */
inline const std::string & ComponentHandler::GetName(const uint2 & uLabelIndex)
//*******************************************************************
{
  return (*m_pvpComponents)[BoundIndex(uLabelIndex)]->GetName();
} // GetName()


/**
 * Public method to lock/unlock a component
 * @param uLabelIndex value of label index
 * @param bLock lock flag
 */
inline void ComponentHandler::SetLocked(const uint2 & uLabelIndex, 
                                        const bool & bLock)
//*******************************************************************
{
  if (BoundIndex(uLabelIndex) == uLabelIndex)
  {
    (*m_pvpComponents)[uLabelIndex]->SetLocked(bLock);
  }
} // SetLocked()


/**
 * Public method to get the locked flag for a component
 * @param uLabelIndex value of label index
 * @return Lock flag
 */
inline bool ComponentHandler::IsLocked(const uint2 & uLabelIndex)
//*******************************************************************
{
  return (*m_pvpComponents)[BoundIndex(uLabelIndex)]->IsLocked();
} // IsLocked()


/**
 * Public method to set/unset label visible.
 * @param uLabelIndex value of label index
 * @param bIntensityVisible density visible flag
 */
inline void ComponentHandler::SetIntensityVisible(const uint2 & uLabelIndex, 
                                                  const bool & bIntensityVisible)
//*******************************************************************
{
  if (BoundIndex(uLabelIndex) == uLabelIndex)
  {
    (*m_pvpComponents)[uLabelIndex]->SetIntensityVisible(bIntensityVisible);
  }
} // SetIntensityVisible()


/**
 * Public method to get density visible flag
 * @param uLabelIndex value of label index
 * @return Intensity visible flag
 */
inline const bool ComponentHandler::IsIntensityVisible(const uint2 & uLabelIndex)
//*******************************************************************
{
  return (*m_pvpComponents)[BoundIndex(uLabelIndex)]->IsIntensityVisible();
} // IsIntensityVisible()


/**
 * Public method to set/unset label visible flag
 * @param uLabelIndex value of label index
 * @param bLabelVisible label visible flag
 */
inline void ComponentHandler::SetLabelVisible(const uint2 & uLabelIndex, 
                                              const bool & bLabelVisible)
//*******************************************************************
{
  if (BoundIndex(uLabelIndex) == uLabelIndex)
  {
    (*m_pvpComponents)[uLabelIndex]->SetLabelVisible(bLabelVisible);
  }
} // SetLabelVisible


/**
 * Public method to get label visible
 * @param uLabelIndex value of label index
 * @return Label visible flag.
 */
inline const bool ComponentHandler::IsLabelVisible(const uint2 & uLabelIndex)
//*******************************************************************
{
  return (*m_pvpComponents)[BoundIndex(uLabelIndex)]->IsLabelVisible();
} // IsLabelVisible()


/**
 * Set label color
 * @param uLabelIndex value of label index
 * @param color RGBA value of label color.
 */
inline void ComponentHandler::SetColor(const uint2 & uLabelIndex, 
                                       const PixelRGBA<uint1> & color) 
//*******************************************************************
{
  if (BoundIndex(uLabelIndex) == uLabelIndex)
  {
    (*m_pvpComponents)[uLabelIndex]->SetColor(color); 
  }
} // SetColor()


/**
 * Public method to get label color
 * @param uLabelIndex value of label index
 * @return Label color
 */
inline const PixelRGBA<uint1> & ComponentHandler::GetColor(const uint2 & uLabelIndex)
//*******************************************************************
{
  return (*m_pvpComponents)[BoundIndex(uLabelIndex)]->GetColor(); 
} // GetColor()


/**
 * Get method for vector of region tag volumes
 * @param uCompLabel component label
 * @return Vector of region tag volumes
 */
inline std::vector<vxBlockVolume<bool> *> & ComponentHandler::GetRegTagVols(uint2 uCompLabel)
//*******************************************************************
{
  if (uCompLabel >= MAXN_COMP)
  {
    uCompLabel = (MAXN_COMP-1);
  }

  return(m_vRegTagVols[uCompLabel]);
} // GetRegTagVols()


/**
 * Public method to set pointer to a segmentation volume
 * @param pAnyVolume pointer to any volume
 */
inline void ComponentHandler::SetSegmentVolume(vxBlockVolume<uint2> * pSegLabelVolume)
//*******************************************************************
{
  try
  {
    if (pSegLabelVolume == NULL)
    {
      throw ex::NullPointerException(LogRec("Segmentation volume pointer is null", "ComponentHandler", "SetSegmentVolume"));
    }

    // Set the segmentation volume:
    m_pSegLblVol = pSegLabelVolume;
    m_segVols.SetSegLabelVol(m_pSegLblVol);

    // Set the associated mask tag volume:
    m_maskTagVol.Clear();
    m_maskTagVol.GetHeader().SetVolDim(m_pSegLblVol->GetHeader().GetVolDim());
    m_maskTagVol.GetHeader().SetVoxelUnits(m_pSegLblVol->GetHeader().GetVoxelUnits());

    // Set region tag volume header:
    m_regTagVolHdr.SetVolDim(m_pSegLblVol->GetHeader().GetVolDim());
    m_regTagVolHdr.SetVoxelUnits(m_pSegLblVol->GetHeader().GetVoxelUnits());
  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("SetSegmentVolume failed", "ComponentHandler", "SetSegmentVolume"));
  }
}


/**
 * Private method to bound index into component vector
 * @param uLabelIndex Index of component label.
 * @return Bounded index.
 */
inline uint2 ComponentHandler::BoundIndex(const uint2 & uLabelIndex)
//*******************************************************************
{
  uint2 uTmp(0);
  if (0 < uLabelIndex && uLabelIndex < m_pvpComponents->GetCount())
  {
    if ((*m_pvpComponents)[uLabelIndex] != NULL)
    {
      uTmp = uLabelIndex;
    }
  }

  return uTmp;
} // BoundIndex()


#undef FILE_REVISION


// Revision History:
// $Log: ComponentHandler.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.31  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.30  2003/06/07 15:35:47  michael
// reformatting
//
// Revision 1.29  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.28  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.27  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.26  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.25  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.24  2003/01/22 21:51:48  ingmar
// dimension and units are now Triples
//
// Revision 1.23  2002/11/13 01:54:13  michael
// further cleanup
//
// Revision 1.22  2002/11/13 00:11:46  michael
// account for changes in vxComponent and vxComponentArray
//
// Revision 1.21  2002/09/11 15:45:45  jaddonisio
// Bound component count.
//
// Revision 1.20  2002/09/05 22:48:11  jaddonisio
// New vxComponent.
//
// Revision 1.19  2002/06/18 16:06:03  soeren
// renamed boom to a real error message
//
// Revision 1.18  2002/06/10 18:23:05  jaddonisio
// Initialize new component count.
//
// Revision 1.17  2002/06/04 22:50:50  ingmar
// added additional exceptions and cleaned up throw statement syntax
//
// Revision 1.16  2002/06/03 21:30:47  jaddonisio
// Coding standard changes.
//
// Revision 1.15  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.14  2002/05/01 19:50:54  jaddonisio
// VolumeSet/AnyVolume no longer used.
//
// Revision 1.13  2002/05/01 13:57:41  jaddonisio
// Removed AnyVolume references.
//
// Revision 1.12  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.11  2002/04/18 16:21:03  jaddonisio
// i. New methods to set and get properties of components.
// ii. Contains an instance of the SegmentationVolumes class.
//
// Revision 1.10  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.9  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.8  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.7  2001/11/30 23:48:19  jaddonisio
// Removed 'GetSeedPoints' method.
//
// Revision 1.6  2001/11/16 20:00:34  jaddonisio
// Removed call to update mask volume for speedup.
//
// Revision 1.5  2001/11/16 00:26:04  jaddonisio
// New vector of RegionTagVolumes.
//
// Revision 1.4  2001/11/11 02:21:51  jaddonisio
// Get method for seed points of components.
//
// Revision 1.3  2001/11/09 21:53:23  jaddonisio
// Get selected component.
//
// Revision 1.2  2001/01/13 09:27:16  jaddonisio
// First integration of Interactive Segmentation into v3D Explorer.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/ComponentHandler.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: ComponentHandler.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
