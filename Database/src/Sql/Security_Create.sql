USE [Security]
GO
/****** Object:  User [Apollo\mkontak]    Script Date: 3/13/2014 8:09:33 AM ******/
CREATE USER [Apollo\mkontak] FOR LOGIN [APOLLO\mkontak] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  User [v3dsa]    Script Date: 3/13/2014 8:09:33 AM ******/
CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  User [v3duser]    Script Date: 3/13/2014 8:09:33 AM ******/
CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  Schema [v3d]    Script Date: 3/13/2014 8:09:33 AM ******/
CREATE SCHEMA [v3d]
GO
GRANT DELETE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_acquireContext]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================================================================
-- Author:		Mark Kontak
-- Create date: 5/30/2008
-- Description:	Acquires a context for the specified user. The license type should be left as NULL the deafult
--              so that the users license type can be used. This TYPE is available in case a specific licenese 
--              is required, such as for testing.
--
--              Exceptions:
--  
--              901230        Unknown context
--              901201        User already has a context
--
-- ==============================================================================================================
CREATE PROCEDURE [v3d].[sp_acquireContext] 
( 
@LOGIN AS NVARCHAR(16) = '',                -- User/Login acquiring the context
@PASSWORD AS NVARCHAR(32) = '',             -- Users password
@NEW_PASSWORD AS NVARCHAR(32) = '',         -- New password if a change is occuring
@CONTEXT_ID AS NVARCHAR(256) = '' OUTPUT,   -- Context ID (Id set then the ID will be renewed, login/pwd will be ignored )
@TTL AS INTEGER = 0                         -- Time to live (Set the default to 0 meanung we use the system default)
)
AS
BEGIN

  -- SET NOCOUNT ON added to prevent extra result sets from
  -- interfering with SELECT statements.
  SET NOCOUNT ON;


  DECLARE @ID AS UNIQUEIDENTIFIER
  DECLARE @LT AS NVARCHAR(16)
  DECLARE @SYS AS BIT
  DECLARE @Table AS TABLE ( ID UNIQUEIDENTIFIER )
  DECLARE @IS_SERVICE_ACCOUNT AS BIT
  DECLARE @IS_INTERNAL_ACCOUNT AS BIT
  DECLARE @RC AS INT
  
  
  SELECT  @RC = 0,
          @CONTEXT_ID = ISNULL(@CONTEXT_ID,''),
          @LOGIN = ISNULL(@LOGIN,''),
          @PASSWORD = ISNULL(@PASSWORD,''),
          @NEW_PASSWORD = ISNULL(@NEW_PASSWORD, '')


  DECLARE @ROWCOUNT AS INTEGER

  BEGIN TRY
   
    -- =====================================================================================
    -- Check the system to make sure that it hase not been disabled or needs to be disabled
    -- =====================================================================================
    EXEC  [System].[v3d].[sp_checksystem]   
 
    -- ========================================================================================
    -- If the CONTEXT ID and Login were supplied then attempt to increment the reference count
    -- ========================================================================================
    IF LEN(@CONTEXT_ID) > 0 AND  LEN(@LOGIN) > 0
    BEGIN
            
      UPDATE [v3d].[Contexts] SET [ref] = [ref] + 1 WHERE [id] = @CONTEXT_ID AND [login] = @LOGIN 

      -- ===============================================================================
      -- If nothing was updated then we have an unknown context (900991) Context not found
      -- ===============================================================================
      IF @@ROWCOUNT = 0   
        EXEC [master].[dbo].[sp_throwNotFoundException] 'Context'
 
      RETURN @@ROWCOUNT
     
    END
    
    -- ======================================================================================
    -- Else we check the LOGIN/PASSWORD
    -- ======================================================================================
    ELSE
    BEGIN      
    
      SET @LOGIN = RTRIM(@LOGIN)
      

      -- =====================
      -- Decrypt the password
      -- =====================
      EXEC [v3d].[sp_decryptPassword]
        @ENCRYPTED = @PASSWORD,
        @DECRYPTED = @PASSWORD OUT
    
      -- Check for any contexts that have gone stale and marks them stale
      EXEC [v3d].[sp_cleanupExpiredContexts]

      -- =====================================================================================================
      -- If a new password is supplied then issue a password change which performs an authorization of the 
      -- login/old_password and sets the login/new_password. 
      -- ====================================================================================================
      IF @NEW_PASSWORD IS NOT NULL AND LEN(@NEW_PASSWORD) > 0
      BEGIN

        
        -- =====================
        -- Decrypt the password
        -- =====================
        EXEC [v3d].[sp_decryptPassword]
          @ENCRYPTED = @NEW_PASSWORD,
          @DECRYPTED = @NEW_PASSWORD OUT,
          @TAG = 'New Password'

        -- ======================
        -- Change the password
        -- ======================
        EXEC [v3d].[sp_changePassword] 
          @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893', 
          @LOGIN = @LOGIN, 
          @OLD_PASSWORD = @PASSWORD, 
          @NEW_PASSWORD = @NEW_PASSWORD
      END
      ELSE
      BEGIN

        -- =======================
        -- Authenticate the user
        -- ======================
        EXEC [v3d].[sp_authenticateUser] 
          @LOGIN = @LOGIN,
          @PASSWORD = @PASSWORD
        
      END
      
      -- Check if it is an internal account
      SELECT  @IS_INTERNAL_ACCOUNT = [v3d].[fn_isInternalAccount](@LOGIN),
              @IS_SERVICE_ACCOUNT  = [v3d].[fn_isServiceAccount](@LOGIN)
      
      -- =========================================================================================================
      -- Determine if the use already has a context (901202). Only checked if we are not an internal account
      -- ========================================================================================================
      IF @IS_INTERNAL_ACCOUNT = 0 AND ( SELECT COUNT(*) FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[login] = @LOGIN ) > 0 
        EXEC [master].[dbo].[sp_throwException]  901202;

      -- ===================================================
      -- If the TTL is not passed in then use the default
      -- ===================================================
      IF @TTL = 0
        SET @TTL = CONVERT(INT,[v3d].[fn_getSetting]('ContextTtl', '0'))

      
      -- ============================================================================================
      -- If the account is an internal account ( sys=1 and lt='*' ) it will create the context only
      -- once the first time. All other acquisitions for this user will have the same context. These
      -- are internal accounts that cannot be changed or modified due to the system bit being set.
      -- ===========================================================================================
      IF @IS_INTERNAL_ACCOUNT = 1
	    BEGIN
	      INSERT INTO @Table SELECT [id] FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[login] = @LOGIN 
	      IF @@ROWCOUNT = 0
	        INSERT INTO [v3d].[Contexts] ( [login], [ttl] ) OUTPUT INSERTED.id INTO @Table VALUES ( @LOGIN, 2147483647 )	 
	      ELSE
	        UPDATE [v3d].[Contexts] SET [ref] = [ref] + 1 WHERE [v3d].[Contexts].[login] = @LOGIN 
	    END
    	
	    -- ====================================================================================================
	    -- If the account is a service account then create the context with no license. This is used for all 
	    -- system accounts such as vxservice. They are only allowed one login and adher to the rules of our
	    -- normal user contexts. No license requirement allows the account to access the system for maintenence.
	    -- =====================================================================================================
	    ELSE IF @IS_SERVICE_ACCOUNT = 1  
	    BEGIN
 	      INSERT INTO [v3d].[Contexts] ( [login], [ttl] ) OUTPUT INSERTED.id INTO @Table VALUES ( @LOGIN, @TTL )	    	
	    END
    	
	    -- =========================================================================================================
	    -- If account does not fall into the top two categories then it is a regular user account and will consume
	    -- a license and create a context.
	    -- ==========================================================================================================
	    ELSE
      BEGIN
      
        
        DECLARE @SYSID AS NVARCHAR(255)
        DECLARE @LICENSE AS NVARCHAR(256)

        -- Get the system id
        SELECT @SYSID = [sysid] FROM [System].[v3d].[Install] 
       
        -- Get the next available license
        EXEC [v3d].[sp_acquireLicense]
          @SYSID   = @SYSID,
          @LICENSE = @LICENSE OUTPUT
       	  
              
        -- Inesrt a new context in the database
        INSERT INTO [v3d].[Contexts] ( [login], [lic], [ttl] ) OUTPUT INSERTED.id INTO @Table VALUES ( @LOGIN,  @LICENSE, @TTL )	   
         
        
	    END
    	
    	
      --- Get the currently inserted CONTEXT id
      SELECT @ID = ID FROM @Table
	    
	    
      SET @CONTEXT_ID = CONVERT(NVARCHAR(256), @ID)
    	
      -- Add audit entry for successful action
      EXEC [Auditing].[v3d].[sp_insertUserHistory]  
       @LOGIN = @LOGIN, 
       @ACTION = N'login', 
       @STATUS = 1

      RETURN 1
   		        
    END -- ELSE CONTEXT ID was not supplied
      	    



	END TRY
	BEGIN CATCH


    DECLARE @ERROR_MESSAGE AS NVARCHAR(2048)

    SELECT @ERROR_MESSAGE = ERROR_MESSAGE()

 
     -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'login', 
     @STATUS = 0,
     @MESSAGE =  @ERROR_MESSAGE

 
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_acquireContext'
	  
	END CATCH

    
END





GO
GRANT EXECUTE ON [v3d].[sp_acquireContext] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_authenticate]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/16/2011
-- Description: Authenticates the information passed in.
--              Context ID only then the context is checked to make sure it it in the database
--              and not expired and the license/user are also checked.
-- =============================================================================================
CREATE PROCEDURE [v3d].[sp_authenticate]
@ID NVARCHAR(256) = '',     -- Context Id
@LOGIN NVARCHAR(32) = '',   -- User Login
@PASSWORD NVARCHAR(32)      -- User Password
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
  BEGIN TRY
 
    -- ===============================================================================
    -- Password must be supplied. (900990) Password: Missing or Invalid
    -- ===============================================================================
    IF @PASSWORD IS NULL OR LEN(@PASSWORD) = 0 
 	     EXEC [master].[dbo].[sp_throwInvalidDataException] 'Password';


    -- ======================
    -- Decrypt the password
    -- =====================
    EXEC [v3d].[sp_decryptPassword]
      @ENCRYPTED = @PASSWORD,
      @DECRYPTED = @PASSWORD OUTPUT
     


    -- ================================================================================
    -- If the CONTEXT id and the Users Login were supplied then make sure the context
    -- exists with the specified login
    -- ===============================================================================
    IF @ID IS NOT NULL AND LEN(@ID) > 0 
    BEGIN

      DECLARE @LIC AS NVARCHAR(256)
      DECLARE @ROLE AS NVARCHAR(32)
      DECLARE @CONTEXT_LOGIN AS NVARCHAR(32)
      DECLARE @EXP AS DATETIMEOFFSET(7)
      
      SELECT  @EXP = NULL,
              @ROLE = '',
              @CONTEXT_LOGIN = ''

      IF LEN(@LOGIN) > 0 
        SELECT @ROLE = LOWER([role]) FROM [v3d].[Users] WHERE [login] = @LOGIN
 
      SELECT @LIC = [lic], @CONTEXT_LOGIN = [login] FROM [v3d].[Contexts] WHERE [id] = @ID

      -- ==============================================================
      -- Context not found. (900991) Context not found
      -- ===============================================================
      IF @@ROWCOUNT = 0
         EXEC [master].[dbo].[sp_throwNotFoundException] 'Context';

      -- ====================================================================================
      -- Invalid login/user, does not match the context. (901203) Missing or invalid login
      -- ====================================================================================
      IF @LOGIN IS NOT NULL AND LEN(@LOGIN) > 0 
      BEGIN

        -- =============================================
        -- Only check the user if they are not service
        -- =============================================
        IF @ROLE <> 'service' AND @CONTEXT_LOGIN <> @LOGIN 
         EXEC [master].[dbo].[sp_throwInvalidDataException]  'Login';

      END
      ELSE
        SET @LOGIN = @CONTEXT_LOGIN
          


      -- ==========================================================================================
      -- Only check the license if the account associated with the context is an internal account
      -- 
      -- Internal accounts do not consume a license
      -- ========================================================================================
      IF  [v3d].[fn_isInternalAccount](@CONTEXT_LOGIN) = 0 AND [v3d].[fn_isServiceAccount](@CONTEXT_LOGIN) = 0 
      BEGIN
    
 
        SELECT @EXP = [exp] FROM [v3d].[Licenses] WHERE [key] = @LIC

        -- ============================
        -- Invalid license exception
        -- (900991) License: Not FOund
        -- ============================
        IF @@ROWCOUNT = 0 
          EXEC [master].[dbo].[sp_throwNotFoundException] 'License';

      END

   
      EXEC [v3d].[sp_authenticateUser]
          @LOGIN = @LOGIN,
          @PASSWORD = @PASSWORD
      
      -- ================================================================================================================
      -- Licesne expired exception. This is checked last so that we guearatee the user was authorized and not expired
      -- so that an application can make this assumtion. (900993) License expired
      -- ================================================================================================================
      IF @EXP IS NOT NULL AND @EXP < SYSDATETIMEOFFSET() 
         EXEC [master].[dbo].[sp_throwException] 900993, 'License';

    END
    
    -- =============================================================
    -- Else if the Login/Password was supplied and not the context
    -- =============================================================
    ELSE IF @LOGIN IS NOT NULL AND LEN(@LOGIN) > 0 
    BEGIN
    
      -- =========================================================================================
      -- Just authentciate the user. If the user/Password is incorrect then and exception will
      -- be thrown and cought below.
      -- =========================================================================================
      EXEC [v3d].[sp_authenticateUser]
        @LOGIN    = @LOGIN,
        @PASSWORD = @PASSWORD
          
    END
    ELSE
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Parameters';


 	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_authenticate'
	  
	END CATCH
  
END







GO
GRANT EXECUTE ON [v3d].[sp_authenticate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_authenticate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_authenticateUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================================================================
--
-- Author:		  Mark Kontak
--
-- Create date: 1/25/2010
--
-- Description:	Authenticates the user using checking the password specified. We do not check for a missing or invalid
--              login or password since that should have been done prior to calling this procedure.
--              
--              Exceptions:
--
--                  900991      Account not found   
--                  901232      Account is locked
--                  901204      Password change is required 
--                  901206      Password expired
--                  901236      Account expired
--                  
--                  Windows Authentication
--
--                  901200    Login failed
--                  901207    Account restricted
--                  901208    Invalid login hours
--                  901209    Cannot log into current workstation
--                  901210    Account disabled
--                  901211    Login type not granted 
--
-- ==============================================================================================================================
CREATE PROCEDURE [v3d].[sp_authenticateUser]
(
@LOGIN    NVARCHAR(16),       -- User
@PASSWORD NVARCHAR(256)       -- Password (Plain Text)
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- User state variable
  DECLARE @STATE AS INTEGER
  DECLARE @AUTH AS NVARCHAR(32)
  DECLARE @DOMAIN AS NVARCHAR(64)
  DECLARE @USER_EXPIRATION AS DATETIMEOFFSET
  DECLARE @ERROR_MESSAGE NVARCHAR(4000);
  DECLARE @ERROR_NUMBER INT
  DECLARE @ERROR_LINE INT
  DECLARE @ERROR_SEVERITY INT
  DECLARE @ERROR_STATE INT
  DECLARE @ERROR_PROCEDURE NVARCHAR(128)


  BEGIN TRY
  
    -- ==============================================
    -- Make sure the password is not NULL or blank
    -- (900990) Password: Missing or Invalid
    -- ===============================================
    IF @PASSWORD IS NULL OR LEN(@PASSWORD) = 0
         EXEC [master].[dbo].[sp_throwInvalidDataException] 'Password'

		-- Get the current users state and password ttl
	  SELECT @STATE = [state],  @AUTH = [auth], @DOMAIN = [dom], @USER_EXPIRATION = [exp]  FROM [v3d].[Users] WHERE [login] = @LOGIN

	  --===================================================================
	  -- If the user is not in the V3D system. (900991) Account: Not Found
	  --===================================================================
	  IF @@ROWCOUNT = 0 
          EXEC [master].[dbo].[sp_throwNotFoundException]  'Account'


	  -- =============================================================
	  -- If the USER is locked. 
    -- (900994) Account: Locked
	  -- =============================================================
	  IF [v3d].[fn_isState](@STATE,'locked') = 1
          EXEC [master].[dbo].[sp_throwException] 900994, 'Account'

	  -- =============================================================
	  -- If the USER is disabled. 
    -- (900994) Account: Disabled
	  -- =============================================================
	  IF [v3d].[fn_isState](@STATE,'disabled') = 1
	         EXEC [master].[dbo].[sp_throwException] 900997, 'Account'


    --===================================================================================
    -- If the authentication type is Viatronix continue othewise we need to execute the 
    -- Windows athentication stored procedure.
    --===================================================================================
    IF @AUTH = 'Viatronix'
    BEGIN
    

	    --=======================================================================================================
	    -- If the USER needs to change their password raise an exception. (901223) Password change is required 
	    --=======================================================================================================
	    IF [v3d].[fn_isState](@STATE, 'pwdchg') = 1
	         EXEC [master].[dbo].[sp_throwException] 901223


      DECLARE @LAST_PASSWORD AS NVARCHAR(32)
      DECLARE @ENCRYPTED_PASSWORD NVARCHAR(32)
      DECLARE @PASSWORD_EXPIRATION DATETIMEOFFSET 
      
      -- ===================================================
      -- Get the last password in the users password list
      -- ===================================================
      SELECT TOP(1) @LAST_PASSWORD = [pwd], @PASSWORD_EXPIRATION = [exp] FROM [v3d].[UserPasswords] WHERE [login] = @LOGIN ORDER BY [date] DESC
      
      -- ==============================================================================
      -- If the user contains no password then (901223) Pasword change is required
      -- ==============================================================================
      IF @@ROWCOUNT = 0
 	         EXEC [master].[dbo].[sp_throwException] 901223
     
      -- ==========================================================================================
      -- Get the users encrypted passwoord so that we can check if it matches the last password
      -- ==========================================================================================
      EXEC [v3d].[sp_encrypt]
		      @value = @PASSWORD,
		      @encryptedValue = @ENCRYPTED_PASSWORD OUTPUT
    		
	    --===================================================================================================
	    -- Check if the user's specified password matches what we have. (900990) Password: Missing or Invald
      --===================================================================================================
	    IF @LAST_PASSWORD <> @ENCRYPTED_PASSWORD
	         EXEC [master].[dbo].[sp_throwInvalidDataException] 'Password'

      --============================================================================================
      -- If the users password has a (T)ime (T)o (L)ive and the current date has surpassed the 
      -- password expiration date then raise en error indicating the passord has expired. 
      -- (900993) Password expired
      --===========================================================================================
      IF @PASSWORD_EXPIRATION IS NOT NULL AND @PASSWORD_EXPIRATION < SYSDATETIMEOFFSET() 
  	         EXEC [master].[dbo].[sp_throwException] 900993, 'Password'
  

      -- =====================================================================================================================================
      -- Check if the account has expired. This check is last so that the password is validated first. If this excpetion is trapped in an 
      -- application it can be assumed that all other condictions have passed. (900993) Account: Expired
      -- =====================================================================================================================================
      IF @USER_EXPIRATION IS NOT NULL AND @USER_EXPIRATION < SYSDATETIMEOFFSET()
      BEGIN

        -- Set the users state to expired
        UPDATE [v3d].[Users] SET [state] = [state] & [v3d].[fn_getSetting]('expired',0) WHERE [login] = @LOGIN

  	    EXEC [master].[dbo].[sp_throwException] 900993, 'Account'
     END

    END
    ELSE
    BEGIN
    
      BEGIN TRY
        -- ==============================
        -- Perform window authentication
        -- =============================
        EXEC [v3d].sp_windowsAuthentication 
              @DOMAIN = @DOMAIN,
              @LOGIN = @LOGIN,
              @PASSWORD = @PASSWORD
       END TRY
       BEGIN CATCH

         DECLARE @CODE AS NVARCHAR(32)
           
         -- Get the exception information o that we can rethrow
         SELECT @ERROR_MESSAGE     = ERROR_MESSAGE(),
                @ERROR_NUMBER      = ERROR_NUMBER()
   
          EXEC [v3d].[sp_errorHandler]
            @PROCEDURE = 'sp_authenticateUser',
            @RETHROW = 0

     
		      DECLARE @START AS INT
		  
		      SET @START = PATINDEX('%90%', @ERROR_MESSAGE )

		      IF @START > 0
		      BEGIN
			      SET @CODE = SUBSTRING(@ERROR_MESSAGE,@START,6)

			      IF ISNUMERIC(@CODE) = 1 
			      BEGIN

				      SET @ERROR_NUMBER = CONVERT(INT,@CODE)

				      IF SUBSTRING(@ERROR_MESSAGE,7,1) = '|'
				      BEGIN
				        DECLARE @PARAM AS NVARCHAR(32)

				        SET @PARAM = SUBSTRING(@ERROR_MESSAGE,8, LEN(@ERROR_MESSAGE) - 7)

                EXEC [master].[dbo].[sp_throwException] 
                    @ERROR_NUMBER = @ERROR_NUMBER,
                    @ERROR_MESSAGE = @PARAM

				      END
				      ELSE
                EXEC [master].[dbo].[sp_throwException] @ERROR_NUMBER


			      END
			      ELSE
              EXEC [master].[dbo].[sp_throwException] @ERROR_NUMBER
         END
		      ELSE
            EXEC [master].[dbo].[sp_throwException] @ERROR_NUMBER
       
          
           
       END CATCH
        
    END
      
    RETURN 1
  
  END TRY
  BEGIN CATCH


    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_authenticateUser'

  END CATCH
  
END







GO
GRANT EXECUTE ON [v3d].[sp_authenticateUser] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_changePassword]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =================================================================================================
-- Author:		  Mark Kontak
-- Create date: 5/30/2008
-- Description:	Attempts to change the users password. First checks for the user in the database
--              then validates the password before inserting a password into the users history.
--              If the encrypted password returned from the validatePassword stored procedure is 
--              blank that means the authentication type was not VIATRONIX and password changing
--              is unsupported
--
--              Exceptions:
--
--                          901201       Unknown user
--                          901239       Password change is unsupported
--
-- ==================================================================================================
CREATE PROCEDURE [v3d].[sp_changePassword] 
(
@ID NVARCHAR(256) = '',           -- Context of user performing the change
@LOGIN nvarchar(16),              -- User to change password on
@OLD_PASSWORD nvarchar(256) = '', -- Old password
@NEW_PASSWORD nvarchar(256)       -- New password
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)
  
  DECLARE @ENCRYPTED_PASSWORD AS NVARCHAR(256)
  DECLARE @STATE AS BIGINT
  DECLARE @PWDCHG AS BIGINT
  DECLARE @TRANSCOUNT AS INT
  
  
  SELECT  @TRANSCOUNT = @@TRANCOUNT,
          @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)  

  BEGIN TRY

    SET XACT_ABORT ON

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION


    -- ============================================================
    -- Check for an invalid context - 
    -- (900990) Login: missing or invalid
    -- ============================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0 
  	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';


    DECLARE @IGNORE_FREQUENCY_SETTING AS BIT


    -- Get the previous state just in case of failure we can reset it back
    SELECT @STATE = [state],
           @IGNORE_FREQUENCY_SETTING = 0,
           @PWDCHG = [v3d].[fn_getState]('pwdchg')  
           FROM [v3d].[Users] WHERE [login] = @LOGIN

    -- =====================================================================================
	  -- If the user is not in the V3D system then raise an error. (900991) Account not found
	  -- =====================================================================================
	  IF @@ROWCOUNT = 0 
 	    EXEC [master].[dbo].[sp_throwNotFoundException] 'Account';
 
    -- Unset the password change flag if it is set
    UPDATE [v3d].[Users] SET [state] = [state] ^ @PWDCHG WHERE [login] = @LOGIN AND ([state] & @PWDCHG ) = @PWDCHG
 
    -- ================================================================================================
    -- Only check the ID if it was specified. This is used for when an adminitrator does a password
    -- change on another user.
    -- ================================================================================================
    IF LEN(@ID) > 0 
    BEGIN 

      -- ===========================================================================
      -- Check if the user owning the context is authorized to change the password
      -- ===========================================================================
      EXEC [v3d].[sp_isAuthorized]
        @ID = @ID,
        @LOGIN = @LOGIN,
        @MASK = 'UserManagement'

        -- Ignore the frequency setting if an administrator is changing the password.
        SET @IGNORE_FREQUENCY_SETTING = 1

    END
    ELSE
    BEGIN

 

      -- =================================================================
      -- Validates the user old password first before makeing any change
      -- =================================================================
      EXEC [v3d].[sp_authenticateUser] 
        @LOGIN = @LOGIN,
        @PASSWORD = @OLD_PASSWORD


    END


    -- ==================================================================================
    -- If the user change password was set then ignore the frequency setting so the user 
    -- is not locked out of the system.
    -- ==================================================================================
    IF (@STATE & @PWDCHG) = @PWDCHG
      SET @IGNORE_FREQUENCY_SETTING = 1
  
 
	  -- ========================================================================
    -- Inserts the users new password if valid
    -- ========================================================================
    EXEC [v3d].[sp_insertUserPassword]
        @LOGIN = @LOGIN, 
        @IGNORE_FREQUENCY = @IGNORE_FREQUENCY_SETTING,
        @PASSWORD = @NEW_PASSWORD

    
    -- Add audit entry for successful action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'chgpwd', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN
     
    IF @TRANSCOUNT = 0 
      COMMIT TRANSACTION
     
    RETURN 1

  END TRY
  BEGIN CATCH


    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION
 

    DECLARE @ERROR_MESSAGE AS NVARCHAR(1024)

    SELECT @ERROR_MESSAGE = ERROR_MESSAGE();

    
    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'chgpwd', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE

     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_changePassword'
   
  END CATCH

    

 

END







GO
GRANT EXECUTE ON [v3d].[sp_changePassword] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_characterCount]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================
-- Author:		  Mark Kontak
-- Create date: 1/29/2010
-- Description:	Counts occurances of the characetsr in @CHARACTERS that appear in @STRING
-- ============================================================================================
CREATE PROCEDURE [v3d].[sp_characterCount]
(
@CHARACTERS AS NVARCHAR(255),        -- Character list
@STRING AS NVARCHAR(32),            -- String to check
@COUNT AS INT OUTPUT                -- Counts of characters in string
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  -- Length of the string
	DECLARE @LENGTH INT
	
	-- Starting index
	DECLARE @START INT
	
	-- Set the length variable
	SELECT  @LENGTH = LEN(@STRING),
	        @START = 1,
	        @COUNT = 0
	
	--============================================================
	-- Loop through the STRING to count the special chracters
	--============================================================
	WHILE @START <= @LENGTH
	BEGIN 
	  IF PATINDEX( '%' + SUBSTRING(@STRING,@START,1) + '%', @CHARACTERS COLLATE SQL_Latin1_General_Cp1_CS_AS ) > 0 
	  BEGIN
	    SET @COUNT = @COUNT + 1
	  END
	  SET @START = @START + 1
	END -- END WHILE
	
	
END





GO
GRANT EXECUTE ON [v3d].[sp_characterCount] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_cleanupExpiredContexts]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================
-- Author:		  Mark Kontak
-- Create date: 5/2/2008
-- Description:	Cleans up any expired contexts
-- ================================================
CREATE PROCEDURE [v3d].[sp_cleanupExpiredContexts]
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

  -- Delete all expired contexts
  DELETE FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[exp] <= SYSDATETIMEOFFSET()
  
  --- Return the number of rows affected
  RETURN @@ROWCOUNT
  
  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_cleanupExpiredContexts'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_cleanupExpiredContexts] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_decrypt]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_decrypt]
	@value [nvarchar](4000),
	@decryptedValue [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Security].[StoredProcedures].[sp_decrypt]
GO
/****** Object:  StoredProcedure [v3d].[sp_decryptPassword]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 12/06/2012
-- Description:	Wraps the password decryption so that I can trap the any exception 
--              from the CLR procedure. Usually if the password come in non-encrypted
-- ====================================================================================
CREATE PROCEDURE [v3d].[sp_decryptPassword]
@ENCRYPTED NVARCHAR(128),         -- Encrypted Password
@DECRYPTED NVARCHAR(128) OUTPUT,  -- Decrypted Password
@TAG NVARCHAR(32) = 'Password'
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- ======================
  -- Decrypt the password
  -- =====================
  BEGIN TRY
      EXEC [v3d].[sp_decrypt]
        @value = @ENCRYPTED,
        @decryptedValue = @DECRYPTED OUTPUT
  END TRY
  BEGIN CATCH

     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_decryptPassword',
      @ERROR_CODE = 900990,
      @P1 = 'Password'

  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_decryptPassword] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_decryptPassword] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteRole]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/24/2009
-- Description:	Returns the contexts table in xml format includes the license and the 
--
--              Exceptions:
--
--              901237    Must specify role name.
--
-- NOTE:  Currently all roles are added during the creation of the Security database as of
--        Console 3.5. The requirement to add roles by an administrator was removed in 
--        Console 3.5. Will keep this procedure in-case the requirement is added back into 
--        the Security Subsystem.
-- ============================================================================================
CREATE PROCEDURE [v3d].[sp_deleteRole]
(
@ID NVARCHAR(256) = '',            -- Context ID
@NAME NVARCHAR(32) = NULL,         -- Role name to be deleted
@RECORD AS XML = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
	BEGIN TRY
	    

    -- ==============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- If we open the delete then the [set] (settings) value needs to check for a bit of 8 being set
    -- ===============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'Service'


    IF @NAME IS NULL AND @RECORD IS NOT NULL
    BEGIN
     DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @NAME          = [name]
            FROM OPENXML(@IDOC, N'/role', 1) WITH 
            ( 
              [name]            nvarchar(16)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END


    -- ========================================================================================================
    -- Make sure that a name was specified or we cannot add the role. (900990) Role name is missing or invalid
    -- ========================================================================================================
    IF @NAME IS NULL OR  LEN(@NAME) = 0 
 	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Role Name';
 

    -- Delete the Role
    DELETE [v3d].[Roles] WHERE [name] = @NAME
    
    RETURN @@ROWCOUNT
  
 	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteRole'
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deleteRole] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================
-- Author:		  Mark Kontak
-- Create date: 1/29/2010
-- Description: Deletes a user from the system. Uses the context to
--              make sure the user performing the removal is authorized.
--
--              Exceptions:
--              
--              901228     Cannot delete self
--              901229     Cannot delete system account
-- =======================================================================
CREATE PROCEDURE [v3d].[sp_deleteUser]
(
@ID NVARCHAR(256) = '',      -- Context ID
@LOGIN NVARCHAR(64) = NULL,  -- User to be deleted
@RECORD XML = NULL,          -- Record
@RELEASE_CONTEXT BIT = 0     -- Flag used to idicate that the users context is to be deleted also
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @ROWCOUNT AS INT
  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

  SET @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)

	BEGIN TRY
	    
	  
	    
	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'UserManagement'

    IF @LOGIN IS NULL AND @RECORD IS NOT NULL
    BEGIN
     DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @LOGIN          = [login]
            FROM OPENXML(@IDOC, N'/user', 1) WITH 
            ( 
              [login]            nvarchar(64)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END




  
    -- =======================================================================================================
    -- Make sure that a login was specified or we cannot add the user. (900990) Login: Missing or invalid
    -- ======================================================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0
   	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';
       
    -- ===============================================
    -- Cannot delete self. (901205) Cannot delete self
    -- ===============================================
    IF @LOGIN = @CONTEXT_LOGIN 
 	    EXEC [master].[dbo].[sp_throwException] 901205;

     -- ==============================================================================================
    -- Cannot update a system account unless you are service 
    -- (901204) Cannot update system account
    -- ==============================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Users] WHERE [login] = @LOGIN AND ( [set] & 1) = 1 )
    BEGIN
      BEGIN TRY
        EXEC [v3d].[sp_isAuthorized]
          @ID = @ID,
          @MASK = 'Service'
      END TRY
      BEGIN CATCH
        EXEC [master].[dbo].[sp_throwException] 901206;
      END CATCH
    END
    
      
    -- ============================================
    -- Releases the user context if the bit is set
    -- ===========================================
    IF @RELEASE_CONTEXT = 1
    BEGIN
      EXEC [v3d].[sp_releaseContext]
        @ID = @ID,
        @LOGIN = @LOGIN,
        @FORCE = 1
    END

    -- Delete the users password history first 
    DELETE [v3d].[UserPasswords] WHERE [v3d].[UserPasswords].login = @LOGIN
 
    -- Delete the user record
    DELETE [v3d].[Users] WHERE [v3d].[Users].login = @LOGIN

    SET @ROWCOUNT = @@ROWCOUNT
    
    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'delete', 
     @STATUS = 1,
     @USER = @CONTEXT_LOGIN

    RETURN @ROWCOUNT

        
	END TRY
	BEGIN CATCH
	
    DECLARE @ERROR_MESSAGE NVARCHAR(4000);

    -- Get the exception information o that we can rethrow
    SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()
 
    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'delete', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteUser'
	  
	END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_deleteUser] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteUserProfile]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 09/28/2011
-- Description: Delete the config setting
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteUserProfile]
@ID NVARCHAR(256),
@LOGIN NVARCHAR(32) = NULL,
@NAME NVARCHAR(16) = NULL,
@RECORD AS XML = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  

  DECLARE @CONFIG AS XML
 

  BEGIN TRY
 
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'UserManagement'
	 
    IF ( @LOGIN IS NULL AND @NAME IS NULL ) AND @RECORD IS NOT NULL
    BEGIN

      DECLARE @IDOC AS INTEGER
 
        -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT  @LOGIN      = [login],
              @NAME       = [name]
            FROM OPENXML(@IDOC, N'/userprofile', 1) WITH 
            ( 
              [login]   nvarchar(16),
              [name]    nvarchar(64)
            )


      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END

    -- ===============================================================================
    -- Make sure the login is specified. (900990) Login is missing or invalid
    -- ===============================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0 
   	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';
 
   -- ===============================================================================
    -- Make sure profile name is specified. (900990) Profile Name  is missing or invalid
    -- ===============================================================================\
    IF @NAME IS NULL OR LEN(@NAME) = 0 
   	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Profile Name'
 

    --- Delete the user config
    DELETE FROM [v3d].[UserProfiles] WHERE [login] = @LOGIN AND [name] = @NAME
  
    --- Returns the rowcount
    RETURN @@ROWCOUNT
  

        
	END TRY
	BEGIN CATCH
	
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteUserProfile'
	  
	END CATCH

  
END






GO
GRANT EXECUTE ON [v3d].[sp_deleteUserProfile] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_encrypt]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_encrypt]
	@value [nvarchar](4000),
	@encryptedValue [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Security].[StoredProcedures].[sp_encrypt]
GO
/****** Object:  StoredProcedure [v3d].[sp_errorHandler]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/08/2013
-- Description:	Error Handler
-- =============================================
CREATE PROCEDURE [v3d].[sp_errorHandler] 
@PROCEDURE NVARCHAR(128),
@ERROR_CODE INT = 0,
@DEADLOCK BIT = 0,
@RETHROW BIT = 1,
@P1 NVARCHAR(128) = '',
@P2 NVARCHAR(128) = ''
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

   DECLARE @DATABASE AS NVARCHAR(64)
   DECLARE @RETURN AS INT

   SELECT @DATABASE = DB_NAME()

   EXEC @RETURN = [master].[dbo].[sp_errorHandler]
      @DATABASE = @DATABASE,
      @PROCEDURE = @PROCEDURE,
      @ERROR_CODE = @ERROR_CODE,
      @P1 = @P1,
      @P2 = @P2,
      @DEADLOCK = @DEADLOCK,
      @RETHROW = @RETHROW
 

  RETURN @RETURN

END



GO
GRANT EXECUTE ON [v3d].[sp_errorHandler] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_errorHandler] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_forcePasswordChange]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================
-- Author:		  Mark Kontak
-- Create date: 04/19/2010
-- Description:	Forces a user to change their password upon the next login
-- =========================================================================
CREATE PROCEDURE [v3d].[sp_forcePasswordChange]
(
@ID AS NVARCHAR(256) = '',  -- Context id of user forcing the password change
@LOGIN NVARCHAR(32)         -- User to be have their password bit set
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

  SET @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)

  BEGIN TRY

    -- Make sure the user forcing a password change is authorized
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @LOGIN = @LOGIN,
      @MASK = 'UserManagement'
      
    -- =======================================================
    -- A User is required. (900990) Login missing or invalid
    -- =======================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0
  	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login'
 
    -- Update the USER state to indicate a password change is necessary
    UPDATE [v3d].Users SET [state] = [state] | [v3d].[fn_getState]('pwdchg') WHERE [login] = @LOGIN 
    
    -- =================================================================================
	  -- If the user is not in the V3D system then raise an error. (900991) Account not found
	  -- =================================================================================
	  IF @@ROWCOUNT = 0 
 	  	EXEC [master].[dbo].[sp_throwNotFoundException] 'Account'
 

  	-- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'frcpwdchg', 
     @STATUS = 1,
     @USER = @CONTEXT_LOGIN
       	
  	-- Return 1
	  RETURN 1
	
	END TRY
	BEGIN CATCH
	
    DECLARE @ERROR_MESSAGE NVARCHAR(4000);
  

    -- Get the exception information o that we can rethrow
    SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()


    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'frcpwdchg', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_forcePasswordChange'

	END CATCH
	
END





GO
GRANT EXECUTE ON [v3d].[sp_forcePasswordChange] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getAuthenticationType]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================================================
-- Author:		  Mark Kontak
-- Create date: 01/18/2013 
-- Description:	Returns the allowable authentication types
--
-- Paramaters:
--              @QUERY      This is a xml query specifying the fields to search by. Each attribute 
--                          corresponds to a column in the user table.
--              @RESULTS    This is the XML result set of the query
-- ==========================================================================================================
CREATE PROCEDURE [v3d].[sp_getAuthenticationType]
(
@ID       NVARCHAR(256),               -- Context Id
@QUERY    XML = '<authenticationtype />',    -- Query xml
@RESULTS  XML OUTPUT                   -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;



  DECLARE @RC AS INT


  BEGIN TRY
  
    -- Make sure the context is valid
    EXEC [v3d].[sp_validateContext]
      @ID = @ID,
      @CHECK_EXPIRATION = 1
      
     
    DECLARE @IDOC AS INT

    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @AUTH_ID        AS NVARCHAR(MAX)
    DECLARE @AUTH_DESC      AS NVARCHAR(256)
    DECLARE @AUTH_DEFAULT   AS BIT

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @AUTH_ID         = [id], 
            @AUTH_DESC       = [desc],
            @AUTH_DEFAULT    = [default]
            FROM OPENXML(@IDOC, N'/authenticationtype', 3) 
            WITH 
            (
              [id]               nvarchar(MAX),
              [desc]             nvarchar(256),
              [default]          bit   
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  
    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SELECT  @WHERE = [System].v3d.fn_buildStringComparison(N'@AUTH_ID', @AUTH_ID, '[id]') + [System].v3d.fn_buildStringComparison(N'@AUTH_DESC', @AUTH_DESC, '[desc]'),
            @SELECT = 'SELECT *, [v3d].[fn_getVersion](''AuthenticationTypes'') AS [ver] FROM [v3d].[AuthenticationTypes] '
        
    IF @AUTH_DEFAULT IS NOT NULL 
      SET @WHERE = @WHERE + ' [default] = @AUTH_DEFAULT AND '
            
   
    DECLARE @LEN AS INT
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''authenticationtype''), TYPE , ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS        XML OUTPUT, ' +
                      N'@AUTH_ID        NVARCHAR(MAX), ' + 
                      N'@AUTH_DESC      NVARCHAR(256), ' +
                      N'@AUTH_DEFAULT   BIT, ' +
                      N'@COUNT          INT OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @AUTH_ID      = @AUTH_ID,
            @AUTH_DESC    = @AUTH_DESC,
            @AUTH_DEFAULT = @AUTH_DEFAULT,
            @COUNT          = @RC OUTPUT,
            @RESULTS        = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="authenticationtype" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"authenticationtype"} into (/collection)[1]')   
 
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getAutenticationType'

  
  END CATCH
  
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getAuthenticationType] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getAuthenticationType] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getContext]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/24/2009
-- Description:	Returns the contexts table in xml format includes the license and the 
--              users associated with the context.
--
--              <collection type="context" >
--                <context ... >
--                  <license ... />
--                  <user ... />
--                  <workspace ... />
--                </context>
--              </collection>
-- ======================================================================================
CREATE PROCEDURE [v3d].[sp_getContext]
(
@ID NVARCHAR(255),                    -- Context ID
@QUERY AS XML = '<context />',        -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
  DECLARE @LOGIN AS NVARCHAR(512)


  BEGIN TRY

	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'Administration'

      
    DECLARE @IDOC AS INT

    DECLARE @KEY AS NVARCHAR(1000)
    DECLARE @CONTEXT_ID AS NVARCHAR(1000)


    
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @CONTEXT_ID       = [id], 
            @LOGIN            = [login],
            @KEY              = [lic]
            FROM OPENXML(@IDOC, N'/context', 3) 
            WITH 
            (
              [id]    nvarchar(1000),
              [login] nvarchar(512),
              [lic]   nvarchar(1000)
            )
     
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  
  
    DECLARE @LEN AS INT
    DECLARE @WHERE AS NVARCHAR(1000)
    DECLARE @SELECT NVARCHAR(1000)
  
    SELECT  @ID = @CONTEXT_ID,
            @WHERE = [System].v3d.fn_buildStringComparison(N'@ID', @CONTEXT_ID, '[v3d].[Contexts].[id]') + [System].v3d.fn_buildStringComparison(N'@LOGIN', @LOGIN, '[v3d].[Contexts].[login]'),
            @SELECT = 'SELECT [v3d].[Contexts].*, [v3d].[fn_getVersion](''Contexts'') AS [ver] FROM [v3d].[Contexts] '
   
  
    SET @LEN = LEN(@WHERE)
  
    IF @LEN > 0
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)


    DECLARE @CONTEXTS AS XML

    SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''context''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
 

    EXEC sp_executesql 
            @SELECT,
            N'@RESULTS XML OUTPUT, @COUNT INT OUTPUT, @LOGIN NVARCHAR(16), @KEY NVARCHAR(255),  @ID NVARCHAR(256)',
            @KEY      = @KEY,
            @ID       = @CONTEXT_ID,
            @LOGIN    = @LOGIN,
            @COUNT    = @RC OUTPUT,
            @RESULTS  = @RESULTS OUTPUT
    
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="context" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"context"} into (/collection)[1]')   

    RETURN @RC
  END TRY
  BEGIN CATCH 
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getContext'
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_getContext] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLicense]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================
-- Author:		  Mark Kontak
-- Create date: 12/0/2012
-- Description:	Gets the licenses from the Licensing Subsytem
-- ============================================================
CREATE PROCEDURE [v3d].[sp_getLicense]
(
@ID NVARCHAR(256),
@QUERY AS XML = '<license />',        -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INTEGER

  BEGIN TRY
  
     
    -- Make sure the context use is authorize
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'Administration'



    DECLARE @REMOTE_RESULTS AS NVARCHAR(2048)
    DECLARE @REMOTE_QUERY AS NVARCHAR(2048)
       
    SET @REMOTE_QUERY = CONVERT(NVARCHAR(2948), @QUERY)
   
    EXEC @RC = [v3d].[sp_getLicenses] 
      @QUERY = @REMOTE_QUERY,
      @RESULTS = @REMOTE_RESULTS OUTPUT

    SET @RESULTS = CONVERT(XML,@REMOTE_RESULTS)

    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getLicense'

  
  END CATCH
END



GO
/****** Object:  StoredProcedure [v3d].[sp_getPasswordHint]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================================
-- Author:		  Mark Kontak
-- Create date: 07/25/2011
-- Description:	Returns the password hints based on the default password template
-- ===============================================================================
CREATE PROCEDURE [v3d].[sp_getPasswordHint]
(
@ID NVARCHAR(256),
@QUERY AS XML = '<passwordhint />',        -- Query xml
@RESULTS AS XML OUTPUT                        -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  

  DECLARE @OPEN AS BIT


  BEGIN TRY
 

    -- Make sure the context use is authorize
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'UserManagement'


 
    DECLARE @TEMPLATE AS XML

    -- ===================================
    -- Gets the default password template
    -- ====================================
    SELECT @TEMPLATE = ( SELECT * FROM [v3d].[PasswordTemplates] WHERE [default] = 1 FOR XML RAW('passwordtemplate') )
 
    -- ======================================================================================
    -- Throw a NoutFoundException indicating that the default password template is not set
    -- ======================================================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Default Password Template'

 
    DECLARE @HINT AS NVARCHAR(1024)
    DECLARE @HINT_NAME AS NVARCHAR(32)
    DECLARE @NAME AS NVARCHAR(64)
    DECLARE @VALUE AS NVARCHAR(64)

    -- Temporary table to hold the hints with the template values infused in
    DECLARE @TABLE TABLE ( [name] NVARCHAR(32), [hint] NVARCHAR(1024) )

    -- Hint cursor so iterate through all the hints so that I can dynamically replace the embedded password template values
    DECLARE HINTS_CURSOR CURSOR LOCAL FOR SELECT [name], [hint] FROM [v3d].[PasswordHints] 

    -- Dynamic template node list ( I make it dynamic so that I only have to perform the Xpath query once )
    DECLARE COMPARE_CURSOR CURSOR LOCAL DYNAMIC FOR 
            SELECT '@' + n.value('local-name(.)', 'VARCHAR(MAX)') AS name, n.value('.', 'VARCHAR(MAX)') AS value 
              FROM @TEMPLATE.nodes('passwordtemplate/template/*') AS a(n)

    -- open the compare cursor
    OPEN COMPARE_CURSOR

    -- Open the hinst cursor
    OPEN HINTS_CURSOR

    SET @OPEN = 1

    FETCH NEXT FROM HINTS_CURSOR INTO @HINT_NAME, @HINT

    WHILE @@FETCH_STATUS = 0
    BEGIN
      

 

  
      FETCH FIRST FROM COMPARE_CURSOR INTO @NAME, @VALUE

      WHILE @@FETCH_STATUS = 0
      BEGIN
        

        SELECT @HINT = REPLACE(@HINT,@NAME,@VALUE)


        FETCH NEXT FROM COMPARE_CURSOR INTO @NAME, @VALUE

      END

      INSERT INTO @TABLE ( [name], [hint] ) VALUES ( @HINT_NAME, @HINT )

      FETCH NEXT FROM HINTS_CURSOR INTO @HINT_NAME, @HINT
  
    END

    -- Close the HINTS CURSOR
    CLOSE HINTS_CURSOR
    DEALLOCATE HINTS_CURSOR

    -- CLose the COMPARE CURSOR
    CLOSE COMPARE_CURSOR
    DEALLOCATE COMPARE_CURSOR

    
    SET @OPEN = 0

   
    DECLARE @SELECT NVARCHAR(MAX)
    DECLARE @PARAMETERS NVARCHAR(MAX)


    -- ========================================================
    -- Generate the password hints xml from the temprary table
    -- =========================================================
    SELECT @RESULTS = ( SELECT [hint] FROM @TABLE FOR XML RAW('passwordhint'), TYPE, ROOT('collection') )

       
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="passworndhint" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"passworndhint"} into (/collection)[1]')   
 
      
    RETURN 1

  END TRY
  BEGIN CATCH

    IF @OPEN = 1
    BEGIN
      -- Close the HINTS CURSOR
      CLOSE HINTS_CURSOR
      DEALLOCATE HINTS_CURSOR

      -- CLose the COMPARE CURSOR
      CLOSE COMPARE_CURSOR
      DEALLOCATE COMPARE_CURSOR
    END
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getRole'

  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getPasswordHint] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getPasswordHint] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getPasswordTemplate]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/25/2011
-- Description:	Returns the password templates
-- =============================================
CREATE PROCEDURE [v3d].[sp_getPasswordTemplate]
(
@ID NVARCHAR(256),
@QUERY AS XML = '<passwordtemplate />',        -- Query xml
@RESULTS AS XML OUTPUT                        -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  

  DECLARE @RC AS INT


  BEGIN TRY
 

    -- Make sure the context use is authorize
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'UserManagement'


    DECLARE @IDOC AS INT
    DECLARE @DEFAULT AS BIT

    SELECT @DEFAULT = 0

      
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @DEFAULT   = [default]
            FROM OPENXML(@IDOC, N'/passwordtemplate', 3) 
            WITH 
            (
              [default]     bit
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
      
    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)
    DECLARE @LEN AS INT

    SELECT @SELECT = 'SELECT *, [v3d].[fn_getVersion](''PasswordTemplates'') AS [ver] FROM [v3d].[PasswordTemplates] '

    IF @DEFAULT IS NOT NULL
      SELECT @WHERE = 'WHERE [default] = @DEFAULT'

   
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + ' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = ( ' + @SELECT + ' FOR XML RAW(''passwordtemplate''), TYPE , ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS XML OUTPUT, '    +
                      N'@DEFAULT  BIT, '   +
                      N'@COUNT INT OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @DEFAULT      = @DEFAULT,
            @COUNT        = @RC OUTPUT,
            @RESULTS      = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="passwordtemplate" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"passwordtemplate"} into (/collection)[1]')   
 
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getRole'

  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getPasswordTemplate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getPasswordTemplate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getRight]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/26/2011
-- Description:	Gets the rights
-- =============================================
CREATE PROCEDURE [v3d].[sp_getRight]
(
@ID       NVARCHAR(256),              -- Context Id
@QUERY    XML = '<right />',          -- Query xml
@RESULTS  XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;



  DECLARE @NAME AS NVARCHAR(512)
  DECLARE @RC AS INT

  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'UserManagement'

     
    DECLARE @IDOC AS INT

      
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @NAME           = [name]
            FROM OPENXML(@IDOC, N'/right', 3) 
            WITH 
            (
              [name]                nvarchar(32)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
      
 

    IF @NAME IS NOT NULL AND LEN(@NAME) > 0 
      SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion]('Rights') AS [ver] FROM [v3d].[Rights] WHERE [name] = @NAME FOR XML RAW('right'), TYPE, ROOT('collection') )
    ELSE
      SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion]('Rights') AS [ver] FROM [v3d].[Rights] FOR XML RAW('right'), TYPE, ROOT('collection') )

    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="right" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"right"} into (/collection)[1]')   

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getRight'

  
  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getRight] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getRight] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getRightNative]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/26/2011
-- Description:	Gets the rights
-- =============================================
CREATE PROCEDURE [v3d].[sp_getRightNative]
(
@ID       NVARCHAR(256),              -- Context Id
@QUERY    XML = '<right />',          -- Query xml
@RESULTS  NVARCHAR(MAX) OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC AS INT
  DECLARE @XML AS XML

  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC @RC = [v3d].[sp_getRight]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT

    SET @RESULTS  = CONVERT(NVARCHAR(MAX),@XML)


    RETURN @RC
     
  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getRightNative'

  
  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getRightNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getRightNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getRole]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/25/2011
-- Description:	Returns the roles xml
-- =============================================
CREATE PROCEDURE [v3d].[sp_getRole]
(
@ID       NVARCHAR(256),              -- Context Id
@QUERY    XML = '<role />',           -- Query xml
@RESULTS  XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @PRMS AS INT
  DECLARE @DESC AS NVARCHAR(256)
  DECLARE @SET AS BIGINT
  DECLARE @NAME AS NVARCHAR(512)


  DECLARE @RC AS INT


  BEGIN TRY
 

    -- Make sure the context use is authorize
    EXEC [v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'UserManagement'


    DECLARE @IDOC AS INT

      
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @NAME   = [name],
            @PRMS   = [prms],
            @DESC   = [desc],
            @SET    = [set]
            FROM OPENXML(@IDOC, N'/role', 3) 
            WITH 
            (
              [name]        nvarchar(32),
              [prms]        int,
              [desc]        nvarchar(256),
              [set]         int
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
      
    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SELECT  @WHERE =  [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]') + [System].v3d.fn_buildStringComparison(N'@DESC', @DESC, '[desc]'),
            @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Roles'') AS [ver] FROM [v3d].[Roles] '     
       
 
    IF @PRMS IS NOT NULL 
        SET @WHERE = @WHERE + ' [prms] = @PRMS AND '
           
    -- ===============
    -- Settings mask
    -- ================
    IF @SET IS NOT NULL OR @SET <> 0
    BEGIN
      SET @SET = (@SET & 15)
      SET @WHERE = @WHERE + '([set] & @SET) = @SET AND '
    END
    ELSE
    BEGIN

      -- ============================================================
      -- Only service users can perform queries on the settings mask
      -- =============================================================
      BEGIN TRY

        -- ======================================
        -- Check if they are SERVICE (Privledged)
        -- ======================================
        EXEC [v3d].[sp_isAuthorized]
          @ID = @ID,
          @MASK = 'Service'

 
      END TRY
      BEGIN CATCH
        SET @WHERE = @WHERE + '([set] & 2) <> 2 AND '     
      END CATCH

    END

    
    DECLARE @LEN AS INT
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + ' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = ( ' + @SELECT + ' FOR XML RAW(''role''), TYPE , ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS XML OUTPUT, '    +
                      N'@NAME NVARCHAR(512), '   +
                      N'@PRMS INT,'             + 
                      N'@DESC NVARCHAR(256), '  +
                      N'@SET  BIGINT, '            +
                      N'@COUNT INT OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @NAME         = @NAME,
            @DESC         = @DESC,
            @PRMS         = @PRMS,
            @SET          = @SET,
            @COUNT        = @RC OUTPUT,
            @RESULTS      = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="role" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"role"} into (/collection)[1]')   
 
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getRole'

  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getRole] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getRole] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getService]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/25/2013
-- Description:	Get resource will run the standard stored procedure sp_get<RESOURCE> if found otherwise
--              will assume the resource is a table abd return all rows.
-- =====================================================================================================
CREATE PROCEDURE [v3d].[sp_getService]
(
@ID       NVARCHAR(256),    -- Context Id
@QUERY    XML = NULL,       -- Query xml
@RESULTS  XML OUTPUT        -- Result set
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
 
  DECLARE @PARAMETERS AS NVARCHAR(MAX)
  DECLARE @PROCEDURE AS NVARCHAR(512) 
  DECLARE @RESOURCE AS NVARCHAR(128)
  DECLARE @SINGULAR AS NVARCHAR(128)

  SELECT  @RESOURCE = ''
         
  BEGIN TRY
      
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'Service'

    

    -- ================================================================
    -- Resource name is missing or invalid due to the query being NULL
    -- ================================================================
    IF @QUERY IS NULL 
       EXEC [master].[dbo].[sp_throwInvalidDataException] 'Service Query'


    DECLARE @RESOURCE_QUERY AS XML

    DECLARE @IDOC AS INT
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @RESOURCE = [tbl],
            @RESOURCE_QUERY  = [query]
            FROM OPENXML(@IDOC, N'/service', 3) 
            WITH 
            (
              [tbl]     nvarchar(64),
              [query]   xml  './*'
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

     -- ================================================================
    -- Resource name is missing or invalid due to the tag missing
    -- ================================================================ 
    IF @RESOURCE IS NULL OR LEN(@RESOURCE) = 0
        EXEC [master].[dbo].[sp_throwInvalidDataException]  'Resource Name'

    -- ====================================================
    -- Gets the resource name which is the XML Nodes Tag
    -- ====================================================
    IF @RESOURCE_QUERY IS NOT NULL
    BEGIN
      DECLARE @INNER AS NVARCHAR(64)
      SET @INNER = CONVERT(NVARCHAR(128),@RESOURCE_QUERY.query('local-name((/*)[1])'))

      IF UPPER(@INNER) <> UPPER(@RESOURCE)
        EXEC [master].[dbo].[sp_throwInvalidDataException]  'Query Resource'

    END

    
    -- ====================================================================
    -- If the resource ends is a ies then replace that with a y to make it
    -- singular. If it ends with and s then strip it off
    -- ====================================================================
    IF SUBSTRING(@RESOURCE,LEN(@RESOURCE) - 2, 3) = 'ies'
      SELECT @RESOURCE = LEFT(@RESOURCE, LEN(@RESOURCE) - 3 ) + 'y'
    ELSE IF  SUBSTRING(@RESOURCE,LEN(@RESOURCE), 1) = 's'
      SELECT @RESOURCE = LEFT(@RESOURCE, LEN(@RESOURCE) - 1 )


    SELECT @PROCEDURE = 'sp_get' + @RESOURCE


    -- ===================================================================
    -- Check if there is a sp_get storeded procedure for this resource
    -- ===================================================================
    IF EXISTS( SELECT * FROM sys.sysobjects WHERE [name] = @PROCEDURE )
    BEGIN

      DECLARE @EXEC AS NVARCHAR(MAX)

      SELECT @EXEC = 'EXEC [v3d].[' + @PROCEDURE + '] @ID = ''' + @ID + ''', @QUERY = @QUERY, @RESULTS = @RESULTS OUTPUT'

    
      SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                        N'@QUERY              XML '

      EXEC sp_executesql 
              @EXEC,
              @PARAMETERS,
              @QUERY            = @QUERY,
              @RESULTS          = @RESULTS OUTPUT


     END
     ELSE
     BEGIN
  
        SET @SINGULAR = @RESOURCE
       
        IF NOT EXISTS ( SELECT * FROM sys.sysobjects WHERE [name] = @RESOURCE AND [type] = 'U' )
        BEGIN 
        
          -- ============================================================
          -- If the resource ends is a y then add ies otherwise add s
          -- ===========================================================
          IF SUBSTRING(@RESOURCE,LEN(@RESOURCE), 1) = 'y'
            SELECT @RESOURCE = LEFT(@RESOURCE, LEN(@RESOURCE) -1 ) + 'ies'
          ELSE
            SELECT @RESOURCE = @RESOURCE + 's'
          
          -- ========================
          -- Resource was not found
          -- ========================
          IF NOT EXISTS ( SELECT * FROM sys.sysobjects WHERE [name] = @RESOURCE AND [type] = 'U' )
            EXEC [master].[dbo].[sp_throwNotFoundException] 'Resource', @SINGULAR

        END

        DECLARE @SELECT AS NVARCHAR(MAX)

        SELECT @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''' + @RESOURCE + ''') AS [ver] FROM [v3d].' + @RESOURCE + ' FOR XML RAW(''' + @SINGULAR + '''), TYPE, ROOT(''collection'') )' 

   
        SET @PARAMETERS = N'@RESULTS            XML OUTPUT '

        EXEC sp_executesql 
                @SELECT,
                @PARAMETERS,
                @RESULTS          = @RESULTS OUTPUT


      IF @RESULTS IS NULL
        SET @RESULTS = '<collection id="''' + @SINGULAR + '''" />'
      ELSE
        SET @RESULTS.modify('insert attribute id {sql:variable("@SINGULAR")} into (/collection)[1]')   
 
  
     END

     RETURN 1

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getService'
  
  END CATCH


END



GO
GRANT EXECUTE ON [v3d].[sp_getService] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================================================
-- Author:		  Mark Kontak
-- Create date: 03/01/2010  
-- Description:	Returns the either a single user or a collection of users depending on the parameters 
--              passed in. If the @LOGIN is set then a single use is returned and the @QUERY is ignored.
--              If the @LOGIN is NULL then the @QUERY is used.
--
-- Paramaters:
--              @QUERY      This is a xml query specifying the fields to search by. Each attribute 
--                          corresponds to a column in the user table.
--              @RESULTS    This is the XML result set of the query
-- ==========================================================================================================
CREATE PROCEDURE [v3d].[sp_getUser]
(
@ID       NVARCHAR(256),               -- Context Id
@QUERY    XML = '<user />',    -- Query xml
@RESULTS  XML OUTPUT                   -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;



  DECLARE @RC AS INT


  BEGIN TRY

    -- Make sure the context is valid
    EXEC [v3d].[sp_validateContext]
      @ID = @ID,
      @CHECK_EXPIRATION = 1



      
     
    DECLARE @IDOC AS INT

    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @NAME           AS NVARCHAR(64)
    DECLARE @FROM_EXPDATE   AS DATETIMEOFFSET
    DECLARE @TO_EXPDATE     AS DATETIMEOFFSET
    DECLARE @FROM_DATEADDED AS DATETIMEOFFSET
    DECLARE @TO_DATEADDED   AS DATETIMEOFFSET
    DECLARE @STATE          AS INT
    DECLARE @DOMAIN         AS NVARCHAR(512)
    DECLARE @APPS           AS INT
    DECLARE @SET            AS BIGINT
    DECLARE @AUTH           AS NVARCHAR(512)
    DECLARE @ROLE           AS NVARCHAR(512)
    DECLARE @LOGIN          AS NVARCHAR(512)

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @LOGIN          = [login], 
            @NAME           = [name],
            @DOMAIN         = [domain], 
            @SET            = [set],
            @APPS           = [apps],
            @AUTH           = [auth],
            @ROLE           = [role],
            @FROM_EXPDATE   = [fromExpirationDate],
            @TO_EXPDATE     = [toExpirationDate],
            @FROM_DATEADDED = [fromDateAdded],
            @TO_DATEADDED   = [toDateAdded],
            @STATE          = [state]
            FROM OPENXML(@IDOC, N'/user', 3) 
            WITH 
            (
              [login]               nvarchar(512),
              [name]                nvarchar(512),
              [auth]                nvarchar(512),
              [apps]                int,
              [set]                 bigint,
              [state]               int,
              [role]                nvarchar(512),
              [domain]              nvarchar(512),
              [fromExpirationDate]  datetimeoffset 'exp/@from',
              [toExpirationDate]    datetimeoffset 'exp/@to',
              [fromDateAdded]       datetimeoffset 'add/@from',
              [toDateAdded]         datetimeoffset 'add/@to'               
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  
    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
            
    SELECT @WHERE = [System].v3d.fn_buildStringComparison(N'@LOGIN', @LOGIN, '[login]') + [System].v3d.fn_buildStringComparison(N'@ROLE', @ROLE, '[role]')

      
    IF @STATE IS NOT NULL 
      SET @WHERE = @WHERE + ' [state] = @STATE AND '
            
      -- ===============
    -- Settings mask
    -- ================
    IF @SET IS NOT NULL OR @SET <> 0
    BEGIN
      SET @SET = (@SET & 15)
      SET @WHERE = @WHERE + '([set] & @SET) = @SET AND '
    END
    ELSE
    BEGIN
      BEGIN TRY

        -- ======================================
        -- Check if they are SERVICE (Privledged)
        -- ======================================
        EXEC [v3d].[sp_isAuthorized]
          @ID = @ID,
          @MASK = 'Service'

 
      END TRY
      BEGIN CATCH
        SET @WHERE = @WHERE + '([set] & 2) <> 2 AND '     
      END CATCH

    END

            

    IF @APPS IS NOT NULL
        SET @WHERE = @WHERE + ' ( [apps] & @APPS ) > 0 AND '

      
    SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_EXPDATE, N'@FROM_EXPDATE', @TO_EXPDATE, N'@TO_EXPDATE', N'[exp]')
     
    SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_DATEADDED, N'@FROM_DATEADDED', @TO_DATEADDED, N'@TO_DATEADDED', N'[add]')
      
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]')
      
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@AUTH', @AUTH, '[auth]')
      
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@DOMAIN', @DOMAIN, '[dom]')    
 

    SET @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Users'') AS [ver], ' +
                  '          ( SELECT [prms] FROM [v3d].[Roles] WHERE [name] = [role] ) AS [perm], ' +
                  '          ( SELECT [id] FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[login] = [v3d].[Users].[login] ) [ctx] ' +
                  '          FROM [v3d].[Users] '
    
    DECLARE @LEN AS INT
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''user''), TYPE , ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS        XML OUTPUT, ' +
                      N'@FROM_EXPDATE   DATETIMEOFFSET(3), ' + 
                      N'@TO_EXPDATE     DATETIMEOFFSET(3), ' +
                      N'@FROM_DATEADDED DATETIMEOFFSET(3), ' +
                      N'@TO_DATEADDED   DATETIMEOFFSET(3), '  +
                      N'@NAME           NVARCHAR(64), ' +
                      N'@AUTH           NVARCHAR(32), ' +
                      N'@LOGIN          NVARCHAR(32), ' +
                      N'@DOMAIN         NVARCHAR(512), ' +
                      N'@ROLE           NVARCHAR(128), ' +
                      N'@APPS           INT, ' +
                      N'@STATE          INT, ' +
                      N'@COUNT          INT OUTPUT, ' +
                      N'@SET            BIGINT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_EXPDATE     = @TO_EXPDATE,
            @FROM_EXPDATE   = @FROM_EXPDATE,
            @FROM_DATEADDED = @FROM_DATEADDED,
            @TO_DATEADDED   = @TO_DATEADDED,
            @NAME           = @NAME,
            @AUTH           = @AUTH,
            @SET            = @SET,
            @LOGIN          = @LOGIN,
            @ROLE           = @ROLE,
            @APPS           = @APPS,
            @DOMAIN         = @DOMAIN,
            @STATE          = @STATE,
            @COUNT          = @RC OUTPUT,
            @RESULTS        = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="user" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"user"} into (/collection)[1]')   
 
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getUser'
  
  END CATCH
  
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getUser] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getUser] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getUserNative]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================================================
-- Author:		  Mark Kontak
-- Create date: 03/01/2010  
-- Description:	Returns the either a single user or a collection of users depending on the parameters 
--              passed in. If the @LOGIN is set then a single use is returned and the @QUERY is ignored.
--              If the @LOGIN is NULL then the @QUERY is used.
--
-- Paramaters:
--              @QUERY      This is a xml query specifying the fields to search by. Each attribute 
--                          corresponds to a column in the user table.
--              @RESULTS    This is the XML result converted to a string
-- ==========================================================================================================
CREATE PROCEDURE [v3d].[sp_getUserNative]
(
@ID       NVARCHAR(256),            -- Context Id
@QUERY    XML = '<user />',         -- Query xml
@RESULTS  NVARCHAR(MAX) OUTPUT      -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;



  DECLARE @RC AS INT
  DECLARE @XML AS XML


  BEGIN TRY

    -- Make sure the context is valid
    EXEC @RC =  [v3d].[sp_getUser]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT


    SET @RESULTS = CONVERT(NVARCHAR(MAX),@XML)
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getUserNative'
  
  END CATCH
  
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getUserNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getUserNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getUserPassword]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/25/2011
-- Description:	Returns the passwords xml
-- =============================================
CREATE PROCEDURE [v3d].[sp_getUserPassword]
(
@LOGIN AS NVARCHAR(32) = NULL,
@PASSWORDS AS XML OUTPUT
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  


  DECLARE @RC AS INT


  BEGIN TRY
  
 
    
    DECLARE @LEN AS INT  
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SELECT  @WHERE = '', 
            @SELECT = 'SELECT * FROM [v3d].[UserPasswords] ' 
   
    IF @LOGIN IS NOT NULL
      SET @WHERE = ' [login] = @LOGIN AND '

    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @PASSWORDS = ( ' + @SELECT + ' FOR XML RAW(''password''), TYPE, ROOT(''passwords'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@LOGIN NVARCHAR(64), ' +
                      N'@COUNT INT OUTPUT, ' + 
                      N'@PASSWORDS XML OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @LOGIN        = @LOGIN,
            @COUNT        = @RC OUTPUT,
            @PASSWORDS    = @PASSWORDS OUTPUT
      
    IF @PASSWORDS IS NULL 
      SET @PASSWORDS = '<passwords />'
   
      
    RETURN @RC

  END TRY
  BEGIN CATCH 
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getPasswords'

  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getUserPassword] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getUserPassword] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getUserProfile]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Gets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_getUserProfile]
(
@ID       NVARCHAR(256),               -- Context Id
@QUERY    XML = '<userprofile />',          -- Query xml
@RESULTS  XML OUTPUT                   -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT

  DECLARE @DESC AS NVARCHAR(256)
  DECLARE @LOGIN AS NVARCHAR(1024)
  DECLARE @NAME AS NVARCHAR(1024)
 

  BEGIN TRY
  
     -- Make sure the context is valid
    EXEC [v3d].[sp_validateContext]
      @ID = @ID,
      @CHECK_EXPIRATION = 1
    
    DECLARE @IDOC AS INT

  
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @LOGIN   = [login],
            @NAME    = [name]
            FROM OPENXML(@IDOC, N'/userprofile', 3) 
            WITH 
            (
              [login]       nvarchar(1024),
              [name]        nvarchar(1024)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
 
    -- =================================================================   
    -- Raise an invlaid login exception. 
    --  (900990) Login is missing or invalid
    -- =================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException]'Login';
  
    
    IF @NAME IS NULL OR LEN(@NAME) = 0 
      SET @RESULTS = ( SELECT *, [System].[v3d].[fn_getVersion]('UserProfiles') AS [ver] 
                      FROM [v3d].[UserProfiles] WHERE [login] = @LOGIN FOR XML RAW('userprofile'), TYPE, ROOT('collection')  )
    ELSE
      SET @RESULTS = ( SELECT *, [System].[v3d].[fn_getVersion]('UserProfiles') AS [ver]  
                    FROM [v3d].[UserProfiles] WHERE [login] = @LOGIN AND [name] = @NAME FOR XML RAW('userprofile'), TYPE, ROOT('collection') ) 

    
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="userprofile" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"userprofile"} into (/collection)[1]')   
          
    RETURN @RC
    
    
   END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_UserProfile'

  
  END CATCH
   
    
END





GO
GRANT EXECUTE ON [v3d].[sp_getUserProfile] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getUserProfile] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getUserProfileNative]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Gets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_getUserProfileNative]
(
@ID       NVARCHAR(256),               -- Context Id
@QUERY    XML = '<userprofile />',     -- Query xml
@RESULTS  NVARCHAR(MAX) OUTPUT         -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT
  DECLARE @XML AS XML

  BEGIN TRY
  
     EXEC @RC = [v3d].[sp_getUserProfile]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT
    
    SET @RESULTS  = CONVERT(NVARCHAR(MAX),@XML)
          
    RETURN @RC
    
    
   END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_UserProfileNative'

  
  END CATCH
   
    
END





GO
GRANT EXECUTE ON [v3d].[sp_getUserProfileNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getUserProfileNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertRole]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- ==========================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/24/2009
-- Description:	Returns the contexts table in xml format includes the license and the 
--
--              Exceptions:
--
--              901237    Must specify role name.
--
-- NOTE:  Currently all roles are added during the creation of the Security database as of
--        Console 3.5. The requirement to add roles by an administrator was removed in 
--        Console 3.5. Will keep this procedure in-case the requirement is added back into 
--        the Security Subsystem.
-- ============================================================================================
CREATE PROCEDURE [v3d].[sp_insertRole]
(
@ID NVARCHAR(256) = '',      -- Context ID
@RECORD XML                  -- User XML 
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @NAME NVARCHAR(32)             
  DECLARE @DESCRIPTION NVARCHAR(64)
  DECLARE @RIGHTS BIGINT
  DECLARE @SET AS BIGINT
  DECLARE @IDOC AS INTEGER

	BEGIN TRY
	    

    -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'Service'


    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @NAME         = [name],
            @DESCRIPTION  = [desc],
            @SET          = [set],
            @RIGHTS       = [prms]
          FROM OPENXML(@IDOC, N'/role', 1) WITH 
          ( 
            [name]    nvarchar(32),
            [desc]    nvarchar(64),
            [prms]    bigint,
            [set]     bigint
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- ========================================================================================================
    -- Make sure that a name was specified or we cannot add the role. (900990) Role name is missing or invalid
    -- ========================================================================================================
    IF @NAME IS NULL OR  LEN(@NAME) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Role Name' ;

    -- =============================================
    -- Make sure the roles does not already exist
    -- (900992) Role: Already exists
    -- =============================================
    IF EXISTS ( SELECT * FROM [v3d].[Roles] WHERE [name] = @NAME )
       EXEC [master].[dbo].[sp_throwException]  900992,'Role';

    -- Insert the Role
    INSERT INTO [v3d].[Roles] ( [name], [desc], [prms], [set] )
            VALUES ( @NAME, ISNULL(@DESCRIPTION,''), ISNULL(@RIGHTS,0), ISNULL(@SET,0) );

    RETURN 1

  
 	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertRole'
	  
	END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_insertRole] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertRole] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================================================
-- Author:		  Mark Kontak
-- Create date: 1/28/2010
-- Description:	Adds user to the database. If context will validate against the permission masks to make
--              sure the user has the proper authorization.
--
--              V3D0927     No login specified
--              V3D0914     User already exists
--
-- =========================================================================================================
CREATE PROCEDURE [v3d].[sp_insertUser]
(
@ID NVARCHAR(256) = '',        -- Context ID
@RECORD XML                    -- User XML 
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)
  DECLARE @IDOC AS INTEGER
  DECLARE @PASSWORD AS NVARCHAR(256)
  DECLARE @LOGIN AS NVARCHAR(16)
  DECLARE @NAME AS NVARCHAR(64)
  DECLARE @EXPDATE  AS DATETIMEOFFSET
  DECLARE @SET AS BIGINT
  DECLARE @AUTH AS NVARCHAR(32)
  DECLARE @DOMAIN AS NVARCHAR(32)
  DECLARE @ROLE NVARCHAR(32)
  DECLARE @PTMP AS NVARCHAR(32)
  DECLARE @AMASK AS INT
  DECLARE @STATE AS INT
  DECLARE @TRANSCOUNT AS INT

  SELECT @TRANSCOUNT = @@TRANCOUNT,
        @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)	
  

	BEGIN TRY

	    IF @TRANSCOUNT = 0
        BEGIN TRANSACTION
 	    
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @LOGIN      = [login],
            @NAME       = [name],
            @EXPDATE    = [exp],
            @SET        = [set],
            @AUTH       = [auth],
            @DOMAIN     = [dom],
            @STATE      = [state],
            @PASSWORD   = [pwd],
            @AMASK      = [apps],
            @PTMP       = [ptmp],
            @ROLE       = [role]
          FROM OPENXML(@IDOC, N'/user', 1) WITH 
          ( 
            [login]   nvarchar(16),
            [name]    nvarchar(64),
            [exp]     datetimeoffset(0),
            [set]     bigint,
            [auth]    nvarchar(32),
            [dom]     nvarchar(32),
            [state]   int,
            [pwd]     nvarchar(256),
            [role]    nvarchar(32),
            [ptmp]    nvarchar(32),
            [apps]    int
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    
   
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authorized to perform this task.
    -- ============================================================================================
    EXEC [v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'UserManagement'

   
  
    -- =========================================================================================================================
    -- Set the password to a 0 length string insteand of NULL. We first check the stored procedure parameter before attempting
    -- to use the embedded password in the xml
    -- =========================================================================================================================
    SET @PASSWORD = ISNULL(@PASSWORD,'')
      
    -- Used to determine if the user already exists
    DECLARE @COUNT AS INT
    
   
    -- ================================================================================================
    -- Make sure that a login was specified or we cannot add the user. (900990) Missing or invalid login
    -- =================================================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0 
     EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';
   
    -- =====================================================================================
    -- If the user is already in the Users table then raise (900992) User already exists
    -- =====================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Users] WHERE [login] = @LOGIN )
      EXEC [master].[dbo].[sp_throwException] 900992, 'Account';
    
      -- ==========================================
    -- Make sure a role was specified
    --   (900990) Role: Missing or Invalid
    -- =========================================
    IF @ROLE IS NULL OR LEN(@ROLE) = 0
       EXEC [master].[dbo].[sp_throwInvalidDataException] 'Role' ;
  
    -- =================================================================
    -- Make sure the specified role is valid. ( 901234 ) Invalid role
    -- =================================================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[Roles] WHERE [name] = @ROLE )
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Role' ;
    
    -- ===============================================
    -- Make sure if the state is supplied it is valid
    --  (900991) State: Not Found
    -- ===============================================
    IF @STATE IS NOT NULL AND NOT EXISTS ( SELECT * FROM [v3d].[UserStates] WHERE [value] = @STATE ) 
      EXEC [master].[dbo].[sp_throwNotFoundException] 'User State' ;
 
    -- ==================================================================
    -- If the authentication type was not specified then get the default
    -- ====================================================================
    IF @AUTH IS NULL OR LEN(@AUTH) = 0
    BEGIN
      SELECT @AUTH = [id] FROM [v3d].[AuthenticationTypes] WHERE [default] = 1
      
      -- =========================================
      -- No authentication type default found
      -- ==========================================
      IF @AUTH IS NULL 
        EXEC [master].[dbo].[sp_throwException]  900996,'Authentication Type' ;
       
    END
    ELSE IF NOT EXISTS ( SELECT * FROM [v3d].[AuthenticationTypes] WHERE [id] = @AUTH )
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Authentication Type' ;
    
    -- ==================================================================
    -- If the password template was not specified then get the default
    -- ====================================================================
    IF @PTMP IS NULL OR LEN(@PTMP) = 0
    BEGIN
      SELECT @PTMP = [id] FROM [v3d].[PasswordTemplates] WHERE [default] = 1

      -- =========================================
      -- No password template deafult found 
      -- ==========================================
      IF @PTMP IS NULL 
        EXEC [master].[dbo].[sp_throwException]  900996,'Password Template';
 
    END
    ELSE IF NOT EXISTS ( SELECT * FROM [v3d].[PasswordTemplates] WHERE [id] = @PTMP )
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Password Template';
 
      
          
    -- The default authentication is Viatronix
    SET @AUTH = ISNULL(@AUTH,'Viatronix')
    
    -- ============================================================================
    -- Make sure the password is supplied if the authentication type is Viatronix
    -- ============================================================================
    IF UPPER(@AUTH) = 'VIATRONIX' AND ( @PASSWORD IS NULL OR LEN(@PASSWORD) = 0 )
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Password';
    
    -- ===================================================================================
    -- Decrypt the password since all passwords a travel through the system encrypted and 
    -- are only decrypted at an edn-point. This encrypted password is not the password 
    -- that is stored another algorithm is used to generate the encrypted password that is
    -- stored in the database.
    -- ===================================================================================
    EXEC [v3d].[sp_decryptPassword]
      @ENCRYPTED = @PASSWORD,
      @DECRYPTED = @PASSWORD OUTPUT

    -- ===============================================================================================
    -- Unless the person performing the insert is a service account the settings valud cannot be set
    -- ===============================================================================================
    BEGIN TRY
      EXEC [v3d].[sp_isAuthorized]
        @ID = @ID,
        @MASK = 'Service'
    END TRY
    BEGIN CATCH

      -- ======================================================
      -- Will allow the privledge setting to be set by an admin
      -- ======================================================
      SET @SET = ( @SET & 2 )

    END CATCH

    -- ============================================================================
    -- If the expiration date was not supplied dont include it in the insert 
    -- ============================================================================
    IF @EXPDATE IS NULL
      INSERT INTO [v3d].[Users] ( [login], [name], [set], [auth], [dom],  [apps], [role], [state], [ptmp] ) VALUES
                        ( @LOGIN, ISNULL(@NAME,''), ISNULL(@SET,0), @AUTH, ISNULL(@DOMAIN,''), 
                          ISNULL(@AMASK, 0), 
                          ISNULL(@ROLE,''), ISNULL(@STATE,0), @PTMP )
    ELSE
       INSERT INTO [v3d].[Users] ( [login], [name], [set], [exp], [auth], [dom], [apps], [role], [state], [ptmp] ) VALUES
                        ( @LOGIN, ISNULL(@NAME,''), ISNULL(@SET,0), @EXPDATE, @AUTH, ISNULL(@DOMAIN,''), 
                          ISNULL(@AMASK, 0), 
                          ISNULL(@ROLE,''), ISNULL(@STATE,0), @PTMP )
                          
    -- ==========================================================================
    -- If a password was passed in then change it now
    -- ==========================================================================
    IF UPPER(@AUTH) = 'VIATRONIX' 
    BEGIN
        
       -- Change the users password with the password in the xml
       EXEC	[v3d].[sp_changePassword]
            @ID = @ID,
            @LOGIN = @LOGIN,
            @NEW_PASSWORD = @PASSWORD

		  -- =========================================================================================================
		  -- Reset the users state just in-case the password change bit was set. This way the user is required to 
		  -- change it themselves.
		  -- =========================================================================================================
		  UPDATE [v3d].[Users] SET [state] = ISNULL(@STATE,0) WHERE [login] = @LOGIN

    END    

      
  
     -- Add audit entry for successful action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'insert', 
     @STATUS = 1,
     @USER = @CONTEXT_LOGIN, 
     @EXT = @RECORD
     
    -- ================================================================
    -- No pasword was specified so we need to force a password change
    -- ================================================================ 
    IF UPPER(@AUTH) = 'VIATRONIX' AND LEN(@PASSWORD) = 0 
    BEGIN    
            
       -- No password was specified we force a password change
       EXEC [v3d].[sp_forcePasswordChange]
            @ID = @ID,
            @LOGIN = @LOGIN
              
    END

    IF @TRANSCOUNT = 0 AND @@TRANCOUNT > 0
      COMMIT  TRANSACTION
 
     RETURN 1
    
 	END TRY
	BEGIN CATCH

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION
 

    DECLARE @ERROR_MESSAGE AS NVARCHAR(1024)

    SELECT @ERROR_MESSAGE = ERROR_MESSAGE()
  
    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'insert', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE


     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertUser'

   
	END CATCH


  
END





GO
GRANT EXECUTE ON [v3d].[sp_insertUser] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertUserPassword]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================================================================================
-- Author:		  Mark Kontak
-- Create date: 1/29/2010
-- Description: Validates the password against the complexity template of the user associated with the login.
-- =======================================================================================================================================
CREATE PROCEDURE [v3d].[sp_insertUserPassword]
(
@LOGIN NVARCHAR(16),            -- Users Login
@PASSWORD NVARCHAR(256),         -- Password to validate (Not Encrypted)
@IGNORE_FREQUENCY BIT = 0,      -- Flag indicating we are to ignore the frequency setting
@VALIDATE_ONLY BIT = 0          -- Validate Only flag will not insert the PASSWORD into the Passwords table
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @COUNT AS INT
  DECLARE @EXDATE AS DATETIMEOFFSET(7)
  DECLARE @TYPE AS NVARCHAR(64)
  DECLARE @ENCRYPTED_PASSWORD AS NVARCHAR(128)
 

  DECLARE @VALUE NVARCHAR(255)
  DECLARE @ULOGIN NVARCHAR(16)
  DECLARE @UPASSWORD NVARCHAR(32)
  
  
  DECLARE @ERROR_MESSAGE NVARCHAR(4000);
  DECLARE @ERROR_NUMBER INT
  DECLARE @ERROR_LINE INT
  DECLARE @ERROR_SEVERITY INT
  DECLARE @ERROR_STATE INT
  DECLARE @ERROR_PROCEDURE NVARCHAR(128)
  DECLARE @PWDTMP AS XML
  DECLARE @LENGTH AS INT
  DECLARE @START AS INT
  DECLARE @TRANSCOUNT AS INT

  SELECT @TRANSCOUNT = @@TRANCOUNT

  BEGIN TRY
  
    SET XACT_ABORT ON

    IF @TRANSCOUNT = 0
     BEGIN TRANSACTION

    -- ===========================================
    -- Make sure the login was supplied
    --  (900990) Login: Missing or Invalid
    -- ==========================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';
 
    -- ===========================================
    -- Make sure the password was supplied
    --  (900990) Password: Missing or Invalid
    -- ==========================================
    IF @PASSWORD IS NULL OR LEN(@PASSWORD) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Password';
 

    -- Get the 
    SET @PWDTMP = ( SELECT [v3d].[PasswordTemplates].[template] 
                      FROM [v3d].[Users] INNER JOIN [v3d].[PasswordTemplates] 
                        ON [v3d].[Users].[ptmp] = [v3d].[PasswordTemplates].[id]
                     WHERE [v3d].[Users].[login] = @LOGIN FOR XML RAW('root') )

    -- ==============================================
    -- If the account was not found then raise the 
    --   (900991) Account: Not Found 
    -- ==============================================
    IF @@ROWCOUNT = 0
       EXEC [master].[dbo].[sp_throwNotFoundException] 'Account';
 
    -- ===============================================
    -- Make sure the account has a password template
    --   (900991) Password Template: Not Found 
    -- ==============================================
    IF @PWDTMP IS NULL
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Password Template';

    SET @ULOGIN     = UPPER(@LOGIN)
    SET @UPASSWORD  = UPPER(@PASSWORD)


    DECLARE @UPPER AS INT
    DECLARE @LOWER AS INT
    DECLARE @SPECIAL AS INT
    DECLARE @NUMERIC AS INT
    DECLARE @MAX AS INT
    DECLARE @MIN AS INT
    DECLARE @REPITITION AS INT
    DECLARE @SPACES AS BIT
    DECLARE @EMBED AS BIT
    DECLARE @DIFFER AS INT
    DECLARE @TTL AS INT
    DECLARE @REUSE AS INT
    DECLARE @FREQUENCY AS INT
    DECLARE @IDOC AS INT
         
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @PWDTMP

    SELECT  @UPPER      = [upper],
            @LOWER      = [lower],
            @SPECIAL    = [special],
            @NUMERIC    = [numeric],
            @REPITITION = [repitition], 
            @SPACES     = [spaces],
            @MIN        = [min],
            @MAX        = [max],
            @EMBED      = [embed],
            @TTL        = [ttl],
            @REUSE      = [reuse],
            @FREQUENCY  = [frequency],
            @DIFFER     = [differ]
            FROM OPENXML(@IDOC, N'/root/template', 3) 
            WITH 
            (
              [upper]       int,
              [lower]       int,
              [special]     int,
              [numeric]     int,
              [repitition]  int,
              [spaces]      bit,
              [embed]       bit,
              [min]         int,
              [max]         int,
              [ttl]         int,
              [reuse]       int,
              [frequency]   int,
              [differ]      int
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

        
    -- Encrypt the password
    EXEC [v3d].[sp_encrypt] 
      @value = @PASSWORD,
      @encryptedValue = @ENCRYPTED_PASSWORD OUTPUT


    DECLARE @CURRENT_PASSWORD AS NVARCHAR(128)

    -- Get the current password which is encrypted
    SELECT TOP(1) @CURRENT_PASSWORD = [pwd] FROM [v3d].[UserPasswords] WHERE [login] = @LOGIN ORDER BY [date] DESC

    -- =====================================================================================================================
    -- If the REUSE count is set and the new password exists in any of the users last @REUSE passwords then throw exception
    -- =====================================================================================================================
    IF @REUSE > 0 AND 
       EXISTS ( SELECT * FROM (SELECT TOP(@REUSE) [pwd] 
                                 FROM [v3d].[UserPasswords] 
                                WHERE [login] = @LOGIN ORDER BY [date] DESC ) AS [pwds] 
                 WHERE [pwd] = @ENCRYPTED_PASSWORD  )
        EXEC [master].[dbo].[sp_throwException] 901221;

    -- =================================================================================
    -- If the character difference count is set  and the current password exists then
    -- check each character in the new and old password.
    -- ================================================================================
     IF @DIFFER > 0 AND @CURRENT_PASSWORD IS NOT NULL
     BEGIN

      -- Decrypt the password
      EXEC [v3d].[sp_decrypt]
        @value = @CURRENT_PASSWORD,
        @decryptedValue = @CURRENT_PASSWORD OUTPUT

      SET @COUNT = 0
      SET @START = 1
      SET @LENGTH = LEN(@PASSWORD)
      IF @LENGTH > LEN(@CURRENT_PASSWORD)
        SET @LENGTH = LEN(@CURRENT_PASSWORD)

      WHILE @START <= @LENGTH
	    BEGIN 
        
        IF SUBSTRING(@PASSWORD,@START,1) <> SUBSTRING(@CURRENT_PASSWORD,@START,1)
          SET @COUNT = @COUNT + 1

        SET @START = @START + 1

	    END -- END WHILE

      -- ==================================================================================
      -- If the number of matches to the old password is reater then the amount 
      -- it is to be different the raise (901213) Password must differ by X characters 
      -- =================================================================================
      IF @COUNT < @DIFFER 
        EXEC [master].[dbo].[sp_throwException] 901213, @DIFFER;

       
      
     END

    -- ======================================================================================================================
    -- Check for frequent password changes. 
    --    If the user is changing their password more times then the FREQUENCY variable states in hours then an error is 
    --    raised indicating that they must wait 
    --        (901222) Cannot change password again, until %d day(s) has past. 
    --
    --    Exceptions: (Bypasses this check)
    --        An service/administrator will ignore this option if they are changing their own password.
    --        An service/administrator will ignore this option if they are changing someone elses password.
    --        A user is considered privledged ( [set] & 2 ) = 2
    -- ======================================================================================================================
    IF @FREQUENCY > 0 AND @IGNORE_FREQUENCY = 0 AND
        EXISTS ( SELECT * FROM [v3d].[UserPasswords] WHERE [login] = @LOGIN AND SYSDATETIMEOFFSET() < DATEADD(hour,@FREQUENCY,[date]) ) AND
        EXISTS ( SELECT * FROM [v3d].[Users] INNER JOIN [v3d].[Roles] ON [v3d].[Roles].[name] = [v3d].[Users].[role] 
                  WHERE [v3d].[fn_isRightSet]([v3d].[Roles].[prms],'Administration') = 0 OR
                        [v3d].[fn_isRightSet]([v3d].[Roles].[prms],'Service') = 0 OR 
                        ([v3d].[Users].[set] & 2 ) = 2 ) 
       EXEC [master].[dbo].[sp_throwException] 901222, @FREQUENCY;
   
     
 
    --===================================================
    -- Make sure the login is not embedded in the password
    -- (901214) Password cannot contain login
    --===================================================
    IF @EMBED = 0 AND PATINDEX('%' + @ULOGIN + '%', @UPASSWORD ) > 0 
      EXEC [master].[dbo].[sp_throwException] 901214;
   
    
    --====================================================
    -- Make sure the password does not exceeed the max len
    -- (901226) Password exceeed max length
    --====================================================
    IF LEN(@PASSWORD) > @MAX 
     EXEC [master].[dbo].[sp_throwException] 901226, 'Password Length', @MAX;


    --====================================================
    -- Make sure the password does not exceeed the min len
    -- (901225) Password exceeed max length
    --====================================================
    IF LEN(@PASSWORD) < @MIN 
      EXEC [master].[dbo].[sp_throwException] 901225, 'Password Length', @MIN;
 
    --===================================================
    -- Make sure the login is not embedded in the password
    -- (901217) Spaces not allowed in password
    --===================================================
    IF @SPACES = 0 AND PATINDEX('% %', @UPASSWORD ) > 0 
       EXEC [master].[dbo].[sp_throwException] 901217;

    
    IF @SPECIAL > 0 
    BEGIN

      -- Count the characters 			
      EXEC [v3d].[sp_characterCount]
          @CHARACTERS = '!:;|[]{}@#$%^&*()-_+=''"\/?<>,.~`',
          @STRING = @PASSWORD,
          @COUNT = @COUNT OUTPUT

      -- ===========================================================
      -- Make sure we have the correct number of secial characters
      -- (901218) Minimum special chars
      -- ===========================================================
      IF @COUNT < @SPECIAL
        EXEC [master].[dbo].[sp_throwException] 901225, 'Special Characters', @SPECIAL;

    END
    
    -- ===============================================================
    -- We only need to count if there is a NUMERIC character minimum
    -- ==============================================================
    IF @NUMERIC > 0 
    BEGIN
        -- Count the characters 			
      EXEC [v3d].[sp_characterCount] 
          @CHARACTERS = '0123456789',
          @STRING = @PASSWORD,
          @COUNT = @COUNT OUTPUT

      -- ===========================================================
      -- Make sure we have the correct number of secial characters
      -- (901219) Minimum numerc chars
      -- ===========================================================
      IF @COUNT < @NUMERIC
        EXEC [master].[dbo].[sp_throwException] 901225, 'Numeric Characters', @NUMERIC;

    END
        
    -- ===========================================================
    -- We only need to count if there is a UPPER character minimum
    -- ===========================================================
    IF @UPPER > 0 
    BEGIN
        -- Count the characters 			
      EXEC [v3d].[sp_characterCount] 
          @CHARACTERS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
          @STRING = @PASSWORD,
          @COUNT = @COUNT OUTPUT

      -- ===========================================================
      -- Make sure we have the correct number of secial characters
      -- (901219) Minimum numerc chars
      -- ===========================================================
      IF @COUNT < @UPPER
        EXEC [master].[dbo].[sp_throwException] 901225, 'Uppercase Charaters', @UPPER ;

    END
     
    -- ===========================================================
    -- We only need to count if there is a LOWER character minimum
    -- ===========================================================
    IF @LOWER > 0 
    BEGIN

      -- Count the characters 			
      EXEC [v3d].[sp_characterCount] 
          @CHARACTERS = 'abcdefghijklmnopqrstuvwxyz',
          @STRING = @PASSWORD,
          @COUNT = @COUNT OUTPUT

      -- ===========================================================
      -- Make sure we have the correct number of secial characters
      -- (901219) Minimum lower chars
      -- ===========================================================
      IF @COUNT < @LOWER
        EXEC [master].[dbo].[sp_throwException] 901225, 'Lowercase Characeters', @LOWER  ;

    END
      

 
    DECLARE @CH AS CHAR
    DECLARE @TABLE AS TABLE 
    (
      ch  CHAR PRIMARY KEY,
      cnt  INT DEFAULT 0
    )
	
	  -- Set the length variable
	  SET @LENGTH = LEN(@UPASSWORD)
	
	  -- Set the starting index
	  SET @START = 1
	
	
	  --============================================================
	  -- Loop through the STRING to count the special chracters
	  --============================================================
	  WHILE @START <= @LENGTH
	  BEGIN 
        
      SET @CH = CONVERT(CHAR,SUBSTRING(@UPASSWORD,@START,1))

      IF EXISTS ( SELECT * FROM @TABLE WHERE [ch] = @CH )
        UPDATE @TABLE SET [cnt] = [cnt] + 1 WHERE [ch] = @CH
      ELSE
        INSERT INTO @TABLE ( ch ) VALUES ( @CH )
	
      SET @START = @START + 1

	  END -- END WHILE

    -- ======================================
    -- Maximum character repititions allowed 
    -- ======================================
    IF EXISTS ( SELECT * FROM @TABLE WHERE [cnt] > @REPITITION )
	    EXEC [master].[dbo].[sp_throwException] 901226, 'Character Repititions', @REPITITION ;

    DECLARE @RC AS INT

    SET @RC = 1

    IF @VALIDATE_ONLY = 0
    BEGIN


      -- ============================================================================================
      -- If the TTL is not present in the xml or is <= 0 then assume that the password do not expire
      -- ============================================================================================
      IF @TTL IS NULL OR @TTL <= 0
        INSERT [v3d].[UserPasswords] ( [login], [pwd] ) VALUES ( @LOGIN, @ENCRYPTED_PASSWORD )
      ELSE
         INSERT [v3d].[UserPasswords] ( [login], [pwd], [exp] ) VALUES ( @LOGIN, @ENCRYPTED_PASSWORD, DATEADD(day, @TTL, SYSDATETIMEOFFSET()) )
  
      SET @RC = @@ROWCOUNT

    END

    IF @TRANSCOUNT = 0 
      COMMIT TRANSACTION
        
    RETURN @RC
    
  END TRY
	BEGIN CATCH

 
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION
 

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertPassword'
	  
	  
	END CATCH
	  

END





GO
/****** Object:  StoredProcedure [v3d].[sp_insertUserProfile]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Sets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertUserProfile]
@ID NVARCHAR(256),
@RECORD XML OUTPUT
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @LOGIN NVARCHAR(32)
  DECLARE @NAME NVARCHAR(32)
  DECLARE @IDOC AS INTEGER
  DECLARE @PROFILE AS XML

  BEGIN TRY
 
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'UserManagement'
	 
      -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @LOGIN      = [login],
            @NAME       = [name],
            @PROFILE    = [profile]
          FROM OPENXML(@IDOC, N'/userprofile', 1) WITH 
          ( 
            [login]   nvarchar(16),
            [name]    nvarchar(64),
            [profile]  xml './profile'
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    

    -- ==============================================================================
    -- Make sure the login is specified. (900990) Login is missing or invalid
    -- ===============================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Login' 

    -- ===================================================================================
    -- Make sure the profile name is specified. (900990) Profile Name is missing or invalid
    -- ===================================================================================
    IF @NAME IS NULL OR LEN(@NAME) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Profile Name' 

    -- =====================================
    -- Make sure the profile was specified
    -- ====================================
    IF @PROFILE IS NULL
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Profile'
    ELSE
      SELECT @PROFILE = @PROFILE.query('./profile/*')

    -- ======================================================
    -- Make sure the user exists, (900991) Account not found
    -- ======================================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[Users] WHERE [login] = @LOGIN )
       EXEC [master].[dbo].[sp_throwNotFoundException]  'Account' 

    UPDATE [v3d].[UserProfiles] SET [profile] = @PROFILE WHERE [login] = @LOGIN AND [name] = @NAME

    IF @@ROWCOUNT = 0 
     INSERT [v3d].[UserProfiles] ( [login], [name], [profile] ) VALUES ( @LOGIN, @NAME, @PROFILE )
  
    RETURN @@ROWCOUNT
    
  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertUserProfile'
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_insertUserProfile] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertUserProfile] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_isAuthorized]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================================
-- Author:		  Mark Kontak 
-- Create date: 2/1/2010
-- Description:	Checks the contexts users permission mask  to make sure the 
--              administration bit is set. This is only done if the EnableDatabaseAuthorization
--              setting is true. 
--
--              Exceptions 
--
--              V3D0911     Context was not found
--              V3D0912     Context has expired
--              V3D0927     Invalid context (Missing context)
--              V3D0940     User does not have proper authorization
--
-- =======================================================================================================
CREATE PROCEDURE [v3d].[sp_isAuthorized]
(
@ID             NVARCHAR(256),                    -- Context ID
@LOGIN          NVARCHAR(16) = '',                -- Login performing action ( Only used in not blank )
@MASK           NVARCHAR(32) = 'Administration'   -- Mask to check
)
AS
BEGIN

  
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Value to hold setting
  DECLARE @VALUE          NVARCHAR(32)
  DECLARE @EXP_DATE       DATETIMEOFFSET(7)
  DECLARE @IDLEN          INT
  DECLARE @MESSAGE        NVARCHAR(MAX)
  DECLARE @CONTEXT_LOGIN  NVARCHAR(16)



  BEGIN TRY
      
      
    SELECT @ID = RTRIM(@ID), @IDLEN = LEN(@ID), @VALUE = 'FALSE', @CONTEXT_LOGIN = ''
   
    -- ============================================================
    -- Check for an invalid context - (900990) Context missing or invalid
    -- ============================================================
    IF @ID IS NULL OR @IDLEN = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Context'
    

   
    
    -- ====================================================================================
    -- Check for a PRE-DEFINED ID for use by services so that they do not need to login
    -- ====================================================================================
    IF @IDLEN < 40 OR (@IDLEN >= 40 AND SUBSTRING(@ID,1,40) <> 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893') 
    BEGIN
    
      -- Get Context Login
      SELECT  @CONTEXT_LOGIN = [v3d].[Contexts].[login], 
              @EXP_DATE = [v3d].[Contexts].[exp] 
               FROM [v3d].[Contexts] 
              WHERE [v3d].[Contexts].[id] = CONVERT(UNIQUEIDENTIFIER,@ID) 
    
      -- ========================================================
      -- Make sure the CONTEXT exists 
      -- (900991) Context not found
      -- ========================================================
      IF @@ROWCOUNT = 0 
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Context'

      
      -- =================================================================
      -- Make sure the CONTEXT has not expired - (900993) Context expired
      -- =================================================================
      IF @EXP_DATE < SYSDATETIMEOFFSET()
       EXEC [master].[dbo].[sp_throwException] 900993, 'Context' 
     

   
      -- ========================================================================================================
  	  -- If the LOGIN was specified and it does not equal the context login then make sure the admin bit is set
  	  -- ========================================================================================================
  	  IF @CONTEXT_LOGIN <> @LOGIN 
  	  BEGIN
    	
        -- ===================================================================================
        -- If no rows came back then the user is not authorized - (901201) User not authorized
        -- ====================================================================================
        IF  NOT EXISTS ( SELECT *        FROM [v3d].[Users] 
                                   INNER JOIN [v3d].[Roles] 
                                           ON [v3d].[Roles].[name] = [v3d].[Users].[role] 
                                        WHERE [login] = @CONTEXT_LOGIN AND ( [role] = 'Service' OR [v3d].[fn_isRightSet]([v3d].[Roles].[prms],@MASK) > 0 ) ) 
          EXEC [master].[dbo].[sp_throwException] 901201
       

      END -- IF LEN(@LOGIN) > 0 AND @CONTEXT_LOGIN <> @LOGIN 
    
    END
    ELSE
    BEGIN
      DECLARE @START AS INT
      SET @START = PATINDEX('%|%', @ID )
     
      SET @CONTEXT_LOGIN = 'Internal'
      
      IF @START > 0 
      BEGIN
       SET @START = @START + 1
       SET @CONTEXT_LOGIN = SUBSTRING(@ID, @START, @IDLEN - @START)
      END
      
    END

    -- Returns 1 (OK)
    RETURN 1

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_isAuthrozied'

  END CATCH
  
    
END





GO
GRANT EXECUTE ON [v3d].[sp_isAuthorized] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_isAuthorized] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_purgeContexts]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================
-- Author:		  Mark Kontak
-- Create date: 12/01/2010
-- Description:	Purge all contexts (used for testing)
-- ===================================================
CREATE PROCEDURE [v3d].[sp_purgeContexts]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DELETE FROM [v3d].[Contexts]
  
  RETURN @@ROWCOUNT
 END





GO
GRANT EXECUTE ON [v3d].[sp_purgeContexts] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_purgeDatabase]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/09/2010
-- Description:	Purges the database of all licenses, contexts and no-system users
-- ===================================================================================
CREATE PROCEDURE  [v3d].[sp_purgeDatabase]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DELETE FROM [v3d].[Contexts]
  DELETE FROM [v3d].[Roles] WHERE ( [set] & 1) <> 1
  
  DECLARE @LOGIN AS NVARCHAR(20)
  
  -- Non System Users
  DECLARE USERS_CURSOR CURSOR FOR SELECT [login] FROM [v3d].[Users] WHERE ( [set] & 1) <> 1
  
  OPEN USERS_CURSOR
  
  FETCH NEXT FROM USERS_CURSOR INTO @LOGIN
  
  WHILE @@FETCH_STATUS = 0
  BEGIN
  

    DELETE FROM [v3d].[UserPasswords] WHERE [login] = @LOGIN
    DELETE FROM [v3d].[UserProfiles] WHERE [login] = @LOGIN
    DELETE FROM [v3d].[Users] WHERE [login] = @LOGIN
    
    FETCH NEXT FROM USERS_CURSOR INTO @LOGIN
  END
  
  CLOSE USERS_CURSOR
  DEALLOCATE USERS_CURSOR
  
  
  
END





GO
GRANT EXECUTE ON [v3d].[sp_purgeDatabase] TO [v3dsa] AS [dbo]
GO
DENY EXECUTE ON [v3d].[sp_purgeDatabase] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_reindexTable]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================
-- Author:		  Mark Kontak
-- Create date: 10/15/2013
-- Description:	Reindexes all tables in the database
-- =======================================================
CREATE PROCEDURE [v3d].[sp_reindexTable]
(
@TABLE AS NVARCHAR(128) = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    DECLARE @DBTABLE AS NVARCHAR(128)

    IF @TABLE IS NULL OR LEN(@TABLE) = 0 
    BEGIN

      -- =================================================================
      -- Get the actual table name since I will allow case insenstivity
      -- ==================================================================
      IF NOT EXISTS (SELECT * FROM sys.sysobjects AS Tables WHERE [type] = 'U' AND [name] = @TABLE )
        EXEC [master].[dbo].[sp_throwNotFoundException] 'Table', @TABLE;
 
      PRINT '================================================================================================='
      PRINT 'Reindexing [System].[v3d].[' + @TABLE + ']'

      SET @DBTABLE = 'v3d.' + @TABLE

      DBCC DBREINDEX ( @DBTABLE, ' ', 70)
     
      PRINT ''
      PRINT '================================================================================================='

    END
    ELSE
    BEGIN
      DECLARE TABLE_CURSOR CURSOR FOR SELECT  [name] FROM sys.sysobjects AS Tables WHERE [type] = 'U'

      PRINT '================================================================================================='

      OPEN TABLE_CURSOR


      FETCH  NEXT  FROM TABLE_CURSOR INTO @TABLE

      WHILE @@FETCH_STATUS = 0
      BEGIN
  
        PRINT 'Reindexing [System].[v3d].[' + @TABLE + ']'

        SET @DBTABLE = 'v3d.' + @TABLE

        DBCC DBREINDEX ( @DBTABLE, ' ', 70)

        PRINT ''
        PRINT '================================================================================================='

        FETCH  NEXT  FROM TABLE_CURSOR INTO @TABLE

      END

      CLOSE TABLE_CURSOR
      DEALLOCATE TABLE_CURSOR
	  END

 END TRY
 BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_reindexTable'
 END CATCH
END


GO
GRANT EXECUTE ON [v3d].[sp_reindexTable] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_releaseContext]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================================================
-- Author:		Mark Kontak
-- Create date: 6/2/208
-- Description:	Release the context from the system. You may specify the context ID or the users login
-- ================================================================================================================================
CREATE PROCEDURE [v3d].[sp_releaseContext] 
( 
@ID AS NVARCHAR(256) = '',     -- ID of context to release
@LOGIN AS NVARCHAR(16) = '',   -- Login
@FORCE AS BIT = 0              -- Flag used to force a release (ignores reference count)             
) 
AS
BEGIN
  -- SET NOCOUNT ON added to prevent extra result sets from
  -- interfering with SELECT statements.
  SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)
  DECLARE @RETURN AS INT

  -- Initial return value is 0
  SET @RETURN = 0
  SET @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)

  BEGIN TRY
  
	  -- ================================================================================
	  -- If the ID and LOGIN are not specified we have an error. (900990) Context is missing or invalid
	  -- =================================================================================
	  IF (@ID IS NULL OR LEN(@ID) = 0) AND (@LOGIN IS NULL OR LEN(@LOGIN) = 0)
	   EXEC [master].[dbo].[sp_throwInvalidDataException] 'Context'


    -- ==========================================================================================
    -- Delete the context based on the parameters passwed in. If the login was specified then
    -- we delete based on the login otherwise we delete based on the context id.
    -- ===========================================================================================
    IF LEN(@LOGIN) > 0
    BEGIN
    
      -- ========================================================
      -- Only delete the context if thea account is not internal
      -- ========================================================
      IF [v3d].[fn_isInternalAccount](@LOGIN) = 0 
      BEGIN
      
        IF LEN(@ID) > 0
        BEGIN
        
          -- ====================================================================================================
          -- make sure the user associated with the context id supplied is authorized to delete the context
          -- of another user. Only need user management if the login is begin released by another user.
          -- ====================================================================================================
          EXEC [v3d].[sp_isAuthorized] 
            @ID = @ID,
            @MASK = 'UserManagement',
            @LOGIN = @LOGIN
            
          -- ===============================================================================
          -- If the force flag is set then it is assumed that the users context needs to be 
          -- released do to a race condition so we are going to ignore the reference count
          -- ===============================================================================  
          IF @FORCE = 1 
          BEGIN
            -- Delete the context based on the login 
            DELETE FROM [v3d].[Contexts] WHERE [login] = @LOGIN 
            
            -- Return row count
            SET @RETURN = @@ROWCOUNT

          END
            
        END
        
        --- Decrement the Context
        UPDATE [v3d].[Contexts] SET [ref] = [ref] - 1 WHERE [login] = @LOGIN
        
        
        IF @@ROWCOUNT > 0
        BEGIN
  
          -- Delete the context based on the login  and if the reference count is 0
          DELETE FROM [v3d].[Contexts] WHERE [login] = @LOGIN AND [ref] <= 0

          -- Return row count
          SET @RETURN = @@ROWCOUNT

        END
        
      END
      
    END
    ELSE 
    BEGIN
    
      -- Get the login associated with the context
      SELECT @LOGIN = [login] FROM [v3d].[Contexts] WHERE [id] = @ID 
      
      -- ========================================================
      -- Only delete the context if thea account is not internal
      -- ========================================================
      IF [v3d].[fn_isInternalAccount](@LOGIN) = 0 
      BEGIN

        --- Decrement the Context
        UPDATE [v3d].[Contexts] SET [ref] = [ref] - 1 WHERE [login] = @LOGIN
        
        -- Delete the context based on the login  and if the reference count is 0
        DELETE FROM [v3d].[Contexts] WHERE [login] = @LOGIN AND [ref] <= 0
   
           -- Return row count
        SET @RETURN = @@ROWCOUNT
   
      END
      
    END
  	  	
    -- ==============================================================
    -- Only insert user history if the there was a context deleted
    -- ============================================================
    IF @RETURN > 0
    BEGIN

  	  -- Add audit entry for successful action
      EXEC [Auditing].[v3d].[sp_insertUserHistory]
       @LOGIN = @LOGIN, 
       @ACTION = N'logout', 
       @USER = @CONTEXT_LOGIN, 
       @STATUS = 1

    END
    --ELSE
    --BEGIN

  	 -- -- Add audit entry for failed action
    --  EXEC [Auditing].[v3d].[sp_insertUserHistory]
    --   @LOGIN = @LOGIN, 
    --   @ACTION = N'logout', 
    --   @STATUS = 0,
    --   @USER = @CONTEXT_LOGIN, 
    --   @MESSAGE = 'No context to release'

    --END

    RETURN @RETURN
    
  END TRY
  BEGIN CATCH
  
    DECLARE @ERROR_MESSAGE NVARCHAR(4000);


    -- Get the exception information o that we can rethrow
    SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()
  
    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'logout', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_releaseContext'
 END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_releaseContext] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_releaseContext] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_renewContext]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		Mark Kontak	
-- Create date: 6/2/2008	
-- Description:	Renews the login context if it exists
--
--              Exceptions:
--              901226         Missing or invalid context
--              901230         Unknown context
--              901227         Context expired
--              901007         License expired
--              901236         Account expired
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_renewContext] 
( 
@ID AS NVARCHAR(256),               -- Context id to be renewed
@TTL AS INTEGER = 0,                -- New time to live 
@CHECK_EXPIRATION BIT = 1           -- Check the license and user expiration flag
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
	
  DECLARE @LOGIN AS NVARCHAR(32)
  DECLARE @LOGOUT AS BIT
  
  SET @LOGOUT = 0

  BEGIN TRY
  
  
	  -- =======================================================================
	  -- If the CONTEXT ID is blank then it is invalid, (900990) Context is missing or invalid
	  -- =======================================================================
	  IF @ID IS NULL OR LEN(@ID) = 0 
	   EXEC [master].[dbo].[sp_throwInvalidDataException] 'Context' 


	  -- Row count
	  DECLARE @ROWCOUNT AS INTEGER
    DECLARE @EXPIRES AS DATETIMEOFFSET
    DECLARE @TYPE AS NVARCHAR(16)
    DECLARE @CONTEXT_TTL AS INTEGER
    DECLARE @KEY AS NVARCHAR(256)
    DECLARE @IS_SERVICE_ACCOUNT AS BIT
    DECLARE @CURRENT AS DATETIMEOFFSET
      
  
    --- Get information from the context 
    SELECT @EXPIRES = [exp], @CONTEXT_TTL = [ttl], @KEY = [lic], @LOGIN = [login], @CURRENT = SYSDATETIMEOFFSET() 
    FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[id] = @ID

    -- =================================
    -- Verify that the context exist
    -- 900991  Context not found
    -- =================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Context'

    -- Determine if the account is a service account
    SET @IS_SERVICE_ACCOUNT = [v3d].[fn_isServiceAccount](@LOGIN)

	  
	  -- ==============================================================================
	  -- If the account is not a service account then skip the checking and just renew
	  -- =============================================================================
	  IF @IS_SERVICE_ACCOUNT = 0
	  BEGIN 

	    -- ===================================================
      -- At this point any exception should force a logout
      -- ====================================================
      SET @LOGOUT = 1
  
      -- =====================================================================================
      -- Check the system to make sure that it hase not been disabled or needs to be disabled
      -- =====================================================================================
      EXEC  [System].[v3d].[sp_checksystem]   
      

      -- ==================
      -- Check the license 
      -- ===================
      EXEC [v3d].[sp_validateLicense]  
        @KEY = @KEY,
        @CHECK_EXPIRED = @CHECK_EXPIRATION

   	
      -- ============================================================================================
      -- Check if the user has expired and if so  delete the context (900993) Account expired
      -- ============================================================================================
	    IF @CHECK_EXPIRATION = 1 AND EXISTS  ( SELECT * FROM [v3d].[users] WHERE [login] = @LOGIN AND [exp] < SYSDATETIMEOFFSET() )
         EXEC [master].[dbo].[sp_throwException] 900993,  'Account'
 
      -- ===================================================================================
      -- Check if the context has expired and if so delete the context and raise an error
      -- indicating that the (900993) context has expired
      -- ===================================================================================
      IF @EXPIRES < @CURRENT 
        EXEC [master].[dbo].[sp_throwException] 900993, 'Context'
                  
      
    END -- END ... If this is not a servce account
      
    -- Set the TTL to the current setting if none was specfied
    IF @TTL = 0 
      SET @TTL = @CONTEXT_TTL
      
    -- Update the context
    UPDATE [v3d].[Contexts] SET v3d.[Contexts].[mod] = SYSDATETIMEOFFSET(), [v3d].[Contexts].[ttl] = @TTL WHERE [v3d].[Contexts].id = @ID

    RETURN @@ROWCOUNT

	END TRY
	BEGIN CATCH

    -- =========================
    -- If LOGOUT flag is true 
    -- =========================
    IF @LOGOUT = 1 
    BEGIN

    	
      DECLARE @ERROR_MESSAGE NVARCHAR(4000);

      -- Get the exception information o that we can rethrow
      SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()

      -- Delete Context
	    DELETE [v3d].[Contexts]  WHERE [v3d].[Contexts].id = @ID    

      -- Add audit entry for failed action
      EXEC [Auditing].[v3d].[sp_insertUserHistory]  
          @LOGIN = @LOGIN, 
          @ACTION = N'Logout', 
          @STATUS = 1,
          @MESSAGE = @ERROR_MESSAGE

    END

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_renewContext'
  
	END CATCH


  
END





GO
GRANT EXECUTE ON [v3d].[sp_renewContext] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_renewContext] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateRole]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- ==========================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/24/2009
-- Description:	Returns the contexts table in xml format includes the license and the 
--
--              Exceptions:
--
--              901237    Must specify role name.
--
-- NOTE:  Currently all roles are added during the creation of the Security database as of
--        Console 3.5. The requirement to add roles by an administrator was removed in 
--        Console 3.5. Will keep this procedure in-case the requirement is added back into 
--        the Security Subsystem.
-- ============================================================================================
CREATE PROCEDURE [v3d].[sp_updateRole]
(
@ID NVARCHAR(256) = '',            -- Context ID
@RECORD XML                       -- User XML 
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @NAME NVARCHAR(32)             
  DECLARE @DESCRIPTION NVARCHAR(64)
  DECLARE @SET BIGINT
  DECLARE @RIGHTS BIGINT
  DECLARE @PWDEXP INT
  DECLARE @IDOC AS INTEGER

	BEGIN TRY
	    

    -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'Service'


    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @NAME         = [name],
            @DESCRIPTION  = [desc],
            @SET          = [set],
            @RIGHTS       = [prms]
          FROM OPENXML(@IDOC, N'/role', 1) WITH 
          ( 
            [name]    nvarchar(32),
            [set]     bigint,
            [desc]    nvarchar(64),
            [prms]    bigint
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- ========================================================================================================
    -- Make sure that a name was specified or we cannot add the role. (900990) Role name is missing or invalid
    -- ========================================================================================================
    IF @NAME IS NULL OR  LEN(@NAME) = 0 
  	  EXEC [master].[dbo].[sp_throwInvalidDataException] 'Role Name'

    DECLARE @RC AS INT

    SET @RC = 0

    -- Update the Role
    UPDATE [v3d].[Roles] 
      SET [prms] = ISNULL(@RIGHTS,[prms]), 
          [desc] = ISNULL(@DESCRIPTION,[desc]),
          [set]  = ISNULL(@SET,[set])
          WHERE [name] = @NAME

    
    RETURN @RC
  
 	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateRole'
	  
	END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_updateRole] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateRole] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:	    Mark Kontak
-- Create date: 1/29/2010
-- Description:	Updates the user
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateUser]
(
@ID     NVARCHAR(256),            -- Context ID
@RECORD XML OUTPUT                -- User XML 
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

  DECLARE @IDOC AS INTEGER
  DECLARE @LOGIN AS NVARCHAR(16)
  DECLARE @NAME AS NVARCHAR(64)
  DECLARE @EXPDATE  AS NVARCHAR(64)
  DECLARE @AUTH AS NVARCHAR(64)
  DECLARE @AUTHDOMAIN AS NVARCHAR(64)
  DECLARE @AMASK AS INT
  DECLARE @SET AS BIGINT
  DECLARE @STATE AS INT
  DECLARE @PTMP AS NVARCHAR(32)
  DECLARE @ENCRYPTED_PASSWORD AS NVARCHAR(32)
  DECLARE @RETURN AS INT


  SELECT @CONTEXT_LOGIN = [v3d].[fn_getLogin](@ID)

	BEGIN TRY
	    
	  -- Read the usr xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @LOGIN      = [login],
            @NAME       = [name],
            @EXPDATE    = [exp],
            @STATE      = [state],
            @AUTH       = [auth],
            @AUTHDOMAIN = [dom],
            @AMASK      = [apps],
            @SET        = [set],
            @PTMP       = [ptmp]
          FROM OPENXML(@IDOC, N'/user', 1) WITH 
          ( 
            [login]   nvarchar(16),
            [name]    nvarchar(64),
            [exp]     nvarchar(64),
            [state]   int,
            [auth]    nvarchar(64),
            [dom]     nvarchar(64),
            [apps]    int,
            [set]     bigint,
            [ptmp]    nvarchar(32)
          )


	    
	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].sp_isAuthorized 
        @ID = @ID,
        @MASK = 'UserManagement'
        
    -- ==============================================================
    -- Check for an invalid login (900990) Login missing or invalid
    -- ===============================================================
    IF @LOGIN IS NULL OR ( @LOGIN IS NOT NULL AND LEN(RTRIM(@LOGIN)) = 0 )
  	  EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login' 

    -- =========================================================
    -- If the users exists raise an error (900991) Account not found
    -- =========================================================
    IF ( SELECT COUNT(*) FROM [v3d].[Users] WHERE [login] = @LOGIN ) = 0 
   	  EXEC [master].[dbo].[sp_throwNotFoundException] 'Account' 

    -- ===============================================
    -- Make sure if the state is supplied it is valid
    --  (900991) State: Not Found
    -- ===============================================
    IF @STATE IS NOT NULL AND NOT EXISTS ( SELECT * FROM [v3d].[UserStates] WHERE [value] = @STATE ) 
   	  EXEC [master].[dbo].[sp_throwNotFoundException] 'State'  

    -- ===============================================
    -- Make sure if the password template is supplied it is valid
    --  (900991) Password Template: Not Found
    -- ===============================================
    IF @PTMP IS NOT NULL AND NOT EXISTS ( SELECT * FROM [v3d].[PasswordTemplates] WHERE [id] = @PTMP ) 
   	  EXEC [master].[dbo].[sp_throwNotFoundException]  'Password Template' 

    -- ===============================================
    -- Make sure if the password template is supplied it is valid
    --  (900991) Password Template: Not Found
    -- ===============================================
    IF @AUTH IS NOT NULL AND NOT EXISTS ( SELECT * FROM [v3d].[AuthenticationTypes] WHERE [id] = @AUTH ) 
   	  EXEC [master].[dbo].[sp_throwNotFoundException]  'Authentication Type'  

    -- ==============================================================================================
    -- Cannot update a system account unless you are service 
    -- (901204) Cannot update system account
    -- ==============================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Users] WHERE [login] = @LOGIN AND ( [set] & 1) = 1 )
    BEGIN
      
      BEGIN TRY
        EXEC [v3d].[sp_isAuthorized]
          @ID = @ID,
          @MASK = 'Service'
      END TRY
      BEGIN CATCH
        EXEC [master].[dbo].[sp_throwException] 901204     
      END CATCH

    END

      
    -- ===============================================================================================
    -- Unless the person performing the update is a service account the settings value cannot be set
    -- ===============================================================================================
    IF @SET IS NOT NULL 
    BEGIN
      BEGIN TRY
        EXEC [v3d].[sp_isAuthorized]
          @ID = @ID,
          @MASK = 'Service'
      END TRY
      BEGIN CATCH

        -- ======================================================
        -- Will allow the privledge setting to be set by an admin
        -- ======================================================
        IF @SET IS NOT NULL 
          SET @SET = ( @SET & 2 )
        ELSE
          SET @SET = NULL

      END CATCH
    END

    --============================================================================
    -- Update the user
    --============================================================================
    UPDATE [v3d].Users SET  [name]    = ISNULL(@NAME,[name]), 
                            [state]   = ISNULL(@STATE,[state]),
                            [auth]    = ISNULL(@AUTH,[auth]), 
                            [dom]     = ISNULL(@AUTHDOMAIN,[dom]), 
                            [apps]    = ISNULL(@AMASK,[apps]),
                            [exp]     = @EXPDATE,
                            [ptmp]    = ISNULL(@PTMP,[ptmp]),
                            [set]     = ISNULL(@SET, [set])
                            WHERE [login] = @LOGIN

    -- =========================================================================================
    -- Record the ROW COUNT of the UPDATE to be returned. It will be 1 if the user was updated
    -- ==========================================================================================
    SET @RETURN = @@ROWCOUNT
    
    SET @LOGIN = ISNULL(@LOGIN,'')
    SET @AMASK = ISNULL(@AMASK,0)
    


    -- Add audit entry for successful action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN   = @LOGIN, 
     @ACTION  = N'update', 
     @STATUS  = 1,
     @USER    = @CONTEXT_LOGIN, 
     @EXT     = @RECORD

     

    RETURN @RETURN



	END TRY
	BEGIN CATCH

    DECLARE @ERROR_MESSAGE NVARCHAR(4000);
 

    -- Get the exception information o that we can rethrow
    SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()


     
   SET @LOGIN = ISNULL(@LOGIN,'')

    -- Add audit entry for failed action
    EXEC [Auditing].[v3d].[sp_insertUserHistory]  
     @LOGIN = @LOGIN, 
     @ACTION = N'update', 
     @STATUS = 0,
     @USER = @CONTEXT_LOGIN, 
     @MESSAGE = @ERROR_MESSAGE

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateUser'
	  
	END CATCH
  

  
  
END





GO
GRANT EXECUTE ON [v3d].[sp_updateUser] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateUser] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateUserProfile]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:	    Mark Kontak
-- Create date: 1/29/2010
-- Description:	Updates the user
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateUserProfile]
(
@ID     NVARCHAR(256) = '',       -- Context ID
@RECORD XML OUTPUT                -- User XML 
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
  DECLARE @LOGIN NVARCHAR(32)
  DECLARE @NAME NVARCHAR(32)
  DECLARE @IDOC AS INTEGER
  DECLARE @PROFILE AS XML
 
  BEGIN TRY
 
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'UserManagement'
	 
      -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @LOGIN        = [login],
            @NAME         = [name],
            @PROFILE     = [profile]
          FROM OPENXML(@IDOC, N'/userprofile', 1) WITH 
          ( 
            [login]     nvarchar(16),
            [name]      nvarchar(64),
            [profile]   xml './profile'
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    

    -- ==============================================================================
    -- Make sure the login is specified. (900990) Login is missing or invalid
    -- ===============================================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login'

    -- ===================================================================================
    -- Make sure the profile name is specified. (900990) Profile Name is missing or invalid
    -- ===================================================================================
    IF @NAME IS NULL OR LEN(@NAME) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Profile Name'

    -- =====================================
    -- Make sure the profile was specified
    -- ====================================
    IF @PROFILE IS NULL
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Profile' 
    ELSE
      SELECT @PROFILE = @PROFILE.query('./profile/*')

    -- Make sure the user exists (900991) Account not found
    IF NOT EXISTS ( SELECT * FROM [v3d].[Users] WHERE [login] = @LOGIN )
       EXEC [master].[dbo].[sp_throwNotFoundException]  'Account' 

    -- =================================================================
    -- Updates the config
    -- =================================================================
    UPDATE [v3d].[UserProfiles] SET [profile] = @PROFILE WHERE [login] = @LOGIN AND [name] = @NAME
  
    RETURN @@ROWCOUNT
    
  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateUserProfile'
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updateUserProfile] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateUserProfile] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_validateContext]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/16/2011
-- Description: Validates the context/lic and user
-- =============================================================================================
CREATE PROCEDURE [v3d].[sp_validateContext]
@ID NVARCHAR(256),          -- Context Id
@LOGIN NVARCHAR(32) = '',   -- Login
@CHECK_EXPIRATION BIT = 1   -- Flag to indicate that we are checking the license and user expiration
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
  BEGIN TRY
  
    DECLARE @LIC AS NVARCHAR(256)


    -- ===========================
    -- Check for the god context
    -- ==========================
    IF @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893' 
      RETURN 1

    -- ===========================================
    --- Make sure the cpontext is was specified
    -- (900990) Context: Missing or Invalid
    -- ============================================
    IF @ID IS NULL OR LEN(@ID) = 0 
       EXEC [master].[dbo].[sp_throwInvalidDataException] 'Context'
       
 
    -- ==========================================================================================================
    -- If the login was specified then make sure that the context/login pair go together otherwise
    -- just get the login from the context entry.
    -- ==========================================================================================================
    IF LEN(@LOGIN) > 0
      SELECT @LIC = [lic] FROM [v3d].[Contexts] WHERE [id] = @ID AND [login] = @LOGIN 
    ELSE
      SELECT @LIC = [lic], @LOGIN = [login] FROM [v3d].[Contexts] WHERE [id] =  @ID
 

    -- ===============================================================
    -- Context not found (900991) Context not found
    -- ===============================================================
    IF @@ROWCOUNT = 0
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Context'
   
    -- ===================================================================
    -- Validate the context's license iff we are not a service account
    -- ===================================================================
    IF [v3d].[fn_isServiceAccount](@LOGIN) = 0
      EXEC [v3d].[sp_validateLicense] 
        @KEY = @LIC,
        @CHECK_EXPIRED = @CHECK_EXPIRATION


    DECLARE @EXP AS DATETIMEOFFSET(7)

    SELECT @EXP = [exp] FROM [v3d].[Users] WHERE [login] = @LOGIN

    -- =================
    -- (900991) Account not found
    -- =================
    IF @@ROWCOUNT = 0 
     EXEC [master].[dbo].[sp_throwNotFoundException]  'Account'

    -- ============================
    -- (900993) Account expired 
    -- ==========================
    IF @EXP IS NOT NULL AND (  @CHECK_EXPIRATION = 1 AND  @EXP < SYSDATETIMEOFFSET() )
      EXEC [master].[dbo].[sp_throwException] 900993, 'Account' 
       

 	END TRY
	BEGIN CATCH

     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_validateContext'
	  
	END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_validateContext] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_validateContext] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_windowsAuthentication]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_windowsAuthentication]
	@domain [nvarchar](4000),
	@login [nvarchar](4000),
	@password [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Security].[StoredProcedures].[sp_windowsAuthentication]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDatabase]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/20/2013
-- Description:	Returns the xml representation of the table/columns for this database
-- ===================================================================================
CREATE FUNCTION [v3d].[fn_getDatabase]
(
)
RETURNS XML
AS
BEGIN
	-- Declare the return variable here
	DECLARE @RESULTS AS XML
  DECLARE @NAME AS NVARCHAR(32)

  SET @NAME = 'Security'
  
  SELECT @RESULTS = 
    ( SELECT @NAME AS [name], [filename], 
                      ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'MS_Description' AND [class] = 0 ) AS [desc], 
      ( SELECT  [name], 
                ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'MS_Description' AND [major_id] = Tables.id AND [minor_id] = 0 ) AS [desc],
                ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'Resource' AND [major_id] = Tables.id ) AS [res], 
                ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'ServicePermissions' AND [major_id] = Tables.id ) AS [sp],
	      ( SELECT [name], [length],
		      ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'MS_Description' AND [major_id] = Columns.id AND [minor_id] = Columns.colid ) AS [desc],
            ( SELECT [value] FROM sys.extended_properties WHERE [name] = 'ColumnHeader' AND [major_id] = Columns.id AND [minor_id] = Columns.colid ) AS [hdr]
              FROM sys.syscolumns AS Columns WHERE Columns.id = Tables.id FOR XML RAW('column'), TYPE, ROOT('columns') ) 
                FROM sys.sysobjects AS Tables WHERE [type] = 'U' FOR XML RAW('table'), TYPE, ROOT('tables') ) 
                  FROM master.sys.sysdatabases AS Databases WHERE [name] = @NAME FOR XML RAW('database') )

	-- Return the result of the function
	RETURN @RESULTS

END


GO
GRANT EXECUTE ON [v3d].[fn_getDatabase] TO [v3dsa] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getExpirationDate]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/13/2010  
-- Description:	Calculates the expiration by adding the TTL to the DATE specified using the units
--              specified in the setting
-- ===============================================================================================
CREATE FUNCTION [v3d].[fn_getExpirationDate]
(
@DATE AS DATETIMEOFFSET(7),   -- Starting date/time
@TTL AS INT,                  -- Time to live
@SETTING AS NVARCHAR(32)      -- Setting to use
)
RETURNS DATETIMEOFFSET(7)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @EXPIRES AS DATETIMEOFFSET(7)
	DECLARE @UNITS AS NVARCHAR(32)

  -- Get the UNITS but default to minute if not found
  SET @UNITS = LOWER([v3d].[fn_getSetting](@SETTING,'minute'))

  IF @UNITS = 'day'
    SET @EXPIRES = DATEADD(day, @TTL, @DATE )
  ELSE IF @UNITS = 'minute'
    SET @EXPIRES = DATEADD(minute, @TTL, @DATE )
  ELSE IF @UNITS = 'second'
    SET @EXPIRES = DATEADD(second, @TTL, @DATE )
  ELSE IF @UNITS = 'month'
    SET @EXPIRES = DATEADD(month, @TTL, @DATE )
  ELSE IF @UNITS = 'year'
    SET @EXPIRES = DATEADD(year, @TTL, @DATE )
  
  
	-- Return the result of the function
	RETURN @EXPIRES

END





GO
GRANT EXECUTE ON [v3d].[fn_getExpirationDate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getExpirationDate] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getLogin]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================
-- Author:		  Mark Kontak
-- Create date: 1/2/2013
-- Description:	Gets the login asscoaited with the context id
-- ================================================================
CREATE FUNCTION [v3d].[fn_getLogin] 
(
@ID NVARCHAR(256)
)
RETURNS NVARCHAR(32)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @LOGIN AS NVARCHAR(32)

  SET @LOGIN = ''


  IF @ID IS NOT NULL AND LEN(@ID) > 0 
  BEGIN
    IF @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893' 
      SET @LOGIN = 'V3D-System'
    ELSE
      SELECT @LOGIN = [login] FROM [v3d].[Contexts] NOLOCK WHERE [id] = CONVERT(uniqueidentifier,@ID)
  END

	-- Return the result of the function
	RETURN @LOGIN

END



GO
GRANT EXECUTE ON [v3d].[fn_getLogin] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getLogin] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSetting]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================
-- Author:		  Maek Kontak 
-- Create date: 11/02/2009
-- Description:	Gets the security system setting value
-- ====================================================
CREATE FUNCTION [v3d].[fn_getSetting] 
(
@NAME AS NVARCHAR(32),
@DEFAULT AS NVARCHAR(512) = '' 
)
RETURNS NVARCHAR(512)
AS
BEGIN

  DECLARE @RETURN AS NVARCHAR(32)

  SET @RETURN = @DEFAULT

  SET @RETURN = [System].[v3d].[fn_getSetting](@NAME,@DEFAULT)

  RETURN @RETURN

END





GO
/****** Object:  UserDefinedFunction [v3d].[fn_getState]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================
-- Author:		  Mark Kontak
-- Create date: 08/18/2010
-- Description:	Gets the state using the specified id
-- ==============================================================
CREATE FUNCTION [v3d].[fn_getState]
(
@ID NVARCHAR(16)
)
RETURNS SMALLINT
AS
BEGIN
  
  --- Returns the selected value 
	RETURN ( SELECT [value] FROM [v3d].[UserStates] NOLOCK WHERE [id] = @ID )

END





GO
GRANT EXECUTE ON [v3d].[fn_getState] TO [v3dsa] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getVersion]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================================
-- Author:		  Mark Kontak
-- Create date: 06/10/2010
-- Description:	Local functaion that calls though to the [System] function
-- ===========================================================================
CREATE FUNCTION [v3d].[fn_getVersion]
(
@OBJECT NVARCHAR(32)
)
RETURNS NVARCHAR(16)
AS
BEGIN

  DECLARE @VERSION AS NVARCHAR(32)

 
  SELECT @VERSION = [version] FROM [v3d].[Versions] NOLOCK WHERE [object] = @OBJECT

  RETURN ISNULL(@VERSION,'')

END





GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3dsa] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isInternalAccount]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================
-- Author:		  Mark Kontak
-- Create date: 12/17/2010
-- Description: Determines if the login is an internal account
-- ==============================================================
CREATE FUNCTION [v3d].[fn_isInternalAccount]
(
@LOGIN AS NVARCHAR(64)
)
RETURNS BIT
AS
BEGIN

  RETURN ( SELECT COUNT(*) FROM [v3d].[Users] WHERE [login] = @LOGIN AND ( [set] & 1 )  = 1  )

END





GO
GRANT EXECUTE ON [v3d].[fn_isInternalAccount] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isInternalAccount] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isRightSet]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================
-- Author:		  Mark Kontak
-- Create date: 07/26/2011
-- Description:	Returns the BITMASK if set 0 otherwise
-- ======================================================
CREATE FUNCTION [v3d].[fn_isRightSet] 
(
@MASK AS BIGINT,
@NAME AS NVARCHAR(32)
)
RETURNS TINYINT
AS
BEGIN
	-- Declare the return variable here
  DECLARE @RETURN AS TINYINT

  
  IF EXISTS ( SELECT * FROM [v3d].[Rights] WHERE [name] = @NAME AND ( @MASK & [mask] ) = [mask] )
    SET @RETURN = 1
  ELSE
    SET @RETURN = 0

  RETURN @RETURN


END





GO
GRANT EXECUTE ON [v3d].[fn_isRightSet] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isRightSet] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isServiceAccount]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================
-- Author:		  Mark Kontak
-- Create date: 12/17/2010
-- Description: Determines if the login is a system account
-- ==============================================================
CREATE FUNCTION [v3d].[fn_isServiceAccount]
(
@LOGIN AS NVARCHAR(64)
)
RETURNS BIT
AS
BEGIN

  RETURN ( SELECT COUNT(*) FROM [v3d].[Users] WHERE [login] = @LOGIN AND [role] = 'Service' )

END





GO
GRANT EXECUTE ON [v3d].[fn_isServiceAccount] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isServiceAccount] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isState]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================================
-- Author:		  Mark Kontak
-- Create date: 08/19/2010
-- Description:	Determines in the state is set to the id specified
-- ==================================================================
CREATE FUNCTION [v3d].[fn_isState]
(
@STATE SMALLINT,
@ID NVARCHAR(16)
)
RETURNS BIT
AS
BEGIN

	DECLARE @RETURN AS BIT

	SET @RETURN = 0
	
  SET @RETURN = ( SELECT COUNT(*) [value] FROM [v3d].[UserStates] WHERE [id] = LOWER(@ID) AND (( @STATE & [value] ) = [value] ) )
  
  RETURN @RETURN

END





GO
GRANT EXECUTE ON [v3d].[fn_isState] TO [v3dsa] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isUserLoggedIn]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =================================================================
-- Author:		  Mark Kontak
-- Create date: 5/4/2010
-- Description: Determines if the USER is logged in
-- ==================================================================
CREATE FUNCTION [v3d].[fn_isUserLoggedIn]
(
@LOGIN AS NVARCHAR(255)
)
RETURNS BIT
AS
BEGIN

  DECLARE @COUNT AS INT
  
  SET @COUNT = 0
  
  -- Get the context
  SELECT @COUNT = COUNT(*) FROM [v3d].[Contexts] WHERE [login] = @LOGIN
  
  RETURN CONVERT(bit,@COUNT)

END





GO
GRANT EXECUTE ON [v3d].[fn_isUserLoggedIn] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isUserLoggedIn] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[AuthenticationTypes]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[AuthenticationTypes](
	[id] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[default] [bit] NOT NULL,
 CONSTRAINT [PK_AuthenticationTypes] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[AuthenticationTypes] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[AuthenticationTypes] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[AuthenticationTypes] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[AuthenticationTypes] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[AuthenticationTypes] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Contexts]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Contexts](
	[id] [uniqueidentifier] NOT NULL,
	[lic] [nvarchar](255) NOT NULL,
	[login] [nvarchar](16) NOT NULL,
	[iss] [datetimeoffset](7) NOT NULL,
	[mod] [datetimeoffset](7) NOT NULL,
	[ttl] [int] NOT NULL,
	[ref] [smallint] NOT NULL,
	[exp]  AS ([v3d].[fn_getExpirationDate]([mod],[ttl],'ContextTtlUnits')),
 CONSTRAINT [PK_CONTEXTS] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Contexts] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Contexts] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Contexts] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Contexts] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Contexts] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[PasswordHints]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[PasswordHints](
	[name] [nvarchar](64) NOT NULL,
	[hint] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_PasswordHints] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [v3d].[PasswordTemplates]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[PasswordTemplates](
	[id] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[default] [bit] NOT NULL,
	[template] [xml] NOT NULL,
 CONSTRAINT [PK_PasswordTemplates] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[PasswordTemplates] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[PasswordTemplates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[PasswordTemplates] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[PasswordTemplates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[PasswordTemplates] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Rights]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Rights](
	[name] [nvarchar](32) NOT NULL,
	[mask] [bigint] NOT NULL,
	[desc] [nvarchar](255) NOT NULL,
 CONSTRAINT [PK_RIGHTS] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Rights] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Rights] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Rights] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Rights] TO [v3dsa] AS [dbo]
GO
/****** Object:  Table [v3d].[Roles]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Roles](
	[name] [nvarchar](16) NOT NULL,
	[prms] [bigint] NOT NULL,
	[desc] [nvarchar](128) NOT NULL,
	[set] [bigint] NOT NULL,
 CONSTRAINT [PK_Roles_1] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Roles] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Roles] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Roles] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Roles] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Roles] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[UserPasswords]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[UserPasswords](
	[login] [nvarchar](16) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[exp] [datetimeoffset](7) NULL,
	[pwd] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_PASSWORDS] PRIMARY KEY CLUSTERED 
(
	[login] ASC,
	[date] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[UserPasswords] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[UserPasswords] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserPasswords] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[UserPasswords] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserPasswords] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[UserProfiles]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[UserProfiles](
	[login] [nvarchar](32) NOT NULL,
	[name] [nvarchar](16) NOT NULL,
	[profile] [xml] NOT NULL,
 CONSTRAINT [PK_Configs] PRIMARY KEY CLUSTERED 
(
	[login] ASC,
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[UserProfiles] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[UserProfiles] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserProfiles] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[UserProfiles] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserProfiles] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Users]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Users](
	[login] [nvarchar](16) NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[state] [smallint] NOT NULL,
	[add] [datetimeoffset](7) NOT NULL,
	[exp] [datetimeoffset](7) NULL,
	[set] [bigint] NOT NULL,
	[auth] [nvarchar](32) NOT NULL,
	[dom] [nvarchar](32) NOT NULL,
	[apps] [int] NOT NULL,
	[role] [nvarchar](16) NOT NULL,
	[ptmp] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_USERS] PRIMARY KEY CLUSTERED 
(
	[login] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Users] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Users] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Users] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Users] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Users] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[UserStates]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[UserStates](
	[id] [nvarchar](16) NOT NULL,
	[value] [smallint] NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_States] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[UserStates] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[UserStates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserStates] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[UserStates] TO [v3dsa] AS [dbo]
GO
/****** Object:  Table [v3d].[Versions]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Versions](
	[object] [nvarchar](32) NOT NULL,
	[type] [nvarchar](32) NOT NULL,
	[version] [nvarchar](16) NOT NULL,
 CONSTRAINT [PK_Versions_1] PRIMARY KEY CLUSTERED 
(
	[object] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Versions] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Versions] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Versions] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Versions] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Versions] TO [v3duser] AS [dbo]
GO
ALTER TABLE [v3d].[AuthenticationTypes] ADD  CONSTRAINT [DF_Table_1_name]  DEFAULT ('') FOR [id]
GO
ALTER TABLE [v3d].[AuthenticationTypes] ADD  CONSTRAINT [DF_AuthenticationTypes_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[AuthenticationTypes] ADD  CONSTRAINT [DF_AuthenticationTypes_deafult]  DEFAULT ((0)) FOR [default]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_CONTEXTS_ID]  DEFAULT (newid()) FOR [id]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_Contexts_licenseKey]  DEFAULT ('') FOR [lic]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_CONTEXTS_USERS_LOGIN]  DEFAULT ('') FOR [login]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_CONTEXTS_DATE_ISSUED]  DEFAULT (sysdatetimeoffset()) FOR [iss]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_CONTEXTS_TTL_DATE]  DEFAULT (sysdatetimeoffset()) FOR [mod]
GO
ALTER TABLE [v3d].[Contexts] ADD  CONSTRAINT [DF_Contexts_ref]  DEFAULT ((1)) FOR [ref]
GO
ALTER TABLE [v3d].[PasswordHints] ADD  CONSTRAINT [DF_PasswordHints_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[PasswordHints] ADD  CONSTRAINT [DF_PasswordHints_hint]  DEFAULT ('') FOR [hint]
GO
ALTER TABLE [v3d].[PasswordTemplates] ADD  CONSTRAINT [DF_PasswordTemplates_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[PasswordTemplates] ADD  CONSTRAINT [DF_PasswordTemplates_default]  DEFAULT ((0)) FOR [default]
GO
ALTER TABLE [v3d].[Rights] ADD  CONSTRAINT [DF_RIGHTS_BITMASK_POS]  DEFAULT ((0)) FOR [mask]
GO
ALTER TABLE [v3d].[Rights] ADD  CONSTRAINT [DF_RIGHTS_DESCRIPTION]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Roles] ADD  CONSTRAINT [DF_Roles_prms]  DEFAULT ((0)) FOR [prms]
GO
ALTER TABLE [v3d].[Roles] ADD  CONSTRAINT [DF_ROLES_DESCRIPTION]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Roles] ADD  CONSTRAINT [DF_Roles_set]  DEFAULT ((0)) FOR [set]
GO
ALTER TABLE [v3d].[UserPasswords] ADD  CONSTRAINT [DF_PASSWORD_HISTORY_CHANGE_DATE]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[UserPasswords] ADD  CONSTRAINT [DF_Passwords_pwd]  DEFAULT ('') FOR [pwd]
GO
ALTER TABLE [v3d].[UserProfiles] ADD  CONSTRAINT [DF_Configs_type]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[UserProfiles] ADD  CONSTRAINT [DF_Translucency_trans]  DEFAULT ('') FOR [profile]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_PROPERTIES]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_DATE_ADDED]  DEFAULT (sysdatetimeoffset()) FOR [add]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_HIDDEN]  DEFAULT ((0)) FOR [set]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_AUTHENTICATION_TYPE]  DEFAULT ((1)) FOR [auth]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_AUTHENTICATION_DOMAIN]  DEFAULT ('') FOR [dom]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_USERS_APPLICATION_MASK]  DEFAULT ((0)) FOR [apps]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_Users_roleId]  DEFAULT ('') FOR [role]
GO
ALTER TABLE [v3d].[Users] ADD  CONSTRAINT [DF_Users_ptmp]  DEFAULT ('') FOR [ptmp]
GO
ALTER TABLE [v3d].[UserStates] ADD  CONSTRAINT [DF_States_desc]  DEFAULT ('') FOR [desc]
GO
/****** Object:  Trigger [v3d].[tr_contextDelete]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================================================
-- Author:		Mark Kontak
-- Create date: 6/9/2008
-- Description:	Upon deletion of the context the license if there is one assicated will be released.
-- ===================================================================================================
CREATE TRIGGER [v3d].[tr_contextDelete]
   ON  [v3d].[Contexts]
   AFTER DELETE
AS 
BEGIN

  -- SET NOCOUNT ON added to prevent extra result sets from
  -- interfering with SELECT statements.
  SET NOCOUNT ON;

  DECLARE @LICENSES AS NVARCHAR(MAX)



  -- ========================================================================================================================
  -- reset the inuse flag essemtially releasing the license (Does the same thing as release sp_releaseLicense). I changed
  -- this to hopefully fix the deadlock issues when multple thread attempt to release the context.
  -- ========================================================================================================================
  SELECT @LICENSES = COALESCE(@LICENSES+',' ,'') + [lic] FROM [DELETED]

  EXEC [v3d].[sp_releaseLicenses] 
    @LICENSES = @LICENSES
     
  -- ========================================================================================================================
  -- Delete all locks associated with the context
  -- ========================================================================================================================
  DELETE [Storage].[v3d].[Locks] FROM DELETED WHERE [ctx] = CONVERT(NVARCHAR(256),DELETED.[id])

END



GO
/****** Object:  Trigger [v3d].[tr_deleteUser]    Script Date: 3/13/2014 8:09:33 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description:	Deletes the configs associated with the user
-- =============================================================
CREATE TRIGGER [v3d].[tr_deleteUser] 
   ON  [v3d].[Users]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Insert statements for trigger here
  DELETE [v3d].[UserProfiles] FROM DELETED WHERE [v3d].[UserProfiles].[login] = DELETED.login

  -- Insert statements for trigger here
  DELETE [v3d].[UserPasswords] FROM DELETED WHERE [v3d].[UserPasswords].[login] = DELETED.login

END



GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Authentication type id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AuthenticationTypes', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Authentication type description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AuthenticationTypes', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the valid authentication types that can be selected for a user' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AuthenticationTypes'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'3' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AuthenticationTypes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'License consumed by this context' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Contexts', @level2type=N'COLUMN',@level2name=N'lic'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User associated with this context' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Contexts', @level2type=N'COLUMN',@level2name=N'login'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the context was issued' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Contexts', @level2type=N'COLUMN',@level2name=N'iss'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the context was modified (set upon insert and renew)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Contexts', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Time to live for this context in minutes' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Contexts', @level2type=N'COLUMN',@level2name=N'ttl'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of password hints that need to formatted based on the password template' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PasswordHints'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PasswordHints'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Password template identifier' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PasswordTemplates', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Password template' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PasswordTemplates', @level2type=N'COLUMN',@level2name=N'template'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'''''' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Rights', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Roles Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Roles', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Bit mask of rights' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Roles', @level2type=N'COLUMN',@level2name=N'prms'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Settings mask for the roles' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Roles', @level2type=N'COLUMN',@level2name=N'set'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the password expires' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserPasswords', @level2type=N'COLUMN',@level2name=N'exp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Password' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserPasswords', @level2type=N'COLUMN',@level2name=N'pwd'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User State' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the user was added to the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'add'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date/Time the users account expires (NULL means it does not expire)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'exp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'If 1 indicates that the user is locked and cannot be deleted or modified. Used of vxservice and administartor' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'set'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Authentication type ( 1-Viatronix, 2-Windows )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'auth'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Windows domain if the auth = ''Windows''' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'dom'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Mask of applications the user is allowed to access' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'apps'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Password template associated with user' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Users', @level2type=N'COLUMN',@level2name=N'ptmp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'State id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserStates', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Values of state' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserStates', @level2type=N'COLUMN',@level2name=N'value'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description of the license state' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserStates', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of object version used to return the ver attribute in the get procedures.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
