#pragma once

#include "vxUrl.h"

namespace io
{



/**
 * Defines the transport layer for files
 */
class VX_BASE_DLL TransportLayer
{

#pragma region constructors

private:

  /// Constructor
  TransportLayer();

#pragma endregion

#pragma region methods

public:
  /// Initializes the transport layer 
  void Initialize(const std::string & sXml);

#pragma endregion

#pragma region properties

public:

  /// Returns true if the transport is set to the specified protocol
  bool IsProtocol(Protocols protocol)
  { return m_url.IsProtocol(protocol); }

  /// Gets the current TransportLayer
  static TransportLayer & GetCurrentTransportLayer();

  /// Gets the TransportLayer protocol
  const Url & GetUrl() const
  { return m_url; }

  void SetUrl(const Url & url)
  { m_url = url; }


  const uint4 GetCacheSize() const
  { return m_uCacheSize; }

  const void SetCacheSize(uint4 iCacheSize) 
  { m_uCacheSize = iCacheSize; }

#pragma endregion


#pragma region fields

private:

  /// Defines the url for this transport layer
  Url m_url;


  /// Buffering size
  uint4 m_uCacheSize;

#pragma endregion

};  // class TransportLayer


} // namespace io