// $Id: vxComponentArray.C,v 1.18 2008/04/24 21:55:34 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


/**
 * In case someone else than me (michael@viatronix.com) needs to
 * look at this, too, here is some description of what is going on:
 * This class encapsulates the vector of vxComponent which can be
 * used to index into it with the label index (e.g. rendering) but
 * it also provides a chronological history in which order these
 * components were created (second vector). In case a component is 
 * removed, the 'NONE' component is inserted into the indexable 
 * vector such that there are no NULL-pointers that could cause a 
 * crash during run time.
 *
 * The current implementation has redundancy (one vector would be 
 * enough) and this should be addressed later on.
 */


// includes
#include "stdafx.h"
#include "vxComponentArray.h"
#include "utilities.h"
#include "vxComponentData.h"

// defines
#define FILE_REVISION "$Revision: 1.18 $"


/**
 * Default constructor for vxComponentArray class
 */
vxComponentArray::vxComponentArray()
{
  m_vpComponentsByHistory.reserve(256);
  m_vpComponentsByLabel.reserve(256);

  CommonInit(); // 'Background' component is label = 0.
} // constructor


/**
 * Copy Constructor
 *
 * @param other   component array
 */
vxComponentArray::vxComponentArray(const vxComponentArray & other)
{
  if (this != &other)
  {
    DeepCopy(other);
  }
} // copy constructor

  
/**
 * Constructor for creating object from objects
 *
 * @param vIndexedComponents   vector of components
 * @param mapLabelToHistory    label to index map
 */
vxComponentArray::vxComponentArray(std::vector<vxComponent*> vIndexedComponents, 
                                   std::map<uint2, uint2> mapLabelToHistory)
{

  m_vpComponentsByHistory.reserve( 256 );
  m_vpComponentsByLabel.reserve( 256 );

  CommonInit();

  InitializeFromComponents( vIndexedComponents, mapLabelToHistory );

} // constructor


/**
 * Copy Constructor
 */
const vxComponentArray & vxComponentArray::operator=(const vxComponentArray & other)
{
  if (this != &other)
  {
    DeepCopy(other);
  }
  return *this;
}


/**
 * Destructor for vxComponentArray class
 */
vxComponentArray::~vxComponentArray()
{
	//TODO: Fix this.This was causing error in Lung Viewer closing after an Undo
	int4 TODO__FIX_THIS_AFTER_RSNA_2006;

//   De-allocate the actual objects
  std::vector<vxComponent *>::iterator iter;

  for ( iter = m_vpComponentsByHistory.begin(); iter != m_vpComponentsByHistory.end(); ++iter )
  {
    vxComponent * pComponent( * iter );
    if ( pComponent != NULL )
    {
      if ( pComponent->m_pComponentData != NULL )
      {
        delete pComponent->m_pComponentData;
        pComponent->m_pComponentData = NULL;
      }
      delete pComponent;
      pComponent = NULL;
    }
  }
} // destructor



/**
 * initialize from vector of components and map
 *
 * @param vIndexedComponents   vector of components
 * @param mapLabelToHistory    label to index map
 */
void vxComponentArray::InitializeFromComponents( std::vector< vxComponent * > vIndexedComponents,
                                                 std::map< uint2, uint2 > mapLabelToHistory )
{

  // destruct the current components
  for ( std::vector<vxComponent *>::iterator iter( m_vpComponentsByHistory.begin() );
        iter != m_vpComponentsByHistory.end();
        ++iter )
  {
    vxComponent * pComponent( * iter );
    delete pComponent;
    pComponent = NULL;
  }

  int4 iNumberOfComponents(mapLabelToHistory.size());

  if (iNumberOfComponents == 0)
  {
    CommonInit();
    return;
  }

  m_vpComponentsByHistory.clear();
  m_vpComponentsByLabel.clear();
  m_mapLabelToHistory.clear();

  m_vpComponentsByHistory.resize(1);
  m_vpComponentsByLabel.resize(1);

  m_mapLabelToHistory = mapLabelToHistory;

  // initialize the 'Background' component
  vxComponent * pComponent;
  pComponent = new vxComponent;
  *pComponent = *vIndexedComponents[0];
  m_vpComponentsByLabel[0]   = pComponent;
  m_vpComponentsByHistory[0] = pComponent;
  m_iSelectedComponentIndex = 0;

  if (iNumberOfComponents == 1)
    return;

  // create vectors and map
  ///////////////////////////
  std::vector<vxComponent*>::iterator iterObj;
  int4 i(1), j(1);

  //TODO: Fix me (do not assume the 2 vectors are the same size)
  m_vpComponentsByHistory.resize(iNumberOfComponents);

  for (iterObj=vIndexedComponents.begin()+1, i=1; iterObj != vIndexedComponents.end(); ++iterObj, ++i)
  {
    if ((**iterObj).GetLabel() == 0) // 'hole' identified by the 'Background' component
    {
      LogErr( "******Duplicate background label**********", "", "" );
      m_vpComponentsByLabel.push_back(m_vpComponentsByLabel[0]);
    }
    else
    {
      pComponent  = new vxComponent;
      *pComponent = **iterObj;
      m_vpComponentsByLabel.push_back(pComponent);

      const uint2 uHistoryIndex(GetHistoryFromLabel(i));
      m_vpComponentsByHistory[uHistoryIndex] = pComponent;
    }
  } // for all indices, except Background

} // InitializeFromComponents()


/**
 * DeepCopy
 *
 * @param other   component array
 */
void vxComponentArray::DeepCopy(const vxComponentArray & other)
{
  m_iSelectedComponentIndex = other.m_iSelectedComponentIndex;

  // first: clean-up what is here
  m_mapLabelToHistory.clear();
  m_vpComponentsByLabel.clear();
  std::vector<vxComponent *>::iterator iter;
  for (iter=m_vpComponentsByHistory.begin(); iter!=m_vpComponentsByHistory.end(); ++iter)
    if (*iter != NULL)
      delete *iter;
  m_vpComponentsByHistory.clear();

  // second: copy map (easy)
  m_mapLabelToHistory = other.m_mapLabelToHistory;

  // third: reserve a few entries
  m_vpComponentsByHistory.reserve(256);
  m_vpComponentsByLabel.reserve(256);

  // fourth: make a deep copy of ptr components of the history vector
  vxComponent * pComponent;
  std::vector<vxComponent *>::const_iterator iterOther;
  for (iterOther=other.m_vpComponentsByHistory.begin(); iterOther!=other.m_vpComponentsByHistory.end(); ++iterOther)
  {
    pComponent = new vxComponent;
    *pComponent = **iterOther;
    m_vpComponentsByHistory.push_back(pComponent);
  }

  // fifth: make pointers in index vector to point to objects in history vector
  m_vpComponentsByLabel = other.m_vpComponentsByLabel;
  int4 i;
  for (i=0, iter=m_vpComponentsByLabel.begin(); iter!=m_vpComponentsByLabel.end(); ++iter, ++i)
  {
    int4 iIndex = m_mapLabelToHistory[i];
    *iter = m_vpComponentsByHistory[m_mapLabelToHistory[i]];
  }
} // DeepCopy()


/**
 * CommonInit
 * clears the vectors and initializes them for 'Background' component
 */
void vxComponentArray::CommonInit()
{
  // clear vectors
  m_vpComponentsByHistory.clear();
  m_vpComponentsByLabel.clear();
  m_mapLabelToHistory.clear();

  // initialize with 'Background' component
  m_vpComponentsByHistory.resize(1);
  m_vpComponentsByLabel.resize(1);
  m_vpComponentsByHistory[0] = new vxComponent(0, "Background", PixelRGBA<uint1>(0,0,0,0), true, false, false, false);
  m_vpComponentsByHistory[0]->SetNameEditable( false );
  m_vpComponentsByHistory[0]->SetRemovable(  false );
  m_vpComponentsByHistory[0]->SetGrowable( false );
  m_vpComponentsByHistory[0]->SetShrinkable( false );
  m_vpComponentsByLabel[0] = m_vpComponentsByHistory[0];
  m_mapLabelToHistory[0] = 0;
  m_iSelectedComponentIndex = 0;
} // CommonInit()


/**
 * Set selected component index
 *
 * @param index   component index to select
 */
void vxComponentArray::SetSelectedComponentIndex(const int4 iIndex)
{
  if (iIndex < 0 || iIndex >= m_vpComponentsByHistory.size())
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds: " + ToAscii(iIndex), "vxComponentArray", "SetSelectedComponentIndex()"));
  }
  else
    m_iSelectedComponentIndex = iIndex; 
}


/**
 * GetMaxIndex
 *
 * @return maximum index
 */
uint2 vxComponentArray::GetMaxLabel() const
{
  int4 iMaxIndex(0);

  for (int4 iIndex(0); iIndex<m_vpComponentsByLabel.size(); ++iIndex)
    if (m_vpComponentsByLabel[iIndex] != NULL)
      if (m_vpComponentsByLabel[iIndex]->GetLabel() > iMaxIndex)
        iMaxIndex = m_vpComponentsByLabel[iIndex]->GetLabel();

  return iMaxIndex;
} // GetMaxLabel()


/**
 * Public function to add a component to the array
 *
 * @param sName   component name 
 * @param color   component color
 * @return        new component's label index
 */
uint2 vxComponentArray::AddComponent(const std::string & sName, 
                                     const PixelRGBA<uint1> & color)
{
  // Determine label index for new component(i.e.,search for first not 'Background' position):
  uint2 uIndex(1); // 'Background' is excluded
  while (uIndex<m_vpComponentsByLabel.size() && m_vpComponentsByLabel[uIndex]->GetLabel() != 0) 
  { 
    ++uIndex;
  }
  if (uIndex == m_vpComponentsByLabel.size())
  {
    m_vpComponentsByLabel.resize(uIndex+1);
  }

  // create new component, put it in index and history vector, and update map
  vxComponent * pNewComponent = new vxComponent(uIndex, sName, color, true, true, true, false);
  m_vpComponentsByLabel[uIndex] = pNewComponent;
  m_vpComponentsByHistory.push_back(pNewComponent);
  m_mapLabelToHistory[uIndex] = m_vpComponentsByHistory.size()-1;

  return m_vpComponentsByHistory.size()-1;
} // AddComponent()


/**
 * Public function to remove a component by history index
 *
 * @param uIndex   index into array for component to be removed
 */
void vxComponentArray::RemoveComponent(const uint2 & uIndex) 
{
  if (uIndex < m_vpComponentsByHistory.size())
  {
    // remove the component
    m_mapLabelToHistory.erase(m_vpComponentsByHistory[uIndex]->GetLabel());
    m_vpComponentsByLabel[m_vpComponentsByHistory[uIndex]->GetLabel()] = m_vpComponentsByHistory[0];
    // remove the component data, if any
	  delete m_vpComponentsByHistory[uIndex];
    m_vpComponentsByHistory.erase(m_vpComponentsByHistory.begin()+uIndex);

    // potentially fix the selected index
    if ( m_iSelectedComponentIndex == uIndex )
    {
      m_iSelectedComponentIndex = m_vpComponentsByHistory.size() - 1;
    }
    else if ( m_iSelectedComponentIndex > uIndex )
    {
      m_iSelectedComponentIndex--;
    }

    std::map<uint2, uint2>::iterator iter;

    for (iter = m_mapLabelToHistory.begin(); iter != m_mapLabelToHistory.end(); iter++)
    {
      if (iter->second > uIndex)
        iter->second = iter->second-1;
    }
    for (iter = m_mapLabelToHistory.begin(); iter != m_mapLabelToHistory.end(); iter++)
    {
      uint2 f, s;
      f = iter->first;
      s = iter->second;
      //LogDbg("map: " + ToAscii(f) + ", " + ToAscii(s), "", "");
    }
  } // if valid index
  else
  {
    LogDbg("Trying to remove a component that is out of bounds: " + ToAscii(uIndex) + " out of " + ToAscii(m_vpComponentsByHistory.size()),
           "vxComponentArray", "RemoveComponent");
  }
} // RemoveComponent()


/**
 * Compact
 * Cleans the vectors so that the m_vpComponentsByLabel has no "holes"
 */
void vxComponentArray::Compact()
{
  std::vector<vxComponent *> vpComponentsByLabel;
  std::map<uint2, uint2> mapLabelToHistory;

  // Background
  vpComponentsByLabel.resize(m_vpComponentsByHistory.size());
  vpComponentsByLabel[0] = m_vpComponentsByHistory[0];
  mapLabelToHistory[0] = 0;

  // all other components
  std::map<uint2, uint2>::iterator iter;
  for (int4 i(1), j(1); i<m_vpComponentsByLabel.size(); i++)
  {
    if (m_vpComponentsByLabel[i] != m_vpComponentsByLabel[0])
    {
      vpComponentsByLabel[j] = m_vpComponentsByLabel[i];
      vpComponentsByLabel[j]->SetLabel(j);

      iter = m_mapLabelToHistory.find(i);
      mapLabelToHistory[j] = iter->second;

      j++;
    }
  } // for all components, except Background

  m_vpComponentsByLabel = vpComponentsByLabel;
  m_mapLabelToHistory = mapLabelToHistory;
} // Compact()


/**
 * Returns the (history)index of given (label)index
 *
 * @param uIndexLabel   index of label
 * @return              index of history
 */
uint2 vxComponentArray::GetHistoryFromLabel(uint2 uIndexLabel)
{
  uint2 uIndexHistory(0);
  std::map<uint2, uint2>::iterator iter(m_mapLabelToHistory.find(uIndexLabel));

  if (iter != m_mapLabelToHistory.end())
  {
    uIndexHistory = iter->second;
  }

  return uIndexHistory;
} // GetHistoryFromLabel()


/**
 * Returns if there are any visible labels
 *
 * @return    Flag to indicate existence of any visible labels
 */
bool vxComponentArray::AreAnyLabelsVisible() const
{
  std::vector<vxComponent *>::const_iterator iter;

  for (iter = m_vpComponentsByHistory.begin(); iter != m_vpComponentsByHistory.end(); ++iter)
    if (*iter != NULL)
      if ((*iter)->IsLabelVisible() == true)
        return true; // early return for performance

  return false;
} // AreAnyLabelsVisible()


/**
 * Returns if there are any densities that are hidden
 *
 * @return    Flag to indicate existence of any hidden densities
 */
bool vxComponentArray::AreAnyIntensitiesInvisible() const
{
  std::vector<vxComponent *>::const_iterator iter;

  for (iter = m_vpComponentsByHistory.begin(); iter != m_vpComponentsByHistory.end(); ++iter)
    if (*iter != NULL)
      if ((*iter)->IsIntensityVisible() == false)
        return true; // early return for performance

  return false;
} // AreAnyDensitiesInvisible()


/**
 * output the state of the object to a string
 *
 * @return as a string
 */
std::string vxComponentArray::ToString() const
{
  
  std::string output( "vxComponentArray by history:\n" );

  for ( std::vector< vxComponent * >::const_iterator iterHistory( m_vpComponentsByHistory.begin() );
        iterHistory != m_vpComponentsByHistory.end();
        ++iterHistory )
  {
    output += "  " + ( * iterHistory )->ToString() + "\n";
  }


  output += "\nvxComponentArray by label:\n";

  for ( std::vector< vxComponent * >::const_iterator iterLabel( m_vpComponentsByLabel.begin() );
    iterLabel != m_vpComponentsByLabel.end();
    ++iterLabel )
  {
    output += "  " + ( * iterLabel )->ToString() + "\n";
  }

  return output;
  
} // ToString()


// undefines
#undef FILE_REVISION


// $Log: vxComponentArray.C,v $
// Revision 1.18  2008/04/24 21:55:34  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.17  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.16  2007/05/02 16:14:15  gdavidson
// Added reminder to fix InitializeFromComponents at a later time
//
// Revision 1.15.2.2  2008/01/03 04:27:37  jmeade
// comments and standards.
//
// Revision 1.15.2.1  2007/04/27 22:34:57  gdavidson
// Added log statement to track when additional background components are added
//
// Revision 1.15  2007/01/30 21:21:09  geconomos
// added include for vxComponent.h
//
// Revision 1.14  2006/12/14 22:08:47  romy
// component data changes incorporated
//
// Revision 1.13  2006/11/15 16:56:10  romy
// fixed compiler error
//
// Revision 1.12  2006/11/15 16:54:48  geconomos
// commented out deletion of component data
//
// Revision 1.11  2006/11/13 19:59:49  romy
// temporaraily fix added for LUNG Viewer Issue
//
// Revision 1.10  2006/11/09 21:50:07  romy
// moved  GetComponentIndex method to componentUtils
//
// Revision 1.9  2006/11/09 19:50:53  romy
// added GetComponentIndex method
//
// Revision 1.8  2006/10/10 21:35:02  romy
// temp fix added for Bostom Demo
//
// Revision 1.7  2006/09/21 14:53:37  geconomos
// made AreAnyLabelsVisible() and AreAnyIntensitiesInvisible() const member funstions
//
// Revision 1.6  2006/03/02 15:49:28  frank
// spelling
//
// Revision 1.5  2005/05/09 19:11:22  frank
// fixed Background component defaults
//
// Revision 1.4  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.6.1  2005/02/09 14:24:38  frank
// updated the selected index after component removal
//
// Revision 1.3  2004/07/12 18:02:40  frank
// added component data
//
// Revision 1.2  2004/04/14 13:59:21  frank
// implemented To String function
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.26  2004/02/11 22:44:04  michael
// replaced "NONE" by "Background"
//
// Revision 1.25  2004/01/05 18:58:13  michael
// added "mailto:"
//
// Revision 1.24  2003/11/21 16:16:25  michael
// took out log message
//
// Revision 1.23  2003/06/04 19:00:35  jmeade
// code review.
//
// Revision 1.22  2003/05/07 11:44:00  michael
// initial code review
//
// Revision 1.21  2003/04/28 16:59:25  michael
// fixed issue #3116 and 3065
//
// Revision 1.20  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.19  2003/04/14 21:08:57  michael
// cosmetics
//
// Revision 1.18  2003/04/09 21:40:02  michael
// fixed compacting of component array
//
// Revision 1.17  2003/04/09 15:05:31  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.16  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.15  2003/04/07 15:10:59  michael
// cosmetics
//
// Revision 1.14  2002/11/27 16:03:02  michael
// fixed compiler error that occurred in VC6 but not in VC7
//
// Revision 1.13  2002/11/26 13:40:38  michael
// renamed method (GetHistoryFromLabel()), fixed Compact() and bug in creation from map and vector
//
// Revision 1.12  2002/11/23 01:11:50  michael
// fixed the crash when rebuilding the vxComponentArray
//
// Revision 1.11  2002/11/15 23:10:23  ingmar
// made the component "NONE" be without label color by default -> more room for frater drawing of background areas
//
// Revision 1.10  2002/11/14 22:42:42  michael
// fixed assignment operator and copy constructor in vxComponentArray
//
// Revision 1.9  2002/11/14 20:08:51  michael
// added DeepCopy to vxComponentArray and removed redundant code
//
// Revision 1.8  2002/11/14 16:31:26  michael
// added mapping of 'NONE' component to std::map
//
// Revision 1.7  2002/11/14 01:21:32  michael
// further cleanup, added constructor from vector and map as well as operator=
//
// Revision 1.6  2002/11/13 01:54:02  michael
// further cleanup
//
// Revision 1.5  2002/11/13 00:36:50  michael
// first step towards a cleanup of the components(table)
//
// Revision 1.3  2002/10/18 17:50:26  ingmar
// use of new vxcComponent constructor -> shorter more efficient code
//
// Revision 1.2  2002/09/11 15:45:11  jaddonisio
// New map to get component index from component label.
//
// Revision 1.1  2002/09/05 23:16:27  jaddonisio
// Initial version: New vxComponent.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxComponentArray.C,v 1.18 2008/04/24 21:55:34 jmeade Exp $
// $Id: vxComponentArray.C,v 1.18 2008/04/24 21:55:34 jmeade Exp $
