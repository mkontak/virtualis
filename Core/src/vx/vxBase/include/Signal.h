// $Id: Signal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
 // Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete history at bottom of file.

#ifndef Signal_h
#define Signal_h


// include declarations 
#include "10_math.h"
#include "Typedef.h"
#include <ostream>
#include <vector>


// forward declarations


/**
 * Class for regular, one-dimensional (time-varying) signals.
 */
class Signal : Sec_10_math
{
// Member functions
public:
  
  /// Default constructor
  Signal();
  
  /// Copy constructor
  Signal(const Signal & other);  

  /// Overloaded Assignment operator
  inline Signal & operator =(const Signal & other);

  /// Test function to verify and demonstrate the class functionality.
  static bool Test(std::ostream & os = std::cout);
 
// Data Members
public:

  /// The time-varying data
  std::vector<float4> m_vAmplitudes;

  /// The starting time
  float4 m_fStartTime;
  
  /// The ending time
  float4 m_fEndTime;
  
}; // class Signal


#endif // Signal_h

// $Log: Signal.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:48:47  michael
// code review
//
// Revision 1.1  2002/04/20 20:09:34  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Signal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Signal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
