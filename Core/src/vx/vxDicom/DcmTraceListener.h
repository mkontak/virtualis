#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;

ref class DcmTraceListener : TextWriterTraceListener
{


    #region construction

    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="stream">stream</param>
    public DcmTraceListener( Stream stream ) : TextWriterTraceListener( stream )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="writer">TextWriter</param>
    public DcmTraceListener( TextWriter writer ) : TextWriterTraceListener( writer )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="filename">file name</param>
    public DcmTraceListener( String ^ msFilename ) : TextWriterTraceListener( Environment.ExpandEnvironmentVariables( msFilename ))
    {
      string file = Path.GetFullPath( msFilename );
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="stream">stream</param>
    /// <param name="name">name</param>
    public DcmTraceListener( Stream stream, String ^ msName ) : TextWriterTraceListener( stream, msName )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="writer">TextWriter</param>
    /// <param name="name">name</param>
    public DcmTraceListener( TextWriter writer, String ^ msName ) : TextWriterTraceListener( writer, msName )
    {
    }


    /// <summary>
    /// Creates a new instance of LogWriterTraceListener
    /// </summary>
    /// <param name="filename">file name</param>
    /// <param name="name">name</param>
    public DcmTraceListener( String ^ msFilename, String ^ msName ) : TextWriterTraceListener( Environment::ExpandEnvironmentVariables( msFilename ), msName )
    {
    }

    #endregion

#pragma region methods

public:

  /// Handles the event tracing
  override void TraceEvent( TraceEventCache ^ mpEvenetCache, String ^ msSource, TraceEventType eventType, int id, String ^ msMessage );
 

  /// Handles the event tracing
  override void TraceEvent( TraceEventCache ^ mpEvenetCache, String ^ msSource, TraceEventType eventType, int id, String ^ msFormat, params object[] args );


private:

  /// Determines if tracing should happen
  bool ShouldTrace( TraceEventCache ^ mpEvenetCache, String ^ msSource, TraceEventType eventType, int id, String ^ msFormatOrMessage, params object[] args );

#pragma endregion

#pragma region fields

private:

  /// <summary>
  /// header portion
  /// </summary>
  const System::String ^ m_msHeader = "<{0} source=\"{1}\" date=\"{2}\" ";

  /// <summary>
  /// middle portion
  /// </summary>
  const System::String ^  m_msMiddle = "class=\"{0}\" method=\"{1}\" >";

  /// <summary>
  /// tail portion
  /// </summary>
  const System::String ^ m_msTail = "<![CDATA[{0}]]></{1}>";
    
#pragma endregion

} // class DicomTraceListener


