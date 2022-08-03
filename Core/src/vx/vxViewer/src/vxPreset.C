// $Id: vxPreset.C,v 1.4 2006/12/21 15:06:14 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxPreset.h"
#include "vxCamera.h"
#include "vxVisualizationPreset.h"


// namespaces
using namespace nsCOLOR;
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Default constructor
 */
vxPreset::vxPreset(): 
m_sName("undefined"),
m_sModality("undefined"),
m_sBodyPart("undefined"),
m_iUserId(0),
m_sDicomGroup("undefined"),
m_sDicomField("undefined"),
m_sDicomValue("undefined"),
m_iInitialCameraPreset(0),
m_iInitialVisualizationPreset2D(0),
m_iInitialVisualizationPreset3D(0),
m_iSelectedVisualizationPreset2D(-1),
m_iSelectedVisualizationPreset3D(-1)
{
} // vxPreset()


/**
 * Copy constructor
 *
 * @param other the other preset
 */
vxPreset::vxPreset( const vxPreset & other )
{
  *this = other;
} // vxPreset( other )


/**
 * Destructor
 */
vxPreset::~vxPreset()
{
  int i(0);

  // remove the 2D presets
  for ( i=0; i<m_vpVisualizationPresets2D.size(); ++i )
  {
    if ( m_vpVisualizationPresets2D[i] != NULL )
    {
      delete m_vpVisualizationPresets2D[i];
      m_vpVisualizationPresets2D[i] = NULL;
    }
  }

  // remove the 3D presets
  for ( i=0; i<m_vpVisualizationPresets3D.size(); ++i )
  {
    if ( m_vpVisualizationPresets3D[i] != NULL )
    {
      delete m_vpVisualizationPresets3D[i];
      m_vpVisualizationPresets3D[i] = NULL;
    }
  }
} // ~vxPreset()


/**
 * Assignment operator
 *
 * @param the other preset
 *
 * @return const reference to this
 */
const vxPreset & vxPreset::operator =( const vxPreset & other )
{
  if ( this != & other )
  {
    m_sName                         = other.m_sName;
    m_sModality                     = other.m_sModality;
    m_sBodyPart                     = other.m_sBodyPart;
    m_iUserId                       = other.m_iUserId;
    m_sDicomGroup                   = other.m_sDicomGroup;
    m_sDicomField                   = other.m_sDicomField;
    m_sDicomValue                   = other.m_sDicomValue;
    m_iInitialCameraPreset          = other.m_iInitialCameraPreset;
    m_iInitialVisualizationPreset2D = other.m_iInitialVisualizationPreset2D;
    m_iInitialVisualizationPreset3D = other.m_iInitialVisualizationPreset3D;
    m_vpCameraPresets               = other.m_vpCameraPresets;

    // make deep copies of the cameras and visualization presets
    int i(0);
    for ( i=0; i<other.m_vpVisualizationPresets2D.size(); ++i )
      m_vpVisualizationPresets2D.push_back( new vxVisualizationPreset( * other.m_vpVisualizationPresets2D[i] ) );
    for ( i=0; i<other.m_vpVisualizationPresets3D.size(); ++i )
      m_vpVisualizationPresets3D.push_back( new vxVisualizationPreset( * other.m_vpVisualizationPresets3D[i] ) );
  }

  return * this;
} // operator =()


#undef FILE_REVISION


// Revision History:
// $Log: vxPreset.C,v $
// Revision 1.4  2006/12/21 15:06:14  gdavidson
// Issue #5189 & 5106: Added SelectedVisualizationPreset2D and SelectedVisualizationPreset3D
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
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
// Revision 1.1  2003/08/11 11:30:22  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxPreset.C,v 1.4 2006/12/21 15:06:14 gdavidson Exp $
// $Id: vxPreset.C,v 1.4 2006/12/21 15:06:14 gdavidson Exp $
