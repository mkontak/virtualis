#ifndef MXSTDSLIM_INCLUDED // -*- C++ -*-
#define MXSTDSLIM_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Core simplification interface.  The MxStdSlim class defines the
  interface which all simplification classes conform to.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdSlim.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxStdModel.h"
#include "MxHeap.h"

#include "MxVec3.h"
#include "MxVec4.h"

#define MX_PLACE_ENDPOINTS 0
#define MX_PLACE_ENDORMID  1
#define MX_PLACE_LINE      2
#define MX_PLACE_OPTIMAL   3

#define MX_WEIGHT_UNIFORM       0
#define MX_WEIGHT_AREA          1
#define MX_WEIGHT_ANGLE         2
#define MX_WEIGHT_AVERAGE       3
#define MX_WEIGHT_AREA_AVG      4
#define MX_WEIGHT_RAWNORMALS    5

class MxStdSlim 
{
protected:
	MxStdModel *m;
	MxHeap heap;

public:
	uint4 m_iValidVerts;
	uint4 m_iValidFaces;
	bool m_bIsInitialized;

	int4 m_iPlacementPolicy;
	int4 m_iWeightingPolicy;
	bool m_bWillJoinOnly;

	float8 m_fBoundaryWeight;
	float8 m_fCompactnessRatio;
	float8 m_fMeshingPenalty;
	float8 m_fLocalValidityThreshold;
	uint4 m_uVertexDegreeLimit;

public:
	MxStdSlim(MxStdModel *m0);

	virtual void initialize() = 0;
	virtual bool decimate(uint4) = 0;

	MxStdModel& model() { return *m; }

protected:
	Vec4 triangle_raw_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	Vec3 triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	Vec3 triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	Vec4 triangle_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	float8 triangle_compactness(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	float8 triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3);
};

// MXSTDSLIM_INCLUDED
#endif
