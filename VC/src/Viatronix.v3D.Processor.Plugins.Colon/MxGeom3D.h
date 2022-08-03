#ifndef MXGEOM3D_INCLUDED // -*- C++ -*-
#define MXGEOM3D_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Handy 3D geometrical primitives

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGeom3D.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxVec3.h"
#include "MxVec4.h"

// extern void mx3d_box_corners(const Vec3& min, const Vec3& max, Vec3 *v);

class MxBounds 
{
public:

    Vec3 min, max;
    Vec3 center;
    float8 radius;
    uint4 points;

    void reset();
    void add_point(const float8 *v, bool will_update=true);
    void add_point(const float4 *v, bool will_update=true);
    void complete();
    void merge(const MxBounds&);

    MxBounds() { reset(); }
};

// MXGEOM3D_INCLUDED
#endif
