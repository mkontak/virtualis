// $Id: vxRaytracer.inl,v 1.2 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille  (mailto:frank@viatronix.com)


/**
 * Check is the intenisty of the specified label is visible.
 */
bool vxRaytracer::IsIntensityVisible( const uint4 uLabelIndex )
{
  if( !m_arguments.bShowLabels )
    return true;
  return (*m_arguments.pvComponents)[uLabelIndex].IsIntensityVisible();
} // IsIntensityVisible( const uint4 uLabelIndex )


// $Log: vxRaytracer.inl,v $
// Revision 1.2  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.1  2006/12/11 19:28:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRaytracer.inl,v 1.2 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxRaytracer.inl,v 1.2 2007/03/02 19:17:42 geconomos Exp $
