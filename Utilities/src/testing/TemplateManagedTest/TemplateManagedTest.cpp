// ManagedBitmapTest.cpp : main project file.

#include "stdafx.h"


using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace Viatronix::Dicom;
using namespace System::Collections::Generic;




int main(array<System::String ^> ^args)
{

  //String ^ auditing = Viatronix::Encryption::Crypto::Encrypt("user id=v3dsa;password=jkl789@;database=Auditing;server=");
  //  String ^ system = Viatronix::Encryption::Crypto::Encrypt("user id=v3dsa;password=jkl789@;database=System;server=");
  //    String ^ storage = Viatronix::Encryption::Crypto::Encrypt("user id=v3dsa;password=jkl789@;database=Storage;server=");
  //      String ^ licensing = Viatronix::Encryption::Crypto::Encrypt("user id=v3dsa;password=jkl789@;database=Licensing;server=");
  //              String ^ security = Viatronix::Encryption::Crypto::Encrypt("user id=v3dsa;password=jkl789@;database=Security;server=");


 Viatronix::Dicom::Image ^ image = gcnew Viatronix::Dicom::Image();

 image->PatientName = "Test^Me2";
 image->StudyInstanceUid =  Viatronix::Dicom::Utility::GenerateUid();
 image->SeriesInstanceUid = Viatronix::Dicom::Utility::GenerateUid();

 System::Drawing::Bitmap ^ bm = (Bitmap ^)Bitmap::FromFile("D:\\Testing\\Image\\Test.bmp");

 image->SetPixelData(bm);

 image->Save("D:\\Testing\\Image\\Test2.dcm");


  return 0;
}
