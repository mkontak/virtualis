// $Id: vxDatasetsInfo.h,v 1.3 2006/11/09 19:42:12 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#pragma once

class vxDataset;

// class declaration
class VX_VIEWER_DLL vxDatasetsInfo
{
// member functions
public:

  /// gets the number of loaded datasets
  static const int4 GetNumberOfDatasets() { return m_vDatasetList.size(); }

	//Adds dataset to the dataset list
	static void AddDataset( vxDataset * dataset ) { m_vDatasetList.push_back( dataset ); }

	///Gets the dataset from the list at a particulat index
	static vxDataset* GetDataset ( int index ) 
	{
		if ( m_vDatasetList.size() > 0 )
		{
			std::list< vxDataset * >::const_iterator iter = m_vDatasetList.begin();
			std::advance( iter, index );
			return * iter;
		}
		return NULL;
	}//GetDataset ( int index ) const

// member variables
private:

	///List of datasets
	static std::list< vxDataset * > m_vDatasetList;
  
}; // class vxDatasets


// $Log: vxDatasetsInfo.h,v $
// Revision 1.3  2006/11/09 19:42:12  romy
// added GetDataset method
//
// Revision 1.2  2006/11/08 22:56:12  romy
// added vxDataset class
//
// Revision 1.1  2006/08/31 14:12:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDatasetsInfo.h,v 1.3 2006/11/09 19:42:12 romy Exp $
// $Id: vxDatasetsInfo.h,v 1.3 2006/11/09 19:42:12 romy Exp $
