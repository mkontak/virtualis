#ifndef MXBLOCK2_INCLUDED // -*- C++ -*-
#define MXBLOCK2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock2 provides typed access to 2D data blocks.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlock2.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxBlock2 : public MxBlock<T>
{
private:
    uint4 W, H;

protected:
    MxBlock2() { }
    void init_block(uint4 w, uint4 h)
	{ W=w;  H=h;  MxBlock<T>::init_block(w*h); }
    void resize_block(uint4 w, uint4 h)
	{ W=w;  H=h;  MxBlock<T>::resize_block(w*h); }

    T&       raw(uint4 i, uint4 j)       { return MxBlock<T>::raw(j*W+i); }
    const T& raw(uint4 i, uint4 j) const { return MxBlock<T>::raw(j*W+i); }

#define __MXCK(i,j) AssertBound(i<W); AssertBound(j<H)

public:
    // Exported constructors/destructors
    // 
    MxBlock2(uint4 w, uint4 h) { init_block(w,h); }

    // Accessors
    //
    T&       ref(uint4 i, uint4 j)              { __MXCK(i,j); return raw(i,j); }
    const T& ref(uint4 i, uint4 j) const        { __MXCK(i,j); return raw(i,j); }
    T&       operator()(uint4 i, uint4 j)       { return ref(i,j); }
    const T& operator()(uint4 i, uint4 j) const { return ref(i,j); }

    uint4 width() const { return W; }
    uint4 height() const { return H; }

    void resize(uint4 w, uint4 h) { resize_block(w,h); }

#undef __MXCK
};


// MXBLOCK2_INCLUDED
#endif
