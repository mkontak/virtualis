// $Id: StudyLoad.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StudyLoad.h
// Description: Contains the enum and the callback for the incremental study load functionality
// Created: Oct 4, 2000
// Author: Kevin Kreeger (kevin@viatronix.net)


#if !defined(StudyLoad_h)
#define StudyLoad_h

#include "Typedef.h"

namespace ReaderLib
{

  namespace DatasetOrientations
  {
    enum TypeEnum { eNONE = 0, eDEFAULT = 0, eSUPINE = 1, ePRONE = 2, eBOTH = 3, ePRIMARY, eSECONDARY, eSWAP };
  }

  #if _MSC_VER > 1000
  #pragma once
  #endif // _MSC_VER > 1000

  namespace StudyLoad
  {
    enum StepsEnum
    {
      eSTART = 0, eLARGESIZE_QUERY, eHEADER, eSEGMENTS, eAXIAL, eSAGITTAL, eCORONAL, eTHRESHOLD, eVOLFIELDS,
        eVOLUME, eDFS, eDFE, eCSI, eSLD, eINS, eSUBMARINE, eBOOKMARKS, eLOAD_COMPLETE, eINITIALIZE,
        eINITIALIZE_COMPLETE
    };

    // Note: "newOrientation" can be set to DEFAULT, SUPINE, or PRONE.  If set to
    // DEFAULT, then the current orientation is unchanged, if set to SUPINE or PRONE,
    // then all subsequent steps are assumed to have the same orientation as given.
    typedef int4 (*LoadCallbackFn)(void * pUserData, const StudyLoad::StepsEnum ePrevStep,
      const StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
      const DatasetOrientations::TypeEnum eNewOrientation);

    class LoadCallback
    {
      public:
        LoadCallback(const StudyLoad::LoadCallbackFn fn, void * pUserData = NULL) : m_pFunction(fn),
          m_pUserData(pUserData), m_eCurrentStep(StudyLoad::eSTART), m_eOrientation(DatasetOrientations::eSUPINE)
        {
        }

        LoadCallback(const StudyLoad::LoadCallback & src) { *this = src; }

        LoadCallback &operator=(const StudyLoad::LoadCallback & src)
        {
          m_pFunction = src.m_pFunction;
          m_pUserData = src.m_pUserData;
          m_eCurrentStep = src.m_eCurrentStep;
          return *this;
        }

        /// Returns the previous studyload step
        StudyLoad::StepsEnum NextStep(const StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete, const DatasetOrientations::TypeEnum eNewOrientation)
        {
          if (eNewOrientation == DatasetOrientations::eSUPINE)
          {
            m_eOrientation = DatasetOrientations::eSUPINE;
          }
          else if (eNewOrientation == DatasetOrientations::ePRONE)
          {
            m_eOrientation = DatasetOrientations::ePRONE;
          }
          
          if (m_pFunction)
          {
            m_pFunction(m_pUserData, m_eCurrentStep, eNextStep, fPercentComplete, m_eOrientation);
          }

          StudyLoad::StepsEnum eRetVal = m_eCurrentStep;
          m_eCurrentStep = eNextStep;
          return eRetVal;
        }

      private:
        LoadCallbackFn m_pFunction;
        void *m_pUserData;
        StepsEnum m_eCurrentStep;
        DatasetOrientations::TypeEnum m_eOrientation;
    };

  } // namespace StudyLoad

} // namespace ReaderLib

#endif // !defined(StudyLoad_h)

// $Log: StudyLoad.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4.16.2  2005/04/13 18:23:37  jmeade
// Issue 4117: Identify which datasets are loading.
//
// Revision 3.4.16.1  2001/01/02 09:24:31  jmeade
// Create separate stage for loading volume fields (w/ 2d slice loading).
//
// Revision 3.4  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.3  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.1.2.2  2002/04/02 16:59:36  jmeade
// Issue 2026: Allow open of single dataset from Study Info.
//
// Revision 3.1.2.1  2002/02/27 18:59:01  kevin
// Added user message Box for large studies (Note: had to pass
// mesage back to main thread to get the box displayed from
// the worker load thread)
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/24 20:30:58  jmeade
// Coding standards
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:18   ingmar
// Initial revision.
// Revision 1.10  2001/04/07 02:57:43  jmeade
// code conventions
//
// Revision 1.9  2001/02/06 14:13:19  frank
// Revamped study load progress display.
//
// Revision 1.8  2001/01/26 02:45:59  jeff
// changed enum
//
// Revision 1.7  2001/01/26 01:58:08  jeff
// okay Kevin, you were right, we need to differentiate PaintComplete from LoadComplete
//
// Revision 1.6  2001/01/11 13:08:45  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.5  2000/12/27 23:04:02  jeff
// TRANSVERSE ==> AXIAL, CROSSECT ==> CROSS_SECTION
//
// Revision 1.4  2000/12/19 14:49:40  frank
// Added "loading..." message during initialization of painting
//
// Revision 1.3  2000/11/22 16:55:50  frank
// Updated progress meter with all data fields
//
// Revision 1.2  2000/11/16 15:21:35  jeff
// LoadCallback class to keep track of study load state information
//
// Revision 1.1  2000/10/11 15:43:59  kevin
// Initial Version
//
// $Id: StudyLoad.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/StudyLoad.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
