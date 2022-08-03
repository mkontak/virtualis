// $Id: Quad.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete history at bottom of file.



/**
 * Default Constructor
 */
template <class T>
Quad<T>::Quad()
: m_x(static_cast<T>(0)), m_y(static_cast<T>(0)), m_z(static_cast<T>(0)), m_t(static_cast<T>(0))
{
} // Default Constructor


/**
 * Constructor from four scalars
 * @param & scalarX
 * @param & scalarY
 * @param & scalarZ
 * @param & scalarT
 */
template <class T>
Quad<T>::Quad(const T & scalarX, const T & scalarY, const T & scalarZ, const T & scalarT)
: m_x(scalarX), m_y(scalarY), m_z(scalarZ), m_t(scalarT)
{
} // Constructor from four scalars


/**
 * Copy Constructor.
 * @param  other const Quad<T> &
 */
template <class T> 
Quad<T>::Quad(const Quad<T> & other)
  : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z), m_t(other.m_t)
{
} // end of Quad<T>(const Quad<T> & other)


/**
 * Copy Constructor.
 * @param  normal const Triple<T> &
 */
template <class T> 
Quad<T>::Quad(const Triple<T> & otherTriple)
 : m_x(otherTriple.m_x), m_y(otherTriple.m_y), m_z(otherTriple.m_z), m_t(static_cast<T>(0))
{
} // end of Quad<T>(const Triple<T> & otherTriple)


/**
 * Assignment operator.
 * @param  other const Quad<T>&
 * @return Quad<T>
 */
template <class T> 
inline Quad<T> & Quad<T>::operator = (const Quad<T> & other)
{
  m_x = other.m_x; 
  m_y = other.m_y; 
  m_z = other.m_z;
  m_t = other.m_t;
  return * this;
} // end of operator = (const Quad<T> & other)


/**
 * Assignment operator.
 * @param  normal const Normal<T>&
 * @return Quad<T> &
 */
template <class T> 
inline Quad<T> & Quad<T>::operator = (const Triple<T> & otherTriple)
{
  m_x = otherTriple.m_x;
  m_y = otherTriple.m_y;
  m_z = otherTriple.m_z;
  m_t = static_cast<T>(0);
  return * this;
} // end of operator = (const Triple<T> & otherTriple)


/**
 * Equals binary operator.
 * @param  other const Quad<T>&
 * @return bool
 */
template <class T> 
inline bool Quad<T>::operator == (const Quad<T> & other) const 
{ 
  return ( m_x==other.m_x && m_y==other.m_y && m_z==other.m_z && m_t==other.m_t);
} // end of operator == (const Quad<T> & other) const


/**
 * Equals binary operator for Quad<float>.
 * @param  other const Quad<float>&
 * @return bool
 */
template <> 
inline bool Quad<float4>::operator == (const Quad<float4> & other) const 
{ 
  const float4 fEpsilon(static_cast<float4>(0.0001));
  return ((fabs(m_x-other.m_x) < fEpsilon) && 
          (fabs(m_y-other.m_y) < fEpsilon) && 
          (fabs(m_z-other.m_z) < fEpsilon) &&
          (fabs(m_t-other.m_t) < fEpsilon));
} // end of operator == (const Quad<float> & other) const


/**
 * Equals binary operator for Quad<double>.
 * @param  other const Quad<double>&
 * @return bool
 */
template <> 
inline bool Quad<float8>::operator == (const Quad<float8> & other) const 
{ 
  const float8 fEpsilon(static_cast<float8>(0.0001));
  return ((fabs(m_x-other.m_x) < fEpsilon) && 
          (fabs(m_y-other.m_y) < fEpsilon) && 
          (fabs(m_z-other.m_z) < fEpsilon) && 
          (fabs(m_t-other.m_t) < fEpsilon) );
} // end of operator == (const Quad<float8> & other) const


/**
 * Not Equals binary operator.
 * @param  other const Quad<T>&
 * @return bool
 */
template <class T> 
inline bool Quad<T>::operator != (const Quad<T> & other) const 
{ 
  return ( m_x!=other.m_x || m_y!=other.m_y || m_z!=other.m_z || m_t!=other.m_t);
} // end of operator != (const Quad<T> & other) const 


/**
 * Not Equals binary operator for Quad<float>.
 * @param  other const Quad<float>&
 * @return bool
 */
template <> 
inline bool Quad<float4>::operator != (const Quad<float4> & other) const 
{ 
  const float4 fEpsilon(static_cast<float4>(0));
  return ((fabs(m_x-other.m_x) > fEpsilon) || 
          (fabs(m_y-other.m_y) > fEpsilon) || 
          (fabs(m_z-other.m_z) > fEpsilon) ||
          (fabs(m_t-other.m_t) > fEpsilon) );
} // end of operator != (const Quad<float> & other) const


/**
 * Not Equals binary operator for Quad<double>.
 * @param  other const Quad<double>&
 * @return bool
 */
template <> 
inline bool Quad<float8>::operator != (const Quad<float8> & other) const 
{ 
  const float8 fEpsilon(static_cast<float8>(0));
  return ((fabs(m_x-other.m_x) > fEpsilon) ||
          (fabs(m_y-other.m_y) > fEpsilon) ||
          (fabs(m_z-other.m_z) > fEpsilon) ||
          (fabs(m_t-other.m_t) > fEpsilon) );
} // end of operator != (const Quad<double> & other) const



/**
 * Overloaded >> Operator.
 * Accepts from istream 3 values for the Quad<T> object
 * @param is istream &
 * @param Quad Quad<T> &
 * @return istream &
 */ 
template <class T> 
std::istream & operator>>(std::istream & is, Quad<T> & myQuad)
{
  is >> myQuad.m_x >> myQuad.m_y >> myQuad.m_z >> myQuad.m_t;
  return is;
} // end of operator>>(istream &is, Quad<T> & Quad)


/**
 * Overloaded << Operator.
 * Directs the three data members to the ostream, reference of which is passed
 * AsOstream member function is called which is virtual, hence the appropriate
 * function is called, which shows the class of the object of which it is an 
 * instance of.
 * @param os ostream &
 * @param Quad Quad<T> &
 * @return ostream &
 */  
template <class T> 
std::ostream & operator<<(std::ostream &os, const Quad<T> & myQuad)
{
  os << "Quad:(" << myQuad.m_x ;
  os << "," << myQuad.m_y ;
  os << "," << myQuad.m_z ;
  os << "," << myQuad.m_t ;
  os << ")";
  return os;
} // end of operator<<(ostream &os, const Quad<T> & Quad)



/**
 * Returns the three data members concatenated in a single string object.
 * returns a string like: (m_x,m_y,m_z,m_t).
 * @return string 
 */
template <class T> 
std::string Quad<T>::AsString() const
{
  std::strstream sStr;
  sStr << *this;
  std::string sQuadStr(sStr.str(), sStr.pcount());
  std::string sTemp = sQuadStr.substr(sQuadStr.find_first_of('('), 
                                       (sQuadStr.length() - 
                                        sQuadStr.find_first_of('(')) );
  return sTemp;
} // end od AsString()



/**
 * Test function for template class Quad<T>.
 *
 * @param T type of the Quad
 * @param os output stream
 * @return bool bSuccess
 *
 */
template <class T>
bool Quad<T>::TestByType(std::ostream & os) 
{
  static const int1 * const sBoolStr[] = {"    Oh No ! :-( ","   :-) "};
  bool bSuccess(true);
  bool bOk(true);

  T i(2.125F),j(2.125F),k(2.125F),l(2.145F);
  T t1(1.75F);
  T t2(2.5F);
  T t3(3.75F);
  T t4(4.5F);
  
  // (t5,t6,t7,t8) have values very close to (i,j,k,l),
  // which can be used to test equal and not equal operators.
  T t5(2.12415F);
  T t6(2.12490F);
  T t7(2.12495F);
  T t8(2.12495F);
  
  typedef Quad<T> QuadT;

  os << endl << "-Testing Quad class for type: " << typeid(T).name();
  os << endl << "i=" << i << " j=" << j << " k=" << k << " l=" << l << endl;
  os << "t1=" << t1 << " t2=" << t2 << " t3=" << t3 << endl;
  os << "t4=" << t4 << " t5=" << t5 << " t6=" << t6 << endl;
  os << "t7=" << t7 << " t8=" << t8 << endl;
  
  os << "size of Quad<T>: " ;
  os << sizeof(Quad<T>) << " Bytes" << endl;

  // tests default constructor 
  os << "Quad<T> quad1: quad1= ";
  Quad<T> quad1;
  os << quad1 << endl;
  bOk = (quad1.m_x == 0 && 
         quad1.m_y == 0 && 
         quad1.m_z == 0 &&
         quad1.m_t == 0);
  bSuccess = bSuccess && bOk;
  os << sBoolStr[bOk];

  // tests constructor with arguments
  os << "quad2=Quad<T>(t1,t2,t3,t4): quad2= ";
  Quad<T> quad2(t1,t2,t3,t4);
  os << quad2 << endl;
  bOk = (quad2.m_x == t1 &&
         quad2.m_y == t2 &&
         quad2.m_z == t3 &&
         quad2.m_t == t4);
  bSuccess = bSuccess && bOk;
  os << sBoolStr[bOk]; 

  // tests constructor again with different arguments
  os << "quad5=Quad<T>(t4,t5,t6,t7): quad5= ";
  Quad<T> quad5(t4,t5,t6,t7);
  os << quad5 << endl;
  bOk = (quad5.m_x == t4 &&
         quad5.m_y == t5 &&
         quad5.m_z == t6 &&
         quad5.m_t == t7);
  bSuccess = bSuccess && bOk;
  os << sBoolStr[bOk]; 

  // test Destructor
  os << "Test Destructor ";
  Quad<T> * quad4 = new Quad<T>(i,j,k,l);
  os << "quad4: " << quad4;
  try
  {
    delete quad4;
    os << " deleted quad4. " << endl;
    bOk = true;
  }
  catch (...)
  {
    os << " unable to delete quad4." << endl;
    bOk = false;
  }
  bSuccess = bSuccess && bOk;
  os << sBoolStr[bOk]; 

  // tests Assignment operator
  os << "quad2=Quad<T>(i,j,k): quad2= ";
  quad2 = Quad<T>(i,j,k,l);
  os << quad2 << endl;
  bOk = (quad2.m_x == i &&
         quad2.m_y == j &&
         quad2.m_z == k &&
         quad2.m_t == l);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  //test copy constructor from Quad
  os << "Quad<T> quad3(Quad2): quad3= ";
  quad2 = Quad<T>(i,j,k,l);
  Quad<T> quad3(quad2);
  os << quad3 << endl;
  bOk = (quad3.m_x == i &&
         quad3.m_y == j &&
         quad3.m_z == k &&
         quad3.m_t == l);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;
 
  // Tests if the data members could be assigned
  os << "quad1.m_x=t2; quad1.m_y=t2; quad1.m_z=t2; quad1.m_t=t2; quad1= ";
  quad1.m_x = t2; quad1.m_y = t2; quad1.m_z = t2; quad1.m_t=t2;
  bOk = (quad1.m_x == t2 &&
         quad1.m_y == t2 &&
         quad1.m_z == t2 &&
         quad1.m_t == t2);
  bSuccess = bSuccess && bOk;
  os << quad1 << endl;
  os << sBoolStr[bOk];

  // tests equals operator
  os << "Is quad2 == Quad<T>(i,j,k,l): " ;
  os << (bOk = (quad2 == Quad<T>(i,j,k,l))) << endl;
  os << sBoolStr[bOk == 1];
  bSuccess = bSuccess && (bOk == 1);

  // tests equals operator second
  os << "Is qQuad5 == Quad<T>(i,j,k,l): " ;
  os << (bOk = (quad5 == Quad<T>(i,j,k,l))) << endl;
  os << sBoolStr[bOk == 1];
  bSuccess = bSuccess && (bOk == 1);

  // tests not equals operator
  os << "Is quad2 != Quad<T>(i,j,k,l): " ;
  os << (bOk = (quad2 != Quad<T>(i,j,k,l))) << endl; 
  os << sBoolStr[bOk == 0];
  bSuccess = bSuccess && (bOk == 0);

  // tests not equals operator second
  os << "Is quad5 != Quad<T>(i,j,k,l): " ;
  os << (bOk = (quad5 != Quad<T>(i,j,k,l))) << endl; 
  os << sBoolStr[bOk == 0];
  bSuccess = bSuccess && (bOk == 0);

  // tests AsString function
  string sTemp = quad2.AsString();
  os << "sTemp=quad2.AsString(): sTemp=  ";
  os << sTemp << endl;
  bOk = (sTemp == quad2.AsString());
  bSuccess &= bOk;
  os << sBoolStr[bOk];

  // tests size of Quad<T>
  os << "4 * sizeof(T): " << sizeof(T);
  os << " == size of Quad<T>: " << sizeof(Quad<T>) << endl;
  bOk = ( (4 * sizeof(T)) == sizeof(Quad<T>) );
  bSuccess &= bOk;
  os << sBoolStr[bOk];
 
  os << endl;
  if (bSuccess) 
  {
    os << "Quad test is a success." << endl;
  }	
  else 
  {
    os << "Quad test is NOT a success" << endl;
  } // endif (bSuccess)
  
  return bSuccess;
  
} // end of TestByType

  
  
// $Log: Quad.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/05/16 11:53:30  michael
// code review
//
// Revision 3.6  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 3.5  2003/01/22 21:03:31  ingmar
// no longer derived from Triple
//
// Revision 3.4  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.3  2002/01/09 15:34:34  soeren
// removed truncation warning
//
// Revision 3.2  2001/12/24 18:27:16  ingmar
// fixed member initialization
//
// Revision 3.1  2001/12/24 18:06:52  frank
// Added Quad class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quad.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Quad.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
