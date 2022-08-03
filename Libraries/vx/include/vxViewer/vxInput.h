// $Id: vxInput.h,v 1.3 2007/01/16 17:04:53 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

#ifndef vxInput_h
#define vxInput_h



// forward declarations
template <class T> class Point2D;


/**
 * This class is a pure interface class that provides 
 * the possible inputs to windows, manipulators, and
 * others that need to act upon any given input event.
 */
class VX_VIEWER_DLL vxInput
{
/// enums
public:
  
  /// the various mouse buttons
  enum ButtonEnum
  {
    BUTTON_LEFT   = 0,
    BUTTON_MIDDLE = 1,
    BUTTON_RIGHT  = 2,
  };
  
  /// the various key states
  enum KeyStateEnum
  {
    STATE_SHIFT    = 0x0001,
    STATE_CTRL     = 0x0002,
    STATE_ALT      = 0x0004,
    STATE_LBUTTON  = 0x0010,
    STATE_MBUTTON  = 0x0020,
    STATE_RBUTTON  = 0x0040,
  };
  
  /// the various key types (see http://www.asciitable.com for additional codes)
  enum KeyTypeEnum
  {
    KEY_BACKSPACE         = 8,
    KEY_TAB               = 9,
    KEY_ENTER             = 13,
    KEY_SHIFT             = 16,
    KEY_CONTROL           = 17,
    KEY_ESCAPE            = 27,
    KEY_PAGE_SPACE        = 32,
    KEY_PAGE_UP           = 33,
    KEY_PAGE_DOWN         = 34,
    KEY_END               = 35,
    KEY_HOME              = 36,
    KEY_ARROW_LEFT        = 37,
    KEY_ARROW_UP          = 38,
    KEY_ARROW_RIGHT       = 39,
    KEY_ARROW_DOWN        = 40,
    KEY_0                 = 48,
    KEY_1                 = 49,
    KEY_2                 = 50,
    KEY_3                 = 51,
    KEY_4                 = 52,
    KEY_5                 = 53,
    KEY_6                 = 54,
    KEY_7                 = 55,
    KEY_8                 = 56,
    KEY_9                 = 57,
    KEY_A                 = 65,
    KEY_B                 = 66,
    KEY_C                 = 67,
    KEY_D                 = 68,
    KEY_E                 = 69,
    KEY_F                 = 70,
    KEY_G                 = 71,
    KEY_H                 = 72,
    KEY_I                 = 73,
    KEY_J                 = 74,
    KEY_K                 = 75,
    KEY_L                 = 76,
    KEY_M                 = 77,
    KEY_N                 = 78,
    KEY_O                 = 79,
    KEY_P                 = 80,
    KEY_Q                 = 81,
    KEY_R                 = 82,
    KEY_S                 = 83,
    KEY_T                 = 84,
    KEY_U                 = 85,
    KEY_V                 = 86,
    KEY_W                 = 87,
    KEY_X                 = 88,
    KEY_Y                 = 89,
    KEY_Z                 = 90,
    KEY_LFET_SINGLE_QUOTE = 96,
    KEY_a                 = 97,
    KEY_b                 = 98,
    KEY_c                 = 99,
    KEY_d                 = 100,
    KEY_e                 = 101,
    KEY_f                 = 102,
    KEY_g                 = 103,
    KEY_h                 = 104,
    KEY_i                 = 105,
    KEY_j                 = 106,
    KEY_k                 = 107,
    KEY_l                 = 108,
    KEY_m                 = 109,
    KEY_n                 = 110,
    KEY_o                 = 111,
    KEY_F1                = 112,
    KEY_F2                = 113,
    KEY_F3                = 114,
    KEY_F4                = 115,
    KEY_F5                = 116,
    KEY_F6                = 117,
    KEY_F7                = 118,
    KEY_F8                = 119,
    KEY_F9                = 120,
    KEY_F10               = 121,
    KEY_F11               = 122,
    KEY_F12               = 123,
    KEY_DEL               = 127,
    KEY_PLUS_KEYPAD       = 107,
    KEY_MINUS_KEYPAD      = 109,
    KEY_EQUALS            = 187,
    KEY_MINUS             = 189,
  };
  
  /// default constructor
  vxInput() {};

  /// destructor
  virtual ~vxInput() {}

  /// event handler can be reimplemented in a subclass to receive wheel events
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive mouse move events
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive mouse down events
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive mouse up events
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive mouse down events
  virtual bool OnDoubleClick(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive keyboard down events
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive keyboard up events
  virtual bool OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState) {return false;}

  /// event handler can be reimplemented in a subclass to receive mouse enter events
  virtual void OnMouseEnter() {}

  /// event handler can be reimplemented in a subclass to receive mouse leave events
  virtual void OnMouseLeave() {}

  /// event handler can be reimplemented in a subclass to receive SetCursor events for the viewer
  virtual bool OnSetCursor() { return false; }

  /// returns the modal flag
  static bool GetModal() { return m_bModal; }

  /// sets the modal flag
  static void SetModal( bool bModal ) { m_bModal = bModal; }

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  static int4 GetMouseWheelQuantum() { return 120; }

private:

  /// copy constructor
  vxInput(const vxInput & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxInput & operator=(const vxInput & other); // should only be public if really implemented!

  /// indicates if inputs are in a modal state
  static bool m_bModal;  

}; // vxInput

#endif // vxInput_h


// $Log: vxInput.h,v $
// Revision 1.3  2007/01/16 17:04:53  gdavidson
// Removed OnTimer
//
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.2  2005/02/08 15:34:56  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.1.1.1.6.1  2005/02/07 13:50:22  frank
// added numeric keypad numbers
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/02/09 20:33:21  dongqing
// add control key
//
// Revision 1.20  2004/01/12 14:19:38  frank
// fixed problem with manipulating on oblique with shift held down
//
// Revision 1.19  2004/01/09 17:17:29  frank
// lowercase characters are not used, they are actually the function keys
//
// Revision 1.18  2004/01/07 18:30:54  frank
// Added additional key codes
//
// Revision 1.17  2003/12/16 19:04:53  frank
// Added additional key definitions
//
// Revision 1.16  2003/12/10 12:44:35  frank
// spelling
//
// Revision 1.15  2003/12/09 16:22:28  wenli
// tab key definitation
//
// Revision 1.14  2003/12/08 21:23:41  frank
// Added the tab key code - we need to add them all when we get time
//
// Revision 1.13  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.12  2003/04/29 19:41:24  frank
// Added key up message handling and performed code review
//
// Revision 1.11  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.10  2003/04/18 19:56:30  michael
// initial code review
//
// Revision 1.9  2003/04/16 12:47:36  michael
// performed initial code review
//
// Revision 1.8  2003/03/12 15:20:38  frank
// Added a constant mouse wheel quantum unit
//
// Revision 1.7  2003/02/19 19:01:16  michael
// added serialization of annotations to the managed code
//
// Revision 1.6  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
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
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxInput.h,v 1.3 2007/01/16 17:04:53 gdavidson Exp $
// $Id: vxInput.h,v 1.3 2007/01/16 17:04:53 gdavidson Exp $
