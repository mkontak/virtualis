// $Id: Rect.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Ana Paula ana@viatronix.com


// Includes
#include "stdafx.h"
#include "Rect.h"


// Namespaces
using namespace std;

/**
 * Test function for the template class Rect<T>.
 * @param os output stream
 * @return bool bOk
 */
bool Rect<int4>::Test(ostream & os)
{
  bool bOk(true);
  

  bOk &= Rect<int4>::TestByType(os);
  bOk &= Rect<uint1>::TestByType(os);
  bOk &= Rect<float4>::TestByType(os);

  return (bOk);
} // end of Test


//$Log: Rect.C,v $
//Revision 1.3  2006/01/18 20:36:48  geconomos
//Initial effort to migrate to visual studio 2005 underway!
//
//Revision 1.2  2004/03/04 19:03:36  geconomos
//Commented out code to prevent linker error for 7.1 complier. Need to revisit this.
//
//Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
//Moved and restructured code from the old xp module. This helps 
//recompiling everything in one go and also reduces the compile
//time for the individual projects due to the new library structure.
//
//Revision 3.2  2003/05/16 13:14:52  frank
//formatting
//
//Revision 3.1  2003/05/16 11:44:21  michael
//code review
//
//Revision 3.0  2001/10/14 23:02:18  ingmar
//set revision of all files to 3.0 as part of pvcs to cvs move
//
//Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
//moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:18   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:18   ingmar
// Initial revision.
//Revision 1.1  2001/09/01 00:09:40  soeren
//renamed math directory
//
//Revision 1.6  2001/05/31 21:32:22  ana
//changes regarding code review
//
//Revision 1.5  2001/05/29 20:37:18  ana
//unsigned char test added
//
//Revision 1.4  2001/05/23 21:34:38  ana
//finished TestByType function
//
//Revision 1.3  2001/05/22 21:30:50  ana
//Created tests functions for Rect<T> class.
//
//
//$Header: /CVS/cvsRepository/V3D/src/vxBase/src/Rect.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $
//$Id: Rect.C,v 1.3 2006/01/18 20:36:48 geconomos Exp $
