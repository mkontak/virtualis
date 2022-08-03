// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilStrings.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


// namespace
namespace util
{

  // typedefs
  typedef std::vector<std::string> STRINGLIST;


  /**
   * Implements String utilities
   */
  class  VX_LOGGER_DLL Strings
  {

  public:

    /// determines if the item specified is contained in the vector
    static bool Contains( STRINGLIST & source, const std::string & item  );
    
    /// delete the item specified is contained in the vector
    static void Remove( STRINGLIST & source, const std::string & item  );

    /// Replaces all occurrences of character with the replacement character
    static std::string & Replace(std::string & sSource, const char search, const char replace);

    /// Replaces all occurrences of character with the replacement character
    static std::string & Replace(std::string & sSource, const std::string & sSearch, const std::string & sReplace);

    /// Removes all occurrences of character 
    static std::string & Remove(std::string & sSource, const char item);

    /// Format the data into the destination
    static std::string Format(const std::string & sFormat, ... );

    /// Converts std::string to std::wstring
    static std::wstring ToWString(const std::string & sString);

    /// Converts std::wstring to std::string
    static std::string ToString(const std::wstring & sWideString);

    /// Converts the GUID to a string
    static std::string ToString(GUID guid);

    /// Join the strings together using the delimiter
    static std::string Join(std::vector<std::string> & list, const std::string & delimiter  );

    // Splits string by delimiter
    static std::vector<std::string> Split(const std::string & sSource, const char delimiter );
  
    // Splits string by delimiter (Inclusive meaning any delimiter in the list is matched)
    static std::vector<std::string> Split(const std::string & sSource, const std::string & delimiters );

    /// Scribs the &gt and &lt from the xml converting then to > and < respectivly
    static inline void ScrubXml(std::string & sXml)  
    { util::Strings::Replace(util::Strings::Replace(sXml, "&gt;", ">"), "&lt;", "<"); }


  private:

    /// Splits the string based on the delimiter list
    static std::vector<std::string> & Split(const std::string & sSource, const std::string & delimiters, std::vector<std::string> & elements);

    /// Splits the string based on the single delimiter
    static std::vector<std::string> & Split(const std::string & sSource, const char delimiter, std::vector<std::string> & elements);

    // Formats the string using the format and argument list
    static void FormatArgList(std::string & sDestination, const std::string & sFormat, va_list args);

  };  // class Strings

};  // namespace utils

#pragma warning(pop)