// $Id: vxDatasetsInfo.C,v 1.3 2006/11/09 19:42:12 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxDatasetsInfo.h"
#include "vxDataset.h"

// static initialization
std::list< vxDataset * > vxDatasetsInfo::m_vDatasetList( NULL );



// $Log: vxDatasetsInfo.C,v $
// Revision 1.3  2006/11/09 19:42:12  romy
// added GetDataset method
//
// Revision 1.2  2006/11/08 22:56:12  romy
// added vxDataset class
//
// Revision 1.1  2006/08/31 14:12:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxDatasetsInfo.C,v 1.3 2006/11/09 19:42:12 romy Exp $
// $Id: vxDatasetsInfo.C,v 1.3 2006/11/09 19:42:12 romy Exp $
