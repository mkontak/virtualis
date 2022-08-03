// $Id: vxModifiedInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxModifiedInfo.h"


/**
 * Constructor
 * @param typeinfo information
 * @param id
 * @param uFlag modified information
 * @param text
 */
vxModifiedInfo::vxModifiedInfo(const type_info & typeinfo, 
                               const vxID & id, 
                               uint4 uFlag, 
                               std::string sText):
m_pSender(NULL),
m_typeinfo(typeinfo),
m_id(id),
m_uFlag(uFlag),
m_sText(sText)
//*******************************************************************
{
} // vxModifiedInfo


/**
 * Copy Constructor
 * @param other instance
 */
vxModifiedInfo::vxModifiedInfo(const vxModifiedInfo & other):
m_pSender(other.m_pSender),
m_typeinfo(other.m_typeinfo),
m_uFlag(other.m_uFlag),
m_sText(other.m_sText),
m_id(other.m_id)
//*******************************************************************
{
} // vxModifiedInfo


/**
 * Creates a copy of this object on the heap
 * @param ppObject new object
 */
void vxModifiedInfo::Clone(vxModifiedInfo ** ppObject) const 
//*******************************************************************
{
  (*ppObject) = new vxModifiedInfo(*this);
} // Clone()


/**
 * Appends the modified information flag
 * @param uFlag modified information
 */
void vxModifiedInfo::AddFlag(const uint4 uFlag) 
//*******************************************************************
{
  m_uFlag |= uFlag;
}


/**
 * Constructor
 * @param client type information
 * @param index of the modified object
 * @param id
 * @param uFlag modified information.
 */
vxModifiedIndexedInfo::vxModifiedIndexedInfo(const type_info & typeinfo, 
                                             int4 iIndex, 
                                             const vxID & id, 
                                             uint4 uFlag):
vxModifiedInfo(typeinfo, id),
m_iIndex(iIndex)
//*******************************************************************
{
  AddFlag(uFlag);
} // vxModifiedIndexedInfo


/**
 * Copy Constructor
 * @param other instance
 */
vxModifiedIndexedInfo::vxModifiedIndexedInfo(const vxModifiedIndexedInfo & other):
vxModifiedInfo((vxModifiedInfo)other),
m_iIndex(other.m_iIndex)
//*******************************************************************
{
} // vxModifiedIndexedInfo


/**
 * Creates a copy of this object on the heap
 * @param ppObject new object
 */
void vxModifiedIndexedInfo::Clone(vxModifiedInfo ** ppObject) const 
//*******************************************************************
{
  (*ppObject) = new vxModifiedIndexedInfo(*this);
}


// Revision History:
// $Log: vxModifiedInfo.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/06/18 15:19:52  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.4  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.3  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.2  2002/08/05 16:20:07  dmitry
// Text information added to ModifiedInfo.
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.5  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.4  2002/06/07 20:15:21  dmitry
// vxModifiedIDInfo class added
//
// Revision 1.3  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxModifiedInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxModifiedInfo.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $