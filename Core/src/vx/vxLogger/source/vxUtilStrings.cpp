// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilStrings.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxUtilStrings.h"

// using
using namespace util;

/**
 * Converts the std::wstring to a std::string
 *
 * @param sWideString     Widws string to be converted
 */
std::string Strings::ToString(const std::wstring & sWideString)
{
  std::string sAscii;
  std::for_each(sWideString.begin(), sWideString.end(), [&sAscii] ( WCHAR wch ) 
  {
    sAscii.push_back(static_cast<char>(wch));
  });


  return sAscii;
} // ToString(const std::wstring & sWideString)

/**
 * Converts the guid to a string
 */
std::wstring Strings::ToWString(const std::string & sString)
{
  std::wstring sWide;

  std::for_each(sString.begin(), sString.end(), [&sWide] ( CHAR ch ) 
  {
    sWide.push_back(static_cast<wchar_t>(ch));
  });

  return sWide;
} // ToWString(const std::string & sString)



/**
 * Converts the guid to a string
 */
std::string Strings::ToString(GUID guid)
{
  OLECHAR buffer[512];

  StringFromGUID2(guid, buffer, 512);

  std::string sReturn;
  std::string sGuid = util::Strings::ToString(buffer);

  std::for_each( sGuid.begin(), sGuid.end(), [&sReturn] ( char ch )  { if ( ch != '{' && ch != '}' ) sReturn.push_back(ch); } );

  return sReturn;

} // ToString(GUID guid)




/**
 * Formats a variable list of arguments using the printf argument specifier's in the format
 *
 * @param sFormat     Format string 
 * @param args        Variable argument list that has been initialized
 *
 * @return std::string that will be copied when returned.
 */
void Strings::FormatArgList(std::string & sDestination, const std::string & sFormat, va_list args)
{    

  sDestination = "";
  char * buffer(__nullptr);
  char * realloc_ptr(__nullptr);

  try
  {

    // result of the _vsnprintf
    int   result = -1;

    // Initial length of the buffer
    int   length = 40000;    

   
    // ==================================================
    // While there are still % tags in the format string
    // ==================================================
    do  
    {        
      size_t size ( length * sizeof(char) );

      
      if ( ( realloc_ptr = (char *)realloc(buffer, size)) == __nullptr )
        throw std::exception("Failed to expand the buffer for string formatting");

      buffer = realloc_ptr;

      // Initialize to 0
      memset(buffer, 0, sizeof(buffer));        
    
      // Perform the translation
      result = _vsnprintf_s(buffer, size, length, sFormat.c_str(), args);    

      // Increase the buffer length
      length = length + 2048;    

    }  while ( result == -1 );
  
    // Set up std string
    sDestination.assign(buffer);    
  
  
  }
  catch ( ... )
  {
    //LogErr(Format("Failed to format message (%s), invalid arguments", sFormat), "Strings", "Format");
  }

  if ( buffer != __nullptr )
    free( buffer );


} // FormatArgList(const std::string & sFormat, va_list args)

/**
 * Formats a variable list of arguments using the printf argument specifier's in the format
 *
 * @param sFormat     Format string 
 * @param ....        Variable argument list
 *
 * @return std::string that will be copied when returned.
 */
std::string Strings::Format(const std::string sFormat, ... )
{

  std::string sReturn("");

  // ============================================================
  // If the format string is empty then return and empty string
  // ===========================================================
  if ( sFormat.empty() ) 
    return sReturn;    
 
  // Argument list
  va_list args;    

  // Initialize list
  va_start(args, sFormat);  


  FormatArgList(sReturn, sFormat, args);

  // lose the list
  va_end(args);

  // Return
  return sReturn;


} // Format(const std::string & sFormat, ... )

#pragma region Split Methods

/**
 * Splits the string using the delimiter supplied
 *
 * @param str       String to be split
 * @param delimiter Delimiter
 * @param element   Vector to store the elements in
 */
std::vector<std::string> & Strings::Split(const std::string & str, const std::string & delimiters, std::vector<std::string> & elements) 
{

  std::string element;

  // =============================================================================================
  // Look for any delimiter in the list. Once we have found multiple lines we are done. This is 
  // because I assume that the delimiters are not mixed.
  // ============================================================================================
  std::for_each(str.begin(), str.end(), [&element, &delimiters, &elements ] ( char ch ) 
  {
    if ( delimiters.find((ch)) != std::string::npos )
    {
      if ( element.size() > 0 )
        elements.push_back(element);

      element.clear();
    }
    else 
    {
      element.push_back(ch);
     
    }

  });

  if ( element.size() > 0 )
    elements.push_back(element);

  return elements;

} // Split(const std::string & str, char delimiter, std::vector<std::string> & elements) 
  
/**
 * Splits the string using the delimiter supplied
 *
 * @param str       String to be split
 * @param delimiter Delimiter
 * @param element   Vector to store the elements in
 */
std::vector<std::string> & Strings::Split(const std::string & str, const char delimiter, std::vector<std::string> & elements) 
{
  
  std::stringstream ss(str);    
  std::string item;    
  while( std::getline(ss, item, delimiter) ) 
  {         
    elements.push_back(item);    
  }

 
  return elements;
} // Split(const std::string & str, const char delimiter, std::vector<std::string> & elements)  


/**
 * Splits the string using the delimiter
 *
 * @param str         String
 * @param delimiters  Delimiter list
 * @param inclusive   Inclusive flag ( true - any delimiter matched, flase only one )
 */
std::vector<std::string> Strings::Split(const std::string & sSource, const std::string & delimiters ) 
{    
  std::vector<std::string> elements;    
  return Split(sSource, delimiters, elements);
} //Split(const std::string &s, char delim) 

/**
 * Splits the string using the delimiter
 *
 * @param str       String
 * @param delimiter Delimiter
 */
std::vector<std::string> Strings::Split(const std::string & sSource, const char delimiter) 
{    
  std::vector<std::string> elements;

  // Only one delimiter
  return Split(sSource, delimiter, elements);

} //Split(const std::string &s, const char delimiter ) 



/**
 * Determines if the specified item is in the source vector
 *
 * @param source    Vector to check in
 * @param item      Item to check for
 *
 * @return True if the item is found false otherwise.
 */
bool Strings::Contains(STRINGLIST & source , const std::string & item)
{

  bool contains = false;

  //========================================================================
  // Atempt to add each item in the array if it does not already exists
  //=======================================================================
  std::for_each( source.begin(), source.end(), [&item, &contains] ( std::string sourceItem ) 
  {
    // ===========================================================================
    // If there is a - at the beginning then attempt to remove it from the list
    // ===========================================================================
    if ( item == sourceItem )
    {
      contains = true;
      return;
    }
  } );

  return contains;

} // Contains(const STRINGLIST & source , const std::string & item)

/**
 * Replaces the ch with replace
 *
 * @param sSource   Source   String
 * @param search    Character to be replaced
 * @param replace   Replacement character
 */
std::string & Strings::Replace(std::string & sSource, const char search, const char replace)
{
  std::replace(sSource.begin(), sSource.end(), search, replace);

  return sSource;
} // Replace


/**
 * Replaces the sFind string with the sReplace string
 *
 * @param sSource       String
 * @param sSearch       Search string
 * @param sReplace      Replacement string
 */
std::string & Strings::Replace(std::string & sSource, const std::string & sSearch, const std::string & sReplace)
{

  size_t pos = 0;

  while ( pos != std::string::npos )
  {

    pos = sSource.find(sSearch, pos);

    if ( pos != std::string::npos )
      sSource.replace( pos, sSearch.length(), sReplace.c_str(), sReplace.length() );

  }

  return sSource;
} // Replace


/**
 * Replaces the ch with replace
 *
 * @param sStr      String
 * @param item      Character to be removed
 */
std::string & Strings::Remove(std::string & sSource, const char item)
{
  sSource.erase(std::remove(sSource.begin(), sSource.end(), item), sSource.end());
  return sSource;
} // Replace

/**
 * Removes all occurrences of the item in the source
 *
 * @param source    Vector to check in
 * @param item      Item to check for
 *
 */
void Strings::Remove(STRINGLIST & source , const std::string & item)
{
  // ==============================
  // Remove all copies of the item
  // =============================
  while ( true )
  {
    STRINGLIST::iterator it(std::find(source.begin(), source.end(), item));

    if ( it == source.end() )
      break;

    source.erase(it);
  }


} // Remove(STRINGLIST & source , const std::string & item)


/**
 * Joins the strings together using the delimiter
 *
 * @param sSource     String list
 * @param sDelimniter Delimiter
 *.
 * @return string with all values delimitied
 */
std::string Strings::Join(STRINGLIST & list, const std::string & delimiter )
{

  std::string join;
  

  std::for_each(list.begin(), list.end(),  [ &join, delimiter ] ( std::string  value )
  {
    if ( ! join.empty() )
     join.append(delimiter);
    join.append(value);
  });


  return join;

} // Join()

#pragma endregion