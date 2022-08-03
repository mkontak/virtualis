/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: include most dcmdata files that are usually required
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCTK_H
#define DCTK_H

#include "config/osconfig.h"    /* make sure OS specific configuration is included first */

// various headers
#include "dcmdata/dctypes.h"
#include "dcmdata/dcswap.h"
#include "dcmdata/dcistrma.h"
#include "dcmdata/dcostrma.h"
#include "dcmdata/dcvr.h"
#include "dcmdata/dcxfer.h"
#include "dcmdata/dcuid.h"
#include "dcmdata/dcvm.h"
#include "ofstd/ofdefine.h"

// tags and dictionary
#include "dcmdata/dctagkey.h"
#include "dcmdata/dctag.h"
#include "dcmdata/dcdicent.h"
#include "dcmdata/dchashdi.h"
#include "dcmdata/dcdict.h"
#include "dcmdata/dcdeftag.h"

// basis classes
#include "dcmdata/dcobject.h"
#include "dcmdata/dcelem.h"

// classes for management of sequences and other lists
#include "dcmdata/dcitem.h"
#include "dcmdata/dcmetinf.h"
#include "dcmdata/dcdatset.h"
#include "dcmdata/dcsequen.h"
#include "dcmdata/dcfilefo.h"
#include "dcmdata/dcdicdir.h"
#include "dcmdata/dcpixseq.h"

// element classes for string management (8-bit)
#include "dcmdata/dcbytstr.h"
#include "dcmdata/dcvrae.h"
#include "dcmdata/dcvras.h"
#include "dcmdata/dcvrcs.h"
#include "dcmdata/dcvrda.h"
#include "dcmdata/dcvrds.h"
#include "dcmdata/dcvrdt.h"
#include "dcmdata/dcvris.h"
#include "dcmdata/dcvrtm.h"
#include "dcmdata/dcvrui.h"

// element classes for string management (8-bit and/or 16-bit in later extensions)
#include "dcmdata/dcchrstr.h"
#include "dcmdata/dcvrlo.h"
#include "dcmdata/dcvrlt.h"
#include "dcmdata/dcvrpn.h"
#include "dcmdata/dcvrsh.h"
#include "dcmdata/dcvrst.h"
#include "dcmdata/dcvrut.h"

// element class for byte and word value representations
#include "dcmdata/dcvrobow.h"
#include "dcmdata/dcpixel.h"
#include "dcmdata/dcovlay.h"

// element classes for binary value fields
#include "dcmdata/dcvrat.h"
#include "dcmdata/dcvrss.h"
#include "dcmdata/dcvrus.h"
#include "dcmdata/dcvrsl.h"
#include "dcmdata/dcvrul.h"
#include "dcmdata/dcvrulup.h"
#include "dcmdata/dcvrfl.h"
#include "dcmdata/dcvrfd.h"
#include "dcmdata/dcvrof.h"

// misc supporting tools
#include "dcmdata/cmdlnarg.h"

#endif /* DCTK_H */

/*
 * CVS/RCS Log:
 * $Log: dctk.h,v $
 * Revision 1.18  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.17  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.16  2009-09-28 13:29:38  joergr
 * Moved general purpose definition file from module dcmdata to ofstd, and
 * added new defines in order to make the usage easier.
 *
 * Revision 1.15  2005/12/08 16:28:46  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.14  2005/11/28 15:28:56  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.13  2002/12/06 12:19:30  joergr
 * Added support for new value representation Other Float String (OF).
 *
 * Revision 1.12  2002/08/27 16:55:40  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.11  2001/06/01 15:48:45  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:19  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/02/29 11:48:38  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.8  1999/03/31 09:24:50  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
