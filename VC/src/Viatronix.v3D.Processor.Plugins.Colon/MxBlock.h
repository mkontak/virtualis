#ifndef MXBLOCK_INCLUDED // -*- C++ -*-
#define MXBLOCK_INCLUDED
#if !defined(__GNUC__)
  #pragma once
#endif

/************************************************************************

  MxBlock provides typed access a contiguous block of elements.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

  $Id: MxBlock.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"

template<class T>
class MxBlock : public MxEQ {
private:
  uint4 N;
  T * block;

protected:
  MxBlock() {}

  void init_block(uint4 n)
  {
    // Allocate memory for block
    N = n; 
		block = new T [n];
  }

  void resize_block(uint4 n)
  {
    T *old = block;
    block = new T [n];

    memmove(block, old, sizeof(T) * N);
    
    N = n;
    delete [] old;
  }

  void free_block()
  {
#ifdef WIN32
    // The Microsoft compiler has a problem with the standard syntax below.  
    // For some reason, expanding it into the following pointer-based
    // version makes it work.  Don't ask me why.
    delete [] block;
#else
    // Call the relevant destructors for each element before
    // freeing the block.  Has now effect for types like 'int'.
    //for ( uint4 i=0; i<N; i++ ) 
   // {
	//		block[i].~T();
		//}
    delete [] block;
#endif
  }

  T & raw(uint4 i) { return (block[i]);}
  const T& raw(uint4 i) const { return (block[i]);}

public:
  MxBlock(uint4 n) { init_block(n);}
  ~MxBlock() { free_block();}

  // Standard accessors
  //
  T&       ref(uint4 i)              { AssertBound(i<N);return (block[i]);}
  const T& ref(uint4 i) const        { AssertBound(i<N);return (block[i]);}
  T&       operator()(uint4 i)       { return (ref(i));}
  const T& operator()(uint4 i) const { return (ref(i));}
#ifdef __GNUC__
  // Most compilers can apply the casting operators and then use the
  // standard array [].  GCC (2.7.2) seems to need explicit operators.
  //
  T&       operator[](uint4 i)       { return (block[i]);}
  const T& operator[](uint4 i) const { return (block[i]);}
#endif

  operator const T*() const { return (block);}
  operator       T*()       { return (block);}
  uint4 length() const { return (N);}

  // Primitive methods for altering the data block
  //
  void resize(uint4 n) { resize_block(n);}
  void bitcopy(const T *array, uint4 n) // copy bits directly
  { 
    memcpy(block, array, MIN(n,N)*sizeof(T));
  }
  void copy(const T *array, const uint4 n) // copy using assignment operator
  { 
    for (uint4 i=0; i<MIN(n,N); i++) 
      block[i] = array[i];
  }
  void bitcopy(const MxBlock<T>& b) { bitcopy(b, b.length());}
  void copy(const MxBlock<T>& b) { copy(b, b.length());}
};

// MXBLOCK_INCLUDED
#endif