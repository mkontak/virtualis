// $Id: vxTimeVolumeIO.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxTimeVolumeIO.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ 
// 
// Copyright© 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li binli@viatronix.com
//
// Complete history at bottom of file.




// Revision History:
// $Log: vxTimeVolumeIO.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:51:41  michael
// code review
//
// Revision 1.2  2002/07/15 20:12:20  frank
// Replaced template parameter with int2.
//
// Revision 1.1  2002/07/10 18:34:07  frank
// Initial version.
//
// Revision 1.10  2002/06/10 20:57:58  binli
// Reset & Initialization(overload 3)
//
// Revision 1.9  2002/06/07 20:34:44  binli
// function WriteSelectedPhaseVolume
//
// Revision 1.8  2002/06/06 21:53:41  uday
// update.
//
// Revision 1.7  2002/06/06 20:05:14  binli
// read/write of metafile: test & corrected.
//
// Revision 1.6  2002/06/06 16:46:26  binli
// no comments
//
// Revision 1.5  2002/06/05 22:18:10  uday
// Added ReadPhaseVolumes().
//
// Revision 1.4  2002/06/05 20:45:34  uday
// Added the following functions
// 1> WritePhaseVolumes()
// 2> WriteSelectedBlockVolume()
//
// Revision 1.3  2002/06/05 17:06:10  uday
// 1> Modified Read/Write using V3DFile.
// 2> Access methods for phase volumes.
//
// Revision 1.2  2002/06/03 19:31:03  binli
// Serialization of PhaseVolume
//
// Revision 1.1  2002/05/30 21:38:35  binli
// creating TimeVolume class: Serialization
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxTimeVolumeIO.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxTimeVolumeIO.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $