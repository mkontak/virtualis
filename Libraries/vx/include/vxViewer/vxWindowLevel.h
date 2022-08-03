// $Id: vxWindowLevel.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:meissner@viatronix.com)

/**
 *  set window level
 */

#ifndef vxWindowLevel_h
#define vxWindowLevel_h


// includes
#include "ZeroOne.h"


// class definition
class VX_VIEWER_DLL vxWindowLevel
{
// functions
public:

  /// default constructor
  vxWindowLevel();

  /// constructor
  vxWindowLevel(ZeroOne fWindow, ZeroOne fLevel):
      m_fWindow(fWindow),
      m_fLevel(fLevel) {};

  /// Constructor
  //vxWindowLevel(uint2 uWindow, uint2 uLevel):
      //m_fWindow(HU::ConvertFromHU(uWindow-1000)), // HACK since it is only valid for CT data
      //m_fLevel(HU::ConvertFromHU(uLevel)) {};

  /// copy constructor
  vxWindowLevel(const vxWindowLevel & other);

  /// Default destructor
  ~vxWindowLevel() {};

  /// Get window value
  const ZeroOne & GetWindow() const { return m_fWindow; };

  /// Set window value
  void SetWindow(const ZeroOne & fWindow) { m_fWindow = fWindow; };

  /// Get level value
  const ZeroOne & GetLevel() const { return m_fLevel; };

  /// Set level value
  void SetLevel(const ZeroOne & fLevel) { m_fLevel = fLevel; };

  /// assignment operator
  const vxWindowLevel & operator=(const vxWindowLevel & other);

// data
private:
  
  /// window value
  ZeroOne m_fWindow;

  /// level value
  ZeroOne m_fLevel;
};


#endif // vxWindowLevel_h


// Revision History:
// $Log: vxWindowLevel.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/01/05 19:00:25  michael
// added "mailto:"
//
// Revision 1.10  2003/04/25 19:16:10  dongqing
// code review
//
// Revision 1.9  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.8  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.7  2003/04/18 15:31:25  frank
// Removed HUconverter
//
// Revision 1.6  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.5  2002/10/14 15:30:00  michael
// made GetWindow() and GetLevel() const methods
//
// Revision 1.4  2002/10/13 19:00:02  michael
// added comment
//
// Revision 1.3  2002/10/01 15:45:56  michael
// Added -1000 for Window
//
// Revision 1.2  2002/10/01 13:56:41  michael
// Added constructor to take uint2 and automatically do the converstion. It
// is somehow problematic since HU is for CT only but for now, it saves calls.
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.11  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.10  2002/06/10 15:18:12  dmitry
// Constructor moved to .C file
//
// Revision 1.9  2002/05/23 16:36:24  frank
// Brightened the default window/level.
//
// Revision 1.8  2002/05/01 13:49:00  michael
// Started code review related stuff ...
//
// Revision 1.7  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 1.6  2002/04/21 21:58:28  michael
// removed HU conversion from WindowLevel class since it is not a property of this object
//
// Revision 1.5  2002/04/21 20:07:06  frank
// Added HU conversion functionality.
//
// Revision 1.4  2002/04/20 03:06:31  michael
// added source file of vxWindowLevel
//
// Revision 1.3  2002/04/18 17:45:37  michael
// set default window level to another value
//
// Revision 1.1  2002/04/12 22:19:22  michael
// added initial version of the window level class
//
// $Id: vxWindowLevel.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxWindowLevel.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
