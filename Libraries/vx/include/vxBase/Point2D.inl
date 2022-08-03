// $Id: Point2D.inl,v 1.3 2005/08/05 13:02:33 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com



// define declarations
#define FILE_REVISION "$Revision: 1.3 $"



/**
 * Default constructor for class Point2D<Type>
 */
template <class Type> 
Point2D<Type>::Point2D()
: m_x(static_cast<Type>(0)),m_y(static_cast<Type>(0))
{
} // Default Constructor



/**
 * Constructor with two arguments
 * @param x const Type &
 * @param y const Type &
 */
template <class Type> 
Point2D<Type>::Point2D(const Type & x, const Type & y)
: m_x(static_cast<Type>(x)),m_y(static_cast<Type>(y))
{
} // end of Point2D<Type>(const Type & x, const Type & y)



/**
 * Constructor from Point2D<Type>
 * @param other const Point2D<Type> &
 */
template <class Type> 
Point2D<Type>::Point2D(const Point2D<Type> & other)
: m_x(other.X()),m_y(other.Y())
{
} // end of Point2D<Type>(const Point2D<Type> & other)

/**
 * Distance
 * @param point1 const Point2D<Type>
 * @param point2 const Point2D<Type>
 * @return float8
 */
template<class Type>
float8 Distance(const Point2D<Type> & point1, const Point2D<Type> & point2)
{
  Point2D<Type> p(point1 - point2);
  return(sqrt(static_cast<float8>(p.m_x) * static_cast<float8>(p.m_x) + static_cast<float8>(p.m_y) * static_cast<float8>(p.m_y)));
} // Distance



/**
 * Assignment Set operator
 * @param x const Type &
 * @param y const Type &
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::Set(const Type & x, const Type & y)
{
  m_x = x; 
  m_y = y;
  return *this;
} // end of Set(const Type & x, const Type & y)


/** 
 * Assignment operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
const Point2D<Type> & Point2D<Type>::operator=(const Point2D<Type> & other)
{
  m_x = Type(other.m_x); 
  m_y = Type(other.m_y);
  return *this;
} // end of operator=(const Point2D<Type> & other)


/**
 * @param & other
 * @return
 */
template <class Type> inline
const bool Point2D<Type>::operator == (const Point2D<Type> & other) const
{
  return(m_x == other.m_x && m_y == other.m_y);
} // operator ==


/**
 * @param & other
 * @return
 */
template <class Type> inline
const bool Point2D<Type>::operator != (const Point2D<Type> & other) const
{
  return !(*this == other);
} // operator !=


/**
 * Unary plus operator
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator + () const
{
  return *this;
} // end of operator+() const


/**
 * Unary minus operator
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator - () const
{
  return Point2D<Type>(-m_x,-m_y);
} // end of operator -() const


/**
 * Binary const plus operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator +(const Point2D<Type> & other) const
{
  return Point2D<Type>(m_x + other.m_x, m_y + other.m_y);
} // end of operator +(const Point2D<Type> & other) const

/**
 * Binary const minus operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator -(const Point2D<Type> & other) const
{
  return Point2D<Type>(m_x - other.m_x, m_y - other.m_y);
} // end of operator -(const Point2D<Type> & other) const


/**
 * Binary const times operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator*(const Point2D<Type> & other) const
{
  return Point2D<Type>(m_x * other.m_x, m_y * other.m_y);
} // end of operator*(const Point2D<Type> & other) const


/**
 * Binary const divide operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator /(const Point2D<Type> & other) const
{
  if (other.m_x == 0 || other.m_y == 0)
    throw ex::IllegalArgumentException(LogRec("other.m_x or other.m_y is 0", "Point2D", "operator /"));
  return Point2D<Type>(m_x / other.m_x, m_y / other.m_y);
} // end of operator /(const Point2D<Type> & other) const


/**
 * Binary += operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator +=(const Point2D<Type> & other)
{
  m_x += other.m_x; 
  m_y += other.m_y;
  return *this;
} // end of operator +=(const Point2D<Type> & other)


/**
 * Binary -= operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator -=(const Point2D<Type> & other)
{
  m_x -= other.m_x; 
  m_y -= other.m_y;
  return *this;
} // end of operator -=(const Point2D<Type> &other)


/**
 * Binary *= operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator *=(const Point2D<Type> & other)
{
  m_x *= other.m_x; 
  m_y *= other.m_y;
  return *this;
} // end of operator *=(const Point2D<Type> &other)


/**
 * Binary /= operator
 * @param other const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator /=(const Point2D<Type> & other)
{
  if (other.m_x == 0 || other.m_y == 0)
    throw ex::IllegalArgumentException(LogRec("other.m_x or other.m_y is 0", "Point2D", "operator /="));
  m_x /= other.m_x; 
  m_y /= other.m_y;
  return *this;
} // end of operator /=(const Point2D<Type> &other)


/**
 * Binary const + operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator +(const Type & scalar) const
{
  return Point2D<Type>(m_x + scalar, m_y + scalar);
} // end of operator +(const Type &scalar) const


/**
 * Binary const - operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator -(const Type & scalar) const
{
  return Point2D<Type>(m_x - scalar, m_y - scalar);
} // end of operator -(const Type &scalar) const


/**
 * Binary const * operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator *(const Type & scalar) const
{
  return Point2D<Type>(m_x * scalar, m_y * scalar);
} // end of operator *(const Type &scalar) const


/**
 * Binary const / operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> Point2D<Type>::operator /(const Type & scalar) const
{
  return Point2D<Type>(m_x / scalar, m_y / scalar);
} // end of operator /(const Type &scalar) const


/**
 * Binary const += operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator +=(const Type & scalar)
{
  m_x += scalar; 
  m_y += scalar;
  return *this;
} // end of operator +=(const Type & scalar)


/**
 * Binary const -= operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator -=(const Type & scalar)
{
  m_x -= scalar;
  m_y -= scalar;
  return *this;
} // end of operator -=(const Type & scalar)


/**
 * Binary const *= operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator *=(const Type & scalar)
{
  m_x *= scalar;
  m_y *= scalar;
  return *this;
} // end of operator *=(const Type & scalar)


/**
 * Binary const /= operator
 * @param scalar const Point2D<Type>
 * @return Point2D<Type>
 */
template <class Type> inline
Point2D<Type> & Point2D<Type>::operator /=(const Type & scalar)
{
  if (scalar == 0)
    throw ex::IllegalArgumentException(LogRec("scalar is 0", "Point2D", "operator /="));
  m_x /= scalar;
  m_y /= scalar;
  return *this;
} // end of operator /=(const Type & scalar)



/**
 * Overloaded << Operator.
 * Directs the three data members to the ostream, reference of which is passed
 * AsOstream member function is called which is virtual, hence the appropriate
 * function is called, which shows the class of the object of which it is an 
 * instance of.
 * @param os ostream &
 * @param point Point<Type> &
 * @return ostream &
 */  
//template <class Type> 
//inline std::ostream & operator<<(std::ostream & os, const Point2D<Type> & point)
//{
//  os << "Point2D:(" ;
//  os << point.m_x << ",";
//  os << point.m_y << ")"; 
//  return os;
//} // end of operator<<(ostream & os, const Point2D<Type> & point)
//


/**
 * >> Operator
 * @param is istream &
 * @param point Point<Type> &
 * @return is istream &
 */
//template <class Type> 
//inline std::istream & operator>>(std::istream & is, Point2D<Type> & point)
//{
//  // skipp "Point2D:("
//  char iOneChar='x';
//  for ( ; iOneChar!= '('; is >> iOneChar )
//    ; // noop
//
//  is >> point.m_x >> iOneChar; //","
//  is >> point.m_y >> iOneChar; //")"
//
//  return is;
//} // end of operator>>(istream &is, Point2D<Type> & point)


/**
 * convert class state to string
 * @return
 */
template <class Type> 
std::string Point2D<Type>::ToString() const
{
  std::strstream ss; 
  ss << "Point2D("<<X()<<", "<<Y()<<")"<<std::ends; 
  return( std::string(ss.str()) );
} // ToString()


/**
 * Test function for template class Point2D<Type>.
 * @param Type type of the Point2D
 * @param os output stream
 * @return bool bSuccess
 *
 */
template <class Type>
bool Point2D<Type>::TestByType(std::ostream & os)
{
  static const int1 * const sBoolStr[] = {"    Oh No ! :-( ","    :-) "};
  bool bSuccess(true);
  bool bOk(true);
  Type i(8.125),j(8.125);
  Type t1(0), t2(0);

  os << endl << "-Testing Point2D class for type: " << typeid(Type).name();
  os << endl << "i=" << i << " j=" << j << endl;
  os << "t1=" << t1 << " t2=" << t2 << endl;

  os <<"size of Point<Type>: ";
  os << sizeof(Point<Type>) << " Bytes" << endl;

  // tests default constructor.
  os << "Point2D<Type> point1= ";
  Point2D<Type> point1; 
  os << point1 << endl; 
  bOk = (point1.m_x == 0 && 
         point1.m_y == 0);
  os << sBoolStr[bOk] ;
  bSuccess = bSuccess && bOk;

  // tests constructor with arguments.
  os << "point2=Point2D<Type>(i,j): point2=  "; 
  Point2D<Type> point2;
  point2 = Point2D<Type> (i,j);
  os << point2 << endl; 
  bOk = (point2.m_x == i && 
         point2.m_y == j);
  os << sBoolStr[bOk] ; 
  bSuccess = bSuccess && bOk;

  // tests copy construct from Point2D<Type>
  os << "Point2D<Type> point3(point2): point3= ";
  Point2D<Type> point3(point2);
  os << point3 << endl;
  bOk = (point3.m_x == i &&
         point3.m_y == j);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests assignment Set operator
  os << "point1= " << point1 << "point1.Set(i,j)= ";
  os << point1.Set(i,j) << endl;
  bOk = (point1.m_x == i &&
         point1.m_y == j);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests assignment operator
  os << "Point2D<Type> point4=point3: point4= ";
  Point2D<Type> point4=point3;
  os << point4 << endl;
  bOk = (point4.m_x == i &&
         point4.m_y == j);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests unary + operator
  point1 = point2;
  os << "+point2= " << (+point2) << endl; 
  bOk = (point2.m_x == point1.m_x && 
         point2.m_y == point1.m_y);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests unary - operator
  // this test fails for unsigned char, which explains the "if"
  if ( (typeid(Type).name() != typeid(unsigned char).name()))
  {
    point1 = point2;
    os << "-point2= " << (point2=(-point2)) << endl ; 
    bOk = (point2.m_x == (-1*(point1.m_x)) && 
           point2.m_y == (-1*(point1.m_y))); 
    os << sBoolStr[bOk];
    bSuccess = bSuccess && bOk;
    point2 = -point2; // make point2 positive again for the next text
  } // endif Type != unsigned char

  // tests binary + operator between two points
  os << "point3=point3+Point2D<Type>(i,j): point3= "; 
  point1 = point3;
  point3=point3+Point2D<Type>(i,j); 
  os << point3 << endl; 
  bOk = (point3.m_x == (i+point1.m_x) && 
         point3.m_y == (j+point1.m_y));
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary - operator between two points
  os << "point3=point3-Point2D<Type>(i,j): point3= "; 
  point4 = point3;
  point3=point3-Point2D<Type>(i,j); 
  os << point3 << endl; 
  bOk = (point3.m_x == (point4.m_x - i) && 
         point3.m_y == (point4.m_y - j));
  os << sBoolStr[bOk]; 
  bSuccess = bSuccess && bOk;

  // tests binary * operator between two points
  os << "point4=point3*point1: point4= ";
  point4=point3*point1;
  os << point4 << endl;
  bOk = ((point4.m_x == (point3.m_x * point1.m_x)) &&
         (point4.m_y == (point3.m_y * point1.m_y)));
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary / operator between two points
  os << "point5=point5/point3: point5= ";
  Point2D<Type> point5=point4/point3;
  os << point5 << endl;
  bOk = ((point5.m_x == (point4.m_x / point3.m_x)) &&
         (point5.m_y == (point4.m_y / point3.m_y)));
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary += operator
  os << "point2+=point1: point2= "; 
  t1 = point1.m_x+point2.m_x;
  t2 = point1.m_y+point2.m_y;
  point2+=point1;
  os << point2 << endl;
  bOk = (point2.m_x == t1 && 
         point2.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary -= operator
  os << "point3-=point2: point3= "; 
  t1 = point3.m_x-point2.m_x;
  t2 = point3.m_y-point2.m_y;
  point3-=point2;
  os << point3 << endl;
  bOk = (point3.m_x == t1 && 
         point3.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary *= operator
  os << "point1*=point2: point1= "; 
  t1 = point1.m_x*point2.m_x;
  t2 = point1.m_y*point2.m_y;
  point1*=point2;
  os << point1 << endl;
  bOk = (point1.m_x == t1 && 
         point1.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary /= operator
  os << "point1/=point2: point1= "; 
  t1 = point1.m_x/point2.m_x;
  t2 = point1.m_y/point2.m_y;
  point1/=point2;
  os << point1 << endl;
  bOk = (point1.m_x == t1 && 
         point1.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary + operator between a point and a scalar
  os << "point3=point3+i: point3= ";
  t1 = point3.m_x+i;
  t2 = point3.m_y+i;
  point3=point3+i;
  os << point3 << endl;
  bOk = (point3.m_x == t1 &&
         point3.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary - operator between a point and a scalar
  os << "point3=point3-i: point3= ";
  t1 = point3.m_x-i;
  t2 = point3.m_y-i;
  point3=point3-i;
  os << point3 << endl;
  bOk = (point3.m_x == t1 &&
         point3.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary * operator between a point and a scalar
  os << "point2=point2*i: point2= ";
  t1 = point2.m_x*i;
  t2 = point2.m_y*i;
  point2=point2*i;
  os << point2 << endl;
  bOk = (point2.m_x == t1 &&
         point2.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary / operator between a point and a scalar
  os << "point2=point2/i: point2= ";
  t1 = point2.m_x/i;
  t2 = point2.m_y/i;
  point2=point2/i;
  os << point2 << endl;
  bOk = (point2.m_x == t1 &&
         point2.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary += operator between a point and a scalar
  os << "point4+=i: point4= ";
  t1 = point4.m_x+i;
  t2 = point4.m_y+i;
  point4+=i;
  os << point4 << endl;
  bOk = (point4.m_x == t1 &&
         point4.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests binary -= operator between a point and a scalar
  os << "point4-=i: point4= ";
  t1 = point4.m_x-i;
  t2 = point4.m_y-i;
  point4-=i;
  os << point4 << endl;
  bOk = (point4.m_x == t1 &&
         point4.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary *= operator between a point and a scalar
  os << "point1*=i: point1= ";
  t1 = point1.m_x*i;
  t2 = point1.m_y*i;
  point1*=i;
  os << point1 << endl;
  bOk = (point1.m_x == t1 &&
         point1.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary /= operator between a point and a scalar
  os << "point1/=i: point1= ";
  t1 = point1.m_x/i;
  t2 = point1.m_y/i;
  point1/=i;
  os << point1 << endl;
  bOk = (point1.m_x == t1 &&
         point1.m_y == t2);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests DistanceTo between two points
  os << "fDist=Distance(Point2D(12,13),point1): fDist= ";
  Point2D<Type>point6(12,13);
  float8 fDist = Distance(point6,point1);
  os << fDist << endl;
  point2 = point6 - point1;
  bOk = (fDist == (sqrt(pow(static_cast<float8>(point2.m_x), 2.0) + 
                        pow(static_cast<float8>(point2.m_y), 2.0))));
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // I don't think I can test GLDraw circle

  os << endl;  

  if (bSuccess)
  {
    os << "Point2D test is a success." << endl;
  }
  else
  {
    os << "Point2D test is NOT a success" << endl;
  }//endif (bSuccess)

  return bSuccess;

  return bOk;

} // end of TestByType



#undef FILE_REVISION



// end of Point2D.inl



// $Log: Point2D.inl,v $
// Revision 1.3  2005/08/05 13:02:33  geconomos
// moved implementation of operator<< and operator>> to .h
//
// Revision 1.2  2004/03/04 19:03:59  geconomos
// Moved distance implementation from .h to .inl
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 3.9  2003/05/16 11:43:47  michael
// code review
//
// Revision 3.8  2003/05/14 16:12:00  dongqing
// code review
//
// Revision 3.7  2003/03/04 17:59:42  frank
// Added ToString()
//
// Revision 3.6  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 3.3  2003/01/09 00:11:57  ingmar
// No longer derived from Point3D
//
// Revision 3.2  2002/10/14 15:34:29  michael
// made assignment operator to take const obejct reference and return const obj ref
//
// Revision 3.1  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:16   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:14   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.4  2001/08/22 23:31:12  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.3  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.2  2001/05/21 21:42:19  ana
// test function added
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Point2D.inl,v 1.3 2005/08/05 13:02:33 geconomos Exp $
// $Id: Point2D.inl,v 1.3 2005/08/05 13:02:33 geconomos Exp $