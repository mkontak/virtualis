#pragma once

#define _CRT_SECURE_NO_DEPRECATE


#include "Logger.h"
#include "vxBaseLibrary.h"




#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#define OPEN_CRYPTO_NS namespace crypto {
#define CLOSE_CRYPTO_NS }
#define USING_CRYPTO_NS using namespace crypto;



inline std::string trim_right(const std::string &source , const std::string& t = " ")
{
std::string str = source;
return str.erase( str.find_last_not_of(t) + 1);
}

inline std::string trim_left( const std::string& source, const std::string& t = " ")
{
std::string str = source;
return str.erase(0 , source.find_first_not_of(t) );
}

inline std::string trim(const std::string& source, const std::string& t = " ")
{
std::string str = source;
return trim_left( trim_right( str , t) , t );
} 
