// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RegexUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "RegexUtils.h"
#include <regex>



USING_UTILITIES_NS


/**
 *  split expression
 *
 *  @param sText
 *  @param sSplitter
 *  @param output
 */
void RegularExpression::Split( const std::string & sText, const std::string & sSplitter, std::vector< std::string > & output )
{
  Split( sText, sSplitter, [&output]( const std::string & sToken ) { output.push_back( sToken ); });
} // Split


/**
 *  split expression
 *
 *  @param sText
 *  @param sSplitter
 *  @param function
 */
void RegularExpression::Split( const std::string & sText, const std::string & sSplitter, StringTokenFunction function )
{
  typedef std::map< std::string, std::regex > RegexMap;
  static RegexMap rxMap;

  RegexMap::iterator iter = rxMap.find( sSplitter );
  if( iter == rxMap.end() )
    iter = rxMap.insert( std::make_pair( sSplitter, std::regex( sSplitter ))).first;

  std::regex & rx = iter->second;
  for( std::sregex_token_iterator iter( sText.begin(), sText.end(), rx, -1 ), iterEnd; iter != iterEnd; ++iter )
    function((*iter).str() );
} // Split
