// $Id: vxRendererCropBox.C,v 1.9 2006/05/31 16:18:22 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxRendererCropBox.h"
#include "vxEnvironment.h"
#include "vxCropBox.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Constructor
 * @param environment the environment
 */ 
vxRendererCropBox::vxRendererCropBox(vxEnvironment & environment)
:vxRenderer(environment)
{
} // vxRendererCropBox()


/**
 * Draw the crop box
 */
void vxRendererCropBox::Render()
{
  try
  {
    vxShareableObject<vxCropBox> * pCropBox( GetEnvironment().GetCropBox() );
    pCropBox->SetEnvironment( & GetEnvironment() );

    if ( pCropBox != NULL )
    {

      // save the OpenGL state
      glPushAttrib( GL_ALL_ATTRIB_BITS );

      // draw the crop box outline
      const uint4 uAxisIndex( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
      const int4 iPlaneOrthoDistance( vxUtils::CalculateOffsetDistance( GetEnvironment().GetPlaneOrtho()->GetDistance() /
        GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] + 0.5F,
        GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim()[ uAxisIndex ],
        GetEnvironment().GetOffset(),
        GetEnvironment().GetMaximumOffset() ) );
      float4 fOrthoDistanceWorld( iPlaneOrthoDistance * GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] );
      const BoundingRect<float4> & rect = pCropBox->GetCropRect( uAxisIndex );
      Point3Df corners[ 4 ];
      corners[ 0 ] = Point3Df( rect.GetMinX(), rect.GetMinY(), 0 );
      corners[ 1 ] = Point3Df( rect.GetMaxX(), rect.GetMinY(), 0 );
      corners[ 2 ] = Point3Df( rect.GetMaxX(), rect.GetMaxY(), 0 );
      corners[ 3 ] = Point3Df( rect.GetMinX(), rect.GetMaxY(), 0 );
      glLineWidth( 1.0F );
      glColor3f( 0, 1, 0 );
      glBegin( GL_LINE_LOOP );
	  int i(0);
      for ( ; i<4; ++i )
      {
        switch ( uAxisIndex )
        {
        case 0: glVertex3f( fOrthoDistanceWorld, corners[ i ].m_x, corners[ i ].m_y ); break;
        case 1: glVertex3f( corners[ i ].m_y, fOrthoDistanceWorld, corners[ i ].m_x ); break;
        case 2: glVertex3f( corners[ i ].m_x, corners[ i ].m_y, fOrthoDistanceWorld ); break;
        }
      }
      glEnd();

      // draw the center handle
      vxHandle handle = pCropBox->GetHandle( 4 );
      handle.PreRender( GetEnvironment() );
      handle.SetSelected( i == pCropBox->GetActiveHandle() && GetEnvironment().GetActiveEnvironment() == & GetEnvironment() );
      handle.Render( GetEnvironment() );

      // restore the OpenGL state
      glPopAttrib();

    } // crop box exists?
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "vxRendererCropBox", "Render" );
  }
} // Render()


// undefines
#undef FILE_REVISION


// $Log: vxRendererCropBox.C,v $
// Revision 1.9  2006/05/31 16:18:22  geconomos
// porting to visual studio 2005
//
// Revision 1.8  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.7  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.6  2005/05/16 13:49:42  frank
// made the crop box easier to handle
//
// Revision 1.5  2001/01/03 19:30:22  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.4  2004/04/06 16:26:41  frank
// unified environment access naming
//
// Revision 1.3  2004/03/11 17:46:23  frank
// typo
//
// Revision 1.2  2004/03/11 17:42:54  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.14  2003/06/03 20:31:08  michael
// added check to see if volume is valid (each dimension > 0)
//
// Revision 1.13  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.12  2003/05/19 14:38:33  frank
// Removed unused return parameter
//
// Revision 1.11  2003/05/16 15:34:07  dongqing
// code review
//
// Revision 1.10  2003/05/16 14:09:28  michael
// code review
//
// Revision 1.9  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.8  2003/01/22 22:01:47  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.7  2002/11/02 03:09:31  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.6  2002/09/24 15:33:15  frank
// Clarified variable names.
//
// Revision 1.5  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.4  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.3  2002/08/08 15:08:05  uday
// Update DrawHandlePoints().
//
// Revision 1.2  2002/06/27 14:06:52  michael
// cosmetics
//
// Revision 1.1  2002/06/11 14:56:31  manju
// Renderer to display handle points of crop box.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererCropBox.C,v 1.9 2006/05/31 16:18:22 geconomos Exp $
// $Id: vxRendererCropBox.C,v 1.9 2006/05/31 16:18:22 geconomos Exp $
