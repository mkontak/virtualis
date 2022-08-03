// $Id: vxCameraPreset.C,v 1.2 2005/02/10 16:32:12 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxCameraPreset.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
vxCameraPreset::vxCameraPreset()
//*******************************************************************
{
} // default constructor


/**
 * Constructor
 * @param name
 * @param camera
 * @param box
 */
vxCameraPreset::vxCameraPreset(const std::string & sName, 
                               const vxCamera & camera, 
                               const Box<float4> & boundingBox)
//*******************************************************************
{
  m_sName       = sName;
  m_camera      = camera;
  m_boundingBox = boundingBox;
} // constructor


/**
 * Copy constructor
 * @param camera
 * @return const reference to this
 */
vxCameraPreset::vxCameraPreset(const vxCameraPreset & cameraPreset)
//*******************************************************************
{
  *this = cameraPreset;
}


/**
 * Assignment operator
 * @param camera
 * @return const reference to this
 */
const vxCameraPreset & vxCameraPreset::operator=(const vxCameraPreset & cameraPreset)
//*******************************************************************
{
  if (this != &cameraPreset)
  {
    m_sName       = cameraPreset.m_sName;
    m_camera      = cameraPreset.m_camera;
    m_boundingBox = cameraPreset.m_boundingBox;
  }
  return *this;
}


#undef FILE_REVISION 


// Revision History:
// $Log: vxCameraPreset.C,v $
// Revision 1.2  2005/02/10 16:32:12  michael
// Added initial version (after recovering some older version from Attic)
//
// Revision 1.1.2.1  2005/02/10 15:46:01  michael
// Added initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCameraPreset.C,v 1.2 2005/02/10 16:32:12 michael Exp $
// $Id: vxCameraPreset.C,v 1.2 2005/02/10 16:32:12 michael Exp $
