/*
 * Copyright 1999-2002,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: MVSCPPDefs.hpp,v 1.2 2006/02/09 13:55:21 geconomos Exp $
 */

#if !defined(MVSCPPDEFS_HPP)
#define MVSCPPDEFS_HPP

// ---------------------------------------------------------------------------
//  Include some runtime files that will be needed product wide
// ---------------------------------------------------------------------------
#include <sys/types.h>  // for size_t and ssize_t
#include <limits.h>  // for MAX of size_t and ssize_t

// ---------------------------------------------------------------------------
//  A define in the build for each project is also used to control whether
//  the export keyword is from the project's viewpoint or the client's.
//  These defines provide the platform specific keywords that they need
//  to do this.
// ---------------------------------------------------------------------------
#define PLATFORM_EXPORT _Export
#define PLATFORM_IMPORT

// ---------------------------------------------------------------------------
//  Indicate that we do not support native bools
//  If the compiler can handle boolean itself, do not define it
// ---------------------------------------------------------------------------
//#define NO_NATIVE_BOOL

// ---------------------------------------------------------------------------
//  Each compiler might support L"" prefixed constants. There are places
//  where it is advantageous to use the L"" where it supported, to avoid
//  unnecessary transcoding.
//  If your compiler does not support it, don't define this.
// ---------------------------------------------------------------------------
#define XML_LSTRSUPPORT

// ---------------------------------------------------------------------------
//  Indicate that we support C++ namespace
//  Do not define it if the compile cannot handle C++ namespace
// ---------------------------------------------------------------------------
#define XERCES_HAS_CPP_NAMESPACE

// ---------------------------------------------------------------------------
//  Define our version of the XML character
// ---------------------------------------------------------------------------
typedef unsigned short XMLCh;
// typedef wchar_t XMLCh;

// ---------------------------------------------------------------------------
//  Define unsigned 16 and 32 bits integers
// ---------------------------------------------------------------------------
typedef unsigned short XMLUInt16;
typedef unsigned int   XMLUInt32;

// ---------------------------------------------------------------------------
//  Define signed 32 bits integers
// ---------------------------------------------------------------------------
typedef int            XMLInt32;

// ---------------------------------------------------------------------------
//  XMLSize_t is the unsigned integral type.
// ---------------------------------------------------------------------------
#if defined(_SIZE_T) && defined(SIZE_MAX) && defined(_SSIZE_T) && defined(SSIZE_MAX)
    typedef size_t              XMLSize_t;
    #define XML_SIZE_MAX        SIZE_MAX
    typedef ssize_t             XMLSSize_t;
    #define XML_SSIZE_MAX       SSIZE_MAX
#else
    typedef unsigned long       XMLSize_t;
    #define XML_SIZE_MAX        ULONG_MAX
    typedef long                XMLSSize_t;
    #define XML_SSIZE_MAX       LONG_MAX
#endif

// ---------------------------------------------------------------------------
//  Force on the Xerces debug token if it was on in the build environment
// ---------------------------------------------------------------------------
#if defined(_DEBUG)
#define XERCES_DEBUG
#endif


// ---------------------------------------------------------------------------
//  Provide some common string ops that are different/notavail on MVSCPP
// ---------------------------------------------------------------------------

//
// This is a upper casing function. Note that this will not cover
// all NLS cases such as European accents etc. but there aren't
// any of these in the current uses of this function in Xerces.
// If this changes in the future, than we can re-address the issue
// at that time.
//
inline char mytoupper(const char toUpper)
{
    if ((toUpper >= 0x61) && (toUpper <= 0x7A))
        return char(toUpper - 0x20);
    return toUpper;
}

inline char mytolower(const char toLower)
{
    if ((toLower >= 0x41) && (toLower <= 0x5A))
        return char(toLower + 0x20);
    return toLower;
}

inline XMLCh mytowupper(const XMLCh toUpper)
{
    if ((toUpper >= 0x61) && (toUpper <= 0x7A))
       return XMLCh(toUpper - 0x20);
    return toUpper;
}

inline XMLCh mytowlower(const XMLCh toLower)
{
    if ((toLower >= 0x41) && (toLower <= 0x5A))
       return XMLCh(toLower + 0x20);
    return toLower;
}

int stricmp(const char* const str1, const char* const  str2);
int strnicmp(const char* const str1, const char* const  str2, const unsigned int count);



// ---------------------------------------------------------------------------
//  The name of the DLL that is built by the MVSCPP version of the system.
// ---------------------------------------------------------------------------
const char* const Xerces_DLLName = "libxerces-c";

#endif  // MVSCPPDEFS_HPP
