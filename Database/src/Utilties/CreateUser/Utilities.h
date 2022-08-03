#pragma once

typedef struct Error
{
  int  Code;
  CString Message;
} ErrorMessage;

class Utilities
{

public:

  static Error ParseMessage(const std::string & message);

};