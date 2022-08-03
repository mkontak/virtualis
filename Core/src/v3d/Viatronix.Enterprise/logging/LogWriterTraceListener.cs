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
using System.Globalization;
using System.IO;

namespace Viatronix.Enterprise.Logging
{
  public class LogWriterTraceListener : TextWriterTraceListener
  {

    #region fields

    /// <summary>
    /// header portion
    /// </summary>
    private const string m_header = "<{0} source=\"{1}\" date=\"{2}\" ";

    /// <summary>
    /// middle portion
    /// </summary>
    private const string m_middle = "class=\"{0}\" method=\"{1}\" >";

    /// <summary>
    /// tail portion
    /// </summary>
    private const string m_tail = "<![CDATA[{0}]]></{1}>";
    
    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="stream">stream</param>
    public LogWriterTraceListener( Stream stream ) : base( stream )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="writer">TextWriter</param>
    public LogWriterTraceListener( TextWriter writer ) : base( writer )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="filename">file name</param>
    public LogWriterTraceListener( string filename ) : base( Environment.ExpandEnvironmentVariables( filename ))
    {
      string file = Path.GetFullPath( filename );
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="stream">stream</param>
    /// <param name="name">name</param>
    public LogWriterTraceListener( Stream stream, string name ) : base( stream, name )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="writer">TextWriter</param>
    /// <param name="name">name</param>
    public LogWriterTraceListener( TextWriter writer, string name ) : base( writer, name )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="filename">file name</param>
    /// <param name="name">name</param>
    public LogWriterTraceListener( string filename, string name ) : base( Environment.ExpandEnvironmentVariables( filename ), name )
    {
    }

    #endregion

    #region methods

    /// <summary>
    /// Handles the event tracing
    /// </summary>
    /// <param name="eventCache">TraceEventCache</param>
    /// <param name="source">source string</param>
    /// <param name="eventType">event type</param>
    /// <param name="id">id</param>
    /// <param name="message">message</param>
    public override void TraceEvent( TraceEventCache eventCache, string source, TraceEventType eventType, int id, string message )
    {
      TraceEvent( eventCache, source, eventType, id, message, string.Empty, string.Empty );
    } // TraceEvent( eventCache, source, eventType, id, message )


    /// <summary>
    /// Handles the event tracing
    /// </summary>
    /// <param name="eventCache">TraceEventCache</param>
    /// <param name="source">source string</param>
    /// <param name="eventType">event type</param>
    /// <param name="id">id</param>
    /// <param name="format">string format message</param>
    /// <param name="args">format args</param>
    public override void TraceEvent( TraceEventCache eventCache, string source, TraceEventType eventType, int id, string format, params object[] args )
    {
      if ((this.Filter == null) || ShouldTrace( eventCache, source, eventType, id, format, args ))
      {
        DateTime time = (eventCache != null) ? eventCache.DateTime.ToLocalTime() : DateTime.Now;
        string type = ( eventType == TraceEventType.Verbose ) ? "debug" : eventType.ToString().ToLower();

        this.Write( string.Format( CultureInfo.InvariantCulture, m_header, type, source, time.ToString( "o", CultureInfo.InvariantCulture )));
        this.Write( string.Format( CultureInfo.InvariantCulture, m_middle, args ));
        this.WriteLine( string.Format( CultureInfo.InvariantCulture, m_tail, format, type ));
      }
    } // TraceEvent( eventCache, source, eventType, id, format, args )


    /// <summary>
    /// Determines if tracing should happen
    /// </summary>
    /// <param name="cache">TraceEventCache</param>
    /// <param name="source">source string</param>
    /// <param name="eventType">event type</param>
    /// <param name="id">id</param>
    /// <param name="formatOrMessage">string format message</param>
    /// <param name="args">format args</param>
    /// <returns>true if tracing should happen; otherwise false</returns>
    private bool ShouldTrace( TraceEventCache cache, string source, TraceEventType eventType, int id, string formatOrMessage, params object[] args )
    {
      return this.Filter.ShouldTrace( cache, source, eventType, id, formatOrMessage, args, null, null );
    } // ShouldTrace( cache, source, eventType, id, formatOrMessage, args )

    
    #endregion

  } // class LogWriterTraceListener
} // namespace Viatronix.Enterprise.Logging
