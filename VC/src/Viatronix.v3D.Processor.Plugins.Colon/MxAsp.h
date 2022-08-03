#ifndef MXASP_INCLUDED // -*- C++ -*-
#define MXASP_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxAsp

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxAsp.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxDynBlock.h"

enum MxAspType
{
  MXASP_NULL=0,
  MXASP_BOOL=1,
  MXASP_INT=2,
  MXASP_UINT=3,
  MXASP_FLOAT=4,
  MXASP_DOUBLE=5,
  MXASP_PROC=6,
  MXASP_USER_TYPE=7
};

class MxAspVar;
class MxAspStore;

typedef void (*MxAspProc)(MxAspVar *, MxAspStore *);

struct MxAspProcDefn
{
  MxAspProc proc;
  MxAspType arg_type;
  uint4 args_expected;
};


class MxAspVar : public MxEQ
{
private:
  MxAspType type;
  union {
	  void     *any;
    bool     *bool_var;
    int4     *int4_var;
    uint4    *uint4_var;
    float4   *float4_var;
    float8   *float8_var;
    MxAspProcDefn *proc_var;
  } as;

  uint4 size;
  uint4 flags;
  int1 *name;

protected:
  void clear();

public:
  MxAspVar() { clear(); }
  MxAspVar(const int1 *n,MxAspType t,void *v,uint4 len=1)
	{ clear(); bind(n,t,v,len); }
    ~MxAspVar() { unbind(); }

    void bind(const int1 *, MxAspType, void *, uint4 len=1);
    void unbind();

    const int1 *nameof() const { return name; }
    uint4 length() const { return size; }
    MxAspType get_type() const { return type; }
    ostream& print(ostream&) const;

    void own_memory(bool);
    bool is_memory_owned();

#define mxasp_defset(V)  bool set_##V(V, uint4 i=0)
    mxasp_defset(bool);
    mxasp_defset(int4);
    mxasp_defset(uint4);
    mxasp_defset(float4);
    mxasp_defset(float8);
#undef mxasp_defset

#define mxasp_defget(V)  V *get_##V(V *, uint4 i=0)
    mxasp_defget(bool);
    mxasp_defget(int4);
    mxasp_defget(uint4);
    mxasp_defget(float4);
    mxasp_defget(float8);
#undef mxasp_defget

    bool read_from(const int1 *, uint4 i=0);
    bool apply(int4 argc, const int1 **argv, MxAspStore *store);
    bool apply(MxAspVar *arg, MxAspStore *store);
};


class MxAspStore 
{
private:
    MxDynBlock<MxAspVar> vars;

public:
    MxAspStore();

    MxAspVar *lookup(const int1 *name) const;
    MxAspVar *defvar(const int1 *name, MxAspType, void *, uint4 len=1);
    MxAspVar *defun(const int1 *name, MxAspProc proc,
		    MxAspType type=MXASP_NULL, uint4 expects=0);

    bool execute_command(int4 argc, const int1 *cmd, const int1 **argv);

    void write(ostream&);
};

inline ostream& operator<<(ostream& out, const MxAspVar& v)
{ return out<<v.nameof()<<" ",v.print(out); }

extern MxAspType mxasp_type_from_name(const int1 *);
extern void *mxasp_alloc_value(MxAspType, uint4 size=1);
extern void mxasp_apply(MxAspVar *proc, MxAspVar *arg, MxAspStore *store);
extern void mxasp_apply(MxAspProc proc, MxAspVar *arg, MxAspStore *store);

// MXASP_INCLUDED
#endif
