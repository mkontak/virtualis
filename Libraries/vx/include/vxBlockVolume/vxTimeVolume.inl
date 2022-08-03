// $Id: vxTimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxTimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ 
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
// $Log: vxTimeVolume.inl,v $
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
// Revision 1.14  2002/06/18 19:27:10  binli
// added some functions
//
// Revision 1.13  2002/06/18 17:41:17  binli
// added new class: ECGRender
//
// Revision 1.12  2002/06/11 13:39:11  binli
// copy constructor & assign operator
//
// Revision 1.11  2002/06/10 20:57:58  binli
// Reset & Initialization(overload 3)
//
// Revision 1.10  2002/06/07 20:50:55  binli
// removed test code
//
// Revision 1.9  2002/06/07 20:41:18  binli
// test/code cleaned.
//
// Revision 1.8  2002/06/06 21:32:34  uday
// Added GetPhaseVolumeVector().
//
// Revision 1.7  2002/06/06 16:45:59  binli
// function GetPhaseVolume/GetPhaseVolumeContainer
//
// Revision 1.6  2002/06/05 17:06:10  uday
// 1> Modified Read/Write using V3DFile.
// 2> Access methods for phase volumes.
//
// Revision 1.5  2002/06/05 15:18:24  binli
// Instance function
//
// Revision 1.4  2002/06/03 19:31:44  binli
// improved ResetSelections().
//
// Revision 1.3  2002/05/31 20:33:57  binli
// removed a constructor
//
// Revision 1.2  2002/05/30 21:38:19  binli
// creating vxTimeVolume class: Selection
//
// Revision 1.1  2002/05/29 21:46:00  binli
// Initial version of vxTimeVolume class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxTimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxTimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $