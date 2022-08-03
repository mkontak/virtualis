// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfLibrary.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <comdef.h>
#include <gdiplus.h>
#include <memory>
#include <istream>

#include "QuickPDFDLL0916.h"
#include "Logger.h"

typedef std::vector<std::shared_ptr<Gdiplus::Bitmap>> IMAGELIST;
