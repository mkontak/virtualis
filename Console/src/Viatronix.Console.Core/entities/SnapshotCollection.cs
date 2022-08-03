// $Id: SnapshotCollection.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
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
using System.Xml;

namespace Viatronix.Console.Entities
{

  public delegate void SnapshotCollectionHandler( int index, Snapshot snapshot );

	/// <summary>
	/// Summary description for SnapshotCollection.
	/// </summary>
	[Serializable]
	public class SnapshotCollection : CollectionBase
	{

    #region fields

    /// <summary>
    /// the current version of the SnapshotCollection object
    /// </summary>
    public const string Version = "2.0";

    /// <summary>
    /// collection of snapshots that have been deleted
    /// </summary>
    private ArrayList m_deletedSnapshots = null;

    /// <summary>
    /// raised when a snapshot is added to the collection
    /// </summary>
    public event SnapshotCollectionHandler Added = null;
  
    /// <summary>
    /// raised when a snapshot is removed from the collection
    /// </summary>
    public event SnapshotCollectionHandler Removed = null;

    #endregion

    #region construction

    /// <summary>
    /// Default consturctor
    /// </summary>
    public SnapshotCollection()
    {
      m_deletedSnapshots = new ArrayList();
    }


    /// <summary>
    /// Constructor from snapshots file.
    /// </summary>
    /// <param name="filename">Snapshot's filename.</param>
		public SnapshotCollection( string filename ) : this()
		{
      Read( filename );
		}

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the snapshot at the specified index.
    /// </summary>
    public Snapshot this[ int index ]
    {
      get { return (Snapshot) this.List[ index ]; }
      set { this.List[ index ] = value; }
    } // Indexer

    #endregion

    #region override methods

    /// <summary>
    /// Called after a snapshot has been inserted to the collection.
    /// </summary>
    /// <param name="index">Index of added item.</param>
    /// <param name="value">Instance of snapshot that was inserted.</param>
    protected override void OnInsertComplete( int index, object value )
    {
      base.OnInsertComplete( index, value );
      if( this.Added != null )
        this.Added( index, (Snapshot) value );
    } // OnInsertComplete( index, value )


    /// <summary>
    /// Called when a snapshot is removed from the collection.
    /// </summary>
    /// <param name="index">Index of removed item.</param>
    /// <param name="value">Instance of snapshot that was removed.</param>
    protected override void OnRemove( int index, object value )
    {
      base.OnRemove( index, value );
      
      ((Snapshot) value ).Image.Dispose();
      m_deletedSnapshots.Add( value );
    } // OnRemove( index, value )


    /// <summary>
    /// Called after a snapshot has been removed from the collection.
    /// </summary>
    /// <param name="index">Index of removed item.</param>
    /// <param name="value">Instance of snapshot that was removed.</param>
    protected override void OnRemoveComplete( int index, object value )
    {
      base.OnRemoveComplete( index, value );

      if( this.Removed != null )
        this.Removed( index, (Snapshot) value );
    } // OnRemoveComplete( index, value )

    #endregion

    #region methods


    /// <summary>
    /// Adds a Snapshot to the collection.
    /// </summary>
    /// <param name="snapshot">Snapshot to add</param>
    /// <returns>Index of snapshot</returns>
    public int Add( Snapshot snapshot )
    {
      return this.List.Add( snapshot );
    } // Add( snapshot )


    /// <summary>
    /// Inserts a Snapshot object at the specified index.
    /// </summary>
    /// <param name="index">Index to insert snasphot.</param>
    /// <param name="snapshot">Snapshot object to insert.</param>
    public void Insert( int index, Snapshot snapshot )
    {
      this.List.Insert( index, snapshot );
    } // Insert( index, snapshot )


    /// <summary>
    /// Removes the specified snapshot.
    /// </summary>
    /// <param name="snapshot">Snapshot object to remove.</param>
    public void Remove( Snapshot snapshot )
    {
      this.List.Remove( snapshot );
    } // Remove( snapshot )


    /// <summary>
    /// Indicates if the specified Snapshot is contained within the collection.
    /// </summary>
    /// <param name="snapshot">Snapshot object to check.</param>
    /// <returns>True if Snapshot is contained; false otherwise.</returns>
    public bool Contains( Snapshot snapshot )
    {
      return this.List.Contains( snapshot );
    } // Contains( snapshot )


    /// <summary>
    /// Writes out the snapshots to the snapshots file.
    /// </summary>
    /// <param name="filename">Snapshot file name.</param>
    public void Write( string filename )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // build the directory string to hold the snapshots
      DirectoryInfo snapshotDir = fileInfo.Directory;

      // delete snapshots
      foreach( Snapshot snapshot in m_deletedSnapshots )
      {
        FileInfo[] files = snapshotDir.GetFiles( snapshot.Id.ToString() + ".png" );
        foreach( FileInfo file in files )
          file.Delete();
      }

      XmlTextWriter writer = new XmlTextWriter( filename, System.Text.Encoding.UTF8 );
      writer.Formatting = Formatting.Indented;
      writer.WriteStartDocument( false );
      writer.WriteStartElement( "snapshots" );
      writer.WriteAttributeString( "version", SnapshotCollection.Version );

      // write out each snapshot element
      foreach( Snapshot snapshot in this.List )
      {
        writer.WriteStartElement( "snapshot" );
        writer.WriteAttributeString( "version", Snapshot.Version );

        string snapshotFile = snapshot.Id.ToString() + ".png";
        writer.WriteElementString( "filename", snapshotFile );
        writer.WriteEndElement();

        snapshot.Image.Save( Path.Combine( snapshotDir.FullName, snapshotFile ), System.Drawing.Imaging.ImageFormat.Png );
      }

      writer.WriteEndElement();
      writer.WriteEndDocument();

      writer.Flush();
      writer.Close();
    } // Write( filename )


    /// <summary>
    /// Reads all the snapshots from the snapshots file.
    /// </summary>
    /// <param name="filename">Snapshot file name.</param>
    public void Read( string filename )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.Directory.FullName ))
        return;

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version == "1.0" )
        ReadVersion1( doc, fileInfo );
      else if( version == "2.0" )
        ReadVersion2( doc, fileInfo );
      else
        throw new ArgumentException( "Invalid xml version for snapshot xml." ); 
    } // Read( filename )


    /// <summary>
    /// Reads all the snapshots from the snapshots file.
    /// </summary>
    /// <param name="filename">Snapshot file name.</param>
    /// <returns>The number of snapshots</returns>
    public static int GetSnapshotCount( string filename )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.Directory.FullName ))
        throw new FileNotFoundException( "The specified file does not exist [PATH=" + filename + "]" );

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version != "1.0" && version != "2.0" )
        throw new ArgumentException( "Invalid xml version for snapshot xml." ); 

      return doc.SelectNodes( "/snapshots/snapshot/filename" ).Count;
    } // GetSnapshotCount( filename )


    /// <summary>
    /// Reads all the snapshots from the snapshots file.
    /// </summary>
    /// <param name="filename">Snapshot file name.</param>
    /// <returns>The number of snapshots</returns>
    public static Snapshot GetSnapshot( string filename, int index )
    {
      FileInfo fileInfo = new FileInfo( filename );

      // bail if the diretory doesn't exist
      if( !Directory.Exists( fileInfo.Directory.FullName ))
        throw new FileNotFoundException( "The specified file does not exist [PATH=" + filename + "]" );

      XmlDocument doc = new XmlDocument();
      doc.Load( filename );

      string version = doc.DocumentElement.Attributes[ "version" ].Value;
      if( version != "1.0" && version != "2.0" )
        throw new ArgumentException( "Invalid xml version for snapshot xml." ); 

      XmlNodeList nodes = doc.SelectNodes( "/snapshots/snapshot/filename" );
      if( index >= nodes.Count )
        throw new IndexOutOfRangeException( "The requested index exceeds the number of possible snapshots." );

      if( version == "1.0" )
        return ReadVersion1( nodes[ index ], fileInfo.Directory.FullName );
      else
        return ReadVersion2( nodes[ index ], fileInfo.Directory.FullName );
    } // GetSnapshotCount( filename )


    /// <summary>
    /// Reads version "1.0" of the serialized data
    /// </summary>
    /// <param name="doc">XmlDocument containg serialized data</param>
    /// <param name="fileInfo">Full file path to serialized xml data.</param>
    private void ReadVersion1( XmlDocument doc, FileInfo fileInfo )
    {
      foreach( XmlNode node in doc.SelectNodes( "/snapshots/snapshot/filename" ))
        Add( ReadVersion1( node, fileInfo.DirectoryName ));
    } // ReadVersion1( doc, fileInfo )


    /// <summary>
    /// Reads version "2.0" of the serialized data
    /// </summary>
    /// <param name="doc">XmlDocument containg serialized data</param>
    /// <param name="fileInfo">Full file path to serialized xml data.</param>
    private void ReadVersion2( XmlDocument doc, FileInfo fileInfo )
    {
      foreach( XmlNode node in doc.SelectNodes( "/snapshots/snapshot/filename" ))
        Add( ReadVersion2( node, fileInfo.DirectoryName ));
    } // ReadVersion2( doc, fileInfo )


    /// <summary>
    /// Returns the snapshot described by the xml node.
    /// </summary>
    /// <param name="node">Xml description of the snapshot</param>
    /// <param name="directory">The snapshot directory.</param>
    /// <returns>The snapshot image.</returns>
    private static Snapshot ReadVersion1( XmlNode node, string directory )
    {
      string filename = node.InnerText;
      if( filename.StartsWith( "." + Path.DirectorySeparatorChar ))
        filename = Path.Combine( directory, filename.Replace( @".\snapshots", string.Empty ).TrimStart( Path.DirectorySeparatorChar ));

      return new Snapshot( filename );
    } // ReadVersion1( node, directory )

    
    /// <summary>
    /// Returns the snapshot described by the xml node.
    /// </summary>
    /// <param name="node">Xml description of the snapshot</param>
    /// <param name="directory">The snapshot directory.</param>
    /// <returns>The snapshot image.</returns>
    private static Snapshot ReadVersion2( XmlNode node, string directory )
    {
      return new Snapshot( Path.Combine( directory, node.InnerText ));
    } // ReadVersion2( node, directory )

    #endregion

	} // class SnapshotCollection
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: SnapshotCollection.cs,v $
// Revision 1.1  2006/01/03 16:18:13  gdavidson
// Moved from Viatronix.v3D.Viewer.Core
//
// Revision 1.3  2005/11/08 21:43:11  gdavidson
// Modified the implementation for the ReadVersion1 method
//
// Revision 1.2  2005/10/11 14:05:02  gdavidson
// Cleaned up the snapshot object.
//
// Revision 1.1  2005/09/26 13:06:36  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/SnapshotCollection.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $
// $Id: SnapshotCollection.cs,v 1.1 2006/01/03 16:18:13 gdavidson Exp $

#endregion