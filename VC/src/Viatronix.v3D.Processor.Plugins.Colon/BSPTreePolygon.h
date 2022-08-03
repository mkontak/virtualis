// $Id: BSPTreePolygon.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BSPTreePolygon.h  
// Description: interface for the BSPTreePolygon class. 
// Created: 07/01/2000 
// Author: Bin Li
//
// Complete History at bottom of file.


#ifndef BSP_TREE_POLYGON_h
#define BSP_TREE_POLYGON_h


class BSPTreePolygon
{
	// member functions
	//-----------------

	// Constructor/Destructor/Access functions.
	//-----------------------------------------

public:
	/// Constructor.
	BSPTreePolygon();

	/// Destructor.
	virtual ~BSPTreePolygon();
	
	/// set up the maximum number of polygons in the leaf of the BSP-Tree.
	inline void SetPolyNumPerFile(const int4 iNum);
	
	/// get to know the number of polygon files (segments).
	inline int4 GetPolygonFileNo() const;

	/// set up threshold.
	inline void SetThreshold(const int4 iThreshold);
	
	/// get the true number of polygons generated.
	inline int4 GetPolygonNumber() const;

	/// get the pointer to the polygon buffer.
	float4 * GetPtrToPolygonBuffer() const;

	// Primary/Auxiliary functions
	//----------------------------

	/// generation polygons in Marching Cubes in the branch of the BSP-Tree.
	void PolygonGen(uint1 *pDataPtr, Triple<uint4> & vSize,	Triple<float4> & vUnit, uint4 uSegId) ;

	/// a structure to store cell_table.
	class CellEntry 
	{
	public:
    uint1 nverts;		// number of vertices
    int4 verts[8];		// vertex number
    uint1 nedges;		// number of edges
    int4 edges[12];	// edge number
    uint1 npolys;		// number of polygons
    int4 polys[30];	// polygon number
	};

private:
	/// get the index to cell_table.
	int4	 PreMarchingCubes(int4 iSizeX, int4 iSizeY, int4 iSizeZ, uint4 uSegId);

	/// calculate indexes of a 'stick' of voxels and
	// get to know the number of polygons in each cell along the stick. 
	void CalcIndex(uint1 *pData, int4 iDimX, int4 iXdimYdim, int4 *pIndex, const uint4 uThresh);
	
	/// recursively bi-divides the data volume follow BSP-Tree construction method
	// when dividing the volume, the algorithm try to find a position that if the volume 
	// is divided at this position, the number of polygons in two sub-volumes are almost same.
	// The dividing stops when the number of polygons in the sub-volume.
	// reaches a pre-set value (valued by 'm_plgNoPerFile'). 
	void BSPTreeConstruct(int4 x0, int4 y0, int4 z0, int4 x1, int4 y1, int4 z1, int4 iPlgNo);
	
	/// generate polygons in Marching Cubes.
	void BSPPolygonGenerate(int4 x0, int4 y0, int4 z0, int4 x1, int4 y1, int4 z1);

	/// count the total number of polygons in this data set (based on the calculation in 'Cal_index()'.
	int4  ColonWall(uint4);

	/// calculate the number of polygons in the boundary area - [(y0,z0),(y1,z1)] in slice x0.
	int4  XSlicePlgNo(int4 x0, int4 y0, int4 z0, int4 y1, int4 z1);
	
	/// calculate the number of polygons in the boundary area - [(x0,z0),(x1,z1)] in slice y0.
	int4  YSlicePlgNo(int4 y0, int4 x0, int4 z0, int4 x1, int4 z1);

	/// calculate the number of polygons in the boundary area - [(x0,y0),(x1,y1)] in slice z0.
	int4  ZSlicePlgNo(int4 z0, int4 x0, int4 y0, int4 x1, int4 y1);

	/// the following 3 functions calculate the accumulated gray value around a 3x3 area. 
	// the center of this area is pointed by 'Ptr'. 
	// The contributions of points in the area are weighted according to their position,
	float4 ValueX(uint1 *puPtr);	// a 3x3 area which is perpendicular to X-axle.
	float4 ValueY(uint1 *puPtr);	// a 3x3 area which is perpendicular to Y-axle.
	float4 ValueZ(uint1 *puPtr);	// a 3x3 area which is perpendicular to Z-axle.

	// Data members
	//-------------
private:
	/// size of the data set.
	int4 m_iDimX, m_iDimY, m_iDimZ;

	// units of the data volume.
	Triple<float4> m_vUnits;

	/// number of polygons in each polygon file (each segment has its own polygon file). 
	int4 m_iPlgNoPerFile;

	/// number of polygon files.
	int4	m_iPlgFileNo;
	
	///  threshold.
	int4	m_iThreshold;
	
	/// current segment ID.
	uint4 m_uSegId;

	/// pointer to input data set.
	uint1 *m_pIndata;
	
	/// a buffer storing the pre-calculated indexes in each voxel. 
	// using these stored values to index into cell_table for polygon information.
	uint1 *m_pPlgIndex;

	/// number of polygons that generated.
	int4 m_iPlgNoGenerated;

	/// pointer to the polygons buffer that generated.
	float4 * m_pPlgBuffer;
}; // end Class BSPTreePolygon

#include "BSPTreePolygon.inl"

#endif // BSP_TREE_POLYGON_h


// $Log: BSPTreePolygon.h,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:38:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:28   ingmar
// Initial revision.
// Revision 1.17  2001/07/25 16:16:42  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.16  2001/05/30 15:43:31  binli
// code conventions
//
// Revision 1.15  2001/04/12 14:38:47  dmitry
// Sec_j_tri base class added
//
// Revision 1.14  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.13  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.12  2001/03/27 15:25:30  binli
// 'const' functions for safety.
//
// Revision 1.11  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.10  2001/03/21 19:01:48  binli
// moved 'cell_table' to class BSPTreePolygon
//
// Revision 1.9  2001/01/02 17:59:51  binli
// fixed bug of 'zero' triangle in a segment
//
// Revision 1.8  2000/12/04 18:42:11  binli
// write out triangles in World coordinates.
//
// Revision 1.7  2000/10/31 21:09:50  binli
// changed the order of the pipeline (for the purpose of proper skeleton rendering)
//
// Revision 1.6  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.4.10.1  2000/10/19 20:37:01  binli
// code optimization
//
// Revision 1.4  2000/09/20 15:02:03  binli
// Viatronix C++ coding walk through -- 2nd
//
// Revision 1.3  2000/09/20 14:42:44  binli
// Viatronix C++ coding walk through (moved some inline functions to .inl file)
//
// Revision 1.2  2000/09/14 18:15:13  binli
// fitted to Viatronix C++ code convention and standard
//
// Revision 1.1.1.1  2000/08/11 22:15:53  ingmar
// unit tests complete
//
// 
// 10    9/01/00 12:50p Binli
// 
// 9     9/01/00 12:49p Binli
// change the sub-pipeline of polygon generation (2) : gap the 'polygon
// generation'
// sub-module and 'data format transfer' sub-module, so no temporary
// file(s)
// '.plg' is(are) generated.
// 
// 8     8/31/00 4:42p Binli
// change the sub-pipeline of polygon generation (1) : gap the 'shrink'
// sub-module and 'polygon generation' sub-module, so no temporary file(s)
// '.shr' is(are) generated.
// 
// 7     8/29/00 12:57p Antonio
// updated footer
// 
// *****************  Version 6  *****************
// User: Binli           Date:  8/22/00  Time:  1:53p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 5  *****************
// User: Antonio         Date:  8/21/00  Time:  6:26p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
// 
// *****************  Version 4  *****************
// User: Sarang          Date:  8/20/00  Time:  6:49p
// Checked in $/v2k/src/Triangles
// Comment:
//   The whole pipeline now works under linux+gcc. Removed tons of bugs
// and syntax errors found by gcc.
// 
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
// 
// *****************  Version 3  *****************
// User: Binli           Date:  8/09/00  Time:  2:12p
// Checked in $/v2k/src/Triangles
// 
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  4:34p
// Created BSPTreePolygon.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/BSPTreePolygon.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: BSPTreePolygon.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
