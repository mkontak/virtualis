
-- ==================================================
-- Only install VC if it was not instaled already
-- ==================================================
IF NOT EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'VC' AND [inst] = 1 )
BEGIN

  -- ================================================================================
  -- If the product entry already exists then update the install flag. Otherwise
  -- insert a product row.
  -- ================================================================================
  IF EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'VC' )
    UPDATE [System].[v3d].[Applications] 
             SET    [inst] =1, 
                    [mask] = 1,
	                  [name] = 'Virtual Colonoscopy'	 
      WHERE [id] = 'VC'
  ELSE
    INSERT INTO [System].[v3d].[Applications] 
                ( [id], [name], [mask],  [inst] ) 
           VALUES ( 'VC', 'Virtual Colonoscopy', 1,  1 )       

  INSERT INTO [System].[v3d].Templates ( [id], [app], [mod], [desc],  [default], [tasks] ) 
      VALUES ( 'vc-ec', 'VC', '', 'VC With Electronic Cleansing',  1, 
      '<task name="pipeline" exe="$(ProcessorDirectory)VC\vcPipeline.exe" log="4" type="Preprocessed" idx="2" max="600" >
          <params>
              <param name="ElectronicCleansing" value="Yes" cmd="-stage all" />
          </params>
        </task>' )

  INSERT INTO [System].[v3d].Templates ( [id], [app], [mod], [desc],  [default], [tasks] ) 
      VALUES ( 'vc-noec', 'VC', '', 'VC Without Electronic Cleansing',  0, 
               '<task name="pipeline" exe="$(ProcessorDirectory)VC\vcPipeline.exe" log="4" type="Preprocessed" idx="2" max="600" >
                  <params>
                    <param name="ElectronicCleansing" value="No" cmd="-stage allnoec" />
                  </params>
                </task>' )

END