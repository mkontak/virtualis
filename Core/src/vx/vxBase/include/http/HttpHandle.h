// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpHandle.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once


#include <winhttp.h>

namespace http
{

/**
 * Implements a class that encapsulate a HTTP handle
 */
class VX_BASE_DLL  HttpHandle
{
protected:

    // Constructor
    HttpHandle();

    // Destructor (Closes the handle)
    virtual ~HttpHandle();

private:

    /// Copy constructor
    HttpHandle(const HttpHandle & handle);

protected:


    /// Closes the handle
    void Close();

    /// Sets the option
    void SetOption(DWORD option, const void* value,  DWORD length);

    /// Query the option
    void QueryOption(DWORD option, void* value, DWORD& length) const;



public:

    /// Gets the Handle
    const HINTERNET GetHandle() const
    { return m_handle; }

protected:

    // Wrapped handle
    HINTERNET m_handle;

};  // class HttpHandle

} // namespace http