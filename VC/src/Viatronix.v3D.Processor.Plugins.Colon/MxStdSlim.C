/************************************************************************

  Surface simplification using quadric error metrics

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdSlim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxStdSlim.h"

MxStdSlim::MxStdSlim(MxStdModel *m0)
    : heap(64)
{
	m = m0;

	// Externally visible variables
	m_iPlacementPolicy = MX_PLACE_OPTIMAL;
	m_iWeightingPolicy = MX_WEIGHT_AREA;
	m_fBoundaryWeight = 1000.0;
	m_fCompactnessRatio = 0.0;
	m_fMeshingPenalty = 1.0;
	m_fLocalValidityThreshold = 0.0;
	m_uVertexDegreeLimit = 24;
	m_bWillJoinOnly = false;

	m_iValidFaces = 0;
	m_iValidVerts = 0;
	m_bIsInitialized = false;

	uint4 i;
	for (i=0; i<m->face_count(); i++) 
		if(m->face_is_valid(i))  
			m_iValidFaces ++;
    
	for (i=0; i<m->vert_count(); i++) 
		if(m->vertex_is_valid(i))  
			m_iValidVerts ++;
}

Vec4 MxStdSlim::triangle_raw_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 n = triangle_raw_normal(v1, v2, v3);
	return Vec4(n, -(n*v1));
}

Vec3 MxStdSlim::triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 a = v2 - v1;
	Vec3 b = v3 - v1;
	return a^b;
}

Vec3 MxStdSlim::triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	Vec3 n = triangle_raw_normal(v1, v2, v3);
	unitize(n);

	return n;
}

Vec4 MxStdSlim::triangle_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    Vec3 n = triangle_normal(v1, v2, v3);

    return Vec4(n, -(n*v1));
}

const float8 FOUR_ROOT3 = 6.928203230275509;

float8 MxStdSlim::triangle_compactness(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	float8 L1 = norm2(v2 - v1);
	float8 L2 = norm2(v3 - v2);
	float8 L3 = norm2(v1 - v3);

	return FOUR_ROOT3 * triangle_area(v1, v2, v3) / (L1+L2+L3);
}

float8 MxStdSlim::triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	return 0.5 * norm(triangle_raw_normal(v1, v2, v3));
}