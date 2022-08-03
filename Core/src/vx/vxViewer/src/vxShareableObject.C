// $Id: vxShareableObject.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxShareableObject.h"


/** 
 * Helper class returning the primary observer 
 * @return observer
 */
vxObserver * & vxShareableObjectHelper::PrimaryObserver()
//*******************************************************************
{
  static vxObserver * primaryObserver;
  return primaryObserver;
}


/** 
* Helper class finding the reference count map
* @return map
*/
std::map<vxObserver *, int4> & vxShareableObjectHelper::ObserverReferenceCountMap()
//*******************************************************************
{
  static std::map<vxObserver *, int4> observerReferenceCountMap;

  return observerReferenceCountMap;
}


// Revision History:
// $Log: vxShareableObject.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/25 12:49:06  michael
// cosmetics
//
// Revision 1.4  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.3  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.2  2003/01/06 18:56:00  ingmar
// moved static instance to static member functions
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.4  2002/06/12 20:55:19  michael
// added funtionality so that on mouse drag window/level only gets updated in
// current viewer, update on all others OnButtonUp()
//
// Revision 1.3  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxShareableObject.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxShareableObject.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
