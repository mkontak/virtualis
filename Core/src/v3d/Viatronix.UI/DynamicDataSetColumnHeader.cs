// $Id: DynamicDataSetColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Data;
using System.Reflection;
using System.Collections;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// Column generated from xml and populates the subitems using reflection.
  /// </summary>
  public class DynamicDataSetColumnHeader : DynamicColumnHeader
  {

    #region fields

    /// <summary>
    /// The type of the object used by the column.
    /// </summary>
    private Type m_type = null;

    /// <summary>
    /// Collection of properties.
    /// </summary>
    private ArrayList m_properties = new ArrayList();

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a DynamicDataSetColumnHeader.
    /// </summary>
    protected DynamicDataSetColumnHeader()
    {
    } // DynamicDataSetColumnHeader()

    #endregion

    #region override methods

    public override object Clone()
    {
      DynamicDataSetColumnHeader header = new DynamicDataSetColumnHeader();
      CopyTo( header );

      return header;
    } // Clone()

    #endregion

    #region methods

    /// <summary>
    /// Returns the value for the column from the provided object.
    /// </summary>
    /// <param name="obj">The object containing the value.</param>
    /// <returns>The value of the object.</returns>
    public override object GetValue( object obj )
    {
      try
      {
        DataRow row = (DataRow) obj;
        return row[ this.Property ];
      }
      catch( Exception )
      {
        return null;
      }
    } // GetValue( obj )


    #endregion

  } // class DynamicDataSetColumnHeader
} // namespace Viatronix.UI


#region revision history

// $Log: DynamicDataSetColumnHeader.cs,v $
// Revision 1.3  2005/10/18 17:54:53  gdavidson
// Added sorting of columns.
//
// Revision 1.2  2005/06/22 14:51:38  gdavidson
// Changed the access modifier for the constructor.
//
// Revision 1.1  2005/06/03 14:47:46  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DynamicDataSetColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $
// $Id: DynamicDataSetColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $

#endregion
