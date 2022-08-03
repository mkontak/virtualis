// $Id: ZeroOne.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ZeroOne.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)


// includes
#include "stdafx.h"
#include "ZeroOne.h"


/**
 * Assignment operator, specialized for T=ZeroOne
 * @param ZeroOne value
 * @return const reference to this
 */
template <>
const ZeroOne & ZeroOne<class ZeroOne>::operator=(const ZeroOne & v)
//*******************************************************************
{
  m_v = v.m_v;

  return *this;
}



// Revision history:
// $Log: ZeroOne.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.3  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.2  2002/10/16 22:21:38  michael
// fixed bug, operator=(const ZeroOne & obj) needs to assign obj.m_v!!!
//
// Revision 1.1  2002/10/14 15:13:25  michael
// should be used once we move to MS VC 7.0 compiler
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ZeroOne.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ZeroOne.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
