// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlParameter.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once

// includes
#include "Exception.h"

namespace sql
{


#pragma region enums



/**
 * Parameter direction
 */
typedef enum VX_BASE_DLL ParameterDirections
{
#if (ODBCVER >= 0x0380)
  PD_INOUT_STREAM     = SQL_PARAM_INPUT_OUTPUT_STREAM,
  PD_OUTPUT_STREAM    = SQL_PARAM_OUTPUT_STREAM,
#endif
  PD_UNKNOWN       = SQL_PARAM_TYPE_UNKNOWN,
  PD_INPUT         = SQL_PARAM_INPUT,
  PD_INOUT         = SQL_PARAM_INPUT_OUTPUT,
  PD_RESULT_COL    = SQL_RESULT_COL,
  PD_OUTPUT        = SQL_PARAM_OUTPUT,
  PD_RETURN        = SQL_RETURN_VALUE

} ParameterDirections;


/**
 * C-Types Enumeration
 */
typedef enum VX_BASE_DLL CTypes
{
#if (ODBCVER >= 0x0300)
  CT_NUMERIC = SQL_C_NUMERIC,
#endif  

  CT_DEFAULT    = SQL_C_DEFAULT,
  CT_CHAR       = SQL_C_CHAR,
  CT_LONG       = SQL_C_LONG,
  CT_SHORT      = SQL_C_SHORT,
  CT_FLOAT      = SQL_C_FLOAT,
  CT_DOUBLE     = SQL_C_DOUBLE,
  CT_BIT        = SQL_C_BIT,
  CT_DATE       = SQL_C_DATE,
  CT_TIME       = SQL_C_TIME,
  CT_TIMESTAMP  = SQL_C_TIMESTAMP,
  CT_TINYINT    = SQL_C_TINYINT,
  CT_BINARY     = SQL_C_BINARY
} CTypes;


/**
 * SQL Type enumeration
 */
typedef enum VX_BASE_DLL SqlTypes
{
#if (ODBCVER >= 0x0300)
  ST_DATETIME     = SQL_DATETIME,
#endif

  ST_UNKNOWN      =  SQL_UNKNOWN_TYPE,
  ST_CHAR         =  SQL_CHAR,
  ST_NUMERIC      =  SQL_NUMERIC,
  ST_DECIMAL      =  SQL_DECIMAL,
  ST_INTEGER      =  SQL_INTEGER,
  ST_SMALLINT     =  SQL_SMALLINT,
  ST_FLOAT        =  SQL_FLOAT,
  ST_REAL         =  SQL_REAL,
  ST_DOUBLE       =  SQL_DOUBLE,
  ST_VARCHAR      =  SQL_VARCHAR,
  ST_BINARY       =  SQL_BINARY,
  ST_BIT          =  SQL_BIT,
  ST_DATE         =  SQL_DATE,
  ST_TIME         =  SQL_TIME,
  ST_TIMESTAMP    =  SQL_TIMESTAMP,
  ST_TINYINT      =  SQL_TINYINT,
  ST_LONGVARCHAR  = SQL_LONGVARCHAR,
  ST_VARBINARY    = SQL_VARBINARY
} SqlTypes;

#pragma endregion


#pragma region class SqlParameter

 /**
   * Implements SqlParameter used for binding parameters to a command
   */
class VX_BASE_DLL SqlParameter 
{

protected:

  /// Default constructor
  SqlParameter(const std::string & sName, ParameterDirections eDirection = PD_INPUT) :
  m_eSqlType(ST_VARCHAR),
  m_eCType(CT_CHAR),
  m_eDirection(eDirection),
  m_sName(sName),
  m_iLength(0),
  m_iNumber(0),
  m_iSize(0)
  {
  }


public:

  /// Creates SqlParameter
  template<class T>
  static std::shared_ptr<SqlParameter> Create(const std::string & sName, const T & value, int iSize = SQL_NTS, SqlTypes eSqlType = ST_VARCHAR, ParameterDirections eDirection = PD_INPUT)
  {
    std::shared_ptr<SqlParameter> pParameter(std::shared_ptr<SqlParameter>(new SqlParameter(sName, eDirection)));

    if ( pParameter == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate SqlParameter","SqlParameter", "Create"));


    std::ostringstream os;

    os << value;
    
    std::string sValue(os.str());

    const int iValueSize(  iSize == SQL_NTS ? (int)sValue.size() + 1: iSize  );
    pParameter->m_value = std::shared_ptr<unsigned char>( new unsigned char[iValueSize],  [] (unsigned char * value) { delete [] value; } );

    RtlZeroMemory(pParameter->m_value.get(),iValueSize);

    if ( pParameter->m_value == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer","SqlParameter", "Create"));

    memcpy(pParameter->m_value.get(), sValue.c_str(), sValue.size());

    pParameter->m_iLength = SQL_NTS;
    pParameter->m_iSize   = ( iSize == SQL_NTS ? (int)sValue.size() : iSize );
    pParameter->m_eSqlType = eSqlType;
  
    return pParameter;

  } // Create(const std::string & sName, const T & value, int iSize, ParameterDirections eDirection = PD_INPUT)

  
public:

  /// Gets the parameter direction
  virtual const ParameterDirections GetDirection() 
  { return m_eDirection; }

  /// Sets the parameter direction
  void SetDirection(ParameterDirections direction)
  { m_eDirection = direction; }

  /// Gets the C-Type
  virtual const CTypes GetCType() 
  { return m_eCType; }

  /// Sets the C-Type
  void SetCType(CTypes ctype) 
  { m_eCType = ctype; }

  /// Gets th SQL Type
  virtual const SqlTypes GetSqlType() 
  { return m_eSqlType; }

  // Sets the Sql Type
  virtual void SetSqlType(SqlTypes eSqlType) 
  { m_eSqlType = eSqlType; }

  /// Gets the value
  virtual SQLCHAR * GetValue()
  { return m_value.get();   }

  /// Get the size of the buffer
  virtual const int GetSize()
  { return m_iSize; }

  /// Gets the name
  virtual const std::string & GetName() const
  { return m_sName; }

  /// Gets the length
  SQLLEN GetLength()
  { return m_iLength; }

  ///
  SQLLEN * GetLengthPtr()
  { return &m_iLength; }

  /// Gets the parameter number
  unsigned int GetNumber()
  { return m_iNumber; }

protected:

  /// Direction
  ParameterDirections m_eDirection;

  /// C-Type
  CTypes m_eCType;

  /// Sql -Type
  SqlTypes m_eSqlType;

  /// Size
  int m_iSize;

  ///
  std::shared_ptr<unsigned char> m_value;

  /// Parameter name
  std::string m_sName;

  ///
  SQLLEN m_iLength;

  /// Parameter number
  int m_iNumber;

private:

  /// Friend
  friend class SqlOutParameter;
  friend class SqlInOutParameter;
  friend class SqlInParameter;
  friend class SqlCommand;

}; // class SqlParameter



#pragma endregion

#pragma region class SqlInOutParameter

/**
 * Implements an SqlInOutParameter
 */
class SqlInOutParameter : public SqlParameter
{
protected:
	/// constructor
  SqlInOutParameter(const std::string & sName) :
  SqlParameter(sName, PD_INOUT)
  {
  } //

public:

  ///
  template<class T>
  static std::shared_ptr<SqlParameter> Create(const std::string & sName, const T & value, int iSize, SqlTypes eSqlType = ST_VARCHAR )
  {
    std::shared_ptr<SqlParameter> pParameter(SqlParameter::Create<T>(sName, value, iSize, eSqlType, PD_INOUT));

    if ( pParameter == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate SqlInOutParameter","SqlOutParameter","Create"));
  
    return pParameter;

  } // Create(const std::string & sName,  int iSize)


}; // SqlOutParameter



#pragma endregion


#pragma region class SqlOutParameter

/**
 * Implements an SqlOutParameter
 */
class SqlOutParameter : public SqlParameter
{
protected:

  // Constructor
  SqlOutParameter(const std::string & sName) :
  SqlParameter(sName, PD_OUTPUT)
  {
  } // 

public:

  ///
  static std::shared_ptr<SqlParameter> Create(const std::string & sName, int iSize, SqlTypes eSqlType = ST_VARCHAR )
  {
    std::shared_ptr<SqlParameter> pParameter(std::shared_ptr<SqlParameter>((SqlParameter *)new SqlOutParameter(sName)));

    if ( pParameter == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate SqlParameter","SqlOutParameter","Create"));


    pParameter->m_value = std::shared_ptr<unsigned char>( new unsigned char[iSize], [] (unsigned char * value) { delete [] value; } );

    RtlZeroMemory(pParameter->m_value.get(),iSize);

    if ( pParameter->m_value == __nullptr )
      throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer for value","SqlOutParameter","Create"));

    pParameter->m_iSize = iSize;
    pParameter->m_eSqlType = eSqlType;
   
    return pParameter;

  } // Create(const std::string & sName,  int iSize)


}; // SqlOutParameter



#pragma endregion

#pragma region class SqlInParameter

/**
 * Implements an SqlOutParameter
 */
class SqlInParameter : public SqlParameter
{
protected:

  // Constructor
  SqlInParameter(const std::string & sName) :
  SqlParameter(sName, PD_INPUT)
  {
  } //

public:

  /// Creates the SqlInParameter
  template<class T>
  static std::shared_ptr<SqlParameter> Create(const std::string & sName,  const T & value, SqlTypes eSqlType = ST_VARCHAR )
  {
    
   std::shared_ptr<SqlParameter> pParameter(SqlParameter::Create<T>(sName, value, SQL_NTS, eSqlType, PD_INPUT));

   if ( eSqlType == ST_VARCHAR )
     pParameter->m_iLength = SQL_NTS;

   return pParameter;

  } // Create(const std::string & sName,  const T & value, SqlTypes eSqlType = ST_VARCHAR )

}; // class SqlInParameter


#pragma endregion

}; // namespace sql