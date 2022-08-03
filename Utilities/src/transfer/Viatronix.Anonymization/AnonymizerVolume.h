// $Id: AnonymizerVolume.h,v 1.8 2007/03/12 14:43:20 mkontak Exp $ 
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
 ^ Anonimizes the Volume Files
 */
public ref class AnonymizerVolume : public Anonymizer
{

public:

  ///Constructor
  AnonymizerVolume(void);

  ///Constructor 
  AnonymizerVolume( AnonymizationInformation ^ anonymizationInfo );

  ///Destructor
  ~AnonymizerVolume()
  { this->!AnonymizerVolume(); }

  /// Finalizer
  !AnonymizerVolume();

  ///Anonymize methid
  virtual void Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles ) override;

  ///Anonimizes the Block Volume
  void AnonimizeBlockVolume( System::String ^ sSourceFile, System::String ^ sDestFile );

  ///Anonimizes the Linear volume
  void AnonymizeLinearVolume( System::String ^ sSourceFile, System::String ^ sDestFile);

private:

  /// Sets the var map value
  void SetVarMap(System::IntPtr varMapPtr, System::String ^ sName, System::String ^ sDesc, System::String ^ sValue);

};// class AnonymizerVolume

CLOSE_ANMZN_NS


// $Log: AnonymizerVolume.h,v $
// Revision 1.8  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.7  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.6  2006/03/01 20:51:50  mkontak
// Modified to fix problem with setting sld volume headers.
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerVolume.h,v 1.8 2007/03/12 14:43:20 mkontak Exp $
// $Id: AnonymizerVolume.h,v 1.8 2007/03/12 14:43:20 mkontak Exp $