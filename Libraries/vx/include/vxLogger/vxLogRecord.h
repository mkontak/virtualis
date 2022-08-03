// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogRecord.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

// forward declarations
namespace xml
{
class VX_LOGGER_DLL XmlNode;
}

/**
 * class implements a logging record
 */
class VX_LOGGER_DLL vxLogRecord
{

public:

 // Severity/Log level
 enum VX_LOGGER_DLL SeverityEnum
 {
    SL_NONE        = 0,   // Tracing is not on
    SL_CRITICAL    = 1,   // Abnormal exit or termination
    SL_FATAL       = 1,   // Deprecated name for Abnormal exit or termination
    SL_ERROR       = 2,   // Severe errors that need logging
    SL_EXCEPTION   = 2,   // Same as error (Backward compatibility)
    SL_WARNING     = 3,   // Warnings such as allocation failure
    SL_INFORMATION = 4,   // Includes non-error cases(e.g.,Entry-Exit)
    SL_TIMING      = 5,   // Timing
    SL_DEBUG       = 6,   // Debug is used for backward compatibility
    SL_VERBOSE     = 6,   // Detailed traces from intermediate steps
    SL_TEMPORARY   = 6,   // Backward compatibility
    SL_HIGHEST     = 5    // Highest debugging ( Used for backward compatibility )
 };

 
public:

#pragma region constructors/destructors

  /// Default constructor
  vxLogRecord();

 /// Construct the loge record from the xml
  vxLogRecord( const std::string & sXml);

  /// COnstruct fron the xmo node
  vxLogRecord(const xml::XmlNode & pNode);

  /// Copy constructor
  vxLogRecord ( const vxLogRecord & record );

  /// Constructor
  vxLogRecord(  const std::string & sMessage, const std::string & sClassName, 
                const std::string & sMethod, const SeverityEnum eSeverity = SL_ERROR);


  /// constructor 
  vxLogRecord( const std::string & sFileName,  const int iLine,
               const std::string & sClassName, const std::string & sMethod, 
               const std::string & sMessage, const vxLogRecord::SeverityEnum eSeverity = SL_ERROR);

  /// Destructor
  virtual ~vxLogRecord() {}

#pragma endregion

  /// FOrmats the message
  std::string FormatMessage(vxLogMessageFormats eFormat);

  /// Creates the XML for the record
  std::string ToXml();


  /// converts the string to a severity enumerator value
  static SeverityEnum ToSeverityEnum(const std::string & sSeverity);

private:

  // Initializes the object using the xml
  void Initialize(const std::string & sXml);

  // INitializes the object using the xml node
  void Initialize(xml::XmlNode & node);

  /// Creates a non-xml representation of the record
  std::string ToString();

public:


  /// Gets the time stamp
  const std::string GetTimeStamp() const;

 /// returns the current severity level
  inline const SeverityEnum GetSeverityLevel() const 
  { return m_eSeverity; }

  /// returns a string representation of the severity level enumeration
  const std::string GetLevelAsString() const;

  /// returns a string representation of the severity level enumeration
  const std::string GetLevelAsSpacedString() const;

  /// Gets the class
  inline const std::string & GetClassName() const
  { return m_sClassName; }

  /// Gets the method
  inline const std::string & GetMethod() const
  { return m_sMethod; }

  /// Used for backward compatiblity
  inline const std::string & GetTextMessage() const
  { return m_sMessage; } 

  /// retrieves the line number of the sepecified file
  inline const int GetLine() const 
  { return m_iLine; }

    /// retrieves the process identification
  inline const int GetProcessId() const 
  { return m_iProcessId; }

  /// retrieves the thread identification
  inline const int GetThreadId() const 
  { return m_iThreadId; }

    /// retrieves the file name
  inline const std::string & GetFileName() const 
  { return m_sFileName; }

private:


  /// Log records severity
  SeverityEnum m_eSeverity;
 
  /// process identification
  int m_iProcessId;

  /// thread identification
  int m_iThreadId;

  /// line number in source code
  int m_iLine;

  /// file Name
  std::string m_sFileName;


  /// class name
  std::string m_sClassName;

  /// function name
  std::string m_sMethod;

  /// text message
  std::string m_sMessage;


  /// Used for event data (UTC)
  SYSTEMTIME m_time;

  /// Record delimiter
  static const char m_delimiter;


};  // class vxLogRecord


#pragma warning(pop)