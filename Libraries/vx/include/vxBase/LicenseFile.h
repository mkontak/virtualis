// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LicenseFile.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Wmi.h"

class VX_BASE_DLL LicenseFile
{
protected:

    /// constructor
  LicenseFile( ) { }

public:


  /// creates a license file from license data
  void Create();

  /// opens the license file
  void Open( const std::string & sFile ) { Open( std::ifstream( sFile, std::ios::in )); }
  void Open( std::istream & input );

  /// saves the license file
  void Save( const std::string & sFile ) { Save( std::ofstream( sFile, std::ios::out )); }
  void Save( std::ostream & output );

  /// determines if the license is valid
  bool IsValid();

  /// Generates a license file on disk
  static void Generate( const std::string & sFile );

  /// checks validity of the license
  static bool Check( const std::string & sFile );

private:

  /// system id
  std::string m_systemID;


}; //LicenseFile

