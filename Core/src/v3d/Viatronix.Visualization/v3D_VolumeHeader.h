// $Id: v3D_VolumeHeader.h,v 1.2 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright � 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (meissner@viatronix.com)


#pragma once


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class VolumeHeader
{
// functions
public:
  
  /// constructor
  VolumeHeader( System::IntPtr data );
  
// properties
public:

  /// gets a vxVolumeHeader pointer
  System::IntPtr GetVolumeHeaderPtr() { return System::IntPtr( m_pVolumeHeader ); }

// data
private:

  /// volume header
  vxVolumeHeader * m_pVolumeHeader;

}; // VolumeHeader


CLOSE_VISUALIZATION_NS 


// Revision History:
// $Log: v3D_VolumeHeader.h,v $
// Revision 1.2  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/07/29 11:03:01  michael
// adjusted the segmentation preset to use a volume (volume header) to put
// together the ranges for the auto segments based on modality etc.
//
// Revision 1.1  2003/06/04 17:18:11  michael
// added initial version for wrapping vxVolumeHeader
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VolumeHeader.h,v 1.2 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_VolumeHeader.h,v 1.2 2006/10/04 18:29:06 gdavidson Exp $