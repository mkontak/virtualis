// $Id: vxPhaseInfo.C,v 1.1 2007/09/19 14:41:52 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Greg Davidson (gdavidson@viatronix.com)

#include "StdAfx.h"
#include "vxPhaseInfo.h"


/**
 * constructor
 */
vxPhaseInfo::vxPhaseInfo() :
  m_iIndex( 0 ),
  m_iCount( 0 ),
  m_fPercentage( 0.0F )
{
}


/**
 * constructor
 *
 * @param           iIndex        phase index
 * @param           iCount        total phase count
 * @param           fPercentage        phase percentage
 */
vxPhaseInfo::vxPhaseInfo( int4 iIndex, int4 iCount, float4 fPercentage ) :
  m_iIndex( iIndex ),
  m_iCount( iCount ),
  m_fPercentage( fPercentage )
{
}

 
// $Log: vxPhaseInfo.C,v $
// Revision 1.1  2007/09/19 14:41:52  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxPhaseInfo.C,v 1.1 2007/09/19 14:41:52 gdavidson Exp $
// $Id: vxPhaseInfo.C,v 1.1 2007/09/19 14:41:52 gdavidson Exp $