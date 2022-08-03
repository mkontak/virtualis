// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLoggerInfo.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// includes
#include "vxLogEnums.h"
#include "vxLogRecord.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


// forward declarations
namespace xml
{
class VX_LOGGER_DLL XmlNode;
}


/**
 * Logger Info Class contains the base information for the logger
 */
class VX_LOGGER_DLL vxLoggerInfo
{

// member functions
public:

  /// Constructors
  vxLoggerInfo( const std::string & sHandle,
                const std::string & sFilename,
                const vxLogRecord::SeverityEnum eLogLevel = m_eDefaultLogLevel,
                const vxLogMessageFormats eFormat = vxLogMessageFormats::FORMAT_DEFAULT,
                const std::string & sServer = "",
                const unsigned int iPort = 0 ) ;

  /// Constructs the logger using the xml
  vxLoggerInfo(const std::string & sXml);

  /// Constructs the logger using the xml node
  vxLoggerInfo(const xml::XmlNode & node);

  /// Copy constructor
  vxLoggerInfo(const vxLoggerInfo & loggerInfo );


public:

  /// Overloaded == operator
  inline bool operator==(const vxLoggerInfo & rhs)
  { return Equals(rhs); }

  /// Overloaded == operator
  inline bool operator!=(const vxLoggerInfo & rhs)
  { return !Equals(rhs); }

  /// Equals
  inline bool Equals(const vxLoggerInfo & logger)
  { return ( const_cast<vxLoggerInfo &>(logger).m_sHandle == m_sHandle && const_cast<vxLoggerInfo &>(logger).m_sSystem == m_sSystem ? true : false ) ; }


private:

  /// Initializes the object using the xml string
  void Initialize(const std::string & sXml);

  /// Initializes the object using the xml node
  void Initialize(xml::XmlNode & node);


#pragma region properties

public:

  /// Gets the flag indicating if the server information is valid
  const bool IsServerInfoValid() const;

  /// Gets flag indicating is the logger is server based or not
  const bool IsServerBased() const;

  ///Set the LogLevel
  void SetLogLevel( const vxLogRecord::SeverityEnum & eSeverity )
  { m_eLogLevel = eSeverity; };

  ///Get the Log Level
  const vxLogRecord::SeverityEnum GetLogLevel() const
  { return m_eLogLevel; }

  /// Gets the system
  const std::string & GetSystem() const
  { return m_sSystem; }

  /// Sets the system
  void SetSystem(const std::string & sSystem)
  { m_sSystem = sSystem; }

  /// Get Name
  virtual const std::string & GetHandle() const
  { return m_sHandle; }


  ///Gets the Log File
  virtual std::string GetFilename() const
  { return m_sFilename; }

  /// Sets the file name
  virtual void SetFilename(const std::string & sFilename);

  /// Gets the port
  const unsigned int GetPort() const
  { return m_iPort; }

  /// Sets the port
  void SetPort(unsigned int iPort)
  { m_iPort = iPort; }

  /// Gets the server name/ip
  const std::string & GetServer() const
  { return m_sServer; }

  /// Sets the server name/ip
  void SetServer(const std::string & sServer) 
  { m_sServer = sServer; }

  
  static const std::string & GetDefaultFilename()
  { return m_sDefaultFilename; }

  static const vxLogRecord::SeverityEnum GetDefaultLogLevel()
  { return m_eDefaultLogLevel; }


#pragma endregion


protected:

  /// Extension
  std::string m_sExtension;

  /// Server
  std::string m_sServer;

  /// Port to send to
  unsigned int m_iPort;

  /// Origin of the logger 
  std::string m_sSystem;

  /// Log Level
  vxLogRecord::SeverityEnum m_eLogLevel;


  /// Filename
  std::string m_sFilename;

  /// Handle for logger
  std::string m_sHandle;


  /// Default file name
  static std::string m_sDefaultFilename;

  /// Default level
  static vxLogRecord::SeverityEnum m_eDefaultLogLevel;

  /// Type of message record
  vxLogMessageFormats m_eFormat;

};  // class vxLogger


#pragma warning(pop)


