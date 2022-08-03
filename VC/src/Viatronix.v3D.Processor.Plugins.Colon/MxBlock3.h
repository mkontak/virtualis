#ifndef MXBLOCK3_INCLUDED // -*- C++ -*-
#define MXBLOCK3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock3 provides typed access to 3D data blocks.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlock3.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxBlock3 : public MxBlock<T>
{
private:
    uint4 W, H, D;

protected:
    MxBlock3() { }

    void init_block(uint4 w, uint4 h, uint4 d)
	{ W=w; H=h; D=d;  MxBlock<T>::init_block(w*h*d); }
    void resize_block(uint4 w, uint4 h, uint4 d)
	{ W=w; H=h; D=d;  MxBlock<T>::resize_block(w*h*d); }
    void free_block() { MxBlock<T>::free_block(); }

    T& raw(uint4 i, uint4 j, uint4 k) { return MxBlock<T>::raw(k*W*H + j*W + i); }
    const T& raw(uint4 i, uint4 j, uint4 k) const
		{ return MxBlock<T>::raw(k*W*H + j*W + i); }

#define __MXCK(i,j,k) AssertBound(i<W); AssertBound(j<H); AssertBound(k<D)

public:
    MxBlock3(uint4 w, uint4 h, uint4 d) { init_block(w, h, d); }
    ~MxBlock3() { free_block(); }

    T&       ref(uint4 i,uint4 j,uint4 k)       {__MXCK(i,j,k);return raw(i,j,k);}
    const T& ref(uint4 i,uint4 j,uint4 k) const {__MXCK(i,j,k);return raw(i,j,k);}
    T&       operator()(uint4 i,uint4 j,uint4 k)       { return ref(i,j,k); }
    const T& operator()(uint4 i,uint4 j,uint4 k) const { return ref(i,j,k); }

    uint4 width() const { return W; }
    uint4 height() const { return H; }
    uint4 depth() const { return D; }

    void resize(uint4 w, uint4 h, uint4 d) { resize_block(w,h,d); }

#undef __MXCK
};

// MXBLOCK3_INCLUDED
#endif
