// $Id: vxComponentArray.h,v 1.9 2007/03/22 01:07:37 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (mailto:jaddonisio@viatronix.com)

/**
 * Class that holds a collection of vxComponent class. It is strictly based on
 * the creationb history of components, not on their actual label index. The
 * label index can also be obtained but most access functions are creation
 * history based, so watch out!!!
 */

#ifndef vxComponentArray_h
#define vxComponentArray_h


// includes
#include <vector>
#include "vxComponent.h"
#include <map>


// class definition
class VX_VIEWER_DLL vxComponentArray 
{
// functions
public:

  /// default constructor
  vxComponentArray();

  /// copy constructor
  vxComponentArray(const vxComponentArray & other);

  /// constructor
  vxComponentArray(std::vector<vxComponent*> vIndexedComponents, 
                   std::map<uint2, uint2> mapIndexToHistory);

  /// destructor
  virtual ~vxComponentArray();

  /// initialize from vector of components and map
  void InitializeFromComponents( std::vector< vxComponent * > vIndexedComponents,
                                 std::map< uint2, uint2 > mapIndexToHistory );

  /// get selected component index
  inline int4 GetSelectedComponentIndex() const { return m_iSelectedComponentIndex; }

  /// set selected component index
  void SetSelectedComponentIndex(const int4 iIndex);

  /// assignment operator
  const vxComponentArray & operator=(const vxComponentArray & other);

  /// function to add a component to the list and returns new history index
  uint2 AddComponent(const std::string & sName, const PixelRGBA<uint1> & color);

  /// function to remove a component given by its history index from the list
  void RemoveComponent(const uint2 & uHistoryIndex);

  /// compact array of components(i.e., remove "holes"/extra 'None')
  void Compact();

  /// return number of components (from history vector -> without "holes")
  inline uint4 GetCount() const { return m_vpComponentsByHistory.size(); }

  /// operator [], to access array elements (index of generation history)
  inline vxComponent * operator[](const uint4 & uIndex) { return m_vpComponentsByHistory[uIndex]; }

  /// returns if there are any visible labels
  bool AreAnyLabelsVisible() const;

  /// returns if there are any densities that are hidden
  bool AreAnyIntensitiesInvisible() const;
  
  /// output the state of the object to a string
  std::string ToString() const;

  //////////////////////////////////////////////////
  //
  // Helper functions to enable faster rendering
  // by having a label vector that can be indexed
  //

      /// return maximum index used so far
      uint2 GetMaxLabel() const;

      /// function to get a vector of vxComponents, indexed by label
      inline const std::vector<vxComponent *> & ArrayByLabel() const { return m_vpComponentsByLabel; }

      /// return the history index from tha label index
      uint2 GetHistoryFromLabel(uint2 uIndex);

      /// return the maps from label index to history index
      inline std::map<uint2,uint2> GetMapLabelToHistory() const { return m_mapLabelToHistory; }

  //
  //////////////////////////////////////////////////

private:

  /// initialize with 'NONE' component only
  void CommonInit();

  /// make a deep copy
  void DeepCopy(const vxComponentArray & other);

// data
private:

  /// selected component index
  int4 m_iSelectedComponentIndex;

  /// vector of vxComponents, in order of creation!!!
  std::vector<vxComponent *> m_vpComponentsByHistory;         
  
  /// vector of vxComponents, used for indexing by label
  std::vector<vxComponent *> m_vpComponentsByLabel;
  
  /// map from label to history
  std::map<uint2, uint2> m_mapLabelToHistory;
}; // end class vxComponentArray


#endif // vxComponentArray_h


// $Log: vxComponentArray.h,v $
// Revision 1.9  2007/03/22 01:07:37  geconomos
// removed incorrect "inline" fom GetSelectedIndex
//
// Revision 1.8  2006/11/09 21:50:23  romy
// moved  GetComponentIndex method to componentUtils
//
// Revision 1.7  2006/11/09 19:50:53  romy
// added GetComponentIndex method
//
// Revision 1.6  2006/09/21 14:53:37  geconomos
// made AreAnyLabelsVisible() and AreAnyIntensitiesInvisible() const member funstions
//
// Revision 1.5  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.4  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.6.1  2005/02/15 21:15:56  frank
// removed unused definition
//
// Revision 1.3  2004/07/12 18:02:40  frank
// added component data
//
// Revision 1.2  2004/04/14 13:59:21  frank
// implemented To String function
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/01/29 17:00:43  frank
// code review
//
// Revision 1.21  2004/01/05 18:58:13  michael
// added "mailto:"
//
// Revision 1.20  2003/06/04 19:00:35  jmeade
// code review.
//
// Revision 1.19  2003/06/04 18:46:30  jmeade
// code review.
//
// Revision 1.18  2003/04/28 16:59:25  michael
// fixed issue #3116 and 3065
//
// Revision 1.17  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.16  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.15  2003/04/09 21:40:02  michael
// fixed compacting of component array
//
// Revision 1.14  2003/04/09 15:05:31  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.13  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.12  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.11  2003/03/10 15:00:28  geconomos
// Added EVENT_NAME_MODIFIED flag.
//
// Revision 1.10  2002/11/26 13:40:38  michael
// renamed method (GetHistoryFromLabel()), fixed Compact() and bug in creation from map and vector
//
// Revision 1.9  2002/11/14 20:08:51  michael
// added DeepCopy to vxComponentArray and removed redundant code
//
// Revision 1.8  2002/11/14 01:21:32  michael
// further cleanup, added constructor from vector and map as well as operator=
//
// Revision 1.7  2002/11/13 01:54:02  michael
// further cleanup
//
// Revision 1.6  2002/11/13 00:36:50  michael
// first step towards a cleanup of the components(table)
//
// Revision 1.4  2002/10/31 12:56:56  geconomos
// Added EVENT_SELECTED .
//
// Revision 1.3  2002/09/27 19:30:58  jaddonisio
// Comments.
//
// Revision 1.2  2002/09/11 15:45:11  jaddonisio
// New map to get component index from component label.
//
// Revision 1.1  2002/09/05 23:16:27  jaddonisio
// Initial version: New vxComponent.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxComponentArray.h,v 1.9 2007/03/22 01:07:37 geconomos Exp $
// $Id: vxComponentArray.h,v 1.9 2007/03/22 01:07:37 geconomos Exp $
