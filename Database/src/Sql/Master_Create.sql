USE [master]
GO

IF NOT EXISTS (SELECT * FROM master.dbo.sysusers WHERE name = 'v3dsa')
  CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[dbo]
GO
IF NOT EXISTS (SELECT * FROM master.dbo.sysusers WHERE name = 'v3duser')
  CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[dbo]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_errorHandler' )
  DROP PROCEDURE [dbo].[sp_errorHandler]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_deleteSystemMessages' )
  DROP PROCEDURE [dbo].[sp_deleteSystemMessages]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_insertSystemMessage' )
  DROP PROCEDURE [dbo].[sp_insertSystemMessage]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_startup' )
  DROP PROCEDURE [dbo].[sp_startup]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_throwNotFoundException' )
  DROP PROCEDURE [dbo].[sp_throwNotFoundException]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_throwInvalidDataException' )
  DROP PROCEDURE [dbo].[sp_throwInvalidDataException]
GO

IF EXISTS ( SELECT * FROM sysobjects WHERE [type]='P' AND [name] = 'sp_throwException' )
  DROP PROCEDURE [dbo].[sp_throwException]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================
-- Author:		  Mark Kontak
-- Create date: 10/22/2013
-- Description:	Throws exception formatting the message
-- =======================================================
CREATE PROCEDURE [dbo].[sp_throwException]
@ERROR_NUMBER INT,
@P1 NVARCHAR(512) = '',
@P2 NVARCHAR(512) = '',
@ERROR_STATE INT = 1,
@ERROR_MESSAGE NVARCHAR(2048) = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- ========================================================
  -- If the ERROR  MESSAGE was specified then do not format
  -- ========================================================
  IF @ERROR_MESSAGE IS NULL
    SELECT @ERROR_MESSAGE = FORMATMESSAGE(@ERROR_NUMBER, @P1, @P2); 


  THROW @ERROR_NUMBER, @ERROR_MESSAGE, @ERROR_STATE

END
GO
GRANT EXECUTE ON [dbo].[sp_throwException] TO [v3dsa] 
GO
GRANT EXECUTE ON [dbo].[sp_throwException] TO [v3duser] 
GO

/****** Object:  StoredProcedure [dbo].[sp_errorHandler]    Script Date: 11/8/2013 12:39:46 PM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/08/2013
-- Description:	Error Handler
-- =============================================
CREATE PROCEDURE [dbo].[sp_errorHandler] 
@DATABASE NVARCHAR(64),
@PROCEDURE NVARCHAR(128),
@ERROR_CODE INT = 0,
@P1 NVARCHAR(128) = '',
@P2 NVARCHAR(128) = '',
@DEADLOCK BIT = 0,
@RETHROW BIT = 1
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  IF @@TRANCOUNT > 0 AND XACT_STATE() = -1
    ROLLBACK TRANSACTION
 
  DECLARE @ERROR_MESSAGE        NVARCHAR(2048)
  DECLARE @ERROR_SEVERITY       TINYINT
  DECLARE @ERROR_STATE          TINYINT
  DECLARE @ERROR_NUMBER         INT
  DECLARE @ERROR_PROCEDURE      SYSNAME
  DECLARE @ERROR_LINE           INT
           
  SELECT  @ERROR_MESSAGE      = ERROR_MESSAGE(), 
          @ERROR_SEVERITY     = ERROR_SEVERITY(), 
          @ERROR_STATE        = ERROR_STATE(), 
          @ERROR_NUMBER       = ERROR_NUMBER(),
          @ERROR_PROCEDURE    = ERROR_PROCEDURE(), 
          @ERROR_LINE         = ERROR_LINE()

  EXEC [dbo].[sp_logErrorMessage] @DATABASE, @PROCEDURE, @ERROR_NUMBER, @ERROR_MESSAGE
 
 -- =============================================
  -- Deadlock Error Number or  if specified
  -- =============================================
	IF ( @DEADLOCK = 1 AND @ERROR_NUMBER = 1205 ) OR @RETHROW = 0
		RETURN @ERROR_NUMBER;
  ELSE
  BEGIN
    IF @ERROR_NUMBER < 900980 AND @ERROR_CODE = 0
      THROW 900998, @ERROR_MESSAGE, @ERROR_STATE
    ELSE
      IF @ERROR_CODE = 0
        THROW @ERROR_NUMBER, @ERROR_MESSAGE, @ERROR_STATE
      ELSE
        EXEC [dbo].[sp_throwException] @ERROR_CODE, @P1, @P2
  END


  RETURN 1;

END


GO
GRANT EXECUTE ON [dbo].[sp_errorHandler] TO [v3dsa] 
GO
GRANT EXECUTE ON [dbo].[sp_errorHandler] TO [v3duser] 
GO




/****** Object:  StoredProcedure [v3d].[sp_deleteSystemMessages]    Script Date: 10/22/2013 1:49:28 PM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

-- ========================================================================
-- Author:		Mark Kontak
-- Create date: 07/30/2012
-- Description:	Deletes all auditing vatronix messages from the system list
-- =========================================================================
CREATE PROCEDURE [dbo].[sp_deleteSystemMessages]
@START INT = 900980,
@END INT = 904000
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  
  DECLARE @MSGNUM AS INT


  SET @MSGNUM = @START

  WHILE @MSGNUM <= @END
  BEGIN
    
    BEGIN TRY
      EXEC sp_dropmessage
        @msgnum   = @MSGNUM
    END TRY
    BEGIN CATCH
     -- Ignore (Dont care if the message does not exist)    
    END CATCH

    SET @MSGNUM = @MSGNUM + 1
  END
 

END
GO
GRANT EXECUTE ON [dbo].[sp_deleteSystemMessages] TO [v3dsa] 
GO


/****** Object:  StoredProcedure [v3d].[sp_insertSystemMessage]    Script Date: 10/22/2013 1:42:03 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/30/2012
-- Description:	Add system message
-- =============================================
CREATE PROCEDURE [dbo].[sp_insertSystemMessage]
@NUMBER INT,            -- Error Message Number
@TEXT NVARCHAR(MAX),    -- Error Text
@SEVERITY INT = 10      -- Severity
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @MSGTEXT AS NVARCHAR(MAX)

  SET @MSGTEXT = CONVERT(NVARCHAR(10), @NUMBER) + ': ' + @TEXT

  EXEC sp_addmessage
    @msgnum   = @NUMBER,
    @severity = @SEVERITY,
    @msgtext  = @MSGTEXT,
    @replace  = 'replace'

END
GO
GRANT EXECUTE ON [dbo].[sp_insertSystemMessage] TO [v3dsa] AS [dbo]
GO


SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 10/22/2013
-- Description:	Throws invalid/missing data exception 
-- ==========================================================================
CREATE PROCEDURE [dbo].[sp_throwInvalidDataException]
@P1 NVARCHAR(512) = '',
@P2 NVARCHAR(512) = ''
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  EXEC [dbo].[sp_throwException] 900990, @P1, @P2
 
END
GO
GRANT EXECUTE ON [dbo].[sp_throwInvalidDataException] TO [v3dsa] 
GO
GRANT EXECUTE ON [dbo].[sp_throwInvalidDataException] TO [v3duser] 
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 10/22/2013
-- Description:	Throws not found exception
-- ==========================================================================
CREATE PROCEDURE [dbo].[sp_throwNotFoundException]
@P1 NVARCHAR(512) = '',
@P2 NVARCHAR(512) = ''
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  EXEC [dbo].[sp_throwException] 900991, @P1, @P2
 
END
GO
GRANT EXECUTE ON [dbo].[sp_throwNotFoundException] TO [v3dsa] 
GO
GRANT EXECUTE ON [dbo].[sp_throwNotFoundException] TO [v3duser] 
GO

/****** Object:  StoredProcedure [dbo].[sp_startup]    Script Date: 5/24/2013 1:05:20 PM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

-- ===============================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/15/2009
-- Description:	Storedprocedure encapsulates all additional statup procedures that are to be run
-- ===============================================================================================
CREATE PROCEDURE [dbo].[sp_startup]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
    
  -- ===============================================================================================
  -- If the Audit Database exists then purge error messages
  -- ===============================================================================================
	IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Auditing')
	BEGIN

    -- Purge old error messages 
    EXEC [Auditing].[v3d].[sp_purgeErrorMessages]

	END

  -- ===============================================================================================
  -- If the System Database exists then check the system and purge any directories that could not
  -- be deleted during normal operation.
  -- ===============================================================================================
	IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'System')
	BEGIN

	  -- Check the system for any issues including the id
	  EXEC [System].[v3d].[sp_checksystem]
      @CHECK_ID= 1

    -- Purge old error messages 
    EXEC [System].[v3d].[sp_purgeErrorMessages]

	END

  -- ===============================================================================================
  -- If the Security Database exists then check the system and purge any directories that could not
  -- be deleted during normal operation.
  -- ===============================================================================================
	IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Security')
	BEGIN

	  -- Check the system for any issues including the id
	  EXEC [System].[v3d].[sp_checksystem]
      @CHECK_ID= 1

    -- Purge old error messages 
    EXEC [Security].[v3d].[sp_purgeErrorMessages]

	END

  -- ===============================================================================================
  -- If the Licensing Database exists then check the system 
  -- ===============================================================================================
	IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Licensing')
	BEGIN

	  -- Check the system for any issues including the id
	  EXEC [System].[v3d].[sp_checksystem]
      @CHECK_ID= 1

    -- Purge old error messages 
    EXEC [Licensing].[v3d].[sp_purgeErrorMessages]


	END


  -- ==================================================================================================
  -- If the Storage Database exists then delete any receivers or dicom servers so that we start fresh
  -- ==================================================================================================
	IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Storage')
	BEGIN
   
    -- Delete any list of receivers
	  EXEC [Storage].[v3d].[sp_deleteReceiver]
      @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893'

	  -- Delete any dicom server list
	  EXEC [Storage].[v3d].[sp_deleteDicomServer]
      @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893'

    -- Delete all locks upon startup
    DELETE [Storage].[v3d].[Locks] 

    -- Resets any jobs that were in process
	  EXEC [Storage].[v3d].[sp_resetJobs]
      @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893'

    -- Purge old error messages 
    EXEC [Storage].[v3d].[sp_purgeErrorMessages]

    -- Clean up any dangling references
    EXEC [Storage].[v3d].[sp_cleanupReferences]

    -- Clean up any dangling volumes
    EXEC [Storage].[v3d].[sp_cleanupVolumes]

 	END

END
GO


-- Set the startup proecdure for the database instance
EXEC sp_procoption N'[dbo].[sp_startup]', 'startup', '1'

GO

