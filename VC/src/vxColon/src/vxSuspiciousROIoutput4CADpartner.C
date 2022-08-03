// $Id: vxSuspiciousROIoutput4CADpartner.C,v 1.1.2.13 2008/06/24 13:03:18 dongqing Exp $
//
// Copyright© 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: vxSuspiciousROIoutput4CADpartner.C
// Owner(s): Dongqing Chen dongqing@viatronix.net


/** 
 *  This class is to output suspicious ROI infomation to a text file for 
 *  CAD partner creating truth. It is not for release product.
 */


#include "stdafx.h"
#include "vxSuspiciousROIoutput4CADpartner.h"
#include "vxCADFindings.h"
#include "vxCadROI.h"
#include <queue>
#include "Timer.h"

using namespace hdr;
using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.1.2.13 $"

/** 
 *  Constructor
 *
 *  @param sFileName the output file full path name
 */
vxSuspiciousROIoutput4CADpartner::vxSuspiciousROIoutput4CADpartner( std::string & sFileName )
: m_sFileName( sFileName )
{
} // default constructor 


/** 
 *  destructor
 */
vxSuspiciousROIoutput4CADpartner::~vxSuspiciousROIoutput4CADpartner()
{
} // destructor


/** Write out the file
 *
 *  @param dataset the assoicated dataset to get suspicious ROIs
 */
void vxSuspiciousROIoutput4CADpartner::Write( const Dataset & dataset )
{
  const int4 iNumFindings = dataset.m_suspiciousRegions.GetCount();

  if( iNumFindings < 1 )
  {
     FILE * fp = fopen(m_sFileName.c_str(), "wb");

     if( fp == NULL )
     {
        LogErr("Fail to write out suspicious ROI for CAD vendor", "vxSuspiciousROIoutput4CADpartner.C", "Write");
        return;
     } // if

     int4 i = 0;

     /// keep the dataset name in the file
     fprintf(fp, "%s\n", m_sFileName.c_str());
     fprintf(fp, "Number of findings = %d\n", i);

     fclose(fp);

     return;
  } //

  FILE * fp = fopen(m_sFileName.c_str(), "wb");

  if( fp == NULL )
  {
    LogErr("Fail to write out suspicious ROI for CAD vendor", "vxSuspiciousROIoutput4CADpartner.C", "Write");
    return;
  } // if

  /// keep the dataset name in the file
  fprintf(fp, "%s\n", m_sFileName.c_str());
  fprintf(fp, "Number of findings = %d\n", iNumFindings);

  /// set all necessary parameter from the dataset 
  m_vfUnits = dataset.m_vUnits;
  m_viDim   = dataset.m_vDim;
  m_vfWorldRCSdicom = dataset.m_fields.m_vfShiftOffsetForDICOMWorldCoordinate;
  m_sPatientOrientation = dataset.m_fields.m_sPatientOrientation;

  register int4 i, j, n;
  register Vector3D<int4> vi;

  for( i=0; i<iNumFindings; i++ )
  {
		fprintf(fp, ">> Finding number: %d\n", i+1);

    Vector3D<float4> vfLow = dataset.m_suspiciousRegions.GetFinding(i).GetCentroidMM().GetVector();
    vi = Vector3D<int4>( int4(vfLow.m_x/m_vfUnits.m_x+0.5F), int4(vfLow.m_y/m_vfUnits.m_y+0.5F), int4(vfLow.m_z/m_vfUnits.m_z+0.5F) ); 

    ConvertVol2WorldCoordinate( vi, vfLow );
		fprintf(fp, "User click point in dicom world RCS: (%.1f, %.1f, %.1f)\n", vfLow.m_x, vfLow.m_y, vfLow.m_z);
    
    vfLow = Vector3D<float4>( 99999.0F, 99999.0F, 99999.0F);
    Vector3D<float4> vfUp  = Vector3D<float4>(-99999.0F,-99999.0F,-99999.0F);

		vxCAD::vxCadROI roi = dataset.m_suspiciousRegions.GetFinding(i).GetVolumeROI();
    ComputeBoundingBox( roi, vfLow, vfUp );

		fprintf(fp, "Bounding box lower corner: (%.1f, %.1f, %.1f)\n", vfLow.m_x, vfLow.m_y, vfLow.m_z);
		fprintf(fp, "Bounding box upper corner: (%.1f, %.1f, %.1f)\n", vfUp.m_x, vfUp.m_y, vfUp.m_z);

    n = -1;
    n = dataset.m_suspiciousRegions.GetFinding(i).GetVolumeROI().GetCount();
		fprintf(fp, "Number of voxels = %d\n", n);
		if( n > 0 )
		{
			for( j=n-1; j>=0; j-- )
			{
        vi = Vector3D<int4>(roi.GetVoxel(j).GetX(), roi.GetVoxel(j).GetY(), roi.GetVoxel(j).GetZ());

        ConvertVol2WorldCoordinate( vi, vfLow );
        
        fprintf(fp, "(%.1f, %.1f, %.1f) ", vfLow.m_x, vfLow.m_y, vfLow.m_z);
			} // for j all voxels
      fprintf(fp, "\n");
		} // if n > 0
  } // for all findings

  fclose(fp);

} // Write


/** 
 *   Compute the bounding box
 *
 *  @param roi the current volume roi
 *  @param vfLow the lower conner
 *  @param vfUp the upper conner
 */
void vxSuspiciousROIoutput4CADpartner::ComputeBoundingBox( const vxCAD::vxCadROI & roi, 
               Vector3D<float4> & vfLow, Vector3D<float4> & vfUp )
{
  const int iN = roi.GetCount();
  register int4 i;
  register Vector3D<int4> vi;
  register Vector3D<float4> vf;

	for( i=iN-1; i>=0; i-- )
  {
    vi = Vector3D<int4>(roi.GetVoxel(i).GetX(), roi.GetVoxel(i).GetY(), roi.GetVoxel(i).GetZ());
    
    ConvertVol2WorldCoordinate( vi, vf );

    if( vf.m_x < vfLow.m_x )
		{
			vfLow.m_x = vf.m_x;
		} // if

    if( vf.m_y < vfLow.m_y )
		{
			vfLow.m_y = vf.m_y;
		} // if

    if( vf.m_z < vfLow.m_z )
		{
			vfLow.m_z = vf.m_z;
		} // if

    if( vf.m_x > vfUp.m_x )
		{
			vfUp.m_x = vf.m_x;
		} // if

    if( vf.m_y > vfUp.m_y )
		{
			vfUp.m_y = vf.m_y;
		} // if

    if( vf.m_z > vfUp.m_z )
		{
			vfUp.m_z = vf.m_z;
		} // if
	} // for i

} // GomputeBoundingBox


/** convert volume coordinate to DICOM RCS world coordinate
 *
 *  @param viVolCoordinate
 *  @param vfWorldCoordinate
 */
void vxSuspiciousROIoutput4CADpartner::ConvertVol2WorldCoordinate( const Vector3D<int4> & viVolCoordinate,
          Vector3D<float4> & vfWorldCoordinate )
{
  vfWorldCoordinate.m_z = m_vfWorldRCSdicom.m_z + viVolCoordinate.m_z * m_vfUnits.m_z;

  if( m_sPatientOrientation == "HFS" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x + float4(viVolCoordinate.m_x) * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y + float4(viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "FFS" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x - (float4(m_viDim.m_x)-viVolCoordinate.m_x) * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y + float4(viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "HFP" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x - (float4(m_viDim.m_x)-viVolCoordinate.m_x) * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y - (float4(m_viDim.m_y)-viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "FFP" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x + viVolCoordinate.m_x * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y - (float4(m_viDim.m_y)-viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "HFDR" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x - (float4(m_viDim.m_x)-viVolCoordinate.m_x) * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y + viVolCoordinate.m_y * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "FFDR" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x - (float4(m_viDim.m_x)-viVolCoordinate.m_x) * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y - (float4(m_viDim.m_y)-viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "HFDL" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x + viVolCoordinate.m_x * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y - (float4(m_viDim.m_y)-viVolCoordinate.m_y) * m_vfUnits.m_y;
  }
  else if( m_sPatientOrientation == "FFDL" )
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x + viVolCoordinate.m_x * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y + viVolCoordinate.m_y * m_vfUnits.m_y;
  }
  else
  {
    vfWorldCoordinate.m_x = m_vfWorldRCSdicom.m_x + viVolCoordinate.m_x * m_vfUnits.m_x;
    vfWorldCoordinate.m_y = m_vfWorldRCSdicom.m_y + viVolCoordinate.m_y * m_vfUnits.m_y;
  }

} // ConvertVol2WorldCoordinate


#undef FILE_REVISION


// Revision History:
// $Log: vxSuspiciousROIoutput4CADpartner.C,v $
// Revision 1.1.2.13  2008/06/24 13:03:18  dongqing
// if the finding list is empty, still write out the text file for record consistency.
//
// Revision 1.1.2.12  2008/04/28 20:39:00  dongqing
// fix the incorrect transfer matrix for RCS in DL/DR cases
//
// Revision 1.1.2.11  2008/04/21 22:41:01  dongqing
// fix the bug on the output coordinates
//
// Revision 1.1.2.10  2008/03/03 17:21:41  dongqing
// fix the orientation bug in the ConvertVol2WorldCoordinate
//
// Revision 1.1.2.9  2008/03/03 15:32:33  dongqing
// fix typo in the ConvertVol2DicomWorldCoordinate
//
// Revision 1.1.2.8  2008/02/29 20:57:41  dongqing
// add code for CAD partner to see the user click points dicom RCS
// world coordinate
//
// Revision 1.1.2.7  2008/02/29 15:52:54  dongqing
// Create a new function for converting Vol Coordinate to Dicom RCS world
// coordinate
//
// Revision 1.1.2.6  2008/02/28 19:19:09  dongqing
// the DICOM RCS convertion using Image Orientation Patient Cosine
// only now. The HFS stuff is no longer used for that purpose.
//
// Revision 1.1.2.5  2008/01/21 22:40:18  dongqing
// change the output English wording
//
// Revision 1.1.2.4  2008/01/21 15:47:23  dongqing
// output formating
//
// Revision 1.1.2.3  2008/01/21 14:39:02  dongqing
// fix the error on the finding index
//
// Revision 1.1.2.2  2008/01/21 13:46:50  dongqing
// complete 1st time
//
// Revision 1.1.2.1  2008/01/18 22:38:15  dongqing
// initial check in the suspicious ROI output for CAD vendor codes
//
// Revision 1.0  2008/01/17  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/vxSuspiciousROIoutput4CADpartner.C,v 1.1.2.13 2008/06/24 13:03:18 dongqing Exp $
// $Id: vxSuspiciousROIoutput4CADpartner.C,v 1.1.2.13 2008/06/24 13:03:18 dongqing Exp $