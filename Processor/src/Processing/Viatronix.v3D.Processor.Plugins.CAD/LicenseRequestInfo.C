// $Id: LicenseRequestInfo.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "LicenseRequestInfo.h"


/**
 * Constructor
 */
LicenseRequestInfo::LicenseRequestInfo()
{
} // LicenseRequestInfo()

/**
 * Constructor from product id, user id, series id
 *
 * @param   sProductId    product id
 * @param   sUserId       user id
 * @param   sSeriesId     series id
 */
LicenseRequestInfo::LicenseRequestInfo( const std::string & sProductId, const std::string & sUserId, const std::string & sSeriesId ) : 
 m_sProductId( sProductId ), m_sUserId( sUserId ), m_sSeriesId( sSeriesId )
{
} // LicenseRequestInfo( const std::string & sProductId, const std::string & sUserId, const std::string & sSeriesId )


/**
 * Desturctor
 */
LicenseRequestInfo::~LicenseRequestInfo()
{
} // ~LicenseRequestInfo()


// $Log: LicenseRequestInfo.C,v $
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2006/01/16 15:42:28  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LicenseRequestInfo.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
// $Id: LicenseRequestInfo.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
