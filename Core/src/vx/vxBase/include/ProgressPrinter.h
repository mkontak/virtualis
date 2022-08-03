// $Id: ProgressPrinter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ana Paula Centeno (mailto:ana@viatronix.com)

/*
  Simple class to print the percentage progress
*/

#ifndef ProgressPrinter_h
#define ProgressPrinter_h


// class definition
class ProgressPrinter
{
// functions
public:

  /// show percentage progress
  inline static void ShowProgress(const bool & bShow, const float4 & fNowVal, const float4 & fOnePercentVal)
  {
    // assumes onePercentVal > 1
    static int4 iLastPercent(0); // static is not thread safe, but the only way for fast execution
    if ( !bShow )
      return;
    if ( iLastPercent == int4(fNowVal/fOnePercentVal) )
      return;

    iLastPercent = int4(fNowVal/fOnePercentVal);
    std::cout.width(4);
    std::cout<<iLastPercent<<"%\b\b\b\b\b"<<std::flush;
  } // ShowProgress()
}; // ProgressPrinter


#endif // ProgressPrinter_h


// Revision History:
// $Log: ProgressPrinter.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/07/11 19:49:14  michael
// final code reviews
//
// Revision 3.4  2003/06/04 18:33:23  michael
// tookout namespace declaration, should not be in header file!!!
//
// Revision 3.3  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.2  2002/03/08 22:36:00  michael
// include cleanup
//
// Revision 3.1  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.1   Sep 19 2001 14:30:00   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.1  2001/06/01 17:33:02  ana
// PrintProgress function replaced by ProgressPrinter class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ProgressPrinter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ProgressPrinter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
