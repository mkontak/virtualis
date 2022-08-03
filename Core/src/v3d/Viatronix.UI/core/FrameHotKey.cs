// $Id: FrameHotKey.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Xml;

namespace Viatronix.UI
{
	/// <summary>
	/// Represents a  "Hot Key" then when pressed will perform a specific action. 
	/// </summary>
  public abstract class FrameHotKey
	{
    #region fields


    /// <summary>
    /// current xml version
    /// </summary>
    private const string PLUGIN_HOTKEY_XML_VERSION = "1.0";

    /// <summary>
    /// key assigned to hot key
    /// </summary>
    private System.Windows.Forms.Keys m_assignedKey = System.Windows.Forms.Keys.None;

    #endregion

    #region properties


    /// <summary>
    /// Gets or sets the key assigned to this hot key
    /// </summary>
    public System.Windows.Forms.Keys AssignedKey
    {
      get { return m_assignedKey; }
      set { m_assignedKey = value; }
    } // AssignedKey

    
    #endregion

    #region methods

    /// <summary>
    /// Executes the action for the hot key.
    /// </summary>
    public abstract void Execute();

    #endregion

    #region serialization

    /// <summary>
    /// Creates a new FrameHotKey instance from the specified xml node.
    /// </summary>
    /// <param name="parent">XmlNode containing initialization data</param>
    /// <returns>New FrameHotKey instance.</returns>
    public static FrameHotKey Create(  XmlNode parent )
    {
      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != PLUGIN_HOTKEY_XML_VERSION )
      {
        throw new ArgumentException( "Invalid xml version for class FrameHotKey." );
      }

      FrameHotKey hotkey = ( FrameHotKey )System.Activator.CreateInstance( System.Type.GetType(  parent.Attributes[ "typename" ].Value ) );

      hotkey.AssignedKey = (System.Windows.Forms.Keys) Enum.Parse( typeof(System.Windows.Forms.Keys), parent.Attributes[ "key" ].Value, true );

      return hotkey;
    } // FromXml( XmlNode parent )

    
    #endregion

	} // class FrameHotKey
} // namespace Viatronix.Viewer.Plugins

#region revision history

// $Log: FrameHotKey.cs,v $
// Revision 1.2.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.2  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.1  2004/09/15 17:49:38  geconomos
// moved files
//
// Revision 1.1  2004/06/09 19:12:20  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/FrameHotKey.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $
// $Id: FrameHotKey.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $

#endregion