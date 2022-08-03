/************************************************************************

  Heap data structure

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxHeap.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxHeap.h"


////////////////////////////////////////////////////////////////////////
//
// Internal functions for manipulating the heap

inline void MxHeap::place(MxHeapable *x, uint4 i)
{
    ref(i) = x;
    x->set_heap_pos(i);
}

void MxHeap::swap(uint4 i, uint4 j)
{
    MxHeapable *tmp = ref(i);

    place(ref(j), i);
    place(tmp, j);
}

void MxHeap::upheap(uint4 i)
{
    MxHeapable *moving = ref(i);
    uint4 index = i;
    uint4 p = parent(i);

    while( index>0 && moving->heap_key() > ref(p)->heap_key() )
    {
	place(ref(p), index);
	index = p;
	p = parent(p);
    }

    if( index != i )
	place(moving, index);
}

void MxHeap::downheap(uint4 i)
{
    MxHeapable *moving = ref(i);
    uint4 index = i;
    uint4 l = left(i);
    uint4 r = right(i);
    uint4 largest;

    while( l<length() )
    {
	if( r<length() && ref(l)->heap_key() < ref(r)->heap_key() )
	    largest = r;
	else 
	    largest = l;

	if( moving->heap_key() < ref(largest)->heap_key() )
	{
	    place(ref(largest), index);
	    index = largest;
	    l = left(index);
	    r = right(index);
	}
	else
	    break;
    }

    if( index != i )
	place(moving, index);
}

////////////////////////////////////////////////////////////////////////
//
// Exported interface to the heap
//

void MxHeap::insert(MxHeapable *t, float4 v)
{
    t->heap_key(v);

    uint4 i = add(t);
    t->set_heap_pos(i);

    upheap(i);
}

void MxHeap::update(MxHeapable *t, float4 v)
{
    SanityCheck( t->is_in_heap() );
    t->heap_key(v);

    uint4 i = t->get_heap_pos();

    if( i>0 && v>ref(parent(i))->heap_key() )
	upheap(i);
    else
	downheap(i);
}

MxHeapable *MxHeap::extract()
{
    if( length() < 1 ) return NULL;

    swap(0, length()-1);
    MxHeapable *dead=drop();

    downheap(0);
    dead->not_in_heap();
    return dead;
}

MxHeapable *MxHeap::remove(MxHeapable *t)
{
    if( !t->is_in_heap() ) return NULL;

    int4 i = t->get_heap_pos();
    swap(i, length()-1);
    drop();
    t->not_in_heap();

    if( ref(i)->heap_key() < t->heap_key() )
	downheap(i);
    else
	upheap(i);

    return t;
}
