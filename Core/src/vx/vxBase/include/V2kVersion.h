// $Id: V2kVersion.h,v 1.2 2007/11/03 00:02:37 jmeade Exp $
// $ Header: $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: V2kVersion.h
// Description: Header file to keep the current V2K version
// Created: 15 Jan 2001
// Author: Ingmar Bitter

#ifndef V2kVersion_h
#define V2kVersion_h


/// This file should be updated whenever the V3D-Colon Processor version change
#define V2K_VERSION  "Version 4.2 11/20/2013"
#define V2K_REVISION "Version 4.2 11/20/2013"

#endif // V2kVersion_h

// Revision History:
// $Log: V2kVersion.h,v $
// Revision 1.2  2007/11/03 00:02:37  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.1.1.1.28.1  2007/10/24 14:58:53  dongqing
// update the version to 3.1
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:30:04   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.53  2001/08/28 23:48:51  ingmar
// new BlockVector class used in BlockVolumes
//
// Revision 1.52  2001/08/20 19:21:59  nchimera
// Set lower thread priority for overall better server performance.
//
// Revision 1.51  2001/08/08 21:30:19  nchimera
// Changed streams to Logger::
//
// Revision 1.50  2001/08/08 21:09:48  ingmar
// added TargetVersion
//
// Revision 1.49  2001/08/01 21:37:13  ingmar
// new exception handling
//
// Revision 1.48  2001/05/17 00:22:17  ingmar
// DeciderHullOutside now has its own files
//
// Revision 1.47  2001/05/13 19:54:09  ingmar
// added DeciderHullOutside
//
// Revision 1.46  2001/05/13 17:23:07  ingmar
// improved regionGrowIterator and added DeciderHullInside
//
// Revision 1.45  2001/04/28 01:30:54  ingmar
// added Neighbor6/18/26Walker, NeighborIterator, RegionGrowIterator
//
// Revision 1.44  2001/04/24 23:41:58  ingmar
// added #define V2K_REVISION
//
// Revision 1.43  2001/04/06 22:55:10  ingmar
// upped ins threshold to same as used in rendering
//
// Revision 1.42  2001/04/05 03:51:47  ingmar
// started to organize docu
//
// Revision 1.41  2001/03/31 21:28:01  ingmar
// fixes to triangle stage => larger buffers
//
// Revision 1.40  2001/03/24 20:45:07  ingmar
// changed I1, I2, I4 .. to int1 int2 int4 int8 uint1 ..
//
// Revision 1.39  2001/03/16 01:01:20  ingmar
// added I1 I2 I4 I8 U1 U2 U4 U8 F4 F8
//
// Revision 1.38  2001/03/15 19:02:15  ingmar
// added check for empty parameter file in CmdLineClass
//
// Revision 1.37  2001/03/14 05:06:28  ingmar
// lock test
//
// Revision 1.36  2001/03/14 05:05:03  ingmar
// lock test
//
// Revision 1.35  2001/03/14 02:37:54  ingmar
// New Anonymizer
//
// Revision 1.34  2001/03/10 14:18:13  ingmar
// -who now reveals who a patient is (decryption)
//
// Revision 1.33  2001/03/09 19:23:55  ingmar
// better use dicom format when encrypting the name for the volume header
//
// Revision 1.32  2001/03/06 18:53:28  ingmar
// in Dicom: remove m_studySerPosition == 0.0F
//
// Revision 1.31  2001/03/05 16:37:11  ingmar
// compute thread will now only be started for 250+ slices (1mm recon)
//
// Revision 1.30  2001/03/04 20:49:21  ingmar
// better anonymize and cmd line switches
//
// Revision 1.29  2001/03/02 17:08:20  ingmar
// insert missing ID in .vol header
//
// Revision 1.28  2001/03/01 23:32:51  ingmar
// triangle stage cleanup
//
// Revision 1.27  2001/02/28 19:28:42  ingmar
// -noname now enables/disables patient anonomization upon dicom receive
//
// Revision 1.26  2001/02/28 14:31:54  ingmar
// fixed lost of small database interface problems
//
// Revision 1.25  2001/02/27 15:40:24  ingmar
// added encrypted original name to the Volume header
//
// Revision 1.24  2001/02/26 01:33:31  ingmar
// Patients have PersonName as name representation in code and similarly in the database
//
// Revision 1.23  2001/02/20 22:19:39  ingmar
// rebuild database from files on disk
//
// Revision 1.22  2001/02/20 15:12:02  ingmar
// Siemens bed removed during ecv stage
//
// Revision 1.21  2001/02/19 03:52:48  ingmar
// batabase rebulder - work in progress
//
// Revision 1.20  2001/02/15 14:30:14  ingmar
// removed Seedpoints.[Ch] as they are now a simple std::vector
//
// Revision 1.19  2001/02/13 01:31:38  ingmar
// better DatabeaseReBuilder
//
// Revision 1.18  2001/02/12 16:12:54  ingmar
// back to Franks skeleton for custumer delivery
//
// Revision 1.17  2001/02/12 04:05:05  ingmar
// enabled iterator heap grow
//
// Revision 1.16  2001/02/10 03:57:16  ingmar
// better movement warning
// lung removal by volume/surface ratio
//
// Revision 1.15  2001/02/06 04:56:33  ingmar
// specialized linear volume read to use less temporary memory
//
// Revision 1.14  2001/02/06 00:31:20  ingmar
// better lung removal and classification on very well cleansed data
//
// Revision 1.13  2001/02/04 20:58:23  ingmar
// added debug trace print statements
//
// Revision 1.12  2001/02/03 01:16:29  ingmar
// added enums for Hazard warnings
//
// Revision 1.11  2001/01/31 21:30:28  ingmar
// centerline finding back to 26 connected
//
// Revision 1.10  2001/01/31 18:35:41  ingmar
// improved bone/fluid seperation
//
// Revision 1.9  2001/01/31 16:12:26  ingmar
// removed machine ID from filenames
//
// Revision 1.8  2001/01/30 23:21:11  ingmar
// INS volume in INS stage as blocked volume, iterator only code
//
// Revision 1.7  2001/01/30 23:19:42  ingmar
// time log file and lung removal also in INS
//
// Revision 1.6  2001/01/26 23:28:46  ingmar
// better merging of non uniform fluid components, INS still with pointer and itertor code
//
// Revision 1.5  2001/01/23 22:39:13  ingmar
// added averageIntensities to HrdMap of .cla volume
//
// Revision 1.4  2001/01/23 15:05:19  ingmar
// database finished and tested, crypt class with pointer to data + size
//
// Revision 1.3  2001/01/19 17:54:01  ingmar
// new Crypt class and pre-bone removal, stilll some lung segments
//
// Revision 1.2  2001/01/16 17:21:28  dave
// added V2k Build version to header
//
// Revision 1.1  2001/01/15 22:39:04  ingmar
// initial V2kVersion file
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/V2kVersion.h,v 1.2 2007/11/03 00:02:37 jmeade Exp $
// $Id: V2kVersion.h,v 1.2 2007/11/03 00:02:37 jmeade Exp $
