USE [Security]
GO


BEGIN



  DELETE FROM [v3d].[PasswordHints]
  DELETE FROM [v3d].[UserPasswords]
  DELETE FROM [v3d].[Users]
  DELETE FROM [v3d].[UserStates]
  DELETE FROM [v3d].[Roles]
  DELETE FROM [v3d].[Rights]
  DELETE FROM [v3d].[Versions]
  DELETE FROM [v3d].[Rights]
  DELETE FROM [v3d].[PasswordTemplates]
  DELETE FROM [v3d].[AuthenticationTypes]

END
GO


-- =================================================================
-- Insert the object versions
-- =================================================================
BEGIN
  
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  VALUES ( 'Security',   'database', '4.0' )

  -- Insert all tables
  INSERT INTO [v3d].[Versions]  ( [object], [type], [version] )  SELECT [name], 'table', '4.0' FROM sys.tables
  

END
GO





-- =============================
-- Insert User states
-- ============================
BEGIN
  INSERT INTO [v3d].[UserStates] ( [id], [value], [desc] )  VALUES ( 'available',     0, 'Account is available for use (Default)' )
  INSERT INTO [v3d].[UserStates] ( [id], [value], [desc] )  VALUES ( 'locked',        1, 'Account is locked' )
  INSERT INTO [v3d].[UserStates] ( [id], [value], [desc] )  VALUES ( 'pwdchg',      256, 'Indicates a password change is required' )
  INSERT INTO [v3d].[UserStates] ( [id], [value], [desc] )  VALUES ( 'expired',       4, 'Account expired' )
  INSERT INTO [v3d].[UserStates] ( [id], [value], [desc] )  VALUES ( 'disabled',      8, 'Account disabled' )
END
GO

-- =======================
-- Insert User Permssions
-- =======================
BEGIN

  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Any',                0,	  'Allows anything' )

  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Add',                1,	  'Allows adding within the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Delete',             2,	  'Allows deletion within the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Modify',             4,	  'Allows modification within the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'View',               8,	  'Allows viewing/reading within the Enterprise System' )


  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Security',           16,	  'Allows access to Security in the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Storage',            32,	  'Allows access to Storage in the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Processing',         64,	  'Allows access to Processing in the Enterprise System' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Auditing',          128,	  'Allows access to Auditing in the Enterprise System' )

  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Finding',           256,	  'Allows access to findings in a V3D Viewer' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Reporting',        1024,	  'Allows access to reporting' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Tranferring',      2048,   'Allows transferring of data out of the Enterprise System' )

  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Administration',   4096,   'Allows administration opertaions' )
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Service',  	      8192,	  'Allows service operations' )
 
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'Launching',  	   16384,	  'Allows launching or viewers' )

  -- 31
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'UserManagement',           (1 | 2| 4 | 8 | 16),	  'Allows user management in the Enterprise System' )

  -- 47
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'StorageManagement',        (1 | 2| 4 | 8 | 32),	  'Allows storage management adding, deleting and modifiy objects' )

  -- 79
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'ProcessingManagement',     (1 | 2| 4 | 8 | 64),	  'Allows processing management adding, deleting and modifiy storage objects' )

  -- 143
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'AuditingManagement',       (1 | 2| 4 | 8 | 128),	  'Allows auditing management adding, deleting and modifiy auditing objects' )

  -- 1032
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'ViewReport',               ( 8 | 1024),	  'Allows viweing or report only' )

  -- 1039
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'ReportingManagement',      (1 | 2| 4 | 8 | 1024),	  'Allows report management adding, deleting, modifying and viewing' )

  -- 271
  INSERT INTO [v3d].[Rights] ( [name], [mask], [desc] )  VALUES ( 'FindingManagement',        (1 | 2| 4 | 8 | 256),	  'Allows finding management adding, deleting, modifying and viweing' )


END
GO


-- =====================
-- Authentication types
-- =====================
BEGIN
  INSERT INTO [v3d].[AuthenticationTypes] ( [id], [desc], [default] ) VALUES ( 'Viatronix', 'Viatronix password authentication', 1 )
  INSERT INTO [v3d].[AuthenticationTypes] ( [id], [desc], [default] ) VALUES ( 'Windows', 'Windows password authentication', 0 )
END

-- ====================
-- Password Templates
-- ====================
BEGIN

  INSERT 
    INTO [v3d].[PasswordTemplates] ( [id], [desc], [default], [template] ) 
  VALUES ( 'dod', 'DOD Password Complexity', 0,
   '<upper>1</upper>
    <lower>1</lower>
    <special>1</special>
    <numeric>1</numeric>
    <repitition>1</repitition>
    <ttl>34</ttl>
    <frequency>24</frequency>
    <reuse>10</reuse>
    <spaces>false</spaces>
    <differ>4</differ>
    <embed>false</embed>
    <max>24</max>
    <min>15</min>')

  INSERT 
    INTO [v3d].[PasswordTemplates] ( [id], [desc], [default], [template] ) 
  VALUES ( 'standard', 'Standard Password Complexity', 1,
   '<upper>0</upper>
    <lower>1</lower>
    <special>1</special>
    <numeric>1</numeric>
    <repitition>1</repitition>
    <ttl>-1</ttl>
    <reuse>1</reuse>
    <frequency>0</frequency>
    <spaces>false</spaces>
    <differ>2</differ>
    <embed>false</embed>
    <max>16</max>
    <min>8</min>')

   INSERT 
    INTO [v3d].[PasswordTemplates] ( [id], [desc], [default], [template] ) 
  VALUES ( 'hippa', 'HIPPA Password Complexity', 0,
   '<upper>0</upper>
    <lower>1</lower>
    <special>1</special>
    <numeric>1</numeric>
    <repitition>1</repitition>
    <ttl>30</ttl>
    <frequency>24</frequency>
    <reuse>5</reuse>
    <spaces>false</spaces>
    <differ>4</differ>
    <embed>false</embed>
    <max>16</max>
    <min>8</min>')
END

-- ====================
-- Password Hints
-- ====================
BEGIN

  INSERT INTO [v3d].[PasswordHints] ( [name], [hint] ) VALUES ( 'length',  'Password must be between @min and @max characters.' )
  INSERT INTO [v3d].[PasswordHints] ( [name], [hint] ) VALUES ( 'special', 'The password must contain at least @special special character(s) (!,@,#,$,%,...).' )
  INSERT INTO [v3d].[PasswordHints] ( [name], [hint] ) VALUES ( 'numeric', 'The password must contain at least @numeric numeric character(s).' )
  INSERT INTO [v3d].[PasswordHints] ( [name], [hint] ) VALUES ( 'login',   'Password must be different then the login name.' )


END

-- ==========================================================================================================
-- Insert default roles
-- ==========================================================================================================
BEGIN
 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Service', 'Service role', ( 1 | 32 | 64	),  ( 8192 | 4096 | 31 | 47 | 79 | 143 | 1039 | 271 | 16384 ) )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Administrator', 'Administrator role', ( 1	| 32 | 64), ( 4096 | 31 | 47 | 79 | 2048 )  )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Operator', 'Operator role', ( 1	| 32 | 64), ( 47 | 79 )  )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Physician', 'Physician Role', ( 1	| 32 | 64), ( 47 | 79 | 1039 | 2048 | 271 | 16384) )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Phy-Admin', 'Physician/Administrator role', ( 1| 32 | 64	), ( 4096 | 31 | 47 | 79 | 1039 | 271 | 2048 | 16384 ) )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Op-Admin', 'Operator/Administrator role', ( 1	| 32 | 64), ( 4096 | 31 | 47 | 79 | 2048 ) )

 INSERT INTO [v3d].[Roles] ([name], [desc], [set], [prms] ) 
      VALUES ( 'Auditor', 'Auditor role', ( 1 | 16	| 32 | 64 ), ( 143 ) )
END
GO



-- ==============================================================================================================
-- Insert the standard users
--
-- vxservice 		   Is a regular user that will consume a license type of 'user'
-- administrator	 Is a regular user that will consume a license type of 'user'	
-- auditor         The only user allowed into the audting system. 
-- ==============================================================================================================
BEGIN
  INSERT INTO [v3d].Users ( [login], [name], [role], [apps], [set], [auth] ) 
      VALUES ( 'vxservice', 'Viatronix Service', 'Service', 2147483647, ( 1 | 2 | 16 | 32 | 64), 'Windows')

  INSERT INTO [v3d].Users ( [login], [name], [role], [apps], [set], [auth] ) 
      VALUES ( 'administrator', 'Administrator', 'Service', 0, ( 1 | 2 | 16 | 32 | 64), 'Windows')

  INSERT INTO [v3d].Users ( [login], [name], [role], [apps], [set], [auth], [state] )
       VALUES ( 'auditor', 'Auditor', 'Auditor', 0, ( 1 | 2 | 16 | 32 | 64 ), 'Viatronix', [v3d].[fn_getState]('pwdchg') )

  INSERT INTO [v3d].[UserPasswords] ( [login], [pwd], [exp] ) VALUES ( 'auditor', 'ONpue8z8AUY9WpX1taZUHkK4AfhfOypo0/8JHsngUak=', DATEADD(year, 1000, SYSDATETIMEOFFSET()) )
END
GO

