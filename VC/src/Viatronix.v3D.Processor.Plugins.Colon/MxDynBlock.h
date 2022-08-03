#ifndef MXDYNBLOCK_INCLUDED // -*- C++ -*-
#define MXDYNBLOCK_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxDynBlocks are blocks that automatically grow to fit the data added
  to them.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDynBlock.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxBlock.h"
#include "Typedef.h"

template<class T>
class MxDynBlock : public MxBlock<T>
{
private:
  uint4 fill;

public:
  MxDynBlock() 
  { 
    init_block(8); 
    fill=0; 
  }
  
  MxDynBlock(uint4 n) 
  { 
    init_block(n); 
    fill=0; 
  }

  T& operator()(uint4 i)       
  { 
    AssertBound(i<fill); 
    return ref(i); 
  }
    
  const T& operator()(uint4 i) const 
  { 
    AssertBound(i<fill); 
    return ref(i); 
  }

  uint4 length() const 
  { 
    return fill; 
  }
    
  uint4 total_space() const 
  { 
    return MxBlock<T>::length(); 
  }
    
  T& last() 
  { 
    AssertBound(fill>0); 
    return raw(fill-1); 
  }
    
  const T& last() const 
  { 
    AssertBound(fill>0); 
    return raw(fill-1); 
  }
  
  uint4 add()
	{
    if ( length()==total_space() ) resize(total_space() * 2);
    return fill++;
	}
    
  uint4 add(const T& t)
  {
    uint4 i = add();
    raw(i) = t;
    return i;
  }

  void reset() { fill = 0; }
    
  T& drop() { return raw(--fill); }
    
  void drop(uint4 d) { fill -= d; }
    
  bool find(const T& t, uint4 *index=NULL)
  {
    for(uint4 i=0; i<fill; i++)
      if( raw(i) == t )
      {
      if( index ) *index = i;
        return true;
      }
    return false;
  }
    
  void remove(uint4 i)
	{ 
    AssertBound(i<fill); 
    fill--; 
    raw(i) = raw(fill); 
  }
    
  void remove_inorder(uint4 i)
	{ 
    memmove(&raw(i), &raw(i+1), (--fill - i)*sizeof(T)); 
  }
};

template<class T, int4 T_SIZE>
class MxSizedDynBlock : public MxDynBlock<T>
{
public:   
  MxSizedDynBlock(uint4 n=T_SIZE) : MxDynBlock<T>(n) { }
};

// MXDYNBLOCK_INCLUDED
#endif
