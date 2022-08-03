#ifndef MXSTDMODEL_INCLUDED // -*- C++ -*-
#define MXSTDMODEL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxStdModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdModel.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/


#include "MxBlockModel.h"

typedef MxSizedDynBlock<uint4, 6> MxFaceList;
typedef MxSizedDynBlock<uint4, 6> MxVertexList;



class MxPairContraction : public MxEQ
{
public:
    MxPairContraction() { }
    MxPairContraction(const MxPairContraction& c) { *this = c; }

    MxPairContraction& operator=(const MxPairContraction& c);

    MxVertexID v1, v2;
    float4 dv1[3], dv2[3];  // dv2 is not really necessary

    uint4 delta_pivot;
    MxFaceList delta_faces;
    MxFaceList dead_faces;
};

class MxFaceContraction : public MxEQ
{
public:

    MxFaceID f;
    float4 dv1[3], dv2[3], dv3[3];

    MxFaceList delta_faces;
    MxFaceList dead_faces;
};

typedef MxPairContraction MxPairExpansion;

// Masks for internal tag bits
#define MX_VALID_FLAG 0x01
#define MX_PROXY_FLAG 0x02
#define MX_TOUCHED_FLAG 0x04

class MxStdModel : public MxBlockModel
{
private:
    struct vertex_data : public MxEQ {
        uint1 mark, tag;             // Internal tag bits
        uint1 user_mark, user_tag;   // External tag bits
    };
    struct face_data : public MxEQ {
        uint1 mark, tag;             // Internal tag bits
        uint1 user_mark, user_tag;   // External tag bits
    };

    MxDynBlock<vertex_data> v_data;
    MxDynBlock<face_data> f_data;
    MxDynBlock<MxFaceList *> face_links;

protected:

    //
    // Accessors for internal tag and mark bits
    uint4 v_check_tag(MxVertexID i, uint4 tag) const {return v_data(i).tag&tag;}
    void v_set_tag(MxVertexID i, uint4 tag) { v_data(i).tag |= tag; }
    void v_unset_tag(MxVertexID i, uint4 tag) { v_data(i).tag &= ~tag; }
    uint1 vmark(MxVertexID i) const { return v_data(i).mark; }
    void vmark(MxVertexID i, uint1 m) { v_data(i).mark = m; }

    uint4 f_check_tag(MxFaceID i, uint4 tag) const { return f_data(i).tag&tag; }
    void f_set_tag(MxFaceID i, uint4 tag) { f_data(i).tag |= tag; }
    void f_unset_tag(MxFaceID i, uint4 tag) { f_data(i).tag &= ~tag; }
    uint1 fmark(MxFaceID i) const { return f_data(i).mark; }
    void fmark(MxFaceID i, uint1 m) { f_data(i).mark = m; }

protected:
    MxVertexID alloc_vertex(float4, float4, float4);
    void free_vertex(MxVertexID);
    void free_face(MxFaceID);
    MxFaceID alloc_face(MxVertexID, MxVertexID, MxVertexID);
    void init_face(MxFaceID);

public:
    MxStdModel(uint4 nvert, uint4 nface)
	: MxBlockModel(nvert,nface),
	  v_data(nvert), f_data(nface), face_links(nvert)
	{
	}
    virtual ~MxStdModel();

    ////////////////////////////////////////////////////////////////////////
    //  Tagging and marking
    //
    uint4 vertex_is_valid(MxVertexID i) const
	{ return v_check_tag(i,MX_VALID_FLAG); }
    void vertex_mark_valid(MxVertexID i) { v_set_tag(i,MX_VALID_FLAG); }
    void vertex_mark_invalid(MxVertexID i) { v_unset_tag(i,MX_VALID_FLAG); }

    uint4 face_is_valid(MxFaceID i) const {return f_check_tag(i,MX_VALID_FLAG);}
    void face_mark_valid(MxFaceID i) { f_set_tag(i,MX_VALID_FLAG); }
    void face_mark_invalid(MxFaceID i) { f_unset_tag(i,MX_VALID_FLAG); }

    uint4 vertex_is_proxy(MxVertexID i) const
	{ return v_check_tag(i,MX_PROXY_FLAG); }
    void vertex_mark_proxy(MxVertexID i) { v_set_tag(i,MX_PROXY_FLAG); }
    void vertex_mark_nonproxy(MxVertexID i) { v_unset_tag(i,MX_PROXY_FLAG); }

    //
    // Accessors for external tag and mark bits
    uint4 vertex_check_tag(MxVertexID i, uint4 tag) const
	{ return v_data(i).user_tag&tag; }
    void vertex_set_tag(MxVertexID i, uint4 tag) { v_data(i).user_tag|=tag; }
    void vertex_unset_tag(MxVertexID i, uint4 tag) {v_data(i).user_tag&= ~tag;}
    uint1 vertex_mark(MxVertexID i) { return v_data(i).user_mark; }
    void vertex_mark(MxVertexID i, uint1 m) { v_data(i).user_mark=m; }

    uint4 face_check_tag(MxFaceID i, uint4 tag) const
	{ return f_data(i).user_tag&tag; }
    void face_set_tag(MxFaceID i, uint4 tag) { f_data(i).user_tag|=tag; }
    void face_unset_tag(MxFaceID i, uint4 tag) {f_data(i).user_tag&= ~tag;}
    uint1 face_mark(MxFaceID i) { return f_data(i).user_mark; }
    void face_mark(MxFaceID i, uint1 m) { f_data(i).user_mark = m; }


    ////////////////////////////////////////////////////////////////////////
    //  Vertex proxy management and proxy-aware accessors
    //
    MxVertexID add_proxy_vertex(MxVertexID);
    MxVertexID& vertex_proxy_parent(MxVertexID);
    MxVertexID resolve_proxies(MxVertexID v);
    float4 *vertex_position(MxVertexID v);


    ////////////////////////////////////////////////////////////////////////
    //  Neighborhood collection and management
    //
    void mark_neighborhood(MxVertexID, uint2 mark=0);
    void collect_unmarked_neighbors(MxVertexID, MxFaceList& faces);
    void mark_neighborhood_delta(MxVertexID, int2 delta);
    void partition_marked_neighbors(MxVertexID, uint2 pivot,
				    MxFaceList& below, MxFaceList& above);

    void mark_corners(const MxFaceList& faces, uint2 mark=0);
    void collect_unmarked_corners(const MxFaceList& faces,MxVertexList& verts);

    void collect_edge_neighbors(MxVertexID, MxVertexID, MxFaceList&);
    void collect_vertex_star(MxVertexID v, MxVertexList& verts);

    MxFaceList& neighbors(MxVertexID v) { return *face_links(v); }
    const MxFaceList& neighbors(MxVertexID v) const { return *face_links(v); }

    void compute_vertex_normal(MxVertexID v, float4 *);
    void synthesize_normals();

    ////////////////////////////////////////////////////////////////////////
    // Primitive transformation operations
    //
    void remap_vertex(MxVertexID from, MxVertexID to);
    MxVertexID split_edge(MxVertexID v1,MxVertexID v2,float4 x,float4 y,float4 z);
    MxVertexID split_edge(MxVertexID v1, MxVertexID v2);

    void flip_edge(MxVertexID v1, MxVertexID v2);

    // split_face3
    void split_face4(MxFaceID f, MxVertexID *newverts=NULL);

    void unlink_face(MxFaceID f);

    ////////////////////////////////////////////////////////////////////////
    // Contraction and related operations
    //
    void compact_vertices();
    void remove_degeneracy(MxFaceList&);

    // Pair contraction interface
    void compute_contraction(MxVertexID, MxVertexID,
			     MxPairContraction *, const float4 *vnew=NULL);
    void apply_contraction(const MxPairContraction&);
    void apply_expansion(const MxPairExpansion&);
    void contract(MxVertexID v1, MxVertexID v2,
		  const float4 *, MxPairContraction *);

    // Triple contraction interface
    void compute_contraction(MxFaceID, MxFaceContraction *);
    void contract(MxVertexID v1, MxVertexID v2, MxVertexID v3,
		  const float4 *vnew, MxFaceList& changed);

    // Generalized contraction interface
    void contract(MxVertexID v1, const MxVertexList& rest,
		  const float4 *vnew, MxFaceList& changed);
};


//extern void mx_render_model(MxStdModel&);
//extern void mx_draw_mesh(MxStdModel&, float8 *color=NULL);
//extern void mx_draw_wireframe(MxStdModel&, float8 *color=NULL);
//extern void mx_draw_boundaries(MxStdModel&);
//extern void mx_draw_pointcloud(MxStdModel&);

// MXSTDMODEL_INCLUDED
#endif
