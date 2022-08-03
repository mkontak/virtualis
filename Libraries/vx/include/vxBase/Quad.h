// $Id: Quad.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
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

#ifndef Quad_h
#define Quad_h



// includedeclarations 
#include "Triple.h"



/**
 * Template Class for Quads.
 * $Id: Quad.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
 */
template <class Type>
class Quad 
{
// Member functions.
public:
  
  /// Constructor for the class Template Quad<Type>.
  Quad<Type>();
  
  /// Constructor for the class Template Quad<Type>.
  Quad<Type>(const Type & scalarX, const Type & scalarY, const Type & scalarZ, const Type & scalarT);

  /// Copy Constructor from Quad<Type>.
  template<class T2>
	Quad<Type>(const Quad<T2> & other)
  {
    m_x = static_cast<Type>(other.m_x);
    m_y = static_cast<Type>(other.m_y); 
    m_z = static_cast<Type>(other.m_z);
    m_t = static_cast<Type>(other.m_t);
  }

  /// Copy Constructor from Quad<Type>.
  Quad<Type>(const Quad<Type> & other);  

  /// Copy Constructor from Triple<Type>.
  Quad<Type>(const Triple<Type> & otherTriple);

  /// Overloaded Assignment operator from Quad<Type>.
  inline Quad<Type> & operator =(const Quad<Type> & other);

  /// Assignment operator.
  inline Quad<Type> & operator =(const Triple<Type> & otherTriple);

  /// Casting Assignment.
  template <class C>
  inline Quad<Type> & Cast(const Quad<C> & other)
  {
    m_x = static_cast<Type>(other.m_x);
    m_y = static_cast<Type>(other.m_y); 
    m_z = static_cast<Type>(other.m_z);
    m_t = static_cast<Type>(other.m_t);
    return * this;
  } /// end of Cast(const Quad<C> & other)

  /// Overloaded Equals binary operator.
  inline bool operator ==(const Quad<Type> & other) const;
  
  /// Overloaded Not Equals binary operator.
  inline bool operator !=(const Quad<Type> & other) const; 

  /// Returns the three data members concatenated in a single string object.
  std::string AsString() const;
  
  /// Test function to verify and demonstrate the class functionality.
  static bool Test(std::ostream & os = std::cout);
  
  /// Test function to verify just one particular template instance 
  static bool TestByType(std::ostream & os=std::cout);

  /// Overloaded >> Operator for Quad<Type>.
  friend std::istream & operator >>(std::istream & is, Quad<Type> & myQuad);

  /// Overloaded << Operator for Quad<Type>.
  friend std::ostream & operator <<(std::ostream & os, const Quad<Type> & myQuad);

// Data Members.
public:

      /// the x component
      Type m_x;

      /// the y component
      Type m_y;
    
      /// the z component
      Type m_z;

      /// the t component
      Type m_t;

};// end of class Quad



#include "Quad.inl"



#endif // Quad_h



// $Log: Quad.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/05/16 11:53:30  michael
// code review
//
// Revision 3.5  2003/01/22 22:52:38  ingmar
// added missing includes
//
// Revision 3.4  2003/01/22 21:03:31  ingmar
// no longer derived from Triple
//
// Revision 3.3  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.2  2001/12/24 18:27:16  ingmar
// fixed member initialization
//
// Revision 3.1  2001/12/24 18:06:52  frank
// Added Quad class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Quad.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Quad.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
