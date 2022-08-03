// $Id: vxEasySelection.h,v 1.2 2006/12/14 22:01:47 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <string>
#include "vxElement.h"
#include "Serialization.h"
#include "vxComponentData.h"


/**
 * Class to maintain the data associated with an easy selected component
 */
class VX_VIEWER_DLL vxEasySelection : public vxElement, public vxComponentData
{
// member functions
public:

  /// constructor
  vxEasySelection();

  /// destructor
  virtual ~vxEasySelection();

  //Clone method
  vxEasySelection * Clone() const;

  /// set the label of the selection
  inline void SetLabel( const uint2 & uLabel ) { m_uLabel = uLabel; }

  /// set the label of the selection
  inline const uint2 & GetLabel() const { return m_uLabel; }

  /// sets whether this selection is grown in the vesselness volume
  inline void SetVesselness( const bool & bVesselness ) { m_bVesselness = bVesselness; }

  /// gets whether this selection is grown in the vesselness volume
  inline const bool & GetVesselness() const { return m_bVesselness; }

// vxComponentData implementation
public:

  /// return the component data name
  inline const std::string GetName() const { return "vxEasySelection"; }

  /// if allowed to perform a grow operation
  inline virtual bool IsGrowSupported() { return false; }

  /// if allowed to perform a shrink operation
  inline virtual bool IsShrinkSupported() { return false; }

  /// perform a grow on the component data (will be called after the component grows)
  inline virtual void GrowComponentData() {}

  /// perform a shrink on the component data (will be called after the component shrinks)
  inline virtual void ShrinkComponentData() {}

// vxISerialize implementation
public:

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxEasySelection"; }

// data members
private:

  /// label of the component
  uint2 m_uLabel;

  /// whether this selection is grown in the vesselness volume
  bool m_bVesselness;

}; // Class vxEasySelection


// $Log: vxEasySelection.h,v $
// Revision 1.2  2006/12/14 22:01:47  romy
// added Clone interface to Component Data
//
// Revision 1.1  2006/09/25 13:17:33  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:58:16  vxconfig
// formatting
//
// Revision 1.1  2005/06/07 20:05:47  vxconfig
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxEasySelection.h,v 1.2 2006/12/14 22:01:47 romy Exp $
// $Id: vxEasySelection.h,v 1.2 2006/12/14 22:01:47 romy Exp $
