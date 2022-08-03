// $Id: AnonymizerVrx.h,v 1.9 2007/03/12 14:43:20 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragmas
#pragma once

// includes
#include "anonymizer.h"

OPEN_ANMZN_NS

/**
 * Anonimizes VRX Files
 */
public ref class AnonymizerVrx : public Anonymizer
{

public:

  ///constructor
  AnonymizerVrx(void);

  ///destructor
  ~AnonymizerVrx(void);

  ///Constructior
  AnonymizerVrx( AnonymizationInformation ^ anonymizationInfo );

  ///ANonimize method
  virtual void Anonymize(  List<String ^> ^ sSourceFiles,  List<String ^>  ^ sDestFiles ) override;

private:

   ///Anonimizes the VRX file
  void AnonymizeVrxFile ( System::String ^ sDestFile  );

}; // class AnonymizerVrx

CLOSE_ANMZN_NS

// $Log: AnonymizerVrx.h,v $
// Revision 1.9  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.8  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.7  2006/07/21 21:30:14  romy
// modifeid project
//
// Revision 1.6  2006/07/21 16:17:23  romy
// fixed conversion issue
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerVrx.h,v 1.9 2007/03/12 14:43:20 mkontak Exp $
// $Id: AnonymizerVrx.h,v 1.9 2007/03/12 14:43:20 mkontak Exp $
