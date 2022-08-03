/*=============================================================================
        File: matrix.cc
     Purpose:       
    Revision: $Id: matrix_double.cc,v 1.1.1.1 2004/03/02 03:58:35 geconomos Exp $
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

#include "matrix.cc"

namespace PLib {
  
  void Matrix<double>::qSort(){
    qsort((char*)m,rows()*cols(),sizeof(double),compareDouble) ;
  }

#ifdef NO_IMPLICIT_TEMPLATES

  template class Matrix<double> ;
  
  template Matrix<double> operator+(const Matrix<double>&,const Matrix<double>&);
  template Matrix<double> operator-(const Matrix<double>&,const Matrix<double>&);
  template Matrix<double> operator*(const Matrix<double>&,const Matrix<double>&);
  template Matrix<double> operator*(const double,const Matrix<double>&);
  template Matrix<double> operator*(const Complex&,const Matrix<double>&);
  template Vector<double> operator*(const Matrix<double>&,const Vector<double>&);
  template int operator==(const Matrix<double>&,const Matrix<double>&);
  //template int operator!=(const Matrix<double>&,const Matrix<double>&);
  template Matrix<double> comm(const Matrix<double>&,const Matrix<double>&);


#endif

}
