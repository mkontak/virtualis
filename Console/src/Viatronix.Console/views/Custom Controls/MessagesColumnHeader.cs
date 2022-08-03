// $Id: MessagesColumnHeader.cs,v 1.2 2005/10/18 17:46:03 gdavidson Exp $
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
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Xml;

using Viatronix.UI;

namespace Viatronix.Console
{
	/// <summary>
	/// Column generated from xml and populates the subitems using reflection.
	/// </summary>
	public class MessagesColumnHeader : DynamicDataSetColumnHeader, IDynamicImageColumnHeader
	{

    #region fields

    /// <summary>
    /// Collection of images for the column.
    /// </summary>
    private static ImageList m_imageList = new ImageList();

    #endregion

    #region construction

    /// <summary>
    /// Loads the images.
    /// </summary>
    static MessagesColumnHeader()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();
      m_imageList.Images.Add( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.Console.res.error.bmp" )));
      m_imageList.Images.Add( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.Console.res.Information.bmp" )));
      m_imageList.Images.Add( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.Console.res.warning.bmp" )));
    } // MessagesColumnHeader()

    #endregion

    #region IDynamicImageColumnHeader Members

    /// <summary>
    /// Returns a collection of small images.
    /// </summary>
    public ImageList SmallImages
    {
      get { return m_imageList; }
    } // SmallImages


    /// <summary>
    /// Returns a collection of large images.
    /// </summary>
    public ImageList LargeImages
    {
      get { return m_imageList; }
    } // LargeImages


    /// <summary>
    /// Returns the index of the image associated with the object.
    /// </summary>
    /// <param name="obj">The object being displayed.</param>
    /// <returns>The index of the image.</returns>
    public int GetIndex( object obj )
    {
      switch( ConvertToString( GetValue( obj )))
      {
        case "Error":
          return 0;
        case "Information":
          return 1;
        case "Warning":
          return 2;
        default:
          throw new NotImplementedException( "The specified object doesnot have an associated image." );
      }   
    } // GetIndex( obj )

    #endregion

  } // class DynamicColumnHeader
} // namespace Viatronix.UI


#region revision history

// $Log: MessagesColumnHeader.cs,v $
// Revision 1.2  2005/10/18 17:46:03  gdavidson
// Modified the DynamicColumnHeader object.
//
// Revision 1.1  2005/06/22 15:19:21  gdavidson
// Inital Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/Custom Controls/MessagesColumnHeader.cs,v 1.2 2005/10/18 17:46:03 gdavidson Exp $
// $Id: MessagesColumnHeader.cs,v 1.2 2005/10/18 17:46:03 gdavidson Exp $

#endregion
