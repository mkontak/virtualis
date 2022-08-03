USE [master]
GO


BEGIN

  -- Delete all viatronix messages
  EXEC [dbo].[sp_deleteSystemMessages]

END
GO




-- ======================
-- Insert the ERROR CODES
-- ======================
BEGIN

 -- =======================================================
  -- Core Error Codes ( 900980-90999 )
  -- =======================================================
  EXEC [dbo].[sp_insertSystemMessage] 900980, '%s : System Disabled'           -- SystemDisabledException
  EXEC [dbo].[sp_insertSystemMessage] 900981, '%s : Not Installed'             -- NotInstalledException
  EXEC [dbo].[sp_insertSystemMessage] 900982, '%s : Not Registered'            -- NotRegisteredException
  EXEC [dbo].[sp_insertSystemMessage] 900983, '%s : Processing'                -- ProcessingException
  EXEC [dbo].[sp_insertSystemMessage] 900984, '%s : Application lock failed'   -- ApplicationLockException  
  EXEC [dbo].[sp_insertSystemMessage] 900990, '%s : Missing or invalid'        -- InvalidDataException
  EXEC [dbo].[sp_insertSystemMessage] 900991, '%s %s: Not Found '              -- NotFoundException
  EXEC [dbo].[sp_insertSystemMessage] 900992, '%s : Already exists'            -- AlreadyExistsException
  EXEC [dbo].[sp_insertSystemMessage] 900993, '%s : Expired'                   -- ExpiredException
  EXEC [dbo].[sp_insertSystemMessage] 900994, '%s : Locked'                   -- LockedException
  EXEC [dbo].[sp_insertSystemMessage] 900995, '%s : Unsupported'               -- UnsupportedException
  EXEC [dbo].[sp_insertSystemMessage] 900996, '%s : No Default'                -- NoDefaultException
  EXEC [dbo].[sp_insertSystemMessage] 900997, '%s : Disabled'                  -- DisabledException
  EXEC [dbo].[sp_insertSystemMessage] 900998, 'System Error'                   -- SystemException


  -- =======================================================
  -- Licensing Error Codes  ( 901000 - 901100 )
  -- =======================================================
  EXEC [dbo].[sp_insertSystemMessage] 901000, 'Cannot register V3D licensing, it is in use'
  EXEC [dbo].[sp_insertSystemMessage] 901001, 'No available license'
  EXEC [dbo].[sp_insertSystemMessage] 901002, 'license is in use'
  EXEC [dbo].[sp_insertSystemMessage] 901003, 'Server is already registered'
  EXEC [dbo].[sp_insertSystemMessage] 901004, 'Licenses already added'


  -- =============================================
  -- Security Error Code ( 901200 - 901300 )
  -- ============================================
  EXEC [dbo].[sp_insertSystemMessage] 901200, 'Login failed'
  EXEC [dbo].[sp_insertSystemMessage] 901201, 'User does not have the proper authorization'
  EXEC [dbo].[sp_insertSystemMessage] 901202, 'User already has a context'
  EXEC [dbo].[sp_insertSystemMessage] 901203, 'Password Invalid : %s' 
  EXEC [dbo].[sp_insertSystemMessage] 901204, 'Cannot update a system account'
  EXEC [dbo].[sp_insertSystemMessage] 901205, 'Cannot delete self.'
  EXEC [dbo].[sp_insertSystemMessage] 901206, 'Cannot delete system account.'
  EXEC [dbo].[sp_insertSystemMessage] 901207, 'Account is restricted'
  EXEC [dbo].[sp_insertSystemMessage] 901208, 'Invalid login hours'
  EXEC [dbo].[sp_insertSystemMessage] 901209, 'Cannot log into current workstation'
  EXEC [dbo].[sp_insertSystemMessage] 901211, 'Login type not granted'
  EXEC [dbo].[sp_insertSystemMessage] 901212, 'Invalid identity'
 
  EXEC [dbo].[sp_insertSystemMessage] 901213, 'Password must differ previous password by %d characters'
  EXEC [dbo].[sp_insertSystemMessage] 901214, 'Login cannot be enbeded in password'

  EXEC [dbo].[sp_insertSystemMessage] 901217, 'Spaces not allowed in password'
  EXEC [dbo].[sp_insertSystemMessage] 901221, 'Password cannot be reused.'
  EXEC [dbo].[sp_insertSystemMessage] 901222, 'Cannot change password again, until %d day(s) has past'
  EXEC [dbo].[sp_insertSystemMessage] 901223, 'Password change is required'

  EXEC [dbo].[sp_insertSystemMessage] 901225, '%s: Minumum of %s was not met' 
  EXEC [dbo].[sp_insertSystemMessage] 901226, '%s: Maximum of %s was exceeded' 


  -- ====================================================
  -- Storage Error Messages ( 901400 - 901500 )
  -- =====================================================
  EXEC [dbo].[sp_insertSystemMessage] 901400, '%s : Missing or invalid dicom field.'
  EXEC [dbo].[sp_insertSystemMessage] 901402, '%s : Failed to serialize xml'
  EXEC [dbo].[sp_insertSystemMessage] 901403, 'Deletion of directory failed'
  EXEC [dbo].[sp_insertSystemMessage] 901404, 'Deletion of file failed'
  EXEC [dbo].[sp_insertSystemMessage] 901405, 'Series contains references.'
  EXEC [dbo].[sp_insertSystemMessage] 901406, 'Series contains dependents.'
  EXEC [dbo].[sp_insertSystemMessage] 901407, 'Series is processing.'
  EXEC [dbo].[sp_insertSystemMessage] 901408, 'Series contains no images'
  EXEC [dbo].[sp_insertSystemMessage] 901430, 'No valid launchable data was found'
  EXEC [dbo].[sp_insertSystemMessage] 901431, 'Cannot launch while processing'
  EXEC [dbo].[sp_insertSystemMessage] 901432, 'Cannot launch from the specified type - %s'
  EXEC [dbo].[sp_insertSystemMessage] 901433, '%s : Cannot launch while processing'
  EXEC [dbo].[sp_insertSystemMessage] 901434, '%s : Cannot launch series is in failed state'
  EXEC [dbo].[sp_insertSystemMessage] 901435, 'Cannot luanch multi-series linked to same dicom'
  EXEC [dbo].[sp_insertSystemMessage] 901436, 'Session does not match %s application'
  EXEC [dbo].[sp_insertSystemMessage] 901437, 'Series has not been processed for %s'
  EXEC [dbo].[sp_insertSystemMessage] 901438, 'Cannot specify a %s and %s together'
  EXEC [dbo].[sp_insertSystemMessage] 901439, 'Cannot specify more then %s %s'
 

  -- ===================================================== 
  -- System Error Codes ( 901800 - 901900 )
  -- =====================================================
  EXEC [dbo].[sp_insertSystemMessage] 901800, 'Missing average dataset size setting'
  EXEC [dbo].[sp_insertSystemMessage] 901801, 'Invalid average dataset size'
  EXEC [dbo].[sp_insertSystemMessage] 901802, 'No storage devices defined'
  EXEC [dbo].[sp_insertSystemMessage] 901803, 'Missing local setting for storage device'
  EXEC [dbo].[sp_insertSystemMessage] 901804, 'Out of space'
  EXEC [dbo].[sp_insertSystemMessage] 901805, 'Falied to get disk space'
  EXEC [dbo].[sp_insertSystemMessage] 903017, 'Invalid storage device'


 
END
GO






