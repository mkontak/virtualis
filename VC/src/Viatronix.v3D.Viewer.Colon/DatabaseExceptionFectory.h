#pragma once

#include "Exception.h"

class DatabaseExceptionFactory
{

public:

  static void Throw(ex::VException & ex, const std::string & sClass, const std::string & sMethod);


};  // class DatabaseExceptionFactory