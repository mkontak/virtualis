/************************************************************************

  MxBlockModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlockModel.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxBlockModel.h"
#include "MxVector.h"

////////////////////////////////////////////////////////////////////////
//
// Basic allocation routines
//

MxFaceID MxBlockModel::alloc_face(MxVertexID v1, MxVertexID v2, MxVertexID v3)
{
    return faces.add(MxFace(v1,v2,v3));
}

MxVertexID MxBlockModel::alloc_vertex(float4 x, float4 y, float4 z)
{
    return vertices.add(MxVertex(x,y,z));
}

MxVertexID MxBlockModel::add_vertex(float4 x, float4 y, float4 z)
{
    MxVertexID id = alloc_vertex(x,y,z);
    init_vertex(id);
    return id;
}

void MxBlockModel::remove_vertex(MxVertexID v)
{
    AssertBound( v < vertices.length() );

    free_vertex(v);
    vertices.remove(v);
    if( normal_binding() == MX_PERVERTEX ) normals->remove(v);
    if( color_binding() == MX_PERVERTEX ) colors->remove(v);
    if( texcoord_binding() == MX_PERVERTEX ) tcoords->remove(v);
}

void MxBlockModel::remove_face(MxFaceID f)
{
    AssertBound( f < faces.length() );

    free_face(f);
    faces.remove(f);
    if( normal_binding() == MX_PERFACE ) normals->remove(f);
    if( color_binding() == MX_PERFACE ) colors->remove(f);
    if( texcoord_binding() == MX_PERFACE ) tcoords->remove(f);
}

MxFaceID MxBlockModel::add_face(uint4 v1,
				uint4 v2,
				uint4 v3,
				bool will_link)
{
    MxFaceID id = alloc_face(v1, v2, v3);
    if( will_link )  init_face(id);
    return id;
}

uint4 MxBlockModel::add_color(float4 r, float4 g, float4 b)
{
    assert( colors );
    MxColor c(r, g, b);
    return colors->add(c);
}

uint4 MxBlockModel::add_normal(float4 x, float4 y, float4 z)
{
    MxNormal n(x, y, z);
    return normals->add(n);
}

/* uint4 MxBlockModel::add_texcoord(float4 s, float4 t)
{
    return tcoords->add(MxTexCoord(s,t));
} */

/*uint4 MxBlockModel::add_texmap(MxRaster *t, const int1 *name)
{
    if( !t ) return MXID_NIL;
    if( !name ) name = "tex";

    if( tex ) delete tex;
    if( tex_name ) delete tex_name;

    tex_name = strdup(name);
    tex = t;
    return 0;
} */

////////////////////////////////////////////////////////////////////////
//
// Property binding
//

static const int1 *bindings[] = {
    "unbound",
    "face",
    "vertex",
    NULL
};

static
uint4 binding_size(MxBlockModel& m, uint1 i)
{
    switch( i )
    {
    case MX_UNBOUND: return 0;
    case MX_PERVERTEX: return MAX(1, m.vert_count());
    case MX_PERFACE: return MAX(1, m.face_count());
    default: return 0;
    }
}

const int1 *MxBlockModel::binding_name(int4 b)
{
    if( b > MX_MAX_BINDING )
	return NULL;
    else
	return bindings[b];
}
    
int4 MxBlockModel::parse_binding(const int1 *name)
{
    for(int4 i=0; i<=MX_MAX_BINDING; i++)
	if( streq(bindings[i], name) )  return i;

    return MX_UNBOUND;
}

void MxBlockModel::color_binding(uint1 b)
{
    int4 size = binding_size(*this, b);

    if( b==MX_UNBOUND )
    {
	if( colors ) { delete colors; colors=NULL; }
	binding_mask &= (~MX_COLOR_MASK);
    }
    else
    {
	if( colors )
	    colors->reset();
	else
	    colors = new MxDynBlock<MxColor>(size);
	binding_mask |= MX_COLOR_MASK;
    }

    cbinding=b;
}

void MxBlockModel::normal_binding(uint1 b)
{
    int4 size = binding_size(*this, b);

    if( b==MX_UNBOUND )
    {
	if( normals ) { delete normals; normals=NULL; }
	binding_mask &= (~MX_NORMAL_MASK);
    }
    else
    {
	if( normals )
	    normals->reset();
	else
	    normals = new MxDynBlock<MxNormal>(size);
	binding_mask |= MX_NORMAL_MASK;
    }

    nbinding=b;
}

void MxBlockModel::texcoord_binding(uint1 b)
{
    if( b!=MX_UNBOUND && b!=MX_PERVERTEX )
	fatal_error("Illegal texture coordinate binding.");

    int4 size = binding_size(*this, b);
    if( tcoords )  tcoords->reset();
    else tcoords = new MxDynBlock<MxTexCoord>(size);

    tbinding = b;
}

////////////////////////////////////////////////////////////////////////
//
// Utility methods for computing intacteristics of faces.
//

void MxBlockModel::compute_face_normal(MxFaceID f, float4 *n, bool will_unitize)
{
    float4 *v1 = vertex(face(f)[0]);
    float4 *v2 = vertex(face(f)[1]);
    float4 *v3 = vertex(face(f)[2]);

    float4 a[3], b[3];

    mxv_sub(a, v2, v1, 3);
    mxv_sub(b, v3, v1, 3);
    mxv_cross3(n, a, b);
    if( will_unitize )
	mxv_unitize(n, 3);
}

void MxBlockModel::compute_face_normal(MxFaceID f, float8 *n,bool will_unitize)
{
    float4 *v1 = vertex(face(f)[0]);
    float4 *v2 = vertex(face(f)[1]);
    float4 *v3 = vertex(face(f)[2]);

    float8 a[3], b[3];
    for(int4 i=0; i<3; i++) { a[i] = v2[i]-v1[i];  b[i] = v3[i]-v1[i]; }

    mxv_cross3(n, a, b);
    if( will_unitize )
	mxv_unitize(n, 3);
}

float8 MxBlockModel::compute_face_area(MxFaceID f)
{
    float8 n[3];

    compute_face_normal(f, n, false);
    return 0.5 * mxv_norm(n, 3);
}

float8 MxBlockModel::compute_face_perimeter(MxFaceID fid, bool *flags)
{
    float8 perim = 0.0;
    const MxFace& f = face(fid);

    for(uint4 i=0; i<3; i++)
    {
	if( !flags || flags[i] )
	{
	    float4 *vi = vertex(f[i]),  *vj = vertex(f[(i+1)%3]), e[3];
	    perim += mxv_norm(mxv_sub(e, vi, vj, 3), 3);
	}
    }

    return perim;
}

float8 MxBlockModel::compute_corner_angle(MxFaceID f, uint4 i)
{
    uint4 i_prev = (i==0)?2:i-1;
    uint4 i_next = (i==2)?0:i+1;

    float4 e_prev[3], e_next[3];
    mxv_unitize(mxv_sub(e_prev, corner(f, i_prev), corner(f, i), 3), 3);
    mxv_unitize(mxv_sub(e_next, corner(f, i_next), corner(f, i), 3), 3);

    return acos(mxv_dot(e_prev, e_next, 3));
}
