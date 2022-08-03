// $Id: AnonymizerDummyFile.h,v 1.4 2007/03/12 14:43:19 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Romy Sreedharan] [ romy@viatronix.com]

// prgamas
#pragma once

// includes
#include "anonymizer.h"

// namespaces
OPEN_ANMZN_NS

/*
 ^ A dummy anonymizer which copies files from src to anonimized destpath just for the sake of it
 */
public ref class AnonymizerDummyFile :  public Anonymizer
{

public:

  ///Constructor
  AnonymizerDummyFile();

  ///One arg constructor
  AnonymizerDummyFile( AnonymizationInformation ^ anonymizationInfo );

  ///destructor
  virtual ~AnonymizerDummyFile();

  ///Anonymize method
  virtual void Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles ) override;
  
}; 
///class AnonymizerDummyFile
CLOSE_ANMZN_NS

// $Log: AnonymizerDummyFile.h,v $
// Revision 1.4  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.3  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.2  2006/04/06 18:50:30  romy
// header change
//
// Revision 1.1  2006/02/27 17:08:20  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerDummyFile.h,v 1.4 2007/03/12 14:43:19 mkontak Exp $
// $Id: AnonymizerDummyFile.h,v 1.4 2007/03/12 14:43:19 mkontak Exp $