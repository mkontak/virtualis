// $Id: ZeroOne.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ZeroOne.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)

/*
  A floating point number representation that automatically 
  clamps all operations to the [0, 1] interval
*/

#ifndef ZeroOne_h
#define ZeroOne_h

// class definition
class ZeroOne 
{
// functions
public:

	/// default constructor
	inline ZeroOne();

	/// constructor
	inline ZeroOne(const float4 value);

	/// get scalar
	inline const float4 V() const {return m_v;}

	/// operator() convert to float
	inline operator float () const;

	/// operator /=
	inline ZeroOne & operator /= (const float4 v);

	/// operator *=
	inline ZeroOne & operator *= (const float4 v);

	/// operator +=
	inline ZeroOne & operator += (const float4 v);
	
	/// operator -=
	inline ZeroOne & operator -= (const float4 v);

	/// operator=
	inline const ZeroOne & operator=(const ZeroOne & rhs);

	/// operator= (from scalar)
	template <class T>
	inline const ZeroOne & operator=(const T v); // compiler hack since 6.0 does not support partial template specialization. Thus, we use parameter overloading (ref vs. non ref)

private:

  /// clamp values to [0.0, 1.0]
	inline void Clamp();

// data
private:

  /// actual float value that is kept in [0.0, 1.0] range
	float4 m_v;
}; // ZeroOne


#include "ZeroOne.inl"


#endif // ZeroOne_h


// $Log: ZeroOne.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.11  2003/05/16 11:48:48  michael
// code review
//
// Revision 3.10  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 3.9  2003/01/13 20:17:51  ingmar
// added default constructor
//
// Revision 3.8  2001/01/02 19:39:45  ingmar
// better documentation structure
//
// Revision 3.7  2001/01/02 18:53:35  ingmar
// better documentation structure
//
// Revision 3.6  2002/10/14 15:13:39  michael
// fixed assignment operator bug ... but should be replaced by partial template
// specialization once moving to the MS VC 7.0 compiler
//
// Revision 3.5  2002/10/13 21:40:44  michael
// fixed assignmen operator and removed ambiguity
//
// Revision 3.4  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.3  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.2  2002/02/25 23:12:13  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.1.2.1  2002/04/08 17:48:07  frank
// Added a return type to various functions that require it.
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:22   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:47:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:24   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:41  soeren
// renamed math directory
//
// Revision 1.1  2001/07/10 20:15:37  liwei
// Added to ZeroOne to handle variables of the value in [0, 1]
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ZeroOne.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ZeroOne.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
