========================================================================
       CONSOLE APPLICATION : PurgeLogs
========================================================================

Syntax:

PurgeLogs [-logpath PATH_TO_LOG_FILES]  [-maxdays DAYS_TO_WAIT_BEFORE_DELETING] [-archive ACHIVE_SUB_DIRECTORY]

Synopsis:

-logpath      Specifies the log path to be purged. ( Default is C:\Program Data\Viatronix\Log).
              
-maxdays      Specifies the maximum number of days to keep the old log files around (Deafult is 7).

-archive      Specifies the archive sub-directory off the log path to bew used, (Default is "ArchiveLogs")


Description:

This program will purge all the log files found in the path specified in the -logPath
arguement. The files to be purged are only those with a .txt or .xml extension but all 
database backup log files will be ignored. 

The first stage of the process will perform the purging by moving the files to the old 
logs path which is a sub-directory "Old" off the -logpath. If the old path does not 
exist it will be created. Once the files have been moved any xml files will be 
re-headered so that the style sheet will work. 


The second stage of the prcess will delete and files in the old log path that are older 
-maxdays old. 



NOTE: This files will only be moved once a day, which means that repeatedly running this 
      application with the same parameters will have no affect after it was run the first
      time. The preserves the old log files so that they are not overritten acidentally.


Example(s):


PurgeLog

Executing the application with no arguements will purge all files in the C:\V-System\Log 
directory and move th files to the C:\V-System\Log\Old diectory. All files in the 
C:\V-System\Log\Old directory older then 7 days will be deleted.


PurgeLog -maxdays 0

Executing ths application with following arguements will purge the files in the C:\V-System\Log
directory. No files will be saved since the numbar of days to save the files is 0.


Possible Issues:

If the log path changes then the old log saved in the previous old log path will never be 
cleaned up. The program has no memory of what it did only what it's about to do.