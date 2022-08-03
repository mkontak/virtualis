// $Id: vxController.C,v 1.2 2004/05/20 12:45:12 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxController.h"


/**
 * constructor
 *
 * @param environment the environment
 */
vxController::vxController( vxEnvironment & environment ) :
m_pEnvironment( & environment )
{
} // vxController()


// $Log: vxController.C,v $
// Revision 1.2  2004/05/20 12:45:12  frank
// added get and set environment methods
//
// Revision 1.1  2004/05/19 15:47:08  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxController.C,v 1.2 2004/05/20 12:45:12 frank Exp $
// $Id: vxController.C,v 1.2 2004/05/20 12:45:12 frank Exp $
