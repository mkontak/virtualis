sc create "EnterpriseService" start= auto binPath= "C:\Program Files\Viatronix\Services\EnterpriseService\EnterpriseService.exe" depend= "MSSQL$V3DENT" DisplayName= "Viatronix Enterprise Service"
sc description "EnterpriseService" "Manages the access of all Viatronix resources from workstations, viewers and utilities"