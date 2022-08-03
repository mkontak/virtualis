#ifndef MXMATH_INCLUDED // -*- C++ -*-
#define MXMATH_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Standard math include file for the MixKit library.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMath.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include <math.h>
#include "Typedef.h"

// NOTE: The use of mx_real is deprecated, and the typedef will
//       be going away in the near future.
//
#ifdef MIX_REAL_FLOAT
typedef float4 mx_real;
#else
#define MIX_REAL_DOUBLE
typedef float8 mx_real;
#endif

// Some systems use HUGE_VAL instead of HUGE
#if !defined(HUGE) && defined(HUGE_VAL)
#define HUGE HUGE_VAL
#endif

// Handle platforms, such as Win32, which don't define M_PI in <math.h>
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592308
#endif

#ifndef FEQ_EPS
#define FEQ_EPS 1e-6
#define FEQ_EPS2 1e-12
#endif
inline bool FEQ(float8 a,float8 b,float8 eps=FEQ_EPS) { return fabs(a-b)<eps; }
inline bool FEQ2(float8 a,float8 b,float8 eps=FEQ_EPS2) {return fabs(a-b)<eps;}
#ifdef MIX_HAVE_FABSF
inline bool FEQ(float4 a,float4 b,float4 eps=FEQ_EPS) { return fabsf(a-b)<eps; }
inline bool FEQ2(float4 a,float4 b,float4 eps=FEQ_EPS2) { return fabsf(a-b)<eps; }
#endif

#if defined(WIN32) || !defined(MIX_HAVE_RINT)
inline float8 rint(float8 x) { return floor(x + 0.5); }
#endif

#ifdef MIX_HAVE_RANDOM
inline float8 random1() { return (float8)random() / (float8)LONG_MAX; }
inline int1   random_byte() { return (int1)(random() & 0xff); }
#else
inline float8 random1() { return (float8)rand() / (float8)RAND_MAX; }
inline int1   random_byte() { return (int1)(rand() & 0xff); }
#endif

#ifndef MIX_NO_AXIS_NAMES
enum Axis {X=0, Y=1, Z=2, W=3};
#endif

// MXMATH_INCLUDED
#endif
