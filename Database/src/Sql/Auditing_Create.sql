USE [Auditing]
GO
/****** Object:  User [Apollo\mkontak]    Script Date: 3/13/2014 8:12:35 AM ******/
CREATE USER [Apollo\mkontak] FOR LOGIN [APOLLO\mkontak] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  User [v3dsa]    Script Date: 3/13/2014 8:12:35 AM ******/
CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  User [v3duser]    Script Date: 3/13/2014 8:12:35 AM ******/
CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  Schema [v3d]    Script Date: 3/13/2014 8:12:35 AM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_deleteProcessingHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 07/19/2013
-- Description:	Delete processing history only supports date removal
--
--
-- =========================================================================
CREATE PROCEDURE [v3d].[sp_deleteProcessingHistory]
@ID NVARCHAR(256),
@FROM DATETIMEOFFSET(7) = NULL,
@TO DATETIMEOFFSET(7) = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'AuditingManagement'


    IF @FROM IS NULL AND @TO IS NULL
      DELETE [v3d].[ProcessingHistory] 
    ELSE IF @FROM IS NOT NULL AND @TO IS NOT NULL
      DELETE [v3d].[ProcessingHistory] WHERE [date] >= @FROM AND [date] <= @TO
    ELSE IF @FROM IS NOT NULL 
      DELETE [v3d].[ProcessingHistory] WHERE [date] >= @FROM 
    ELSE IF @TO IS NOT NULL
      DELETE [v3d].[ProcessingHistory] WHERE [date] <= @TO 

 
  END TRY
	BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteProcessingHistory'
	  
	END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteProcessingHistory] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteReceivingHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 07/19/2013
-- Description:	Delete processing history only supports date removal
--
--
-- =========================================================================
CREATE PROCEDURE [v3d].[sp_deleteReceivingHistory]
@ID NVARCHAR(256),
@FROM DATETIMEOFFSET(7) = NULL,
@TO DATETIMEOFFSET(7) = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'AuditingManagement'


    IF @FROM IS NULL AND @TO IS NULL
      DELETE [v3d].[ReceivingHistory] 
    ELSE IF @FROM IS NOT NULL AND @TO IS NOT NULL
      DELETE [v3d].[ReceivingHistory] WHERE [date] >= @FROM AND [date] <= @TO
    ELSE IF @FROM IS NOT NULL 
      DELETE [v3d].[ReceivingHistory] WHERE [date] >= @FROM 
    ELSE IF @TO IS NOT NULL
      DELETE [v3d].[ReceivingHistory] WHERE [date] <= @TO 

 
  END TRY
	BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteReceivingHistory'
	  
	END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteReceivingHistory] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteStudyHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 07/19/2013
-- Description:	Delete processing history only supports date removal
--
--
-- =========================================================================
CREATE PROCEDURE [v3d].[sp_deleteStudyHistory]
@ID NVARCHAR(256),
@FROM DATETIMEOFFSET(7) = NULL,
@TO DATETIMEOFFSET(7) = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'AuditingManagement'


    IF @FROM IS NULL AND @TO IS NULL
      DELETE [v3d].[StudyHistory] 
    ELSE IF @FROM IS NOT NULL AND @TO IS NOT NULL
      DELETE [v3d].[StudyHistory] WHERE [date] >= @FROM AND [date] <= @TO
    ELSE IF @FROM IS NOT NULL 
      DELETE [v3d].[StudyHistory] WHERE [date] >= @FROM 
    ELSE IF @TO IS NOT NULL
      DELETE [v3d].[StudyHistory] WHERE [date] <= @TO 

 
  END TRY
	BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteStudyHistory'
	  
	END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteStudyHistory] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteUserHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 07/19/2013
-- Description:	Delete processing history only supports date removal
--
--
-- =========================================================================
CREATE PROCEDURE [v3d].[sp_deleteUserHistory]
@ID NVARCHAR(256),
@FROM DATETIMEOFFSET(7) = NULL,
@TO DATETIMEOFFSET(7) = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'AuditingManagement'


    IF @FROM IS NULL AND @TO IS NULL
      DELETE [v3d].[UserHistory] 
    ELSE IF @FROM IS NOT NULL AND @TO IS NOT NULL
      DELETE [v3d].[UserHistory] WHERE [date] >= @FROM AND [date] <= @TO
    ELSE IF @FROM IS NOT NULL 
      DELETE [v3d].[UserHistory] WHERE [date] >= @FROM 
    ELSE IF @TO IS NOT NULL
      DELETE [v3d].[UserHistory] WHERE [date] <= @TO 

 
  END TRY
	BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteUserHistory'
	  
	END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteUserHistory] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_errorHandler]    Script Date: 3/13/2014 8:12:35 AM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getService]    Script Date: 3/13/2014 8:12:35 AM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_insertAgentJobHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		Mark Kontak
-- Create date: 10/15/2009
-- Description:	Inserts the redceiving or updates the image count
-- ===================================================================
CREATE PROCEDURE [v3d].[sp_insertAgentJobHistory]
(
@ID NVARCHAR(256),
@RECORD XML
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @IDOC AS INT
  DECLARE @RC AS INT

  DECLARE @NAME         AS NVARCHAR(64)
  DECLARE @PROCEDURE    AS NVARCHAR(1024)
  DECLARE @MESSAGE      AS NVARCHAR(1024)
  DECLARE @STATE        AS INT
  DECLARE @DATE         AS DATETIMEOFFSET(7)

  RETRY:

  BEGIN TRY

      SET XACT_ABORT ON 
 
    -- =======================================================================================================
    -- Make sure the user can add a study if the authorization flag is set. The flag is set by default but
    -- there is no need to perform multilple authroization when called from another stored procedure.
    -- =======================================================================================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'AuditingManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      SELECT  @NAME         = [name], 
              @PROCEDURE    = [proc],
              @STATE        = [state],
              @MESSAGE      = [msg],
              @DATE         = [date]
              FROM OPENXML(@IDOC, N'/agentjobhistory', 3) 
              WITH 
              (
                [name]            nvarchar(128),
                [proc]            nvarchar(1024),
                [state]           int,
                [msg]             nvarchar(1024),
                [date]            datetimeoffset(7)
              )
   

    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    
    -- ====================================================================================================
    -- If Agent Job Name was supplied
    --    (900990) Study Uid: Missing or invalid
    -- ====================================================================================================
    IF @NAME IS NULL OR LEN(@NAME) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Agent Job Name'
    
    -- Insert history record
    INSERT [v3d].[AgentJobHistory] ( [name], [proc], [state], [msg] ) 
      VALUES ( @NAME, ISNULL(@PROCEDURE,''), ISNULL(@STATE,0), ISNULL(@MESSAGE,'') )
             
  
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_insertAgentJobHistory'


  END CATCH
  	
END




GO
GRANT EXECUTE ON [v3d].[sp_insertAgentJobHistory] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertReceivingHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		Mark Kontak
-- Create date: 10/15/2009
-- Description:	Inserts the redceiving or updates the image count
-- ===================================================================
CREATE PROCEDURE [v3d].[sp_insertReceivingHistory]
(
@SERVER NVARCHAR(32) = '',			  -- V3D Dicom Servers AE Title
@RECEIVER NVARCHAR(256),		      -- Receivers GUID
@SOURCE NVARCHAR(32) = '',			  -- Calling AE Title (PACS, Workstation)
@UID NVARCHAR(256)		            -- Series UID
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	IF ( SELECT COUNT(*) FROM [v3d].[ReceivingHistory] WHERE uid = @UID AND rcvr = @RECEIVER ) = 1
	BEGIN
	  -- Update the image count
	  UPDATE [v3d].[ReceivingHistory] SET icnt = ( icnt + 1 ) WHERE uid = @UID AND rcvr = @RECEIVER
	END
	ELSE
	BEGIN
	  -- Insert history
	  INSERT INTO [v3d].[ReceivingHistory] ( svr, src, rcvr, uid ) VALUES ( @SERVER, @SOURCE, @RECEIVER, @UID )
	END
	
END




GO
GRANT EXECUTE ON [v3d].[sp_insertReceivingHistory] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertReceivingHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertStudyHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/31/2010
-- Description: Adds Study History audit
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertStudyHistory]
(
@UID NVARCHAR(128),             -- Study UID
@ACTION NVARCHAR(16),           -- Action being performed
@LOGIN NVARCHAR(16),            -- User performing the action
@MESSAGE NVARCHAR(MAX) = '',    -- Message
@NAME AS NVARCHAR(512) = ''     -- Patient Name
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;  
  
  BEGIN TRY

    -- Insert into study history
    INSERT INTO [v3d].[StudyHistory] ( [uid], [action], [login], [msg], [name] ) 
        VALUES ( @UID, LOWER(@ACTION), ISNULL(@LOGIN,''), ISNULL(@MESSAGE,''), ISNULL(@NAME,'') )

  END TRY
	BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertStudyHistory'
	  
	END CATCH

  
END




GO
GRANT EXECUTE ON [v3d].[sp_insertStudyHistory] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertStudyHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertUserHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/31/2010
-- Description: Adds UserHistory audit
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertUserHistory]
(
@LOGIN NVARCHAR(16),            -- Login the action is being acted on
@ACTION NVARCHAR(16),           -- Action being performed
@USER NVARCHAR(16) = '',        -- User performing the action
@STATUS BIT = 1,                -- Status of action (1) Success, (0) Failed
@MESSAGE NVARCHAR(2000) = ' ',  -- Message
@EXT AS XML = ''               -- Extended properties
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;  
  
  BEGIN TRY

    -- =======================================================
    -- A login is required. 
    -- (900990) Login missing or invalid
    -- =======================================================
    IF @LOGIN IS NULL OR LEN(@LOGIN) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Login';
    
     -- =======================================================
    -- A action is required. 
    -- (900990) Action missing or invalid
    -- =======================================================
    IF @ACTION IS NULL OR LEN(@ACTION) = 0
    EXEC [master].[dbo].[sp_throwInvalidDataException] 'Action';
       
  
    -- Insert into user history
    INSERT INTO [v3d].[UserHistory] ( [login], [action], [user], [msg], [status], [ext] ) 
      VALUES ( @LOGIN, LOWER(@ACTION), ISNULL(@USER,''),  ISNULL(@MESSAGE,''), @STATUS, ISNULL(@EXT,'') )
  
  END TRY
	BEGIN CATCH

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertUserHistory'
	  
	END CATCH
  
END




GO
GRANT EXECUTE ON [v3d].[sp_insertUserHistory] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertUserHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_reindexTable]    Script Date: 3/13/2014 8:12:35 AM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_getDatabase]    Script Date: 3/13/2014 8:12:35 AM ******/
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

  SET @NAME = 'Auditing'
  
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
/****** Object:  UserDefinedFunction [v3d].[fn_getVersion]    Script Date: 3/13/2014 8:12:35 AM ******/
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

  SELECT @VERSION = [version] FROM [v3d].[Versions] WHERE [object] = @OBJECT

	-- Return the result of the function
	RETURN @VERSION
END





GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getVersion] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[AgentJobHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[AgentJobHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[name] [nvarchar](64) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[proc] [nvarchar](1024) NOT NULL,
	[state] [int] NOT NULL,
	[msg] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_AgentJobHistory] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [v3d].[ProcessingHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[ProcessingHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[series] [nvarchar](128) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[stdte] [datetimeoffset](7) NOT NULL,
	[name] [nvarchar](256) NOT NULL,
	[study] [nvarchar](128) NOT NULL,
	[mod] [nvarchar](16) NOT NULL,
	[app] [nvarchar](16) NOT NULL,
	[tid] [nvarchar](16) NOT NULL,
	[proc] [xml] NOT NULL,
	[msg] [nvarchar](512) NOT NULL,
	[state] [int] NOT NULL,
	[ort] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_ProcessingHistory] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[ProcessingHistory] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[ProcessingHistory] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[ProcessingHistory] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[ProcessingHistory] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[ProcessingHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[ReceivingHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[ReceivingHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[rcvr] [nvarchar](256) NOT NULL,
	[icnt] [int] NOT NULL,
	[src] [nvarchar](32) NOT NULL,
	[svr] [nvarchar](32) NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[msg] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_ReceivingHistory] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT INSERT ON [v3d].[ReceivingHistory] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[ReceivingHistory] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[ReceivingHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[StudyHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[StudyHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[action] [nvarchar](32) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[name] [nvarchar](256) NOT NULL,
	[login] [nvarchar](16) NOT NULL,
	[username] [nvarchar](128) NOT NULL,
	[msg] [nvarchar](2048) NOT NULL,
	[ext] [xml] NOT NULL,
 CONSTRAINT [PK_StudyHistory] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
/****** Object:  Table [v3d].[UserHistory]    Script Date: 3/13/2014 8:12:35 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[UserHistory](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[login] [nvarchar](32) NOT NULL,
	[action] [nvarchar](16) NOT NULL,
	[status] [bit] NOT NULL,
	[user] [nvarchar](16) NOT NULL,
	[msg] [nvarchar](2000) NOT NULL,
	[ext] [xml] NOT NULL,
 CONSTRAINT [PK_UserHistory] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[UserHistory] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[UserHistory] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[UserHistory] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[UserHistory] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[UserHistory] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[UserHistory] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Versions]    Script Date: 3/13/2014 8:12:35 AM ******/
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
ALTER TABLE [v3d].[AgentJobHistory] ADD  CONSTRAINT [DF_AgentJobHistory_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[AgentJobHistory] ADD  CONSTRAINT [DF_AgentJobHistory1_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[AgentJobHistory] ADD  CONSTRAINT [DF_AgentJobHistory1_state]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [v3d].[AgentJobHistory] ADD  CONSTRAINT [DF_AgentJobHistory1_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_series]  DEFAULT ('') FOR [series]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_stdte]  DEFAULT (sysdatetimeoffset()) FOR [stdte]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_study]  DEFAULT ('') FOR [study]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_mod]  DEFAULT ('') FOR [mod]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_app]  DEFAULT ('') FOR [app]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_tid]  DEFAULT ('') FOR [tid]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_state]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [v3d].[ProcessingHistory] ADD  CONSTRAINT [DF_ProcessingHistory_desc]  DEFAULT ('') FOR [ort]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivinHistory_uid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivinHistory_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivingHistory_receiver]  DEFAULT ('') FOR [rcvr]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivinHistory_imageCount]  DEFAULT ((0)) FOR [icnt]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivingHistory_message]  DEFAULT ('') FOR [src]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivingHistory_server]  DEFAULT ('') FOR [svr]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivingHistory_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[ReceivingHistory] ADD  CONSTRAINT [DF_ReceivingHistory_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_uid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_action]  DEFAULT ('') FOR [action]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_user]  DEFAULT ('') FOR [login]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_username]  DEFAULT ('') FOR [username]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_StudyHistory_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[StudyHistory] ADD  CONSTRAINT [DF_Table_1_descv]  DEFAULT ('') FOR [ext]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_login]  DEFAULT ('') FOR [login]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_activity]  DEFAULT ('') FOR [action]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_status]  DEFAULT ((1)) FOR [status]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_user]  DEFAULT ('') FOR [user]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_description]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[UserHistory] ADD  CONSTRAINT [DF_UserHistory_prop]  DEFAULT ('') FOR [ext]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobHistory', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the Agent Job history' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'AgentJobHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Identifier for record' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series  Uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'series'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date of the entry (Either the failed or complete date)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the task started' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'stdte'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient Name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study Uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'study'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series modality' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Application the process was sun under' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Template Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'tid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Process/Task used in this entry' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'proc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Error message' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'State (Completed or Failed)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series orientation' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory', @level2type=N'COLUMN',@level2name=N'ort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the processing history for the Eneterprise System' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ProcessingHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Unique identifier' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the dicom was reecieved' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Receiver''s id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'rcvr'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Image count' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'icnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Source of the dicom (AE Title)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'src'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Our dicom server receiving the dicom' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'svr'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains error message if a race condition was detected suring the receiving process.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a history of DICOM data received ' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'ReceivingHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Action being performed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'action'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date of the operation' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User performing the action' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'login'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Users full name not just their login.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'username'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Message' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Extended properties' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory', @level2type=N'COLUMN',@level2name=N'ext'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a history of study/patient actions' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'StudyHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Unqiue identifier' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Status bit 1 indicates success, 0 indicates failed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory', @level2type=N'COLUMN',@level2name=N'status'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Login of the user who performed the action' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory', @level2type=N'COLUMN',@level2name=N'user'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Brief description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a history of user actions (add, delete, password change, .. .)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'UserHistory'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of object version used to return the ver attribute in the get procedures.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
