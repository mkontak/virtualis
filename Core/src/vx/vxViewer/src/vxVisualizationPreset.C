// $Id: vxVisualizationPreset.C,v 1.8 2007/06/06 18:22:26 romy Exp $
//
// Copyright © 2003 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "PixelRGBA.h"
#include "vxSerializer.h"

#include "vxVisualizationPreset.h"



// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor
 */
vxVisualizationPreset::vxVisualizationPreset()
: m_sName( "undefined" ),
m_sPresetImageFile( "" ),
m_bShowPresetImage( false ),
m_backgroundColor( 0, 0, 0 ),
m_foregroundColor( 1, 1, 1 ),
m_pColormap( new vxColormap() ),
m_pRenderingMode( new vxRenderingMode() )
{
} // vxVisualizationPreset()


/**
 * Copy constructor
 *
 * @param other the other preset
 */
vxVisualizationPreset::vxVisualizationPreset( const vxVisualizationPreset & other ) : 
m_sName( "undefined" ),
m_sPresetImageFile( "" ),
m_bShowPresetImage( false ),
m_backgroundColor( 0, 0, 0 ),
m_foregroundColor( 1, 1, 1 ),
m_pColormap( new vxColormap() ),
m_pRenderingMode( new vxRenderingMode() )
{
  if (this != &other)
  {
    *this = other;
  }
} // vxVisualizationPreset( other )


/**
 * constructor
 *
 * @param pColormap the colormap
 * @param pRenderingMode the rendering mode
 */
vxVisualizationPreset::vxVisualizationPreset( vxColormap * pColormap, vxRenderingMode * pRenderingMode )
: m_sName( "undefined" ),
m_sPresetImageFile( "" ),
m_bShowPresetImage( false ),
m_backgroundColor( 0, 0, 0 ),
m_foregroundColor( 1, 1, 1 )
{
  m_pColormap      = pColormap;
  m_pRenderingMode = pRenderingMode;
} // vxVisualizationPreset( colormap, rendering mode )


/**
 * Destructor
 */
vxVisualizationPreset::~vxVisualizationPreset()
{
  //if( m_pColormap )
  //  delete m_pColormap;
  //
  //if( m_pRenderingMode )
  //  delete m_pRenderingMode;
} // ~vxVisualizationPreset()


/**
 * Assignment operator
 *
 * @param other the other preset
 *
 * @return const reference to this
 */
const vxVisualizationPreset & vxVisualizationPreset::operator =( const vxVisualizationPreset & other )
{
  if ( this != & other )
  {
    m_sName             = other.m_sName;
    m_sPresetImageFile  = other.m_sPresetImageFile;
		m_bShowPresetImage  = other.m_bShowPresetImage;
    *m_pColormap        = *other.m_pColormap;
    *m_pRenderingMode   = *other.m_pRenderingMode;
    m_foregroundColor   = other.m_foregroundColor;
    m_backgroundColor   = other.m_backgroundColor;
  }

  return * this;
} // operator =()


// undefines
#undef FILE_REVISION


// $Log: vxVisualizationPreset.C,v $
// Revision 1.8  2007/06/06 18:22:26  romy
// added a new flag to Indicate whether we want to display the Preset Image or not
//
// Revision 1.7  2007/03/23 14:34:55  romy
// Property presetImageFile added
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/11/22 21:16:29  geconomos
// commented out destructor code (revisit after rsna 2005)
//
// Revision 1.4  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.6.3  2005/02/17 21:02:19  geconomos
// added reminded warning message
//
// Revision 1.3.6.2  2005/02/17 20:59:51  geconomos
// added the ownership of colormaps and rendering modes (will revisit)
//
// Revision 1.3.6.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.3  2004/05/17 19:08:13  frank
// moved visualization preset serialization to its own class
//
// Revision 1.2  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/05 13:34:20  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVisualizationPreset.C,v 1.8 2007/06/06 18:22:26 romy Exp $
// $Id: vxVisualizationPreset.C,v 1.8 2007/06/06 18:22:26 romy Exp $
