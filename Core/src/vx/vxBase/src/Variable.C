// $Id: Variable.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#include "stdafx.h"
#include "Variable.h"


/**
 * Constructor
 */
Variable::Variable()
{
} // Variable()


/**
 * Destructor
 */
Variable::~Variable()
{
} // ~Variable()


/**
 * Copy constructor
 *
 * @param other the other variable
 */
Variable::Variable( const Variable & other )
{
  m_sName = other.m_sName;
  m_sValue = other.m_sValue;
  m_sDescription = other.m_sDescription;
} // Variable( other )


/**
 * Assignment operator
 *
 * @param other the other variable
 * @return a reference to itself
 */
Variable & Variable::operator=( const Variable & other )
{
  m_sName = other.m_sName;
  m_sValue = other.m_sValue;
  m_sDescription = other.m_sDescription;
  return * this;
} // operator =()


/** 
 * Equality operator
 *
 * @param other the other variable
 * @return true if ==
 */
bool Variable::operator ==(const Variable & other) const
{
  return ((m_sName == other.m_sName) && (m_sValue == other.m_sValue) && (m_sDescription == other.m_sDescription));
} // operator ==


/** 
 * Inequality operator
 *
 * @param other the other variable
 * @return true if !=
 */
bool Variable::operator !=( const Variable & other ) const
{
  return ! ( * this == other );
} // operator !=()


// $Log: Variable.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/20 16:28:52  frank
// code review
//
// Revision 3.2  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.1  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:54   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:46  soeren
// renamed image directory
//
// Revision 1.3  2001/07/02 16:28:49  frank
// Added inequality test.
//
// Revision 1.2  2001/06/19 19:08:24  frank
// Updated usage.
//
// Revision 1.1  2001/06/19 15:53:06  frank
// Initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Variable.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Variable.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
