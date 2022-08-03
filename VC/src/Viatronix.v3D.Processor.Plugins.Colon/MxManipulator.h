#ifndef MXMANIPULATOR_INCLUDED // -*- C++ -*-
#define MXMANIPULATOR_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxManipulator

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxManipulator.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxGeom3D.h"
#include "Typedef.h"
#include "DocumentationVC2k.h"

class MxManipulator 
{
public:

    virtual void init(const MxBounds&, float4) { }

    bool mouse(int4 kind, int4 which, int4 x, int4 y)
	{
	    if( kind&0xf == 1 ) return mouse_down(which, x, y);
	    else if( kind&0xf == 2 ) return mouse_up(which, x, y);
	    else return motion(x, y);
	}
    virtual bool mouse_down(int4 which, int4 x, int4 y, int4 kind=1) = 0;
    virtual bool mouse_up(int4 which, int4 x, int4 y, int4 kind=2) = 0;
    virtual bool motion(int4 x, int4 y) = 0;
    virtual bool animate() { return false; }

    virtual void apply() = 0;
    virtual void unapply() = 0;
    virtual void draw() { }
};

// MXMANIPULATOR_INCLUDED
#endif
