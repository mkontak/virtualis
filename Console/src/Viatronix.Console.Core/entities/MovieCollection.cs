// $Id: MovieCollection.cs,v 1.4 2007/03/01 19:05:22 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Drawing;
using System.Xml;

namespace Viatronix.Console.Entities
{

  /// <summary>
  /// delegate for MovieCollection events
  /// </summary>
  public delegate void MovieEventHandler( Movie movie );

	/// <summary>
	/// Summary description for MovieCollection.
	/// </summary>
	[Serializable]
	public class MovieCollection : CollectionBase, IDisposable
	{

    #region fields

    /// <summary>
    /// the current version of the MovieCollection object
    /// </summary>
    public const string Version = "2.0";

    /// <summary>
    /// "preview unavailable" image
    /// </summary>
    private static readonly Image NotAvailableImage = null;

    /// <summary>
    /// collection of movies that have been deleted
    /// </summary>
    private ArrayList m_deletedMovies = null;

    /// <summary>
    /// collection of movies that have been added
    /// </summary>
    private ArrayList m_addedMovies = null;

    /// <summary>
    /// determines if the dispose method has already been called
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// raised when a movie is added to the collection
    /// </summary>
    public event MovieEventHandler Added = null;

    /// <summary>
    /// raised when a movie is removed from the collection
    /// </summary>
    public event MovieEventHandler Removed = null;

    /// <summary>
    /// raised when the collection is cleared
    /// </summary>
    public event EventHandler Cleared = null;

    /// <summary>
    /// raised when a movie is copied from the collection.
    /// </summary>
    public event EventHandler CopyProgress = null;

    #endregion

    #region construction

    static MovieCollection()
    {
      // create the image
      NotAvailableImage = new Bitmap( 90, 90, System.Drawing.Imaging.PixelFormat.Format24bppRgb );

      // create a graphics instance from the newly created bitmap
      using( Graphics gfx = Graphics.FromImage( NotAvailableImage ))
      {
        // clear it out with black
        gfx.Clear( Color.Black );

        // the text will be centered both horizontally and vertically
        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;

        // bounds for text rendering is the bitmap dimensions
        RectangleF bounds = new RectangleF( 0.0F, 0.0F, NotAvailableImage.Width, NotAvailableImage.Height );

        gfx.DrawString( "Preview Unavailable", Viatronix.UI.Theme.TabFont, Brushes.Gold, bounds, format );
      }
    } // MovieCollection()


    /// <summary>
    /// Default consturctor
    /// </summary>
    public MovieCollection()
    {
      m_deletedMovies = new ArrayList();
      m_addedMovies = new ArrayList();
    } // MovieCollection()


    /// <summary>
    /// Constructor from movies file.
    /// </summary>
    /// <param name="filename">Movie's filename.</param>
		public MovieCollection( string filename ) : this( filename, false )
		{
		} // MovieCollection( filename )


    /// <summary>
    /// Constructor from movies file.
    /// </summary>
    /// <param name="filename">Movie's filename.</param>
    public MovieCollection( string filename, bool readOnly ) : this()
    {
      Read( filename, readOnly );
    } // MovieCollection( filename )


    /// <summary>
    /// Finalizer
    /// </summary>
    ~MovieCollection()
    {
      Dispose( false );
    } // ~MovieCollection()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the Movie at the specified index.
    /// </summary>
    public Movie this[ int index ]
    {
      get { return (Movie) this.List[ index ]; }
      set { this.List[ index ] = value; }
    } // Indexer


    /// <summary>
    /// Returns the colection of added movies
    /// </summary>
    public ArrayList AddedMovies
    {
      get { return m_addedMovies; }
    } // AddedMovies

    #endregion

    #region override methods

    /// <summary>
    /// Called after a movie has been inserted to the collection.
    /// </summary>
    /// <param name="index">Index of added item.</param>
    /// <param name="value">Instance of movie that was inserted.</param>
    protected override void OnInsertComplete( int index, object value )
    {
      base.OnInsertComplete( index, value );

      Movie movie = (Movie) value;
      m_addedMovies.Add( movie );

      if( this.Added != null )
        this.Added( movie );
    } // OnInsertComplete( index, value )


    /// <summary>
    /// Called when a movie is removed from the collection.
    /// </summary>
    /// <param name="index">Index of removed item.</param>
    /// <param name="value">Instance of movie that was removed.</param>
    protected override void OnRemove( int index, object value )
    {
      base.OnRemove( index, value );
      
      m_deletedMovies.Add( value );
    } // OnRemove( index, value )


    /// <summary>
    /// Called after a movie has been removed from the collection.
    /// </summary>
    /// <param name="index">Index of removed item.</param>
    /// <param name="value">Instance of movie that was removed.</param>
    protected override void OnRemoveComplete( int index, object value )
    {
      base.OnRemoveComplete( index, value );

      if( this.Removed != null )
        this.Removed( (Movie) value );
    } // OnRemoveComplete( index, value )


    /// <summary>
    /// Called when the collection is cleared.
    /// </summary>
    protected override void OnClearComplete()
    {
      base.OnClearComplete();

      if( this.Cleared != null )
        this.Cleared( this, EventArgs.Empty );
    } // OnClearComplete()



    #endregion

    #region methods


    /// <summary>
    /// Adds a Movie to the collection.
    /// </summary>
    /// <param name="movie">Movie to add</param>
    /// <returns>Index of movie</returns>
    public int Add( Movie movie )
    {
      return this.List.Add( movie );
    } // Add( movie )


    /// <summary>
    /// Inserts a Movie object at the specified index.
    /// </summary>
    /// <param name="index">Index to insert snasphot.</param>
    /// <param name="movie">Movie object to insert.</param>
    public void Insert( int index, Movie movie )
    {
      this.List.Insert( index, movie );
    } // Insert( index, movie )


    /// <summary>
    /// Removes the specified movie.
    /// </summary>
    /// <param name="movie">Movie object to remove.</param>
    public void Remove( Movie movie )
    {
      this.List.Remove( movie );
    } // Remove( movie )


    /// <summary>
    /// Indicates if the specified Movie is contained within the collection.
    /// </summary>
    /// <param name="movie">Movie object to check.</param>
    /// <returns>True if Movie is contained; false otherwise.</returns>
    public bool Contains( Movie movie )
    {
      return this.List.Contains( movie );
    } // Contains( movie )


    /// <summary>
    /// Writes out the snapshots to the snapshots file.
    /// </summary>
    /// <param name="filename">Movie file name.</param>
    public void Write( string filename )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // delete movies
      foreach( Movie movie in m_deletedMovies )
      {
        if( movie.ReadOnly )
          continue;

        if( File.Exists( movie.PreviewFilename ))
          File.Delete( movie.PreviewFilename );

        if( File.Exists( movie.Filename ))
          File.Delete( movie.Filename );
      }

      XmlTextWriter writer = new XmlTextWriter( filename, System.Text.Encoding.UTF8 );
      writer.Formatting = Formatting.Indented;
      writer.WriteStartDocument( false );
      writer.WriteStartElement( "movies" );
      writer.WriteAttributeString( "version", MovieCollection.Version );

      // write out each movie element
      foreach( Movie movie in this.List )
      {
        FileInfo fileInfoMovie = new FileInfo( movie.Filename );
        FileInfo previewInfoMovie = new FileInfo( movie.PreviewFilename );

        writer.WriteStartElement( "movie" );
        writer.WriteAttributeString( "version", Movie.Version );
        writer.WriteElementString( "filename", fileInfoMovie.Name );
        writer.WriteElementString( "preview_filename", previewInfoMovie.Name );
        writer.WriteEndElement();
      }

      writer.WriteEndElement();
      writer.WriteEndDocument();

      writer.Flush();
      writer.Close();

      m_addedMovies.Clear();
    } // Write( filename )


    /// <summary>
    /// Reads all the snapshots from the snapshots file.
    /// </summary>
    /// <param name="filename">Movie file name.</param>
    public void Read( string filename )
    {
      Read( filename, false );
    } // Read( filename )


    /// <summary>
    /// Reads all the snapshots from the snapshots file.
    /// </summary>
    /// <param name="filename">Movie file name.</param>
    /// <param name="readonly">Marks the movies as readonly.</param>
    public void Read( string filename, bool readOnly )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.DirectoryName ))
        return;

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version == "1.0" )
        ReadVersion1( doc, fileInfo, readOnly );
      else if( version == "2.0" )
        ReadVersion2( doc, fileInfo, readOnly );
      else
        throw new ArgumentException( "Invalid xml version for movie xml." ); 

      // reset the added movies array
      m_addedMovies.Clear();
    } // Read( filename )


    /// <summary>
    /// Returns the number of movies defined in the file.
    /// </summary>
    /// <param name="filename">The movie xml file.</param>
    /// <returns>The number of movies.</returns>
    public static int GetMovieCount( string filename )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.DirectoryName ))
        throw new FileNotFoundException( "The specified file does not exist [PATH=" + filename + "]" );

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version != "1.0" && version != "2.0" )
        throw new ArgumentException( "Invalid xml version for movie xml." ); 

      return doc.SelectNodes( "/movies/movie" ).Count;
    } // GetMovieCount( filename )


    /// <summary>
    /// Returns the movie at the specified location.
    /// </summary>
    /// <param name="filename">The movie xml file.</param>
    /// <param name="index">THe index of the movie.</param>
    /// <returns>A movie object.</returns>
    public static Movie GetMovie( string filename, int index )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.DirectoryName ))
        throw new FileNotFoundException( "The specified file does not exist [PATH=" + filename + "]" );

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version != "1.0" && version != "2.0" )
        throw new ArgumentException( "Invalid xml version for movie xml." ); 

      XmlNodeList nodes = doc.SelectNodes( "/movies/movie" );
      if( index >= nodes.Count )
        throw new IndexOutOfRangeException( "The requested index exceeds the number of possible movies." );

      if( version == "1.0" )
        return ReadVersion1( nodes[ index ], fileInfo.Directory.Parent.FullName, false );
      else
        return ReadVersion2( nodes[ index ], fileInfo.Directory.FullName, false );
    } // GetMovie( filename, index )


    /// <summary>
    /// Reads version "1.0" of the serialized data
    /// </summary>
    /// <param name="doc">XmlDocument containg serialized data</param>
    /// <param name="fileInfo">Full file path to serialized xml data.</param>
    /// <param name="readonly">Marks the movies as readonly.</param>
    private void ReadVersion1( XmlDocument doc, FileInfo fileInfo, bool readOnly )
    {
      foreach( XmlNode node in doc.SelectNodes( "/movies/movie" ))
        Add( ReadVersion1( node, fileInfo.Directory.Parent.FullName, readOnly ));
    } // ReadVersion1( doc, fileInfo )


    /// <summary>
    /// Returns the movie object described in the xml.
    /// </summary>
    /// <param name="node">Movie xml.</param>
    /// <param name="directory">The movie's directory</param>
    /// <param name="readOnly">true if the movie is read only; otherwise false</param>
    /// <returns>A movie object</returns>
    private static Movie ReadVersion1( XmlNode node, string directory, bool readOnly )
    {
      string filename = node.SelectSingleNode( "filename" ).InnerText;
      string previewFilename = node.SelectSingleNode( "preview_filename" ).InnerText;

      // expand relative paths
      if( filename.StartsWith( "." + Path.DirectorySeparatorChar ))
      {
        filename = Path.Combine( directory, filename.Remove( 0, 2 ));
        previewFilename = Path.Combine( directory, previewFilename.Remove( 0, 2 ));
      }

      return CreateMovie( filename, previewFilename, readOnly );
    } // ReadVersion1( node, directory, readOnly )


    /// <summary>
    /// Reads version "2.0" of the serialized data
    /// </summary>
    /// <param name="doc">XmlDocument containg serialized data</param>
    /// <param name="fileInfo">Full file path to serialized xml data.</param>
    /// <param name="readonly">Marks the movies as readonly.</param>
    private void ReadVersion2( XmlDocument doc, FileInfo fileInfo, bool readOnly )
    {
      foreach( XmlNode node in doc.SelectNodes( "/movies/movie" ))
        Add( ReadVersion2( node, fileInfo.DirectoryName, readOnly ));
    } // ReadVersion2( doc, fileInfo )


    /// <summary>
    /// Returns the movie object described in the xml.
    /// </summary>
    /// <param name="node">Movie xml.</param>
    /// <param name="directory">The movie's directory</param>
    /// <param name="readOnly">true if the movie is read only; otherwise false</param>
    /// <returns>A movie object</returns>
    private static Movie ReadVersion2( XmlNode node, string directory, bool readOnly )
    {
      string filename = Path.Combine( directory, node.SelectSingleNode( "filename" ).InnerText );
      string previewFilename = Path.Combine( directory, node.SelectSingleNode( "preview_filename" ).InnerText );

      return CreateMovie( filename, previewFilename, readOnly );
    } // ReadVersion2( node, directory, readOnly )


    /// <summary>
    /// Adds a new Movie instance to the collection from the specified  movie and prview filenames.
    /// </summary>
    /// <param name="filename">Full file path of movie file.</param>
    /// <param name="previewFilename">Full file path of preview image</param>
    private static Movie CreateMovie( string filename, string previewFilename, bool readOnly )
    {
      Movie movie = new Movie();
      movie.Filename = filename;
      movie.PreviewFilename = previewFilename;
      movie.ReadOnly = readOnly;

      if( File.Exists( movie.PreviewFilename ))
      {
        using( Image image = Image.FromFile( movie.PreviewFilename ))
          movie.PreviewImage = (Image) image.Clone();
      }
      else
        movie.PreviewImage = NotAvailableImage;

      return movie;
    } // AddMovie( filename, previewFilename )


    /// <summary>
    /// Copies the movies to the destination directory.
    /// </summary>
    /// <param name="directory">Destination directory.</param>
    public void Copy( string directory )
    {
      // delete movies
      foreach( Movie movie in this.List )
      {
        if( m_deletedMovies.Contains( movie ))
          continue;

        string destination = Path.Combine( directory, Path.GetFileName( movie.Filename ));
        if( !File.Exists( destination ))
          File.Copy( movie.Filename, destination, true );

        destination = Path.Combine( directory, Path.GetFileName( movie.PreviewFilename ));
        if( !File.Exists( destination ))
          File.Copy( movie.PreviewFilename, destination, true );

        if( this.CopyProgress != null )
          this.CopyProgress( this, EventArgs.Empty );
      }
    } // CopyMovies( directory )


    #endregion

    #region IDisposable Members

    /// <summary>
    /// Cleans up resources
    /// </summary>
    public void Dispose()
    {
      Dispose( true );
      GC.SuppressFinalize( this );
    } // Dispose()


    /// <summary>
    /// Frees up resources
    /// </summary>
    /// <param name="disposing">true if freeing managed resources</param>
    protected virtual void Dispose( bool disposing )
    {
      if( m_disposed )
      {
        if( disposing )
        {
          foreach( Movie movie in m_addedMovies )
          {
            if( File.Exists( movie.PreviewFilename ))
              File.Delete( movie.PreviewFilename );

            if( File.Exists( movie.Filename ))
              File.Delete( movie.Filename );
          }
        }
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class MovieCollection
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: MovieCollection.cs,v $
// Revision 1.4  2007/03/01 19:05:22  gdavidson
// Commented code
//
// Revision 1.3  2006/04/20 19:45:45  gdavidson
// Added AddedMovies property
//
// Revision 1.2  2006/01/09 19:44:57  gdavidson
// FIxed bug when reading v2.0 of a movie collection
//
// Revision 1.1  2006/01/03 16:18:13  gdavidson
// Moved from Viatronix.v3D.Viewer.Core
//
// Revision 1.4  2005/11/22 03:23:59  gdavidson
// Modified how version 1 movie xml is read.
//
// Revision 1.3  2005/10/18 17:56:52  gdavidson
// Added Cleared event to the MovieCollection.
//
// Revision 1.2  2005/10/06 17:06:29  gdavidson
// Added a Copy method.
//
// Revision 1.1  2005/09/26 13:06:36  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/MovieCollection.cs,v 1.4 2007/03/01 19:05:22 gdavidson Exp $
// $Id: MovieCollection.cs,v 1.4 2007/03/01 19:05:22 gdavidson Exp $

#endregion