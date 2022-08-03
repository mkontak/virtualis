// $Id: MatrixNxM.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright © 2002 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: S. Yoakum-Stover   suzi@viatronix.net

#ifndef MatrixNxM_h
#define MatrixNxM_h

// include declarations
#include <string>
#include <math.h>
#include "typedef.h"

class MatrixNxM
{
// Member Functions
public:

  /// Default constructor
  MatrixNxM();
  
  /// Constructor
  MatrixNxM(uint4 uNRows, uint4 uMCols);
  
  /// Constructor
  MatrixNxM(float4 fV, uint4 uNRows, uint4 uMCols);
  
  /// Default destructor
  ~MatrixNxM();

  /// Change the size o the matrix
  void Resize(uint4 uNRows, uint4 uMCols);
  
  /// [r][c] access function
  float4 & operator()(uint4 uRow, uint4 uCol);
  
  /// [r][c] access function
  float4 operator()(uint4 uRow, uint4 uCol) const;

  /// Debug output
  void PrintToFile(std::string message);

// Data Members
private:

  /// stores elements in row-major order
  float4 ** m_hV;  

  /// Size of the matrix
  uint4 m_uNumRows;
  
  /// Size of the matrix
  uint4 m_uNumCols;
};


#endif //MatrixNxM_h

// $Log: MatrixNxM.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.7.4.3  2003/05/27 12:16:13  frank
// code review
//
// Revision 1.7.4.2  2003/05/23 19:34:12  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.7.4.1  2003/05/01 16:56:06  kevin
// Code Walkthrough cosmetic changes
//
// Revision 1.7  2002/10/23 00:25:08  kevin
// Code cleanup after last round of S/P registration algorithm development
//
// Revision 1.5  2002/09/11 19:13:58  kevin
// Proper deleting of the pointer to the valarray stl object
//
// Revision 1.4  2002/06/19 19:50:01  ana
// Bugs fixes in the new registration.
//
// Revision 1.3  2002/06/12 20:10:16  suzi
// Huge refactoring and implementation of new registration with multiple seeds.
//
// Revision 1.2  2002/06/07 21:30:07  suzi
// Implementation almost complete.
//
// Revision 1.1  2002/06/07 19:46:03  suzi
// First implementation of MatchingMetrics (registration code).
//
// Revision 1.5  2002/06/03 19:38:29  suzi
// Added normal form.
//
// Revision 1.4  2002/05/02 18:31:28  suzi
// Added default constructor.
//
// Revision 1.3  2002/05/01 22:50:29  suzi
// no message
//
// Revision 1.2  2002/04/30 21:08:04  suzi
// Added more operator overloads and a GetMaxValue function.
//
// Revision 1.1  2002/04/30 19:23:08  suzi
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/MatrixNxM.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: MatrixNxM.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $

