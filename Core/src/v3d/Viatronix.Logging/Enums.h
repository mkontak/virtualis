#pragma once 

OPEN_LOGGING_NS

  
/**
 * Dicom Events
 */
public enum class Events
{
  None = 0,       // No Events
  Started,        // Started
  Completed,      // Completed
  Progress,       // Progression
  Failed,         // Failed
  Aborted         // Aborted
};  // Events


/**
 * 
 */
public value struct EventDescriptor 
{
public:

  unsigned short    Id;
  unsigned char     Version;
  unsigned char     Channel;
  unsigned char     Level;
  unsigned char     Opcode;
  unsigned short    Task;
  unsigned __int64  Keyword;

};

CLOSE_LOGGING_NS