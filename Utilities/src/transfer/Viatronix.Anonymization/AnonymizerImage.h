// $Id: AnonymizerImage.h,v 1.8.2.1 2007/03/27 00:49:27 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragmas
#pragma once

// includes
#include "anonymizer.h"

// namespaces
OPEN_ANMZN_NS

/**
 ^ Anonimizes the Images
 */
public ref class AnonymizerImage : public Anonymizer
{

public:

  ///Constructor
  AnonymizerImage();

  //destructor
  ~AnonymizerImage()
  { this->!AnonymizerImage(); }

  !AnonymizerImage();

  ///Anonimizes Images 
  AnonymizerImage( AnonymizationInformation ^ anonymizationInfo );

  ///Anonymize methos
  virtual void Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles ) override;

private:
   ///Delegate to display the Field Selector Interface.
   delegate void ShowRedactorInterfaceDelegate();

   ///Method which displays the fieldselector dialog.
   void ShowRedactorInterface( );

    List<String ^>  ^ m_sSourceFiles;
   
    List<String ^>  ^ m_sDestFiles;

    /// An auto reset event to wait until the redactor dialog has closed to continue.
    System::Threading::AutoResetEvent ^ m_mpResetEvent;

}; //class AnonymizerImage

CLOSE_ANMZN_NS

// $Log: AnonymizerImage.h,v $
// Revision 1.8.2.1  2007/03/27 00:49:27  romy
// displaying of redactor modified.
//
// Revision 1.8  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.7  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.6  2005/10/20 15:16:12  romy
// added progress events and Exceptions
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerImage.h,v 1.8.2.1 2007/03/27 00:49:27 romy Exp $
// $Id: AnonymizerImage.h,v 1.8.2.1 2007/03/27 00:49:27 romy Exp $