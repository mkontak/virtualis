#ifndef MXGEOPRIMS_INCLUDED // -*- C++ -*-
#define MXGEOPRIMS_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxGeoPrims

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGeoPrims.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "Typedef.h"

//
// Standard names for ID types
//
typedef uint4 MxVertexID;
typedef uint4 MxFaceID;

//
// Convenient nicknames
//
#define VID MxVertexID
#define FID MxFaceID

#if !defined(MX_COLOR_RGBA) && !defined(MX_COLOR_ABGR)
#  define MX_COLOR_RGBA
#endif

class MxColor : public MxEQ
{
private:
    inline uint1 _ftop(float4 x)
	{ return (uint1)((x>1.0f?1.0f:x)*255.0f); }
    inline float4 _ptof(uint1 c) const { return ((float4)c) / 255.0f; }

public:
    union {
#if defined(MX_COLOR_RGBA)
	struct { uint1 r, g, b, a; } chan;
#elif defined(MX_COLOR_ABGR)
	struct { uint1 a, b, g, r; } chan;
#else
#  error "Packed color format illegal or left unspecified"
#endif
	uint4 word;
    } as;

    MxColor() { }
    MxColor(float4 r, float4 g, float4 b) { as.chan.a=0; set(r, g, b); }

    void set(float4 r, float4 g, float4 b)
	{ as.chan.r=_ftop(r);  as.chan.g=_ftop(g);  as.chan.b=_ftop(b); }
    void set(const float4 *c)
	{ as.chan.r=_ftop(c[0]);as.chan.g=_ftop(c[1]);as.chan.b=_ftop(c[2]); }
    void set(const float8 *c)
	{ as.chan.r=_ftop(c[0]);as.chan.g=_ftop(c[1]);as.chan.b=_ftop(c[2]); }

    float4 R() const { return _ptof(as.chan.r); }
    float4 G() const { return _ptof(as.chan.g); }
    float4 B() const { return _ptof(as.chan.b); }
};

class MxTexCoord : public MxEQ
{
public:
    float4 u[2];

    MxTexCoord() { }
    MxTexCoord(float4 s, float4 t) { u[0]=s; u[1]=t; }
    MxTexCoord(const MxTexCoord& t) { *this = t; }

    float4& operator[](int4 i)       { return u[i]; }
    float4  operator[](int4 i) const { return u[i]; }
    operator const float4*() const { return u; }
    operator       float4*()       { return u; }

    MxTexCoord& operator=(const MxTexCoord& t)
	{ u[0]=t[0];  u[1]=t[1]; return *this; }
};


class MxVertex : public MxEQ
{
public:
    union {
	float4 pos[3];
	struct { MxVertexID parent, next, prev; } proxy;
    } as;

    MxVertex() { }
    MxVertex(float4 x, float4 y, float4 z) {as.pos[0]=x;as.pos[1]=y;as.pos[2]=z;}
    MxVertex(const MxVertex& v) { *this = v; }
    
    MxVertex& operator=(const MxVertex& v)
	{ as.pos[0]=v.as.pos[0]; as.pos[1]=v.as.pos[1];
	  as.pos[2]=v.as.pos[2]; return *this; }
    operator const float4*() const { return as.pos; }
    operator float4*() { return as.pos; }

    float4& operator()(int4 i)       { return as.pos[i]; }
    float4  operator()(int4 i) const { return as.pos[i]; }
    //
    // The [] operator is preferred over ()
    //
    float4& operator[](int4 i)       { return as.pos[i]; }
    float4  operator[](int4 i) const { return as.pos[i]; }
};

class MxNormal : public MxEQ
{
private:
    inline int2 _ftos(float4 x)
	{ return (int2)rint((x>1.0f?1.0f:x)*(float4)SHRT_MAX); }
    inline int2 _dtos(float8 x)
	{ return (int2)rint((x>1.0?1.0:x)*(float8)SHRT_MAX); }

    inline float4 _stof(int2 s) const { return (float4)s/(float4)SHRT_MAX; }
    inline float8 _stod(int2 s) const { return (float8)s/(float8)SHRT_MAX; }

    int2 dir[3];

public:
    MxNormal() { }
    MxNormal(float4 x, float4 y, float4 z) { set(x,y,z); }
    MxNormal(const float4 *v) { set(v); }
    MxNormal(const float8 *v) { set(v); }

    inline void set(float8 x, float8 y, float8 z)
	{ dir[0]=_dtos(x);  dir[1]=_dtos(y);  dir[2]=_dtos(z); }
    inline void set(const float4 *v)
	{ dir[0]=_ftos(v[0]);  dir[1]=_ftos(v[1]);  dir[2]=_ftos(v[2]); }
    inline void set(const float8 *v)
	{ dir[0]=_dtos(v[0]);  dir[1]=_dtos(v[1]);  dir[2]=_dtos(v[2]); }

    float4 operator[](uint4 i) const { AssertBound(i<3); return _stof(dir[i]); }
    int2 raw(uint4 i) const { return dir[i]; }
};
    
class MxEdge : public MxEQ
{
public:
    MxVertexID v1, v2;

    MxEdge() { v1=v2=MXID_NIL; }
    MxEdge(MxVertexID a, MxVertexID b) { v1=a; v2=b; }
    MxEdge(const MxEdge& e) { v1=e.v1;  v2=e.v2; }

    MxVertexID opposite_vertex(MxVertexID v)
	{
	    if( v==v1 ) return v2;
	    else { SanityCheck(v==v2); return v1; }
	}
};

class MxFace : public MxEQ
{
public:
    MxVertexID v[3];

    MxFace() { }
    MxFace(MxVertexID v0, MxVertexID v1, MxVertexID v2)
	{ v[0]=v0;  v[1]=v1;  v[2]=v2; }
    MxFace(const MxFace& f) { v[0]=f.v[0]; v[1]=f.v[1]; v[2]=f.v[2]; }

    MxVertexID& operator()(int4 i)       { return v[i]; }
    MxVertexID  operator()(int4 i) const { return v[i]; }
    //
    // The [] operator is now preferred over the () operator.
    //
    MxVertexID& operator[](int4 i)       { return v[i]; }
    MxVertexID  operator[](int4 i) const { return v[i]; }

    int4 remap_vertex(MxVertexID from, MxVertexID to)
	{
	    int4 nmapped = 0;
	    for(int4 i=0; i<3; i++)
		if( v[i]==from )
		{
		    v[i]=to;
		    nmapped++;
		}
	    return nmapped;
	}

    uint4 find_vertex(MxVertexID i)
	{
	    if( v[0]==i ) return 0;
	    else if( v[1]==i ) return 1;
	    else { SanityCheck(v[2]==i); return 2; }
	}

    MxVertexID opposite_vertex(MxVertexID v0, MxVertexID v1)
	{
	    if( v[0]!=v0 && v[0]!=v1 ) return v[0];
	    else if( v[1]!=v0 && v[1]!=v1 ) return v[1];
	    else { SanityCheck( v[2]!=v0 && v[2]!=v1 ); return v[2]; }
	}

    bool is_inorder(MxVertexID v0, MxVertexID v1)
	{
	    if( v[0]==v0 ) return v[1]==v1;
	    else if( v[1]==v0 ) return v[2]==v1;
	    else { SanityCheck(v[2]==v0); return v[0]==v1; }
	}
};


inline ostream& operator<<(ostream& out, const MxVertex& v)
{
    return out << "v " << v(0) << " " << v(1) << " " << v(2);
}

inline ostream& operator<<(ostream& out, const MxFace& f)
{
    return out << "f " << f(0)+1 << " " <<  f(1)+1 << " " <<  f(2)+1;
}

inline ostream& operator<<(ostream& out, const MxColor& c)
{
    return out << "c " << c.R() << " " << c.G() << " " << c.B();
}

inline ostream& operator<<(ostream& out, const MxNormal& n)
{
    return out << "n " << n[0] << " " << n[1] << " " << n[2];
}

inline ostream& operator<<(ostream& out, const MxTexCoord& t)
{
    return out << "r " << t[0] << " " << t[1];
}

#ifdef MXGL_INCLUDED
/* inline void glC(const MxColor& c)
{
    glColor3ub(c.as.chan.r, c.as.chan.g, c.as.chan.b);
}
inline void glT(const MxTexCoord& t) { glTexCoord2fv(t); }
inline void glV(const MxVertex& v) { glVertex3fv(v); }
inline void glN(const MxNormal& n) { 
    //
    // !!WARNING!! glNormal3sv doesn't work for some reason, and neither
    // glNormal3s.  Oddly, glNormal3i correctly.  Is this some sort of GL bug
    // or am I doing something wrong??
    //
    glNormal3i(n.raw(0), n.raw(1), n.raw(2));
}

inline void glC(const MxColor *c)    { glC(*c); }
inline void glT(const MxTexCoord *t) { glT(*t); }
inline void glV(const MxVertex *v)   { glV(*v); }
inline void glN(const MxNormal *n)   { glN(*n); } */
#endif

// MXGEOPRIMS_INCLUDED
#endif
