
-- ==================================================
-- Only install VAS if it was not installed already
-- ==================================================
IF NOT EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'VAS' AND [inst] = 1 )
BEGIN

  -- ================================================================================
  -- If the product entry already exists then update the install flag. Otherwise
  -- insert a product row.
  -- ================================================================================
  IF EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'VAS' )
    UPDATE [System].[v3d].[Applications] 
             SET    [inst] =1, 
                    [mask] = 1,
	                  [name] = 'Vascular'	 
      WHERE [id] = 'VAS'
  ELSE
    INSERT INTO [System].[v3d].[Applications] 
                ( [id], [name], [mask],  [inst] ) 
           VALUES ( 'VAS', 'Vascular', 1, 1 )       

  INSERT INTO [System].[v3d].Templates ( [id], [app], [mod], [desc],  [default], [tasks] ) 
      VALUES ( 'vas', 'VAS', '', 'Vascular preprocessing pipeline',  1, 
      '<task name="pipeline" exe="$(ProcessorDirectory)VAS\vasPipeline.exe" log="4" type="Preprocessed" idx="2" max="600" >
        </task>' )
END