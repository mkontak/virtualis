// $Id: vxRuler2D.C,v 1.0 12/05/2013 Dongqing Exp $
//
// Copyright © 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Dongqing Chen (mailto:dongqing@viatronix.com)


// includes
#include "StdAfx.h"
#include "Point2D.h"
#include "vxViewerType.h"
#include "vxRuler2D.h"


// namespaces
using namespace std;

#define FILE_REVISION "$Revision: 1.0 $"



/**
 * constructor
 */
vxRuler2D::vxRuler2D()
{
}//vxRuler2D()


/** 
 *  Default constructor
 * Initializes the ruler object and its base
 *
 * @param pViewerType  The viewer type
 * @param size   The size of the view
 * @param bForUnitOnly  The flag to indicate ruler application purpose
 */
vxRuler2D::vxRuler2D(const vxViewerType * pViewerType, const std::pair<uint2, uint2> & size, const bool & bForUnitOnly)
{
	//initialize the start and end positions
	m_fViewWidth		= size.first;
	m_fViewHeight		= size.second;

	///positions of the ruler in screen coordinates
	Point2D<float4> startPos;
	Point2D<float4> endPos;

	startPos.m_x = 20.0F;
	startPos.m_y = size.second / 3.5F;

	endPos.m_x   = startPos.m_x;
  endPos.m_y   = size.second - startPos.m_y;

	//Convert the ruler positions. 
	pViewerType->ConvertScreenToWorldUsingOpenGL( startPos, m_startPoint );
	pViewerType->ConvertScreenToWorldUsingOpenGL( endPos, m_endPoint  );
	
	if (!bForUnitOnly)
	{ // calculate ruler marking position
	  startPos.m_x = startPos.m_x - 1.0F;
	  endPos.m_x = endPos.m_x - 1.0F;

	  /// This is for the dark line
	  pViewerType->ConvertScreenToWorldUsingOpenGL( startPos, m_startPointD );
	  pViewerType->ConvertScreenToWorldUsingOpenGL( endPos, m_endPointD  );

	  /// restore the light line position
	  startPos.m_x = startPos.m_x + 1.0F;
	  endPos.m_x = endPos.m_x + 1.0F;

	  Point3D<float4> fStartPoint, fEndPoint;
	  Point2D<float4> fpt(startPos.m_x + 5.0F, startPos.m_y);

	  float4 fRulerLength = endPos.m_y - startPos.m_y;
	  float4 fStep = fRulerLength / 8.0F;

	  /// calculate all ticking end points
	  for (int4 i = 0; i < 9; i++)
	  {
	    pViewerType->ConvertScreenToWorldUsingOpenGL( startPos, fStartPoint );
	    pViewerType->ConvertScreenToWorldUsingOpenGL( fpt, fEndPoint );

		  m_vTickingStartPoints.push_back(fStartPoint);
		  m_vTickingEndPoints.push_back(fEndPoint);

		  startPos.m_y = startPos.m_y + fStep;
		  fpt.m_y = fpt.m_y + fStep;

	  } // for i
	} // if bForUnitOnly

} // vxRuler2D( pViewerType, size )


/**
 * Destructor
 */
vxRuler2D::~vxRuler2D()
{
}//~vxRuler2D()


// undefines
#undef FILE_REVISION