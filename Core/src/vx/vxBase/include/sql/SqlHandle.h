// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlHandle.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once



namespace sql
{

// forward declaration
class VX_BASE_DLL  SqlError;

/**
 * Wraps a Handle used in the Sql objects. There are 3 different types, but all have the same operations.
 */
class VX_BASE_DLL  SqlHandle
{
public:

  typedef enum SqlHandleTypes
  {
    HT_Environment  = SQL_HANDLE_ENV,     // Environment handle used in SqlConnection
    HT_Connection   = SQL_HANDLE_DBC,     // Connection handle used in SqlConnection
    HT_Statement    = SQL_HANDLE_STMT     // Statement handle used in SqlCOmmand
  } SqlHandleTypes;

private:

  // Constructor
  SqlHandle();

public:

  // Constructs a handle of the type specified
  SqlHandle(SqlHandleTypes type);

  // Destructor (Frees the handle)
  ~SqlHandle();

  // Frees the handle
  void Free();

  // Allocates the handle
  void Allocate();

  // Allocates the handle using the supplied handle as it's dependent
  void Allocate(const SqlHandle & handle);

  // Gets the handle
  const SQLHANDLE GetHandle() const
  { return m_handle; }

  // Gets the handles error
  SqlError GetError();

  // Flag to indicate if the handle is allocated
  const bool IsAllocated() const
  { return ( m_handle != SQL_NULL_HANDLE ); }

private:

  // Handle type
  SqlHandleTypes m_type;
  
  // Handle
  SQLHANDLE m_handle;

  // Friend classes
  friend class SqlCommand;
  friend class SqlConnection;

};  // class SqlHandle

}; // namespace sql