// $Id: AnonymizerDicom.h,v 1.7 2007/03/12 14:43:19 mkontak Exp $ 
//
// Copyright � 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragmas
#pragma once

// include
#include "anonymizer.h"

// namespaces
OPEN_ANMZN_NS 

/**
 ^ Anonimizes DICOM Files.
 */
public ref class AnonymizerDicom :  public Anonymizer
{

public:

  ///Constructor
  AnonymizerDicom(void);

  ///One arg constructor
  AnonymizerDicom( AnonymizationInformation ^ anonymizationInfo );

  ///destructor
  ~AnonymizerDicom(void);

  ///Anonymize method
  virtual void Anonymize(  List<String ^> ^ sSourceFiles,  List<String ^> ^ sDestFiles ) override;

}; //class AnonymizerDicom

CLOSE_ANMZN_NS

// $Log: AnonymizerDicom.h,v $
// Revision 1.7  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.6  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerDicom.h,v 1.7 2007/03/12 14:43:19 mkontak Exp $
// $Id: AnonymizerDicom.h,v 1.7 2007/03/12 14:43:19 mkontak Exp $