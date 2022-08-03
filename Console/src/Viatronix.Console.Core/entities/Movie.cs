// $Id: Movie.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
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
	/// Summary description for Movie.
	/// </summary>
	[Serializable]
	public class Movie : ISerializable
	{

    #region fields

    /// <summary>
    /// the current version for the Movie object
    /// </summary>
    public const string Version = "1.0";

    /// <summary>
    /// path to movie file
    /// </summary>
    private string m_filename = string.Empty;

    /// <summary>
    /// path to movie file preview image
    /// </summary>
    private string m_previewFilename = string.Empty;

    /// <summary>
    /// loaded preview image
    /// </summary>
    private Image m_previewImage = null;

    /// <summary>
    /// readonly attribute
    /// </summary>
    private bool m_readonly = false;

    #endregion

    #region construction

    /// <summary>
    /// Default constructor.
    /// </summary>
		public Movie()
		{
			m_previewImage = new Bitmap( 1, 1 );
		} // Movie()


    /// <summary>
    /// ISerializable constructor
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    protected Movie( SerializationInfo info, StreamingContext context )
    {
      m_filename = info.GetString( "Filename" );
      m_previewFilename = info.GetString( "PreviewFilename" );
      m_readonly = info.GetBoolean( "ReadOnly" );

      using( MemoryStream stream = new MemoryStream( Convert.FromBase64String( info.GetString( "PreviewImage" ))))
        m_previewImage = new Bitmap( Image.FromStream( stream ));
    } // Snapshot( info, context )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the path to the movie file.
    /// </summary>
    public string Filename
    {
      get { return m_filename; }
      set { m_filename = value; }
    } // Filename


    /// <summary>
    /// Gets or sets the path to the movie file preview image
    /// </summary>
    public string PreviewFilename
    {
      get { return m_previewFilename; }
      set { m_previewFilename = value; }
    } // PreviewFilename


    /// <summary>
    /// Gets or set the loaded preview image
    /// </summary>
    public Image PreviewImage
    {
      get { return m_previewImage; }
      set { m_previewImage = value; }
    } // PreviewImage


    /// <summary>
    /// Gets or sets if the movie is readonly.
    /// </summary>
    public bool ReadOnly
    {
      get { return m_readonly; }
      set { m_readonly = value; }
    } // ReadOnly

    #endregion

    #region ISerializable Members

    /// <summary>
    /// Populates a SerializationInfo with the data needed to serialize the target object.
    /// </summary>
    /// <param name="info">The SerializationInfo to populate with data.</param>
    /// <param name="context">The destination for this serialization.</param>
    public void GetObjectData( SerializationInfo info, StreamingContext context )
    {
      info.AddValue( "Filename", m_filename );
      info.AddValue( "PreviewFilename", m_previewFilename );
      info.AddValue( "ReadOnly", m_readonly );

      using( MemoryStream stream = new MemoryStream() )
      {
        m_previewImage.Save( stream, System.Drawing.Imaging.ImageFormat.Png );
        info.AddValue( "PreviewImage", Convert.ToBase64String( stream.ToArray() ));
      }
    } // GetObjectData( info, context )

    #endregion

	} // class Movie
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: Movie.cs,v $
// Revision 1.1  2006/01/03 16:18:13  gdavidson
// Moved from Viatronix.v3D.Viewer.Core
//
// Revision 1.2  2005/10/06 17:06:49  gdavidson
// Added a ReadOnly property.
//
// Revision 1.1  2005/09/26 13:06:36  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/Movie.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
// $Id: Movie.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $

#endregion