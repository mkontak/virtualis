#ifndef MXRASTER_INCLUDED // -*- C++ -*-
#define MXRASTER_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxRaster

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxRaster.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxBlock.h"

class MxRaster : private MxBlock<uint1>
{
private:
    uint4 W, H, nchan;

public:

    MxRaster(uint4 w, uint4 h, uint4 c)
	: MxBlock<uint1>(w*h*c)  { W=w;  H=h;  nchan=c; }

    uint4 width() const { return W; }
    uint4 height() const { return H; }
    uint4 channels() const { return nchan; }
    uint4 length() const { return MxBlock<uint1>::length(); }

#ifdef __GNUC__
    uint1& operator[](uint4 i)       { return raw(i); }
    uint1  operator[](uint4 i) const { return raw(i); }
#endif
    operator uint1 *()
	{ return MxBlock<uint1>::operator uint1*(); }
    operator const uint1 *() const
	{ return MxBlock<uint1>::operator const uint1*(); }

    uint1 *pixel(uint4 i, uint4 j)
	{
	    AssertBound(i<W && j<H);
	    return ((uint1 *)*this) + (j*W + i)*nchan;
	}

    const uint1 *pixel(uint4 i, uint4 j) const
	{
	    AssertBound(i<W && j<H);
	    return ((const uint1 *)*this) + (j*W + i)*nchan;
	}

    uint1 *operator()(uint4 i, uint4 j) { return pixel(i,j); }
    const uint1 *operator()(uint4 i, uint4 j) const {return pixel(i,j);}

    void reverse(uint4 start=0, uint4 end=MXID_NIL);
    void hflip();
    void vflip();
};

////////////////////////////////////////////////////////////////////////
//
// File I/O primitives for rasters.
// The basic format supported is PNM files.  Support for this format
// is built-in, and thus always available.
// 
// extern ostream& pnm_write_header(ostream&,const MxRaster&, bool is_raw=true);
// extern ostream& pnm_write_data(ostream&,const MxRaster&, bool is_raw=true);
// inline ostream& pnm_write(ostream& out, const MxRaster& r, bool is_raw=true)
// { pnm_write_header(out,r,is_raw);  return pnm_write_data(out, r, is_raw); }

// extern MxRaster *pnm_read(istream&);

// (Optional) support for TIFF files.
// If libtiff is not available, these procedures will always fail.
//
// extern bool tiff_write(const int1 *filename, const MxRaster&);
// extern MxRaster *tiff_read(const int1 *filename);

////////////////////////////////////////////////////////////////////////
//
// OpenGL support
//
// #ifdef MXGL_INCLUDED
/*inline void glTexImage(const MxRaster& tex, GLenum target=GL_TEXTURE_2D)
{
    glTexImage2D(target, 0, 3, tex.width(), tex.height(),
		 0, GL_RGB, GL_UNSIGNED_BYTE, (const uint1 *)tex);
}

inline void gluMipmaps(const MxRaster& tex, GLenum target=GL_TEXTURE_2D)
{
    gluBuild2DMipmaps(target, 3, tex.width(), tex.height(),
		      GL_RGB, GL_UNSIGNED_BYTE, (const uint1 *)tex);
}
*/
//inline void glTexImage(const MxRaster *tex, GLenum target=GL_TEXTURE_2D)
//	{ glTexImage(*tex, target); }
//inline void gluMipmaps(const MxRaster *tex, GLenum target=GL_TEXTURE_2D)
//	{ gluMipmaps(*tex, target); }
// #endif

// MXRASTER_INCLUDED
#endif
