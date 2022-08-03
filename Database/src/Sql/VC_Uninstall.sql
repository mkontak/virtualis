DECLARE @CAD AS INT

SELECT @CAD = COUNT(*) FROM [System].[v3d].[Templates] WHERE [app] = 'VAS' AND [cad] <> ''

IF @CAD > 0 
BEGIN
   RAISERROR('Cannot uninstall VAS, must uninstall CAD products first', 16, 1 ) WITH SETERROR
END
ELSE
BEGIN

  -- Delete the VAS processing templates
  DELETE FROM [System].[v3d].[Templates] WHERE [app] = 'VAS'
  
  -- Update the VAS product entries installed bit
  UPDATE [System].[v3d].[Applications] SET [inst] = 0 WHERE [id] = 'VAS'

    
END