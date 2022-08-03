#include "Stdafx.h"
#include "DcmTraceListener.h"


/// <summary>
    /// Handles the event tracing
    /// </summary>
    /// <param name="eventCache">TraceEventCache</param>
    /// <param name="source">source string</param>
    /// <param name="eventType">event type</param>
    /// <param name="id">id</param>
    /// <param name="message">message</param>
void DcmTraceListener::TraceEvent( TraceEventCache ^ mpEventCache, String ^ msSource, TraceEventType eventType, int id, string message )
{
  TraceEvent( mpEventCache, source, eventType, id, message, string.Empty, string.Empty );
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
void DcmTraceListener::TraceEvent( TraceEventCache ^ mpEventCache, String ^ msSource, TraceEventType eventType, int id, string format, params object[] args )
{
  if ((this.Filter == null) || ShouldTrace( eventCache, source, eventType, id, format, args ))
  {
    DateTime time = (mpEventCache != null) ? eventCache.DateTime.ToLocalTime() : DateTime.Now;
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
bool DcmTraceListener::DcmTraceListener::ShouldTrace( TraceEventCache ^ mpEventCache, String ^ msSource, TraceEventType eventType, int id, string formatOrMessage, params object[] args )
{
  return this.Filter.ShouldTrace( mpEventCache, source, eventType, id, formatOrMessage, args, null, null );
} // ShouldTrace( cache, source, eventType, id, formatOrMessage, args )
