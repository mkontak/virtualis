// $Id: Plane.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Ana Paula (ana@viatronix.com)


// Includes
#include "stdafx.h"
#include "Plane.h"
#include "Normal3D.h"

// Namespaces
using namespace std;


// $Log: Plane.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/23 12:35:31  frank
// Moved utility functions to PrimitiveUtilities
//
// Revision 3.9  2003/05/20 16:04:43  dongqing
// code review
//
// Revision 3.8  2003/05/14 19:33:58  frank
// code review
//
// Revision 3.7  2003/01/24 20:25:42  jmeade
// Inline method (GetProjectParallelToPlane) needs to be in the .inl file.
//
// Revision 3.6  2003/01/24 18:19:01  frank
// Removed Plane template parameter
//
// Revision 3.5  2003/01/22 20:58:27  ingmar
// added GetProjectionParallelToPlane()
//
// Revision 3.4  2001/01/02 15:35:25  ingmar
// removed unused tests
//
// Revision 3.3  2002/08/15 18:11:57  wenli
// Moved NormalizeCoefficients to inline file.
//
// Revision 3.2  2002/06/21 16:18:17  frank
// Normalized the coefficients.
//
// Revision 3.1.2.1  2002/05/28 20:25:04  frank
// Normalized the coefficients.
//
// Revision 3.1  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:14   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:10   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.1  2001/05/25 20:52:34  ana
// Plane.C added and TestByTyple method created
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Plane.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Plane.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
