// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileServerHeader.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#include "Logger.h"

namespace io
{

#define STATUS_OK       0
#define STATUS_FAIL     1

/**
 * Transaction Types
 *
 * Only two types of transactions REQUEST and RESPONSE. 
 */
typedef enum FileServerTranscationTypes : char
{
  REQUEST   = 0x01,       
  RESPONSE  = 0x02        
} TypTransactionTypes;


/**
 * Command Types
 */
typedef enum FileServerCommand : short
{
  CMD_OPEN        = 0x0001,       
  CMD_CLOSE       = 0x0002,
  CMD_FLUSH       = 0x0003,
  CMD_SEEK        = 0x0004,
  CMD_READ        = 0x0005,
  CMD_WRITE       = 0x0006,
  CMD_SIZE        = 0x0007,
  CMD_FEOF        = 0x0008,
  CMD_SMART_EOF   = 0x0009,
  CMD_QUERY       = 0x000A,
  CMD_EXISTS      = 0x000B,
  CMD_GETFILES    = 0x000C,
  CMD_GETDIRS     = 0x000D,
  CMD_PING        = 0x000F,
  CMD_DELETE      = 0X0010
} FileServerCommand;


#define LAST_COMMAND        0X0010
#define FILE_HANDLE_SIZE    64

/**
 * File Server Header Interface
 */
typedef struct FILESERVERHEADER
{

  FileServerTranscationTypes Type;             // Type of transcation REQUEST or RESPONSE
  FileServerCommand Command;                   // Command 

  char Handle[FILE_HANDLE_SIZE];    // File Handle on the server


  int4 ModeFlags;                       // Open:    Open mode flags
  int4 CreateFlags;                     // Open:    Create flags 
   
  bool IsEof;                           // EOF inducator (Used during the response)
  int StatusCode;                       // Status Code (Used during the repsponse)

  __int64 Size;                         // Request:   Size of the payload
                                        // Response:  Size of the payload except 

} FILESERVERHEADER;


#define LPFILESERVERHEADER (FILESERVERHEADER *)
#define LPPFILESERVERHEADER (LPFILESERVERHEADER *)

#define FILESERVERHEADER_SIZE sizeof(io::FILESERVERHEADER)



inline std::string ToString(const FileServerCommand & command)
{
  static const char * commands [] = { "Undefined", "Open", "Close", "Flush", "Seek", "Read", "Write", "Size", "EOF", "SmartEOF",  "Query", "Exists", "GetFiles", "GetDirs", "Ping", "Delete" };

  return ( (int)command < 0 || (int)command > LAST_COMMAND ? commands[0] : commands[(int)command] ); 
}

inline std::string ToString(const FileServerTranscationTypes & transactionType)
{
  static const char * transactionTypes [] =  { "Undefined", "Request", "Response" };

  return ( (int)transactionType < 0 || (int)transactionType > 2 ? transactionTypes[0] : transactionTypes[(int)transactionType] ); 
}

inline void ToLogger(const FILESERVERHEADER & header, const std::string & sClass, const std::string & sMethod )
{
  std::ostringstream os;

  os << ToString(header.Type) << ", " << ToString(header.Command) << ", " << header.Handle << ", " << header.Size << ", " << header.ModeFlags << ", " << header.CreateFlags << ", " << header.IsEof << ", " << header.StatusCode;

  LogDbg(os.str(), sClass, sMethod);

}

} // namespace io