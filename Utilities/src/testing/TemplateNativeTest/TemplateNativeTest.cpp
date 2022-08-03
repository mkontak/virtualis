// BitmapTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <algorithm>
#include <regex>

#include "Wmi.h"
#include "crypto\AESCrypt.h"
#include "crypto\CryptKey.h"
#include "crypto\CryptStream.h"
#include "sql\SqlConnection.h"
#include "sql\SqlParameter.h"
#include "sql\SqlHandle.h"
#include "sql\SqlCommand.h"
#include "ImpersonateUser.h"
#include "FilePtr.h"
#include "vxFileServer.h"
#include "vxTransportLayer.h"
#include "FilePtrFactory.h"
#include "dcmimgle\dcmimage.h"
#include "DemoRegistry.h"
#include <time.h>


std::shared_ptr<dcm::DcmImage> dicomImage;
FILE * g_delimtedResults = __nullptr;
std::shared_ptr<dcm::DcmBuffer> pixelData;

int _tmain(int argc, _TCHAR* argv[])
{
  std::string sScrambled = sys::DemoRegistry64::ScrambleApplication("VC");

  std::string sAppplication = sys::DemoRegistry64::UnScrambleApplication(sScrambled); 


  std::string sKey("y/NBNEnJsXH0J7MSic2c6RehuvZg14cia3faisAxjDUo5ZmBW/TvL+R9k0qRlameXbKO8ITwS6ApuJ8UNbOi0Ulryg2sJq+W2iNsdyYejJ75itO8qQzo4r2bn9ht8Ple");

  sys::DemoRegistry64 reg(sys::Applications::VC);


  bool expired = reg.IsExpired();


  reg.SetActivationKey(sKey);

  reg.Activate();



  int i = 0;

}

