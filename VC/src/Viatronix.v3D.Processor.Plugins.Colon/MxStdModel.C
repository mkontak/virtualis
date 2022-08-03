/************************************************************************

  MxStdModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdModel.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxStdModel.h"
#include "MxVector.h"

MxPairContraction& MxPairContraction::operator=(const MxPairContraction& c)
{
    v1 = c.v1;
    v2 = c.v2;
    mxv_set(dv1, c.dv1, 3);
    mxv_set(dv2, c.dv2, 3);

    delta_faces.reset();
    dead_faces.reset();

    for(uint4 i=0; i<c.delta_faces.length(); i++)
	delta_faces.add(c.delta_faces[i]);
    for(uint4 j=0; j<c.dead_faces.length(); j++)
	dead_faces.add(c.dead_faces[j]);

    delta_pivot = c.delta_pivot;

    return *this;
}

MxStdModel::~MxStdModel()
{
    for(uint4 i=0; i<face_links.length(); i++)  delete face_links[i];
}

MxVertexID MxStdModel::alloc_vertex(float4 x, float4 y, float4 z)
{
    MxVertexID id = MxBlockModel::alloc_vertex(x,y,z);
    v_data.add();
    v_data(id).tag = 0x0;
    v_data(id).user_tag = 0x0;
    vertex_mark_valid(id);

    uint4 l = face_links.add(new MxFaceList);
    SanityCheck( l == id );
    SanityCheck( neighbors(id).length() == 0 );

    return id;
}

void MxStdModel::free_vertex(MxVertexID v)
{
    delete face_links[v];
    face_links.remove(v);
    v_data.remove(v);
}

MxFaceID MxStdModel::alloc_face(MxVertexID v1, MxVertexID v2, MxVertexID v3)
{
    MxFaceID id = MxBlockModel::alloc_face(v1,v2,v3);
    f_data.add();
    f_data(id).tag = 0x0;
    f_data(id).user_tag = 0x0;
    face_mark_valid(id);

    return id;
}

void MxStdModel::free_face(MxFaceID f)
{
    f_data.remove(f);
}

void MxStdModel::init_face(MxFaceID id)
{
    neighbors(face(id).v[0]).add(id);
    neighbors(face(id).v[1]).add(id);
    neighbors(face(id).v[2]).add(id);
}

void MxStdModel::mark_neighborhood(MxVertexID vid, uint2 mark)
{
    AssertBound( vid < vert_count() ); 

    for(uint4 i=0; i<neighbors(vid).length(); i++)
    {
	uint4 f = neighbors(vid)(i);
	fmark(f, mark);
    }
}

void MxStdModel::collect_unmarked_neighbors(MxVertexID vid,MxFaceList& faces)
{
    AssertBound( vid < vert_count() ); 

    for(uint4 i=0; i<neighbors(vid).length(); i++)
    {
	uint4 fid = neighbors(vid)(i);
	if( !fmark(fid) )
	{
	    faces.add(fid);
	    fmark(fid, 1);
	}
    }
}

void MxStdModel::mark_neighborhood_delta(MxVertexID vid, int2 delta)
{
    AssertBound( vid < vert_count() );
    for(uint4 i=0; i<neighbors(vid).length(); i++)
    {
	uint4 f = neighbors(vid)(i);
	fmark(f, fmark(f)+delta);
    }
}

void MxStdModel::partition_marked_neighbors(MxVertexID v, uint2 pivot,
					    MxFaceList& lo, MxFaceList& hi)
{
    AssertBound( v < vert_count() );
    for(uint4 i=0; i<neighbors(v).length(); i++)
    {
	uint4 f = neighbors(v)(i);
	if( fmark(f) )
	{
	    if( fmark(f) < pivot )  lo.add(f);
	    else  hi.add(f);
	    fmark(f, 0);
	}
    }
}

void MxStdModel::mark_corners(const MxFaceList& faces, uint2 mark)
{
    for(uint4 i=0; i<faces.length(); i++)
	for(uint4 j=0; j<3; j++)
	    vmark(face(faces(i))(j), mark);
}

void MxStdModel::collect_unmarked_corners(const MxFaceList& faces,
					  MxVertexList& verts)
{
    for(uint4 i=0; i<faces.length(); i++)
	for(uint4 j=0; j<3; j++)
	{
	    MxVertexID v = face(faces(i))(j);
	    if( !vmark(v) )
	    {
		verts.add(v);
		vmark(v, 1);
	    }
	}
}

void MxStdModel::collect_edge_neighbors(uint4 v1, uint4 v2,
					MxFaceList& faces)
{
    mark_neighborhood(v1, 1);
    mark_neighborhood(v2, 0);
    collect_unmarked_neighbors(v1, faces);
}

void MxStdModel::collect_vertex_star(uint4 v, MxVertexList& verts)
{
    const MxFaceList& N = neighbors(v);

    mark_corners(N, 0);
    vmark(v, 1); // Don't want to include v in the star
    collect_unmarked_corners(N, verts);
}

void MxStdModel::compute_vertex_normal(MxVertexID v, float4 *n)
{
    MxFaceList& star = neighbors(v);
    mxv_set(n, 0.0f, 3);

    uint4 i;
    for(i=0; i<star.length(); i++)
    {
	float4 fn[3];

	// Weight normals uniformly
	compute_face_normal(star(i), fn, false);
	//
	// Weight normals by angle around vertex
	// 		uint c = face(star[i]).find_vertex(v);
	// 		compute_face_normal(star[i], fn);
	// 		mxv_scale(fn, compute_corner_angle(star[i], c), 3);

	mxv_addinto(n, fn, 3);
    }
    if( i>0 )
	mxv_unitize(n, 3);
}


void MxStdModel::synthesize_normals()
{
    float4 n[3];

    if( normal_binding() == MX_PERFACE )
    {
	for(MxFaceID f=0; f<face_count(); f++)
	{
	    compute_face_normal(f, n);
	    add_normal(n[X], n[Y], n[Z]);
	}
    }
    else if( normal_binding() == MX_PERVERTEX )
    {
	for(MxVertexID v=0; v<vert_count(); v++)
	{
	    compute_vertex_normal(v, n);
	    add_normal(n[X], n[Y], n[Z]);
	}
    }
    else
	MxMsg::signal(MXMSG_WARN, "Unsupported normal binding.",
		     "MxStdModel::synthesize_normals");
}



void MxStdModel::remap_vertex(uint4 from, uint4 to)
{
    AssertBound( from < vert_count() ); 
    AssertBound( to < vert_count() ); 
    SanityCheck( vertex_is_valid(from) );
    SanityCheck( vertex_is_valid(to) );
    
    for(uint4 i=0; i<neighbors(from).length(); i++)
	face(neighbors(from)(i)).remap_vertex(from, to);

    mark_neighborhood(from, 0);
    mark_neighborhood(to, 1);
    collect_unmarked_neighbors(from, neighbors(to));

    vertex_mark_invalid(from);
    neighbors(from).reset();   // remove links in old vertex
}

uint4 MxStdModel::split_edge(uint4 a, uint4 b)
{
    float4 *v1 = vertex(a), *v2 = vertex(b);

    return split_edge(a, b,
                      (v1[X] + v2[X])/2.0f,
                      (v1[Y] + v2[Y])/2.0f,
                      (v1[Z] + v2[Z])/2.0f);
}

static
void remove_neighbor(MxFaceList& faces, uint4 f)
{
    uint4 j;
    if( faces.find(f, &j) )
	faces.remove(j);
}

uint4 MxStdModel::split_edge(uint4 v1, uint4 v2,
			    float4 x, float4 y, float4 z)
{
    AssertBound( v1 < vert_count() );   AssertBound( v2 < vert_count() );
    SanityCheck( vertex_is_valid(v1) ); SanityCheck( vertex_is_valid(v2) );
    SanityCheck( v1 != v2 );

    MxFaceList faces;
    collect_edge_neighbors(v1, v2, faces);
    SanityCheck( faces.length() > 0 );

    uint4 vnew = add_vertex(x,y,z);

    for(uint4 i=0; i<faces.length(); i++)
    {
	uint4 f = faces(i);
	uint4 v3 = face(f).opposite_vertex(v1, v2);
	SanityCheck( v3!=v1 && v3!=v2 );
	SanityCheck( vertex_is_valid(v3) );

	// in f, remap v2-->vnew
	face(f).remap_vertex(v2, vnew);
	neighbors(vnew).add(f);

	// remove f from neighbors(v2)
	remove_neighbor(neighbors(v2), f);

	// assure orientation is consistent
	if( face(f).is_inorder(vnew, v3) )
	    add_face(vnew, v2, v3);
	else
	    add_face(vnew, v3, v2);
    }

    return vnew;
}

void MxStdModel::flip_edge(uint4 v1, uint4 v2)
{
    MxFaceList faces;  collect_edge_neighbors(v1, v2, faces);
    if( faces.length() != 2 ) return;

    uint4 f1 = faces(0);
    uint4 f2 = faces(1);
    uint4 v3 = face(f1).opposite_vertex(v1, v2);
    uint4 v4 = face(f2).opposite_vertex(v1, v2);

    // ?? Should we check for convexity or assume thats been taken care of?

    remove_neighbor(neighbors(v1), f2);
    remove_neighbor(neighbors(v2), f1);
    neighbors(v3).add(f2);
    neighbors(v4).add(f1);

    face(f1).remap_vertex(v2, v4);
    face(f2).remap_vertex(v1, v3);
}

void MxStdModel::split_face4(uint4 f, uint4 *newverts)
{
    uint4 v0 = face(f).v[0];
    uint4 v1 = face(f).v[1];
    uint4 v2 = face(f).v[2];

    uint4 pivot = split_edge(v0, v1);
    uint4 new1 = split_edge(v1, v2);
    uint4 new2 = split_edge(v0, v2);

    if( newverts )
    {
	newverts[0] = pivot;
	newverts[1] = new1;
	newverts[2] = new2;
    }

    flip_edge(pivot, v2);
}

void MxStdModel::compact_vertices()
{
    MxVertexID oldID;
    MxVertexID newID = 0;

    for(oldID=0; oldID<vert_count(); oldID++)
    {
	if( vertex_is_valid(oldID) )
	{
	    if( newID != oldID )
	    {
		vertex(newID) = vertex(oldID);
		if( normal_binding() == MX_PERVERTEX )
		    normal(newID)=normal(oldID);
		if( color_binding() == MX_PERVERTEX )
		    color(newID)=color(oldID);
		if( texcoord_binding() == MX_PERVERTEX )
		    texcoord(newID) = texcoord(oldID);

		// Because we'll be freeing the link lists for the
		// old vertices, we actually have to swap values instead
		// of the simple copying in the block above.
		//
		MxFaceList *t = face_links(newID);
		face_links(newID) = face_links(oldID);
		face_links(oldID) = t;

		vertex_mark_valid(newID);

		for(uint4 i=0; i<neighbors(newID).length(); i++)
		    face(neighbors(newID)(i)).remap_vertex(oldID, newID);
	    }
	    newID++;
	}
    }

    for(oldID = newID; newID < vert_count(); )
	remove_vertex(oldID);
}

void MxStdModel::unlink_face(MxFaceID fid)
{
    MxFace& f = face(fid);
    face_mark_invalid(fid);

    uint4 j; int4 found=0;

    if( neighbors(f(0)).find(fid, &j) )
    {found++; neighbors(f(0)).remove(j);}
    if( neighbors(f(1)).find(fid, &j) )
    { found++; neighbors(f(1)).remove(j); }
    if( neighbors(f(2)).find(fid, &j) )
    { found++; neighbors(f(2)).remove(j); }
    SanityCheck( found > 0 );
    SanityCheck( !neighbors(f(0)).find(fid, &j) );
    SanityCheck( !neighbors(f(1)).find(fid, &j) );
    SanityCheck( !neighbors(f(2)).find(fid, &j) );
}

void MxStdModel::remove_degeneracy(MxFaceList& faces)
{
    for(uint4 i=0; i<faces.length(); i++)
    {
	SanityCheck( face_is_valid(faces(i)) );
	MxFace& f = face(faces(i));

	if( f(0)==f(1) || f(1)==f(2) || f(0)==f(2) )
	    unlink_face(faces(i));
    }
}

void MxStdModel::compute_contraction(MxVertexID v1, MxVertexID v2,
				     MxPairContraction *conx,
				     const float4 *vnew)
{
    conx->v1 = v1;
    conx->v2 = v2;

    if( vnew )
    {
	mxv_sub(conx->dv1, vnew, vertex(v1), 3);
	mxv_sub(conx->dv2, vnew, vertex(v2), 3);
    }
    else
    {
	conx->dv1[X] = conx->dv1[Y] = conx->dv1[Z] = 0.0;
	conx->dv2[X] = conx->dv2[Y] = conx->dv2[Z] = 0.0;
    }

    conx->delta_faces.reset();
    conx->dead_faces.reset();


    // Mark the neighborhood of (v1,v2) such that each face is
    // tagged with the number of times the vertices v1,v2 occur
    // in it.  Possible values are 1 or 2.
    //
    mark_neighborhood(v2, 0);
    mark_neighborhood(v1, 1);
    mark_neighborhood_delta(v2, 1);


    // Now partition the neighborhood of (v1,v2) into those faces
    // which degenerate during contraction and those which are merely
    // reshaped.
    //
    partition_marked_neighbors(v1, 2, conx->delta_faces, conx->dead_faces);
    conx->delta_pivot = conx->delta_faces.length();
    partition_marked_neighbors(v2, 2, conx->delta_faces, conx->dead_faces);
}

void MxStdModel::apply_contraction(const MxPairContraction& conx)
{
    MxVertexID v1=conx.v1, v2=conx.v2;

    // Move v1 to new position
    mxv_addinto(vertex(v1), conx.dv1, 3);

    uint4 i;
    //
    // Remove dead faces
    for(i=0; i<conx.dead_faces.length(); i++)
	unlink_face(conx.dead_faces(i));

    //
    // Update changed faces
    for(i=conx.delta_pivot; i<conx.delta_faces.length(); i++)
    {
	MxFaceID fid = conx.delta_faces(i);
	face(fid).remap_vertex(v2, v1);
	neighbors(v1).add(fid);
    }

    //
    // !!HACK: This is really only a temporary solution to the problem
    if( normal_binding() == MX_PERFACE )
    {
	float4 n[3];
	for(i=0; i<conx.delta_faces.length(); i++)
	{
	    compute_face_normal(conx.delta_faces[i], n);
	    normal(conx.delta_faces[i]) = MxNormal(n);
	}
    }

    //
    // Kill v2
    vertex_mark_invalid(v2);
    neighbors(v2).reset();
}

void MxStdModel::apply_expansion(const MxPairExpansion& conx)
{
    MxVertexID v1=conx.v1, v2=conx.v2;

    mxv_sub(vertex(v2), vertex(v1), conx.dv2, 3);
    mxv_subfrom(vertex(v1), conx.dv1, 3);

    uint4 i,j;
    for(i=0; i<conx.dead_faces.length(); i++)
    {
	MxFaceID fid = conx.dead_faces(i);
	face_mark_valid(fid);
	neighbors(face(fid)(0)).add(fid);
	neighbors(face(fid)(1)).add(fid);
	neighbors(face(fid)(2)).add(fid);
    }

    for(i=conx.delta_pivot; i<conx.delta_faces.length(); i++)
    {
	MxFaceID fid = conx.delta_faces(i);
	face(fid).remap_vertex(v1, v2);
	neighbors(v2).add(fid);
	bool found = neighbors(v1).find(fid, &j);
	SanityCheck( found );
	neighbors(v1).remove(j);
    }

    //
    // !!HACK: This is really only a temporary solution to the problem
    if( normal_binding() == MX_PERFACE )
    {
	float4 n[3];
	for(i=0; i<conx.delta_faces.length(); i++)
	{
	    compute_face_normal(conx.delta_faces[i], n);
	    normal(conx.delta_faces[i]) = MxNormal(n);
	}

	for(i=0; i<conx.dead_faces.length(); i++)
	{
	    compute_face_normal(conx.dead_faces[i], n);
	    normal(conx.dead_faces[i]) = MxNormal(n);
	}
    }

    vertex_mark_valid(v2);
}


void MxStdModel::contract(MxVertexID v1, MxVertexID v2,
			  const float4 *vnew, MxPairContraction *conx)
{
    compute_contraction(v1, v2, conx);
    mxv_sub(conx->dv1, vnew, vertex(v1), 3);
    mxv_sub(conx->dv2, vnew, vertex(v2), 3);
    apply_contraction(*conx);
}

void MxStdModel::compute_contraction(MxFaceID fid, MxFaceContraction *conx)
{
    const MxFace& f = face(fid);

    conx->f = fid;
    conx->dv1[X] = conx->dv1[Y] = conx->dv1[Z] = 0.0;
    conx->dv2[X] = conx->dv2[Y] = conx->dv2[Z] = 0.0;
    conx->dv3[X] = conx->dv3[Y] = conx->dv3[Z] = 0.0;

    conx->delta_faces.reset();
    conx->dead_faces.reset();


    PARANOID( mark_neighborhood(f[0], 0) );
    mark_neighborhood(f[1], 0);
    mark_neighborhood(f[2], 0);

    mark_neighborhood(f[0], 1);
    mark_neighborhood_delta(f[1], +1);
    mark_neighborhood_delta(f[2], +1);

    fmark(fid, 0);		// don't include f in dead_faces

    partition_marked_neighbors(f[0], 2, conx->delta_faces, conx->dead_faces);
    partition_marked_neighbors(f[1], 2, conx->delta_faces, conx->dead_faces);
    partition_marked_neighbors(f[2], 2, conx->delta_faces, conx->dead_faces);
}

void MxStdModel::contract(MxVertexID v1, MxVertexID v2, MxVertexID v3,
			  const float4 *vnew,
			  MxFaceList& changed)
{
    mark_neighborhood(v1, 0);
    mark_neighborhood(v2, 0);
    mark_neighborhood(v3, 0);
    changed.reset();
    collect_unmarked_neighbors(v1, changed);
    collect_unmarked_neighbors(v2, changed);
    collect_unmarked_neighbors(v3, changed);

    // Move v1 to vnew
    vertex(v1)(0) = vnew[X];
    vertex(v1)(1) = vnew[Y];
    vertex(v1)(2) = vnew[Z];

    // Replace occurrences of v2 & v3 with v1
    remap_vertex(v2, v1);
    remap_vertex(v3, v1);

    remove_degeneracy(changed);

    //
    // !!HACK: Only a temporary solution
    if( normal_binding() == MX_PERFACE )
    {
	float4 n[3];
	for(uint4 i=0; i<changed.length(); i++)
	    if( face_is_valid(changed[i]) )
	    {
		compute_face_normal(changed[i], n);
		normal(changed[i]) = MxNormal(n);
	    }
    }
}

void MxStdModel::contract(MxVertexID v1, const MxVertexList& rest,
			  const float4 *vnew, MxFaceList& changed)
{
    uint4 i;

    // Collect all effected faces
    mark_neighborhood(v1, 0);
    for(i=0; i<rest.length(); i++)
	mark_neighborhood(rest(i), 0);

    changed.reset();

    collect_unmarked_neighbors(v1, changed);
    for(i=0; i<rest.length(); i++)
	collect_unmarked_neighbors(rest(i), changed);

    // Move v1 to vnew
    vertex(v1)(0) = vnew[X];
    vertex(v1)(1) = vnew[Y];
    vertex(v1)(2) = vnew[Z];

    // Replace all occurrences of vi with v1
    for(i=0; i<rest.length(); i++)
	remap_vertex(rest(i), v1);

    remove_degeneracy(changed);
}

MxVertexID MxStdModel::resolve_proxies(MxVertexID v)
{
    while( vertex_is_proxy(v) )
	v = vertex(v).as.proxy.parent;
    return v;
}

float4 *MxStdModel::vertex_position(MxVertexID v)
{
    return vertex(resolve_proxies(v));
}

MxVertexID& MxStdModel::vertex_proxy_parent(MxVertexID v)
{
    SanityCheck( vertex_is_proxy(v) );
    return vertex(v).as.proxy.parent;
}

MxVertexID MxStdModel::add_proxy_vertex(MxVertexID parent)
{
    MxVertexID v = alloc_vertex(0, 0, 0); // position will be ignored

    vertex_mark_proxy(v);
    vertex_proxy_parent(v) = parent;

    return v;
}
