#pragma once


OPEN_LOGGING_NS

/**
 * Log level ma directly to vxEtwLogger library
 */
public enum class LogLevel
{
   None        = 0,   // Tracing is not on
   Critical    = 1,   // Abnormal exit or termination
   Fatal       = 1,   // Deprecated name for Abnormal exit or termination
   Error       = 2,   // Severe errors that need logging
   Exception   = 2,   // Same as error
   Warning     = 3,   // Warnings such as allocation failure
   Information = 4,   // Includes non-error cases(e.g.,Entry-Exit)
   Timing      = 5,   // Timing
   Debug       = 6,   // Backward compatible
   Verbose     = 6,   // Detailed traces from intermediate steps
   Highest     = 6,
};  // public enum class LogLevel



/**
 * Logger Types
 */
public enum class MessageFormat
{
  Default = 0,
  Text    = 0,        // Shortened text base
  Xml     = 1         // Xml based
};

CLOSE_LOGGING_NS