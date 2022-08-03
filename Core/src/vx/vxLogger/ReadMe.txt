This library uses the Microsoft's event logging system. Compiling the library in DEBUG mode will open up additional logging for tracing what is happening in the logger. 
In release mode this logging is disabled with the exception that race events will be logged similiar to the old logger. Log files are created in ETL format so the DumpTrace.exe
utility that is in the Utilities repository will be needed to read this file and extract the event data and copy to a new xml file.


Error/Message Log File: C:\ProgramData\Viatronix\Logs\vxEtwLogger.txt if this directory does not exist then the log will be created in the executing directory.

vxLogProvider      Generats and registers a provider with the event system.
vxLogger        Uses the provider and creates the named session that is used to perform trace events.
vxLogFactor     Creates new loggers that are placed in the named list in the form of a map<Name, Logger>
vxLogRecord     Log record 
