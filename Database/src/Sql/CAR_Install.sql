
-- ==================================================
-- Only install CAR if it was not installed already
-- ==================================================
IF NOT EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'CAR' AND [inst] = 1 )
BEGIN

  -- ================================================================================
  -- If the product entry already exists then update the install flag. Otherwise
  -- insert a product row.
  -- ================================================================================
  IF EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = 'CAR' )
    UPDATE [System].[v3d].[Applications] 
             SET    [inst] =1, 
                    [max] = 1, 
                    [mask] = 1,
                    [types] = 'Volume',
	                  [name] = 'Cardiac'	 
      WHERE [id] = 'CAR'
  ELSE
    INSERT INTO [System].[v3d].[Applications] 
                ( [id], [name], [mask], [types], [max], [inst] ) 
           VALUES ( 'CAR', 'Cardiac', 1, 'Volume', 2, 1 )       
END