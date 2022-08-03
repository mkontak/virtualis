// $Id: MeasureLine.h,v 1.2 2006/10/04 04:15:57 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net, Frank Dachille  frank@viatronix.net


// pragmas
#pragma once


// includes
#include "Point.h"
#include "Rect.h"
#include "Normal.h"
#include "Matrix.h"
#include "utilities.h"


/**
 * 3D Measurement line
 */
class MeasureLine
{
public:

  /// Constructor
  MeasureLine();

  /// Copy constructor
  MeasureLine(const MeasureLine & src);

  /// Stores the measure line length (so as not to recalculate repeatedly).
  const float StoreDistance();

  /// Copy operator
  MeasureLine & operator=(const MeasureLine & src);

  /// Reverses the ends of the line (making start the end, and vice-versa).
  void SwapEnds();

  /// Display string operator
  operator std::string() const { return ToAscii(m_f3Ddistance); }

  /// Input stream operator
  friend std::istream& operator>>(std::istream& is, MeasureLine& mline);

  /// Output stream operator
  friend std::ostream& operator<<(std::ostream& os, const MeasureLine& mline);
  
// Type definitions
public:

  /// Ends of a line
  template<class T> class LineEnds
  {
  public:

    /// Input stream operator
    friend std::istream& operator>>(std::istream& is, LineEnds<T>& lineEnds)
    { is >> lineEnds.m_start >> lineEnds.m_end; return is; }

    /// Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const LineEnds<T>& lineEnds)
    { os << lineEnds.m_start << lineEnds.m_end; return os; }

  public:

    /// the start point
    T m_start;
    
    /// the end point
    T m_end;

  }; // template<class T> class LineEnds

// member data
public:

  /// Ends of the line in 3D.
  LineEnds< Point<float4> > m_measureEnds;

  /// Stored line length.
  float m_f3Ddistance;

  /// Ends of the line in 2D.
  LineEnds< Point2D <int4> > m_drawEnds;

  /// Where to display text (e.g. line length).
  enum POSITION { START, MID, END, NODISPLAY } m_ePosn;


	/// camera information when measure was taken
	bool m_bIsCameraInfoAvailable;
	/// World coordinate.
	Point<float4> m_position;
	/// Up vector.
	Normal<float4> m_upDirection;
	/// Orientation.
	Normal<float4> m_orientation;

}; // class MeasureLine


// $Log: MeasureLine.h,v $
// Revision 1.2  2006/10/04 04:15:57  jmeade
// comments.
//
// Revision 1.1  2006/01/31 14:19:11  frank
// code review
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.6.2.1.4.1  2005/03/16 17:12:51  frank
// Issue #3992: removed unused data fields
//
// Revision 3.6.2.1  2003/04/14 16:35:53  jmeade
// Issue 2841:  MeasureLine string operator; Comments, code standards.
//
// Revision 3.6  2002/12/24 03:46:54  jmeade
// Use std::list stream i/o.
//
// Revision 3.5  2002/09/12 16:08:55  jmeade
// Check-in 3.4.8.1 from defunct/unused VC_1-2-3 branch.
//
// Revision 3.4.8.1  2002/07/03 17:42:50  jmeade
// Coding standards.
//
// Revision 3.4  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.3  2002/03/08 17:15:16  michael
// further removed include dependencies
//
// Revision 3.2  2002/03/04 16:43:28  michael
// fomatting and coding guidelines
//
// Revision 3.1.2.1  2002/03/05 20:18:28  binli
// added new ePosn: NODISPLAY
//
// Revision 3.1  2001/12/18 17:17:12  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:32  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 24 2001 14:42:54   frank
// Spelling
// 
//    Rev 2.0   Sep 16 2001 23:50:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:19  ingmar
// new directory structure
//
// Revision 1.19  2001/05/11 19:07:36  jmeade
// Coding conventions
//
// Revision 1.18  2001/03/21 16:17:10  frank
// Added standard deviation calculation to ROI statistics.
//
// Revision 1.17  2001/03/20 15:57:11  frank
// Updated to reflect new coding standards.
//
// Revision 1.16  2001/02/27 00:27:05  jmeade
// CPoint ==> Point2D<>
//
// Revision 1.15  2001/02/09 12:39:01  frank
// Cleared out m_current upon Clear().
//
// Revision 1.14  2001/02/05 20:47:15  jeff
// increase area for handle selection in SelectHandle() (reduce need for
// dexterity on the average user)
//
// Revision 1.13  2001/01/29 20:02:03  frank
// Added some more area statistics.
//
// Revision 1.12  2001/01/29 14:57:48  frank
// Moved selection functionality into class.
//
// Revision 1.11  2001/01/29 02:32:07  frank
// Added histogram.
//
// Revision 1.10  2001/01/26 17:55:42  frank
// Added data members to store statistics.
//
// Revision 1.9  2001/01/25 18:54:43  frank
// Permitted handle selection on measurement areas.
//
// Revision 1.8  2001/01/25 16:05:09  frank
// Added center handle.
//
// Revision 1.7  2001/01/25 14:36:19  frank
// Added MeasureArea for region of interest calculations
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/MeasureLine.h,v 1.2 2006/10/04 04:15:57 jmeade Exp $
// $Id: MeasureLine.h,v 1.2 2006/10/04 04:15:57 jmeade Exp $
