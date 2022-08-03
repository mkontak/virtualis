// $Id: DynamicObjectColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Reflection;
using System.ComponentModel;
using System.Collections;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// Column generated from xml and populates the subitems using reflection.
  /// </summary>
  public class DynamicObjectColumnHeader : DynamicColumnHeader
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
    /// Creates a new instance of a DynamicObjectColumnHeader.
    /// </summary>
    protected DynamicObjectColumnHeader()
    {
    } // DynamicObjectColumnHeader()

    #endregion

    #region override methods

    public override object Clone()
    {
      DynamicObjectColumnHeader header = new DynamicObjectColumnHeader();
      CopyTo( header );
      
      header.m_type = m_type;
      header.m_properties = (ArrayList) m_properties.Clone();

      return header;
    } // Clone()


    public override void Initialize( XmlNode node )
    {
      base.Initialize( node );

      Type type = Type.GetType( node.Attributes[ "type" ].Value );
      m_type = type;
      
      foreach( string propertyName in this.Property.Split( '.' ))
      {
        PropertyDescriptor property = TypeDescriptor.GetProperties( type )[ propertyName ];
        m_properties.Add( property );
        type = property.PropertyType;
      }
    } // Initialize( node )


    /// <summary>
    /// Returns the value for the column from the provided object.
    /// </summary>
    /// <param name="obj">The object containing the value.</param>
    /// <returns>The value of the object.</returns>
    public override object GetValue( object obj )
    {
      try
      {
        object target = obj;
        foreach( PropertyDescriptor property in m_properties )
          target = property.GetValue( target );

        return target;
      }
      catch( Exception  )
      {
        return null;
      }
    } // GetValue( obj )


    #endregion

  } // class DynamicObjectColumnHeader
} // namespace Viatronix.UI


#region revision history

// $Log: DynamicObjectColumnHeader.cs,v $
// Revision 1.3  2005/10/18 17:54:53  gdavidson
// Added sorting of columns.
//
// Revision 1.2  2005/06/22 14:51:38  gdavidson
// Changed the access modifier for the constructor.
//
// Revision 1.1  2005/06/03 14:47:46  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DynamicObjectColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $
// $Id: DynamicObjectColumnHeader.cs,v 1.3 2005/10/18 17:54:53 gdavidson Exp $

#endregion
