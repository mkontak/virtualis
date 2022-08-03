#pragma once

using namespace System;



// namespaces
OPEN_ANMZN_NS 

public ref class AnonymizerUtilities
{
public:

  static Viatronix::Enterprise::Entities::Study ^ CreateStudy(Viatronix::Dicom::ElementCollection ^ mpIImage);

  static Viatronix::Enterprise::Entities::Series ^ CreateSeries(Viatronix::Dicom::ElementCollection ^ mpIImage);
 
  static Viatronix::Dicom::ElementCollection ^ CreateElementList(String ^ sVolumeFile);

};


CLOSE_ANMZN_NS