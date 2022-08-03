// $Id: ControllerHotKeys.cs,v 1.2 2007/03/01 17:09:17 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.Windows.Forms;

namespace Viatronix.UI
{
  /// <summary>
  /// Responsible for maintaining the frame's hot keys
  /// </summary>
  public class ControllerHotKeys : System.Windows.Forms.IMessageFilter
	{
    #region fields

    /// <summary>
    /// hot key map, key is key code, value is functor
    /// </summary>
    Hashtable m_hotkeys = new Hashtable();
  
    #endregion

    #region properties

    /// <summary>
    /// Gets the hot keys for the controller
    /// </summary>
    public Hashtable HotKeys
    {
      get { return m_hotkeys; }
    } // HotKeys

   
    #endregion

    #region methods

    /// <summary>
    /// Filters out a message before it is dispatched. This methods processes the plugin's
    /// hotkeys.
    /// </summary>
    /// <param name="m">The message to be dispatched. You cannot modify this message.</param>
    /// <returns>true to filter the message and prevent it from being dispatched; 
    /// false to allow the message to continue to the next filter or control.
    /// </returns>
    public bool PreFilterMessage( ref Message m )
    {
      // let's check if the main application form is the active one;
      // if it is not then we can assume that a dialog of some sort is up,
      // in this case we will NOT process the hot keys
      if( !(System.Windows.Forms.Form.ActiveForm is Viatronix.UI.Frame ) )
        return false;

      // otherwise process WM_KEYDOWN events
      if( m.Msg == ( int ) Win32.Msgs.WM_KEYDOWN  )
      {
        // convert to Keys enum
        Keys key = ( Keys ) ( int ) m.WParam & Keys.KeyCode;

        // does the map contain a value for the specifed key?
        if( m_hotkeys.Contains( key ) )
        {
          // get the hot key from the map
          FrameHotKey hotkey = ( FrameHotKey ) m_hotkeys[ key ];
          
          // let it do it's thing
          hotkey.Execute();
          return true;
        }
      } // WM_KEYDOWN
      return false;
    } // PreFilterMessage()	

  
    #endregion
	} // class ControllerHotKeys
} // namespace Viatronix.Viewer

#region revision history

// $Log: ControllerHotKeys.cs,v $
// Revision 1.2  2007/03/01 17:09:17  geconomos
// code review preparation
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/ControllerHotKeys.cs,v 1.2 2007/03/01 17:09:17 geconomos Exp $
// $Id: ControllerHotKeys.cs,v 1.2 2007/03/01 17:09:17 geconomos Exp $

#endregion