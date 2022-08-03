
-- ==================================================
-- Only install XP if it was not installed already
-- ==================================================
IF NOT EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'XP' AND [inst] = 1 )
BEGIN

  -- ================================================================================
  -- If the product entry already exists then update the install flag. Otherwise
  -- insert a product row.
  -- ================================================================================
  IF EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'XP' )
    UPDATE [System].[v3d].[Applications] 
             SET    [inst] =1, 
                    [mask] = 1,
	                  [name] = 'Explorer'	 
      WHERE [id] = 'XP'
  ELSE
    INSERT INTO [System].[v3d].[Applications] 
                ( [id], [name], [mask], [inst] ) 
           VALUES ( 'XP', 'Explorer', 1,  1 )       
END