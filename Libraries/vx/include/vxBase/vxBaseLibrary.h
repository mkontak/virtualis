#pragma once


#ifdef VX_BASE_BUILD
#   define  VX_BASE_DLL   __declspec( dllexport )
#elif defined(VX_TEST)
#   define  VX_BASE_DLL
#else
#   define  VX_BASE_DLL   __declspec( dllimport )
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#include <WinSock2.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <comdef.h>
#include <float.h>
#include <gdiplus.h>
#include <io.h>
#include <math.h>
#include <new.h>
#include <process.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <Wbemidl.h>
#include <sqltypes.h>
#include <sql.h>
#include <odbcinst.h>
#include <sqloledb.h>
#include <sqlucode.h>
#include <winhttp.h>
#include <wincrypt.h>

// stl includes
#include <iostream>     // std::cout
#include <algorithm>    // std::transform
#include <vector>       // std::vector
#include <functional>   // std::plus
#include <bitset>
#include <complex>
#include <climits>
#include <cstring>  
#include <iomanip>
#include <ios>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <sstream>
#include <strstream>
#include <typeinfo>
#include <utility>
#include <xutility>
#include <cmath>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <iterator>
#include <ios>
#include <locale>
#include <xlocbuf>




// commonly used vxBase headers
#include "Global.h"
#include "Typedef.h"
#include "Utilities.h" 
#include "Wmi.h"
#include "stdstring.h"
#include "scope_exit.h"
#include "Registry.h"
#include "SoftwareRegistry.h"
#include "DemoRegistry.h"
#include "sql\SqlCommand.h"
#include "sql\SqlConnection.h"
#include "sql\SqlHandle.h"
#include "sql\SqlError.h"
#include "sql\SqlParameter.h"




