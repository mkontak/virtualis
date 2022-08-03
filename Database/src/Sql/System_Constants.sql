USE [System]
GO


BEGIN


  DELETE FROM [v3d].[SystemSettings]
  DELETE FROM [v3d].[Versions]
  DELETE FROM [v3d].[Masks]
  DELETE FROM [v3d].[StudyGroups]
  DELETE FROM [v3d].[Stores]
  DELETE FROM [v3d].[Modalities]
  DELETE FROM [v3d].[SystemConfigs]


END
GO



-- =================================================================
-- Insert the object versions
-- =================================================================
BEGIN
  
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  VALUES ( 'System',     'database', '4.0' )

  -- Insert all tables (Except ErrorMessages which is shared amonst all databases)
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  SELECT [name], 'table', '4.0' FROM [sys].[tables] 
  

END
GO


-- ===============================================================================================================
-- Inserts the products into the system (These are viewers products)
-- ===============================================================================================================
BEGIN
  INSERT INTO [v3d].[Applications] ( [id], [name], [pri], [mask], [inst], [desc], [url], [launch] ) 
        VALUES (  'VC',   'Virtual Colonoscopy', 1, 1, 0, 'Virtual Colonoscory', 
                  '$(ViewersDirectory)VC\V3DViewerColon.exe',
                  '<rule app="VC" min="1" max="2" ort="%S|%P|%DL|%DR" session="LIFO" dicom="FIFO" type="6" />' )

  INSERT INTO [v3d].[Applications] ( [id], [name], [pri],[mask], [inst], [desc], [url], [launch] ) 
        VALUES (  'VAS',  'Vascular', 1, 8, 0, 'Vascular', 
                  '$(ViewersDirectory)VAS\V3DViewer.exe',
                  '<rule app="VAS" min="1" max="2" ort="" session="LIFO" dicom="FIFO" type="2" />' )

  INSERT INTO [v3d].[Applications] ( [id], [name], [pri],[mask], [inst], [desc], [url], [launch]  ) 
        VALUES (  'CS',   'Calcium Scoring', 1, 64, 0, 'Calcium Scoring', 
                  '$(ViewersDirectory)CS\V3DViewer.exe',
                  '<rule app="CS"  min="1" max="1" ort="" session="LIFO" dicom="FIFO" type="1" />' )

  INSERT INTO [v3d].[Applications] ( [id], [name], [pri],[mask], [inst], [desc], [url], [launch]  ) 
        VALUES (  'CAR',  'Cardiac', 1, 2, 0, 'Cardiac', 
                  '$(ViewersDirectory)CAR\V3DViewer.exe',
                  '<rule app="CAR"  min="1"  ort="" session="LIFO" dicom="FIFO" type="1" />' )

  INSERT INTO [v3d].[Applications] ( [id], [name], [pri],[mask], [inst], [desc], [url], [launch]  ) 
        VALUES (  'XP',   'Explorer', 1, 4, 0, 'General 2D/3D Viewer', 
                  '$(ViewersDirectory)XP\V3DViewer.exe',
                  '<rule app="XP"  min="1" max="1" ort="" session="LIFO" dicom="FIFO" type="1" />' )
END
GO

-- ==========================
-- Inserts STUDY_GROUPS
-- ==========================
BEGIN

  INSERT INTO [v3d].[StudyGroups] ( name, [desc] ) VALUES ( '',         'Default group is blank for all studies' )
  INSERT INTO [v3d].[StudyGroups] ( name, [desc] ) VALUES ( 'Training', 'Groups studies as training' )

END
GO

-- ==================================
-- Insert Modalities
-- ==================================
BEGIN

  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('CT' ,'Computed Tomography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('MR' ,'Magnetic Resonance')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('PT' ,'Positron emission tomography (PET)')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('OT' ,'Other')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('XA' ,'X-Ray Angiography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('AU' ,'Audio')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('BI' ,'Biomagnetic imaging')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('CD' ,'Color flow Doppler')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('CR' ,'Computed Radiography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('DD' ,'Duplex Doppler')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('DG' ,'Diaphanography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('DX' ,'Digital Radiography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('ECG' ,'Electrocardiography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('EPS' ,'Cardiac Electrophysiology')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('ES' ,'Endoscopy')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('GM' ,'General Microscopy')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('HC' ,'Hard Copy')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('HD' ,'Hemodynamic Waveform')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('IO' ,'Intra-oral Radiography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('LS' ,'Laser surface scan')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('MG' ,'Mammography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('NM' ,'Nuclear Medicine')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('PR' ,'Presentation State')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('PX' ,'Panoramic X-Ray')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RF' ,'Radio Fluoroscopy')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RG' ,'Radiographic imaging (conventional film/screen)')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RTDOSE' ,'Radiotherapy Dose')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RTIMAGE' ,'Radiotherapy Image')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RTPLAN' ,'Radiotherapy Plan')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RTRECORD' ,'RT Treatment Record')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('RTSTRUCT' ,'Radiotherapy Structure Set')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('SC' ,'Secondary Capture')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('SM' ,'Slide Microscopy')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('ST' ,'Single-photon emission computed tomography (SPECT)')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('TG' ,'Thermography')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('US' ,'Ultrasound')
  INSERT INTO [v3d].[Modalities] ( [code], [desc] ) VALUES ('XC' ,'External-camera Photography')

END
GO

-- =================================================================
-- Insert the Masks
-- =================================================================
BEGIN

  -- Series Type Mask
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'type', 'dicom',          1,  'Dicom' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'type', 'preprocessed',  	2,  'Preprocessed' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'type', 'cad', 	          4,  'Cad' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'type', 'session',  	    8,  'Session' )
   
  -- Series State Mask
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'warning',  	   1, 	'Indicates a warning state (see additional information)' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'receiving',    2,	'Indicates a receiving state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'processing',   4,	'Indicates a processing state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'converting',   8,	'Indicates a converting state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'completed',   16,	'Indicates a completed state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'pending',   	32,	'Indicates a pending state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'failed',   	  64,	'Indicates a failed state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'aborted',   	128,	'Indicates an aborted state' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'limited',   	256,	'Indicates a limited nature for item')
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'locked',     4096,	'Indicates a locked state (being viewed)')
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'state', 'paused',     8192,	'Indicates a paused state')


  -- Orientation Mask
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'HFS',    1, 'HFS' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'FFS',    2, 'FFS' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'HFP',    4, 'HFP' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'FFP',    8, 'FFP' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'HFDL',  16, 'HFDL' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'FFDL',  32, 'FFDL' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'HFDR',  64, 'HFDR' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'orientation', 'FFDR', 128, 'FFDR' )

  -- Plane if the image
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'plane', 'oblique',   8,'Oblique' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'plane', 'axial',     4,'Axial' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'plane', 'coronal',   2,'Coronal' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'plane', 'sagittal',  1,'Sagittal' )

 -- Day Mask
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'sun',     1, 'Sunday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'mon',     2, 'Monday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'tue',    4, 'Tuesday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'wed',  8, 'Wednesday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'thu',  16, 'Thursday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'fri',    32, 'Friday' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'day', 'sat',  64, 'Saturday' )


   -- Dicom Hosts Service Types
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'service', 'query',     1, 'C-FIND (Query)' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'service', 'store',     2, 'C-STORE (Store)' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'service', 'retreive',  4, 'C-MOVE (Retrieve/Push)' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'service', 'print',     8, 'C-PRINT' )
  INSERT INTO [v3d].[Masks] ( [name], [bn], [bit], [desc] )  VALUES ( 'service', 'all',       15, 'All Services' )

END
GO


-- ===================================================
-- Insert system settings for the sub system
-- =================================================== 
BEGIN


 -- ==================
 -- Security Settings
 -- =================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Security',	 'AuthenticationDomain',	'', 'Authentication domain, only used when the authentication type is Windows' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Security',	 'ContextTtlUnits', 'minute', 'Context time to live units' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Security',	 'DefaultUserExpiration', '256', 'Default user expiration setting' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Security',   'ContextTtl', '30', 'Default context ttl' ) 

 -- ==================
 -- Auditing settings
 -- ==================
   
 -- ==================
 -- System settings
 -- =================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'LargestSeriesSize', '100MB', 'Largest series size, used to determine which store to put a series in' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ExcludeIdCheck', 'True', 'Flag to exclude the system id check during periodic system checks (Improves performance)')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'PathSeperator', '\', 'Path Seperator')  

 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'DiskErrorThreshold', '95', 'Determines when errors are generated. Specified as a % value')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'DiskWarningThreshold', '75', 'Determines when warnings are generated. Specified as a % value')  


 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'InstallDirectory', 'C:\Program Files\Viatronix', 'Installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ConfigDirectory', '$(APPDATA)\Viatronix\Config\', 'Config installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ServicesDirectory', '$(InstallDirectory)\Services\', 'Service installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ViewersDirectory', '$(InstallDirectory)\Viewers\', 'Viewers installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ProcessorDirectory', '$(InstallDirectory)\Processing\', 'Processing installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'ComDirectory', '$(InstallDirectory)\COM\', 'COM installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'UtilitiesDirectory', '$(InstallDirectory)\Utilities\', 'Utilities installation directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'LogDirectory', '$(APPDATA)\Viatronix\Logs', 'Logging directory')  
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'System',	'TempDirectory', 'C:\Temp', 'Temp directory')  


 -- ==============
 -- Misc settings
 -- ==============
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Pacs', 'PendingPacsJobsEvent',  'vxPacsJobsPending', 'Pacs Jobs pending named event' ) 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Locking','LockRefreshRate', '1', 'Refresh rate use in locking' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Debug','StoredProcedureLogFilePath', '', 'Stored porocedure log file path used for debugging' )

 -- ===================
 -- Receiving Settings
 -- ==================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Receiving', 'MissingUidAutoCreate', 'False',  'Flag to determine if the study or series uid is mssing auto create one') 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Receiving', 'ReceivingCompletedEvent',  'vxReceivingComplete', 'Receiving completed named event' ) 

 -- ===================
 -- Storage settings
 -- ===================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Storage', 	'SessionModality', '', 'Default modality for a newly created session, '' will use the dicom''s original') 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Storage', 	'LockTtl', '30', 'Lock time to live in minutes.') 
 
 -- ====================
 -- Processing Settings
 -- ====================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Processing', 'Auto', 'True','Flag indicating whether the processing is automatic or manual' ) 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Processing', 'JobPath', '', 'Job Path' )
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Processing', 'SignalTaskAborted',  'True', 'If true we will signal that task was aborted' ) 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Processing', 'PendingJobsEvent', 'WorkQueuePendingJobs', 'Event to be triggered if there is work queue jobs to be processed' ) 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Processing', 'ConversionTask', '<task name="conversion" exe="$(ProcessorDirectory)Conversion\Conversion.exe" log="4" type="Volume" idx="1" max="360"/>', 'Conversion task' ) 

 -- ====================
 -- Launching settings
 -- ===================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc]  ) VALUES ( 'Launching', 'UIDSeperator', '|', 'Delimter used between the UIDs') 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc]  ) VALUES ( 'Launching', 'SessionSelection', 'LIFO', 'The order the sessions will be selected by') 
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc]  ) VALUES ( 'Launching', 'DicomSelection', 'FIFO', 'The order the dicom series will be selected by') 

 -- ====================
 -- Maintenance settings
 -- ===================
 INSERT INTO [v3d].[SystemSettings] ( subsystem, name, value, [desc]  ) VALUES ( 'Maintenance', 'ErrorMessgeTTL', '-30', 'Number of days the error messages are to remain') 

END
GO

-- ===============================
-- Insert configuration settings
-- ==============================
BEGIN

  INSERT INTO [v3d].[SystemConfigs] ( [name], [desc],  [config] ) VALUES ( 'translucency', 'Translucensy Setting', '<translucency ver="1.0">
    <ThresholdsSetting Name="Fat in Pink" Owner="Service User">
      <Threshold Value="0">
        <Previous Red="0" Green="0" Blue="0" Opacity="0"/>
        <Next Red="0" Green="0" Blue="132" Opacity="0"/>
      </Threshold>
      <Threshold Value="30.97815">
        <Previous Red="0" Green="0" Blue="140" Opacity="0.013"/>
        <Next Red="0" Green="0" Blue="200" Opacity="0.091"/>
      </Threshold>
      <Threshold Value="96.37647">
        <Previous Red="0" Green="0" Blue="206" Opacity="0.091"/>
        <Next Red="255" Green="255" Blue="0" Opacity="0.143"/>
      </Threshold>
      <Threshold Value="99.60336">
        <Previous Red="255" Green="128" Blue="192" Opacity="0.143"/>
        <Next Red="255" Green="128" Blue="192" Opacity="0.156"/>
      </Threshold>
      <Threshold Value="119.8252">
        <Previous Red="255" Green="255" Blue="0" Opacity="0.156"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.162"/>
      </Threshold>
      <Threshold Value="126.279">
        <Previous Red="255" Green="0" Blue="0" Opacity="0.305"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.378"/>
      </Threshold>
      <Threshold Value="154.4605">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
      <Threshold Value="256">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
    </ThresholdsSetting>
    <ThresholdsSetting Name="IV Contrast" Owner="Service User">
      <Threshold Value="0">
        <Previous Red="0" Green="0" Blue="0" Opacity="0"/>
        <Next Red="0" Green="0" Blue="140" Opacity="0"/>
      </Threshold>
      <Threshold Value="31.76043">
        <Previous Red="0" Green="0" Blue="140" Opacity="0.013"/>
        <Next Red="0" Green="0" Blue="255" Opacity="0.091"/>
      </Threshold>
      <Threshold Value="48.46203">
        <Previous Red="0" Green="0" Blue="255" Opacity="0.091"/>
        <Next Red="185" Green="192" Blue="255" Opacity="0.143"/>
      </Threshold>
      <Threshold Value="75.29412">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.143"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.156"/>
      </Threshold>
      <Threshold Value="139.9102">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.156"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.162"/>
      </Threshold>
      <Threshold Value="148.1241">
        <Previous Red="255" Green="0" Blue="0" Opacity="0.305"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.378"/>
      </Threshold>
      <Threshold Value="165.0995">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
      <Threshold Value="256">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
    </ThresholdsSetting>
    <ThresholdsSetting Name="Muscle in Yellow" Owner="Service User">
      <Threshold Value="0">
        <Previous Red="0" Green="0" Blue="0" Opacity="0"/>
        <Next Red="0" Green="0" Blue="140" Opacity="0"/>
      </Threshold>
      <Threshold Value="31.76043">
        <Previous Red="0" Green="0" Blue="140" Opacity="0.013"/>
        <Next Red="255" Green="128" Blue="128" Opacity="0.091"/>
      </Threshold>
      <Threshold Value="106.507">
        <Previous Red="255" Green="128" Blue="128" Opacity="0.091"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.143"/>
      </Threshold>
      <Threshold Value="125.9465">
        <Previous Red="255" Green="0" Blue="0" Opacity="0.143"/>
        <Next Red="255" Green="255" Blue="0" Opacity="0.156"/>
      </Threshold>
      <Threshold Value="134.9818">
        <Previous Red="255" Green="255" Blue="0" Opacity="0.156"/>
        <Next Red="255" Green="0" Blue="0" Opacity="0.162"/>
      </Threshold>
      <Threshold Value="145.9337">
        <Previous Red="255" Green="0" Blue="0" Opacity="0.305"/>
        <Next Red="255" Green="128" Blue="128" Opacity="0.378"/>
      </Threshold>
      <Threshold Value="151.9572">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
      <Threshold Value="256">
        <Previous Red="255" Green="255" Blue="255" Opacity="0.378"/>
        <Next Red="255" Green="255" Blue="255" Opacity="0.622"/>
      </Threshold>
    </ThresholdsSetting>
    <WindowLevelSetting Name="dd" Window="1527" Level="747"/>
  </translucency>' )
END
GO

BEGIN

 INSERT INTO [v3d].[Stores] ( name, local, remote, server, priority, enabled ) 
		VALUES ( 'default','D:\vxStudies','\\localhost\Studies', 'localhost', 1, 1) 

END
GO
