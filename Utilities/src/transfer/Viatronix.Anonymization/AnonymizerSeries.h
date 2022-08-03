// pragmas
#pragma once

// includes
#include "anonymizer.h"

// namespace declarations.
using namespace System;
using namespace System::Collections::Generic;

OPEN_ANMZN_NS

// forward declarations.
ref class AnonymizationInformation;

/**
 * Performs anonymization of the series xml.
 */
public ref class AnonymizerSeries : public Anonymizer
{
// construction
public:

  /// Constructor.
  AnonymizerSeries();

  /// Constructor.
  AnonymizerSeries(AnonymizationInformation ^ mpInformation);

  /// Destructor.
  virtual ~AnonymizerSeries();

// methods
public:

  /// Performs the anonymization.
  virtual void Anonymize(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles) override;

}; // class AnonymizerSeries

CLOSE_ANMZN_NS