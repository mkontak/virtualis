// $Id: vxViewerTypeMesh.C,v 1.1 2007/08/09 21:00:45 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


#include "stdafx.h"
#include "vxViewerTypeMesh.h"


/*
 * Constructor
 */
vxViewerTypeMesh::vxViewerTypeMesh(vxEnvironment & environment)
:vxViewerType3D(environment)
{
}//vxViewerTypeMesh(vxEnvironment & environment)

/*
 * destructor
 */
vxViewerTypeMesh::~vxViewerTypeMesh()
{
}//~vxViewerTypeMesh()

/**
 * Returns if the viewer supports dropping of the specified type
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerTypeMesh::CanDragDrop(const DragDropEnum eSourceType)
//*******************************************************************
{
  return ( eSourceType == vxViewerType::CAMERA   || 
           eSourceType == vxViewerType::DATASET  );

} // CanDragDrop()

// $Log: vxViewerTypeMesh.C,v $
// Revision 1.1  2007/08/09 21:00:45  romy
// initial version
//
// $Id: vxViewerTypeMesh.C,v 1.1 2007/08/09 21:00:45 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMesh.C,v 1.1 2007/08/09 21:00:45 romy Exp $
