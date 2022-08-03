// pragmas
#pragma once

// includes
#include "Anonymizer.h"

// namespaces
OPEN_ANMZN_NS

// forward declarations
ref class AnonymizationInformation;

/**
 * Anonymizer for the patient.
 */
public ref class AnonymizerPatient : public Anonymizer
{
// construction  
public:

  /// Default Constructor.
  AnonymizerPatient();

  /// Constructor.
  AnonymizerPatient(AnonymizationInformation ^ mpInfo);

  /// Destructor.
  virtual ~AnonymizerPatient();

// methods
public:

  /// Anonimize the patient xml.
  virtual void Anonymize(List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles) override;

}; // class AnonymizerPatient

CLOSE_ANMZN_NS
