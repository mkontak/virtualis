// $Id: vxPhaseInfo.h,v 1.1 2007/09/19 14:41:52 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Greg Davidson (gdavidson@viatronix.com)

#pragma once

// includes
#include "vxElement.h"


class VX_VIEWER_DLL vxPhaseInfo : public vxElement
{
public:

  /// constructor
  vxPhaseInfo();

  /// constructor
  vxPhaseInfo( int4 iIndex, int4 iCount, float4 fPercentage );

  /// get the phase index
  int4 GetIndex() { return m_iIndex; }

  /// Set the phase index
  void SetIndex( int4 iIndex ) { m_iIndex = iIndex; }

  /// get the total phase count
  int4 GetCount() { return m_iCount; }

  /// Set the total phase count
  void SetCount( int4 iCount ) { m_iCount = iCount; }

  /// get the phase percentage
  int4 GetPercentage() { return m_fPercentage; }

  /// Set the phase percentage
  void SetPercentage( float4 fPercentage ) { m_fPercentage = fPercentage; }

private:

  /// phase index
  int4 m_iIndex;

  /// total phase count
  int4 m_iCount;

  /// phase percentage
  float4 m_fPercentage;

}; // class vxPhaseInfo


// $Log: vxPhaseInfo.h,v $
// Revision 1.1  2007/09/19 14:41:52  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPhaseInfo.h,v 1.1 2007/09/19 14:41:52 gdavidson Exp $
// $Id: vxPhaseInfo.h,v 1.1 2007/09/19 14:41:52 gdavidson Exp $