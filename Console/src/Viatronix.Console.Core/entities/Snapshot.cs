// $Id: Snapshot.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.IO;
using System.Drawing;
using System.Runtime.Serialization;

namespace Viatronix.Console.Entities
{
	/// <summary>
	/// Summary description for Snapshot.
	/// </summary>
  [Serializable]
	public class Snapshot : ISerializable
	{

    #region fields

    /// <summary>
    /// the current version of the Snapshot object.
    /// </summary>
    public const string Version = "1.0";

    /// <summary>
    /// snapshot image
    /// </summary>
    private Image m_image = null;

    /// <summary>
    /// unique id
    /// </summary>
    private Guid m_id = Guid.Empty;

    /// <summary>
    /// file path to image
    /// </summary>
    private string m_filename = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Consturctor from filename.
    /// </summary>
    /// <param name="filename">Image filename.</param>
    public Snapshot( string filename )
    {
      if( File.Exists( filename ))
      {
        using( Image image = Image.FromFile( filename ))
          m_image = new Bitmap( image );

        m_id = new Guid( Path.GetFileNameWithoutExtension( filename ));

        m_filename = filename;
      }
    } // Snapshot( filename )


    /// <summary>
    /// Consturctor from image.
    /// </summary>
    /// <param name="image">Snapshot image.</param>
    public Snapshot( Image image )
    {
      m_id = Guid.NewGuid();
      m_image = image;
    } // Snapshot( image )


    /// <summary>
    /// ISerializable constructor
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    protected Snapshot( SerializationInfo info, StreamingContext context )
    {
      m_id = (Guid) info.GetValue( "ID", typeof( Guid ));
      m_filename = info.GetString( "Filename" );

      using( MemoryStream stream = new MemoryStream( Convert.FromBase64String( info.GetString( "Image" ))))
        m_image = new Bitmap( Image.FromStream( stream ));
    } // Snapshot( info, context )

    #endregion

    #region properties


    /// <summary>
    /// Gets or sets the id.
    /// </summary>
    public Guid Id
    {
      get { return m_id; }
      set { m_id = value; }
    } // Id


    /// <summary>
    /// Gets or sets the image.
    /// </summary>
    public Image Image
    {
      get { return m_image; }
      set { m_image = value; }
    } // Image


    /// <summary>
    /// Gets or sets the snapshots filename.
    /// </summary>
    public string Filename
    {
      get { return m_filename; }
      set { m_filename = value; }
    } // Filename

    #endregion

    #region ISerializable Members

    /// <summary>
    /// Populates a SerializationInfo with the data needed to serialize the target object.
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    public void GetObjectData( SerializationInfo info, StreamingContext context )
    {
      info.AddValue( "ID", m_id );
      info.AddValue( "Filename", m_filename );

      using( MemoryStream stream = new MemoryStream())
      {
        m_image.Save( stream, System.Drawing.Imaging.ImageFormat.Png );
        info.AddValue( "Image", Convert.ToBase64String( stream.ToArray() ));
      }
    } // GetObjectData( info, context )

    #endregion

	} // class Snapshot
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: Snapshot.cs,v $
// Revision 1.1  2006/01/03 16:18:13  gdavidson
// Moved from Viatronix.v3D.Viewer.Core
//
// Revision 1.2  2005/10/11 14:05:02  gdavidson
// Cleaned up the snapshot object.
//
// Revision 1.1  2005/09/26 13:06:36  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/Snapshot.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
// $Id: Snapshot.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $

#endregion