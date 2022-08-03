sc create "AgentService" start= auto binPath= "C:\Program Files\Viatronix\Services\AgentService\AgentService.exe" depend= "MSSQL$V3DENT" DisplayName= "Viatronix Agent Service"
sc description "AgentService" "Processes agent tasks for the Viatronix System"
sc config "AgentService" depend= "MSSQL$V3DENT"