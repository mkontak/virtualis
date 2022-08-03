// $Id: vxRuler2D.h,v 1.0 12/05/2013 Dongqing Exp $
//
// Copyright © 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Dongqing Chen (mailto:dongqing@viatronix.com)

// pragmas
#pragma once

#include "Typedef.h"
#include "Point3D.h"
#include "Normal3D.h"
#include "Vector3D.h"


class vxViewerType;


/**
 * Represents a ruler on different 2D views. It always display on the 
 * left edge of the 2D view.
 */
class VX_VIEWER_DLL vxRuler2D
{

// construction
public:

	///Default constructor
	vxRuler2D( const vxViewerType * pViewerType, const std::pair<uint2,uint2> & size, const bool & bForUnitOnly );

	///Destructor
	~vxRuler2D();

	///Do nothing constructor
	vxRuler2D();

	///implement only if you need
	vxRuler2D( const vxRuler2D & other );

// member functions
public:

	///Get the Start Points ( in World Coordinates  )
	const Point3D<float4> & GetStartPoint() { return m_startPoint; } const 
	const Point3D<float4> & GetStartPointD() { return m_startPointD; } const  // for darker line

	///Get the End Points ( in World Coordinates  )
	const Point3D<float4> & GetEndPoint() { return m_endPoint; } const 
	const Point3D<float4> & GetEndPointD() { return m_endPointD; } const  // for darker line

	/// Get the ticking points ( in world coordinate )
	const std::vector<Point3D<float4>> & GetTickingStartPoints() const { return m_vTickingStartPoints; }
	const std::vector<Point3D<float4>> & GetTickingEndPoints() const { return m_vTickingEndPoints; }

	///Get the length ( Euclidean )
	const float4 GetLength() const { return abs((m_startPoint - m_endPoint).GetLength())  ; }


// member variables
private:

	///Start Position in World Coordinates for both light and dark lines
	Point3D<float4> m_startPoint, m_startPointD;

	///End position in World Coordinates for both light and dark lines
	Point3D<float4> m_endPoint, m_endPointD;

	/// tickering points for the ruler
	std::vector<Point3D<float4>> m_vTickingStartPoints;
	std::vector<Point3D<float4>> m_vTickingEndPoints;

	///Viewport Width
	float4 m_fViewWidth;

	///Viewport Height
	float4 m_fViewHeight;

}; // vxRuler2D

