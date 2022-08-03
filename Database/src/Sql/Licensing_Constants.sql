USE [Licensing]
GO


BEGIN

 DELETE FROM [v3d].[LicenseStates]
 DELETE FROM [v3d].[Versions]
 DELETE FROM [v3d].[LicenseSettings]

END
GO


-- =================================================================
-- Insert the object versions
-- =================================================================
BEGIN
  
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  VALUES ( 'Licensing',   'database', '4.0' )

  -- Insert all tables
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  SELECT [name], 'table', '4.0' FROM sys.tables
  

END
GO




-- =============================
-- Insert License states
-- ============================
BEGIN
  INSERT INTO [v3d].[LicenseStates] ( [id], [value], [desc] )  VALUES ( 'available', 1, 'License is available' )
  INSERT INTO [v3d].[LicenseStates] ( [id], [value], [desc] )  VALUES ( 'inuse',     2, 'License is in use' )
  INSERT INTO [v3d].[LicenseStates] ( [id], [value], [desc] )  VALUES ( 'disabled',  4, 'License is disabled' )
  INSERT INTO [v3d].[LicenseStates] ( [id], [value], [desc] )  VALUES ( 'locked',    8, 'License is locked' )
  INSERT INTO [v3d].[LicenseStates] ( [id], [value], [desc] )  VALUES ( 'expired',   16, 'License has expired' )
END
GO





-- =======================================================================================================================
--
-- Insert Licesning settings 
-- 
-- =======================================================================================================================
BEGIN

 -- ==================
 -- Licensing Settings
 -- =================
 INSERT INTO [v3d].[LicenseSettings] ( subsystem, name, value, [desc] ) VALUES ( 'Licensing',	 'LicenseTtlUnits', 'day', 'Licenses time to live units' )

 -- ====================
 -- Maintenance settings
 -- ===================
 INSERT INTO [v3d].[LicenseSettings] ( subsystem, name, value, [desc]  ) VALUES ( 'Maintenance', 'ErrorMessgeTTL', '-30', 'Number of days the error messages are to remain') 

END
GO
