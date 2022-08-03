// $Id: vxManipulatorSubcube.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorSubcube.h"
#include "vxCropBox.h"
#include "vxEnvironment.h"


// pragmas
#pragma unmanaged


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * default constructor
 * @param environment the environment
 */
vxManipulatorSubcube::vxManipulatorSubcube(vxEnvironment & environment):
vxManipulator(environment)
{
  GetEnvironment().GetCamera()->SetProjectionType( vxCamera::PERSPECTIVE );
} // Constructor()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorSubcube::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{
  bool bHandled( false );

  const float4 fMillimetersPerClick( 2.0F );

  // which key was pressed
  switch ( uKeyId )
  {
  // PageUp key
  case 33:
    GrowSubcube( fMillimetersPerClick * GetKeyMultiplier( uKeyboardState ) );
    bHandled = true;
    break;

  // PageDown key
  case 34:
    GrowSubcube( - fMillimetersPerClick * GetKeyMultiplier( uKeyboardState ) );
    bHandled = true;
    break;

  default:
    break;
  }

  return bHandled;
} // OnKeyPressed


/**
 * Grow the subcube by a number of voxels
 *
 * @param fMillimeters the number of millimeters to grow the subcube by (can be negative)
 */
void vxManipulatorSubcube::GrowSubcube( const float4 fMillimeters )
{
  // get the crop box
  vxShareableObject< vxCropBox > * pCropBox = GetEnvironment().GetCropBox();
  if ( pCropBox != NULL )
  {
    // get the current box
    Point3Df center( pCropBox->GetBox().GetMidPoint() );
    Vector3Df dim( Vector3Df( pCropBox->GetBox().GetDim() ) );
    dim += Vector3Df( fMillimeters, fMillimeters, fMillimeters );

    // ensure it doesn't shrink too much
    const float4 fMinSize( 5.0F );
    if ( dim.m_x < fMinSize ) dim.m_x = fMinSize;
    if ( dim.m_y < fMinSize ) dim.m_y = fMinSize;
    if ( dim.m_z < fMinSize ) dim.m_z = fMinSize;

    // figure the new box position
    Point3Df minPoint( center - dim / 2.0F );
    Point3Df maxPoint( center + dim / 2.0F );

    pCropBox->SetBox(Box<float4>(minPoint, maxPoint));
    pCropBox->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );       
  } // if crop box not NULL

} // GrowSubcube()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorSubcube.C,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/06/05 15:06:52  frank
// updated for visual studio 2005
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/15 18:45:17  frank
// Issue #4357: Fixed shortcut keys in 3D detail view
//
// Revision 1.3  2005/06/08 14:59:51  vxconfig
// added easy select object
//
// Revision 1.2  2005/06/07 18:07:58  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.2.2.1  2004/02/25 14:58:06  frank
// formatting
//
// Revision 1.2  2004/01/08 12:47:27  frank
// formatting
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/04/28 20:13:12  frank
// code review
//
// Revision 1.3  2003/04/10 12:00:44  frank
// disabled mouse wheel
//
// Revision 1.2  2003/03/13 14:06:49  frank
// Removed unused #include
//
// Revision 1.1  2003/02/14 23:31:18  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorSubcube.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxManipulatorSubcube.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
