// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PacsInfo.h
//
// Description: Encapsulates the pas information AETitle, Hostname and port
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)
#pragma once



// ===========================
// Pacs Information Structure
// ==========================
typedef struct PacsInfo
{
  std::string AETitle;
  std::string Hostname;
  uint4 Port;
  bool SupportsEncapsulatedPdf;

} PacsInfo;