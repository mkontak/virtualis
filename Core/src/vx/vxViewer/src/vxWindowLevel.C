// $Id: vxWindowLevel.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxWindowLevel.h"

// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"

/**
 * Default Constructor
 */
vxWindowLevel::vxWindowLevel():
//*******************************************************************
m_fWindow(25.01F/256.0F),
m_fLevel(65.7F/256.0F)
{
} // constructor


/**
 * Copy constructor
 * @param windowLevel
 */
vxWindowLevel::vxWindowLevel(const vxWindowLevel & other)
//*************************************************************
{
  if (this != &other)
  {
    SetWindow(other.GetWindow());
    SetLevel(other.GetLevel());
  }
} // copy constructor


/**
 * Assignment operator
 * @param windowLevel
 * @return const reference to this
 */
const vxWindowLevel & vxWindowLevel::operator=(const vxWindowLevel & other)
//*************************************************************
{
  if (this != &other)
  {
    SetWindow(other.GetWindow());
    SetLevel(other.GetLevel());
  }
  return *this;
} // vxWindowLevel()

#undef FILE_REVISION

// Revision History:
// $Log: vxWindowLevel.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/01/05 19:00:25  michael
// added "mailto:"
//
// Revision 1.5  2003/04/25 19:16:10  dongqing
// code review
//
// Revision 1.4  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.3  2003/04/24 13:58:14  michael
// initial code reviews
//
// Revision 1.2  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/06/10 15:18:12  dmitry
// Constructor moved to .C file
//
// Revision 1.1  2002/04/20 03:06:21  michael
// added source file of vxWindowLevel. Contains copy constructor and assignment operator
//
// $Id: vxWindowLevel.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxWindowLevel.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
