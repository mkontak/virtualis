#ifndef MXBLOCKMODEL_INCLUDED // -*- C++ -*-
#define MXBLOCKMODEL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlockModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlockModel.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxDynBlock.h"
#include "MxGeoPrims.h"
#include "MxRaster.h"

#define MX_UNBOUND 0x0
#define MX_PERFACE 0x1
#define MX_PERVERTEX 0x2
#define MX_MAX_BINDING 0x2

#define MX_NORMAL_MASK   0x3
#define MX_COLOR_MASK    (0x3<<2)
#define MX_TEXTURE_MASK  (0x3<<4)
#define MX_ALL_MASK      (MX_NORMAL_MASK|MX_COLOR_MASK|MX_TEXTURE_MASK)

class MxBlockModel 
{
private:
    uint1 cbinding, nbinding, tbinding;

    uint4 flags;

    // Required blocks
    MxDynBlock<MxVertex> vertices;
    MxDynBlock<MxFace> faces;

    // Optional blocks
    MxDynBlock<MxNormal> *normals;
    MxDynBlock<MxColor> *colors;
    MxDynBlock<MxTexCoord> *tcoords;
    // prop_block *properties;  // Indirect block for arbitrary properties

    // Optional texture map
    MxRaster *tex;
    int1 *tex_name;

protected:
    virtual MxVertexID alloc_vertex(float4, float4, float4);
    virtual void init_vertex(MxVertexID) { }
    virtual void free_vertex(MxVertexID) { }
    virtual MxFaceID alloc_face(MxVertexID, MxVertexID, MxVertexID);
    virtual void init_face(MxFaceID) { }
    virtual void free_face(MxFaceID) { }

public:
    uint4 binding_mask;

public:
    MxBlockModel(int4 nvert, int4 nface) : vertices(nvert), faces(nface)
	{
	    colors = NULL;  normals = NULL;  tcoords = NULL;
	    cbinding = nbinding = tbinding = MX_UNBOUND;
	    binding_mask = MX_ALL_MASK;
	    tex = NULL;
	    tex_name = NULL;
	}
    virtual ~MxBlockModel()
	{
	    if( normals ) delete normals;
	    if( colors ) delete colors;
	    if( tcoords ) delete tcoords;
	    if( tex ) delete tex;
	    if( tex_name ) delete tex_name;
	}

    uint4 vert_count() const { return vertices.length(); }
    uint4 face_count() const { return faces.length(); }
    uint4 color_count() const { return (colors?colors->length():0); }
    uint4 normal_count() const { return (normals?normals->length():0); }
    uint4 texcoord_count() const { return (tcoords?tcoords->length():0); }

    MxVertexID add_vertex(float4, float4, float4);
    MxFaceID add_face(uint4, uint4, uint4, bool will_link=true);
    uint4 add_color(float4, float4, float4);
    uint4 add_normal(float4, float4, float4);
		// uint add_texcoord(float4, float4);

    MxVertexID add_vertex(float4 *v) { return add_vertex(v[0], v[1], v[2]); }
    MxFaceID add_face(uint4 *f) { return add_face(f[0], f[1], f[2]); }

    void remove_vertex(MxVertexID v);
    void remove_face(MxFaceID f);

    MxVertex& vertex(uint4 i) { return vertices(i); }
    MxFace& face(uint4 i) { return faces(i); }
    MxVertex& corner(MxFaceID f, int2 i) { return vertex(face(f)[i]); }
    MxColor& color(uint4 i) { assert(colors);  return (*colors)(i); }
    MxNormal& normal(uint4 i) { assert(normals); return (*normals)(i); }
    MxTexCoord& texcoord(uint4 i) { assert(tcoords); return (*tcoords)(i); }

    int4 color_binding() { return (cbinding&(binding_mask>>2)); }
    int4 normal_binding() { return (nbinding&binding_mask); }
    int4 texcoord_binding() { return (tbinding&(binding_mask>>4)); }
    void color_binding(uint1 b);
    void normal_binding(uint1 b);
    void texcoord_binding(uint1 b);

    const int1 *binding_name(int4);
    int4 parse_binding(const int1 *);

		// MxRaster *texmap() const { return tex; }
		// const int1 *texmap_name() const { return tex_name; }
		// uint add_texmap(MxRaster *, const int1 *name);

    void compute_face_normal(MxFaceID, float8 *, bool will_unitize=true);
    void compute_face_normal(MxFaceID, float4 *, bool will_unitize=true);
    float8 compute_face_area(MxFaceID);
    float8 compute_face_perimeter(MxFaceID, bool *edge_flags=NULL);

    float8 compute_corner_angle(MxFaceID, uint4);
};


// MXBLOCKMODEL_INCLUDED
#endif
