#pragma once

#include <string>

OPEN_VC_NS

class VX_WRAPPER_DLL ContextState
{
public:

  static bool Acquire( const std::string & name, const std::string & data, std::string & id, bool state );

private:

  ContextState();

};

CLOSE_VC_NS