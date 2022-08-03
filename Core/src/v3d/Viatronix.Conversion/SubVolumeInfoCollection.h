#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace Viatronix::Enterprise::Processing;

OPEN_CONVERSION_NS


ref class SubVolumeInfo;

  
public ref class SubVolumeInfoCollection : IPipelineResults, Dictionary<unsigned int, SubVolumeInfo ^>
{

public:

  /// Creates the Sub-VOlume INfo collection
  SubVolumeInfoCollection(String ^ sJobId, String ^ sTaskName);


public:

  /// Xml representation of the object
  virtual String ^ ToXml();

public:


private:

  // Job Id
  String ^ m_sJobId;

  // Task name
  String ^ m_sTaskName;

};



CLOSE_CONVERSION_NS



