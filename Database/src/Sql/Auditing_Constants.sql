USE [Auditing]
GO


BEGIN


  DELETE FROM [v3d].[Versions]

END
GO



-- =================================================================
-- Insert the object versions
-- =================================================================
BEGIN
  
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  VALUES ( 'Auditing', 'database', '3.6' )

  -- Insert all tables (Except ErrorMessages which is shared amonst all databases)
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  SELECT [name], 'table', '3.6' FROM [sys].[tables] 

END
GO







