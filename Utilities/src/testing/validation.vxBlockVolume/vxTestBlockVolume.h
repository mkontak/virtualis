// $Id: vxTestBlockVolume.h,v 1.0 2011/07/19  dongqing Exp $
//
// Copyright(c) 2000-2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen  dongqing@viatornix.com
//
// Complete History at bottom of file.


#ifndef vxTestBlockVolume_h
#define vxTestBlockVolume_h

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "vxBlockVolume.h"


//class Timer;

/**
* This class encapsulate the validation procedure for all vxBlockVolume components: header and iterators.
* The test procedure is based on an digital phantom designed for CTC dicom study.
*/
class vxTestBlockVolume
{
  // member functions
public:
  /// Constructor with commadline parameters                         
  vxTestBlockVolume( const std::string & sSourceFilePath, const std::string & sDestFilePath );
  
  /// destructor
  ~vxTestBlockVolume();

  /// run the Pipeline according to current commad line switches. 
  bool Run();                                                           

private:
	/// create raw phantom data
	bool CreateRawPhantom();

	/// validate the header information
	bool ValidateHeader( vxBlockVolume<uint2> & vol, const std::string & sReportFilename );

	/// validate the volume voxel value
	bool ValidateHUvalue( vxBlockVolume<uint2> & vol, const std::string & sBenchmarkFilename, const std::string & sReportFilename );

	/// validate volume iterator
	bool ValidateVolIterator( vxBlockVolume<uint2> & vol, const std::string & sReportFilename );


private:
	/// I/O path
	const std::string & m_sSourceFilePath;
	const std::string & m_sDestFilePath;

}; // end of Pipeline

#endif // vxTestBlockVolume_h


// $Log: vxTestBlockVolume.h,v $
// Revision 1.0 2011/06/21  dongqing
// make the residue percent threshold editable in the VCSetting.XML
//
// $Header: Viatronix.v3D.Processor.Plugins.Colon/vxTestBlockVolume.h,v 1.0 2011/06/21 dongqing Exp $
// $Id: vxTestBlockVolume.h,v 1.0 2011/06/21 dongqing Exp $
