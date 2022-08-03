sc create "WorkQueueService" start= auto binPath= "C:\Program Files\Viatronix\Services\WorkQueueService\WorkQueueService.exe" depend= "MSSQL$V3DENT" DisplayName= "Viatronix Work Queue Service"
sc description "WorkQueueService" "Processes DICOM data for the Viatronix System"
sc config "WorkQueueService" depend= "MSSQL$V3DENT"