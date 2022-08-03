// $Id: vxComponent.h,v 1.10 2007/01/25 21:53:09 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:michael@viatronix.com)


// pragmas
#pragma once


// includes
#include "PixelRGBA.h"



// forward declarations
class vxComponentArray;
class vxComponentData;


// class definition
class vxComponent 
{
// friends
private:

  /// friend to access member data
  friend vxComponentArray;

// functions
public:

  /// default constructor
  vxComponent();

  /// constructor
  vxComponent( uint2 uLabelId, std::string sName, PixelRGBA<uint1> labelColor,        
               bool bIntensityVisible, bool bLabelVisible, bool bLocked, bool bSelected );
  
  /// copy constructor
  vxComponent(const vxComponent & other);
  
  ///Assignement operator overloading
  vxComponent & operator=( const vxComponent & rhs );

  ///Copy the component object with or without the component data to the destination object
  void CopyTo( vxComponent & dest, bool bWithComponentData );

  
  /// destructor
  virtual ~vxComponent(); 

  /// return the component label index
  const uint2 & GetLabel() const { return m_uLabelId; };

  /// return the component name
  const std::string & GetName() const { return m_sName; };

  /// set the component's name
  void SetName(const std::string & sName) { m_sName = sName; };

  /// return the component's color
  const PixelRGBA<uint1> & GetColor() const { return m_labelColor; }; 

  /// return the component's color
  PixelRGBA<uint1> & GetColor() { return m_labelColor; }; 
 
  /// set the component's color
  void SetColor(const PixelRGBA<uint1> & labelColor) { m_labelColor = labelColor; };

  /// return if component's intensity is visible
  const bool & IsIntensityVisible() const { return m_bIntensityVisible; };

  /// set component's intensity visibility
  void SetIntensityVisible(const bool & bIntensityVisible) { m_bIntensityVisible = bIntensityVisible; };

  /// return if component's label is visible
  const bool & IsLabelVisible() const { return m_bLabelVisible; };

  /// set component's label visibility
  void SetLabelVisible(const bool & bLabelVisible) { m_bLabelVisible = bLabelVisible; };

  /// return if component is locked
  const bool & IsLocked() const { return m_bLocked; };
  
  /// set the lock of the component
  void SetLocked(const bool & bLocked) { m_bLocked = bLocked; };

  /// output the state of the object to a string
  std::string ToString() const { return "vxComponent: " + m_sName + ToAscii( uint4( m_uLabelId ) ); }

  /// get if allowed to perform a remove operation
  const bool & GetRemovable() const { return m_bRemovable; }

  /// set if allowed to perform a remove operation
  void SetRemovable( bool bRemovable ) { m_bRemovable = bRemovable; }

  /// get if allowed to perform a grow operation
  const bool & GetGrowable() const { return m_bGrowable; }

  /// set if allowed to perform a grow operation
  void SetGrowable( bool bGrowable ) { m_bGrowable = bGrowable; }

  /// get if allowed to perform a shrink operation
  const bool & GetShrinkable() const { return m_bShrinkable; }

  /// set if allowed to perform a shrink operation
  void SetShrinkable( bool bShrinkable ) { m_bShrinkable = bShrinkable; }

  /// get if allowed to unlock the component
  const bool & GetUnlockable() const { return m_bUnlockable; }

  /// set if allowed to unlock the component
  void SetUnlockable( bool bUnlockable ) { m_bUnlockable = bUnlockable; }

  /// get if user is allowed to edit the name
  const bool & GetNameEditable() const { return m_bNameEditable; }

  /// set if user is allowed to edit the name
  void SetNameEditable( bool bNameEditable ) { m_bNameEditable = bNameEditable; }

  /// get the component metadata
  vxComponentData * GetComponentDataPtr() const { return m_pComponentData; }

  /// set the component metadata
  void SetComponentDataPtr( vxComponentData * pComponentData ) { m_pComponentData = pComponentData; }

protected:

  /// set the component's label
  void SetLabel(const uint2 & uLabel) { m_uLabelId = uLabel; };

// data
private:

  /// component's lock flag              
  bool m_bLocked;     

  /// component's intensity visible flag 
  bool m_bIntensityVisible; 

  /// component's label visible flag     
  bool m_bLabelVisible;     

  /// component's label color            
  PixelRGBA<uint1> m_labelColor;    

  /// component's label id               
  uint2 m_uLabelId;

  /// component's name                   
  std::string m_sName;

  /// metadata for the component
  vxComponentData * m_pComponentData;

  /// if the user is allowed to perform a remove operation
  bool m_bRemovable;

  /// if the user is allowed to perform a grow operation
  bool m_bGrowable;

  /// if the user is allowed to perform a shrink operation
  bool m_bShrinkable;

  /// if the user is allowed to unlock the component
  bool m_bUnlockable;

  /// if the user is allowed to edit the name
  bool m_bNameEditable;

}; // class vxComponent


// $Log: vxComponent.h,v $
// Revision 1.10  2007/01/25 21:53:09  romy
// Fixed the issue of loosing the component data between Tab switching. Calcium scoring was suffering from this with black blocks on Plaques
//
// Revision 1.9  2006/12/14 21:57:51  romy
// reorganized to .h and .C files
//
// Revision 1.8  2006/09/25 13:19:25  geconomos
// added assigment operator
//
// Revision 1.7  2006/02/17 15:14:24  frank
// code review
//
// Revision 1.6  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.2  2005/02/15 19:41:08  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.5.2.1  2005/02/15 17:06:14  frank
// added unlockable attribute
//
// Revision 1.5  2004/12/13 15:32:59  frank
// added name editable property
//
// Revision 1.4  2004/09/21 17:22:39  frank
// implemented default constructor
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
// Revision 1.18  2003/07/11 19:49:14  michael
// final code reviews
//
// Revision 1.17  2003/06/09 16:10:16  michael
// took out redundant include
//
// Revision 1.16  2003/05/14 19:40:10  michael
// code review
//
// Revision 1.15  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.14  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.13  2003/04/07 15:10:59  michael
// cosmetics
//
// Revision 1.12  2003/03/18 16:16:50  geconomos
// Fixed comment text.
//
// Revision 1.11  2002/11/13 00:10:43  michael
// first step towards a cleanup of the components(table)
//
// Revision 1.10  2002/10/18 18:15:28  ingmar
// uups, forgot writable access function
//
// Revision 1.9  2002/10/18 17:49:42  ingmar
// code cleanup
// coding standards
// removal of obsolete functions
//
// Revision 1.8  2002/09/24 16:58:04  michael
// cleaned-up colortable and colormap ... hopefully we'll be able to remove
// member colortable from colormap.
//
// Revision 1.7  2002/09/11 15:44:26  jaddonisio
// Moved 'SetLabel' to protected.
//
// Revision 1.6  2002/09/05 22:37:46  jaddonisio
// New vxComponent.
//
// Revision 1.5  2002/08/12 18:37:10  ingmar
// added default con(de)structors to make linker happy
//
// Revision 1.4  2002/08/12 16:14:24  ingmar
// added access to componentHistogramInfo
//
// Revision 1.3  2002/08/09 22:49:54  ingmar
// added vxComponentHistogramInfo as member
//
// Revision 1.2  2002/08/07 18:31:19  michael
// Fixed two missing "vx'
//
// Revision 1.1  2002/08/07 18:26:44  michael
// Added initial version of vxComponent class, encapsulating the state of one component
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxComponent.h,v 1.10 2007/01/25 21:53:09 romy Exp $
// $Id: vxComponent.h,v 1.10 2007/01/25 21:53:09 romy Exp $
