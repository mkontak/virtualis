// $Id: Quaternion.h,v 1.3 2007/07/20 19:55:38 cqian Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Bin Li (binli@viatronix.com)

#ifndef Quaternion_h
#define Quaternion_h


// includes
#include "Vector.h"


// forward declarations
template <class T> class Matrix;


/**
 * Template class Quaternion<T>
 */
template <class T> 
class Quaternion 
{
// member functions
public:

  /// Default Constructor.
  Quaternion<T>();
 
  /// Constructor from four scalars.
  Quaternion<T>(T x, T y, T z, T w);

  /// Constructor from one vector and one scalar.
  Quaternion<T>(const Vector<T> & v, T w);
  
  /// the length of quaternion.
  inline float4 Length();

  /// normalize the quaternion.
  inline Quaternion<T> &Normalize();
  
  /// multiply operation of quaternions.
  inline Quaternion<T> multiply(const Quaternion & q) const;

  /// Overloaded () Operator.
  inline void operator () (T x, T y, T z, T w);

  /// Assignment Operator  from Quaternion.
  inline Quaternion<T> & operator = (const Quaternion<T> & q);

  /// Overloaded Unary * Operator.
  inline Quaternion<T> operator * (const Quaternion<T> & q) const;

  /// Overloaded Binary *= Operator: quat *= quat.
  inline Quaternion<T> & operator *= (const Quaternion<T> & q);

  /// Overloaded Unary [] Operator.
  inline T operator [] (int index);

  /// Overloaded Unary [] Operator.
  inline const T & operator [] (int index) const;

  /// get a vector from quaternion.
  inline void GetVectorFromQuaternion(Vector<T> & vector) const;

  /// get a 3-element array from quaternion.
  inline void GetArrayFromQuaternion(T * array) const;

  /// return a vector from quaternion.
  inline Vector<T> GetVectorFromQuaternion() const;

  /// set up the value of member m_w.
  inline void SetW(T value);

  /// conjugate set of quat.
  inline void SetConjugate();
  
  /// set quat as conjugate.
  inline Quaternion<T> AsConjugate() const;

  /// set up 'rotation' about the m_longAxis.
  inline void SetRotationAboutAxis(Vector<T> vector, T phi);

  /// Test function to verify and demonstrate the class functionality.
  static bool Test(std::ostream & os=std::cout);

  /// Test function to verify just one particular template instance 
  static bool TestByType(std::ostream & os=std::cout);

public:
  /// build OpenGl matrix (m[16]) from quaternion.
  inline void BuildOpenGLMatrixFromQuat(float4 vfMatrix[16]) const;

  /// build a rotation matrix from quaternion.
  inline void BuildMatrixFromQuat(float4 mfMatrix[][4]);

  /// build a rotation matrix from quaternion.
  inline void BuildMatrixFromQuat(Matrix<float4> & matrix);

  /// accumulate quaternion.
  inline void AddQuats(Quaternion & lastQuat);
  
// data members.
private:

  /// the x component
  T m_x;
  
  /// the y component
  T m_y;
  
  /// the z component
  T m_z;
  
  /// the w component
  T m_w;

}; // Class Quaternion


#include "Quaternion.inl"


#endif // Quaternion_h


// $Log: Quaternion.h,v $
// Revision 1.3  2007/07/20 19:55:38  cqian
// Check in for test
//
// Revision 1.2  2005/08/05 13:04:19  geconomos
// removed derivation from documentation classes
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 3.5  2003/05/16 11:48:47  michael
// code review
//
// Revision 3.4  2003/01/02 19:38:45  ingmar
// better documentation structure
//
// Revision 3.3  2001/01/02 18:52:20  ingmar
// better documentation structure
//
// Revision 3.2  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
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
//    Rev 2.1   Sep 19 2001 14:33:18   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:16   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.18  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.17  2001/05/29 18:38:48  ana
// added head comments
//
// Revision 1.16  2001/05/25 20:54:10  ana
// finished tests in TestByType method
//
// Revision 1.15  2001/05/24 21:27:44  ana
// new tests added to TestByType
//
// Revision 1.14  2001/05/23 21:34:54  ana
// created TestByType function
//
// Revision 1.13  2001/04/12 14:37:28  dmitry
// Sec_3_math base class added
//
// Revision 1.12  2001/04/09 18:03:19  dmitry
// Removed Revision History from Top of File
//
// Revision 1.11  2001/03/30 21:21:27  binli
// changed to template Quaternion
//
// Revision 1.10  2001/03/28 18:02:38  binli
// adjusted and added functions in Quaternion class.
//
// Revision 1.9  2001/03/27 23:19:46  binli
// extended the function of class 'Quaternion'.
//
// Revision 1.8  2001/03/27 15:12:30  binli
// reflected Typedef.h
//
// Revision 1.7  2001/03/26 15:30:37  frank
// Cleaned up a bit.
//
// Revision 1.6  2001/03/23 20:08:52  dmitry
// Updated datatypes as declared in Typedef.h
//
// Revision 1.5  2000/12/28 19:59:44  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.4  2000/09/26 02:58:13  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.3  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/21 21:20:52  binli
// Followed the change of Trackball Class that removed some self-defined  vector operations
//
// Revision 1.1  2000/09/21 18:32:06  binli
// built Class Quaternion (some more functions are needed)
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quaternion.h,v 1.3 2007/07/20 19:55:38 cqian Exp $
// $Id: Quaternion.h,v 1.3 2007/07/20 19:55:38 cqian Exp $
