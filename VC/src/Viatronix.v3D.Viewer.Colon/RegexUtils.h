// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RegexUtils.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <vector>
#include <functional>

OPEN_UTILITIES_NS


namespace RegularExpression
{

  typedef std::function< void( const std::string & sToken )> StringTokenFunction;
   // Splits the text using the spl;itter string
 void Split( const std::string & sText, const std::string & sSplitter, StringTokenFunction function ); 

   // Splits the text using the spl;itter string
 void Split( const std::string & sText, const std::string & sSplitter, std::vector< std::string > & output ); 

} // namespace RegularExpression

CLOSE_UTILITIES_NS