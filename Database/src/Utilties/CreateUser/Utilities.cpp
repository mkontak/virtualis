#include "stdafx.h"
#include "Utilities.h"


/**
 * Parses the error message to extract the error code and remove database preamble 
 */
Error Utilities::ParseMessage(const std::string & message)
{

  Error error;

  if (strncmp(message.c_str(), "[Microsoft]", 11) == 0)
  {

    int start(0);

    for ( int i = 0;  i < message.size(); ++i )
    {

      if (message[i] == ':')
      {
        start = i + 1;
        break;
      }
    }

    error.Code = atoi(message.substr(start - 7, 6).c_str());
    error.Message = message.substr(start, message.size() - start).c_str();

  }
  else
  {
    error.Code = -1;
    error.Message = message.c_str();
  }

  return error;



}