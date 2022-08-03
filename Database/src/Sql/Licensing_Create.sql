USE [Licensing]
GO
/****** Object:  User [v3dsa]    Script Date: 11/14/2013 1:54:19 PM ******/
CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  User [v3duser]    Script Date: 11/14/2013 1:54:19 PM ******/
CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  Schema [v3d]    Script Date: 11/14/2013 1:54:19 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_acquireLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================================================================
-- Author:		Mark Kontak
-- Create date: 5/30.2008
-- Description:	Gets a new available license. First check the system to make sure that it has not already been disabled
--
--              Exceptions:
--
--              901001         No available license
--
-- =======================================================================================================================
CREATE PROCEDURE [v3d].[sp_acquireLicense] 
( 
@SYSID AS NVARCHAR(256),          -- System id of a system that is already registered
@LICENSE NVARCHAR(256) OUTPUT     -- Licnese acquired
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
	BEGIN TRY
	

    -- =======================================
    -- Make sure the system id was specified
    -- ========================================
    IF @SYSID IS NULL OR LEN(@SYSID) = 0 
        EXEC [master].[dbo].[sp_throwInvalidDataException] 'System Id'

    -- Check the V3D Licesning System to make sure that it hase not been disabled or needs to be disabled
    EXEC  [v3d].[sp_checkSystem]

	  -- Validate all licenses first (will mark expired licenses)
	  EXEC [v3d].[sp_validateLicenseKeys]

    -- ==============================================================================
    -- Make sure the server was registered (900982) Server: Not Registered
    -- ===============================================================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[RegisteredServers] WHERE [sysid] = @SYSID )
       EXEC [master].[dbo].[sp_throwException] 900982, 'Server'

    -- Declare a temp table to hold the ID of the license updated
    DECLARE @Table TABLE ([key] NVARCHAR(255))
    
    -- Preform the lookup/update all in one but only on the first row
    UPDATE TOP(1) [v3d].[Licenses] 
              SET [v3d].[Licenses].[state] = [v3d].[fn_getState]('inuse') 
              OUTPUT INSERTED.[key] 
                INTO @Table 
                WHERE [v3d].[fn_isState]([v3d].[Licenses].[state], 'available') = 1

    -- ================================================================================================
    -- Check to make sure the row count was not 0 indicating no LICENSES were available (901001)
    -- ================================================================================================
    IF @@ROWCOUNT = 0 
      EXEC [master].[dbo].[sp_throwException] 901001
  
      
    -- Get the currently inserted license id
    SELECT @LICENSE = [key] FROM @Table

    
    RETURN 1


	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_acquireLicense'
	  
	END CATCH
	

END




GO
GRANT EXECUTE ON [v3d].[sp_acquireLicense] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_acquireLicense] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_addLicenses]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================
-- Author:		Mark Kontak
-- Create date: 6/4/2008
-- Description:	Adds new licenses to the system. Each add call should contain a transaction 
--              id that is used to make sure licenses are only added once.
-- =====================================================================================
CREATE PROCEDURE [v3d].[sp_addLicenses]
(
@TRANSACTION_ID AS NVARCHAR(256),       -- Transaction ID (GUID) assigned by the generate license utility
@SYSTEM_ID AS NVARCHAR(256),            -- System id of this system
@DATE_ISSUED AS DATETIMEOFFSET(7),      -- Date these licenses were issues 
@ISSUED_BY AS NVARCHAR(256),            -- Name of person who issue the license
@LICENSE_COUNT AS INTEGER,              -- Number of licenses to add
@TTL AS INTEGER = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 	DECLARE @V3D_SYSTEM_ID AS NVARCHAR(256)
 
  DECLARE @COUNT AS INTEGER
  DECLARE @KEY AS NVARCHAR(256)
  DECLARE @EXPIRES AS DATETIMEOFFSET(0)

  SELECT @COUNT = 0, @EXPIRES = NULL, @KEY = ''

	BEGIN TRY

    --===============================================================================================================
    -- Check the system to make sure that it hase not been disabled or needs to be disabled. Also we will check the 
    -- the system id since adding licenses can be a bit slower then other operations that will have the system check 
    -- disabled.
    --===============================================================================================================
    EXEC  [v3d].[sp_checkSystem]
          @CHECK_ID = 0


	  -- Get the installed syatem id
    SELECT @V3D_SYSTEM_ID = [sysid] FROM [v3d].[Install] 


    -- ============================================================================================================
	  -- Make sure the were are adding license on the correct system. Needed to add the check to for NULL apparently
    -- SQL server does not see that a variable with a value and a variable that is NULL is not equal (go figure) 
    -- (900990) Missing or invalid data (System Id)
	  -- ===========================================================================================================
	  IF @SYSTEM_ID IS NULL OR @SYSTEM_ID <> @V3D_SYSTEM_ID 
	      EXEC [master].[dbo].[sp_throwInvalidDataException] 'System Id'
 	  
    -- ============================================================================
	  -- Make sure the issued by is specified (900990) Missing or invalid data (Issued-By)
	  -- ============================================================================
	  IF @ISSUED_BY IS NULL OR LEN(@ISSUED_BY) = 0
		      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Issued-By'
 
    -- ============================================================================================
	  -- Make sure the number of licenses to add is greater then 0 (900990) Missing or invalid data (License Count)
	  -- ============================================================================================
	  IF @LICENSE_COUNT IS NULL OR @LICENSE_COUNT < 1
		      EXEC [master].[dbo].[sp_throwInvalidDataException] 'License Count'
 
    -- =====================================================================================
	  -- Make sure the date is correct. That means the the date issued 
	  -- cannot be in the future. Only checking days not hours or minutes
	  -- due to differences in system date and times. (900990) Missing or invalid data (Date)
	  -- =====================================================================================
	  IF @DATE_ISSUED IS NULL OR ( DATEDIFF(day, GETUTCDATE(),@DATE_ISSUED) > 0 )
		      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Issue Date'

    -- ========================================
    -- Time-To-Live must be NOT NULL AND <= 0
    -- ========================================
    IF @TTL IS NOT NULL AND @TTL <= 0
		      EXEC [master].[dbo].[sp_throwInvalidDataException] 'TTL'

    -- =============================================================================================
    -- Make sure the licenses for the transaction was not already added. I check the transaction id 
    -- with the id stored in the license table
    -- ============================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Licenses] WHERE [id] = @TRANSACTION_ID )
       EXEC [master].[dbo].[sp_throwException] 901004

    SET @COUNT = 1

    --- Converts the date issued to the date this system is on
    SET @DATE_ISSUED = SWITCHOFFSET(@DATE_ISSUED, DATEPART(tz,SYSDATETIMEOFFSET()))

    -- Get the expiration date for these licesenses (If the TTL is NULL then NULL is returned)
    SET @EXPIRES =  [v3d].[fn_getExpirationDate](@DATE_ISSUED, @TTL, 'LicenseTtlUnits')

    -- Invalidate all previous licenses
    UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('disabled') 
    
	  -- ===============================================
	  -- Loop until all the license have been added
	  -- ===============================================
    WHILE @COUNT <= @LICENSE_COUNT
	  BEGIN
  	
		  -- Get the license key
      EXEC [v3d].[sp_gethashkey] 
            @systemid = @SYSTEM_ID, 
            @transactionid = @TRANSACTION_ID, 
            @number = @COUNT, 
            @key = @KEY OUTPUT

	    -- Add all new license        		
      INSERT INTO [v3d].[Licenses] ([key], [id], [cnt], [iss], [by], [exp], [state] ) 
                    VALUES ( @KEY, @TRANSACTION_ID,  @COUNT, @DATE_ISSUED, @ISSUED_BY, @EXPIRES, [v3d].[fn_getState]('available')  )

	    -- Update the license count in the V3D file
      UPDATE [v3d].[Install] SET [v3d].[Install].[cnt] = [v3d].[Install].[cnt] + 1

	    -- Increment the license to be added count
      SET @COUNT = @COUNT + 1

    END	-- END ... WHILE @COUNT <> @LICENSE_COUNT
  	
	  RETURN (@COUNT - 1)

	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_addLicenses'
	  
	END CATCH


	
END




GO
GRANT EXECUTE ON [v3d].[sp_addLicenses] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkExpirationDate]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================================================
-- Author:		Mark Kontak
-- Create date: 5/9/2008
-- Description:	Makes sure the system date has not been modified to extend licenes life.
-- ==============================================================================================
CREATE PROCEDURE [v3d].[sp_checkExpirationDate] 
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	-- Local variables
	DECLARE @EXPDATE AS DATETIMEOFFSET
  DECLARE @SUCCESS AS BIT

  BEGIN TRY

	  -- Set the success return value to true
	  SET @SUCCESS = 1

	  -- Get the previous date saved in the V3D table and the installed system id
    SELECT @EXPDATE = [v3d].[Install].[expdate]  FROM [v3d].[Install]

    -- =====================================================================================
	  -- Check to make sure that the system has not expired. If the expiration date is NULL 
    -- then the system never expires.
    -- =====================================================================================
    IF @EXPDATE IS NOT NULL AND @EXPDATE < SYSDATETIMEOFFSET()
	  BEGIN

		  -- Set the success to false
		  SET @SUCCESS = 0

		  -- Disable V3D
		  UPDATE [v3d].[Install] SET [v3d].[Install].enabled = 0, [v3d].[Install].[msg] = 'System has expired'

	  END

	-- Return the success flag
	RETURN @SUCCESS

  END TRY
  BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_checkExpirationDate'

  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_checkExpirationDate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkExpirationDate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkSystem]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =================================================================================================================================
-- Author:		Mark Kontak
-- Create date: 5/9/2008
-- Description:	Performs a system check to make sure that we are running on the orrect system and that the system date has not been 
--				      adjusted. If any of those conditions are detected then the V3D system will be disabled. 
-- ==================================================================================================================================
CREATE PROCEDURE [v3d].[sp_checkSystem] 
( 
@CHECK_ID AS BIT = 0  -- Flag that will allow the overriding of the internal system seting
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
	

  BEGIN TRY
  
    -- Local variables
	  DECLARE @ENABLED AS BIT


    -- Get the current state of the V3D system
    SELECT @ENABLED = [v3d].[Install].[enabled] FROM [v3d].[Install]

    -- ===========================================================================
    -- Raise exception indicating that the (900981) V3D Licensing: Not Installed
    -- ==========================================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwException] 900981, 'V3D Licensing' 

    -- ================================================================================
    -- If the system is currently enabled them we need to make sure nothing has changed
    -- =================================================================================
    IF @ENABLED = 1 
    BEGIN

      -- Check the expiration date 
	    EXEC @ENABLED = [v3d].[sp_checkExpirationDate]

      IF @ENABLED = 1
      BEGIN
      
        -- Check the system date 
	      EXEC @ENABLED = [v3d].[sp_checkSystemDate]

        ---------------------------------------------------------------------------------------
        -- If the system date was OK then check the system id if the CHECK ID flag is set
        ---------------------------------------------------------------------------------------
	      IF @ENABLED = 1 AND @CHECK_ID = 1
	      BEGIN

          -- Check the system id
		      EXEC @ENABLED = [v3d].[sp_checkSystemId]
      
          ---------------------------------------------------------------
          -- If the system id is OK then update the date on the record
          ---------------------------------------------------------------
		      IF @ENABLED = 1
		      BEGIN
			      -- Update the V3D table with the new current date
			      UPDATE [v3d].[Install] SET [v3d].[Install].[sysdate] = SYSDATETIMEOFFSET()
		      END 
		    
		    END

	    END

    END
    
    -- ================================
    -- V3D Licensing has been disabled
    -- ================================
    IF @ENABLED = 0
     EXEC [master].[dbo].[sp_throwException] 900980, 'V3D Licensing' 
   
    RETURN 1

  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_checkSystem'

  END CATCH


END



GO
GRANT EXECUTE ON [v3d].[sp_checkSystem] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkSystem] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkSystemDate]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================================================
-- Author:		Mark Kontak
-- Create date: 5/9/2008
-- Description:	Makes sure the system date has not been modified to extend system/licesne life.
-- ==============================================================================================
CREATE PROCEDURE [v3d].[sp_checkSystemDate] 
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	-- Local variables
	DECLARE @PREV_DATE AS DATETIMEOFFSET
  DECLARE @SUCCESS AS BIT

	-- Set the success return value to true
	SET @SUCCESS = 1

	-- Get the previous date saved in the V3D table and the installed system id
  SELECT @PREV_DATE = [v3d].[Install].[sysdate] FROM [v3d].[Install]

  -- ===============================================================================================
	-- Check to make sure that the date has not been manipulated in the clients favor. This
  -- take the current sysdate from the Install table and check it against the current system 
  -- date now using a day interval verifies that it has not been set back a day or more. A value 
  -- less then 0 will indicate thet the date has been moved back a day or more from the last sysdate
  -- I have stored.
  -- ================================================================================================
  IF DATEDIFF(day, @PREV_DATE, SYSDATETIMEOFFSET()) < 0 
	BEGIN

		-- Set the success to false
		SET @SUCCESS = 0

		-- Disable V3D
		UPDATE [v3d].[Install] SET [v3d].[Install].enabled = 0, [v3d].[Install].[msg] = 'System date has been set back'

	END
	ELSE
	BEGIN
		-- Updates the Install table with the latest system date
		UPDATE [v3d].[Install] SET [v3d].[Install].[sysdate] = SYSDATETIMEOFFSET()
	END

	-- Return the success flag
	RETURN @SUCCESS

END



GO
GRANT EXECUTE ON [v3d].[sp_checkSystemDate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkSystemDate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkSystemId]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================
-- Author:		Mark Kontak
-- Create date: 5/9/2008
-- Description:	Checks the system id in the V3D against the actual systems id
-- =================================================================================
CREATE PROCEDURE [v3d].[sp_checkSystemId]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	DECLARE @SUCCESS AS BIT
  DECLARE @SYSTEM_ID AS NVARCHAR(256)

	-- Set the sucess flag to true
	SET @SUCCESS = 1

	-- Get the previous date saved in the V3D table and the installed system id
  SELECT @SYSTEM_ID = [v3d].[Install].[sysid] FROM [v3d].[Install]

	-- Local system id variable
  DECLARE @SYS_ID AS NVARCHAR(256)

	-- Get the system id of the box we are running on
  EXEC [v3d].sp_getsystemid @SYS_ID OUTPUT

	-- Check to make sure that the database has not been moved to another system	
  IF @SYSTEM_ID <> @SYS_ID
	BEGIN

		SET @SUCCESS = 0

		-- Disable V3D
		UPDATE [v3d].[Install] SET [v3d].[Install].enabled = 0, [v3d].[Install].[msg] = 'Invalid system id [' + @SYS_ID + ']]'

	END

  RETURN @SUCCESS

END



GO
GRANT EXECUTE ON [v3d].[sp_checkSystemId] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkSystemId] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_disableLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/209
-- Description:	Disables the license 
-- =======================================================
CREATE PROCEDURE [v3d].[sp_disableLicense]
(
@KEY NVARCHAR(255)
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
   -- Disables the license making it unavailable (Can only disable the license if it is available )
  UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('disabled') WHERE [key] = @KEY AND [v3d].[fn_isState]([state],'available') = 1
 
 
  RETURN @@ROWCOUNT

END




GO
GRANT EXECUTE ON [v3d].[sp_disableLicense] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_enableLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/209
-- Description:	Enables the license is it was disabled
-- =======================================================
CREATE PROCEDURE [v3d].[sp_enableLicense]
(
@KEY NVARCHAR(255)
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
   -- Enables the license making it available (Only if the license is disabled will it be made available)
  UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('available') WHERE [key]= @KEY AND [v3d].[fn_isState]([state],'disabled') = 1
 
  
  RETURN @@ROWCOUNT

  
END




GO
GRANT EXECUTE ON [v3d].[sp_enableLicense] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_errorHandler]    Script Date: 11/14/2013 1:54:19 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getHashKey]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_getHashKey]
	@systemId [nvarchar](4000),
	@transactionId [nvarchar](4000),
	@number [int],
	@key [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Licensing].[StoredProcedures].[sp_getHashKey]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 05/05/2010
-- Description:	Retrieves the licenses
-- =============================================
CREATE PROCEDURE [v3d].[sp_getLicense]
(
@ID AS NVARCHAR(256) = NULL,          -- Not Used (included so all get procedures have the same interface)
@QUERY AS XML = '<license />',        -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN

  -- SET NOCOUNT ON added to prevent extra result sets from
  -- interfering with SELECT statements.
  SET NOCOUNT ON;

  DECLARE @WRAP AS BIT
  DECLARE @RC AS INT

  SET @WRAP = 1
  
  BEGIN TRY

      
  DECLARE @IDOC AS INT

  DECLARE @STATE AS INT
  DECLARE @FROM_EXP AS DATETIMEOFFSET
  DECLARE @TO_EXP AS DATETIMEOFFSET
  DECLARE @FROM_DATEISSUED AS DATETIMEOFFSET
  DECLARE @TO_DATEISSUED AS DATETIMEOFFSET
  DECLARE @FROM_DATEADDED AS DATETIMEOFFSET
  DECLARE @TO_DATEADDED AS DATETIMEOFFSET
  DECLARE @TRANS_ID AS NVARCHAR(1000)
  DECLARE @KEY AS NVARCHAR(1000)
    
  -- Read the license xml
  EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

  SELECT  @KEY              = [key], 
          @STATE            = [state], 
          @TRANS_ID         = [id],
          @FROM_DATEISSUED  = fromDateIssued,
          @TO_DATEISSUED    = toDateIssued,
          @FROM_DATEADDED   = fromDateAdded,
          @TO_DATEADDED     = toDateAdded,
          @FROM_EXP         = fromExpDate,
          @TO_EXP           = toExpDate
          FROM OPENXML(@IDOC, N'/license', 3) 
          WITH 
          (
            [key]             nvarchar(1000),
            [id]              nvarchar(1000),
            [state]           int,
            [fromDateIssued]  DATETIMEOFFSET 'iss/@from',
            [toDateIssued]    DATETIMEOFFSET 'iss/@to',
            [fromDateAdded]   DATETIMEOFFSET 'add/@from',
            [toDateAdded]     DATETIMEOFFSET 'add/@to',
            [fromExpDate]     DATETIMEOFFSET 'exp/@from',
            [toExpDate]       DATETIMEOFFSET 'exp/@to'               
               
          )
     
   -- Remove the document
   EXEC sp_xml_removedocument @IDOC

 
  
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    SET @WHERE = ''
    SET @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Licenses'') AS [ver] FROM [v3d].Licenses '

     
    SET @WHERE = @WHERE + [v3d].[fn_buildStringComparison](N'@KEY', @KEY, '[key]')
    
    IF @STATE IS NOT NULL
    BEGIN
      SET @WHERE = ' [state] & @STATE > 0 AND '
    END
     
    SET @WHERE = @WHERE + [v3d].[fn_buildDateComparison](@FROM_DATEISSUED, N'@FROM_DATEISSUED', @TO_DATEISSUED, N'@TO_DATEISSUED', N'[iss]')
   
    SET @WHERE = @WHERE + [v3d].[fn_buildDateComparison](@FROM_DATEADDED, N'@FROM_DATEADDED', @TO_DATEADDED, N'@TO_DATEADDED', N'[add]')

    SET @WHERE = @WHERE + [v3d].[fn_buildDateComparison](@FROM_EXP, N'@FROM_EXP', @TO_EXP, N'@TO_EXP', N'[exp]')
        
    SET @WHERE = @WHERE + [v3d].[fn_buildStringComparison](N'@TRANS_ID', @TRANS_ID, '[id]')
  
   
    DECLARE @LEN AS INT
  
    SET @LEN = LEN(@WHERE)
  
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    IF @WRAP = 1
      SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''license''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    ELSE
      SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''license'') ); SET @COUNT = @@ROWCOUNT;'

    
    DECLARE @PARAMETERS AS NVARCHAR(2000)
  
  
  SET @PARAMETERS = N'@RESULTS XML OUTPUT, ' +
                    N'@FROM_DATEISSUED DATETIMEOFFSET, ' +
                    N'@TO_DATEISSUED DATETIMEOFFSET, '   +
                    N'@FROM_DATEADDED DATETIMEOFFSET, '  +
                    N'@TO_DATEADDED DATETIMEOFFSET, '    +
                    N'@FROM_EXP DATETIMEOFFSET, '        +
                    N'@TO_EXP DATETIMEOFFSET, '          +
                    N'@STATE INT,'                          +
                    N'@KEY NVARCHAR(256), ' +
                    N'@COUNT INT OUTPUT, ' +
                    N'@TRANS_ID NVARCHAR(255) '

  
  
  EXEC sp_executesql 
          @SELECT,
          @PARAMETERS,
          @TO_DATEISSUED    = @TO_DATEISSUED,
          @FROM_DATEISSUED  = @FROM_DATEISSUED,
          @FROM_DATEADDED   = @FROM_DATEADDED,
          @TO_DATEADDED     = @TO_DATEADDED,
          @FROM_EXP         = @FROM_EXP,
          @TO_EXP           = @TO_EXP,
          @TRANS_ID         = @TRANS_ID,
          @STATE            = @STATE,
          @KEY              = @KEY,
          @COUNT            = @RC OUTPUT,
          @RESULTS          = @RESULTS OUTPUT
     
    IF @RESULTS IS NULL AND @WRAP = 1
      SET @RESULTS = '<collection id="license" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"license"} into (/collection)[1]')   
 
    RETURN @RC

  END TRY
  BEGIN CATCH 
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getLicense'

  END CATCH

END




GO
GRANT EXECUTE ON [v3d].[sp_getLicense] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getLicense] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLicenseRemote]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================
-- Author:		  Mark Kontak
-- Create date: 1/11/2013
-- Description:	Front-End call used by remote databases because xml is not allowed to passed
--              through.
-- ==============================================================================================
CREATE PROCEDURE [v3d].[sp_getLicenseRemote]
@QUERY NVARCHAR(MAX),
@RESULTS NVARCHAR(MAX) OUTPUT
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	DECLARE @XML AS XML
	DECLARE @QXML AS XML

	SET @QXML = CONVERT(XML, @QUERY)

	EXEC [v3d].[sp_getLicense]
		@QUERY = @QXML,
		@RESULTS = @XML OUT


	SET @RESULTS = CONVERT(NVARCHAR(MAX),@XML)


END


GO
GRANT EXECUTE ON [v3d].[sp_getLicenseRemote] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getLicenseRemote] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getService]    Script Date: 11/14/2013 1:54:19 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getSystemId]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_getSystemId]
	@systemid [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Licensing].[StoredProcedures].[sp_getSystemId]
GO
/****** Object:  StoredProcedure [v3d].[sp_lockLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/26/209
-- Description:	Locks the license
-- =============================================
CREATE PROCEDURE [v3d].[sp_lockLicense]
(
@KEY NVARCHAR(255)
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

 	BEGIN TRY
 	
 	  -- ==========================================
 	  -- Check for an invalid license key (900990)
 	  -- ==========================================
 	  IF @KEY IS NULL OR LEN(@KEY) = 0 
	    EXEC [master].[dbo].[sp_throwInvalidDataException] 'License Key'

    DECLARE @STATE AS INT 
    
    
   
    -- Make sure the license exists (901008)
    IF ( SELECT COUNT(*) FROM [v3d].[Licenses] WHERE [key] = @KEY ) = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'License'
     
    IF ( SELECT COUNT(*) FROM [v3d].[Licenses] WHERE [key] = @KEY  AND [v3d].[fn_isState]([state],'available') = 1 ) = 1 
    BEGIN
      -- Locks license making it unavailable (Can only lock the license if it is available)
      UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('locked') WHERE [key] = @KEY  AND [v3d].[fn_isState]([state],'available') = 1
    END
    ELSE
    BEGIN  
      -- (901002) License in use
      EXEC [master].[dbo].[sp_throwException] 901002
 
    END  
    
    RETURN @@ROWCOUNT

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_lockLicense'

  END CATCH
 
  
END




GO
GRANT EXECUTE ON [v3d].[sp_lockLicense] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_register]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================================
-- Author:		  Mark Kontak
-- Create date: 10/24/2012
-- Paramteres:
--
-- ENABLED        Enable the system flag (NULL is default, means ignore flag).
--                If the system has not been installed and this is the first time this stored 
--                procedure is called then this flag is ignored.
-- EXPDATE        New expiration date if NULL this value is ignored.
-- EXPTTL         Expiration time to live in EXPUNITS (NULL value is ignored)
-- EXPUNITS       Expiration units to be used in conjunction with the EXPTTL.
--                ( day is the default )
-- INFINITE       Flag to indicate that the system never expires.
-- 
--
-- 
-- Description:	Registers V3D Licensing with current server. Parameters are processed based
--              on a specific order so not all will be used.
--
--              EXPDATE   The expiration date is the primary parameter. If this value
--                        is set then the EXPTTL and EXPUNITS are ignored. If set this
--                        values sets the expiration date to value supplied.
--            
--              EXPTTL/   These parameters are only utilized if the expiration date is
--              EXPUNITS  not supplied. If set a new expiration date will be calculated
--                        using current date and time.
--
--              @INFINITE Flag indicating that the system des not expire. This if only
--                        evaluated if the EXPDATE, EXPTTL are NULL. 
-- 
-- ========================================================================================
CREATE PROCEDURE [v3d].[sp_register]
@ENABLE AS BIT = NULL,                  -- Enable the system
@EXPTTL AS INT = NULL,                  -- Expiration time to live
@EXPUNITS AS NVARCHAR(32) = 'day',      -- Expiration units
@EXPDATE AS DATETIMEOFFSET = NULL,      -- V3D System expiration date
@INFINITE AS BIT = NULL                 -- Flag indicating that the system does not expire
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  
	BEGIN TRY

    DECLARE @INSTALLED AS BIT
    DECLARE	@SYSTEMID NVARCHAR(256)
	

    SELECT @INSTALLED = 0, @SYSTEMID = ''


    -- ========================================================================================
    -- Raise exception indication that the system is in-use and cannot be registered (901004)
    -- ========================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Licenses] WHERE [state] = [v3d].[fn_getState]('inuse') )
	    EXEC [master].[dbo].[sp_throwException] 901000


    IF EXISTS ( SELECT * FROM [v3d].[Install] )
      SET @INSTALLED = 1
    ELSE
      EXEC	[v3d].[sp_getsystemid]
		    @systemid = @SYSTEMID OUTPUT




    -- =============================================================================
    -- If a new expiration date is given then we will override the previous value
    -- =============================================================================
    IF @EXPDATE IS NOT NULL 
    BEGIN
      
      IF @INSTALLED = 1
        UPDATE [v3d].[Install] SET [expdate] = @EXPDATE, [enabled] = ISNULL(@ENABLE,[enabled])
      ELSE
        INSERT INTO [v3d].[Install] ([sysid], [expdate]) VALUES (@SYSTEMID, @EXPDATE)

    END

    -- ==================================================================================
    -- If an Expiration TTL and Units are given then we will override the previsous value
    -- ==================================================================================
    ELSE IF @EXPTTL IS NOT NULL 
    BEGIN

      DECLARE @UNITS AS NVARCHAR(64)
      DECLARE @DATE AS DATETIMEOFFSET

      SET @UNITS = LOWER(@EXPUNITS)
      SET @DATE = SYSDATETIMEOFFSET()

      IF @UNITS = 'day'
        SET @EXPDATE = DATEADD(day, @EXPTTL, @DATE )
      ELSE IF @UNITS = 'minute'
        SET @EXPDATE = DATEADD(minute, @EXPTTL, @DATE )
      ELSE IF @UNITS = 'second'
        SET @EXPDATE = DATEADD(second, @EXPTTL, @DATE )
      ELSE IF @UNITS = 'month'
        SET @EXPDATE = DATEADD(month, @EXPTTL, @DATE )
      ELSE IF @UNITS = 'year'
        SET @EXPDATE = DATEADD(year, @EXPTTL, @DATE )
      ELSE  
 	      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Expiration Units'
     
      IF @INSTALLED = 1
        UPDATE [v3d].[Install] SET [expdate] = @EXPDATE, [enabled] = ISNULL(@ENABLE,[enabled])
      ELSE
        INSERT INTO [v3d].[Install] ([sysid], [expdate] ) VALUES (@SYSTEMID, @EXPDATE)


    END

   -- ===================================================================================================
    -- If the system is being set to never expire then 
    -- ==================================================================================================
    ELSE IF @INFINITE = 1
    BEGIN

      IF @INSTALLED = 1
       UPDATE [v3d].[Install] SET [expdate] = NULL, [enabled] = ISNULL(@ENABLE,[enabled])
      ELSE
        INSERT INTO [v3d].[Install] ([sysid], [expdate] ) VALUES (@SYSTEMID, NULL)

    END

    -- ===================================================================================================
    -- If no previous install and no expiration date or ttl was given then use the default of 10 days
    -- ==================================================================================================
    ELSE IF @INSTALLED = 0
    BEGIN

      INSERT INTO [v3d].[Install] ([sysid]) VALUES (@SYSTEMID)

    END


    DECLARE @ID AS NVARCHAR(256)	
    DECLARE @KEY AS NVARCHAR(256)	
    DECLARE @HASH AS NVARCHAR(256)	
	  DECLARE @COUNT AS INTEGER


    -- Set up cursor to check the stale licenses
    DECLARE LICENSE_CURSOR CURSOR LOCAL FOR SELECT [key], [id], [cnt] FROM [v3d].[Licenses] WHERE [state] = [v3d].[fn_getState]('available') 

    -- open the cursor
    OPEN LICENSE_CURSOR

    -- fetch the first record
    FETCH NEXT FROM LICENSE_CURSOR INTO  @KEY, @ID, @COUNT

   -- while there are records in the list
    WHILE @@FETCH_STATUS = 0
    BEGIN
    
    	-- Get the hash key for the license to make sure it is valid
      EXEC [v3d].sp_gethashkey 
          @systemid       = @SYSTEMID, 
          @transactionid  = @ID,
          @number         = @COUNT,
          @key            = @HASH OUTPUT
      
      UPDATE [v3d].[Licenses] SET [key] = @HASH WHERE [key] = @KEY
      
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM LICENSE_CURSOR INTO @KEY, @ID, @COUNT
      
    END

    --- close the cursor
    CLOSE LICENSE_CURSOR

    -- release the cursor resources
    DEALLOCATE LICENSE_CURSOR

    RETURN 1

	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_register'
	  
	END CATCH


END



GO
/****** Object:  StoredProcedure [v3d].[sp_registerSystem]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================
-- Author:		  Mark Kontak
-- Create date: 10/25/2012
-- Description:	Registers a server with this licensing system
-- ===============================================================
CREATE PROCEDURE [v3d].[sp_registerSystem]
@SYSID AS NVARCHAR(256),        -- System id of the server
@NAME AS NVARCHAR(32),          -- DNS/Computer name
@IP AS NVARCHAR(128),           -- IP Address
@INS AS DATETIMEOFFSET,         -- Date the systen was installed
@LS AS NVARCHAR(128) OUTPUT     -- License server
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT

  SET @RC = 0

  BEGIN TRY

    SET @LS = @@SERVERNAME

    -- =====================================================================================
    -- Make sure the SYSTEM ID was specified (900990) Missing or invalid data (System Id)
    -- ====================================================================================
    IF @SYSID IS NULL OR LEN(@SYSID) = 0 
       EXEC [master].[dbo].[sp_throwInvalidDataException] 'System Id'

    -- ==============================================================================
    -- Make sure the name is specified (900990) Missing or invalid data (System Name)
    -- ==============================================================================
    IF @NAME IS NULL OR LEN(@NAME) = 0 
 	      EXEC [master].[dbo].[sp_throwInvalidDataException] 'System Name'

    -- ======================================================================================
    -- Make sure the server was not already registered (901008) Server already registered 
    -- ======================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[RegisteredServers] WHERE [sysid] = @SYSID )
 	      EXEC [master].[dbo].[sp_throwException] 901003

   
    INSERT INTO [v3d].[RegisteredServers] 
                ( [sysid], [name], [ip], [ins] ) 
          VALUES ( @SYSID, @NAME, ISNULL(@IP,''), ISNULL( @INS, SYSDATETIMEOFFSET() )  )

    SET @RC = @@ROWCOUNT


    INSERT INTO [v3d].[RegistrationHistory] 
                ( [sysid], [name], [op], [status] ) 
            VALUES ( @SYSID, @NAME, 'Registered', 'Success')

    RETURN @RC

	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_registerSystem'
 	  
	END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_registerSystem] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_reindexTable]    Script Date: 11/14/2013 1:54:19 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_releaseLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================================
-- Author:		Mark Kontak
-- Create date: 5/9/2008
-- Description:	Release the license. Basically this procedure will reset the inuse flag for the license
-- ======================================================================================================
CREATE PROCEDURE [v3d].[sp_releaseLicense] 
( 
@KEY AS NVARCHAR(255)       -- License to be released
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

	BEGIN TRY
	
	  -- ===================================
 	  -- Check for an invalid license key
 	  -- ===================================
 	  IF @KEY IS NULL OR LEN(@KEY) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'License Key'
	    
    -- reset the inuse flag (If the ID does not exists I dont care)
    UPDATE [v3d].[Licenses] 
      SET [state] = [v3d].[fn_getState]('available') 
      WHERE [key] = @KEY AND [v3d].[fn_isState]([state],'inuse') = 1

    RETURN @@ROWCOUNT
  
  END TRY
  BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_releaseLicense'
	
  END CATCH
  
END




GO
GRANT EXECUTE ON [v3d].[sp_releaseLicense] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_releaseLicense] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_releaseLicenses]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================
-- Author:		  Mark Kontak
-- Create date: 1/11/2013
-- Description:	Mark any licenses inuse as available
-- ===================================================
CREATE PROCEDURE [v3d].[sp_releaseLicenses]
@LICENSES NVARCHAR(MAX)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  IF @LICENSES IS NOT NULL AND LEN(@LICENSES) > 0 
  BEGIN


    BEGIN TRY

      UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('available') 
        FROM [v3d].[fn_parseString](@LICENSES,'|') 
        WHERE [v3d].[Licenses].[key] = [StringValue] AND [v3d].[fn_isState]([state],'inuse') = 1

 
    END TRY
    BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_acquireLicenses'
    END CATCH

  END
 END


GO
GRANT EXECUTE ON [v3d].[sp_releaseLicenses] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_releaseLicenses] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_unlockLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===========================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/209
-- Description:	UNlocks the license if it was already locked
-- ============================================================
CREATE PROCEDURE [v3d].[sp_unlockLicense]
(
@KEY NVARCHAR(255)          -- License Key
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CONTEXT_LOGIN AS NVARCHAR(16)

 	BEGIN TRY
 	
 	  -- ===================================
 	  -- Check for an invalid license key
 	  -- ===================================
 	  IF @KEY IS NULL OR LEN(@KEY) = 0 
 	     EXEC [master].[dbo].[sp_throwInvalidDataException] 'License Key'
	    
    DECLARE @STATE AS INT 
    
    
   
    -- Make sure the key exists
    IF ( SELECT COUNT(*) FROM [v3d].[Licenses] WHERE [key] = @KEY ) = 0
       EXEC [master].[dbo].[sp_throwNotFoundException] 'License'
   
    -- Locks license making it unavailable (Can only lock the license if it is available)
    UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('available') WHERE [key] = @KEY  AND [v3d].[fn_isState]([state],'locked') = 1
   
    RETURN @@ROWCOUNT

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_unlockLicense'

  END CATCH
 
END




GO
GRANT EXECUTE ON [v3d].[sp_unlockLicense] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_unregisterSystem]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 11/09/2012
-- Description:	Unregsiter system
-- =============================================
CREATE PROCEDURE [v3d].[sp_unregisterSystem]
@SYSID AS NVARCHAR(512)       -- System id
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
  DECLARE @NAME AS NVARCHAR(64)

  SET @RC = 0
  SET @NAME = ''

  SELECT @NAME = [name] FROM  [v3d].[RegisteredServers] WHERE [sysid] = @SYSID

  SET @RC = @@ROWCOUNT

  -- If system was found then delete
  IF @RC = 1
    DELETE FROM [v3d].[RegisteredServers] WHERE [sysid] = @SYSID


  DECLARE @STATUS AS NVARCHAR(16)
  DECLARE @MSG AS NVARCHAR(512)

  SET @STATUS = 'Success'
  SET @MSG = ''

  IF @RC = 0 
  BEGIN
    SET @STATUS = 'Failed'
    SET @MSG = 'System not found'
  END

  -- Maintain history of registrations
  INSERT INTO [v3d].[RegistrationHistory] 
              ( [sysid], [name], [op], [status], [msg] ) 
          VALUES ( @SYSID, @NAME, 'Unregistered', @STATUS, @MSG)

END


GO
/****** Object:  StoredProcedure [v3d].[sp_updateExpiredLicenses]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/17/2010
-- Description:	Updates all expired license
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateExpiredLicenses]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	-- Set the state of all expired license
	UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('expired'), [msg] = 'License expired' WHERE ( [exp] IS NOT NULL AND [exp] < SYSDATETIMEOFFSET() )

END




GO
GRANT EXECUTE ON [v3d].[sp_updateExpiredLicenses] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateExpiredLicenses] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_validateLicense]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/25/2012
-- Description:	Checks the spcified license
-- =============================================
CREATE PROCEDURE [v3d].[sp_validateLicense]
@KEY AS NVARCHAR(256),        -- License Key to check
@CHECK_EXPIRED AS BIT = 1,    -- Check if the license has expired
@CHECK_INUSE AS BIT = 0       -- Chck if the license is in use
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    -- ==========================================================================
    -- Updates all license to a state if expired if they have past their prime
    -- ==========================================================================
    EXEC [v3d].[sp_updateExpiredLicenses]
	
    -- =======================================================================================
    -- Make sure the license is not missong or invalid (900990) Missing or invalid license 
    -- =======================================================================================
    IF (@KEY IS NULL OR LEN(@KEY) = 0) 
 	     EXEC [master].[dbo].[sp_throwInvalidDataException] 'License Key'

    DECLARE @STATE AS INT
    SELECT @STATE = [state] FROM [v3d].[Licenses] WHERE [key] = @KEY

    -- =======================================================================================
    -- Make sure the license is not missing or invalid (900990) License not found
    -- =======================================================================================
    IF @@ROWCOUNT = 0 
 	     EXEC [master].[dbo].[sp_throwNotFoundException] 'License'

    -- ================================
    -- Check for an expired license
    -- ================================
    IF @CHECK_EXPIRED = 1 AND [v3d].[fn_isState](@STATE,'expired') = 1
	     EXEC [master].[dbo].[sp_throwException] 900993, 'License'

    -- ========================================================
    -- Check for an inuse license (901002) Licesne is in use 
    -- ========================================================
    IF @CHECK_INUSE = 1 AND [v3d].[fn_isState](@STATE,'inuse') = 1
	     EXEC [master].[dbo].[sp_throwException] 901002

    RETURN 1


  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_validateLicense'
	  
  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_validateLicense] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_validateLicense] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_validateLicenseKeys]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================================================
-- Author:		Mark Kontak
-- Create date: 5/2/2008
-- Description:	Validate the licenses in the V3D_LICENSES table and marking them expired or disabled based on the 
--				failed validation prcess. If all was successful then the license is still good
-- ====================================================================================================================
CREATE PROCEDURE [v3d].[sp_validateLicenseKeys]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	-- Local variables
  DECLARE @KEY AS NVARCHAR(256)
  DECLARE @ID AS NVARCHAR(256)	
	DECLARE @COUNT AS INTEGER
	DECLARE @EXPIRES AS DATETIMEOFFSET
	DECLARE @SYSKEY AS NVARCHAR(256)
	DECLARE @SYSTEM_ID AS NVARCHAR(256)
	DECLARE @ENABLED AS BIT
	DECLARE @HASH AS NVARCHAR(255)
  
  SET @COUNT = 0

	BEGIN TRY
	
	  -- Gets the installed system id
	  SELECT  @SYSTEM_ID= [sysid] FROM [v3d].[Install] 
	
    -- Check the system to make sure that it hase not been disabled or needs to be disabled
    EXEC [v3d].[sp_checksystem] 

    --- Updates all expired licenses
    EXEC [v3d].[sp_updateExpiredLicenses]
    
    -- Set up cursor to check the stale licenses
    DECLARE LICENSE_CURSOR CURSOR LOCAL FOR SELECT [key], [id], [cnt], [exp] FROM [v3d].[Licenses] WHERE [state] = [v3d].[fn_getState]('available') 

    -- open the cursor
    OPEN LICENSE_CURSOR

    -- fetch the first record
    FETCH NEXT FROM LICENSE_CURSOR INTO @KEY, @ID, @COUNT, @EXPIRES

   -- while there are records in the list
    WHILE @@FETCH_STATUS = 0
    BEGIN
    
    	-- Get the hash key for the license to make sure it is valid
      EXEC [v3d].sp_gethashkey 
          @systemid       = @SYSTEM_ID, 
          @transactionid  = @ID,
          @number         = @COUNT,
          @key            = @HASH OUTPUT
      
      -- =======================================
      -- Make sure the system id is correct
      -- =======================================
      IF @KEY <> @HASH 
        UPDATE [v3d].[Licenses] SET [state] = [v3d].[fn_getState]('disabled') , [msg] = 'Invalid key ' + @HASH WHERE [key] = @KEY
      ELSE
        SET @COUNT = @COUNT + 1
      
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM LICENSE_CURSOR INTO @KEY, @ID, @COUNT, @EXPIRES
      
    END

    --- close the cursor
    CLOSE LICENSE_CURSOR

    -- release the cursor resources
    DEALLOCATE LICENSE_CURSOR
    
    -- Return the number of valid licenses
    RETURN @COUNT

	END TRY
	BEGIN CATCH
	
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_validateLicenseKeys'
	  
	END CATCH


END




GO
GRANT EXECUTE ON [v3d].[sp_validateLicenseKeys] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_validateLicenseKeys] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildDateComparison]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Generates a comparison statement for the supplied date as if a range 
--              is to be used.
-- ====================================================================================
CREATE FUNCTION [v3d].[fn_buildDateComparison]
(
@FROM_DATE AS DATETIMEOFFSET(0),
@FROM_PARAM_NAME AS NVARCHAR(128),
@TO_DATE AS DATETIMEOFFSET(0),
@TO_PARAM_NAME AS NVARCHAR(128),
@COL AS NVARCHAR(128)
)
RETURNS NVARCHAR(255)
AS
BEGIN

	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(255)
  DECLARE @FROM_TIME AS NVARCHAR(8)
  DECLARE @TO_TIME AS NVARCHAR(8)
  DECLARE @OFFSET AS NVARCHAR(20)
  
 	SET @COMPARE = ''
  SET @OFFSET = CONVERT(NVARCHAR(20),DATEPART(tz,SYSDATETIMEOFFSET()))

  SET @FROM_TIME = CONVERT(VARCHAR(8),@FROM_DATE,108)
  SET @TO_TIME = CONVERT(VARCHAR(8),@TO_DATE,108)


  
  IF @TO_DATE IS NULL AND @FROM_DATE IS NOT NULL
  BEGIN
  
    IF @FROM_TIME IS NULL OR @FROM_TIME = '00:00:00'
      SET @COMPARE = 'CONVERT(date,' + @COL + ') >= CONVERT(date,' + @FROM_PARAM_NAME + ') AND '                   
    ELSE
      SET @COMPARE = @COL + ' >= SWITCHOFFSET(' + @FROM_PARAM_NAME + ', ' + @OFFSET + ') AND '                   
    
  END
  ELSE IF @TO_DATE IS NOT NULL AND @FROM_DATE IS NULL
  BEGIN
    
    IF @TO_TIME IS NULL OR @TO_TIME = '00:00:00'
      SET @COMPARE = 'CONVERT(date,' + @COL + ') <= CONVERT(date,' + @TO_PARAM_NAME + ') AND '      
    ELSE
      SET @COMPARE = @COL + ' <= SWITCHOFFSET(' + @TO_PARAM_NAME + ', ' + @OFFSET  + ') AND '
    
  END
  ELSE IF @TO_DATE IS NOT NULL AND @FROM_DATE IS NOT NULL
  BEGIN 
  
    -- ==========================================================
    -- If the FROM and TO are equal make the comparison an equal
    -- ===========================================================
    IF @TO_DATE = @FROM_DATE
    BEGIN
      IF @FROM_TIME IS NULL OR @FROM_TIME = '00:00:00'
        SET @COMPARE = 'CONVERT(date,' + @COL + ') = CONVERT(date,' + @FROM_PARAM_NAME + ') AND '
      ELSE
        SET @COMPARE = @COL + ' = SWITCHOFFSET(' + @FROM_PARAM_NAME + ', ' + @OFFSET + ') AND '
    END
    ELSE
    BEGIN

      -- ======================================
      -- Set up the from protion of the compare
      -- =======================================
      IF @FROM_TIME IS NULL OR @FROM_TIME = '00:00:00'
        SET @COMPARE = 'CONVERT(date,' + @COL + ') >= CONVERT(date,' + @FROM_PARAM_NAME + ') AND '
      ELSE
        SET @COMPARE = @COL + ' >= SWITCHOFFSET(' + @FROM_PARAM_NAME + ', ' + @OFFSET + ') AND '

      -- =====================================
      -- Set up the to portion of the compare
      -- =====================================
      IF @TO_TIME IS NULL OR @TO_TIME = '00:00:00'
        SET @COMPARE = @COMPARE + 'CONVERT(date,' + @COL + ') <= CONVERT(date,' + @TO_PARAM_NAME + ') AND '
      ELSE  
        SET @COMPARE = @COMPARE + @COL + ' <= SWITCHOFFSET(' + @TO_PARAM_NAME + ', ' + @OFFSET + ') AND '

    END
    
  END
 
 	-- Return the result of the function
	RETURN @COMPARE

END



GO
GRANT EXECUTE ON [v3d].[fn_buildDateComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildDateComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildLikeStringComparison]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Builds a comparison string for a string value taking into account a % at the beginning
--              and/or end to make use of a like. Used when dynamically building quries.
-- =======================================================================================================
CREATE FUNCTION [v3d].[fn_buildLikeStringComparison] 
(
@NAME AS NVARCHAR(125),
@VALUE AS NVARCHAR(1000),
@COL AS NVARCHAR(126)
)
RETURNS NVARCHAR(1000)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(1000)
	DECLARE @INDEX AS INT
	DECLARE @START AS INT
	
	SET @COMPARE = ''
	
	-- ============================================================================
	-- If the value is not NULL then there will be some sort on comparison string
	-- ============================================================================
  IF @VALUE IS NOT NULL  AND LEN(@VALUE) > 0
  BEGIN
    
    -- ======================================================================================
    -- Check for % and if found we are using a LIKE statement instead of the '=' statement
    -- ======================================================================================
    IF ( LEFT(@VALUE,1) = '%' OR RIGHT(@VALUE,1) = '%' )
    BEGIN
      -- LIKE statement
      SET @COMPARE = @COL + ' LIKE ' + @NAME
    END
    ELSE
    BEGIN
      -- EQUAL statement
      SET @COMPARE = @COL + ' = ' + @NAME 
    END
   
    
  END   -- END ... If the value is not NULL 

  

	-- Return the result of the function
	RETURN @COMPARE

END


GO
GRANT EXECUTE ON [v3d].[fn_buildLikeStringComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildLikeStringComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildStringComparison]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Builds a comparison string for a string value taking into account a % at the beginning
--              and/or end to make use of a like. Used when dynamically building quries.
-- =======================================================================================================
CREATE FUNCTION [v3d].[fn_buildStringComparison] 
(
@NAME AS NVARCHAR(125),
@VALUE AS NVARCHAR(512),
@COL AS NVARCHAR(126)
)
RETURNS NVARCHAR(1000)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(1000)
	DECLARE @INDEX AS INT
	DECLARE @START AS INT
	DECLARE @LEN AS INT
	DECLARE @FLD AS NVARCHAR(128)
	DECLARE @NM AS NVARCHAR(512)
	
	SET @COMPARE = ''
	
	-- ============================================================================
	-- If the value is not NULL then there will be some sort on comparison string
	-- ============================================================================
  IF @VALUE IS NOT NULL  AND LEN(@VALUE) > 0
  BEGIN
    
    SET @START = 1

    SET @INDEX = CHARINDEX('|', @VALUE, @START  )
    
    IF @INDEX > 0 
    BEGIN
    
      
      WHILE @INDEX > 0
      BEGIN
              
        SET @LEN = @INDEX - @START
        SET @FLD = SUBSTRING(@VALUE,@START, @LEN )
      
        SET @NM = 'SUBSTRING(' + @NAME + ', ' + CONVERT(NVARCHAR(10),@START )+ ', ' + CONVERT(NVARCHAR(10),@LEN) + ')'
        SET @COMPARE = @COMPARE + [v3d].[fn_buildLikeStringComparison](@NM, @FLD, @COL ) + ' OR '

        SET @START = @INDEX + 1
        
        SET @INDEX = CHARINDEX('|', @VALUE,@START)

      END
      
      SET @LEN = LEN(@VALUE) - @START + 1
      SET @FLD = SUBSTRING(@VALUE,@START, @LEN )
    
      SET @NM = 'SUBSTRING(' + @NAME + ', ' + CONVERT(NVARCHAR(10),@START )+ ', ' + CONVERT(NVARCHAR(10),@LEN) + ')'
      SET @COMPARE = @COMPARE + [v3d].[fn_buildLikeStringComparison](@NM, @FLD, @COL ) + ' AND '

    END 
    ELSE
    BEGIN
    
      SET @COMPARE = [v3d].[fn_buildLikeStringComparison](@NAME, @VALUE, @COL ) + ' AND '
    
    END
    
  END   -- END ... If the value is not NULL 

  

	-- Return the result of the function
	RETURN @COMPARE 

END


GO
GRANT EXECUTE ON [v3d].[fn_buildStringComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildStringComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDatabase]    Script Date: 11/14/2013 1:54:19 PM ******/
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

  SET @NAME = 'Licensing'
  
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
/****** Object:  UserDefinedFunction [v3d].[fn_getExpirationDate]    Script Date: 11/14/2013 1:54:19 PM ******/
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
@DATE AS DATETIMEOFFSET(0),   -- Starting date/time
@TTL AS INT,                  -- Time to live
@SETTING AS NVARCHAR(32)      -- Setting to use
)
RETURNS DATETIMEOFFSET(0)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @EXPIRES AS DATETIMEOFFSET(0)
	DECLARE @UNITS AS NVARCHAR(32)
  DECLARE @INFINTE AS INT

  IF @TTL IS NULL
  BEGIN
    SET @EXPIRES = NULL
  END
  ELSE
  BEGIN

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
  
  END
  
	-- Return the result of the function
	RETURN @EXPIRES

END




GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSetting]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Maek Kontak 
-- Create date: 11/02/2009
-- Description:	Gets the system setting value
-- =============================================
CREATE FUNCTION [v3d].[fn_getSetting] 
(
@NAME AS NVARCHAR(32),
@DEFAULT AS NVARCHAR(32) = '' 
)
RETURNS NVARCHAR(512)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @VALUE AS NVARCHAR(512)

  SET @VALUE = @DEFAULT
  
	-- Add the T-SQL statements to compute the return value here
	SELECT @VALUE = value FROM [v3d].[LicenseSettings] WHERE [name] = @NAME

    
	-- Return the result of the function
	RETURN @VALUE

END



GO
GRANT EXECUTE ON [v3d].[fn_getSetting] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getSetting] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getState]    Script Date: 11/14/2013 1:54:19 PM ******/
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
RETURNS TINYINT
AS
BEGIN
  
  --- Returns the selected value 
	RETURN ( SELECT [value] FROM [v3d].[LicenseStates] NOLOCK  WHERE [id] = @ID )

END




GO
GRANT EXECUTE ON [v3d].[fn_getState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getState] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getVersion]    Script Date: 11/14/2013 1:54:19 PM ******/
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

	-- Return the result of the function
	RETURN @VERSION
END




GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isLicenseExpired]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================
-- Author:		  Mark Kontak
-- Create date: 10/25/2012
-- Description:	Check if the license specified has expired
-- ==========================================================
CREATE FUNCTION [v3d].[fn_isLicenseExpired]
(
@KEY AS NVARCHAR(256)          -- License Key to check
)
RETURNS BIT
AS
BEGIN

  DECLARE @EXPIRED AS BIT


  IF EXISTS ( SELECT * FROM [v3d].[Licenses] WHERE [key] = @KEY AND  [v3d].[fn_isState]([state],'expired') = 1 )
    SET @EXPIRED = 1
  ELSE 
    SET @EXPIRED = 0

  RETURN @EXPIRED
  
 
END



GO
GRANT EXECUTE ON [v3d].[fn_isLicenseExpired] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isLicenseExpired] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isState]    Script Date: 11/14/2013 1:54:19 PM ******/
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
@STATE TINYINT,
@ID NVARCHAR(16)
)
RETURNS BIT
AS
BEGIN

  DECLARE @RETURN AS BIT
  
  SET @RETURN = ( SELECT COUNT(*) FROM [v3d].[LicenseStates] WHERE [id] = @ID AND [value] = @STATE )
  
  RETURN @RETURN

END




GO
GRANT EXECUTE ON [v3d].[fn_isState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isState] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Install]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Install](
	[sysid] [nvarchar](256) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[sysdate] [datetimeoffset](7) NOT NULL,
	[expdate] [datetimeoffset](7) NULL,
	[enabled] [bit] NOT NULL,
	[msg] [nvarchar](512) NOT NULL,
	[cnt] [int] NOT NULL,
	[ls] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_V3D] PRIMARY KEY CLUSTERED 
(
	[sysid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Install] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Install] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Install] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Install] TO [v3dsa] AS [dbo]
GO
/****** Object:  Table [v3d].[Licenses]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Licenses](
	[key] [nvarchar](256) NOT NULL,
	[id] [nvarchar](256) NOT NULL,
	[cnt] [int] NOT NULL,
	[iss] [datetimeoffset](7) NOT NULL,
	[by] [nvarchar](128) NOT NULL,
	[add] [datetimeoffset](7) NOT NULL,
	[msg] [nvarchar](256) NOT NULL,
	[exp] [datetimeoffset](7) NULL,
	[state] [tinyint] NOT NULL,
 CONSTRAINT [PK_Licenses] PRIMARY KEY CLUSTERED 
(
	[key] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Licenses] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Licenses] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Licenses] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Licenses] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Licenses] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[LicenseSettings]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[LicenseSettings](
	[name] [nvarchar](32) NOT NULL,
	[subsystem] [nvarchar](32) NOT NULL,
	[value] [nvarchar](255) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[chg] [bit] NOT NULL,
 CONSTRAINT [PK_Settings_1] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[LicenseSettings] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[LicenseSettings] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[LicenseSettings] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[LicenseSettings] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[LicenseSettings] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[LicenseStates]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[LicenseStates](
	[id] [nvarchar](16) NOT NULL,
	[value] [tinyint] NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_States] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[LicenseStates] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[LicenseStates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[LicenseStates] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[LicenseStates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[LicenseStates] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[RegisteredServers]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[RegisteredServers](
	[sysid] [nvarchar](256) NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[ip] [nvarchar](128) NOT NULL,
	[ins] [datetimeoffset](7) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
 CONSTRAINT [PK_RegisteredServers] PRIMARY KEY CLUSTERED 
(
	[sysid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[RegisteredServers] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[RegisteredServers] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[RegisteredServers] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[RegisteredServers] TO [v3dsa] AS [dbo]
GO
/****** Object:  Table [v3d].[RegistrationHistory]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[RegistrationHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[sysid] [nvarchar](256) NOT NULL,
	[name] [nvarchar](32) NOT NULL,
	[op] [nvarchar](32) NOT NULL,
	[status] [nvarchar](16) NOT NULL,
	[msg] [nvarchar](512) NOT NULL
) ON [PRIMARY]

GO
/****** Object:  Table [v3d].[Versions]    Script Date: 11/14/2013 1:54:19 PM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_parseString]    Script Date: 11/14/2013 1:54:19 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE FUNCTION [v3d].[fn_parseString]
(
@STRING NVARCHAR(MAX), 
@DELIMITER char(1)
)
RETURNS TABLE 
AS
RETURN
(
  WITH Results AS
		(SELECT	1 AS Ordinal,
				LTRIM(LEFT(@STRING, CHARINDEX(@DELIMITER, @STRING + @DELIMITER)-1)) AS StringValue,
				CONVERT(NVARCHAR(MAX), RIGHT(@STRING + @DELIMITER, len(@STRING) - charindex(@DELIMITER, @STRING+@DELIMITER) + 1)) as Remaining
		UNION ALL
		SELECT Ordinal+1,
				LTRIM(LEFT(Remaining, charindex(@DELIMITER, Remaining)-1)),
				RIGHT(Remaining, LEN(Remaining) - charindex(@DELIMITER, Remaining))
		FROM	Results
		WHERE	LEN(Remaining) > 0)

   SELECT	Ordinal, StringValue FROM	Results
)



GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_SERIAL_NUMBER]  DEFAULT (newid()) FOR [sysid]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_INSTALL_DATE]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_CURRENT_DATE]  DEFAULT (sysdatetimeoffset()) FOR [sysdate]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_Install_expirationDate]  DEFAULT (dateadd(day,(10),sysdatetimeoffset())) FOR [expdate]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_ENABLED]  DEFAULT ((1)) FOR [enabled]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_MESSAGE]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_INSTALL_COUNT]  DEFAULT ((0)) FOR [cnt]
GO
ALTER TABLE [v3d].[Install] ADD  CONSTRAINT [DF_Install_ls]  DEFAULT ('') FOR [ls]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_V3D_LICENSES_LICENSE_KEY]  DEFAULT ('') FOR [key]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_Licenses_transactionId]  DEFAULT ('') FOR [id]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_Licenses_tag]  DEFAULT ((0)) FOR [cnt]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_LICENSES_DATE_ISSUED]  DEFAULT (sysdatetimeoffset()) FOR [iss]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_V3D_LICENSES_DATE_ADDED]  DEFAULT (sysdatetimeoffset()) FOR [add]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_V3D_LICENSES_MESSAGE]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_Licenses_exp]  DEFAULT (sysdatetimeoffset()) FOR [exp]
GO
ALTER TABLE [v3d].[Licenses] ADD  CONSTRAINT [DF_Licenses_state]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [v3d].[LicenseSettings] ADD  CONSTRAINT [DF_Settings_subsystem]  DEFAULT ('') FOR [subsystem]
GO
ALTER TABLE [v3d].[LicenseSettings] ADD  CONSTRAINT [DF_Settings_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[LicenseSettings] ADD  CONSTRAINT [DF_Settings_chg]  DEFAULT ((0)) FOR [chg]
GO
ALTER TABLE [v3d].[LicenseStates] ADD  CONSTRAINT [DF_States_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[RegisteredServers] ADD  CONSTRAINT [DF_RegisteredServers_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[RegisteredServers] ADD  CONSTRAINT [DF_RegisteredServers_ip]  DEFAULT ('') FOR [ip]
GO
ALTER TABLE [v3d].[RegisteredServers] ADD  CONSTRAINT [DF_RegisteredServers_ins]  DEFAULT (sysdatetimeoffset()) FOR [ins]
GO
ALTER TABLE [v3d].[RegisteredServers] ADD  CONSTRAINT [DF_RegisteredServers_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_sysid]  DEFAULT ('') FOR [sysid]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_op]  DEFAULT ('') FOR [op]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_status]  DEFAULT ('') FOR [status]
GO
ALTER TABLE [v3d].[RegistrationHistory] ADD  CONSTRAINT [DF_RegistrationHistory_msg]  DEFAULT ('') FOR [msg]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Expiration date for the licensing system (NULL = infinite, Default is 10 days from current)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'expdate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Number of licenses issued' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'cnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'License server this system is registered to' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'ls'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains installation information for the Viatronix Licensing System' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'License Key' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'key'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Transaction id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Number of licenses for the transaction id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'cnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the license was issued' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'iss'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'The user who issued the license' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'by'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the license was added to the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'add'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Message used for expired, disabled or locked fields to indictae why' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the license expires' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'exp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'State of licenses ( 0-Avail, 1-InUse, 2-Locked, 4-Disabled, 8-Expired )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all licenses installed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Licenses'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Name of the setting' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseSettings', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Value of the setting' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseSettings', @level2type=N'COLUMN',@level2name=N'value'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Flag to determine if the setting is changable by a user' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseSettings', @level2type=N'COLUMN',@level2name=N'chg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all settings used within the Viatronix Licensing System' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseSettings'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseSettings'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'State id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseStates', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Values of state' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseStates', @level2type=N'COLUMN',@level2name=N'value'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description of the license state' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseStates', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the license states ' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseStates'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'LicenseStates'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'System id of the registered server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers', @level2type=N'COLUMN',@level2name=N'sysid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'System name of the registered server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'IP Address of the registered server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers', @level2type=N'COLUMN',@level2name=N'ip'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Installed date of the registered server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers', @level2type=N'COLUMN',@level2name=N'ins'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the server was registered' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of all Viatronix Enterprise Serviers registered with this license server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegisteredServers'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Id associated with this record' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegistrationHistory', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a record of all registration that has occured on this License Server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegistrationHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermnissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'RegistrationHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of object version used to return the ver attribute in the get procedures.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
