// $Id: LicenseRequestInfo.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#pragma once

/// class declaration
class LicenseRequestInfo
{
/// member functions
public:
  
  /// constructor
  LicenseRequestInfo();
  
  /// constructor from product id, user id, series id
  LicenseRequestInfo( const std::string & sProductId, const std::string & sUserId, const std::string & sSeriesId );

  /// destructor
  virtual ~LicenseRequestInfo();

  /// gets the product id
  const std::string & GetProductId() const { return m_sProductId; }

  /// sets the product id
  void SetProductId( const std::string & sProductId ){ m_sProductId = sProductId; }

  /// gets the user id
  const std::string & GetUserId() const { return m_sUserId; }

  /// sets the user id
  void SetUserId( const std::string & sUserId ){ m_sUserId = sUserId; }

  /// gets the series id
  const std::string & GetSeriesId() const { return m_sSeriesId; }

  /// sets the series id
  void SetSeriesId( const std::string & sSeriesId ){ m_sSeriesId = sSeriesId; }

/// memeber variables
private:

  /// product id
  std::string m_sProductId;

  /// user id
  std::string m_sUserId;

  /// series id
  std::string m_sSeriesId;

}; // class LicenseRequestInfo


// $Log: LicenseRequestInfo.h,v $
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2006/01/16 15:42:28  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LicenseRequestInfo.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
// $Id: LicenseRequestInfo.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
