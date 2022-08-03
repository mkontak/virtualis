// $Id: vxComplex.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com

#ifndef vxComplex_h
#define vxComplex_h


class vxComplex : public std::complex<float4>
{
  // public member functions
public:

  /// constructor
  vxComplex(float4 fReal=0, float4 fImag=0) : std::complex<float4>(fReal, fImag)
  {
  }

  /// copy constructor
  vxComplex(const vxComplex & other) : std::complex<float4>(other)
  {
  }

  /// copy constructor
  vxComplex(const std::complex<float4> & other) : std::complex<float4>(other)
  {
  }


  /// cast to float operator
  operator float () const
  {
    return(real());
  }

  vxComplex        operator + (const vxComplex & rhs                 ) const { return (vxComplex(real()     + rhs.real(), imag()     + rhs.imag())); }
  friend vxComplex operator + (const float4    & lhs, const vxComplex & rhs) { return (vxComplex(lhs        + rhs.real(),              rhs.imag())); }
  friend vxComplex operator + (const vxComplex & lhs, const float4    & rhs) { return (vxComplex(lhs.real() + rhs,        lhs.imag()             )); }

  vxComplex        operator - (const vxComplex & rhs                 ) const { return (vxComplex(real()     - rhs.real(), imag()     - rhs.imag())); }
  friend vxComplex operator - (const float4    & lhs, const vxComplex & rhs) { return (vxComplex(lhs        - rhs.real(),            - rhs.imag())); }
  friend vxComplex operator - (const vxComplex & lhs, const float4    & rhs) { return (vxComplex(lhs.real() - rhs,        lhs.imag()             )); }

  vxComplex        operator * (const vxComplex & rhs                 ) const { return (vxComplex(real()     * rhs.real(), imag()     * rhs.imag())); }
  friend vxComplex operator * (const float4    & lhs, const vxComplex & rhs) { return (vxComplex(lhs        * rhs.real(), lhs        * rhs.imag())); }
  friend vxComplex operator * (const vxComplex & lhs, const float4    & rhs) { return (vxComplex(lhs.real() * rhs,        lhs.imag() * rhs       )); }
}; // class vxComplex


#endif // vxComplex_h


// $Log: vxComplex.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.3  2003/01/02 19:39:29  ingmar
// better documentation structure
//
// Revision 1.2  2001/01/02 18:53:22  ingmar
// better documentation structure
//
// Revision 1.1  2002/08/28 15:03:17  ingmar
// initial version
//
// $Id: vxComplex.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
