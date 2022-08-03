sc create "DicomService" start= auto binPath= "C:\Program Files\Viatronix\Services\DicomService\DicomService.exe" depend= "MSSQL$V3DENT" DisplayName= "Viatronix Dicom Service"
sc description "DicomService" "Receives/stores DICOM data into the Viatronix System"
sc config "DicomService" depend= "MSSQL$V3DENT"