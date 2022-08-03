// pragmas
#pragma once

// includes
#include "anonymizer.h"

OPEN_ANMZN_NS

// forward declarations.
ref class AnonymizationInformation;

/**
 * Anonymizer for the study.
 */
public ref class AnonymizerStudy : public Anonymizer
{
// construction.
public:

  /// Constructor.
  AnonymizerStudy();

  /// Constructor.
  AnonymizerStudy(AnonymizationInformation ^ mpInformation);

  /// Destructor.
  virtual ~AnonymizerStudy();

// methods
public:

  /// Performs the anonymization.
  virtual void Anonymize(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles) override;

}; // class AnonymizerStudy

CLOSE_ANMZN_NS
