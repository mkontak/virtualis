#ifndef MXHEAP_INCLUDED // -*- C++ -*-
#define MXHEAP_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Heap

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxHeap.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxDynBlock.h"

class MxHeapable 
{
private:
    float4 import;
    int4 token;

public:
    MxHeapable() { not_in_heap(); heap_key(0.0f); }

    inline bool is_in_heap() { return token != -47; }
    inline void not_in_heap() { token = -47; }
    inline int4 get_heap_pos() { return token; }
    inline void set_heap_pos(int4 t) { token=t; }

    inline void  heap_key(float4 k) { import=k; }
    inline float4 heap_key() const  { return import; }
};


class MxHeap : private MxDynBlock<MxHeapable *>
{
private:
    void place(MxHeapable *x, uint4 i);
    void swap(uint4 i, uint4 j);

    uint4 parent(uint4 i) { return (i-1)/2; }
    uint4 left(uint4 i) { return 2*i+1; }
    uint4 right(uint4 i) { return 2*i+2; }

    void upheap(uint4 i);
    void downheap(uint4 i);

public:
    MxHeap() : MxDynBlock<MxHeapable *>(8) { }
    MxHeap(uint4 n) : MxDynBlock<MxHeapable *>(n) { }

    void insert(MxHeapable *t) { insert(t, t->heap_key()); }
    void insert(MxHeapable *, float4);
    void update(MxHeapable *t) { update(t, t->heap_key()); }
    void update(MxHeapable *, float4);

    uint4 size() const { return length(); }
    MxHeapable       *item(uint4 i)       { return ref(i); }
    const MxHeapable *item(uint4 i) const { return ref(i); }
    MxHeapable *extract();
    MxHeapable *top() { return (length()<1 ? (MxHeapable *)NULL : raw(0)); }
    MxHeapable *remove(MxHeapable *);
};

// MXHEAP_INCLUDED
#endif
