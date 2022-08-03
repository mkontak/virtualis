// $Id: MeasureLine.C,v 1.3 2006/10/04 04:15:57 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "MeasureLine.h"


/**
 * Constructor
 */
MeasureLine::MeasureLine() : m_ePosn(START), m_f3Ddistance(0)
{
	m_bIsCameraInfoAvailable = true;
}


/**
 * Copy constructor
 *
 * @param src   Source.
 */
MeasureLine::MeasureLine(const MeasureLine & src)
{
  *this = src;
}


/**
 * Stores the euclidean distance.
 *
 * @return const float  Distance stored.
 */
const float MeasureLine::StoreDistance()
{
  m_f3Ddistance = m_measureEnds.m_start.GetEuclideanDistance(m_measureEnds.m_end);
  return m_f3Ddistance;
}


/**
 * Copy operator.
 *
 * @param  src             Source.
 * @return MeasureLine &   *this.
 */
MeasureLine & MeasureLine::operator=(const MeasureLine & src)
{
  m_measureEnds.m_start = src.m_measureEnds.m_start;    m_measureEnds.m_end = src.m_measureEnds.m_end;
  m_f3Ddistance = src.m_f3Ddistance;
  m_drawEnds.m_start = src.m_drawEnds.m_start;          m_drawEnds.m_end = src.m_drawEnds.m_end;
  m_ePosn = src.m_ePosn;

	m_bIsCameraInfoAvailable = src.m_bIsCameraInfoAvailable;
	m_position = src.m_position;
	m_orientation = src.m_orientation;
	m_upDirection = src.m_upDirection;

  return *this;
}


/**
 * Swaps line endpoints.
 */
void MeasureLine::SwapEnds()
{
  Swap(m_measureEnds.m_start, m_measureEnds.m_end);
  Swap(m_drawEnds.m_start, m_drawEnds.m_end);
}


/**
 * Input stream operator.
 *
 * @param is          Input stream.
 * @param mline       Measurement line.
 * @return istream&   Input stream.
 */
std::istream& operator>>(std::istream& is, MeasureLine& mline)
{
  int4 iInt4ToEnum(0);
  is >> mline.m_measureEnds >> mline.m_drawEnds;
  is.ignore(1,',');
  is >> iInt4ToEnum; mline.m_ePosn = MeasureLine::POSITION(iInt4ToEnum);
	if (mline.m_bIsCameraInfoAvailable)  /// This unfortunate boost strip has to be used for backward compatibility of older session < 4.2.
	{
	  is.ignore(1, ',');
	  is >> mline.m_position >> mline.m_orientation >> mline.m_upDirection;
	}
  mline.StoreDistance();
  return is;
}


/**
 * Output stream operator.
 *
 * @param os               Output stream.
 * @param mline            Measure line.
 * @return std::ostream&   Output stream.
 */
std::ostream& operator<<(std::ostream& os, const MeasureLine& mline)
{
  os << mline.m_measureEnds << mline.m_drawEnds << "," << int4(mline.m_ePosn) << ",";
	if (mline.m_bIsCameraInfoAvailable)  /// This unfortunate boost strip has to be used for backward compatibility of older session.< 4.2
	{
	  os << mline.m_position << mline.m_orientation << mline.m_upDirection;
	}
  return os;
}


// $Log: MeasureLine.C,v $
// Revision 1.3  2006/10/04 04:15:57  jmeade
// comments.
//
// Revision 1.2  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.1  2006/01/31 14:19:11  frank
// code review
//
// Revision 1.2  2005/11/19 12:05:08  vxconfig
// coding standards
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.4.2.2.4.1.6.1  2005/06/20 17:06:31  jmeade
// Issue 4197: Finally completing the port to Visual Studio 2k3.
//
// Revision 1.4.2.2.4.1  2005/03/16 17:12:50  frank
// Issue #3992: removed unused data fields
//
// Revision 1.4.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.4.2.1  2003/04/14 16:35:53  jmeade
// Issue 2841:  MeasureLine string operator; Comments, code standards.
//
// Revision 1.4  2003/01/22 23:06:37  ingmar
// added missing includes
//
// Revision 1.3  2002/12/24 03:46:54  jmeade
// Use std::list stream i/o.
//
// Revision 1.2  2002/09/12 18:02:41  jmeade
// Check-in from defunct/unused VC_1-2-3 branch:  Code standards.
//
// Revision 1.1.2.1  2002/07/03 17:42:50  jmeade
// Coding standards.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/MeasureLine.C,v 1.3 2006/10/04 04:15:57 jmeade Exp $
// $Id: MeasureLine.C,v 1.3 2006/10/04 04:15:57 jmeade Exp $
