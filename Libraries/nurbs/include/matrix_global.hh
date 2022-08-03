/*=============================================================================
        File: global.hh
     Purpose: Define and include some general definitions valid for all 
              matrix header files
    Revision: $Id: matrix_global.hh,v 1.1.1.1 2004/03/02 03:58:35 geconomos Exp $
  Created by: Philippe Lavoie          (3 Oct, 1996)
 Modified by: 

 Copyright notice:
          Copyright (C) 1996-1998 Philippe Lavoie
 
	  This library is free software; you can redistribute it and/or
	  modify it under the terms of the GNU Library General Public
	  License as published by the Free Software Foundation; either
	  version 2 of the License, or (at your option) any later version.
 
	  This library is distributed in the hope that it will be useful,
	  but WITHOUT ANY WARRANTY; without even the implied warranty of
	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	  Library General Public License for more details.
 
	  You should have received a copy of the GNU Library General Public
	  License along with this library; if not, write to the Free
	  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
=============================================================================*/
#ifndef _Matrix_Global_h_
#define _Matrix_Global_h_

#undef PACKAGE
#undef VERSION

#include "plib_config.hh"

#undef PACKAGE
#undef VERSION

#include <cmath>
#include <iostream>
#include <complex>
#include <strstream>
#include <iomanip>
#include <fstream>

using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::real;
using std::imag;
using std::complex;
using std::ostream;
using std::istream;
using std::ofstream;
using std::ifstream;

//#ifdef USING_VCC
//namespace PLib {
//  const double M_PI = 3.1415926535897932385;
// }
//#endif
  

//#ifdef __GNUG__ 
//typedef double_complex Complex ;
//#else
typedef std::complex<double> Complex ;


//#ifdef __sgi
//inline Complex operator*(double a1, Complex a2)
//{
//  return Complex(a1*real(a2), a1*imag(a2));
//}
//#endif
//#endif 

#include "error.hh"

#endif
