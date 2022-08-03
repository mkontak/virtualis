// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfUtilities.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once 


namespace pdf
{

/**
 * Implements utilities used in the PDF library
 */
class PdfUtilities
{
// methods
public:

  /// Gets the encoder list 
  static void GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

  /// Converts std::string to std::wstring
  static std::wstring ToWString(const std::string & sString);

  /// Converts std::string to wchar_t
  static wchar_t * ToWChar( const std::string & sString );

};  // class PdfUtilities

};  // namespace pdf