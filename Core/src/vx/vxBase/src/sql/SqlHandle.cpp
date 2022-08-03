// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlHandle.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

// includes
#include "stdafx.h"
#include "sql\SqlHandle.h"
#include "Exception.h"
#include "sql\SqlError.h"

// using
using namespace sql;


/**
 * Default constructor
 */
SqlHandle::SqlHandle() :
m_type(HT_Environment),
m_handle(SQL_NULL_HANDLE)
{

} // SqlHandle()


/**
 * Constructs a handle of the specified type
 *
 * @param type
 */
SqlHandle::SqlHandle(SqlHandleTypes type) :
m_type(type),
m_handle(SQL_NULL_HANDLE)
{

} // SqlHandle(SqlHandleTypes type) 


/**
 * Destrcutor
 */
SqlHandle::~SqlHandle()
{
  // Free the handle
  Free();
} // ~SqlHandle()


/**
 * Frees the handle
 */
void SqlHandle::Free()
{
  // ==========================
  // Only free if not null
  // ==========================
  if ( m_handle != SQL_NULL_HANDLE )
  {

    SQLRETURN retcode = SQLFreeHandle((SQLSMALLINT)m_type, m_handle);

    if ( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
      // Cannot get the the SqlError since you need a handle so I just give you and error code.
      LogErr(util::Strings::Format("Failed to free handle(%d) [%ld] [%ld]",m_type, retcode, GetLastError()),"SqlHandle","Free");
    }

  }

  // Null out the handle
  m_handle = SQL_NULL_HANDLE;

} // Free()


/**
 * Allocate SQL handle
 *
 * @param handle    Handle to allocate
 *
 */
void SqlHandle::Allocate(const SqlHandle & handle)
{

  // ===============================================
  // Make sure the handle is not already allocated
  // ===============================================
  if ( m_handle == SQL_NULL_HANDLE )
  {
    SQLRETURN retcode = SQLAllocHandle((SQLSMALLINT)m_type, handle.m_handle, &m_handle);

    if ( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
      throw ex::SqlException(LogRec(util::Strings::Format("Failed to allocate sql handle : %s ", GetError().GetLogMessage().c_str()), "SqlHandle","Allocate"));

  } // END ... If the handle is NULL

} // Allocate(const SqlHandle & handle)


/**
 * Allocate SQL handle
 */
void SqlHandle::Allocate()
{

  // ===============================================
  // Make sure the handle is not already allocated
  // ===============================================
  if ( m_handle == SQL_NULL_HANDLE )
  {
    SQLRETURN retcode = SQLAllocHandle((SQLSMALLINT)m_type, SQL_NULL_HANDLE, &m_handle);

    if ( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
       throw ex::SqlException(LogRec(util::Strings::Format("Failed to allocate sql handle : %s ", GetError().GetLogMessage().c_str()), "SqlHandle","Allocate"));

  } // END ... If the handle is NULL

} // Allocate()


/**
 * Get Error
 */
SqlError SqlHandle::GetError()
{
  return SqlError((SQLSMALLINT)m_type, m_handle);
} // GetError()