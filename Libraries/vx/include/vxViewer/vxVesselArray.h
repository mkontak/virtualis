// $Id: vxVesselArray.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxElement.h"
#include "vxComponentArray.h"
#include "PixelRGBA.h"
#include "vxVessel.h"
#include "vxISerialize.h"
#include "vxShareableObject.h"


/**
 * vxVesselArray Class contains a list of vxVessel and functions to handle it.
 */
class VX_VIEWER_DLL vxVesselArray : public vxElement, public vxISerialize
{

// Enumerations
public:
  
  // the different types of events.
  enum EventsEnum
  {
    EVENT_ADDED       = 0x00000001,
    EVENT_MODIFIED    = 0x00000002,
    EVENT_REMOVED     = 0x00000004,
    EVENT_INITIALIZED = 0x00000008,
  }; // enum EventsEnum


// Member Functions
public:

  /// constructor
  vxVesselArray();

  /// destructor
  virtual ~vxVesselArray();

  /// equality operator
  bool operator ==(const vxVesselArray & other) const;

  /// inequality operator
  bool operator !=(const vxVesselArray & other) const;

  //
  // Input / Output
  //

  /// retrieves the number of bytes of the serialized data
  uint4 GetSerializedDataSize() const;

  /// reads in all the data from serialized form
  void ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// read the vessel from a file
  void Read( const std::string & sFilename );

  /// compact the vessel array so the label indices contain no holes
  void Compact();
  
  /// output to a string
  std::string ToString() const;

  //
  // XML Serialization
  //

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxVesselArray"; }

  //
  // Gets and Sets
  //

  /// Returns a vessel with the specified index.
  vxVessel & GetVessel(const uint2 uNumber);

  /// Get the vessel list
  inline const std::vector<vxVessel> & GetVessels() const { return m_vVesselList; }

  /// Get the easy selected vessel label list
  inline std::vector<uint2> & GetEasySelectedVesselLabels() { return m_vEasySelectedVesselLabelList; }

  /// Sets the component info
  inline void SetComponentInfo( vxShareableObject< vxComponentArray > * pComponentInfo) { m_pComponentInfo = pComponentInfo; }

  /// Gets the component info
  vxComponentArray & GetComponentInfo() const { return *m_pComponentInfo; }

  /// Gets the current vessel index
  inline const int4 & GetCurrentVesselIndex() const { return m_iCurrentVesselIndex; }

  /// Sets the current vessel index
  inline void SetCurrentVesselIndex( const int4 iNewVesselIndex ) { m_iCurrentVesselIndex = iNewVesselIndex; }

  /// Gets the current vessel
  inline vxVessel * GetCurrentVessel()
  {
    if( m_iCurrentVesselIndex > -1 && m_iCurrentVesselIndex < m_vVesselList.size() )
      return & m_vVesselList[m_iCurrentVesselIndex];
    else
      return NULL;
  }

  /// Returns the index of the vessel given a label (-1 if not found)
  int4 GetIndexFromLabel( uint2 uLabel ) const;

  //
  // Main functions
  //

  /// Reset.
  void Reset();

  /// Add a vessel to the list, get the lable of component
  void AddVessel( const vxVessel & vessel, const uint2 & uLabel );

  void PushVessel( const vxVessel & vessel, const uint2 & uLabel );

  /// Remove a vessel from the list.
  void RemoveVessel( uint4 iVesselIndex, const bool bRemoveComponentAlso );

  /// Returns the number of vessels in the vector.
  inline const uint4 GetNumberOfVessels() const { return  m_vVesselList.size(); }

  /// Recalculate the < min, max > diameter points from the contour.
  void RecalculateDiameterPoints();


// Data Members
private:

  /// list of vxVessels
  std::vector< vxVessel > m_vVesselList;

  /// list of easy selected vessels
  std::vector< uint2 > m_vEasySelectedVesselLabelList;

  /// component member
  vxShareableObject< vxComponentArray > * m_pComponentInfo;

  /// current vessel index
  int4 m_iCurrentVesselIndex;

  /// delimiter for debugging I/O
  const static std::string m_sDelimiter;

  /// file version
  static const float4 m_fVersion;

}; // class vxVesselArray


#pragma region revision history


// $Log: vxVesselArray.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:02:41  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:14:38  vxconfig
// removed the vessel array
//
// Revision 1.2  2005/04/12 15:23:45  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.4  2004/04/14 14:01:21  frank
// compacted the vessel array to match component and label volume compaction during reading
//
// Revision 1.3  2004/03/30 21:52:17  frank
// added XML serialization
//
// Revision 1.2  2004/03/06 00:38:51  wenli
// PushVessel is added
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/07 21:46:20  wenli
// Add  m_vEasySelectedVesselLabelList  for easy selected vessels
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.8  2003/05/06 16:21:02  frank
// Removed HUConverter
//
// Revision 1.7  2003/04/30 15:21:19  frank
// Issue #3151: Used component handler to create new components so that they can be handled properly, e.g., grow, shrink, etc.
//
// Revision 1.6  2003/03/26 19:45:32  frank
// Improved vessel removal
//
// Revision 1.5  2003/01/14 16:16:41  dongqing
// Code review
//
// Revision 1.4  2002/11/20 17:08:52  frank
// removed dead code
//
// Revision 1.3  2002/11/13 00:00:37  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.2  2002/11/04 14:54:38  frank
// dead code removed
//
// Revision 1.1  2002/10/31 12:46:53  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.4  2002/10/25 17:11:39  frank
// Added versioning and current selection to (de)serialization.
//
// Revision 1.3  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.2  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.1  2002/09/20 18:54:48  wenli
// v3D vessel component list
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVesselArray.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxVesselArray.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $


#pragma endregion 