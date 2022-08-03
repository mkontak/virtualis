// $Id: vxISerialize.h,v 1.2 2005/04/12 15:23:23 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 *  This class is a pure interface class that defines 
 *  the methods a derived class needs to implement.
 */

#ifndef vxISerialize_h
#define vxISerialize_h


// includes
#include "vxDOM.h"



// forward declarations
template <class T> class Point2D;


// class definition
class VX_VIEWER_DLL vxISerialize
{
// functions
public:
  
  /// default constructor
  vxISerialize() {};

  /// destructor
  virtual ~vxISerialize() {};

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const = 0;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element ) = 0;

  /// name for Xml node
  virtual const std::string GetElementName() const = 0;
}; // vxISerialize


#endif // vxISerialize_h


// Revision History:
// $Log: vxISerialize.h,v $
// Revision 1.2  2005/04/12 15:23:23  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.4  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.3  2003/02/21 20:01:24  michael
// static member initialization and other changes
//
// Revision 1.2  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.1  2003/02/19 16:28:54  michael
// added initial version of interface class for serialization
//
// Revision 1.5  2002/11/22 00:30:02  geconomos
// Added OnSetFocus
//
// Revision 1.4  2002/09/25 16:59:08  geconomos
// Added modal flag and accessors.
//
// Revision 1.3  2002/08/20 22:27:05  uday
// Mouse / KeyBoard events return whether they are handled or not.
//
// Revision 1.2  2002/08/05 16:19:49  dmitry
// OnTimer takes std::string.
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.6  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.5  2002/05/06 21:45:41  dmitry
// OnTimer Added
//
// Revision 1.4  2002/05/02 15:52:10  michael
// completed transition from MouseCursor to vxMouseCursor
//
// Revision 1.3  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.2  2002/03/28 23:13:50  jaddonisio
// Made destructor virtual.
//
// Revision 1.1  2002/03/19 17:14:43  dmitry
// Added initial version of 74_vxUtils
//
// Revision 1.1  2002/03/19 16:40:10  dmitry
// Moved to 75_vxUtils from 75_vxManipulators
//
// Revision 1.1  2002/03/11 19:42:44  michael
// moved xvManipulators, vxISerialize, and vxEnvironment to 75_vxManipulators
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.5  2001/12/19 13:48:43  geconomos
// Changed signature of OnMouseMove.
//
// Revision 1.4  2001/12/18 17:19:00  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 1.3  2001/12/12 18:37:20  michael
// made actual pointer to object private such that derived classes are forced
// to use the Set/Get-methods.
//
// Revision 1.2  2001/12/10 17:23:55  geconomos
// Added constants for key down events to handle arrow keys.
//
// Revision 1.1  2001/12/06 17:24:24  michael
// Added Inputs class and the first manipulator classes. One is an implementation
// class for manipulating a TransferFunction (presets and right mouse).
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxISerialize.h,v 1.2 2005/04/12 15:23:23 geconomos Exp $
// $Id: vxISerialize.h,v 1.2 2005/04/12 15:23:23 geconomos Exp $