// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Diagnostics.Eventing;
using System.IO;

namespace Viatronix.Enterprise.Logging
{
  public sealed class Logger : IDisposable
  {

    #region fields

     /// <summary>
    /// trace source
    /// </summary>
    private TraceSource m_source = null;

    /// <summary>
    /// tracks the lifetime of the object
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Creates an instance of Logger
    /// </summary>
    /// <param name="source">log source</param>
    public Logger(TraceSource source)
    {
      m_source = source;
    } // Logger( source )

    /// <summary>
    /// Creates an instance of Logger
    /// </summary>
    /// <param name="source">log source</param>
    public Logger( string source )
    {
       m_source = new TraceSource( source );
    } // Logger( source )


    #endregion

    #region properties

    public string Name
    {
      get { return m_source.Name; }
    }

    /// <summary>
    /// Returns the collection of listeners
    /// </summary>
    public TraceListenerCollection Listeners
    {
      get { return m_source.Listeners; }
    } // Listeners

    #endregion

    #region methods

    /// <summary>
    /// Logs a critical message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public void Critical( string message, string className, string methodName )
    {
      TraceEvent( TraceEventType.Critical, 0, message, className, methodName );
    } // Critical( message, className, methodName )


    /// <summary>
    /// Logs an error message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public void Error( string message, string className, string methodName )
    {
      TraceEvent( TraceEventType.Error, 0, message, className, methodName );
    } // Error( message, className, methodName )


    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public void Debug( string message, string className, string methodName )
    {
      TraceEvent( TraceEventType.Verbose, 0, message, className, methodName );
    } // Debug( message, className, methodName )


    /// <summary>
    /// Logs a warning message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public void Warning( string message, string className, string methodName )
    {
      TraceEvent( TraceEventType.Warning, 0, message, className, methodName );
    } // Warning( message, className, methodName )


    /// <summary>
    /// Logs an information message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public void Information( string message, string className, string methodName )
    {
      TraceEvent( TraceEventType.Information, 0, message, className, methodName );
    } // Information( message, className, methodName )


    /// <summary>
    /// Logs a message
    /// </summary>
    /// <param name="type">log type</param>
    /// <param name="id">log id</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    private void TraceEvent( TraceEventType type, int id, string message, string className, string methodName )
    {
      m_source.TraceEvent( type, id, message, className, methodName );
    } // TraceEvent( type, id, message, className, methodName )


    /// <summary>
    /// Changes the TexListener's output
    /// </summary>
    /// <param name="source">id</param>
    /// <param name="fileName">log file name</param>
    public void ChangeLog( string source, string fileName )
    {
      ChangeLog( source, fileName, null );
    } // ChangeLog( source, fileName )


    /// <summary>
    /// Changes the TexListener's output
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="fileName">log file name</param>
    /// <param name="type">new SourceLevels or null</param>
    public void ChangeLog( string id, string fileName, SourceLevels? level )
    {
      TextWriterTraceListener listener = m_source.Listeners[ id ] as TextWriterTraceListener;
      if( listener == null )
        throw new InvalidCastException( string.Format( "Unknown TraceListener [{0}]", id ));

      Encoding encodingWithFallback = (Encoding) ( new UTF8Encoding(false) ).Clone();
      encodingWithFallback.EncoderFallback = EncoderFallback.ReplacementFallback;
      encodingWithFallback.DecoderFallback = DecoderFallback.ReplacementFallback;

      string fullPath = Path.GetFullPath( fileName );
      StreamWriter writer = new StreamWriter( fullPath, true, encodingWithFallback, 0x1000 );
      ChangeListener( listener, writer, ( level.HasValue ) ? new EventTypeFilter( level.Value ) : listener.Filter );
    } // ChangeLog( source, fileName, level )


    /// <summary>
    /// Changes the TexListener's output
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="stream">log stream</param>
    public void ChangeLog( string id, Stream stream )
    {
      ChangeLog( id, stream, null );
    } // ChangeLog( id, stream )


    /// <summary>
    /// Changes the TexListener's output
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="stream">log stream</param>
    /// <param name="level">new SourceLevels or null</param>
    public void ChangeLog( string id, Stream stream, SourceLevels? level )
    {
      TextWriterTraceListener listener = m_source.Listeners[ id ] as TextWriterTraceListener;
      if( listener == null )
        throw new InvalidCastException( string.Format( "Unknown TraceListener [{0}]", id ));

      ChangeListener( listener, new StreamWriter( stream ), ( level.HasValue ) ? new EventTypeFilter( level.Value ) : listener.Filter );
    } // ChangeLog( string source, Stream stream )


    /// <summary>
    /// Changes the log filter
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="level"></param>
    public void ChangeFilter( string id, SourceLevels level )
    {
      TextWriterTraceListener listener = m_source.Listeners[ id ] as TextWriterTraceListener;
      if( listener == null )
        throw new InvalidCastException( string.Format( "Unknown TraceListener [{0}]", id ));

      listener.Filter = new EventTypeFilter( level );
    } // ChangeFilter( id, level )


    /// <summary>
    /// Changes out the internal StreamWriter
    /// </summary>
    /// <param name="listener">TextWriterTraceListener</param>
    /// <param name="writer">new StreamWriter</param>
    /// <param name="level">new SourceLevels</param>
    private void ChangeListener( TextWriterTraceListener listener, StreamWriter writer, TraceFilter filter )
    {
      if( listener.Writer != null )
        listener.Writer.Close();

      listener.Writer = writer;
      listener.Filter = filter;
    } // ChangeListener( listener, writer, filter )

    #endregion

    #region IDisposable Members

    /// <summary>
    /// Handles the cleanup of resources
    /// </summary>
    public void Dispose()
    {
      Dispose( true );
      GC.SuppressFinalize( this );
    } // Dispose()


    /// <summary>
    /// Handles the cleanup of resources
    /// </summary>
    /// <param name="disposing">true if the object can dispose of internal managed objects; otherwise false</param>
    private void Dispose( bool disposing )
    {
      if( !m_disposed )
      {
        if( disposing )
        {
          if( m_source != null )
            m_source.Close();
          m_source = null;
        }

        m_disposed = true;
      }
    } // Dispose( disposing )

    #endregion

  } // class Logger
} // namespace Viatronix.Enterprise.Logging
