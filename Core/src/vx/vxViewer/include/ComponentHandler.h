// $Id: ComponentHandler.h,v 1.6 2006/03/29 20:03:19 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/*
  Class that encapsulates the Component Handler. This class provides
  methods to handle mask tag and region tag volumes associated with
  components.
*/

// pragmas
#pragma once


// includes
#include "SegmentationParameters.h"
#include "PixelRGBA.h"
#include "SegmentationVolumes.h"
#include "vxComponentArray.h"
#include "vxShareableObject.h"


// forward declarations
class SegmentationVolumes;
template <class T> class vxBlockVolume;


// defines
#define MAXN_COMP (4096)


// class definition
class VX_VIEWER_DLL ComponentHandler
{
// functions
public:

  /// default constructor
  ComponentHandler();

  /// destructor
  virtual ~ComponentHandler();

public:
  
  /// get the numer of components
  inline uint2 GetComponentCount() const;

  /// get selected component index
  inline uint2 GetSelectedComponentIndex();

  /// set selected component index
  inline void SetSelectedComponentIndex(const uint2 & uIndex);

  /// returns if there are any visible labels
  inline bool AreAnyLabelsVisible();

  /// returns if there are any densities that are hidden
  inline bool AreAnyIntensitiesInvisible();

  /// set the name of a component
  inline void SetName(const uint2 & uLabelIndex, const std::string & sName);

  /// get the name of a component
  inline const std::string & GetName(const uint2 & uLabelIndex);

  /// set the lock flag for a component
  inline void SetLocked(const uint2 & uLabelIndex, const bool & bLock);
 
  /// get the lock flag for a component
  inline bool IsLocked(const uint2 & uLabelIndex);
  
  /// set the active flag for a component
  inline void SetIntensityVisible(const uint2 & uLabelIndex, const bool & bDensityVisible);

  /// get the active flag for a component
  inline const bool IsIntensityVisible(const uint2 & uLabelIndex);

  /// set the label visible flag for a component
  inline void SetLabelVisible(const uint2 & uLabelIndex, const bool & bLabelVisible);

  /// get the label visible flag for a component
  inline const bool IsLabelVisible(const uint2 & uLabelIndex);

  /// Set the color of a component
  inline void SetColor(const uint2 & uLabelIndex, const PixelRGBA<uint1> & color);

  /// get the color of a component
  inline const PixelRGBA<uint1> & GetColor(const uint2 & uLabelIndex); 

  /// get mask tag volume
  inline vxBlockVolume<bool> * GetMaskTagVolume();

  /// get region tag volumes
  inline std::vector< vxBlockVolume<bool> * > & GetRegTagVols(uint2 uCompLabel);

  /// set component pointer
  void SetComponentsPtr(vxShareableObject< vxComponentArray > * pvpComponent) { m_pvpComponents = pvpComponent; };

  /// get component pointer
  vxShareableObject< vxComponentArray > * GetComponentsPtr() { return m_pvpComponents; };

  /// set component pointer
  inline  SegmentationVolumes * GetSegVolPtr() { return &m_segVols; };

  /// get a pointer to a segmentation volume
  inline void SetSegmentVolume(vxBlockVolume<uint2> * pSegLabelVolume);

  /// initialization for component handler
  void Initialize();

  /// test for component segmented
  bool IsComponentSegmented(const uint4 uLabelIndex);

  /// delete all components
  void DeleteAllComps();

  /// delete Component
  void DeleteComp(const uint2 uHistoryIndex);

  /// update based on user inputs to Components
  void OnUserInputUpdate();

  /// modify mask tag volume based new value for region
  void ModifyMaskTagVol(vxBlockVolume<bool> * pRegTagVol, bool bVal);

  /// add Component Region.
  vxBlockVolume<bool> * AddCompReg(uint2 & uCompIndex, const SegParams::RegionAddOptionEnum eRegAddOpt);

  /// remove Component Region
  void RemCompReg(const uint2 & iIndexHistory);

  /// update component mask for locking/unlocking
  void UpdateCompMask(const uint2 & iIndexHistory);

  /// update region tag volumes
  void UpdateRegionTags();

private:

  /// create region tags
  void CreateRegionTagVols();

  /// update mask tag volume based on region tag
  void UpdateMaskTagVol(const vxBlockVolume<uint2> & rSegLblVol);

  /// bound a given (component) index to the valid range
  inline uint2 BoundIndex(const uint2 & uLabelIndex);

// data
private:

  /// id of the selected component
  uint2 m_uSelectedComponentIndex;

  /// vector of region tag volumes
  std::vector<vxBlockVolume<bool> *> m_vRegTagVols[MAXN_COMP];

  /// header for region tag volumes
  vxVolumeHeader m_regTagVolHdr;
  
  /// segmentation volume class
  SegmentationVolumes m_segVols;
  
  /// pointer to the mask tag volume of bool's
  vxBlockVolume<bool> m_maskTagVol;

  /// pointer to segmentation label volume - block volume of uint2
  vxBlockVolume<uint2> * m_pSegLblVol;
  
  /// default reference to segmentation volume - block volume of uint2
  vxBlockVolume<uint2> m_defaultSegLblVol;
  
  /// vector of pointers to components
  vxShareableObject< vxComponentArray > * m_pvpComponents;
  
  /// vector of pointers to components
  vxShareableObject< vxComponentArray > m_vpDefaultComponents;
}; // ComponentHandler


class ComponentColors
{
// member functions
public:

  /// get a color that is not already used
  static PixelRGBA<uint1> GetUniqueColor( vxComponentArray * pComponentArray );

private:

  /// set default colors for components
  static void SetDefaultColors();

// data members
private:

  /// vector of default colors
  static std::vector<PixelRGBA<uint1> > m_vDefaultColors;

}; // class ComponentColors


#include "ComponentHandler.inl"


// $Log: ComponentHandler.h,v $
// Revision 1.6  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.5  2006/02/28 15:23:50  frank
// switched over to shareable objects
//
// Revision 1.4  2006/02/17 15:07:24  frank
// split out component colors from the component handler
//
// Revision 1.3  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.2  2005/07/19 14:15:26  frank
// can't always use const
//
// Revision 1.2.2.1  2005/07/11 17:43:13  frank
// removed duplicate access function
//
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.2  2005/02/15 17:52:40  frank
// increased argument data size
//
// Revision 1.1.1.1.6.1  2005/02/15 17:10:33  frank
// increased maximum components to 4096
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.44  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.43  2003/06/07 15:35:47  michael
// reformatting
//
// Revision 1.42  2003/05/27 13:18:05  frank
// Removed dead includes
//
// Revision 1.41  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.40  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.39  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.38  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.37  2003/04/08 16:59:29  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.36  2003/04/04 12:57:54  frank
// Exported the class to the DLL interface
//
// Revision 1.35  2003/03/26 03:30:20  michael
// formatting
//
// Revision 1.34  2002/12/12 17:07:38  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.33  2002/11/14 01:21:51  michael
// took out redundant code
//
// Revision 1.32  2002/10/17 22:16:45  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.31  2002/10/04 12:52:20  geconomos
// Exported segmentation classes.
//
// Revision 1.30  2002/09/05 22:48:11  jaddonisio
// New vxComponent.
//
// Revision 1.29  2002/08/20 15:10:26  jaddonisio
// New function to calculate mask based on block04(used for new region grow iterator).
//
// Revision 1.28  2002/06/11 20:49:28  jaddonisio
// Create region tags when initialize component handler.
//
// Revision 1.27  2002/06/10 18:24:53  jaddonisio
// New updates for component mask.
//
// Revision 1.26  2002/06/06 19:14:52  manju
// Added method Initialize.
//
// Revision 1.25  2002/06/03 21:30:53  jaddonisio
// Coding standard changes.
//
// Revision 1.24  2002/05/28 15:37:11  jaddonisio
// New method to test if component is segmented.
//
// Revision 1.23  2002/05/03 22:25:47  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.22  2002/05/02 22:25:35  jaddonisio
// Clean up(related to old block volume).
//
// Revision 1.21  2002/05/01 19:50:54  jaddonisio
// VolumeSet/AnyVolume no longer used.
//
// Revision 1.20  2002/05/01 13:57:41  jaddonisio
// Removed AnyVolume references.
//
// Revision 1.19  2002/04/30 20:03:47  jaddonisio
// No more VolumeSet.
//
// Revision 1.18  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.17  2002/04/23 11:58:39  geconomos
// Changed default name for new component.
//
// Revision 1.16  2002/04/18 16:21:03  jaddonisio
// i. New methods to set and get properties of components.
// ii. Contains an instance of the SegmentationVolumes class.
//
// Revision 1.15  2002/04/10 17:12:25  jaddonisio
// New comp string.
//
// Revision 1.14  2002/04/09 15:28:11  geconomos
// Changed the name of a newly added component.
//
// Revision 1.13  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.12  2001/12/07 00:08:27  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.11  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.10  2001/12/05 23:26:05  jaddonisio
// Mod. enum names in SegParams to comply with coding standard.
//
// Revision 1.9  2001/11/30 23:50:31  jaddonisio
// i. Removed seed points member variable.
// ii. Removed 'AddRegToSegVolume' member function.
// iii. Removed 'DelRegToSegVolume' member function.
//
// Revision 1.8  2001/11/18 00:44:10  jaddonisio
// New private method to clear deleted region from internal mask tag volume.
//
// Revision 1.7  2001/11/16 00:26:21  jaddonisio
// New vector of RegionTagVolumes.
//
// Revision 1.6  2001/11/11 05:28:49  jaddonisio
// New method to remove from seg. volume.
//
// Revision 1.5  2001/11/11 02:21:24  jaddonisio
// Record seed points of components.
//
// Revision 1.4  2001/11/09 21:53:23  jaddonisio
// Get selected component.
//
// Revision 1.3  2001/11/08 19:30:18  jaddonisio
// i. New method to set default colors.
// ii. Fixed append region logic.
//
// Revision 1.2  2001/01/13 09:27:16  jaddonisio
// First integration of Interactive Segmentation into v3D Explorer.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/ComponentHandler.h,v 1.6 2006/03/29 20:03:19 geconomos Exp $
// $Id: ComponentHandler.h,v 1.6 2006/03/29 20:03:19 geconomos Exp $
