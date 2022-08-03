#pragma once


#include <iostream>     // std::cout
#include <algorithm>    // std::transform
#include <vector>       // std::vector
#include <functional>   // std::plus
#include <string>
#include <map>
#include <exception>
#include <memory>

#include "dcmnet/dimse.h"
#include "dcmnet/diutil.h"
#include "dcmdata/dcfilefo.h"
#include "dcmdata/dcdebug.h"
#include "dcmdata/dcuid.h"
#include "dcmdata/dcdict.h"
#include "dcmdata/cmdlnarg.h"
#include "dcmdata/dcdicent.h"
#include "dcmdata/dcdatset.h"
#include "dcmdata/dcitem.h"
#include "ofstd/ofconapp.h"
#include "ofstd/ofstd.h"
#include "ofstd/ofdatime.h"
#include "dcmdata/dcuid.h"          // for dcmtk version name 
#include "dcmnet/dicom.h"           // for DICOM_APPLICATION_ACCEPTOR 
#include "dcmdata/dcdeftag.h"       // for DCM_StudyInstanceUID 
#include "dcmdata/dcostrmz.h"       // for dcmZlibCompressionLevel 
#include "dcmnet/dcasccfg.h"        // for class DcmAssociationConfiguration 
#include "dcmnet/dcasccff.h"        // for class DcmAssociationConfigurationFile 
#include "dcmdata/dcdicent.h"
#include "dcmdata/dcvrobow.h"
#include "dcmdata/dcvrpobw.h"
#include "dcmdata/dcistrma.h"
#include <comdef.h>
#include <gdiplus.h>


#ifdef DCM_BUILD
  #define  DCM_DLL   __declspec( dllexport )
#else
  #define  DCM_DLL   __declspec( dllimport )
#endif

typedef std::vector<std::string> STRINGLIST;