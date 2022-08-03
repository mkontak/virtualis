// $Id: vxIntensityStatistic.h,v 1.3 2005/07/13 14:44:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxIntensityStatistic_h
#define vxIntensityStatistic_h

#ifndef VX_VIEWER_DLL
#define VX_VIEWER_DLL _declspec( dllimport )
#endif

// class definition
class VX_VIEWER_DLL vxIntensityStatistic
{
// functions
public:
  
  /// default constructor
  vxIntensityStatistic();
  
  // copy constructor
  vxIntensityStatistic(const vxIntensityStatistic & other);
  
  // assignment operator
  vxIntensityStatistic & operator=(const vxIntensityStatistic & other);

  /// add a value
  void Add(const int2 iValue);

  /// compute the statistic
  void Compute();
  
  // return number of values
  const uint4 GetNumberEntries() const;
  
  /// return the minimum value
  const int2 & GetMinimum() const { return m_iMinimum; }
  
  /// return the maximum value
  const int2 & GetMaximum() const { return m_iMaximum; }
   
  /// return average
  const float4 & GetAverage() const { return m_fAverage; }
  
  /// return standard deviation
  const float4 & GetStandardDeviation() const { return m_fStandardDeviation; }
  
  /// clear the values and computed statistic
  void Clear();

// data
private:
  
  /// minimum value
  int2 m_iMinimum;
  
  /// maximum value
  int2 m_iMaximum;
  
  /// average value
  float4 m_fAverage;
  
  /// standard deviation
  float4 m_fStandardDeviation;
  
  /// actual data
  std::vector<int2> m_vDensities;
}; // class vxIntensityStatistic


#endif // vxIntensityStatistic_h


// Revision History:
// $Log: vxIntensityStatistic.h,v $
// Revision 1.3  2005/07/13 14:44:42  geconomos
// Issue #4255: Improved the speed (and accuracy) of the ROI statistics
//
// Revision 1.2  2004/08/12 20:25:14  frank
// exported classes so they can be used outside of this DLL
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/05/06 14:56:45  michael
// code review
//
// Revision 1.1  2003/01/29 12:29:09  michael
// added reworked initial version of intensity statistic
//
// Revision 1.4  2002/09/25 21:23:08  ingmar
// replaced pow(x,2) with x*x
//
// Revision 1.3  2002/05/07 18:18:49  frank
// Removed dead code.
//
// Revision 1.2  2002/05/03 22:25:46  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.1  2002/03/21 18:38:20  manju
// Moved vxIntensityStatistic and IncrementalvxIntensityStatistic classes to
// 50_dataStruct.
//
// Revision 3.3  2001/12/18 18:46:39  ingmar
// added class Sec_70_algImgProc to structure the doxygen documentation
//
// Revision 3.2  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1  2001/10/18 18:28:32  geconomos
// removed stdafx.h
//
// Revision 3.0  2001/10/14 23:02:29  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 10 2001 20:14:44   frank
// #undef'd FILE_REVISION
// 
//    Rev 2.0   Sep 16 2001 23:49:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:12   ingmar
// Initial revision.
// Revision 1.4  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.3  2001/09/10 09:05:00  ingmar
// fixed use of logger defines
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/06 12:08:33  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIntensityStatistic.h,v 1.3 2005/07/13 14:44:42 geconomos Exp $
// $Id: vxIntensityStatistic.h,v 1.3 2005/07/13 14:44:42 geconomos Exp $
