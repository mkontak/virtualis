USE [System]
GO
/****** Object:  User [Apollo\mkontak]    Script Date: 3/17/2014 12:20:38 PM ******/
CREATE USER [Apollo\mkontak] FOR LOGIN [APOLLO\mkontak] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  User [v3dsa]    Script Date: 3/17/2014 12:20:38 PM ******/
CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  User [v3duser]    Script Date: 3/17/2014 12:20:38 PM ******/
CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  Schema [v3d]    Script Date: 3/17/2014 12:20:38 PM ******/
CREATE SCHEMA [v3d]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkDatabase]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/15/2013
-- Description:	Database check
-- =============================================
CREATE PROCEDURE [v3d].[sp_checkDatabase]
(
@DATABASE AS NVARCHAR(64) = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  BEGIN TRY

    IF @DATABASE IS NOT NULL AND LEN(@DATABASE) > 0 
    BEGIN

      -- =================================================================
      -- Get the actual table name since I will allow case insenstivity
      -- ==================================================================
      IF NOT EXISTS (SELECT * FROM master.sys.sysdatabases AS Tables WHERE [name] = @DATABASE )
        EXEC [master].[dbo].[sp_throwNotFoundException] 'Database'
 
      PRINT '================================================================================================='
      PRINT 'Checking [' + @DATABASE +'] Database '
      PRINT ''

      DBCC CHECKDB ( @DATABASE )

      PRINT ''
      PRINT '================================================================================================='

    END
    ELSE
    BEGIN
      DECLARE DATABASE_CURSOR CURSOR FOR SELECT  [name] FROM master.sys.sysdatabases 

      PRINT '================================================================================================='

      OPEN DATABASE_CURSOR


      FETCH  NEXT  FROM DATABASE_CURSOR INTO @DATABASE

      WHILE @@FETCH_STATUS = 0
      BEGIN
  
        PRINT 'Checking [' + @DATABASE +'] Database '
        PRINT ''

        DBCC CHECKDB ( @DATABASE )

        PRINT ''
        PRINT '================================================================================================='

        FETCH  NEXT  FROM DATABASE_CURSOR INTO @DATABASE

      END

      CLOSE DATABASE_CURSOR
      DEALLOCATE DATABASE_CURSOR
	  END

  END TRY
  BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_checkDatabase'
  END CATCH

   
END



GO
GRANT EXECUTE ON [v3d].[sp_checkDatabase] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkDatabase] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkExpirationDate]    Script Date: 3/17/2014 12:20:38 PM ******/
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
	DECLARE @EXPDATE AS DATETIME
  DECLARE @SUCCESS AS BIT

  BEGIN TRY

	  -- Set the success return value to true
	  SET @SUCCESS = 1

	  -- Get the previous date saved in the V3D table and the installed system id
    SELECT @EXPDATE = [v3d].[Install].[expdate] FROM [v3d].[Install]

    -- ============================================================================
	  -- Check to make sure that the system has not expired. If the date is NULL then
    -- the system never expires.
    -- =============================================================================
    IF @EXPDATE IS NOT NULL AND @EXPDATE < SYSDATETIMEOFFSET()
	  BEGIN

		  -- Set the success to false
		  SET @SUCCESS = 0

		  -- Disable V3D
		  UPDATE [v3d].[Install] SET [v3d].[Install].enabled = 0, [v3d].[Install].msg = 'System has expired'

	  END

	-- Return the success flag
	RETURN @SUCCESS

  END TRY
  BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_cehckExpirationDate'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_checkExpirationDate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_checkExpirationDate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_checkSystem]    Script Date: 3/17/2014 12:20:38 PM ******/
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
  
    --=====================================================================================
    -- If the flag to check the ID is not set we check the internal parameter otherwise 
    -- the id will be check.
    --====================================================================================
    IF @CHECK_ID = 0 
    BEGIN
    
      DECLARE @EXCHK AS NVARCHAR(16)
    
      SET @EXCHK = [v3d].[fn_getSetting](N'ExcludeIdCheck', 'True')
    
      IF UPPER(@EXCHK) <> 'TRUE'
        SET @CHECK_ID = 1
        
       
    END
    
      
    
	  -- Local variables
	  DECLARE @ENABLED AS BIT


    -- Get the current state of the V3D system
    SELECT @ENABLED = [v3d].[Install].[enabled] FROM [v3d].[Install]
    
    -----------------------------------------------------------------------------------
    -- If the system is currently enabled them we need to make sure nothing has changed
    -----------------------------------------------------------------------------------
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
    
    IF @ENABLED = 0
      EXEC [master].[dbo].[sp_throwException] 900980, 'V3D System' 
    
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
/****** Object:  StoredProcedure [v3d].[sp_checkSystemDate]    Script Date: 3/17/2014 12:20:38 PM ******/
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
	DECLARE @PREV_DATE AS DATETIME
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
/****** Object:  StoredProcedure [v3d].[sp_checkSystemId]    Script Date: 3/17/2014 12:20:38 PM ******/
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
  EXEC [v3d].[sp_getsystemid] 
    @systemid = @SYS_ID OUTPUT

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
/****** Object:  StoredProcedure [v3d].[sp_createNamedEvent]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_createNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_createNamedEvent]
GO
GRANT EXECUTE ON [v3d].[sp_createNamedEvent] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_createNamedEvent] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_decrypt]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_decrypt]
	@value [nvarchar](4000),
	@decryptedValue [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_decrypt]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteDicomHost]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 02/20/2014
-- Description: Delete the dicom hsot setting
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteDicomHost]
@ID NVARCHAR(256),
@NAME NVARCHAR(64) = NULL,
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
    EXEC [Security].[v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'StorageManagement'
	 
    IF @NAME IS NULL 
    BEGIN

      DECLARE @IDOC AS INTEGER
 
        -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT  @NAME       = [name]
            FROM OPENXML(@IDOC, N'/dicomhost', 1) WITH 
            ( 
              [name]    nvarchar(64)
            )


      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END

    -- Make sure the dicom host name was supplied
    IF @NAME IS NULL OR LEN(@NAME) = 0
       EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host Name' 
 
     --- Delete the user config
    DELETE FROM [v3d].[DicomHosts] WHERE [name] = @NAME
  
    --- Returns the rowcount
    RETURN @@ROWCOUNT
  

        
	END TRY
	BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteDicomHost'
	  
	END CATCH

  
END






GO
GRANT EXECUTE ON [v3d].[sp_deleteDicomHost] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteDicomHost] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteSystemConfig]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 09/28/2011
-- Description: Delete the configuration setting
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteSystemConfig]
@ID NVARCHAR(256),
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
    EXEC [Security].[v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'StorageManagement'
	 
    IF @NAME IS NULL 
    BEGIN

      DECLARE @IDOC AS INTEGER
 
        -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT  @NAME       = [name]
            FROM OPENXML(@IDOC, N'/systemconfig', 1) WITH 
            ( 
              [name]    nvarchar(64)
            )


      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END

    -- Make sure the configurtaion name was supplied
    IF @NAME IS NULL OR LEN(@NAME) = 0
       EXEC [master].[dbo].[sp_throwInvalidDataException]  'Configuration Name' 
 
     --- Delete the user config
    DELETE FROM [v3d].[SystemConfigs] WHERE [name] = @NAME
  
    --- Returns the rowcount
    RETURN @@ROWCOUNT
  

        
	END TRY
	BEGIN CATCH
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteSystemConfig'
	  
	END CATCH

  
END






GO
GRANT EXECUTE ON [v3d].[sp_deleteSystemConfig] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteSystemConfig] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_encrypt]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_encrypt]
	@value [nvarchar](4000),
	@encryptedValue [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_encrypt]
GO
/****** Object:  StoredProcedure [v3d].[sp_errorHandler]    Script Date: 3/17/2014 12:20:38 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getAgentJob]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		   Mark Kontak
-- Create date:  3/27/2012
-- Description: Gets the agent jobs
-- =============================================
CREATE PROCEDURE  [v3d].[sp_getAgentJob]
(
@ID       NVARCHAR(256),            -- Context Id
@QUERY    XML = '<agentjob />',     -- Query xml (Ignored)
@RESULTS  XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT


  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'SystemManagement'

    SET @RESULTS = ( SELECT * FROM [v3d].[AgentJobs] FOR XML RAW('agentjob'), TYPE, ROOT('collection') )


    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="agentjob" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"agentjob"} into (/collection)[1]')   
  
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getAgentJob'
  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getAgentJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getAgentJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getApplication]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================================================================================================
-- Author:		  Mark Kontak
-- Create date: 10/4/2011
-- Description:	Gets the all the applications in the system. By default only the installed are returned. The list is ordered by the 
--              priority number.
-- ===================================================================================================================================
CREATE PROCEDURE [v3d].[sp_getApplication]
(
@ID         NVARCHAR(256),                          -- Context ID
@APP        NVARCHAR(32) = NULL,                    -- App Id
@QUERY      XML = '<application inst="true" />',    -- Query xml
@RESULTS    XML OUTPUT                              -- Result set
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

    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the UID was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single STUDY item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @APP IS NULL 
    BEGIN
    
      DECLARE @IDOC         AS INT
      DECLARE @UIDS         AS NVARCHAR(MAX)
      DECLARE @DESCRIPTION  AS NVARCHAR(512)
      DECLARE @DEFAULT      AS BIT
      DECLARE @SERIES       AS XML
      DECLARE @INSTALLED    AS BIT

     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      -- Extract the user information 
      SELECT  @APP              = [id],
              @DEFAULT          = [default],
              @INSTALLED        = [inst],
              @UIDS             = [uids],
              @DESCRIPTION      = [desc]
            FROM OPENXML(@IDOC, N'/application', 1) WITH 
            ( 
              [desc]            nvarchar(256),
              [uids]            nvarchar(max),
              [default]         bit,
              [inst]            bit,
              [id]              nvarchar(32)
           )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC

    END -- END If @LOGIN IS NULL

    IF @UIDS IS NOT NULL AND LEN(@UIDS) > 0 
    BEGIN

      EXEC [Storage].[v3d].[sp_getApplications] 
        @ID = @ID,
        @UIDS = @UIDS,
        @RESULTS = @RESULTS OUTPUT

    END
    ELSE
    BEGIN  
    
      DECLARE @SELECT AS NVARCHAR(MAX)
      DECLARE @WHERE AS NVARCHAR(MAX)
      DECLARE @PARAMETERS AS NVARCHAR(MAX)

      SELECT  @APP    = ISNULL(@APP,''),
              @WHERE = '',
              @SELECT = 'SET @RESULTS =  ( SELECT [id], [name], [pri], [desc], [mask], [default], [inst], [v3d].[fn_replaceSettings]([url]) AS [url], [launch], [v3d].[fn_getVersion](''Applications'') AS [ver] FROM [System].[v3d].[Applications] '
            

      IF LEN(@APP) > 0 
        SET @WHERE = ' [id] = @APP AND'

      IF @INSTALLED IS NOT NULL
        SET @WHERE = @WHERE + ' [inst] = @INSTALLED AND'

      IF @DEFAULT IS NOT NULL
        SET @WHERE = @WHERE + ' [default] = @DEFAULT AND'

      DECLARE @LEN AS INT
    
      SET @LEN = LEN(@WHERE)
    
      IF @LEN > 0
        SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)

      SET @SELECT = @SELECT + ' ORDER BY [pri] FOR XML RAW(''application''), TYPE, ROOT(''collection''))'
    
      SET @PARAMETERS = N'@RESULTS        XML OUTPUT, ' +
                        N'@APP            NVARCHAR(32), ' + 
                        N'@DEFAULT        BIT, ' +
                        N'@INSTALLED      BIT, ' +
                        N'@COUNT          INT OUTPUT'

   
      EXEC  sp_executesql 
              @SELECT,
              @PARAMETERS,
              @APP           = @APP,
              @INSTALLED     = @INSTALLED,
              @DEFAULT       = @DEFAULT,
              @COUNT         = @RC OUTPUT,
              @RESULTS       = @RESULTS OUTPUT

      IF @RESULTS IS NULL
        SET @RESULTS = '<collection id="application" />'
      ELSE
        SET @RESULTS.modify('insert attribute id {"application"} into (/collection)[1]')   

   END


   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getApplication'

  END CATCH
 
    
END





GO
GRANT EXECUTE ON [v3d].[sp_getApplication] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getApplication] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getAvailableStore]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_getAvailableStore]
	@store [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_getAvailableStore]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDatabase]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- ===============================================================
-- Author:		  Mark Kontak
-- Create date: 9/18/2013
-- Description:	Creates the list of databases that are Viatronix.
-- ===============================================================
CREATE PROCEDURE [v3d].[sp_getDatabase]
@ID AS NVARCHAR(256),                 -- Context Id
@QUERY AS XML = NULL,                 -- Query parameters (Not Used)
@RESULTS AS XML OUTPUT                -- Xml results
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @DATABASES AS TABLE ( [name] NVARCHAR(32) )

  DECLARE @SELECT AS NVARCHAR(MAX)
  DECLARE @SYS_SELECT AS NVARCHAR(MAX)
  DECLARE @NAME AS NVARCHAR(32)
  DECLARE @FILE AS NVARCHAR(512)
  DECLARE DATABASE_CURSOR CURSOR FOR SELECT [name] FROM [master].[sys].[sysdatabases]
  DECLARe @COUNT AS INT

  BEGIN TRY

    SELECT @SELECT = 'SET @RESULTS = ( SELECT '

    OPEN DATABASE_CURSOR

    FETCH NEXT FROM DATABASE_CURSOR INTO @NAME   

    WHILE @@FETCH_STATUS = 0   
    BEGIN   
 
      BEGIN TRY

        SET @SYS_SELECT = 'SELECT ''' + @NAME + ''' FROM ' + @NAME + '.sys.extended_properties WHERE [name] = ''Viatronix'''

        INSERT INTO @DATABASES ( [name] )  EXEC ( @SYS_SELECT )

        IF @@ROWCOUNT = 1
        BEGIN

          SELECT @SELECT = @SELECT + '[' + @NAME + '].[v3d].[fn_getDatabase](), '

        END

      END TRY
      BEGIN CATCH
        -- Ignore
      END CATCH


      FETCH NEXT FROM DATABASE_CURSOR INTO @NAME     

    END   

    CLOSE DATABASE_CURSOR
    DEALLOCATE DATABASE_CURSOR


    SELECT @SELECT = SUBSTRING(@SELECT, 0, LEN(@SELECT) ) + ' FOR XML PATH(''''), TYPE, ROOT(''collection'') )'


    DECLARE @PARAMETERS AS NVARCHAR(MAX)

    SET @PARAMETERS = N'@RESULTS XML OUTPUT'

   
    EXEC  sp_executesql 
              @SELECT,
              @PARAMETERS,
              @RESULTS  = @RESULTS OUTPUT



    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="database" />'
    ELSE
       SET @RESULTS.modify('insert attribute id {"database"} into (/collection)[1]')   

    RETURN 1

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDatabase'

  END CATCH

END




GO
GRANT EXECUTE ON [v3d].[sp_getDatabase] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDatabase] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDicomHost]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		   Mark Kontak
-- Create date:  3/27/2012
-- Description: Gets the agent jobs
-- =============================================
CREATE PROCEDURE  [v3d].[sp_getDicomHost]
(
@ID       NVARCHAR(256),            -- Context Id
@QUERY    XML = '<dicomhost />',     -- Query xml (Ignored)
@RESULTS  XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT


  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'SystemManagement'


    DECLARE @IDOC         AS INT
    DECLARE @NAME         AS NVARCHAR(MAX)
    DECLARE @AETITLE      AS NVARCHAR(MAX)
    DECLARE @SERVER       AS NVARCHAR(MAX)
    DECLARE @SERVICES     AS INT
    DECLARE @ALL          AS INT

    SELECT @ALL = [v3d].[fn_getMaskBit]('services','all')

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @NAME              = [name],
            @AETITLE           = [aetitle],
            @SERVER            = [server],
            @SERVICES          = [services]
          FROM OPENXML(@IDOC, N'/dicomhost', 1) WITH 
          ( 
            [name]            nvarchar(MAX),
            [aetitle]         nvarchar(MAX),
            [server]          nvarchar(MAX),
            [services]        int
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    DECLARE @SELECT AS NVARCHAR(MAX)
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @PARAMETERS AS NVARCHAR(MAX)

    SELECT  @SERVICES    = ISNULL(@SERVICES,@ALL),
            @WHERE = '',
            @SELECT = 'SET @RESULTS =  ( SELECT [name], [desc], [aetitle], [server], [services], [port], [attributes], [v3d].[fn_getVersion](''DicomHosts'') AS [ver] FROM [System].[v3d].[DicomHosts] '
            
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]')
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@SERVER', @SERVER, '[server]')
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@AETITLE', @AETITLE, '[aetitle]')

    IF @SERVICES IS NOT NULL AND @SERVICES > 0
      SET @WHERE = @WHERE + ' ( [services] | @SERVICES ) = [services] AND '


    DECLARE @LEN AS INT
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)

    SET @SELECT = @SELECT + '  FOR XML RAW(''dicomhost''), TYPE, ROOT(''collection''))'
    
    SET @PARAMETERS = N'@RESULTS       XML OUTPUT, ' +
                      N'@NAME          NVARCHAR(MAX), ' + 
                      N'@SERVER        NVARCHAR(MAX), ' +
                      N'@AETITLE       NVARCHAR(MAX), ' +
                      N'@SERVICES      INT, ' +
                      N'@COUNT          INT OUTPUT'

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @NAME         = @NAME,
            @SERVER       = @SERVER,
            @AETITLE      = @AETITLE,
            @SERVICES     = @SERVICES,
            @COUNT        = @RC OUTPUT,
            @RESULTS      = @RESULTS OUTPUT

    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="dicomhost" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"dicomhost"} into (/collection)[1]')   
  
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDicomHosts'
  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getDicomHost] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDicomHost] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDicomHostNative]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		   Mark Kontak
-- Create date:  3/27/2012
-- Description: Gets the agent jobs
-- =============================================
CREATE PROCEDURE  [v3d].[sp_getDicomHostNative]
(
@ID       NVARCHAR(256),            -- Context Id
@QUERY    XML = '<dicomhost />',     -- Query xml (Ignored)
@RESULTS  NVARCHAR(MAX) OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT
  DECLARE @XML AS XML


  BEGIN TRY
  
    
    EXEC @RC = [v3d].[sp_getDicomHost]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT


    SET @RESULTS = CONVERT(NVARCHAR(MAX),@XML)
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDicomHosts'
  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getDicomHostNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDicomHostNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getModality]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		   Mark Kontak
-- Create date:  3/27/2012
-- Description: Gets the system setting
-- =============================================
CREATE PROCEDURE  [v3d].[sp_getModality]
(
@ID       NVARCHAR(256),            -- Context Id
@CODE     NVARCHAR(10) = NULL,      -- Modality code
@QUERY    XML = '<modality />',     -- Query xml
@RESULTS  XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @RC AS INT


  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the login was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single USER item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @CODE IS NULL
    BEGIN
    
     
      DECLARE @IDOC AS INT
     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      SELECT  @CODE  = [code]
              FROM OPENXML(@IDOC, N'/modality', 3) 
              WITH 
              (
                [code]   nvarchar(10)
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC
      
    END -- END If @NAME IS NULL
  
     
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    SELECT  @CODE = ISNULL(@CODE,''),
            @WHERE = '',      
            @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Modalities'') AS [ver] FROM [v3d].[Modalities] '


     SET @WHERE = [System].v3d.fn_buildStringComparison(N'@CODE', @CODE, '[code]')
          

     DECLARE @LEN AS INT
      
      SET @LEN = LEN(@WHERE)
      
      IF @LEN > 0
      BEGIN
        SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      END
 

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = ( ' + @SELECT  + ' FOR XML RAW(''modality''), TYPE , ROOT(''collection'')  ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS    XML OUTPUT, '    +
                      N'@CODE       NVARCHAR(10), '  + 
                      N'@COUNT INT OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @CODE         = @CODE,
            @COUNT        = @RC OUTPUT,
            @RESULTS      = @RESULTS OUTPUT
      

    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="modality" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"modality"} into (/collection)[1]')   
  
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getModality'
  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getModality] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getModality] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getService]    Script Date: 3/17/2014 12:20:38 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getSite]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/26/2010
-- Description:	Retrieves the Site as XML
-- =============================================
CREATE PROCEDURE [v3d].[sp_getSite]
(
@ID       NVARCHAR(256),             -- Context Id
@QUERY    XML = '<site />',          -- Query xml
@RESULTS  XML OUTPUT                 -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @COLLECTION AS NVARCHAR(512)
  DECLARE @RC AS INT

  SET @COLLECTION = ''
  
  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
       @ID = @ID,
      @MASK = 'StorageManagement'

    -- ===========================================
    -- If there is no site then
    -- 900981 V3D System : Not Installed
    -- ===========================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[Site] )
      EXEC [master].[dbo].[sp_throwException] 900981, 'V3D System'

   
    SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion]('Site') AS [ver] FROM [v3d].[Site] FOR XML RAW('site'), TYPE, ROOT('collection') )
    
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="site" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"site"} into (/collection)[1]')   
     
      
    RETURN @@ROWCOUNT

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getSite'

  
  END CATCH
  

END





GO
GRANT EXECUTE ON [v3d].[sp_getSite] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSite] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getStore]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 3/29/2012
-- Description:	Gets the storage 
-- =============================================
CREATE PROCEDURE [v3d].[sp_getStore]
(
@ID       NVARCHAR(256),           -- Context Id
@QUERY    XML = '<store />',       -- Query xml
@RESULTS  XML OUTPUT               -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
 
  DECLARE @RC AS INT


  BEGIN TRY
      
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    
     
    DECLARE @IDOC AS INT
    DECLARE @NAME AS NVARCHAR(MAX)
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    SELECT  @NAME       = [name]
            FROM OPENXML(@IDOC, N'/store', 3) 
            WITH 
            (
              [name]        nvarchar(MAX)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
      

    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT AS NVARCHAR(MAX)
    
    SET @WHERE = ''
    SET @WHERE =  [v3d].[fn_buildStringComparison]('@NAME', @NAME, '[v3d].[Stores].[name]')  

    DECLARE @LEN AS INT
  
    SET @LEN = LEN(@WHERE)
  
    IF @LEN > 0
      SET @WHERE = 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
  
    
    SET @SELECT = 'SET @RESULTS = ( SELECT [v3d].[Stores].*,  [DriveInfo].[cap], [DriveInfo].[used], [DriveInfo].[avail], [DriveINfo].[msg] FROM
                      [v3d].[Stores] INNER JOIN [v3d].[fn_getDriveInfo]() AS [DriveInfo] ON [DriveInfo].[name] = [v3d].[Stores].[name] ' + 
                      @WHERE + N' FOR XML RAW(''store''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    


    DECLARE @PARAMETERS AS NVARCHAR(MAX)
    
    SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                      N'@NAME               NVARCHAR(MAX), ' +
                      N'@COUNT              INT OUTPUT ' 

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @NAME             = @NAME,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT

    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="store'
    ELSE
      SET @RESULTS.modify('insert attribute id {"store"} into (/collection)[1]')   
  
   
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getStore'

  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getStore] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getStore] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getStudyGroup]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 3/20/2012
-- Description:	Returns the list of groups
-- =============================================
CREATE PROCEDURE [v3d].[sp_getStudyGroup]
(
@ID       NVARCHAR(256),              -- Context Id
@NAME     NVARCHAR(32) = NULL,        -- GROUP NAME to retrieve
@QUERY    XML = '<studygroup />',          -- Query xml
@RESULTS  XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT

  
  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the login was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single USER item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @NAME IS NULL
    BEGIN
    
     
      DECLARE @IDOC AS INT

      --DECLARE @LOGIN AS NVARCHAR(16)
      DECLARE @DESC           AS NVARCHAR(128)

     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      SELECT  @NAME          = [name], 
              @DESC           = [desc]
              FROM OPENXML(@IDOC, N'/studygroup', 3) 
              WITH 
              (
                [name]                nvarchar(32),
                [desc]                nvarchar(128)
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END -- END If @LOGIN IS NULL

    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SET @WHERE = ''      
    SET @SELECT = ''
   
    -- =============================================================================================================================
    -- If the @LOGIN is specified then we are looking for a specific user and a single <user /> ml node will be returned if found. 
    -- Otherwise we are looking for a collection so a <collection><user ... /> ... </collection> will be returned.
    -- =============================================================================================================================
    IF @NAME IS NOT NULL
    BEGIN
      SET @WHERE = ' [name] = @NAME AND '
    END
    ELSE
    BEGIN
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]')
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@DESC', @DESC, '[desc]')
      
      
    END


    SET @SELECT = 'SELECT * FROM [v3d].[StudyGroups] '
    
    DECLARE @LEN AS INT
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT + ' FOR XML RAW(''studygroup''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS        XML OUTPUT, ' +
                      N'@NAME           NVARCHAR(32), ' + 
                      N'@DESC           NVARCHAR(128), ' +
                      N'@COUNT          INT OUTPUT'

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @NAME           = @NAME,
            @DESC           = @DESC,
            @COUNT          = @RC OUTPUT,
            @RESULTS        = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="studygroup" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"studygroup"} into (/collection)[1]')   
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getStudyGroup'
  
  END CATCH
  

END





GO
GRANT EXECUTE ON [v3d].[sp_getStudyGroup] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getStudyGroup] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSystemConfig]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Gets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_getSystemConfig]
(
@ID       NVARCHAR(256),               -- Context Id
@NAME     NVARCHAR(32) = NULL,         -- @NAME
@QUERY    XML = '<systemconfig />',             -- Query xml
@RESULTS  XML OUTPUT                   -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @XML AS XML
  DECLARE @RC AS INT

  
  SET @RC = 1

  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    IF  @NAME IS NULL 
    BEGIN
 
       DECLARE @IDOC AS INT

  
     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      SELECT  @NAME    = [name]
              FROM OPENXML(@IDOC, N'/systemconfig', 3) 
              WITH 
              (
                [name]      nvarchar(32)
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC
    
    END
    
        
    IF @NAME IS NULL OR LEN(@NAME) = 0
    BEGIN

     SET @RESULTS = ( SELECT *, [System].[v3d].[fn_getVersion]('collection') AS [ver] 
            FROM [v3d].[SystemConfigs]   FOR XML RAW('systemconfig'), TYPE, ROOT('collection') )

    END
    ELSE
    BEGIN
      SET @RESULTS = ( SELECT *, [System].[v3d].[fn_getVersion]('collection') AS [ver] 
            FROM [v3d].[SystemConfigs] WHERE  [name] = @NAME  FOR XML RAW('systemconfig'), TYPE, ROOT('collection') )

    END
  
 
   IF @RESULTS IS NULL 
   BEGIN
      SELECT @RC = 0,
             @RESULTS = '<collection id="systemconfig" />'
   END
   ELSE
   BEGIN
      SET  @RESULTS.modify('insert attribute id {"systemconfig"} into (/collection)[1]')   
   END
      
      

            
    RETURN @RC
    
    
   END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getConfig'

  
  END CATCH
   
    
END




GO
GRANT EXECUTE ON [v3d].[sp_getSystemConfig] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSystemConfig] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSystemConfigNative]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Gets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_getSystemConfigNative]
(
@ID       NVARCHAR(256),               -- Context Id
@QUERY    XML = '<systemconfig />',    -- Query xml
@RESULTS  NVARCHAR(MAX) OUTPUT         -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @XML AS XML
  DECLARE @RC AS INT

  
  SET @RC = 1

  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC @RC = [v3d].[sp_getSystemConfig]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT

    SET @RESULTS = CONVERT(NVARCHAR(MAX), @XML)
            
    RETURN @RC
    
    
   END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getSystemConfigNative'

  
  END CATCH
   
    
END




GO
GRANT EXECUTE ON [v3d].[sp_getSystemConfigNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSystemConfigNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSystemId]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_getSystemId]
	@systemid [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_getSystemId]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSystemSetting]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		   Mark Kontak
-- Create date:  3/27/2012
-- Description: Gets the system setting
-- =============================================
CREATE PROCEDURE  [v3d].[sp_getSystemSetting]
(
@ID       NVARCHAR(256),            -- Context Id
@NAME     NVARCHAR(32) = NULL,      -- Setting name
@QUERY    XML = '<systemsetting />',       -- Query xml
@RESULTS  XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @SUBSYSTEM NVARCHAR(256)
  DECLARE @EXPAND AS BIT


  DECLARE @RC AS INT


  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the login was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single USER item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @NAME IS NULL
    BEGIN
    
     
      DECLARE @IDOC AS INT

      --DECLARE @LOGIN AS NVARCHAR(16)
 
     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      SELECT  @SUBSYSTEM  = [subsystem],
              @NAME       = [name],
			        @EXPAND	    = [expand]
              FROM OPENXML(@IDOC, N'/systemsetting', 3) 
              WITH 
              (
                [subsystem]   nvarchar(256),
                [name]        nvarchar(32),
				        [expand]	    bit
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC
      
    END -- END If @NAME IS NULL
  
     
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    SELECT  @NAME = ISNULL(@NAME,''),
			      @EXPAND = ISNULL(@EXPAND,0),
            @WHERE = '',     
            @SELECT = ''


    IF @EXPAND = 1
      SELECT @SELECT = 'SELECT [name], [subsystem], [v3d].[fn_getSetting]([name], [value]) AS [value], [desc], [chg] , @EXPAND AS [expand], [v3d].[fn_getVersion](''SystemSettings'') AS [ver] FROM [v3d].[SystemSettings] '
    ELSE
      SELECT @SELECT = 'SELECT [name], [subsystem], [value], [desc], [chg] , @EXPAND AS [expand], [v3d].[fn_getVersion](''SystemSettings'') AS [ver] FROM [v3d].[SystemSettings] '


    IF LEN(@NAME) > 0
    BEGIN
      SET @WHERE = ' [name] = @NAME AND '
    END
    ELSE
    BEGIN
    
      SET @WHERE = [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]')
        
      SET @WHERE = [System].v3d.fn_buildStringComparison(N'@SUBSYSTEM', @SUBSYSTEM, '[subsystem]')
    

    
    END

     DECLARE @LEN AS INT
      
      SET @LEN = LEN(@WHERE)
      
      IF @LEN > 0
      BEGIN
        SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      END
 

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = ( ' + @SELECT  + ' FOR XML RAW(''systemsetting''), TYPE , ROOT(''collection'')  ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS    XML OUTPUT, '    +
                      N'@NAME       NVARCHAR(64), '  +
                      N'@EXPAND     BIT, ' +
                      N'@SUBSYSTEM  NVARCHAR(256), ' + 
                      N'@COUNT INT OUTPUT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @NAME         = @NAME,
            @SUBSYSTEM    = @SUBSYSTEM,
            @EXPAND       = @EXPAND,
            @COUNT        = @RC OUTPUT,
            @RESULTS      = @RESULTS OUTPUT
      

    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="systemsetting" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"systemsetting"} into (/collection)[1]')   
  
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getSetting'

  
  END CATCH

  
END





GO
GRANT EXECUTE ON [v3d].[sp_getSystemSetting] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSystemSetting] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getTemplate]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the processing templates
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getTemplate]
(
@ID AS NVARCHAR(256),                 -- Context Id
@TID AS NVARCHAR(64) = NULL,         -- Null 
@QUERY AS XML = '<template />',      -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC AS INT


  BEGIN TRY
  
   
   
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'
  
    -- ============================================================================================================
    -- If the template id is not null
    -- ============================================================================================================
    IF @TID IS NULL 
    BEGIN
    
     
      DECLARE @IDOC AS INT

      DECLARE @APP AS NVARCHAR(32)
      DECLARE @MOD AS NVARCHAR(32)
      DECLARE @DEFAULT AS BIT
      DECLARE @LEN AS INT             

 
     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY


      SELECT  @TID              = [id],
              @APP              = [app], 
              @MOD              = [mod],
              @DEFAULT          = [default]
              FROM OPENXML(@IDOC, N'/template', 3) 
              WITH 
              (
                [id]              nvarchar(32),
                [app]             nvarchar(32),
                [mod]             nvarchar(32),
                [default]         bit
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END -- END If @JID or @NAME is null


    SELECT @APP = ISNULL(@APP,'')
    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)
  
    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SET @WHERE = ''      
    SET @SELECT = ''
 
    
    -- =============================================================================================================================
    -- If the @LOGIN is specified then we are looking for a specific user and a single <user /> ml node will be returned if found. 
    -- Otherwise we are looking for a collection so a <collection><user ... /> ... </collection> will be returned.
    -- =============================================================================================================================
    IF @TID IS NOT NULL 
    BEGIN
      SET @WHERE = ' [id] = @TID '
    END
    ELSE
    BEGIN

      
      SET @WHERE = ''

      IF LEN(@APP) > 0 AND ( @MOD IS NOT NULL AND LEN(@MOD) > 0 )
      BEGIN
        IF EXISTS ( SELECT * FROM [v3d].[Templates] WHERE [app] = @APP AND [mod] = @MOD  )
          SET @WHERE = ' [app] = @APP AND [mod] = @MOD AND'
        ELSE
          SET @WHERE = ' [app] = @APP AND [mod] = '''' AND'
      END
      ELSE
      BEGIN
             
        SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@APP', @APP, '[app]')

        IF ( @MOD IS NOT NULL )
        BEGIN
          IF LEN(@MOD) = 0
            SET @WHERE = @WHERE + ' [mod] = '''' AND'
          ELSE
            SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@MOD', @MOD, '[mod]') 
        END

      END
      
      IF @DEFAULT IS NOT NULL
        SET @WHERE = @WHERE + ' [default] = @DEFAULT AND'

      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@TID', @TID, '[id]')

    END

    SET @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Templates'') AS [ver]  FROM [v3d].[Templates] '
       
           
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3) --+ @OR
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT +  ' FOR XML RAW(''template''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS XML OUTPUT, ' +
                      N'@APP NVARCHAR(32), ' + 
                      N'@MOD NVARCHAR(32), ' +
                      N'@TID NVARCHAR(64), ' +
                      N'@DEFAULT BIT, ' +
                      N'@COUNT INT OUTPUT ' 

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @APP      = @APP,
            @MOD      = @MOD,
            @TID      = @TID,
            @DEFAULT  = @DEFAULT,
            @COUNT    = @RC OUTPUT,
            @RESULTS  = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="template" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"template"} into (/collection)[1]')   
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getTemplate'

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getTemplate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getTemplate] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getTemplateId]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 9/13/2013
-- Description:	Gets the template id
-- =============================================
CREATE PROCEDURE [v3d].[sp_getTemplateId]
@TID NVARCHAR(32) OUTPUT,
@APP NVARCHAR(32),
@PROC NVARCHAR(256) = ''
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- =======================================================================================================
  -- If NO Template ID then we can assume conversion. We now need to get the next task from the default
  -- template of the application.
  -- =======================================================================================================
  IF @TID IS NULL OR LEN(@TID) = 0 
  BEGIN
      
    IF @APP IS NOT NULL AND LEN(@APP) > 0
    BEGIN

      -- ============================================================
      -- Attempt to get the default template for the applications
      -- ============================================================
      SELECT @TID = [id] FROM [System].[v3d].[Templates] WHERE [app] = @APP AND [default] = 1

      -- =============================================================================================
      -- If there is no default template for the application then we will take the first template 
      -- returned for the application.
      -- ==============================================================================================
      IF @@ROWCOUNT = 0 
      BEGIN

        DECLARE @DATABASE AS NVARCHAR(64)
        DECLARE @MESSAGE AS NVARCHAR(1024)

        SELECT  @DATABASE = DB_NAME(),
                @MESSAGE = 'No default template for ' + @APP + ' was found, defaulting to the first template entry for the application' 

        EXEC [master].[dbo].[sp_logErrorMessage] @DATABASE, 'sp_getTemplateId', 0, @MESSAGE

     
        -- =============================================
        -- Grab the first template for this application
        -- =============================================
        SELECT TOP 1 @TID = [id] FROM [System].[v3d].[Templates] WHERE [app] = @APP

        -- =================================
        -- No application templates found
        -- =================================
        IF @@ROWCOUNT = 0
        BEGIN

          SELECT @MESSAGE = 'No processing templates for ' + @APP + ' was found, stopping at conversion'

          EXEC [master].[dbo].[sp_logErrorMessage] @DATABASE, 'sp_getTemplateId', 0, @MESSAGE

       END
        
      END -- END ... If no default application template was found

    END -- END ... If Application was supplied
  
      
  END -- END ... If there is no template id associated with the job

END




GO
GRANT EXECUTE ON [v3d].[sp_getTemplateId] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getTemplateId] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertDicomHost]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 02/20/2014
-- Description: Inserts a dicom host item
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertDicomHost]
@ID NVARCHAR(256),
@RECORD XML
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @NAME NVARCHAR(64)
  DECLARE @SERVER NVARCHAR(64)
  DECLARE @AETITLE NVARCHAR(64)
  DECLARE @SERVICES INT
  DECLARE @DESC AS NVARCHAR(512)
  DECLARE @IDOC AS INTEGER
  DECLARE @PORT INT
  DECLARE @ATTRIBUTES AS XML

  BEGIN TRY
 
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'StorageManagement'
	 
      -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @NAME       = [name],
            @DESC       = [desc],
            @SERVER     = [server],
            @AETITLE    = [aetitle],
            @SERVICES   = [services],
            @PORT       = [port],
            @ATTRIBUTES = [attributes]
          FROM OPENXML(@IDOC, N'/dicomhost', 1) WITH 
          ( 
            [name]        nvarchar(64),
            [server]      nvarchar(64),
            [aetitle]     nvarchar(64),
            [services]    int,
            [port]        int,
            [desc]        nvarchar(512),
            [attributes]  xml './attributes'
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    

  
    -- ================================
    -- Make sure the port is specified
    -- ===============================
    IF @PORT IS NULL OR @PORT < 1
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host Port'

 
    -- ===========================================
    -- Make sure the dicom host name is specified
    -- ===========================================
    IF @NAME IS NULL OR LEN(@NAME) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host Name'

     -- ===========================================
    -- Make sure the dicom host server/ip is specified
    -- ===========================================
    IF @SERVER IS NULL OR LEN(@SERVER) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host Server Name'

    -- ===========================================
    -- Make sure the dicom host aetitle is specified
    -- ===========================================
    IF @AETITLE IS NULL OR LEN(@AETITLE) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host AE Title'


    -- ===================================================================================================================
    -- Make sure the AE Title does not already exist. there is an index in the table that does this, but this is a nicer
    -- exception.
    -- ===================================================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[DicomHosts] WHERE [aetitle] = @AETITLE )
     EXEC [master].[dbo].[sp_throwException] 900992, 'AE Title'

    IF @ATTRIBUTES IS NOT NULL
      SET @ATTRIBUTES = @ATTRIBUTES.query('/attributes/*')

    INSERT [v3d].[DicomHosts] ( [name], [desc], [port], [server], [aetitle], [attributes] ) 
    VALUES ( @NAME, @DESC, @PORT, @SERVER, @AETITLE, @ATTRIBUTES )

  
    RETURN @@ROWCOUNT
    
  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertDicomHost'
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_insertDicomHost] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertDicomHost] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertSystemConfig]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2011
-- Description: Sets a user/type config
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertSystemConfig]
@ID NVARCHAR(256),
@RECORD XML
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  
  DECLARE @NAME NVARCHAR(32)
  DECLARE @DESC AS NVARCHAR(512)
  DECLARE @IDOC AS INTEGER
  DECLARE @CONFIG AS XML

  BEGIN TRY
 
 	  -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
        @ID = @ID,
        @MASK = 'StorageManagement'
	 
      -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @NAME       = [name],
            @DESC       = [desc],
            @CONFIG     = [config]
          FROM OPENXML(@IDOC, N'/systemconfig', 1) WITH 
          ( 
            [name]    nvarchar(64),
            [desc]    nvarchar(512),
            [config]  xml './config'
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    

    SET @NAME = ISNULL(@NAME, '' )
 
    -- ===========================================
    -- Make config name is specified
    -- (900990) Configuration Name: Missing or invalid
    -- ===========================================
    IF LEN(@NAME) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Configuration Name'

    -- =====================================
    -- Make sure the config was specified
    -- ====================================
    IF @CONFIG IS NULL
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Config'
    ELSE
      SELECT @CONFIG = @CONFIG.query('./config/*')

 

 
    IF EXISTS ( SELECT * FROM  [v3d].[SystemConfigs]  WHERE  [name] = @NAME )
      UPDATE [v3d].[SystemConfigs] SET [config] = @CONFIG WHERE  [name] = @NAME
    ELSE
      INSERT [v3d].[SystemConfigs] ( [name], [desc], [config] ) VALUES ( @NAME, @DESC, @CONFIG )

  
    RETURN @@ROWCOUNT
    
  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertSystemConfig'
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_insertSystemConfig] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertSystemConfig] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_linkLicenseServer]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 11/7/2012
-- Description:	Links up a License server
-- =============================================
CREATE PROCEDURE [v3d].[sp_linkLicenseServer]
@SERVERNAME AS NVARCHAR(128)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- ==========================================
  -- Unlink the old license server if present
  -- =========================================
  EXEC [v3d].[sp_unlinkLicenseServer]

  IF @@SERVERNAME <> @SERVERNAME
  BEGIN

    DECLARE @SVRNAME AS SYSNAME

    SET @SVRNAME = CONVERT(SYSNAME,@SERVERNAME)

    -- =======================
    -- Add the linked server
    -- =======================
    EXEC sp_addlinkedserver 
      @server = @SVRNAME

    -- Get the server name (SYSNAME)
    SELECT @SVRNAME = name FROM [sys].[servers]

    -- ============================================
    -- The User name and Passwords need to match
    -- ============================================
    EXEC sp_addlinkedsrvlogin 
      @rmtsrvname = @SVRNAME,
      @useself = 'TRUE'
 
 
  END
END




GO
GRANT EXECUTE ON [v3d].[sp_linkLicenseServer] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_regExpMatch]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_regExpMatch]
	@ignoreCase [bit],
	@pattern [nvarchar](4000),
	@value [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_regExpMatch]
GO
GRANT EXECUTE ON [v3d].[sp_regExpMatch] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_regExpMatch] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_register]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================
-- Author:		  Mark Kontak
-- Create date: 10/24/2012
-- Description:	Registers V3D System with current server
-- =========================================================
CREATE PROCEDURE [v3d].[sp_register]
@ENABLE AS SMALLINT = -1,               -- Enable the system
@EXPTTL AS INT = NULL,                  -- Expiration time to live
@EXPUNITS AS NVARCHAR(32) = NULL,       -- Expiration units
@EXPDATE AS DATETIMEOFFSET(7) = NULL    -- V3D System expiration date
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  
	BEGIN TRY
	

    DECLARE @OLDEXPDATE AS DATETIMEOFFSET(7)
    DECLARE	@SYSTEMID NVARCHAR(256)
	
    SET @OLDEXPDATE = NULL


    EXEC	[v3d].[sp_getsystemid]
		    @systemid = @SYSTEMID OUTPUT

    -- Get any previous value set
    IF @ENABLE = 1 OR @ENABLE = 0
      SELECT  @OLDEXPDATE = [expdate] FROM [v3d].[Install]
    ELSE
      SELECT  @OLDEXPDATE = [expdate], @ENABLE = [enabled] FROM [v3d].[Install]

    -- Delete old entry
    DELETE [v3d].[Install]

    -- =============================================================================
    -- If a new expiration date is given then we will override the previous value
    -- =============================================================================
    IF @EXPDATE IS NOT NULL 
    BEGIN
      INSERT INTO [v3d].[Install] ([sysid], [expdate], [enabled] ) VALUES (@SYSTEMID, @EXPDATE, @ENABLE)
    END

    -- ==================================================================================
    -- If an Expiration TTL and Units are given then we will override the previsous value
    -- ==================================================================================
    ELSE IF @EXPTTL IS NOT NULL AND @EXPUNITS IS NOT NULL
    BEGIN

      DECLARE @UNITS AS NVARCHAR(64)
      DECLARE @DATE AS DATETIMEOFFSET(7)

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
     
      INSERT INTO [v3d].[Install] ([sysid], [expdate], [enabled] ) VALUES (@SYSTEMID, @EXPDATE, @ENABLE)

    END

    -- ==================================================================
    -- If there was a previous install then reset the expiration date
    -- ==================================================================
    ELSE IF @OLDEXPDATE IS NOT NULL
    BEGIN
      
     INSERT INTO [v3d].[Install] ([sysid], [expdate], [enabled] ) VALUES (@SYSTEMID, @OLDEXPDATE, @ENABLE)

    END

    -- ===================================================================================================
    -- If no previous install and no expiration date of ttl was given then use the default of 10 days
    -- ==================================================================================================
    ELSE
    BEGIN

     INSERT INTO [v3d].[Install] ([sysid], [enabled] ) VALUES (@SYSTEMID, @ENABLE)

    END


    RETURN 1

	END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_register'
	  
	END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_register] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_registerServer]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================
-- Author:		  Mark Kontak
-- Create date: 11/08/2012
-- Description:	Registers this server with the V3D Licensing system
-- =================================================================
CREATE PROCEDURE [v3d].[sp_registerServer]
@IP AS NVARCHAR(128) = ''           -- IP Address of server (Cannot get it internally)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
  SET @RC = 0

  BEGIN TRY

    DECLARE @SYSID AS NVARCHAR(256)
    DECLARE @SERVERNAME AS NVARCHAR(64)
    DECLARE @INS AS DATETIMEOFFSET(7)
    DECLARE @LS AS NVARCHAR(128)


    SET @SERVERNAME = @@SERVERNAME

    SELECT @SYSID = [sysid], @INS = [date] FROM [v3d].[Install]


    IF @@ROWCOUNT = 0 
        EXEC [master].[dbo].[sp_throwException] 900981, 'V3D Enterprise System'

    -- Unregister first
    EXEC [v3d].[sp_unregisterSystem]
      @SYSID = @SYSID

    -- ========================================================================================
    -- Clear the license server entry in the Install table if we unregistered successfully
    -- ========================================================================================
    IF @RC = 1
      UPDATE [v3d].[Install] SET [ls] = '' 

    -- Register System
    EXEC @RC = [v3d].[sp_registerSystem]
      @SYSID =   @SYSID,
      @NAME = @SERVERNAME,
      @IP = @IP,
      @INS = @INS,
      @LS = @LS OUTPUT

    -- ========================================================================================
    -- Update the license server entry in the Install table if we registered successfully
    -- ========================================================================================
    IF @RC = 1
      UPDATE [v3d].[Install] SET [ls] = @LS 

  
    RETURN @RC
  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_registerServer'
	  
	END CATCH
    
 END




GO
GRANT EXECUTE ON [v3d].[sp_registerServer] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_reindexTable]    Script Date: 3/17/2014 12:20:38 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_resetNamedEvent]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_resetNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_resetNamedEvent]
GO
GRANT EXECUTE ON [v3d].[sp_resetNamedEvent] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_resetNamedEvent] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_setMaskValue]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/20/2010
-- Description:	Sets the mask bits specified.
-- =============================================
CREATE PROCEDURE [v3d].[sp_setMaskValue]
@VALUE  BIGINT OUTPUT,        -- Value to be set
@MASK   NVARCHAR(32),         -- Mask name
@NAME1  NVARCHAR(32) = '',    -- 1st state bit
@NAME2  NVARCHAR(32) = '',    -- 2nd state bit
@NAME3  NVARCHAR(32) = '',    -- 3rd state bit
@NAME4  NVARCHAR(32) = ''     -- 4th state bit
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  --- Sets the specified bits in the value
  SELECT @VALUE = @VALUE | [bit] FROM [v3d].[Masks] 
         WHERE [mask] = @MASK AND 
               ( [name] = @NAME1 OR [name] = @NAME2 OR [name] = @NAME3 OR [name] = @NAME4 )
               
END





GO
GRANT EXECUTE ON [v3d].[sp_setMaskValue] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_setMaskValue] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_setNamedEvent]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_setNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[sp_setNamedEvent]
GO
GRANT EXECUTE ON [v3d].[sp_setNamedEvent] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_setNamedEvent] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_unlinkLicenseServer]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 11/7/2012
-- Description:	Links up a License server
-- =============================================
CREATE PROCEDURE [v3d].[sp_unlinkLicenseServer]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
  DECLARE @SERVERNAME AS SYSNAME

  -- ============================================
  -- Get the server name from the list only id 1
  -- ===========================================
  SELECT @SERVERNAME = [name] FROM [master].[sys].[servers] WHERE [server_id] = 1

  SET @RC = @@ROWCOUNT

  -- =======================================================
  -- Only need to drop the server if a row was returned
  -- =======================================================
  IF @RC = 1
  BEGIN

    EXEC sp_dropserver
      @server = @SERVERNAME,
      @droplogins = 'droplogins'
 
  END

  
  RETURN @RC

 
END




GO
GRANT EXECUTE ON [v3d].[sp_unlinkLicenseServer] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateApplication]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		      Mark Kontak
-- Create date:     08/26/2009
-- Description:	    Updates the application
-- Authorization:   Service only
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_updateApplication]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML                    -- Task to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
  DECLARE @JID_UID AS UNIQUEIDENTIFIER
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'Service'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @AID AS NVARCHAR(32)
    DECLARE @NAME AS NVARCHAR(32)
    DECLARE @MASK AS INT
    DECLARE @DEFAULT AS BIT
    DECLARE @DESC AS NVARCHAR(300)
      
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @AID          = [id],
            @NAME         = [name],
            @MASK         = [mask],
            @DESC         = [desc],
            @DEFAULT      = [default]
            FROM OPENXML(@IDOC, N'/application', 3) 
            WITH 
            (
              [id]            nvarchar(32),
              [name]          nvarchar(32),
              [mask]          int,
              [desc]          nvarchar(300),
              [default]       bit
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
    
    -- =============================================
    -- Makes sure the template id was specified
    -- (900990) Template Id: Missing or Invcalid
    -- =============================================
    IF @AID IS NULL 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Application Id'
    
   
      
    -- ================================================================================
    -- Sets the application 
    -- ================================================================================
    UPDATE [v3d].[Applications] SET [name]       = ISNULL(@NAME,[name]), 
                                    [desc]       = ISNULL(@DESC,[desc]),
                                    [mask]       = ISNULL(@MASK,[mask]),
                                    [default]    = ISNULL(@DEFAULT, [default])
                        WHERE [id] = @AID  

      

    SET @RC = @@ROWCOUNT


    -- ======================================================================================
    -- If this application is set to the default then unset the default flag for any other
    -- ======================================================================================
    IF @DEFAULT = 1
      UPDATE [v3d].[Applications] SET [default] = 0 WHERE [id] <> @AID
     
    RETURN @RC
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateApplication'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updateApplication] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateApplication] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateDicomHost]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		      Mark Kontak
-- Create date:     08/26/2009
-- Description:	    Updates the dicom host
-- Authorization:   Service only
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_updateDicomHost]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML                    -- Task to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
  DECLARE @JID_UID AS UNIQUEIDENTIFIER
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'Service'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @DESC AS NVARCHAR(512)
    DECLARE @NAME AS NVARCHAR(64)
    DECLARE @SERVER AS NVARCHAR(64)
    DECLARE @AETITLE AS NVARCHAR(64)
    DECLARE @PORT AS INT
    DECLARE @SERVICES AS INT
    DECLARE @ATTRIBUTES AS XML

      
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @NAME         = [name],
            @SERVER       = [server],
            @AETITLE      = [aetitle],
            @PORT         = [port],
            @DESC         = [desc],
            @SERVICES     = [services],
            @ATTRIBUTES   = [attributes]
            FROM OPENXML(@IDOC, N'/dicomhost', 3) 
            WITH 
            (
              [server]        nvarchar(64),
              [name]          nvarchar(64),
              [aetitle]       nvarchar(64),
              [port]          int,
              [services]      int,
              [desc]          nvarchar(512),
              [attributes]    xml './attributes'
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
    
    -- =============================================
    -- Makes sure the template id was specified
    -- (900990) Template Id: Missing or Invcalid
    -- =============================================
    IF @NAME IS NULL OR LEN(@NAME) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Dicom Host Name'
    
    -- ===================================================================
    -- If the attributes field was specified then extract the inner xml
    -- ====================================================================
    IF @ATTRIBUTES IS NOT NULL
      SELECT @ATTRIBUTES = @ATTRIBUTES.query('./attributes/*')

       
    -- ================================================================================
    -- Sets the dicom host 
    -- ================================================================================
    UPDATE [v3d].[DicomHosts] SET [server]     = ISNULL(@SERVER,[server]), 
                                  [aetitle]    = ISNULL(@AETITLE,[aetitle]),
                                  [desc]       = ISNULL(@DESC,[desc]),
                                  [services]   = ISNULL(@SERVICES,[services]),
                                  [port]       = ISNULL(@PORT, [port]),
                                  [attributes] = @ATTRIBUTES
                        WHERE [name] = @NAME  

      

      
    RETURN @@ROWCOUNT
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateDicomHost'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updateDicomHost] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateDicomHost] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateSetting]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Updates a system setting
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateSetting]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML                    -- setting to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
  DECLARE @JID_UID AS UNIQUEIDENTIFIER
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @NAME AS NVARCHAR(32)
    DECLARE @SUBSYSTEM AS NVARCHAR(32)
    DECLARE @VALUE AS NVARCHAR(512)
    DECLARE @DESC AS NVARCHAR(256)


     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @SUBSYSTEM        = [subsystem],
            @NAME             = [name],
            @VALUE            = [value],
            @DESC             = [desc]
            FROM OPENXML(@IDOC, N'/setting', 3) 
            WITH 
            (
              [subsystem]      nvarchar(32),
              [name]           nvarchar(32),
              [value]          nvarchar(512),
              [desc]           nvarchar(256)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
   
    -- Updates the setting
    UPDATE [v3d].[SystemSettings] SET   [value]     = ISNULL(@VALUE,[value]), 
                                        [desc]      = ISNULL(@DESC,[desc]),
                                        [subsystem] = ISNULL(@SUBSYSTEM, [subsystem])
                            WHERE [name] = @NAME

    RETURN  @@ROWCOUNT
        
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateSetting'

  END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_updateSetting] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateSetting] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateSystemSetting]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Updates a system setting
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateSystemSetting]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML                    -- setting to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
  DECLARE @JID_UID AS UNIQUEIDENTIFIER
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @NAME AS NVARCHAR(32)
    DECLARE @SUBSYSTEM AS NVARCHAR(32)
    DECLARE @VALUE AS NVARCHAR(512)
    DECLARE @DESC AS NVARCHAR(256)


     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @SUBSYSTEM        = [subsystem],
            @NAME             = [name],
            @VALUE            = [value],
            @DESC             = [desc]
            FROM OPENXML(@IDOC, N'/systemsetting', 3) 
            WITH 
            (
              [subsystem]      nvarchar(32),
              [name]           nvarchar(32),
              [value]          nvarchar(512),
              [desc]           nvarchar(256)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
   
    -- Updates the setting
    UPDATE [v3d].[SystemSettings] SET   [value]     = ISNULL(@VALUE,[value]), 
                                        [desc]      = ISNULL(@DESC,[desc]),
                                        [subsystem] = ISNULL(@SUBSYSTEM, [subsystem])
                            WHERE [name] = @NAME

    RETURN  @@ROWCOUNT
        
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateSystemSetting'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updateSystemSetting] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateSystemSetting] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateTemplate]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description:	Updates the template
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_updateTemplate]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML                    -- Task to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
  DECLARE @JID_UID AS UNIQUEIDENTIFIER
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @TID AS NVARCHAR(32)
    DECLARE @APP AS NVARCHAR(32)
    DECLARE @MOD AS NVARCHAR(32)
    DECLARE @DESC AS NVARCHAR(300)
    DECLARE @DEFAULT AS BIT
    DECLARE @TASKS AS XML
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @TID          = [id],
            @APP          = [app],
            @MOD          = [mod],
            @DESC         = [desc],
            @DEFAULT      = [default],
            @TASKS        = [tasks]
            FROM OPENXML(@IDOC, N'/template', 3) 
            WITH 
            (
              [id]            nvarchar(32),
              [app]           nvarchar(32),
              [mod]           nvarchar(32),
              [desc]          nvarchar(300),
              [default]       bit,
              [tasks]         xml   './tasks'
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
    
    -- =============================================
    -- Makes sure the template id was specified
    -- (900990) Template Id: Missing or Invcalid
    -- =============================================
    IF @TID IS NULL 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Template Id'
    
 
  
    IF @TASKS IS NOT NULL
      SET @TASKS = @TASKS.query('/tasks/*')
   
    -- ======================================================================================================================
    -- Create a temporary table to hold the updated record. This is does because the APP and MOD may not be included in the 
    -- xml record so I will store the information that is already in the database. This is used after the update to clear 
    -- the default flag of any matching app/mod template if the default flag is to be set to TRUE.
    -- =====================================================================================================================
    DECLARE @Table AS TABLE ( [default] bit, [app] nvarchar(10), [mod] nvarchar(10) )
       
    IF @TASKS IS NULL
    BEGIN
      
      -- ================================================================================
      -- Sets the template 
      -- ================================================================================
      UPDATE [v3d].[Templates] SET [app]       = ISNULL(@APP,[app]), 
                                   [desc]      = ISNULL(@DESC,[desc]),
                                   [mod]       = ISNULL(@MOD,[mod]),
                                   [default]   = ISNULL(@DEFAULT, [default])
                         OUTPUT INSERTED.[default], INSERTED.app, INSERTED.mod INTO @Table
                         WHERE [id] = @TID  

    END
    ELSE
    BEGIN

      -- ================================================================================
      -- Sets the template 
      -- ================================================================================
      UPDATE [v3d].[Templates] SET [app]       = ISNULL(@APP,[app]), 
                                   [desc]      = ISNULL(@DESC,[desc]),
                                   [mod]       = ISNULL(@MOD,[mod]),
                                   [default]   = ISNULL(@DEFAULT, [default]),
                                   [tasks]     = @TASKS
                         OUTPUT INSERTED.[default],  INSERTED.app, INSERTED.mod INTO @Table
                         WHERE [id] = @TID  
    

    END

    SET @RC = @@ROWCOUNT



    -- ===========================================================================================
    -- If we are setting the item to be the default we need to remove the deafult flag for the 
    -- other templates for this application.
    -- ===========================================================================================
    IF (@DEFAULT IS NOT NULL AND @DEFAULT = 1) 
    BEGIN
      -- Get the APP and MOD from the updated record. 
      SELECT @APP = [app], @MOD = [mod] FROM @Table

      -- Clear the deafult flag for the prevoious default
      UPDATE [v3d].[Templates] SET [default] = 0 WHERE [id] <> @TID AND [app] = @APP AND [mod] = @MOD AND [default] = 1
    END


    SET @RC = @@ROWCOUNT
 
    RETURN @RC
    
  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateTemplate'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updateTemplate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateTemplate] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Builds a comparison string for a string value taking into account a % at the beginning
--              and/or end to make use of a like. Used when dynamically building queries. Assumes only 
--              a single value.
-- =======================================================================================================
CREATE FUNCTION [v3d].[fn_buildComparison] 
(
@VALUE AS NVARCHAR(512),
@COL AS NVARCHAR(126)
)
RETURNS NVARCHAR(1000)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(1000)
	
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
      SET @COMPARE = @COL + ' LIKE ''' + @VALUE + ''' '
    END
    ELSE
    BEGIN
      -- EQUAL statement
      SET @COMPARE = @COL + ' = ''' + @VALUE + ''' '   
    END
    
  END   -- END ... If the value is not NULL 


	-- Return the result of the function
	RETURN @COMPARE

END





GO
GRANT EXECUTE ON [v3d].[fn_buildComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildDateComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_buildFloatComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Generates a comparison statement for the supplied integers as if a range 
--              is to be used.
-- ====================================================================================
CREATE FUNCTION [v3d].[fn_buildFloatComparison]
(
@FROM AS FLOAT,
@FROM_PARAM_NAME AS NVARCHAR(128),
@TO AS FLOAT,
@TO_PARAM_NAME AS NVARCHAR(128),
@COL AS NVARCHAR(128)
)
RETURNS NVARCHAR(255)
AS
BEGIN

	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(255)
	
	SET @COMPARE = ''

  
  IF @TO IS NULL AND @FROM IS NOT NULL
  BEGIN
  
    SET @COMPARE = @COL + ' <= ' + @TO_PARAM_NAME + ' AND '                   
    
  END
  ELSE IF @TO IS NOT NULL AND @FROM IS NULL
  BEGIN
  
    SET @COMPARE = @COL + ' >= ' + @FROM_PARAM_NAME + ' AND '
    
  END
  ELSE IF @TO IS NOT NULL AND @FROM IS NOT NULL
  BEGIN 
  
    -- ==========================================================
    -- If the FROM and TO are equal make the comparison an equal
    -- ===========================================================
    IF @TO = @FROM
    BEGIN
      SET @COMPARE = @COL + ' = ' + @FROM_PARAM_NAME + ' AND '
    END
    ELSE
    BEGIN
      SET @COMPARE = @COL + ' >= ' + @FROM_PARAM_NAME + ' AND ' + @COL + ' <= ' + @TO_PARAM_NAME + ' AND '
    END
    
  END
 
 	-- Return the result of the function
	RETURN @COMPARE

END





GO
GRANT EXECUTE ON [v3d].[fn_buildFloatComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildFloatComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildIntegerComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 05/07/2010
-- Description:	Generates a comparison statement for the supplied integers as if a range 
--              is to be used.
-- ====================================================================================
CREATE FUNCTION [v3d].[fn_buildIntegerComparison]
(
@FROM AS INT,
@FROM_PARAM_NAME AS NVARCHAR(128),
@TO AS INT,
@TO_PARAM_NAME AS NVARCHAR(128),
@COL AS NVARCHAR(128)
)
RETURNS NVARCHAR(255)
AS
BEGIN

	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(255)
	
	SET @COMPARE = ''

  
  IF @TO IS NULL AND @FROM IS NOT NULL
  BEGIN
  
    SET @COMPARE = @COL + ' <= ' + @TO_PARAM_NAME + ' AND '                   
    
  END
  ELSE IF @TO IS NOT NULL AND @FROM IS NULL
  BEGIN
  
    SET @COMPARE = @COL + ' >= ' + @FROM_PARAM_NAME + ' AND '
    
  END
  ELSE IF @TO IS NOT NULL AND @FROM IS NOT NULL
  BEGIN 
  
    -- ==========================================================
    -- If the FROM and TO are equal make the comparison an equal
    -- ===========================================================
    IF @TO = @FROM
    BEGIN
      SET @COMPARE = @COL + ' = ' + @FROM_PARAM_NAME + ' AND '
    END
    ELSE
    BEGIN
      SET @COMPARE = @COL + ' >= ' + @FROM_PARAM_NAME + ' AND ' + @COL + ' <= ' + @TO_PARAM_NAME + ' AND '
    END
    
  END
 
 	-- Return the result of the function
	RETURN @COMPARE

END





GO
GRANT EXECUTE ON [v3d].[fn_buildIntegerComparison] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildIntegerComparison] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildLikeStringComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_buildStringComparison]    Script Date: 3/17/2014 12:20:38 PM ******/
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
@VALUE AS NVARCHAR(MAX),
@COL AS NVARCHAR(126)
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @COMPARE AS NVARCHAR(MAX)
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
/****** Object:  UserDefinedFunction [v3d].[fn_createUid]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 09/24/2010
-- Description:	Creates a new dicom uid 
-- =============================================
CREATE FUNCTION [v3d].[fn_createUid]()
RETURNS NVARCHAR(128)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @UID AS NVARCHAR(128)
	DECLARE @SITE AS INT
	DECLARE @CURRENT AS DATETIMEOFFSET(7)
  DECLARE @LOCK AS INT
	

	-- There is only one site
	SET @SITE = ( SELECT [id] FROM [System].[v3d].[Site] )

	
	-- Get the current date
	SET @CURRENT = SYSDATETIMEOFFSET()
 

	
	SET @UID = '1.3.114174.' + 
	            CONVERT(NVARCHAR(10),ISNULL(@SITE,0)) + '.' + 
	            CONVERT(NVARCHAR(10),@@SPID) + '.' + 
	            CONVERT(NVARCHAR(10),DATEPART(year,@CURRENT)) + '.' + 
	            CONVERT(NVARCHAR(10),DATEPART(month,@CURRENT)) + '.' + 
	            CONVERT(NVARCHAR(10),DATEPART(day,@CURRENT)) + '.' + 
	            CONVERT(NVARCHAR(10),DATEPART(hour,@CURRENT)) + '.' +  
	            CONVERT(NVARCHAR(10),DATEPART(minute,@CURRENT)) +  '.' + 
	            CONVERT(NVARCHAR(10),DATEPART(second,@CURRENT)) + '.' +  
	            CONVERT(NVARCHAR(10),DATEPART(millisecond,@CURRENT)) + '.' +
	            CONVERT(NVARCHAR(10),DATEPART(nanosecond,@CURRENT)) 


	-- Return the result of the function
	RETURN @UID

END





GO
GRANT EXECUTE ON [v3d].[fn_createUid] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_createUid] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getApplicationMask]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 09/05/2013
-- Description:	Get product mask
-- =============================================
CREATE FUNCTION [v3d].[fn_getApplicationMask] 
(
@CODE AS NVARCHAR(10)
)
RETURNS INT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @MASK AS INT

  SET @MASK = 0

	-- Add the T-SQL statements to compute the return value here
	SELECT @MASK = [mask] FROM [v3d].[Applications] WHERE [id] = @CODE

	-- Return the result of the function
	RETURN @MASK

END




GO
GRANT EXECUTE ON [v3d].[fn_getApplicationMask] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getApplicationMask] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDatabase]    Script Date: 3/17/2014 12:20:38 PM ******/
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

  SET @NAME = 'System'
  
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
GRANT EXECUTE ON [v3d].[fn_getDatabase] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDefaultApplication]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 09/13/2013
-- Description:	Returns the default application
-- =============================================
CREATE FUNCTION [v3d].[fn_getDefaultApplication]
(
)
RETURNS NVARCHAR(32)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @APP AS NVARCHAR(32)

  SET @APP = ''

	SELECT @APP = [id] FROM [System].[v3d].[Applications] WHERE [default] = 1

	-- Return the result of the function
	RETURN @APP

END




GO
GRANT EXECUTE ON [v3d].[fn_getDefaultApplication] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getDefaultApplication] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDriveInfo]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE FUNCTION [v3d].[fn_getDriveInfo]()
RETURNS  TABLE (
	[name] [nvarchar](32) NULL,
	[cap] [bigint] NULL,
	[used] [bigint] NULL,
	[avail] [bigint] NULL,
	[msg] [nvarchar](512) NULL
) WITH EXECUTE AS CALLER
AS 
EXTERNAL NAME [Viatronix.Sql.System].[StoredProcedures].[fn_getDriveInfo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getMaskBit]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Sets the specified bit in the mask
-- ==================================================
CREATE FUNCTION [v3d].[fn_getMaskBit]
(
@NAME AS NVARCHAR(32),        -- Mask type
@BITNAME AS NVARCHAR(16)         -- Bit Name
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @BIT AS BIGINT
	
	SET @BIT = 0
	
	SELECT @BIT = [bit] FROM [System].[v3d].[Masks] NOLOCK WHERE [name] = LOWER(@NAME) AND [bn] = LOWER(@BITNAME)

	-- Return the result of the function
	RETURN @BIT

END





GO
GRANT EXECUTE ON [v3d].[fn_getMaskBit] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getMaskBit] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getMaskBitDesc]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Gets the name of the specified masks bit
--
-- Parameters:
--    @MASK       Name of the mask
--    @BIT        Bit in the mask 
--  
-- Return:      Returns the NAME of the mask/bit found in the 
--              Masks table.
-- =============================================================
CREATE FUNCTION [v3d].[fn_getMaskBitDesc]
(
@NAME AS NVARCHAR(32),        -- Mask name
@BIT AS BIGINT                -- Bit 
)
RETURNS NVARCHAR(512)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @DESC AS NVARCHAR(512)
	
	SET @DESC = ''
	
	SELECT @DESC = [desc] FROM [System].[v3d].[Masks] NOLOCK WHERE [name] = @NAME AND [bit] = @BIT

	-- Return the result of the function
	RETURN @DESC

END





GO
GRANT EXECUTE ON [v3d].[fn_getMaskBitDesc] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getMaskBitDesc] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getMaskBitName]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Gets the name of the specified masks bit
--
-- Parameters:
--    @MASK       Name of the mask
--    @BIT        Bit in the mask 
--  
-- Return:      Returns the NAME of the mask/bit found in the 
--              Masks table.
-- =============================================================
CREATE FUNCTION [v3d].[fn_getMaskBitName]
(
@NAME AS NVARCHAR(32),        -- Mask name
@BIT AS BIGINT                -- Bit 
)
RETURNS NVARCHAR(32)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @BITNAME AS NVARCHAR(32)
	
	SET @BITNAME = ''
	
	SELECT @BITNAME = [bn] FROM [System].[v3d].[Masks] NOLOCK WHERE [name] = LOWER(@NAME) AND [bit] = @BIT

	-- Return the result of the function
	RETURN @BITNAME

END





GO
GRANT EXECUTE ON [v3d].[fn_getMaskBitName] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getMaskBitName] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getMaskDesc]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Gets the description for the mask/bit
--
-- Parameters:
--    @MASK       Name of the mask
--    @BITNAME    Bitname
--  
-- Return:      Returns the DESC of the mask/bit found in the 
--              Masks table.
-- =============================================================
CREATE FUNCTION [v3d].[fn_getMaskDesc]
(
@NAME AS NVARCHAR(32),        -- Mask type
@BITNAME AS NVARCHAR(16)         -- Bit Name

)
RETURNS NVARCHAR(512)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @DESC AS NVARCHAR(512)
	
	SET @DESC = ''
	
	SELECT @DESC = [desc] FROM [System].[v3d].[Masks] NOLOCK WHERE [name] = @NAME AND [bn] = @BITNAME

	-- Return the result of the function
	RETURN @DESC

END





GO
GRANT EXECUTE ON [v3d].[fn_getMaskDesc] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getMaskDesc] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getMaskValue]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================
-- Author:		  Mark Kontak
-- Create date: 10/20/2010
-- Description:	Gets the mask value using the mask and names specified
-- =====================================================================
CREATE FUNCTION [v3d].[fn_getMaskValue]
(
@NAME   NVARCHAR(32),
@BITNAME1  NVARCHAR(32),    -- 1st state bit
@BITNAME2  NVARCHAR(32),    -- 2nd state bit
@BITNAME3  NVARCHAR(32),    -- 3rd state bit
@BITNAME4  NVARCHAR(32)     -- 4th state bit
)
RETURNS BIGINT
AS
BEGIN

  DECLARE @RETURN AS BIGINT
  
  SET @RETURN = 0

  --- Sets the specified bits in the value
  SELECT @RETURN = @RETURN | [bit] FROM [v3d].[Masks] NOLOCK
         WHERE [name] = @NAME AND 
               ( [bn] = @BITNAME1 OR [bn] = @BITNAME2 OR [bn] = @BITNAME3 OR [bn] = @BITNAME4 )

  RETURN @RETURN
  

END





GO
GRANT EXECUTE ON [v3d].[fn_getMaskValue] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getMaskValue] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSetting]    Script Date: 3/17/2014 12:20:38 PM ******/
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
	SELECT @VALUE = [v3d].[fn_replaceSettings]([value]) FROM [v3d].[SystemSettings] NOLOCK WHERE [name] = @NAME

  
     
	-- Return the result of the function
	RETURN @VALUE

END





GO
GRANT EXECUTE ON [v3d].[fn_getSetting] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getSetting] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSiteId]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 12/17/2010
-- Description:	Returns the site id
-- =============================================
CREATE FUNCTION [v3d].[fn_getSiteId]
(
)
RETURNS INT
AS
BEGIN
	DECLARE @ID AS INT
	
	SET @ID = 0
	
	SELECT @ID = [id] FROM [v3d].[Site] NOLOCK

	-- Return the result of the function
	RETURN @ID

END





GO
GRANT EXECUTE ON [v3d].[fn_getSiteId] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getSiteId] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getVersion]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak 
-- Create date: 06/10/2010
-- Description:	Returns the table version
-- =============================================
CREATE FUNCTION [v3d].[fn_getVersion]
(
@OBJECT NVARCHAR(32)             -- Object Name
)
RETURNS NVARCHAR(16)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @VERSION AS NVARCHAR(16)
	
	SET @VERSION = ''

  SELECT @VERSION = [version] FROM [v3d].[Versions] NOLOCK WHERE [object] = @OBJECT
  
  
  RETURN @VERSION

END





GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_installedSystemId]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===========================================================
-- Author:		  Mark Kontak
-- Create date: 05/11/2010
-- Description:	Returns the system id from the installation
-- ==========================================================
CREATE FUNCTION [v3d].[fn_installedSystemId] ()
RETURNS NVARCHAR(255)
AS
BEGIN

  DECLARE @ID AS NVARCHAR(255)
  
  SELECT TOP(1) @ID = [systemId] FROM  [System].[v3d].Install 
  
  RETURN @ID

END





GO
GRANT EXECUTE ON [v3d].[fn_installedSystemId] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_installedSystemId] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isMaskBitSet]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ========================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Determines if the specified bit is set
-- =========================================================
CREATE FUNCTION [v3d].[fn_isMaskBitSet]
(
@VALUE AS BIGINT,              -- Mask value
@NAME AS NVARCHAR(16),         -- mask name
@BITNAME AS NVARCHAR(16)          -- Bit name             
)
RETURNS  BIT
AS
BEGIN
	
	DECLARE @BIT AS BIGINT
	DECLARE @RETURN AS BIT
	
	SET @BIT = 0
	SET @RETURN = 0
	
	SELECT @BIT = [bit] FROM [System].[v3d].[Masks] WHERE [name] = LOWER(@NAME) AND [bn] = LOWER(@BITNAME)

  -- Only check if the mask / name was found
  IF @@ROWCOUNT = 1 AND (( @VALUE & @BIT ) = @BIT )
    SET @RETURN = 1
  
  RETURN @RETURN


END





GO
GRANT EXECUTE ON [v3d].[fn_isMaskBitSet] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isMaskBitSet] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_pathCombine]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 01/13/2012
-- Description:	Combines upto 3 paths
-- =============================================
CREATE FUNCTION [v3d].[fn_pathCombine] 
(
@PATH1 AS NVARCHAR(512),
@PATH2 AS NVARCHAR(512),
@PATH3 AS NVARCHAR(512)
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @RESULT AS NVARCHAR(MAX)
  DECLARE @END AS NVARCHAR(1)
  DECLARE @BEGIN AS NVARCHAR(1)
  DECLARE @PATH_SEPERATOR  AS NVARCHAR(2)
 
  SET @PATH1 = RTRIM(@PATH1)
  SET @PATH2 = LTRIM(@PATH2)
  SET @PATH3 = LTRIM(ISNULL(@PATH3,''))
  
  SET @END = RIGHT(@PATH1,1) 
  SET @BEGIN = LEFT(@PATH2,1)
 
  
  
  SET @PATH_SEPERATOR = [System].[v3d].[fn_getSetting]('PathSeperator','\')
  
  SET @RESULT = @PATH1
  
  IF ( @END = '\' OR @END = '/' )
    SET @RESULT = SUBSTRING(@PATH1,1, LEN(@PATH1) - 1)
    
  SET @RESULT = @RESULT + @PATH_SEPERATOR
 
  IF ( @BEGIN = '\' OR @BEGIN = '/' )
    SET @RESULT = @RESULT + SUBSTRING(@PATH2, 2, LEN(@PATH2) -1 )
  ELSE
    SET @RESULT = @RESULT + @PATH2

  -- ==============================
  -- Add the 3rd path is specified
  -- ==============================
  IF LEN(@PATH3) > 0 
  BEGIN
    SET @BEGIN = LEFT(@PATH3,1)

    SET @RESULT = @RESULT + @PATH_SEPERATOR
 
    IF ( @BEGIN = '\' OR @BEGIN = '/' )
      SET @RESULT = @RESULT + SUBSTRING(@PATH3, 2, LEN(@PATH3) -1 )
    ELSE
      SET @RESULT = @RESULT + @PATH3

  END
    
  RETURN @RESULT    

END





GO
GRANT EXECUTE ON [v3d].[fn_pathCombine] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_pathCombine] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_replaceSettings]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Maek Kontak 
-- Create date: 11/02/2009
-- Description:	Gets the system setting value
-- =============================================
CREATE FUNCTION [v3d].[fn_replaceSettings] 
(
@VALUE AS NVARCHAR(MAX)
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	
  DECLARE @RETURN         AS NVARCHAR(MAX)
  DECLARE @TEMP             AS NVARCHAR(MAX)
  DECLARE @MACRO            AS NVARCHAR(MAX)
  DECLARE @START            AS INT
  DECLARE @END              AS INT
  DECLARe @SETTING          AS NVARCHAR(64)
  DECLARE @START_DELIMITER  AS NVARCHAR(10)
   
  SELECT  @RETURN = @VALUE, 
          @START_DELIMITER = '%' + '$' + '(' + '%%';

  SELECT @START = PATINDEX(@START_DELIMITER, @VALUE )

  -- While we have 
  WHILE @START > 0
  BEGIN


    SET @END = CHARINDEX(')', @RETURN, @START)

    -- =============================================================================
    -- If we found the ending delimiter ')' we can extract the the Setting Macro
    -- =============================================================================
    IF @END > 0 
    BEGIN

      -- Extract the macro and the setting name from the string
      SELECT  @MACRO = SUBSTRING(@RETURN, @START, @END - @START + 1),
              @SETTING = SUBSTRING(@RETURN, @START + 2, @END - @START -2)

 
      -- Get the settings value
      SELECT @TEMP = [v3d].[fn_getSetting](@SETTING, '')

      -- Replace the MACOR with the setting value
      SELECT @RETURN = REPLACE(@RETURN, @MACRO, @TEMP )

      -- Look for the next MACRO
      SELECT @START =  PATINDEX(@START_DELIMITER, @RETURN )
    
  
    END -- END ... If there is an END
    ELSE
      SET @START = 0


 
  END -- END ... While there are MACROS
    
	-- Return the result of the function
	RETURN @RETURN

END


GO
GRANT EXECUTE ON [v3d].[fn_replaceSettings] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_replaceSettings] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_setMaskBit]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Sets the specified bit in the mask
-- ==================================================
CREATE FUNCTION [v3d].[fn_setMaskBit]
(
@VALUE AS BIGINT,              -- Mask
@NAME AS NVARCHAR(32),        -- Mask name
@BITNAME AS NVARCHAR(16)          -- Bit Name
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @BIT AS BIGINT
	
	SET @BIT = 0
	
	SELECT @BIT = [bit] FROM [System].[v3d].[Masks] WHERE [name] = LOWER(@NAME) AND [bn] = LOWER(@BITNAME)

	-- Return the result of the function
	RETURN ( @VALUE | @BIT )

END





GO
GRANT EXECUTE ON [v3d].[fn_setMaskBit] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_setMaskBit] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_setMaskValue]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================
-- Author:		  Mark Kontak
-- Create date: 10/20/2010
-- Description:	Sets the mask value using the specified bits
-- ============================================================
CREATE FUNCTION [v3d].[fn_setMaskValue]
(
@VALUE  BIGINT,             -- Value to start
@NAME   NVARCHAR(32),       -- Mask name
@BITNAME1  NVARCHAR(32),       -- 1st Bit name      
@BITNAME2  NVARCHAR(32),       -- 2nd Bit name      
@BITNAME3  NVARCHAR(32),       -- 3rd Bit name      
@BITNAME4  NVARCHAR(32)        -- 4th Bit name      
)
RETURNS BIGINT
AS
BEGIN

  DECLARE @RETURN AS BIGINT
  
  SET @RETURN = @VALUE

  --- Sets the specified bits in the value
  SELECT @RETURN = @RETURN | [bit] FROM [v3d].[Masks] 
         WHERE [name] = @NAME AND 
               ( [bn] = @BITNAME1 OR [bn] = @BITNAME2 OR [bn] = @BITNAME3 OR [bn] = @BITNAME4 )

  RETURN @RETURN
  
END





GO
GRANT EXECUTE ON [v3d].[fn_setMaskValue] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_setMaskValue] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_unsetMaskBit]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Unsets the specified bit in the mask
-- ==================================================
CREATE FUNCTION [v3d].[fn_unsetMaskBit]
(
@VALUE AS BIGINT,              -- Mask
@NAME AS NVARCHAR(32),        -- Mask name
@BITNAME AS NVARCHAR(16)          -- Bit Name
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @BIT AS BIGINT
	DECLARE @RETURN AS BIGINT
		
	SET @BIT = 0
	
	SELECT @BIT = [bit] FROM [System].[v3d].[Masks] WHERE [name] = LOWER(@NAME) AND [bn] = LOWER(@BITNAME)

  SEt @RETURN =  ( ( @VALUE | @BIT ) ^ @BIT )
  
	-- Return the result of the function
	RETURN @RETURN


END





GO
GRANT EXECUTE ON [v3d].[fn_unsetMaskBit] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_unsetMaskBit] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[AgentJobs]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[AgentJobs](
	[name] [nvarchar](64) NOT NULL,
	[proc] [nvarchar](1024) NOT NULL,
	[type] [nvarchar](16) NOT NULL,
	[ctx] [bit] NOT NULL,
	[freq] [nvarchar](16) NOT NULL,
	[fn] [int] NOT NULL,
	[days] [int] NOT NULL,
	[time] [time](7) NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[enabled] [bit] NOT NULL,
	[params] [nvarchar](1024) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[max] [int] NOT NULL,
	[user] [nvarchar](64) NOT NULL,
	[pwd] [nvarchar](128) NOT NULL,
	[adhoc] [text] NOT NULL,
 CONSTRAINT [PK_AgentJobs_1] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[AgentJobs] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[AgentJobs] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[AgentJobs] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[AgentJobs] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[AgentJobs] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[AgentJobs] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[AgentJobs] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[AgentJobs] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Applications]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Applications](
	[id] [nvarchar](16) NOT NULL,
	[name] [nvarchar](32) NOT NULL,
	[pri] [int] NOT NULL,
	[mask] [int] NOT NULL,
	[cad] [tinyint] NOT NULL,
	[desc] [text] NOT NULL,
	[inst] [bit] NOT NULL,
	[default] [bit] NOT NULL,
	[url] [nvarchar](max) NOT NULL,
	[launch] [xml] NOT NULL,
 CONSTRAINT [PK_Applications] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Applications] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Applications] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Applications] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Applications] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Applications] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[DicomHosts]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[DicomHosts](
	[name] [nvarchar](32) NOT NULL,
	[server] [nvarchar](64) NOT NULL,
	[aetitle] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[port] [int] NOT NULL,
	[services] [int] NOT NULL,
	[attributes] [xml] NULL,
 CONSTRAINT [PK_DicomHosts_1] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[DicomHosts] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[DicomHosts] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[DicomHosts] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[DicomHosts] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[DicomHosts] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Install]    Script Date: 3/17/2014 12:20:38 PM ******/
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
GRANT SELECT ON [v3d].[Install] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Masks]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Masks](
	[name] [nvarchar](32) NOT NULL,
	[bn] [nvarchar](16) NOT NULL,
	[bit] [bigint] NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_Masks] PRIMARY KEY NONCLUSTERED 
(
	[name] ASC,
	[bn] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Masks] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Masks] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Masks] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Masks] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Masks] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Modalities]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Modalities](
	[code] [nvarchar](10) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
 CONSTRAINT [PK_Modalities] PRIMARY KEY CLUSTERED 
(
	[code] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Modalities] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Modalities] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Modalities] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Modalities] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Modalities] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Site]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Site](
	[id] [int] NOT NULL,
	[name] [nvarchar](255) NOT NULL,
	[addr1] [nvarchar](255) NOT NULL,
	[addr2] [nvarchar](255) NOT NULL,
	[city] [nvarchar](64) NOT NULL,
	[state] [nvarchar](64) NOT NULL,
	[zip] [nvarchar](16) NOT NULL,
	[cntry] [nvarchar](64) NOT NULL,
	[int] [bit] NOT NULL,
 CONSTRAINT [SITE_PK] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Site] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Site] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Site] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Site] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Site] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Site] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Stores]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Stores](
	[name] [nvarchar](16) NOT NULL,
	[local] [nvarchar](64) NOT NULL,
	[remote] [nvarchar](64) NOT NULL,
	[server] [nvarchar](32) NOT NULL,
	[priority] [int] NOT NULL,
	[enabled] [bit] NOT NULL,
 CONSTRAINT [PK_Stores] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Stores] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Stores] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Stores] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Stores] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Stores] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[StudyGroups]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[StudyGroups](
	[name] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](128) NOT NULL
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[StudyGroups] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[StudyGroups] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[StudyGroups] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[StudyGroups] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[StudyGroups] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[StudyGroups] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[StudyGroups] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[StudyGroups] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[SystemConfigs]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[SystemConfigs](
	[name] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](512) NOT NULL,
	[config] [xml] NULL,
 CONSTRAINT [PK_Configurations_1] PRIMARY KEY CLUSTERED 
(
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[SystemConfigs] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[SystemConfigs] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[SystemConfigs] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SystemConfigs] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[SystemConfigs] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[SystemConfigs] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[SystemConfigs] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SystemConfigs] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[SystemSettings]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[SystemSettings](
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
GRANT DELETE ON [v3d].[SystemSettings] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[SystemSettings] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[SystemSettings] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SystemSettings] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[SystemSettings] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Templates]    Script Date: 3/17/2014 12:20:38 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Templates](
	[id] [nvarchar](64) NOT NULL,
	[app] [nvarchar](10) NOT NULL,
	[mod] [nvarchar](10) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
	[cad] [nvarchar](128) NOT NULL,
	[default] [bit] NOT NULL,
	[tasks] [xml] NOT NULL,
 CONSTRAINT [PK_Templates] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Templates] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Templates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Templates] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Templates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Templates] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Versions]    Script Date: 3/17/2014 12:20:38 PM ******/
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
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_name]  DEFAULT (N'Job Name') FOR [name]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_proc]  DEFAULT ('') FOR [proc]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_trype]  DEFAULT (N'SQL') FOR [type]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_rqctxt]  DEFAULT ((0)) FOR [ctx]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_freq]  DEFAULT (N'hour') FOR [freq]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_Table_1_freq_num]  DEFAULT ((0)) FOR [fn]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_days]  DEFAULT ((0)) FOR [days]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_enabled]  DEFAULT ((1)) FOR [enabled]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_params]  DEFAULT ('') FOR [params]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_max]  DEFAULT ((0)) FOR [max]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_user]  DEFAULT ('') FOR [user]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_password]  DEFAULT ('') FOR [pwd]
GO
ALTER TABLE [v3d].[AgentJobs] ADD  CONSTRAINT [DF_AgentJobs_adhoc]  DEFAULT ('') FOR [adhoc]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_SUBTYPES_SUB_TYPE]  DEFAULT ('') FOR [id]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Products_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Applications_pri]  DEFAULT ((1)) FOR [pri]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Applications_mask]  DEFAULT ((0)) FOR [mask]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Applications_cad]  DEFAULT ((0)) FOR [cad]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_SUBTYPES_DESCRIPTION]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Products_inst]  DEFAULT ((1)) FOR [inst]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Products_default]  DEFAULT ((0)) FOR [default]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Applications_url]  DEFAULT ('') FOR [url]
GO
ALTER TABLE [v3d].[Applications] ADD  CONSTRAINT [DF_Applications_launching]  DEFAULT ('') FOR [launch]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_DicomHosts_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_Table_1_svr]  DEFAULT ('') FOR [server]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_DicomHosts_aetitle]  DEFAULT ('') FOR [aetitle]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_DicomHosts_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_DicomHosts_port]  DEFAULT ((0)) FOR [port]
GO
ALTER TABLE [v3d].[DicomHosts] ADD  CONSTRAINT [DF_DicomHosts_services]  DEFAULT ((0)) FOR [services]
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
ALTER TABLE [v3d].[Masks] ADD  CONSTRAINT [DF_Masks_mask]  DEFAULT ((0)) FOR [bit]
GO
ALTER TABLE [v3d].[Masks] ADD  CONSTRAINT [DF_Masks_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Modalities] ADD  CONSTRAINT [DF_Modalities_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_address]  DEFAULT ('') FOR [addr1]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_address2]  DEFAULT ('') FOR [addr2]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_city]  DEFAULT ('') FOR [city]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_state]  DEFAULT ('') FOR [state]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_zip]  DEFAULT ('') FOR [zip]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_Site_contry]  DEFAULT ('') FOR [cntry]
GO
ALTER TABLE [v3d].[Site] ADD  CONSTRAINT [DF_SITE_INTERNATIONAL]  DEFAULT ((0)) FOR [int]
GO
ALTER TABLE [v3d].[Stores] ADD  CONSTRAINT [DF_DATA_STORES_NAME]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Stores] ADD  CONSTRAINT [DF_DATA_STORES_ROOT]  DEFAULT ('') FOR [local]
GO
ALTER TABLE [v3d].[Stores] ADD  CONSTRAINT [DF_DATA_STORES_SIZE]  DEFAULT ((0)) FOR [remote]
GO
ALTER TABLE [v3d].[Stores] ADD  CONSTRAINT [DF_DATA_STORES_SERVER]  DEFAULT ('') FOR [server]
GO
ALTER TABLE [v3d].[Stores] ADD  CONSTRAINT [DF_DataStores_enabled]  DEFAULT ((1)) FOR [enabled]
GO
ALTER TABLE [v3d].[StudyGroups] ADD  CONSTRAINT [DF_Groups_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[StudyGroups] ADD  CONSTRAINT [DF_Groups_description]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[SystemSettings] ADD  CONSTRAINT [DF_Settings_subsystem]  DEFAULT ('') FOR [subsystem]
GO
ALTER TABLE [v3d].[SystemSettings] ADD  CONSTRAINT [DF_Settings_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[SystemSettings] ADD  CONSTRAINT [DF_Settings_change]  DEFAULT ((1)) FOR [chg]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_ProcessNew_id]  DEFAULT ('') FOR [id]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_ProcessNew_app]  DEFAULT ('') FOR [app]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_Templates_mod]  DEFAULT ('') FOR [mod]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_ProcessNew_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_Templates_cad]  DEFAULT ('') FOR [cad]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_ProcessNew_default]  DEFAULT ((0)) FOR [default]
GO
ALTER TABLE [v3d].[Templates] ADD  CONSTRAINT [DF_ProcessNew_tasks]  DEFAULT ('') FOR [tasks]
GO
ALTER TABLE [v3d].[Site]  WITH CHECK ADD  CONSTRAINT [CK_SITE] CHECK  (([int]=(0) OR [int]=(1)))
GO
ALTER TABLE [v3d].[Site] CHECK CONSTRAINT [CK_SITE]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Procedure to run (may be stored procedure or executable)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'proc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Flag to indicate the th procedure requires a context id. Only used for T-SQL' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'ctx'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Frequency number may be a # of units or a day mask bit' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'fn'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Parameters used for procedure' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'params'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Maximum time to allow the process to run (Only used for EXE)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'max'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User to run under' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'user'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User password to run under' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs', @level2type=N'COLUMN',@level2name=N'pwd'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of jobs that are to be performed by the database agent' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobs'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Unique application id ( VC, CAR, VAS, ... )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the application name (Used for UI''s)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Priority' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'pri'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Bit Mask' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'mask'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the application mask bit' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'mask'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Installed?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'inst'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Flag indicating that the application is installed, false it is not' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'inst'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Default?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'default'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Default processing flag indicating if the application is to used as the default for processing if we cannot determine an application type from the DICOM' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'default'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Location of the application' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'url'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the launching rules for the application' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications', @level2type=N'COLUMN',@level2name=N'launch'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of all Viatronix applications' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Applications'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Dicom host name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Server name or IP address' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'server'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'AE Title' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'aetitle'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description associated with the dicom host' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Port number the service is listening on' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'port'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Services supported by this host' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'services'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Allowed attributes' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts', @level2type=N'COLUMN',@level2name=N'attributes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of available dicom hosts' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomHosts'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'System Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'sysid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'System Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'sysid'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Date Installed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the system was installed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'System Date' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'sysdate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Current stored system date used to determine tampering' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'sysdate'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Expiration Date' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'expdate'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Expiration date of the system ( NULL = infinite, Default is 10 days from current )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'expdate'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Enabled?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'enabled'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Flag indicating if the sysem is enabled or not' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'enabled'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Error Message' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Error message indicating why the system is disabled' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Licenses Issued' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'cnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Number of licenses issued' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'cnt'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'License Server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'ls'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'License server this system is registered to' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install', @level2type=N'COLUMN',@level2name=N'ls'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains installation information for the Viatronix Enterprise System' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Install'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Masks name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name of bit' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'bn'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Bit Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'bn'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Bit' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'bit'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Bit to be set' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'bit'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description of bit mask' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all the bit mask values for the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'3' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Masks'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Code' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities', @level2type=N'COLUMN',@level2name=N'code'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Modality code as defined in the DICOM standard' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities', @level2type=N'COLUMN',@level2name=N'code'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Modality description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains of list of valid modalities from the DICOM standard' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'3' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Modalities'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site id ( Uniquely identifies the site to Viatronix )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Address Line 1' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'addr1'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s address line 1' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'addr1'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Address Line 2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'addr2'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s address line 2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'addr2'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'City' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'city'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s city name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'city'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'State' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s state ( code of US or name for foreign )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Zip Code' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'zip'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s zip code' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'zip'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Country' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'cntry'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Site''s country' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'cntry'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'International?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'int'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'International flag' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site', @level2type=N'COLUMN',@level2name=N'int'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the site information' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Site'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Name used to access the data store' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Local Root' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'local'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Local root/base path to be added to location fields' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'local'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Remote Root' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'remote'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Remote root/base path preprended to the paths' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'remote'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'server'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Server the datastore resdies on' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'server'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Priority' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'priority'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Order in which the data store are to be used' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'priority'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Enabled?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'enabled'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Determines if the data sore is enabled or disabled' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores', @level2type=N'COLUMN',@level2name=N'enabled'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of valid data storage devices that data can be written or read from' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'3' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Stores'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Group name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Group description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of study groups to be used to help categorize the studies.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyGroups'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Configuration name (unique identifier)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Configuration description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Configuration' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs', @level2type=N'COLUMN',@level2name=N'config'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Configuration xml' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs', @level2type=N'COLUMN',@level2name=N'config'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the configurations used within the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemConfigs'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N' Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Name of the setting' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Subsystem' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'subsystem'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Subsystem the setting belongs to (Helps to further categorize the settings )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'subsystem'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Value' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'value'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Value of the setting' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'value'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description of the setting (What does it do?)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Changable?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'chg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'If true customers may change this setting, flase then only service can' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings', @level2type=N'COLUMN',@level2name=N'chg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all setting used within the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermission', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SystemSettings'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Template id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Application' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Associated application ' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Modality' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Associated modality' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description of template ( What does it do? )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'CAD' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'cad'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Includes CAD (Manufacture code)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'cad'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Default?' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'default'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Default flag' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'default'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Tasks' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'tasks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Tasks list' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates', @level2type=N'COLUMN',@level2name=N'tasks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all the processing templates for the system (based of application)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Templates'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Object Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'object'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Object that is being versioned' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'object'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Object Type' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'type'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Type of object ( Database, Table, ... )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'type'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Version' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'version'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Version of the object' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions', @level2type=N'COLUMN',@level2name=N'version'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of object version used to return the ver attribute in the get procedures.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
