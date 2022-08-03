/************************************************************************

  n-D Quadric Error Metrics

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxQMetric.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxQMetric.h"

static
void symmetric_subfrom(MxMatrix& A, const MxVector& a, const MxVector& b)
{
    for(int4 i=0; i<A.dim(); i++)  for(int4 j=0; j<A.dim(); j++)
	A(i,j) -= a[i]*b[j];
}

MxQuadric::MxQuadric(const MxVector& p1,const MxVector& p2,const MxVector& p3,
		     float8 area)
    : A(p1.dim()), b(p1.dim())
{
    AssertBound( p1.dim()==p2.dim() && p1.dim()==p3.dim() );

    MxVector e1=p2; e1-=p1; unitize(e1); // e1 = p2-p1; unitize
    MxVector e2=p3; e2-=p1;              // e2 = p3-p1
    MxVector t = e1;
    t*=e1*e2; e2-=t; unitize(e2);        // e2 = p3-p1-e1*(e1*(p3-p1)); unitize

    float8 p1e1 = p1*e1;
    float8 p1e2 = p1*e2;

    mxm_identity(A, A.dim());
    symmetric_subfrom(A, e1,e1);
    symmetric_subfrom(A, e2,e2);

    // b = e1*p1e1 + e2*p1e2 - p1
    b=e1; b*=p1e1; t=e2; t*=p1e2; b += t; b -= p1;

    c = p1*p1 - p1e1*p1e1 - p1e2*p1e2;

    r = area;
}

MxQuadric::MxQuadric(const MxQuadric3& Q3, uint4 N)
    : A(N), b(N)
{
    uint4 i, j;

    clear();

    Mat3 A3 = Q3.tensor();
    Vec3 b3 = Q3.vector();

    for(i=0; i<3; i++)
    {
	for(j=0; j<3; j++)
	    A(i,j) = A3(i,j);

	b[i] = b3[i];
    }

    c = Q3.offset();
    r = Q3.area();
}

MxMatrix& MxQuadric::homogeneous(MxMatrix& H) const
{
    AssertBound( H.dim() == A.dim()+1 );

    uint4 i, j;

    for(i=0; i<A.dim(); i++)  for(j=0; j<A.dim(); i++)
	H(i,j) = A(i,j);

    for(i=0; i<b.dim(); i++)
	H(i, b.dim()) = H(b.dim(), i) = b[i];

    H(b.dim(), b.dim()) = c;

    return H;
}

float8 MxQuadric::evaluate(const MxVector& v) const
{
    AssertBound( v.dim() == b.dim() );
    return v*(A*v) + 2*(b*v) + c;
}

bool MxQuadric::optimize(MxVector& v) const
{
    MxMatrix Ainv(A.dim());

    float8 det = A.invert(Ainv);
    if( FEQ(det, 0.0, 1e-12) )
	return false;

    v = (Ainv * b);
    mxv_neg(v, v.dim());

    return true;
}
