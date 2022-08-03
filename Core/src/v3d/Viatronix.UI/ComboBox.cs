// $Id: ComboBox.cs,v 1.2 2005/03/08 19:11:06 frank Exp $
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MainClass.cs  
// Owner: Jeff Meade jmeade@viatronix.com

using System;
using System.Collections;

namespace Viatronix.UI
{
	/// <summary>
	/// Combobox containing object-string mappings.
	/// </summary>
	public class ComboBox : System.Windows.Forms.ComboBox
	{

    #region ObjectString struct

    /// <summary>
    /// Structure for an object-to-string comboBox mapping (to ELIMINATE STRING HARDCODING!!!!)
    /// </summary>
    public struct ObjectString
    {

      #region fields

      /// <summary>
      /// The object of the mapping.
      /// </summary>
      private object m_object;

      /// <summary>
      /// The string of the mapping.
      /// </summary>
      private string m_string;

      #endregion
      
      #region construction

      /// <summary>
      /// Creates a new instance of an ObjectString.
      /// </summary>
      /// <param name="obj">The object value.</param>
      /// <param name="str">The string key.</param>
      public ObjectString( object obj, string str )
      {  
        m_object = obj; 
        m_string = str;  
      } // ObjectString( obj, str )

      #endregion

      #region properties

      /// <summary>
      /// Gets or sets the object of the mapping.
      /// </summary>
      public object Object
      {
        get { return m_object;  }
        set { m_object = value; }
      } // Object

      /// <summary>
      /// Gets or sets the string of the mapping.
      /// </summary>
      public string String
      {
        get { return m_string;  }
        set { m_string = value; }
      } // String
 
      #endregion

    } // struct ObjectString

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a ComboBox control.
    /// </summary>
    public ComboBox()
    {
      DisplayMember = "String";
      ValueMember = "Object";
    } // ComboBox()

    #endregion

	} // class ComboBox 
} // namespace Viatronix.UI

#region revision history

// Revision History
// $Log: ComboBox.cs,v $
// Revision 1.2  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/03/02 16:47:14  gdavidson
// Code review
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/14 17:08:23  jmeade
// Combo box component, for now just with a helper mapping values to
//  strings to eliminate Non-localized string proliferation throughout code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ComboBox.cs,v 1.2 2005/03/08 19:11:06 frank Exp $
// $Id: ComboBox.cs,v 1.2 2005/03/08 19:11:06 frank Exp $

#endregion
