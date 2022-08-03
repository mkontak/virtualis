USE [Storage]
GO
/****** Object:  User [Apollo\mkontak]    Script Date: 3/19/2014 4:44:49 PM ******/
CREATE USER [Apollo\mkontak] FOR LOGIN [APOLLO\mkontak] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  User [v3dsa]    Script Date: 3/19/2014 4:44:49 PM ******/
CREATE USER [v3dsa] FOR LOGIN [v3dsa] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  User [v3duser]    Script Date: 3/19/2014 4:44:49 PM ******/
CREATE USER [v3duser] FOR LOGIN [v3duser] WITH DEFAULT_SCHEMA=[v3d]
GO
/****** Object:  Schema [v3d]    Script Date: 3/19/2014 4:44:49 PM ******/
CREATE SCHEMA [v3d]
GO
GRANT ALTER ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT CONTROL ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT REFERENCES ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT TAKE OWNERSHIP ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT VIEW DEFINITION ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT VIEW CHANGE TRACKING ON SCHEMA::[v3d] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
GRANT EXECUTE ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON SCHEMA::[v3d] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_addApplication]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================================
-- Author:		  Mark Kontak
-- Create date: 01/14/2014
-- Description:	Adds the application in the result set if is not already there and the application
--              specified exists.
-- =======================================================================================================
CREATE PROCEDURE [v3d].[sp_addApplication]
@ID AS NVARCHAR(256),
@APP AS NVARCHAR(32),
@RESULTS AS XML OUTPUT
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
  DECLARE @XML AS XML
    
  SET @RC = 0

  -- ============================================================================================================================
  -- only insert the application into the result set if the application exists and is installed and it is not already in the
  -- result set.
  -- ============================================================================================================================
  IF EXISTS ( SELECT * FROM [System].[v3d].[Applications] WHERE [id] = @APP AND [inst] = 1 ) AND
     NOT EXISTS ( SELECT n.query('.') FROM @RESULTS.nodes('/collection/application[@id=sql:variable(''@APP'')]') AS a(n) )
  BEGIN

    -- =====================
    -- Gets the application
    -- =====================
    EXEC [System].[v3d].[sp_getApplication] 
        @ID = @ID, 
        @APP = @APP, 
        @RESULTS = @XML OUTPUT


    -- Removes the launch rule
    SET @XML.modify('delete (/collection/application/launch)') 

    -- Get the application node
    SELECT @XML = n.query('.') FROM @XML.nodes('/collection/application') AS a(n)
         
    SET @RESULTS.modify('insert sql:variable("@XML") into (/collection)[1]')

    SET @RC = 1

  END

  RETURN @RC

END



GO
GRANT EXECUTE ON [v3d].[sp_addApplication] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_addApplication] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_addSeriesApplications]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================
-- Author:		  Mark Kontak
-- Create date: 01/14/2014
-- Description:	Adds the applications that the series can launch
-- ============================================================================
CREATE PROCEDURE [v3d].[sp_addSeriesApplications]
@ID AS NVARCHAR(256),                 -- Context Id
@APPS NVARCHAR(MAX),                  -- Valid application list (May be NULL)
@UID NVARCHAR(128),                   -- Series uid
@LID UNIQUEIDENTIFIER = NULL OUTPUT,  -- Dataset id
@RESULTS XML OUTPUT                   -- Result set
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @APP AS NVARCHAR(32)
  DECLARE @TYPE AS INT
  DECLARE @LAUNCH AS XML
  DECLARE @TRUE AS BIT
  DECLARE @FALSE AS BIT
  DECLARE @ADD_APP AS BIT
  DECLARE @QUERY AS XML
  DECLARE @SERIES AS XML
  DECLARE @APP_EXISTS AS BIT
  DECLARE @SESSION AS INT
  DECLARE @DICOM   AS INT
  DECLARE @RULE_TYPES AS INT


  SELECT  @TRUE = 1, 
          @FALSE = 0, 
          @LAUNCH = NULL, 
          @ADD_APP = 0,
          @SESSION = [System].[v3d].[fn_getMaskBit]('type', 'session'),
          @DICOM   = [System].[v3d].[fn_getMaskBit]('type', 'dicom'),
          @APP_EXISTS = 0,
          @LID = ISNULL(@LID, NEWID()),
          @QUERY = '<series uid="' + @UID + '" />'
 
  SELECT @APP = [app], @TYPE = [type]
     FROM [v3d].[Series] 
    WHERE [uid] = @UID AND 
          (( [type] != @SESSION AND [System].[v3d].[fn_isMaskBitSet]([state], 'state', 'completed') = 1 ) OR
            [type] = @SESSION  )

  

  -- ==========================================
  -- Make sure the series exists in the system
  -- ==========================================
  IF @@ROWCOUNT = 1
  BEGIN


    -- =========================================================================
    -- If the series has an application then we add that application to the list
    -- ==========================================================================
    IF LEN(@APP) <> 0 
    BEGIN


      SELECT  @APP_EXISTS = @TRUE

      -- =====================================================================
      -- Only need to add the application if it does not exists in the list
      -- =====================================================================
      IF NOT EXISTS ( SELECT n.query('.') FROM @RESULTS.nodes('/collection/application[@id=sql:variable(''@APP'')]') AS a(n) )
      BEGIN

        SELECT @ADD_APP = @TRUE, @APP_EXISTS = @FALSE



        -- =======================================================================================================
        -- If the series type is not a session then we need to check to make sure that the type appears in the 
        -- applications launch rule.
        -- =======================================================================================================
        IF @TYPE <> @SESSION
        BEGIN
        

          -- Get the applications launch rule
          SELECT @LAUNCH = [launch] FROM [System].[v3d].[Applications] WHERE [id] = @APP

          -- ========================================================
          -- Only check the launch rule if the application was found
          -- =======================================================
          IF @@ROWCOUNT = 1
          BEGIN
          

            SELECT @RULE_TYPES  = n.value('@type[1]','int') FROM @LAUNCH.nodes('/rule[1]') AS a(n)

            -- ============================================================================================
            -- Set the flag to add the app to false if the series type does not appear in the launch rule
            -- =============================================================================================
            IF ( @RULE_TYPES & @TYPE ) <> @TYPE 
               SET @ADD_APP = @FALSE
          END
          
        
        
        END -- END ... If the series is not a session

        -- =========================================
        -- If we are to add the app then do it here.
        -- =========================================
        IF @ADD_APP = @TRUE
        BEGIN

          EXEC [v3d].[sp_addApplication] 
              @ID = @ID, 
              @APP = @APP, 
              @RESULTS = @RESULTS OUTPUT
              
           SET @APP_EXISTS = @TRUE
             
        END

      END   -- END ... If the application is not already in the list

    END -- END ... If the series is linked to an specific application

    -- ====================
    -- get the series xml
    -- =====================
    EXEC [v3d].[sp_getSeries] 
        @ID = @ID,
        @QUERY = @QUERY,
        @RESULTS = @SERIES OUTPUT

    -- ====================================================
    -- Delete the references node from under the series
    -- ====================================================    
    SET @SERIES.modify('delete (/collection/series/references)')

    -- =============================================
    -- Delete the proc node from under the series
    -- =============================================
    SET @SERIES.modify('delete (/collection/series/proc)')

    -- =============================================
    -- Delete the volumes from under the series
    -- =============================================
    SET @SERIES.modify('delete (/collection/series/volumes)')

    -- ====================
    -- Get the series node
    -- ====================
    SELECT @SERIES = n.query('.') FROM @SERIES.nodes('/collection/series') AS a(n)

      
    -- ======================================================
    -- For DICOM series we need to check all the applications
    -- ======================================================
    IF @TYPE = @DICOM
    BEGIN

      DECLARE APP_CURSOR CURSOR LOCAL FOR SELECT [id], [launch] FROM [System].[v3d].[Applications] WHERE [inst] = @TRUE AND PATINDEX('%|' + [id] + '|%', @APPS ) > 0 
     
      
      OPEN APP_CURSOR

      FETCH NEXT FROM APP_CURSOR INTO @APP, @LAUNCH

      WHILE @@FETCH_STATUS = 0 
      BEGIN
    
        SELECT @RULE_TYPES  = n.value('@type[1]','int') FROM @LAUNCH.nodes('/rule[1]') AS a(n)

        -- =======================================================================================================
        -- If the Dicom type appears in the laucnh list for the installed application then attempt to add the
        -- application to the list
        -- =======================================================================================================
        IF ( @RULE_TYPES & @TYPE ) = @TYPE
        BEGIN

           EXEC [v3d].[sp_addApplication] 
              @ID = @ID, 
              @APP = @APP, 
              @RESULTS = @RESULTS OUTPUT
 
          -- =============================================================================================================
          -- If the current application/dataset exists then inser the series into the dataset otherwise its a new dataset
          -- =============================================================================================================
          IF NOT EXISTS ( SELECT n.query('.') FROM @RESULTS.nodes('/collection/application[@id=sql:variable("@APP")]/dataset[@id=sql:variable("@LID")]') AS a(n)  )
          BEGIN
            SET @LAUNCH = '<dataset id="' + CONVERT(NVARCHAR(256), @LID) + '"></dataset>'

            SET @LAUNCH.modify('insert sql:variable("@SERIES") into (/dataset)[1]')
  
            SET @RESULTS.modify('insert sql:variable("@LAUNCH") into (/collection/application[@id=sql:variable("@APP")])[1]')
          END
          ELSE
          BEGIN
           SET @RESULTS.modify('insert sql:variable("@SERIES") into (/collection/application[@id=sql:variable("@APP")]/dataset[@id=sql:variable("@LID")])[1]')

          END

        END -- END ... If the series type 'Dicom' exists in the launch list


        FETCH NEXT FROM APP_CURSOR INTO @APP, @LAUNCH

      END -- END ... While there are applications


      CLOSE APP_CURSOR
      DEALLOCATE APP_CURSOR

    END -- END ... If the series  is a DICOM
    
 
    -- =========================================================
    -- If the application exists then we need to add the dataset
    -- =========================================================
    ELSE IF @APP_EXISTS = @TRUE
    BEGIN
 
      -- =============================================================================================================
      -- If the current application/dataset exists then inser the series into the dataset otherwise its a new dataset
      -- =============================================================================================================     
      IF NOT EXISTS ( SELECT n.query('.') FROM @RESULTS.nodes('/collection/application[@id=sql:variable("@APP")]/dataset[@id=sql:variable("@LID")]') AS a(n)  )
      BEGIN
        SET @LAUNCH = '<dataset id="' + CONVERT(NVARCHAR(256), @LID) + '"></dataset>'

        SET @LAUNCH.modify('insert sql:variable("@SERIES") into (/dataset)[1]')
  
        SET @RESULTS.modify('insert sql:variable("@LAUNCH") into (/collection/application[@id=sql:variable("@APP")])[1]')
      END
      ELSE
      BEGIN
        SET @RESULTS.modify('insert sql:variable("@SERIES") into (/collection/application[@id=sql:variable("@APP")]/dataset[@id=sql:variable("@LID")])[1]')

      END


    END -- END ... If the application exists in the list


  END --END .. Else if the application exists 
 
 
END



GO
GRANT EXECUTE ON [v3d].[sp_addSeriesApplications] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_addSeriesApplications] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_associationCompleted]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/14/2011
-- Description:	Performs the necessary actions when the association completion event 
--              has been detected.
-- =====================================================================================
CREATE PROCEDURE [v3d].[sp_associationCompleted]
(
@ID NVARCHAR(256),              -- Context Id
@ASSOCIATION NVARCHAR(128)      -- Association
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON                                              

  DECLARE @COMPLETED AS INT
  DECLARE @RECEIVING AS INT
  DECLARE @PENDING AS INT
  DECLARE @PROCESSING AS INT
  DECLARE @STATE AS INT
  DECLARE @TRANSACTION AS INT
  DECLARE @AUTO AS NVARCHAR(10)
  DECLARE @LOCK AS INT
  DECLARE @LOCK_RESOURCE AS NVARCHAR(128)
  DECLARE @TRANSCOUNT AS INT
  DECLARE @OPEN AS BIT


  DECLARE @RC AS INT

  -- ===========================
  -- Initialize local variables
  -- ============================
  SELECT  @RC = 0,
          @LOCK = 0,
          @OPEN = 0,
          @TRANSCOUNT = @@TRANCOUNT,
          @LOCK_RESOURCE = 'ASSOCIATION_COMPLETED',
          @AUTO = UPPER([System].[v3d].[fn_getSetting]('Auto', 'False')),
          @COMPLETED = [v3d].[fn_getState]('completed'),
          @RECEIVING = [v3d].[fn_getState]('receiving'),
          @PENDING = [v3d].[fn_getState]('pending'),
          @PROCESSING = [v3d].[fn_getState]('processing')

   DECLARE RECEIVER_CURSOR CURSOR LOCAL
      FOR SELECT [uid], [new], [aetitle], [calling], [name]  
           FROM [v3d].[Receivers] 
          WHERE [v3d].[Receivers].[association] = @ASSOCIATION


  RETRY:


  BEGIN TRY
 
    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    SET @OPEN = 0
   
    -- Lock Dicom Receiving
    EXEC @LOCK = sp_getapplock 
              @Resource = @LOCK_RESOURCE, 
              @LockMode = 'Exclusive', 
              @LockOwner = 'Transaction', 
              @LockTimeout = 60000
                
    --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

      EXEC [master].[dbo].[sp_throwException] 901401, 'Association Completed'
        
    END -- END ... If the lock failed

      -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    
    DECLARE @UID AS NVARCHAR(256)
    DECLARE @CNT AS INT
    DECLARE @CALLED_AETITLE AS NVARCHAR(64)
    DECLARE @CALLING_AETITLE AS NVARCHAR(64)
    DECLARE @NAME AS NVARCHAR(128)

    SET @OPEN = 1

    OPEN RECEIVER_CURSOR
   
    FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME


    WHILE @@FETCH_STATUS = 0 
    BEGIN

        IF @CNT > 0 
        BEGIN

          IF @AUTO = 'TRUE' 
          BEGIN
            SET @STATE = @PENDING

            -- Insert a message for each series beging received
            INSERT INTO [v3d].[Messages] ( [type], [app], [name], [src], [msg],  [ort] ) 
                SELECT  0, 
                        [v3d].[Series].[app], 
                        [v3d].[Studies].[name], 
                        [v3d].[Series].[uid], 
                        'Receiving completed - Number of images: ' + CONVERT(NVARCHAR(16),@CNT),                   
                        [v3d].[Series].[ort] 
                  FROM [v3d].[Series]
                      INNER JOIN [v3d].[Studies] ON [v3d].[Studies].[uid] = [v3d].[Series].[suid]
                  WHERE [v3d].[Series].[uid] = @UID

          END
          ELSE
            SET @STATE = @RECEIVING


          -- ====================================================================================================================
          -- Only update  the series state and count as well as insert the job if the series is not being received on another
          -- association.
          -- =====================================================================================================================
          IF NOT EXISTS ( SELECT * FROM [v3d].[Receivers] WHERE [uid] = @UID AND [association] <> @ASSOCIATION )
          BEGIN

            -- =========================================
            -- Updates the series image count and state
            --    Set the PENDING bit
            --    Unset the RECEIVING bit
            -- ==========================================
            UPDATE [v3d].[Series] 
               SET [icnt] = [icnt] + @CNT,
                    [state] = ( [state] | @PENDING | @RECEIVING ) ^ @RECEIVING
             WHERE [v3d].[Series].[uid] = @UID
 
            -- ======================================
            -- Attempt to update a current job first
            -- ======================================
            UPDATE [v3d].[Jobs] 
                SET [icnt] = [icnt] + @CNT
              WHERE [src] = @UID AND [dst] = '' AND ( [state] = @PENDING OR [state] = @RECEIVING )
       
           
            IF @@ROWCOUNT = 0
            BEGIN

              EXEC [v3d].[sp_insertJob]
                  @ID     = @ID,         -- Authentication token
                  @UID    = @UID,        -- DICOM series UID
                  @STATE  = @STATE       -- Set intial state to pending

            END

              


          END -- END ... If the series is not being received on another association
          ELSE
          BEGIN

            -- ===============================
            -- Updates the series image count 
            -- ===============================
            UPDATE [v3d].[Series] 
               SET [icnt] = [icnt] + @CNT,
                   [state] = [state] | @RECEIVING
             WHERE [v3d].[Series].[uid] = @UID


             UPDATE [v3d].[Jobs] 
                SET [state] = @RECEIVING,
                    [icnt] = [icnt] + @CNT
              WHERE [src] = @UID AND [dst] = '' AND  ( [state] = @PENDING OR [state] = @RECEIVING )

          END

          -- Insert history
          INSERT INTO [Auditing].[v3d].[ReceivingHistory] ( [svr], [src], [rcvr], [uid], [icnt], [name] )
              VALUES ( @CALLED_AETITLE, @CALLING_AETITLE, @ASSOCIATION, @UID, @CNT, @NAME )

          -- ============================================
          -- Serialize the series
          -- ===========================================
          EXEC [v3d].[sp_serializeSeries]
                @UID = @UID

       END  -- END ... If the receiver actually received images that means no duplicates

       FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME

    END -- END ... while there are receiver records

    CLOSE RECEIVER_CURSOR
    DEALLOCATE RECEIVER_CURSOR

    SET @OPEN = 0

    --- Deletes all receiving records 
    DELETE FROM [v3d].[Receivers] WHERE [association] = @ASSOCIATION

    -- Number of series-aasociations deleted from the receivers table
    SET @RC = @@ROWCOUNT

    IF @AUTO = 'TRUE' AND @RC > 0
      EXEC [v3d].[sp_raisePendingJobsEvent]

     --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

      EXEC [master].[dbo].[sp_throwException] 901401, 'Association Completed'
        
    END -- END ... If the lock failed

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION;

    RETURN @RC

  END TRY
  BEGIN CATCH
        
    IF @OPEN = 1
    BEGIN
     CLOSE RECEIVER_CURSOR
     DEALLOCATE RECEIVER_CURSOR
    END

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION

   --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

        
      EXEC [master].[dbo].[sp_throwException] 901401, 'Association Completed'
 
    END -- END ... If the lock failed

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_associationCompleted',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
 
END





GO
GRANT EXECUTE ON [v3d].[sp_associationCompleted] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_associationCompleted] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_associationFailed]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/14/2011
-- Description:	Performs the necessary actions when the association failed event 
--              has been detected.
-- =====================================================================================
CREATE PROCEDURE [v3d].[sp_associationFailed]
(
@ID NVARCHAR(256),              -- Context Id
@ASSOCIATION NVARCHAR(128),     -- Associatio Id
@MESSAGE NVARCHAR(512) = ''     -- Message 
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RECEIVING AS INT
  DECLARE @PENDING AS INT
  DECLARE @STATE AS INT
  DECLARE @TRANSACTION AS INT
  DECLARE @AUTO AS NVARCHAR(10)
  DECLARE @LOCK AS INT
  DECLARE @LOCK_RESOURCE AS NVARCHAR(128)
  DECLARE @TRANSCOUNT AS INT
  DECLARE @OPEN AS BIT


  DECLARE @RC AS INT

  -- ===========================
  -- Initialize local variables
  -- ============================
  SELECT  @RC = 0,
          @LOCK = 0,
          @OPEN = 0,
          @TRANSCOUNT = @@TRANCOUNT,
          @LOCK_RESOURCE = 'ASSOCIATION_FAILED',
          @AUTO = UPPER([System].[v3d].[fn_getSetting]('Auto', 'False')),
          @RECEIVING = [v3d].[fn_getState]('receiving'),
          @PENDING = [v3d].[fn_getState]('pending')
 

   DECLARE RECEIVER_CURSOR CURSOR LOCAL
        FOR SELECT [uid], [cnt], [aetitle], [calling], [name]  
              FROM [v3d].[Receivers] 
              WHERE [v3d].[Receivers].[association] = @ASSOCIATION

  RETRY:

  BEGIN TRY

    SET XACT_ABORT ON                                              

    BEGIN TRANSACTION
 
     SET @OPEN = 0
  
    -- Lock Dicom Receiving
    EXEC @LOCK = sp_getapplock 
              @Resource = @LOCK_RESOURCE, 
              @LockMode = 'Exclusive', 
              @LockOwner = 'Transaction', 
              @LockTimeout = 60000
                
    --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

      EXEC [master].[dbo].[sp_throwException] 901401,  'Association Failed'
        
    END -- END ... If the lock failed

      -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    
    DECLARE @UID AS NVARCHAR(256)
    DECLARE @CNT AS INT
    DECLARE @CALLED_AETITLE AS NVARCHAR(64)
    DECLARE @CALLING_AETITLE AS NVARCHAR(64)
    DECLARE @NAME AS NVARCHAR(128)

    SET @OPEN = 1

    OPEN RECEIVER_CURSOR
   
    FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME


    WHILE @@FETCH_STATUS = 0 
    BEGIN

      
      -- Insert a message for each series beging received
      INSERT INTO [v3d].[Messages] ( [type], [app], [name], [src], [msg],  [ort] ) 
          SELECT  0, 
                  [v3d].[Series].[app], 
                  [v3d].[Studies].[name], 
                  [v3d].[Series].[uid], 
                  'Receiving  error - ' + CONVERT(NVARCHAR(16),@CNT) + ' received : ' + @MESSAGE,                   
                  [v3d].[Series].[ort] 
            FROM [v3d].[Series]
                INNER JOIN [v3d].[Studies] ON [v3d].[Studies].[uid] = [v3d].[Series].[suid]
            WHERE [v3d].[Series].[uid] = @UID

      -- ===============================
      -- Updates the series image count 
      -- ===============================
      UPDATE [v3d].[Series] 
         SET [icnt] = [icnt] + @CNT
       WHERE [v3d].[Series].[uid] = @UID


      -- ======================================
      -- Attempt to update a current job first
      -- ======================================
      UPDATE [v3d].[Jobs] 
         SET [state] = @RECEIVING,
             [icnt] = [icnt] + @CNT
       WHERE [src] = @UID AND [dst] = '' AND ( [state] = @PENDING OR [state] = @RECEIVING )
                   
      IF @@ROWCOUNT = 0
      BEGIN
        EXEC [v3d].[sp_insertJob]
                @ID     = @ID,         -- Authentication token
                @UID    = @UID,        -- DICOM series UID
                @STATE  = @RECEIVING   -- Set intial state to pending
      END

      -- =================================================================================
      -- Setting the series state with NO STATE specified sets the statebased on the JOBS
      -- ================================================================================
      EXEC [v3d].[sp_setSeriesState]   
        @UID = @UID
    
 	    -- Insert history
	    INSERT INTO [Auditing].[v3d].[ReceivingHistory] ( [svr], [src], [rcvr], [uid], [icnt], [name], [msg] )
           VALUES ( @CALLED_AETITLE, @CALLING_AETITLE, @ASSOCIATION, @UID, @CNT, @NAME, @MESSAGE )

      -- ============================================
      -- Serialize the series
      -- ===========================================
      EXEC [v3d].[sp_serializeSeries]
            @UID = @UID

        FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME

    END -- END ... while there are receiver records

    CLOSE RECEIVER_CURSOR
    DEALLOCATE RECEIVER_CURSOR

    SET @OPEN = 0

    --- Deletes all receiving records 
    DELETE FROM [v3d].[Receivers] WHERE [association] = @ASSOCIATION

    -- Number of series-aasociations deleted from the receivers table
    SET @RC = @@ROWCOUNT

    --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

       EXEC [master].[dbo].[sp_throwException] 901401, 'Association Failed'
        
    END -- END ... If the lock failed


    COMMIT TRANSACTION;

    RETURN @RC

  END TRY
  BEGIN CATCH
 
    IF @OPEN = 1
    BEGIN
       CLOSE RECEIVER_CURSOR
        DEALLOCATE RECEIVER_CURSOR
    END       
 
    ROLLBACK TRANSACTION

   --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

       EXEC [master].[dbo].[sp_throwException] 901401, 'Association Failed' 
        
    END -- END ... If the lock failed


    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_associationFailed',
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
	
  END CATCH
 
END





GO
GRANT EXECUTE ON [v3d].[sp_associationFailed] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_associationFailed] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_cleanupLocks]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2013
-- Description:	Delete all expired locks
-- =============================================
CREATE PROCEDURE [v3d].[sp_cleanupLocks]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  BEGIN TRY
    
    
    BEGIN TRANSACTION

    DECLARE @TTL AS INT
    DECLARE @PROCEDURE AS NVARCHAR(128)

    -- ====================================================================
    -- Get the Lock Ttl setting, if not found then default to 60 minutes
    -- ===================================================================
    SELECT @TTL = CONVERT(INT, [System].[v3d].[fn_getSetting]('LockTtl','60')),
           @PROCEDURE = OBJECT_NAME(@@PROCID)
    
    -- =========================================================================
    -- Delete any expired locks or locks that do not have an associated context
    -- =========================================================================
    DELETE [v3d].[Locks] WHERE DATEADD(minute,@TTL,[date]) < GETDATE() OR
                              ( [ctx] <> 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893' AND 
                                [ctx] NOT IN ( SELECT [id] FROM [Security].[v3d].[Contexts] WHERE [id] = [v3d].[Locks].[ctx] ) )

    IF  XACT_STATE() = 1
      COMMIT TRANSACTION

    RETURN @@ROWCOUNT
  
  END TRY
  BEGIN CATCH
 
   IF XACT_STATE() = -1
    ROLLBACK TRANSACTION

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE

  END CATCH

END




GO
/****** Object:  StoredProcedure [v3d].[sp_cleanupReferences]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/07/2013
-- Description:	Cleans up any dangling references
-- ==================================================
CREATE PROCEDURE [v3d].[sp_cleanupReferences]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT
 
  SELECT  @RC = 0

  BEGIN TRY

    DELETE [v3d].[SeriesReferences] WHERE [v3d].[SeriesReferences].[uid] NOT IN 
    ( SELECT [uid] FROM [v3d].[Series] WHERE [v3d].[Series].[uid] = [v3d].[SeriesReferences].[uid]  )

    SET @RC = @@ROWCOUNT

    DELETE [v3d].[SeriesReferences] WHERE [v3d].[SeriesReferences].[ruid] NOT IN 
      ( SELECT [uid] FROM [v3d].[Series] WHERE [v3d].[Series].[uid] = [v3d].[SeriesReferences].[ruid]  )

    SET @RC = @RC + @@ROWCOUNT


    RETURN @RC

  END TRY
  BEGIN CATCH
  

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_cleanupRefernces'

  END CATCH
END




GO
GRANT EXECUTE ON [v3d].[sp_cleanupReferences] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_cleanupSeriesMessages]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes any messages associated with the suuplied series uid
-- =============================================================================
CREATE PROCEDURE [v3d].[sp_cleanupSeriesMessages]
(
@UID AS NVARCHAR(128)
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  IF EXISTS ( SELECT * FROM [sys].[databases] WHERE [name] = 'Processing' )
  BEGIN

    DECLARE @RECORD AS NVARCHAR(128)

    SET @RECORD = '<message src="' + @UID + '" />'

    EXEC [v3d].[sp_deleteMessage]  
      @ID = 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893',
      @RECORD = @RECORD

  END
  
  RETURN @@ROWCOUNT

END





GO
GRANT EXECUTE ON [v3d].[sp_cleanupSeriesMessages] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_cleanupSeriesMessages] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_cleanupVolumes]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/07/2013
-- Description:	Cleans up any dangling volumes
-- ==================================================
CREATE PROCEDURE [v3d].[sp_cleanupVolumes]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT

  SELECT  @RC = 0

  BEGIN TRY

    DELETE [v3d].[Volumes] WHERE [v3d].[Volumes].[uid] NOT IN 
    ( SELECT [uid] FROM [v3d].[Series] WHERE [v3d].[Series].[uid] = [v3d].[Volumes].[uid]  )

    SET @RC = @@ROWCOUNT


    RETURN @RC

  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_cleanupVolumes'

  END CATCH
END




GO
GRANT EXECUTE ON [v3d].[sp_cleanupVolumes] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_cleanupVolumes] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_closeSession]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 06/10/2013
-- Description:	Closes the sesion
-- =============================================
CREATE PROCEDURE [v3d].[sp_closeSession]
@ID   NVARCHAR(256),
@UID  NVARCHAR(128)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC				    AS INT
  DECLARE @TRANSCOUNT   AS INT
  DECLARE @SESSION      AS INT
   
  SELECT @TRANSCOUNT  = @@TRANCOUNT,
         @SESSION     = [System].[v3d].[fn_getMaskBit]('type','session')


  BEGIN TRY

    SET XACT_ABORT ON

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    -- ====================================
    -- Make sure the user can add a study
    -- ====================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    

    -- ==================================================================
    -- If the UID was a series we lock the series and all it's referneces
    -- ==================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Series] WHERE [uid] = @UID AND [type] = @SESSION )
    BEGIN

      -- ======================================================================================
      -- Delete the locks associated with the session. The session UID was used as the trans id
      -- =======================================================================================
      EXEC @RC = [v3d].[sp_deleteLock]  
        @ID = @ID,
        @TRANSID = @UID

    END
    ELSE
       EXEC [master].[dbo].[sp_throwNotFoundException] 'Session' 


 
    IF @TRANSCOUNT = 0
       COMMIT TRANSACTION

    RETURN @RC
   
  END TRY
  BEGIN CATCH
   
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION


    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_closeSession'
  
  END CATCH

END




GO
GRANT EXECUTE ON [v3d].[sp_closeSession] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_closeSession] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_completeJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description: Completes the specified job
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_completeJob]
(
@JOBID NVARCHAR(256)
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
	DECLARE @RC AS INT
  DECLARE @TRANSCOUNT AS INT


  SELECT @TRANSCOUNT = @@TRANCOUNT

	BEGIN TRY
 

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    DECLARE @JID                AS UNIQUEIDENTIFIER
    DECLARE @UID                AS NVARCHAR(128)
    DECLARE @COMPLETED          AS INT
    DECLARE @PENDING            AS INT
    DECLARE @PROCESSING         AS INT
    DECLARE @JOB_STATES         AS BIGINT 
    
    -- ======================================
    -- Makes sure the JOB id was specified
    -- (900990) Job Id: Missing or Invalid
    -- =======================================
    IF @JOBID IS NULL OR LEN(@JOBID) = 0
       EXEC [master].[dbo].[sp_throwInvalidDataException]  'Job Id'

    SET @JID = CONVERT(UNIQUEIDENTIFIER,@JOBID);

    DECLARE @VOLUME       AS NVARCHAR(32)
    DECLARE @CID          AS NVARCHAR(32)
    DECLARE @TASK         AS XML
    DECLARE @APP          AS NVARCHAR(16)
    DECLARE @INDEX        AS INT
    DECLARE @TID          AS NVARCHAR(32)
    DECLARE @LOGIN        AS NVARCHAR(32)
    DECLARE @ORT          AS NVARCHAR(32)
    DECLARE @MOD          AS NVARCHAR(32)
    DECLARE @DESC         AS NVARCHAR(512)
    DECLARE @NAME         AS NVARCHAR(256)
    DECLARE @ICNT         AS INT
    DECLARE @SRC          AS NVARCHAR(128)
    DECLARE @STUDY        AS NVARCHAR(128)
    DECLARE @DST          AS NVARCHAR(128)
    DECLARE @STDTE        AS DATETIMEOFFSET(7)
    DECLARE @CRDTE        AS DATETIMEOFFSET(7)
    DECLARE @RAISE_EVENT  AS BIT
 
    -- Extract data from the current job (will be used to seed the next job)
    SELECT  @APP          = [app],                  -- Application type
            @VOLUME       = [vol],                  -- Volume name
            @CID          = [cid],                  -- Conversion id
            @TASK         = [task],                 -- Task XML
            @TID          = [tid],                  -- Template Id
            @DST          = [dst],                  -- Destination UID
            @LOGIN        = [login],                -- Login
            @ORT          = [ort],                  -- Orientation
            @DESC         = [desc],                 -- Series Description
            @NAME         = [name],                 -- Patient name
            @MOD          = [mod],                  -- Modality
            @ICNT         = [icnt],                 -- Image count
            @SRC          = [src],                  -- Source UID
            @STUDY        = [study],                -- Study UID
            @STDTE        = [stdte],                 -- Start Date
            @CRDTE        = [crdte],                 -- Create Date
            @JOB_STATES   = 0,
            @PROCESSING   = [v3d].[fn_getState]('processing'),
            @COMPLETED    = [v3d].[fn_getState]('completed'),
            @PENDING      = [v3d].[fn_getState]('pending'),
            @RAISE_EVENT  = 0
            FROM [v3d].[Jobs] WHERE [id] = @JID

      -- ========================================================
      -- If the JOB is not found then throw a NOT FOUND EXECPTION
      -- ========================================================
      IF @@ROWCOUNT = 0
       EXEC [master].[dbo].[sp_throwNotFoundException] 'Job', @JOBID



      -- Delete job 
      DELETE [v3d].[Jobs] WHERE [id] = @JID

      -- ============================================================================================
      -- If the DST (Destination UID) is not set then use the SRC (Source UID) as the associated
      -- series uid.
      -- =============================================================================================
      IF LEN(@DST) = 0 
        SET @UID = @SRC
      ELSE
        SET @UID = @DST

   
      -- ===========================
      -- Processing history entry
      -- ===========================
      INSERT [Auditing].[v3d].[ProcessingHistory] 
      ( [app], [mod], [msg], [name], [ort], [series], [state], [stdte], [study], [proc], [tid] ) VALUES
      (  ISNULL(@APP,''), @MOD,  '', @NAME, @ORT, @UID, @COMPLETED, ISNULL(@STDTE,sysdatetimeoffset()), @STUDY, @TASK, @TID )

      -- =========================================
      -- Get the state based on the series jobs
      -- ========================================
      SELECT @JOB_STATES = @JOB_STATES | [state] FROM [v3d].[Jobs] WHERE [src] = @UID AND [dst] = '' 

      IF @@ROWCOUNT = 0
         SELECT @JOB_STATES = @JOB_STATES | [state] FROM [v3d].[Jobs] WHERE [dst] = @UID 
    

      -- ==================================================
      -- Sets the series state and clear the message field 
      -- ==================================================
      UPDATE [v3d].[Series]
          SET [state] = @JOB_STATES | @COMPLETED,
              [msg] = ''
        WHERE [uid] = @UID

  
 
      -- 
      DECLARE @MODULE AS NVARCHAR(32)
      DECLARE @TASKS  AS XML
      DECLARE @NEWUID AS NVARCHAR(128)
      DECLARE @TYPE   AS INT
      DECLARE @TMP_TYPE AS NVARCHAR(32)
      DECLARE @SERIES AS XML


      -- Extract the MODULE and next INDEX from the TASK of the current JOB being updated
      SELECT  @MODULE = Task.value('(@name)[1]' , 'nvarchar(32)'),
              @INDEX = ( Task.value('(@idx)[1]' , 'int')  + 1 )
              FROM @TASK.nodes('task') AS N(Task)

       -- Insert the completed message 
      INSERT [v3d].[Messages] 
          ( [type], [app], [name], [src], [ort], [msg] ) VALUES
          ( 0,      @APP,  @NAME,  @SRC,  @ORT,  (@MODULE + ' was successful') ) 
 
        
      -- =========================================================================================
      -- If there is no DESTINATION UID then we are performing a conversion and the volume files
      -- are held under the dicom series. We need to iteratoe over all the volumes to create 
      -- a job for each. We do not have anything that will do this. Most datasets will 
      -- have only a single volume so only a single job will be created. Only cardiac datasets
      -- will create multiple volumes and there is no task needed for after the volume creation.
      -- =========================================================================================
      IF LEN(@DST) = 0
      BEGIN

        DECLARE @VOLUME_APP AS NVARCHAR(32)

        -- Cursor will return all volumes for the series (We use the source UID since that should be the DICOM)
        DECLARE VOLCURSOR CURSOR FOR SELECT [cid], [name], [app], [icnt] FROM [Storage].[v3d].[Volumes] WHERE [uid] = @SRC AND [cid] = @CID
        OPEN VOLCURSOR

        FETCH NEXT FROM VOLCURSOR INTO @CID, @VOLUME, @VOLUME_APP, @ICNT

        -- =================================================================
        -- Loop through all the volumes for the series and add a new job for
        -- the next task step.
        -- =================================================================
        WHILE @@FETCH_STATUS = 0
        BEGIN

          -- =================================================================================
          -- If the volume contains an application then override the current application
          -- that was in the JOB. This application would be the default set in the system 
          -- since the JOBS application id is set in sp_insertJob with the default if an app
          -- is not supplied.
          -- =================================================================================
          IF LEN(@VOLUME_APP) > 0 
            SET @APP = @VOLUME_APP

          -- ===================================
          -- Gets the template ID if prossible
          -- ===================================
          EXEC [System].[v3d].[sp_getTemplateId]    
            @TID = @TID OUTPUT,
            @APP = @APP,
            @PROC = 'sp_completeJob'


          -- =============================
          -- If a template was found 
          -- ===========================
          IF @TID IS NOT NULL AND LEN(@TID) > 0 
          BEGIN

            -- Get the task list from the templates table
            SELECT @TASKS = [tasks], @APP = [app] FROM [System].[v3d].[Templates] WHERE [id] = @TID 

            -- ================================================
            -- Raise exception that the template is not found
            -- ================================================
            IF @@ROWCOUNT = 0
               EXEC [master].[dbo].[sp_throwNotFoundException]  'Template', @TID

            -- Delete all tasks that do not match the next index determined above
            SET @TASKS.modify('delete task[@idx!=sql:variable(''@INDEX'')]')

            -- ========================================================================================================================
            -- Make sure there is a next task befor attempting to create a series and a new job. If there is no next task we are 
            -- at the end and nothing else needs to be done
            -- =======================================================================================================================
            IF @TASKS.exist('task[@name]') = 1
            BEGIN

 
              -- Extract the TYPE from the TASKS of the Template
              SELECT  @RAISE_EVENT  = 1,
                      @NEWUID       = [System].[v3d].[fn_createUid](),
                      @TMP_TYPE     = Task.value('(@type)[1]' , 'nvarchar(32)') FROM @TASKS.nodes('task') AS N(Task)

              SET @TYPE = [System].[v3d].[fn_getMaskBit]('type',@TMP_TYPE)
                      

              -- ===========================
              -- Insert the new series
              -- ===========================
              EXEC [v3d].[sp_insertSeriesInternal]
                @UID          = @NEWUID,
                @STUDY_UID    = @STUDY,
                @ORIENTATION  = @ORT,
                @MODALITY     = @MOD,
                @DESCRIPTION  = @DESC,
                @APP          = @APP,
                @COUNT        = @ICNT,
                @STATE        = @PENDING,
                @TYPE         = @TYPE,
                @PROC         = @TASKS

              -- =========================
              -- Add the series reference
              -- ==========================
              INSERT [v3d].[SeriesReferences] ( [uid], [ruid] ) VALUES (  @NEWUID, @SRC )

              -- =========================================
              -- Adds the Volume reference for the series
              -- =========================================
              INSERT [v3d].[VolumeReferences] ( [uid], [duid], [cid], [name] )  VALUES ( @NEWUID, @SRC, @CID, @VOLUME)

              -- ===========================================================================================
              -- Insert the next JOB (The source is the same as the previous which should be the DICOM )
              -- ===========================================================================================
              INSERT INTO [v3d].[Jobs]   
                            ( [login], [dst], [src], [vol], [cid], [study], [app], [state],  [name], [ort], [mod], [desc], [task], [icnt], [tid]) 
                      VALUES ( @LOGIN,  @NEWUID,  @SRC,  ISNULL(@VOLUME, ''), @CID,  @STUDY,  @APP,  @PENDING, @NAME,  @ORT,  @MOD,  @DESC,  @TASKS, @ICNT, ISNULL(@TID,'') ) 


            END   -- END ... If there is a next TASK


          END -- END ... If a template wsas found
 
          -- Get next volume
          FETCH NEXT FROM VOLCURSOR INTO @CID, @VOLUME, @APP, @ICNT

        END   -- END ... While there are volumes

        CLOSE VOLCURSOR
        DEALLOCATE VOLCURSOR



      END -- 

      -- ==============================
      -- A single next job is created 
      -- ==============================
      ELSE
      BEGIN

        -- ===================================
        -- Gets the template ID if prossible
        -- ===================================
        EXEC [System].[v3d].[sp_getTemplateId]    
          @TID = @TID OUTPUT,
          @APP = @APP,
          @PROC = 'sp_compeletdJob'


        -- Get the task list from the templates table
        SELECT @TASKS = [tasks], @APP = [app] FROM [System].[v3d].[Templates] WHERE [id] = @TID 

        -- ================================================
        -- Raise exception that the template is not found
        -- ================================================
        IF @@ROWCOUNT = 0
           EXEC [master].[dbo].[sp_throwNotFoundException]  'Template', @TID

        -- Delete all tasks that do not match the next index determined above
        SET @TASKS.modify('delete task[@idx!=sql:variable(''@INDEX'')]')


        -- ========================================================================================================================
        -- Make sure there is a next task befor attempting to create a series and a new job. If there is no next task we are 
        -- at the end and nothing else needs to be done
        -- =======================================================================================================================
        IF @TASKS.exist('task[@name]') = 1
        BEGIN

          
          -- Extract the TYPE from the TASKS of the Template
          SELECT  @RAISE_EVENT  = 1,
                  @NEWUID = [System].[v3d].[fn_createUid](),
                  @TMP_TYPE     = Task.value('(@type)[1]' , 'nvarchar(32)') FROM @TASKS.nodes('task') AS N(Task)
          
          SET @TYPE = [System].[v3d].[fn_getMaskBit]('type',@TMP_TYPE)

          -- ===========================
          -- Insert the new series
          -- ===========================
          EXEC [v3d].[sp_insertSeriesInternal]
            @UID          = @NEWUID,
            @STUDY_UID    = @STUDY,
            @ORIENTATION  = @ORT,
            @MODALITY     = @MOD,
            @DESCRIPTION  = @DESC,
            @APP          = @APP,
            @COUNT        = @ICNT,
            @STATE        = @PENDING,
            @TYPE         = @TYPE,
            @PROC         = @TASKS

          -- ==========================
          -- Add the series references
          -- ==========================
          INSERT [v3d].[SeriesReferences] ( [uid], [ruid] ) VALUES (  @NEWUID, @DST)
          INSERT [v3d].[SeriesReferences] ( [uid], [ruid] ) 
              SELECT  @NEWUID, [ruid] FROM [v3d].[SeriesReferences] WHERE [uid] = @DST

          -- ============================
          -- Add the volume references
          -- ============================
          INSERT [v3d].[VolumeReferences] (  [uid], [duid], [cid], [name] ) 
              SELECT @NEWUID, [duid], [cid], [name]  FROM [v3d].[VolumeReferences] WHERE [uid] = @DST

          -- Insert the JOB 
          INSERT INTO [v3d].[Jobs]   
                          ( [login], [dst],    [src], [vol], [cid], [study], [app], [state],  [name], [ort], [mod], [desc], [task], [icnt], [tid]) 
                  VALUES ( @LOGIN,  @NEWUID,  @DST,  ISNULL(@VOLUME,''), @CID, @STUDY,  @APP,  @PENDING, @NAME,  @ORT,  @MOD,  @DESC,  @TASKS, @ICNT, ISNULL(@TID,'') ) 

        END -- END ... If there are more tasks

      END -- END ... Else there is a destination uid so we must processing a non-conversion step/task
  
  

    
    -- =======================================================
    -- Raise event to tell the work queue we have a new job
    -- =======================================================
    IF @RAISE_EVENT = 1
      EXEC [v3d].[sp_raisePendingJobsEvent]


    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN 1 
        
    
  END TRY
  BEGIN CATCH
  
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_completedJob'
  

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_completeJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_completeJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_completeSession]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 06/10/2013
-- Description:	Closes the sesion
-- =============================================
CREATE PROCEDURE [v3d].[sp_completeSession]
@ID   NVARCHAR(256),
@UID  NVARCHAR(128)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC				    AS INT
  DECLARE @SESSION      AS INT
  

  SELECT  @RC         = 0,
          @SESSION    = [System].[v3d].[fn_getMaskBit]('type','session')
 
 

  BEGIN TRY

    SET XACT_ABORT ON

    BEGIN TRANSACTION
    
    -- ====================================
    -- Make sure the user can add a study
    -- ====================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    

    -- ==================================================================
    -- If the UID was a series we lock the series and all it's referneces
    -- ==================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Series] WHERE [uid] = @UID AND [type] = @SESSION )
    BEGIN

      -- Sets the completed flag in the state
      UPDATE [v3d].[Series] SET [state] = [System].[v3d].[fn_setMaskBit]([state],'state','complete') WHERE [uid] = @UID
      
    END
    ELSE
       EXEC [master].[dbo].[sp_throwNotFoundException]  'Session' 


 
    COMMIT TRANSACTION

    RETURN @RC
   
  END TRY
  BEGIN CATCH
   
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION
      
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_completeSession'
  
  END CATCH

END




GO
GRANT EXECUTE ON [v3d].[sp_completeSession] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_completeSession] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_createDirectory]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_createDirectory]
	@DIRECTORY [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_createDirectory]
GO
/****** Object:  StoredProcedure [v3d].[sp_createNamedEvent]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_createNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_createNamedEvent]
GO
/****** Object:  StoredProcedure [v3d].[sp_createReferences]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/20/2013
-- Description:	Creates the series references for the specified series and reference
--              by checking the referenced series own references and dependents.
-- ======================================================================================
CREATE PROCEDURE [v3d].[sp_createReferences]
@DICOM NVARCHAR(128),
@SESSION NVARCHAR(128),
@CID AS NVARCHAR(32)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  BEGIN TRY


    DECLARE @TYPE AS INT
    DECLARE @SESSION_TYPE AS INT
    DECLARE @UID AS NVARCHAR(128)

    DECLARE @TYPE_TABLE AS TABLE ( [type] INT )

    SELECT @SESSION_TYPE = [System].[v3d].[fn_getMaskBit]('type','session')

    DECLARE REFERENCE_CURSOR CURSOR LOCAL FOR SELECT [uid], [type]
             FROM [v3d].[VolumeReferencedSeries] 
            WHERE [duid] = @DICOM AND [cid] = @CID AND [type] <> @SESSION_TYPE
            ORDER BY [date] DESC


    OPEN REFERENCE_CURSOR 

    FETCH NEXT FROM REFERENCE_CURSOR INTO @UID, @TYPE

    WHILE @@FETCH_STATUS = 0
    BEGIN

  
      IF NOT EXISTS ( SELECT * FROM @TYPE_TABLE WHERE [type] = @TYPE )
      BEGIN

        IF NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] WHERE [uid] = @SESSION AND [ruid] = @UID )
        BEGIN
          INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] ) VALUES ( @SESSION, @UID )  

          INSERT INTO @TYPE_TABLE ( [type] ) VALUES ( @TYPE )
        END
      END

      FETCH NEXT FROM REFERENCE_CURSOR INTO @UID, @TYPE

    END



    CLOSE REFERENCE_CURSOR

    DEALLOCATE REFERENCE_CURSOR

   
 
    -- Add volume refernces

 
    IF NOT EXISTS ( SELECT * FROM [v3d].[VolumeReferences] WHERE [uid] = @SESSION AND [cid] = @CID AND [duid] = @DICOM )
      INSERT INTO [v3d].[VolumeReferences] ( [uid], [cid], [name], [duid] ) 
        ( SELECT @SESSION, [cid], [name], [uid] 
            FROM [v3d].[Volumes] 
           WHERE [uid] = @DICOM AND [cid] = @CID  )

 
    IF NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] WHERE [uid] = @SESSION AND [ruid] = @DICOM )
      INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] ) VALUES ( @SESSION, @DICOM )
      

    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_createReferences'

  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_createReferences] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_createReferences] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_createSeriesReferences]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ======================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/20/2013
-- Description:	Creates the series references for the specified series and reference
--              by checking the referenced series own references and dependents.
-- ======================================================================================
CREATE PROCEDURE [v3d].[sp_createSeriesReferences]
@UID NVARCHAR(128),
@RUID NVARCHAR(128)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  BEGIN TRY

    DECLARE @SESSION AS INT


    SELECT @SESSION = [System].[v3d].[fn_getMaskBit]('type','session')

 
    -- Dependents
    INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] ) 
      SELECT @UID, [duid] FROM [v3d].[DependentSeries] WHERE [uid] = @RUID AND [type] <> @SESSION AND
        NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] 
                             WHERE [v3d].[SeriesReferences].[uid] = @UID AND 
                                   [v3d].[SeriesReferences].[ruid] = [v3d].[DependentSeries].[duid] )

     DECLARE @CID AS NVARCHAR(64)
     DECLARE @NAME AS NVARCHAR(64)
     DECLARE @DUID AS NVARCHAR(128)

     -- Since the refrence UID is a non-session it will only ever have a single volume refrence
     SELECT @CID = [cid], @NAME = [name], @DUID = [duid] FROM [v3d].[VolumeReferences] WHERE [uid] = @RUID


     IF NOT EXISTS ( SELECT * FROM [v3d].[VolumeReferences] WHERE [uid] = @UID AND [duid] = @DUID AND [cid] = @CID AND [name] = @NAME )
       INSERT INTO [v3d].[VolumeReferences] ( [uid], [duid], [cid], [name] ) VALUES ( @UID, @DUID, @CID, @NAME )

 
      -- References
    INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] ) 
      SELECT @UID, [ruid] FROM [v3d].[ReferenceSeries] WHERE [uid] = @RUID AND [type] <> @SESSION  AND
        NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] 
                             WHERE [v3d].[SeriesReferences].[uid] = @UID AND [v3d].[SeriesReferences].[ruid] = [v3d].[ReferenceSeries].[ruid] )

                               
 
    IF NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] WHERE [uid] = @UID AND [ruid] = @RUID )
      INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] ) VALUES ( @UID, @RUID )
      

    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_createSeriesReferences'

  END CATCH

END



GO
/****** Object:  StoredProcedure [v3d].[sp_createSession]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Creates a session series
-- =============================================
CREATE PROCEDURE [v3d].[sp_createSession]
@ID AS NVARCHAR(256),
@UIDS AS NVARCHAR(MAX),
@APP AS NVARCHAR(16),                   -- Application
@SESSION_UID AS NVARCHAR(128) OUTPUT,   -- Newly created session uid
@DESCRIPTION AS NVARCHAR(512) = NULL,   -- Description
@SERIES_TYPES AS INT = 0,               -- Series Types (Leaf Nodes from launch rules)
@SOURCE AS NVARCHAR(32) = '',           -- Source of session (LOgin)
@MAXIMUM_DATASETS AS INT = 1            -- Maximum datasets
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON

  DECLARE @TRANSCOUNT AS INT
  DECLARE @OPEN AS INT

  SELECT @TRANSCOUNT = @@TRANCOUNT
	

  BEGIN TRY

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION


 	  -- Modalty
	  DECLARE @MODALITY AS NVARCHAR(16)
	
    DECLARE @SUID AS NVARCHAR(128)
    DECLARE @UID AS NVARCHAR(128)
    DECLARE @RC AS INT
    DECLARE @SERIES_DATE AS DATETIMEOFFSET(7)
    DECLARE @PROCESSING AS BIT
    DECLARE @DATASET_COUNT AS INT
    DECLARE @COMPLETED AS INT
    DECLARE @PROCESSING_COUNT AS INT
    DECLARE @SESSION AS INT
    DECLARE @DICOM AS INT
  
  
    -- Get the current date/time
    SELECT  @SERIES_DATE      = SYSDATETIMEOFFSET(),
            @COMPLETED        = [v3d].[fn_getState]('completed'),
            @MODALITY         = [System].[v3d].[fn_getSetting]('SessionModality',''),
            @RC               = 0,
            @OPEN             = 0,
            @SESSION_UID      = '',
            @DATASET_COUNT    = 0,
            @SESSION          = [System].[v3d].[fn_getMaskBit]('type','session'),
            @DICOM            = [System].[v3d].[fn_getMaskBit]('type','dicom'),
            @SOURCE           = ISNULL(@SOURCE,''),
            @SERIES_TYPES     = ISNULL(@SERIES_TYPES,0),
            @DESCRIPTION      = ISNULL(@DESCRIPTION, @APP + ' Session - ' + CONVERT(NVARCHAR(32),CONVERT(DATETIME,@SERIES_DATE))),
            @PROCESSING       = [v3d].[fn_getState]('processing'),
            @PROCESSING_COUNT = 0;



    -- ======================
    -- Builds the UID Table
    -- =======================
    DECLARE @UID_TABLE AS TABLE (	[uid] [nvarchar](128) NOT NULL )

    INSERT INTO @UID_TABLE ( [uid] ) ( SELECT  SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UIDS) - 1
              AND SUBSTRING(@UIDS, Number, 1) = '|' ) 

  
    DECLARE UIDS_CURSOR CURSOR LOCAL
        FOR SELECT [uid] FROM @UID_TABLE


    OPEN UIDS_CURSOR

    SET @OPEN = 1

    -- fetch the first UID
    FETCH NEXT FROM UIDS_CURSOR INTO @UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0 AND ( @DATASET_COUNT < @MAXIMUM_DATASETS )
    BEGIN

    -- ========================================================================
      -- If no session uid is generated the create the session. This is done in
      -- the loop because I need information from the UID 
      -- ========================================================================
      IF LEN(@SESSION_UID) = 0 
      BEGIN
    
        --- Generate a new UID	
	      SET @SESSION_UID = [System].[v3d].[fn_createUid]()

        IF LEN(@MODALITY) = 0
          SELECT @SUID = [suid], @MODALITY = [mod] FROM [v3d].[Series] WHERE [uid] = @UID
        ELSE
          SELECT @SUID = [suid] FROM [v3d].[Series] WHERE [uid] = @UID

        -- ==========================
        -- Insert the session series
        -- ==========================
        EXEC @RC = [v3d].[sp_insertSeriesInternal]
              @UID          = @SESSION_UID,
              @STUDY_UID    = @SUID,
              @APP          = @APP,
              @MODALITY     = @MODALITY,
              @SOURCE       = @SOURCE,
              @DESCRIPTION  = @DESCRIPTION,
              @TYPE         = @SESSION

      END
    
      -- Series type of the series from list
      DECLARE @SERIES_TYPE AS INT
    
      -- Get the current series type
      SELECT @SERIES_TYPE = [type] FROM [v3d].[Series] WHERE [uid] = @UID
    
    
      -- ==============================================================================================================
      -- If the series is a DICOM then we need to get the last processed series from the dependents 
      -- and take that series and look up all it references. The dependent series as well as it's references
      -- will be added as references to the session.
      -- ================================================================================================================
      IF @SERIES_TYPE = @DICOM
      BEGIN
    
        -- Dependent UID
        DECLARE @DUID AS NVARCHAR(128)
      
        -- Default
        SET @DUID = ''
      
        -- ===================================================================================
        -- Get the last leaf dependent series for the dicom series that is being processed
        -- ===================================================================================
        SELECT TOP(1) @DUID = [uid]
            FROM [v3d].[DependentSeries] 
            WHERE [duid] = @UID 
             AND ( [type] & @SERIES_TYPES ) = [type]
             AND [v3d].[fn_isState]([state],'completed') = 1  
            ORDER BY [date] DESC
 
     
        -- ============================================================
        -- Make sure we found a valid leaf series for this application
        -- ============================================================
        IF @@ROWCOUNT = 1
        BEGIN 
      
          -- ==============================================================================
          -- Insert the dependent series for the DICOM series except any Session series
          -- ==============================================================================
          INSERT INTO [Storage].[v3d].[SeriesReferences]  ( [uid], [ruid] ) 
                SELECT @SESSION_UID, [Storage].[v3d].[SeriesReferences].[uid] FROM [Storage].[v3d].[SeriesReferences] 
                INNER JOIN [Storage].[v3d].[Series] 
                ON [Storage].[v3d].[SeriesReferences].[uid] = [Storage].[v3d].[Series].[uid]
                WHERE [Storage].[v3d].[SeriesReferences].[ruid] = @UID 
                AND [Storage].[v3d].[Series].[type] <> @SESSION
 
          -- =========================
          -- Insert the new refernce
          -- ========================
          INSERT INTO [Storage].[v3d].[SeriesReferences] ([uid], [ruid]) VALUES ( @SESSION_UID, @UID )

           --- Increment the dataset count
          SET @DATASET_COUNT = @DATASET_COUNT + 1
        

        
       END  -- END ... Dependent leafs series were found
     
       -- ===================================================================================
       -- No dependent leaf series found for this series so check if there is any processing
       -- dependent series.
       -- ===================================================================================       
       ELSE
       BEGIN
     
        -- ====================================================================================
        -- Get the last leaf dependent series for the dicom series that is being processed
        -- ===================================================================================
        SELECT @PROCESSING_COUNT = COUNT(*)
            FROM [v3d].[DependentSeries] 
             WHERE [duid] = @UID 
              AND [v3d].[fn_isState]([state],'processing') = 1
           

 
    
       END  -- END ... If a ending processed series was found for this series
    
      END -- END ... If we have a dicom series
    
      -- ==========================================================================================
      -- Else we have a non-dicom series uid so we assume that we will use that series and all
      -- of its references.
      -- ========================================================================================
      ELSE
      BEGIN

          -- ==============================================================
          -- Insert the dependent series from the DICOM series references
          -- ==============================================================
          INSERT INTO [Storage].[v3d].[SeriesReferences]  ( [uid], [ruid] ) 
                SELECT @SESSION_UID, [Storage].[v3d].[SeriesReferences].[ruid] 
                  FROM [Storage].[v3d].[SeriesReferences] 
                 WHERE [Storage].[v3d].[SeriesReferences].[uid] = @UID 
 
          -- ========================
          -- Insert the new refernce
          -- ========================
          INSERT INTO [Storage].[v3d].[SeriesReferences] ([uid], [ruid]) VALUES ( @SESSION_UID, @UID )
             
        --- Increment the dataset count
        SET @DATASET_COUNT = @DATASET_COUNT + 1
        

      END -- END ... Else we have a non-dicom series

       -- fetch the first UID
      FETCH NEXT FROM UIDS_CURSOR INTO @UID

    END

   
    -- ===============================================================
    -- If no valid datasets were found for launching then we
    -- will delete the session and raise a no data found error.
    -- ==============================================================
    IF @DATASET_COUNT = 0 
    BEGIN
    
      -- =====================================
      -- Delete the session that was inserted
      -- ======================================
      DELETE [v3d].[Series] WHERE [uid] = @SESSION_UID
    
      -- =================================================================
      -- If the processing count is greater then 0 then we are processing
      --  901431  - Cannot launch while processing
      --  901430  - No valid launchable data found
      -- =================================================================
      IF @PROCESSING_COUNT > 0
        EXEC [master].[dbo].[sp_throwException] 901431
      ELSE
        EXEC [master].[dbo].[sp_throwException] 901430
      
    END -- END ... If no references were added

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN @RC
  END TRY     
  BEGIN CATCH

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_createSession'

  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_createSession] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_createSession] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_databaseCheck]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/15/2013
-- Description:	Database check
-- =============================================
CREATE PROCEDURE [v3d].[sp_databaseCheck]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  EXEC master.dbo.sp_foreachdb
    @command='
	  IF NOT(SELECT DATABASEPROPERTYEX(''?'',''Updateability''))=''READ_ONLY''
	  BEGIN
		  DBCC CHECKDB (?) 
	  END'
  END




GO
/****** Object:  StoredProcedure [v3d].[sp_deleteDicomServer]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/14/2011
-- Description: Cleans up the server entries
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteDicomServer]
(
@ID       NVARCHAR(256),         -- Context Id
@AETITLE  NVARCHAR(32) = NULL,   -- Servers AE TItle
@SERVER   NVARCHAR(32) = NULL,   -- Server name
@RECORD   XML = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @PROCEDURE AS NVARCHAR(128)


  SELECT @PROCEDURE = OBJECT_NAME(@@PROCID)

  BEGIN TRY
 
    
    -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'


    IF ( @AETITLE IS NULL AND @SERVER IS NULL ) AND @RECORD IS NOT NULL
    BEGIN
     DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @AETITLE          = [aetitle],
             @SERVER           = [server]
            FROM OPENXML(@IDOC, N'/user', 1) WITH 
            ( 
              [aetitle]            nvarchar(64),
              [server]             nvarchar(64)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END


     
    -- Make sure there are no NULL's
    SELECT @AETITLE = ISNULL(@AETITLE,''),
           @SERVER = ISNULL(@SERVER,'')

    IF LEN(@SERVER) = 0 AND LEN(@AETITLE) > 0
    BEGIN
      DELETE FROM [v3d].[DicomServers] WHERE [aetitle] = @AETITLE
    END
    ELSE IF LEN(@SERVER) > 0 AND LEN(@AETITLE) = 0 
    BEGIN
      DELETE FROM [v3d].[DicomServers] WHERE [server] = @SERVER
    END
    ELSE IF LEN(@SERVER) > 0 AND LEN(@AETITLE) > 0
    BEGIN
      DELETE FROM [v3d].[DicomServers] WHERE [server] = @SERVER AND [aetitle] = @AETITLE
    END
    ELSE
    BEGIN
     DELETE FROM [v3d].[DicomServers]
    END
  
    RETURN @@ROWCOUNT

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE

  END CATCH

 
END





GO
GRANT EXECUTE ON [v3d].[sp_deleteDicomServer] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteDicomServer] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteDirectory]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_deleteDirectory]
	@uid [nvarchar](4000),
	@type [nvarchar](4000),
	@directory [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_deleteDirectory]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteDirectoryError]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/10/2013
-- Description:	Deletes the serialization error 
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteDirectoryError]
(
@ID     NVARCHAR(256),          -- Context ID
@RECORD XML = '<directoryerror />'
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @EID AS BIGINT
  DECLARE @IDOC AS INT
  DECLARE @TYPE AS NVARCHAR(32)
  DECLARE @UID AS NVARCHAR(128)
  DECLARE @PATH AS NVARCHAR(1024)

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'StorageManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @EID          = [id]
          FROM OPENXML(@IDOC, N'/directoryerror', 1) WITH 
          ( 
            [id]             bigint
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC


    IF @EID IS NULL
    BEGIN
      
 

      DECLARE DELETE_DIRECTORY_ERRORS CURSOR FOR  SELECT [id], [uid], [type], [path] FROM [v3d].[DirectoryErrors]

      OPEN DELETE_DIRECTORY_ERRORS
      
      
      -- fetch the first record
      FETCH NEXT FROM DELETE_DIRECTORY_ERRORS INTO @EID, @UID, @TYPE,  @PATH

      -- loop while there are records in the cursor
      WHILE @@FETCH_STATUS = 0
      BEGIN

        DELETE [v3d].[DirectoryErrors]  WHERE [id] = @EID

        -- Delete the directory
        EXEC [v3d].[sp_deleteDirectory] 
          @uid = @UID,
          @type = @TYPE,
          @directory = @PATH

        -- fetch the next record
        FETCH NEXT FROM DELETE_DIRECTORY_ERRORS INTO @EID, @UID, @TYPE,  @PATH

      END
  
      -- Close the CURSOR
      CLOSE DELETE_DIRECTORY_ERRORS
       
      -- Deallocate the resources
      DEALLOCATE DELETE_DIRECTORY_ERRORS

    END
    ELSE
    BEGIN

 
      SELECT  @UID = [uid], @PATH = [path] FROM [v3d].[DirectoryErrors] WHERE [id] = @EID

      IF @@ROWCOUNT = 1
      BEGIN
        

        DELETE [v3d].[DirectoryErrors]  WHERE [id] = @ID

        EXEC [v3d].[sp_deleteDirectoryInternal] 
          @UID = @UID,
          @PATH = @PATH



      END 
    
    END

  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_deleteSerializeError'

  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteDirectoryError] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteDirectoryError] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteFile]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_deleteFile]
	@file [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_deleteFile]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteFiles]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_deleteFiles]
	@path [nvarchar](4000),
	@pattern [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_deleteFiles]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the pacs jobs from the database
-- ====================================================
CREATE PROCEDURE [v3d].[sp_deleteJob]
(
@ID     NVARCHAR(256),          -- Context ID
@RECORD XML
)
AS
BEGIN



  DECLARE @RC AS INT
  DECLARE @TRANSCOUNT AS INT

  SELECT @TRANSCOUNT = @@TRANCOUNT

  RETRY:

  BEGIN TRY
    
     SET XACT_ABORT ON

     IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'ProcessingManagement'
          
    DECLARE @IDOC AS INT

    DECLARE @JID            AS NVARCHAR(MAX)
    DECLARE @UID            AS NVARCHAR(128)
    DECLARE @PROCEDURE      AS NVARCHAR(128)
    DECLARE @FROM_CDATE     AS DATETIMEOFFSET(7)
    DECLARE @TO_CDATE       AS DATETIMEOFFSET(7)
    DECLARE @PROCESSING     AS INT


    SELECT @PROCEDURE  = OBJECT_NAME(@@PROCID),
           @PROCESSING = [v3d].[fn_getState]('processing');


    DECLARE @SERIES AS XML

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @JID          = [id],
            @UID          = [src],
            @FROM_CDATE   = [fromCDate],
            @TO_CDATE     = [toCDate]
          FROM OPENXML(@IDOC, N'/job', 1) WITH 
          ( 
            [id]             nvarchar(MAX),
            [src]            nvarchar(128),
            [fromCDate]      datetimeoffset(7) 'cdate/@from',
            [toCDate]        datetimeoffset(7) 'cdate/@to'
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

 
    IF @JID IS NOT NULL
    BEGIN
       DELETE [v3d].[Jobs] WHERE [id] = @JID
 
        SET @RC = @@ROWCOUNT    
    END
    ELSE
    BEGIN


      DECLARE @WHERE AS NVARCHAR(MAX)

      SET @WHERE = ''

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison('@UID', @UID, N'[src]')
 
      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_CDATE, N'@FROM_CDATE', @TO_CDATE, N'@TO_CDATE', N'[cdate]')

      DECLARE @LEN AS INT

      SET @LEN = LEN(@WHERE)
    
      IF @LEN > 0
        SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
 
      DECLARE @DELETE AS NVARCHAR(MAX)   

      SET @DELETE = 'DELETE [v3d].[Jobs]  ' + @WHERE + '; SET @COUNT = @@ROWCOUNT'
   
      DECLARE @PARAMETERS AS NVARCHAR(4000)    
    
      SET @PARAMETERS = N'@FROM_CDATE   DATETIMEOFFSET(7), ' +
                        N'@TO_CDATE     DATETIMEOFFSET(7), ' +
                        N'@JID          NVARCHAR(256), ' +
                        N'@UID          NVARCHAR(128), ' +
                        N'@COUNT        INT OUTPUT '

      EXEC sp_executesql 
            @DELETE,
            @PARAMETERS,
            @TO_CDATE         = @TO_CDATE,
            @FROM_CDATE       = @FROM_CDATE,
            @JID              = @JID,
            @UID              = @UID,
            @COUNT            = @RC OUTPUT

      IF @RC > 0 
      BEGIN
        SET @DELETE = 'DELETE [v3d].[Jobs] ' + @WHERE + '; SET @COUNT = @@ROWCOUNT'
   
        EXEC sp_executesql 
              @DELETE,
              @PARAMETERS,
              @TO_CDATE         = @TO_CDATE,
              @FROM_CDATE       = @FROM_CDATE,
              @JID              = @JID,
              @UID              = @UID,
              @COUNT            = @RC OUTPUT
 
      END
        
      
    END
     
   IF @TRANSCOUNT = 0
    COMMIT TRANSACTION

   RETURN @RC
   
  END TRY
  BEGIN CATCH
   
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION
      
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE,
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
    

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deleteJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteLock]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================================
-- Author:		  Mark Kontak
-- Create date: 04/16/2012
-- Description:	Deletes the lock. Since this procedure is only called internally the 
--              ID is not required.
-- ===================================================================================
CREATE PROCEDURE [v3d].[sp_deleteLock]
(
@ID         NVARCHAR(256),           -- ID is not needed
@TRANSID    NVARCHAR(MAX) = NULL,
@RECORD     XML = NULL
)
AS
BEGIN 

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC				    AS INT
  DECLARE @OPEN         AS BIT
  DECLARE @TRANSCOUNT   AS INT
    

  RETRY:

  SELECT  @RC = 0,
          @OPEN = 0,
          @TRANSCOUNT = @@TRANCOUNT
         
  BEGIN TRY

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'StorageManagement'
 
    -- ===========================================================
    -- If the UID is not specified and the RECORD was then 
    -- the extract it from the RECORD
    -- ============================================================
    IF  @RECORD IS NOT NULL
    BEGIN


      DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @TRANSID          = [tid]
            FROM OPENXML(@IDOC, N'/lock', 1) WITH 
            ( 
              [tid]            nvarchar(MAX)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END

    -- ========================================================================
    -- Make sure that the uid was specified (900990) Uid is missing or invalid
    -- =========================================================================
    IF @TRANSID IS NULL OR LEN(@TRANSID) = 0
  	  EXEC [master].[dbo].[sp_throwInvalidDataException]  'Transaction Id'
 
       
    IF RIGHT(@TRANSID,1) <> '|'
      SET @TRANSID =  @TRANSID + '|'

    IF LEFT(@TRANSID,1) <> '|'
      SET @TRANSID = '|' + @TRANSID 


    DECLARE @TABLE AS TABLE (	[tid] [nvarchar](256) NOT NULL )

    INSERT INTO @TABLE( [tid] ) ( SELECT  SUBSTRING(@TRANSID, Number + 1, CHARINDEX('|',  @TRANSID, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@TRANSID) - 1
              AND SUBSTRING(@TRANSID, Number, 1) = '|' ) 


    DECLARE @DELETE_TRANSID AS NVARCHAR(256)
    DECLARE DELETE_LOCK_CURSOR CURSOR FOR  SELECT [tid] FROM @TABLE

    OPEN DELETE_LOCK_CURSOR
      
      
    -- Cursor is open
    SET @OPEN = 1
      
    -- fetch the first row
    FETCH NEXT FROM DELETE_LOCK_CURSOR INTO @DELETE_TRANSID

    -- ======================================================
    -- loop while there are transaction id's to be deleted
    -- =======================================================
    WHILE @@FETCH_STATUS = 0
    BEGIN

      -- =======================================================
      -- Delete all locks associated with the transaction id
      -- =======================================================
      DELETE [v3d].[Locks] WHERE [tid] = @DELETE_TRANSID

      SET @RC = @RC + @@ROWCOUNT

      -- fetch the next row
      FETCH NEXT FROM DELETE_LOCK_CURSOR INTO @DELETE_TRANSID

    END


    -- Close the CURSOR
    CLOSE DELETE_LOCK_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE DELETE_LOCK_CURSOR

    SET @OPEN = 0

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN @RC
   
  END TRY
  BEGIN CATCH
   
      

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION

    -- ====================================================================
    -- If the cursor is still open then close and deallocte the resources
    -- ====================================================================
    IF @OPEN = 1
    BEGIN
      -- Close the CURSOR
      CLOSE INSERT_LOCK_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE INSERT_LOCK_CURSOR
    END

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteLock',
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
  
  END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_deleteLock] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteLock] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteMessage]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Delets the messages
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteMessage]
(
@ID     NVARCHAR(256),          -- Context ID
@RECORD XML
)
AS
BEGIN

  

  
  DECLARE @RC AS INT
  DECLARE @PROCEDURE AS NVARCHAR(128)

  SELECT @PROCEDURE = OBJECT_NAME(@@PROCID)

  BEGIN TRY
    
    SET XACT_ABORT ON

    BEGIN TRANSACTION

 
     
    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'

          
    DECLARE @IDOC AS INT

    DECLARE @MID AS INT
    DECLARE @TYPE AS SMALLINT
    DECLARE @SOURCE NVARCHAR(128)
    DECLARE @FROM_DATE        AS DATETIMEOFFSET(7)
    DECLARE @TO_DATE          AS DATETIMEOFFSET(7)


    DECLARE @SERIES AS XML

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @MID        = [id],
            @TYPE       = [type],
            @SOURCE     = [src],
            @FROM_DATE  = [fromDate],
            @TO_DATE    = [toDate]
          FROM OPENXML(@IDOC, N'/message', 1) WITH 
          ( 
            [id]             int,
            [type]           smallint,
            [src]            nvarchar(128),
            [host]           nvarchar(32),
            [fromDate]       datetimeoffset(7) 'date/@from',
            [toDate]         datetimeoffset(7) 'date/@to'
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

 
    IF @MID IS NOT NULL
      DELETE [v3d].[Messages] WHERE [id] = @MID
    ELSE
    BEGIN

      DECLARE @WHERE AS NVARCHAR(MAX)

      SET @WHERE = ''

      IF @TYPE IS NOT NULL
        SET @WHERE = ' [type] = @TYPE AND '

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison('@SOURCE', @SOURCE, '[src]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_DATE, N'@FROM_DATE', @TO_DATE, N'@TO_DATE', N'[date]')

      DECLARE @LEN AS INT

      SET @LEN = LEN(@WHERE)
    
      IF @LEN > 0
        SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
 
      DECLARE @DELETE AS NVARCHAR(MAX)   

      SET @DELETE = 'DELETE [v3d].[Messages] ' + @WHERE + '; SET @COUNT = @@ROWCOUNT'
   
      DECLARE @PARAMETERS AS NVARCHAR(4000)    
    
      SET @PARAMETERS = N'@FROM_DATE    DATETIMEOFFSET(7), ' +
                        N'@TO_DATE      DATETIMEOFFSET(7), ' +
                        N'@MID          INT, ' +
                        N'@SOURCE       NVARCHAR(128), ' + 
                        N'@TYPE         SMALLINT, '  +
                        N'@COUNT        INT OUTPUT '

      EXEC sp_executesql 
            @DELETE,
            @PARAMETERS,
            @TO_DATE          = @TO_DATE,
            @FROM_DATE        = @FROM_DATE,
            @MID              = @MID,
            @TYPE             = @TYPE,
            @SOURCE           = @SOURCE,
            @COUNT            = @RC OUTPUT
      
    END
     
   IF @@TRANCOUNT > 0 AND XACT_STATE() = 1
    COMMIT TRANSACTION

   RETURN @RC
   
  END TRY
  BEGIN CATCH
   
    IF @@TRANCOUNT > 0 AND XACT_STATE() = -1
      ROLLBACK TRANSACTION

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE

   
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deleteMessage] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteMessage] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deletePacsJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the pacs jobs from the database
-- ====================================================
CREATE PROCEDURE [v3d].[sp_deletePacsJob]
(
@ID     NVARCHAR(256),          -- Context ID
@JID    INT = NULL,
@RECORD XML
)
AS
BEGIN

  

  BEGIN TRANSACTION

 
  DECLARE @WRAP AS BIT
  DECLARE @RC AS INT

  SET @WRAP = 1

  BEGIN TRY

     
    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

          
    DECLARE @IDOC AS INT

    DECLARE @HOST AS NVARCHAR(1000)
    DECLARE @OP AS SMALLINT
    DECLARE @STATE AS SMALLINT
    DECLARE @FROM_CDATE        AS DATETIMEOFFSET(7)
    DECLARE @TO_CDATE          AS DATETIMEOFFSET(7)


    DECLARE @SERIES AS XML

    
    IF @JID IS NULL
    BEGIN

      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT  @JID        = [id],
              @OP         = [op],
              @STATE      = [state],
              @HOST       = [host],
              @FROM_CDATE  = [fromCDate],
              @TO_CDATE    = [toCDate]
            FROM OPENXML(@IDOC, N'/pacsjob', 1) WITH 
            ( 
              [id]             int,
              [op]             smallint,
              [state]          smallint,
              [host]           nvarchar(1000),
              [fromCDate]       datetimeoffset(7) 'cdate/@from',
              [toCDate]         datetimeoffset(7) 'cdate/@to'
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC

    END


    IF @JID IS NOT NULL
      DELETE [v3d].[PacsJobs] WHERE [id] = @JID
    ELSE
    BEGIN

      DECLARE @WHERE AS NVARCHAR(MAX)

      SET @WHERE = ''

      IF @STATE IS NOT NULL
        SET @WHERE = ' [state] = @STATE AND '

      IF @OP IS NOT NULL
         SET @WHERE = ' [op] = @OP AND '

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison('@HOST', @HOST, '[host]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_CDATE, N'@FROM_CDATE', @TO_CDATE, N'@TO_CDATE', N'[date]')

      DECLARE @LEN AS INT

      SET @LEN = LEN(@WHERE)
    
      IF @LEN > 0
        SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
 
      DECLARE @DELETE AS NVARCHAR(MAX)   

      SET @DELETE = 'DELETE [v3d].[PacsJobs] ' + @WHERE + '; SET @COUNT = @@ROWCOUNT'
   
      DECLARE @PARAMETERS AS NVARCHAR(4000)    
    
      SET @PARAMETERS = N'@RESULTS      XML OUTPUT, ' +
                        N'@FROM_CDATE    DATETIMEOFFSET(7), ' +
                        N'@TO_CDATE      DATETIMEOFFSET(7), ' +
                        N'@JID          INT, ' +
                        N'@HOST         NVARCHAR(1000), ' + 
                        N'@STATE        SMALLINT, '  +
                        N'@OP           SMALLINT, ' +
                        N'@COUNT        INT OUTPUT '

      EXEC sp_executesql 
            @DELETE,
            @PARAMETERS,
            @TO_CDATE          = @TO_CDATE,
            @FROM_CDATE        = @FROM_CDATE,
            @JID              = @JID,
            @HOST             = @HOST,
            @STATE            = @STATE,
            @OP               = @OP,
            @COUNT            = @RC OUTPUT
      
    END
     
   
   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_deletePacsJob'
 
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deletePacsJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deletePacsJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteReceiver]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================
-- Author:		  Mark Kontak
-- Create date: 06/17/2011
-- Description:	Deletes all receivers for th aetitle specified
-- =============================================================
CREATE PROCEDURE [v3d].[sp_deleteReceiver]
(
@ID     NVARCHAR(256),              -- Context ID
@SERVER NVARCHAR(32)  = '%',        -- Computer name
@AETITLE NVARCHAR(32) = '%'        -- AE Title
)
AS
BEGIN

  DECLARE @DEL AS NVARCHAR(1)
  DECLARE @RECEIVING AS INT
 
  SELECT @RECEIVING = [v3d].[fn_getState]('receiving')

  BEGIN TRY
 
    
    -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'
    
    DECLARE @UID AS NVARCHAR(256)
    DECLARE @CNT AS INT
    DECLARE @ASSOCIATION AS NVARCHAR(256)
    DECLARE @CALLED_AETITLE AS NVARCHAR(64)
    DECLARE @CALLING_AETITLE AS NVARCHAR(64)
    DECLARE @NAME AS NVARCHAR(128)


    DECLARE RECEIVER_CURSOR CURSOR FOR 
        SELECT [uid], [cnt], [aetitle], [calling], [name], [association]
           FROM [v3d].[Receivers] 
              WHERE [v3d].[Receivers].[aetitle] LIKE @AETITLE AND [v3d].[Receivers].[server] LIKE @SERVER

    OPEN RECEIVER_CURSOR
   
    FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME, @ASSOCIATION


    WHILE @@FETCH_STATUS = 0 
    BEGIN


        -- ===============================
        -- Updates the series image count 
        -- ===============================
        UPDATE [v3d].[Series] 
           SET [icnt] = [icnt] + @CNT,
               [state] = @RECEIVING
         WHERE [v3d].[Series].[uid] = @UID

        -- ==================================================
        -- Only perform the next steps if the SERIES exists
        -- ================================================
        IF @@ROWCOUNT = 1
        BEGIN
          EXEC [v3d].[sp_insertJob]
              @ID     = @ID,         -- Authentication token
              @UID    = @UID,        -- DICOM series UID
              @STATE  = @RECEIVING   -- Set intial state to receiving
  
 	       -- Insert history
	       INSERT INTO [Auditing].[v3d].[ReceivingHistory] ( [svr], [src], [rcvr], [uid], [icnt], [name] )
              VALUES ( @CALLED_AETITLE, @CALLING_AETITLE, @ASSOCIATION, @UID, @CNT, @NAME )

          -- ============================================
          -- Serialize the series
          -- ===========================================
          EXEC [v3d].[sp_serializeSeries]
                @UID = @UID

        END

        FETCH NEXT FROM RECEIVER_CURSOR INTO @UID, @CNT, @CALLED_AETITLE, @CALLING_AETITLE, @NAME

    END -- END ... while there are receiver records

    CLOSE RECEIVER_CURSOR
    DEALLOCATE RECEIVER_CURSOR

    --- Deletes all receiving records 
    DELETE FROM [v3d].[Receivers] WHERE [association] = @ASSOCIATION

    -- Number of series-aasociations deleted from the receivers table
    RETURN @@ROWCOUNT

  
  END TRY
  BEGIN CATCH
  
      EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_deleteReceiver'


  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deleteReceiver] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteReceiver] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteSerializeError]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/10/2013
-- Description:	Deletes the serialization error 
-- =============================================
CREATE PROCEDURE [v3d].[sp_deleteSerializeError]
(
@ID     NVARCHAR(256),          -- Context ID
@RECORD XML
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @EID AS BIGINT
  DECLARE @UID AS NVARCHAR(128)
  DECLARE @TYPE AS NVARCHAR(64)
  DECLARE @IDOC AS INT

  BEGIN TRY

    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized] 
      @ID = @ID,
      @MASK = 'StorageManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @EID          = [id]
          FROM OPENXML(@IDOC, N'/serializeerror', 1) WITH 
          ( 
            [id]             bigint
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC


    IF @EID IS NULL
    BEGIN

      DECLARE DELETE_SERIALIZE_ERRORS CURSOR FOR  SELECT [id], [uid], [type] FROM [v3d].[SerializeErrors]

      OPEN DELETE_SERIALIZE_ERRORS
      
      
      -- fetch the first record
      FETCH NEXT FROM DELETE_SERIALIZE_ERRORS INTO @EID, @UID, @TYPE

      -- loop while there are records in the cursor
      WHILE @@FETCH_STATUS = 0
      BEGIN

        DELETE [v3d].[SerializeErrors] WHERE [id] = @EID

        IF @TYPE = 'series'
          EXEC [v3d].[sp_serializeSeries]
            @UID= @UID
        ELSE
          EXEC [v3d].[sp_serializeStudy]
            @UID= @UID


 
        -- fetch the next record
        FETCH NEXT FROM DELETE_SERIALIZE_ERRORS INTO @EID, @UID, @TYPE

      END
  
      -- Close the CURSOR
      CLOSE DELETE_SERIALIZE_ERRORS
       
      -- Deallocate the resources
      DEALLOCATE DELETE_SERIALIZE_ERRORS

    END
    ELSE
    BEGIN

      SELECT @TYPE = [type], @UID = [uid] FROM [v3d].[SerializeErrors] WHERE [id] = @EID

      IF @@ROWCOUNT = 1
      BEGIN
        IF @TYPE = 'series'
          EXEC [v3d].[sp_serializeSeries]
            @UID= @UID
        ELSE
          EXEC [v3d].[sp_serializeStudy]
            @UID= @UID
      END 
    
    END

  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_deleteSerializeError'

  END CATCH

END



GO
GRANT EXECUTE ON [v3d].[sp_deleteSerializeError] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the series from the database
-- ==================================================
CREATE PROCEDURE [v3d].[sp_deleteSeries]
(
@ID         NVARCHAR(256),              -- Context ID
@UID        NVARCHAR(MAX) = NULL,       -- UID (list of series uids)
@RECORD     XML = NULL,                 -- SERIES Record (XML)
@TRANSID    UNIQUEIDENTIFIER = NULL,    -- Transction ID
@FORCE      AS BIT = 0,                 -- FORCES the delete by deleting any locks first
@DELETE_STUDY AS BIT = 0                -- Flag indicating that the call was from delete study
)
AS
BEGIN
  
  DECLARE @ERROR_MESSAGE NVARCHAR(4000);
  DECLARE @PROCEDURE NVARCHAR(128)
  DECLARE @SUID           AS NVARCHAR(128)
  DECLARE @OPEN           AS BIT
  DECLARE @RC             AS INT
  DECLARE @SERIES_UID     AS NVARCHAR(128)
  DECLARE @TRANSCOUNT     AS INT
  

  
  RETRY:

  SELECT  @TRANSCOUNT   = @@TRANCOUNT,
          @TRANSID      = ISNULL(@TRANSID,NEWID()),
          @OPEN         = 0,
          @RC           = 0,
          @PROCEDURE    = OBJECT_NAME(@@PROCID)


  BEGIN TRY

    SET XACT_ABORT ON

    IF @TRANSCOUNT = 0 
      BEGIN TRANSACTION 

    -- Make sure that the the user is authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

     -- ===========================================================
    -- If the UID is not specified and the RECORD was then 
    -- the extract it from the RECORD
    -- ============================================================
    IF @UID IS NULL AND @RECORD IS NOT NULL
    BEGIN


      DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @UID          = [uid]
            FROM OPENXML(@IDOC, N'/series', 1) WITH 
            ( 
              [uid]            nvarchar(MAX)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END

    -- ======================================================
    -- Raise exception that the UID is missing or invalid
    -- ======================================================
    IF @UID IS NULL OR LEN(@UID) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Series Uid'


 

    IF RIGHT(@UID,1) <> '|'
      SET @UID =  @UID + '|'

    IF LEFT(@UID,1) <> '|'
      SET @UID = '|' + @UID 


    -- ================================================================================
    -- Attempt to lock all the series here. If any series, reference or dependent is 
    -- locked then an exception will be raised now. Only do this if we were not called
    -- from the sp_deleteStudy which locks everything prior to the call.
    -- ================================================================================
    IF @DELETE_STUDY = 0
    BEGIN
      EXEC [v3d].[sp_lockSeries]
        @ID         = @ID,
        @UID        = @UID,
        @TRANSID    = @TRANSID,
        @DESC       = 'Series delete'
    END

   DECLARE @TABLE AS TABLE (	[uid] [nvarchar](128) NOT NULL )


   INSERT INTO @TABLE ( [uid] ) ( SELECT  SUBSTRING(@UID, Number + 1, CHARINDEX('|',  @UID, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UID) - 1
              AND SUBSTRING(@UID, Number, 1) = '|' ) 


    SET @RC = @@ROWCOUNT


    DECLARE DELETE_SERIES_CURSOR CURSOR LOCAL
        FOR SELECT T.[uid] FROM @TABLE AS T INNER JOIN [v3d].[Series] ON [v3d].[Series].[uid] = T.[uid] ORDER BY [v3d].[Series].[date] DESC

    OPEN DELETE_SERIES_CURSOR
      
      
    -- Cursor is open
    SET @OPEN = 1
      
    -- fetch the first record
    FETCH NEXT FROM DELETE_SERIES_CURSOR INTO @SERIES_UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN

      -- =======================
      -- Deletes a single series
      -- =======================
      EXEC [v3d].[sp_deleteSeriesInternal]
        @ID           = @ID,
        @UID          = @SERIES_UID,
        @TRANSID      = @TRANSID,
        @FORCE        = @FORCE,
        @DELETE_STUDY = @DELETE_STUDY

 
      -- fetch the next record
      FETCH NEXT FROM DELETE_SERIES_CURSOR INTO @SERIES_UID

    END
  
    -- Close the CURSOR
    CLOSE DELETE_SERIES_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE DELETE_SERIES_CURSOR
        
    -- Cursor is closed
    SET @OPEN = 0 


    -- ==========================================================================================
    -- If we are not called from the delet study then delete all locks associated with the 
    -- series delete. This is because there will be series refereces locked and these lockes 
    -- need to be cleared. 
    -- ==========================================================================================
    IF @DELETE_STUDY = 0 
    BEGIN
        
      -- ==========================================
      -- Delete all locks for this transaction
      -- =========================================
      EXEC [v3d].[sp_deleteLock]
        @ID = @ID,
        @TRANSID = @TRANSID

    END 

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION


    IF @DELETE_STUDY = 0 
    BEGIN

      DECLARE @PATH AS NVARCHAR(1024)
      DECLARE @PATH_UID AS NVARCHAR(128)
      DECLARE DELETE_PATHS_CURSOR CURSOR FOR  SELECT [path], [uid] FROM [v3d].[DeletePaths] WHERE [id] = @TRANSID

      OPEN DELETE_PATHS_CURSOR
      
      
      -- fetch the first record
      FETCH NEXT FROM DELETE_PATHS_CURSOR INTO @PATH, @PATH_UID

      -- loop while there are records in the cursor
      WHILE @@FETCH_STATUS = 0
      BEGIN
 

        -- ==============================
        -- Delete the study directory. 
        -- ==============================
        EXEC [v3d].[sp_deleteDirectory] 
          @uid = @PATH_UID,
          @type = 'Series',
          @directory = @PATH
       
   
       -- This is executed as long as the previous fetch succeeds.
        FETCH NEXT FROM DELETE_PATHS_CURSOR INTO @PATH, @PATH_UID

      END  -- END WHILE

      CLOSE DELETE_PATHS_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE DELETE_PATHS_CURSOR


      -- Delete Paths associated with the transaction
      DELETE [v3d].[DeletePaths] WHERE [id] = @TRANSID

    END
    
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    
   -- ====================================================================
    -- If the cursor is still open then close and deallocte the resources
    -- ====================================================================
    IF @OPEN = 1
    BEGIN
      -- Close the CURSOR
      CLOSE DELETE_SERIES_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE DELETE_SERIES_CURSOR
    END


     IF XACT_STATE() = -1
		  ROLLBACK TRANSACTION;

     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE,
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_deleteSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteSeriesDirectory]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_deleteSeriesDirectory]
	@seriesDirectory [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_deleteSeriesDirectory]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteSeriesInternal]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the individual series from the database
-- =============================================================
CREATE PROCEDURE [v3d].[sp_deleteSeriesInternal]
(
@ID           NVARCHAR(256),              -- Context ID
@UID          NVARCHAR(128),              -- UID
@TRANSID      UNIQUEIDENTIFIER = NULL,    -- Transaction ID
@FORCE        BIT = 0,                    -- FORCES the delete by deleting any locks first
@DELETE_STUDY BIT = 0                     -- Flag indicating that the call was from delete study
)
AS
BEGIN

  

  DECLARE @SUID           AS NVARCHAR(128)
  DECLARE @PROCESSING     AS INT
  DECLARE @OPEN           AS BIT
  DECLARE @RC             AS INT
  DECLARE @PROCEDURE      AS NVARCHAR(128)
  
  RETRY:

  SELECT  @OPEN         = 0,
          @RC           = 0,
          @PROCEDURE    = OBJECT_NAME(@@PROCID),
          @PROCESSING = [v3d].[fn_getState]('processing')


  BEGIN TRY
 

    -- ======================================================
    -- Raise exception that the UID is missing or invalid
    -- ======================================================
    IF @UID IS NULL OR LEN(@UID) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Series Uid' 

 
    -- ===================================
    -- Get the study UID from the series
    -- ===================================
    SELECT @SUID = [suid] FROM [v3d].[Series] WHERE [uid] = @UID

    -- ==================================================
    -- Make sure the series exists
    -- (900991) Series: Not Found
    -- ==================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException]  'Series' 


    -- ======================================================================================================
    -- If we are not forcing the series to be deleted and we are not being called from the sp_deleteStudy 
    -- check if the series contains references. 
    --
    -- (901405) Series contains references
    -- =====================================================================================================
    IF @FORCE = 0 AND @DELETE_STUDY = 0 AND EXISTS ( SELECT * FROM [v3d].[SeriesReferences] WHERE [ruid] = @UID )
      EXEC [master].[dbo].[sp_throwException] 901405

    -- ============================================================================
    -- Check if the Series is being processed. If it is then raise an error
    -- otherwise delete the job associated with the series
    -- ============================================================================
    IF @FORCE = 0 AND EXISTS ( SELECT * FROM [v3d].[Jobs] WHERE [src] = @UID AND [state] = @PROCESSING )
    BEGIN
      DECLARE @MSG AS NVARCHAR(512)
      SET @MSG = 'Series(' + @UID + ')'
      EXEC [master].[dbo].[sp_throwException] 900983,  @MSG 
    END
    ELSE
    BEGIN
      -- =========================================
      -- Delete the job associated with the series
      -- ==========================================
      DELETE FROM [v3d].[Jobs] WHERE [src] = @UID
    END


    --- Delete volumes
    DELETE [v3d].[Volumes] WHERE [uid] = @UID

    -- Delete the series refences
    DELETE [v3d].[SeriesReferences] WHERE [uid] = @UID
    
    -- Delete the series dependents
    DELETE [v3d].[SeriesReferences] WHERE [ruid] = @UID


    -- =====================================================================================
    -- Add series path to list of paths that need to be deleted if no rollback occurs
    -- ======================================================================================
    INSERT [v3d].[DeletePaths] ( [id], [uid], [path] ) VALUES ( @TRANSID, @UID, [v3d].[fn_getSeriesPath]('local',@UID) )
    
    
    -- Delete the series
    DELETE [v3d].[Series] WHERE [uid] = @UID
 
    
    -- ==========================================================
    -- Delete the study if the flag is set and no series exists
    -- ==========================================================
    IF @DELETE_STUDY = 0 AND  NOT EXISTS ( SELECT * FROM [v3d].[Series] WHERE [suid] = @SUID )
    BEGIN
      EXEC [v3d].[sp_deleteStudyInternal]
        @ID = @ID,
        @TRANSID = @TRANSID,
        @UID = @SUID
     END

     RETURN 1

  END TRY
  BEGIN CATCH
   
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE,
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY

  END CATCH

END





GO
/****** Object:  StoredProcedure [v3d].[sp_deleteStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the study from the database
-- ==================================================
CREATE PROCEDURE [v3d].[sp_deleteStudy]
(
@ID     NVARCHAR(256),            -- Context ID
@UID    NVARCHAR(MAX) = NULL,     -- Study to be deleted
@RECORD XML = NULL,               -- XML
@FORCE  BIT = 0                   -- FORCE the delete (ignores issues with the series)
)
AS
BEGIN 
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @PROCEDURE NVARCHAR(128)

  DECLARE @OPEN           BIT
  DECLARE @RC             INT
  DECLARE @TRANSID        UNIQUEIDENTIFIER
  DECLARE @STUDY_UID      NVARCHAR(128)
  DECLARE @TRANSCOUNT     INT
  

  RETRY:


  SELECT  @TRANSCOUNT     = @@TRANCOUNT,
          @OPEN           = 0,
          @RC             = 0,
          @PROCEDURE      = OBJECT_NAME(@@PROCID),
          @TRANSID        = NEWID()
 
 
  BEGIN TRY

    SET XACT_ABORT  ON

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION

    -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    -- ===========================================================
    -- If the UID is not specified and the RECORD was then 
    -- the extract it from the RECORD
    -- ============================================================
    IF @UID IS NULL AND @RECORD IS NOT NULL
    BEGIN


      DECLARE @IDOC AS INT

      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      -- Extract the user information 
      SELECT @UID          = [uid]
            FROM OPENXML(@IDOC, N'/study', 1) WITH 
            ( 
              [uid]            nvarchar(MAX)
            )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC


    END

    -- ======================================================
    -- Raise exception that the UID is missing or invalid
    -- ======================================================
    IF @UID IS NULL OR LEN(@UID) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Study Uid' 
 
    
    IF RIGHT(@UID,1) <> '|'
      SET @UID =  @UID + '|'

    IF LEFT(@UID,1) <> '|'
      SET @UID = '|' + @UID 

    -- =======================================================================================
    -- Attempt to lock all the studies series here. If any series, reference or dependent is 
    -- locked then an exception will be raised now.
    -- =======================================================================================
    EXEC [v3d].[sp_lockStudy] 
      @ID = @ID,
      @UID = @UID, 
      @DESC = 'Study delete',
      @TRANSID = @TRANSID OUTPUT


    DECLARE @TABLE AS TABLE (	[uid] [nvarchar](128) NOT NULL )

    INSERT INTO @TABLE ( [uid] ) ( SELECT  SUBSTRING(@UID, Number + 1, CHARINDEX('|',  @UID, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UID) - 1
              AND SUBSTRING(@UID, Number, 1) = '|' ) 

    SET @RC = @@ROWCOUNT


    DECLARE DELETE_STUDIES_CURSOR CURSOR LOCAL
        FOR  SELECT [uid] FROM @TABLE

    OPEN DELETE_STUDIES_CURSOR
      
      
    -- Cursor is open
    SET @OPEN = 1
      
    -- fetch the first record
    FETCH NEXT FROM DELETE_STUDIES_CURSOR INTO @STUDY_UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN
        
      -- Delete Series
      EXEC [v3d].[sp_deleteStudyInternal]
        @ID      = @ID,
        @UID     = @STUDY_UID,
        @FORCE   = @FORCE,
        @TRANSID = @TRANSID
         
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM DELETE_STUDIES_CURSOR INTO @STUDY_UID

    END  -- END WHILE
     
    -- Close the CURSOR
    CLOSE DELETE_STUDIES_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE DELETE_STUDIES_CURSOR
        
    -- Cursor is closed
    SET @OPEN = 0


    -- ==========================================
    -- Delete all locks for this transaction
    -- =========================================
    EXEC [v3d].[sp_deleteLock]
      @ID = @ID,
      @TRANSID = @TRANSID

    
    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION


    DECLARE @PATH AS NVARCHAR(1024)
    DECLARE @ERROR_MESSAGE AS NVARCHAR(2048)
    DECLARE @PATH_UID AS NVARCHAR(128)
    DECLARE DELETE_PATHS_CURSOR CURSOR FOR  SELECT [path], [uid] FROM [v3d].[DeletePaths] WHERE [id] = @TRANSID

    OPEN DELETE_PATHS_CURSOR
      
      
    -- fetch the first record
    FETCH NEXT FROM DELETE_PATHS_CURSOR INTO @PATH, @PATH_UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN
 

      -- ==============================
      -- Delete the study directory
      -- ==============================
      EXEC [v3d].[sp_deleteDirectory] 
        @uid = @PATH_UID,
        @type = 'Study',
        @directory = @PATH
       
 
     -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM DELETE_PATHS_CURSOR INTO @PATH, @PATH_UID

    END  -- END WHILE

    CLOSE DELETE_PATHS_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE DELETE_PATHS_CURSOR

    -- Delete Paths associated with this transaction
    DELETE [v3d].[DeletePaths] WHERE [id] = @TRANSID

    RETURN @RC

  END TRY
  BEGIN CATCH
  
    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION;

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE,
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
   
  
  END CATCH
  

 END





GO
GRANT EXECUTE ON [v3d].[sp_deleteStudy] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_deleteStudy] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_deleteStudyInternal]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 06/21/2011
-- Description:	Deletes the individual study from the database
-- =============================================================
CREATE PROCEDURE [v3d].[sp_deleteStudyInternal]
(
@ID       NVARCHAR(256),            -- Context Id
@UID      NVARCHAR(128),            -- Study to be deleted
@FORCE    BIT = 0,                  -- Flag indicating a forced delete
@TRANSID  UNIQUEIDENTIFIER = NULL   -- Transaction ID
)
AS
BEGIN 
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @ERROR_MESSAGE NVARCHAR(4000);
  DECLARE @ERROR_NUMBER INT
  DECLARE @ERROR_LINE INT
  DECLARE @ERROR_SEVERITY INT
  DECLARE @ERROR_STATE INT
  DECLARE @ERROR_PROCEDURE NVARCHAR(128)

  -- Locked flag
  DECLARE @DIR            NVARCHAR(512)
  DECLARE @SERIES_UID     NVARCHAR(256)

  DECLARE @OPEN           BIT
  DECLARE @NAME           NVARCHAR(128)
  DECLARE @CONTEXT_LOGIN  NVARCHAR(64)
  DECLARE @DELETE_STUDY   BIT
  DECLARE @SERIES_COUNT   INT
  DECLARE @PROCEDURE      NVARCHAR(128)
   

  RETRY:


  SELECT  @OPEN           = 0,
          @DELETE_STUDY   = 0,
          @SERIES_COUNT   = 0,
          @PROCEDURE      = OBJECT_NAME(@@PROCID),
          @CONTEXT_LOGIN  = [Security].[v3d].[fn_getLogin](@ID),
          @NAME           = ''
 
 
  BEGIN TRY

    -- =====================================================================================
    -- Build the series uid list and get the number of series that exist under the study
    -- ====================================================================================
    DECLARE @SERIES_UIDS NVARCHAR(MAX)
    SET @SERIES_UIDS = '|'
    SELECT @SERIES_UIDS = @SERIES_UIDS + [uid] + '|' FROM [v3d].[Series] WHERE [suid] = @UID
    SET @SERIES_COUNT = @@ROWCOUNT


    BEGIN TRY

      -- Get the studies directory
      SELECT @DIR = [dir], @NAME = [name] FROM [v3d].[Studies] WHERE [uid] = @UID
     

      -- =========================================
      -- Study not found
      -- (900991) Study: Not FOund
      -- =========================================
      IF @@ROWCOUNT = 0 
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Study'


      -- ==========================================================
      -- We only need to call the series delete if we have series
      -- =========================================================
      IF @SERIES_COUNT > 0 
      BEGIN

        -- ===========================================
        -- Deletes the stduies series (always force)
        -- ===========================================
        EXEC [v3d].[sp_deleteSeries]
          @ID           = @ID,
          @UID          = @SERIES_UIDS,
          @TRANSID      = @TRANSID,
          @FORCE        = @FORCE,
          @DELETE_STUDY = 1

      END

  
      -- =====================================================================================
      -- Add study path to list of paths that need to be deleted if no rollback occurs
      -- ======================================================================================
      INSERT [v3d].[DeletePaths] ( [id], [uid], [path] ) VALUES ( @TRANSID, @UID, [System].[v3d].[fn_pathCombine]([v3d].[fn_getRoot]('local','default'), @DIR, '') )
      
 
      -- Delete the study
      DELETE [v3d].[Studies] WHERE [uid] = @UID
      

      
      -- Add audit entry for deletion of the study
      EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
       @UID = @UID, 
       @ACTION = N'delete', 
       @LOGIN = @CONTEXT_LOGIN, 
       @NAME = @NAME,
       @MESSAGE = ''
    
       
      RETURN 1
     
    END TRY
    BEGIN CATCH
    
      THROW;
  
    END CATCH
    
  END TRY
  BEGIN CATCH
  
     EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE,
      @DEADLOCK = 1

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
 
  END CATCH
  

 END





GO
/****** Object:  StoredProcedure [v3d].[sp_errorHandler]    Script Date: 3/19/2014 4:44:49 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_failJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description:	Fails the JOB specified 
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_failJob]
(
@JOBID NVARCHAR(256),             -- Job Id
@MESSAGE NVARCHAR(512)            -- Error Message
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
	DECLARE @RC AS INT


	BEGIN TRY


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @JID        AS UNIQUEIDENTIFIER
    DECLARE @FAILED     AS INT
    DECLARE @COMPLETED  AS INT
    DECLARE @WARNING    AS INT
            
     
    -- ======================================
    -- Makes sure the JOB id was specified
    -- (900990) Job Id: Missing or Invalid
    -- =======================================
    IF @JOBID IS NULL OR LEN(@JOBID) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Job Id'

    SELECT  @JID = CONVERT(UNIQUEIDENTIFIER,@JOBID)


    DECLARE @VOLUME AS NVARCHAR(64)
    DECLARE @TASK   AS XML
    DECLARE @UID    AS NVARCHAR(128)
    DECLARE @INDEX  AS INT
    DECLARE @TID    AS NVARCHAR(32)
    DECLARE @LOGIN  AS NVARCHAR(32)
    DECLARE @ORT    AS NVARCHAR(32)
    DECLARE @MOD    AS NVARCHAR(32)
    DECLARE @DESC   AS NVARCHAR(512)
    DECLARE @NAME   AS NVARCHAR(256)
    DECLARE @APP    AS NVARCHAR(16)
    DECLARE @ICNT   AS INT
    DECLARE @SRC    AS NVARCHAR(128)
    DECLARE @STUDY  AS NVARCHAR(128)
    DECLARE @DST    AS NVARCHAR(128)
    DECLARE @STDTE  AS DATETIMEOFFSET(7)
    DECLARE @CRDTE  AS DATETIMEOFFSET(7)
    DECLARE @MODULE AS NVARCHAR(62)

    -- Extract data from the current job (will be used to seed the next job)
    SELECT  @APP        = [app],                  -- Application type
            @VOLUME     = [vol],                  -- Volume name
            @TASK       = [task],                 -- Task XML
            @TID        = [tid],                  -- Template Id
            @DST        = [dst],                  -- Destination UID
            @LOGIN      = [login],                -- Login
            @ORT        = [ort],                  -- Orientation
            @DESC       = [desc],                 -- Series Description
            @NAME       = [name],                 -- Patient name
            @MOD        = [mod],                  -- Modality
            @ICNT       = [icnt],                 -- Image count
            @SRC        = [src],                  -- Source UID
            @STUDY      = [study],                -- Study UID
            @STDTE      = [stdte],                -- Start Date
            @CRDTE      = [crdte],                -- Create Date
            @MODULE     = '',
            @FAILED     = [v3d].[fn_getState]('failed'),
            @COMPLETED  = [v3d].[fn_getState]('completed'),
            @WARNING    = [v3d].[fn_getState]('warning'),
            @MESSAGE    = ISNULL(@MESSAGE,'unspecified error')
            FROM [v3d].[Jobs] WHERE [id] = @JID

    -- ========================================================
    -- If the JOB is not found then throw a NOT FOUND EXECTION
    -- ========================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Job', @JOBID


    -- Extract the module name from the TASK XML
    IF @TASK IS NOT NULL
      SELECT  @MODULE = Task.value('(@name)[1]' , 'nvarchar(32)') FROM @TASK.nodes('task') AS N(Task)


    -- Delete the job
    DELETE [v3d].[Jobs] WHERE [id] = @JID


    -- ===================================================================================================
    -- If the DST (Destination UID) is not set then use the SRC (Source UID) as the associated series uid.
    -- ===================================================================================================
    IF LEN(@DST) = 0 
      SET @UID = @SRC
    ELSE
      SET @UID = @DST

    -- ==========================================================================================================
    -- If the series already has a completed state then do not fail it. This would only occur on a DICOM series
    -- when there were multi conversion if a previous completed successfully and a subsequent failed we set the 
    -- DICOM series state to warning.
    -- ===========================================================================================================
    IF EXISTS ( SELECT * 
                  FROM [v3d].[Series] 
                  WHERE [uid] = @UID AND 
                        ( [System].[v3d].[fn_isMaskBitSet]([state],'state','completed') = 1 OR 
                          [System].[v3d].[fn_isMaskBitSet]([state],'state','warning') = 1 ) )
    BEGIN

      EXEC [v3d].[sp_setSeriesState]
        @UID = @UID,
        @STATE = @WARNING

      -- Set additional state information
      UPDATE [v3d].[Series] SET [msg] = @MESSAGE + ', check the dicom sluce numbers' WHERE [uid] = @UID
 
 
    END
    ELSE
    BEGIN
        EXEC [v3d].[sp_setSeriesState]
        @UID = @UID,
        @STATE = @FAILED

      -- Set additional state information
      UPDATE [v3d].[Series] SET [msg] = @MESSAGE WHERE [uid] = @UID
 
 
    END


 
    -- =========================================================================
    -- Insert processing history even though it states that the task failed
    -- =========================================================================
    INSERT [Auditing].[v3d].[ProcessingHistory] 
    ( [app], [mod], [msg], [name], [ort], [series], [state], [stdte], [study], [proc], [tid] ) VALUES
    ( ISNULL(@APP,''), @MOD, @MESSAGE, @NAME, @ORT, @UID, @FAILED, ISNULL(@STDTE,@CRDTE), @STUDY, @TASK, @TID )


    -- ===============================
    -- Serialize the updated series
    -- ==============================
    EXEC [v3d].[sp_serializeSeries]
      @UID = @UID

    -- =================================
    -- Insert the failed message
    -- =================================
    INSERT [v3d].[Messages] 
        ( [type], [app], [name], [src], [ort], [msg] ) VALUES
        ( 2,      @APP,  @NAME,  @UID,  @ORT,  @MESSAGE ) 


    RETURN 1 
        
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_failJob'
    

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_failJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_failJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_fileExists]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_fileExists]
	@FILEPATH [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_fileExists]
GO
/****** Object:  StoredProcedure [v3d].[sp_getApplications]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================
-- Author:		  Mark Kontak
-- Create date: 12/13/2013
-- Description:	Returns the list of viewer applications launchable from the series specified
-- =============================================================================================
CREATE PROCEDURE [v3d].[sp_getApplications]
@ID AS NVARCHAR(256),         -- Context Id
@UIDS AS NVARCHAR(MAX),       -- UID List
@RESULTS AS XML OUTPUT        -- Result set
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON

  DECLARE @TRUE       AS BIT
  DECLARE @OPEN       AS BIT
  DECLARE @FALSE      AS BIT
  DECLARE @LID        AS UNIQUEIDENTIFIER
  DECLARE @COUNT      AS INT
  DECLARE @APP        AS NVARCHAR(32)
  DECLARE @PAPP       AS NVARCHAR(32)
  DECLARE @TYPE       AS INT
  DECLARE @UID        AS NVARCHAR(128)
  DECLARE @SUID       AS NVARCHAR(128)
  DECLARE @SESSION    AS INT


  DECLARE @SESSION_SPECIFIED AS BIT
  DECLARE @SERIES_SPECIFIED AS BIT
 
  RETRY:
  
  BEGIN TRY
 

    SELECT  @OPEN  = 0,
            @FALSE = 0,
            @TRUE  = 1,
            @PAPP  = NULL,
            @SESSION = [System].[v3d].[fn_getMaskBit]('type','session'),
            @RESULTS = '<collection id="application" ></collection>',
            @SERIES_SPECIFIED = 0,
            @SESSION_SPECIFIED = 0
 

     -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'Storage'
 
    -- ================================================
    -- Missing UIDS (900990) Uids missing or invalid
    -- ================================================
    IF @UIDS IS NULL OR  LEN(@UIDS) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uids' 
   
     SET @UIDS = '|' + @UIDS + '|'

    
    -- =========================================================
    -- Builds the UID Table from the list of UID's passed in
    -- =========================================================
    DECLARE @UID_TABLE AS TABLE (	[id] INT IDENTITY(1,1) NOT NULL, 
                                  [uid] [nvarchar](128) NOT NULL, 
                                  [app] [nvarchar](32) NOT NULL DEFAULT '',
                                  [type] INT NOT NULL DEFAULT 0 )

    INSERT INTO @UID_TABLE ( [uid] ) ( SELECT  SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UIDS) - 1
              AND SUBSTRING(@UIDS, Number, 1) = '|' ) 


    -- Actual UID count
    SET @COUNT = @@ROWCOUNT



    -- ==========================================================================
    -- Create the UID's CURSOR so we can iterate though each UID individually
    -- ==========================================================================
    DECLARE UIDS_CURSOR CURSOR LOCAL
        FOR SELECT [uid] FROM @UID_TABLE 

    SET @OPEN = @TRUE

    OPEN UIDS_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM UIDS_CURSOR INTO @UID

    -- =============================================
    -- loop while there are records in the cursor
    -- =============================================
    WHILE @@FETCH_STATUS = 0
    BEGIN
              
      -- =================================================================
      -- Check to make sure the series exists and grab the current TYPE
      -- =================================================================
      SELECT @TYPE = [type], @SUID = [suid], @APP = [app] FROM [v3d].[Series] WHERE [uid] = @UID

      -- =======================================
      -- Indicate that uid was invalid 
      -- =======================================      
      IF @@ROWCOUNT = 0
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Series'

  
      IF @TYPE = @SESSION AND @SERIES_SPECIFIED = @TRUE
          RETURN

      IF @TYPE <> @SESSION AND @SESSION_SPECIFIED = @TRUE
          RETURN

      IF @PAPP IS NOT NULL AND @PAPP <> @APP
          RETURN
      ELSE
        SET @PAPP = @APP
        

      -- Update 
      UPDATE @UID_TABLE SET [type] = @TYPE, [app] = @APP WHERE [uid] = @UID

  
            
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM UIDS_CURSOR INTO @UID

    END  -- END WHILE
     
    -- Close the CURSOR
    CLOSE UIDS_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE UIDS_CURSOR


    SET @OPEN = @FALSE

    DECLARE @APPS AS NVARCHAR(MAX)
    DECLARE @RULE AS XML
    DECLARE @MAX AS INT
    DECLARE @MIN AS INT

    
    IF @APP IS NOT NULL AND LEN(@APP) > 0 
    BEGIN

 
      SELECT @RULE = [launch] FROM [System].[v3d].[Applications] WHERE [id] = @APP AND [inst] = @TRUE

      IF @@ROWCOUNT = 0 
        RETURN

      SELECT @MAX = n.value('@max[1]', 'int'), @MIN = n.value('@min[1]','int') FROM @RULE.nodes('/rule[1]') AS a(n)

      -- Set variables
      SELECT @MAX   = ISNULL(@MAX,1000),
             @MIN   = ISNULL(@MIN,1)

      
      IF @COUNT < @MIN OR @COUNT > @MAX
        RETURN
      
    END
    ELSE
    BEGIN

      SELECT @APPS = '|'
 
      DECLARE APP_CURSOR CURSOR LOCAL FOR SELECT [id], [launch] FROM [System].[v3d].[Applications] WHERE [inst] = @TRUE

      OPEN APP_CURSOR

      FETCH NEXT FROM APP_CURSOR INTO @APP, @RULE
       
      WHILE @@FETCH_STATUS = 0
      BEGIN

        SELECT @MAX = n.value('@max[1]', 'int'), @MIN = n.value('@min[1]','int') FROM @RULE.nodes('/rule[1]') AS a(n)

        -- Set variables
        SELECT @MAX   = ISNULL(@MAX,1000),
               @MIN   = ISNULL(@MIN,1)

      
      IF @COUNT >= @MIN AND @COUNT <= @MAX
        SET @APPS = @APPS + @APP + '|'


        FETCH NEXT FROM APP_CURSOR INTO @APP, @RULE
 
      END


      CLOSE APP_CURSOR

      DEALLOCATE APP_CURSOR


    END



    -- ==========================================================================
    -- Create the UID's CURSOR so we can iterate though each UID individually
    -- ==========================================================================
    DECLARE UIDS_CURSOR CURSOR LOCAL
        FOR SELECT [uid], [app], [type] FROM @UID_TABLE


    OPEN UIDS_CURSOR

    SET @OPEN = @TRUE

    -- fetch the first UID
    FETCH NEXT FROM UIDS_CURSOR INTO @UID, @APP, @TYPE

    WHILE @@FETCH_STATUS = 0
    BEGIN

      EXEC [v3d].[sp_addSeriesApplications]
        @ID = @ID,
        @APPS = @APPS,
        @LID = @LID OUTPUT,
        @UID = @UID,
        @RESULTS = @RESULTS OUTPUT
 
  

      -- fetch the first UID
      FETCH NEXT FROM UIDS_CURSOR INTO @UID, @APP, @TYPE


    END


    CLOSE UIDS_CURSOR

    DEALLOCATE UIDS_CURSOR

    SET @OPEN = @FALSE
 


	END TRY
	BEGIN CATCH
	
    IF @OPEN = @TRUE
    BEGIN
     -- Close the CURSOR
      CLOSE UIDS_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE UIDS_CURSOR
    END


   DECLARE @ERROR_CODE AS INT

   EXEC @ERROR_CODE = [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getAppLaunchList',
      @DEADLOCK = @TRUE
       

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
 

	  RETURN 0
	  
	END CATCH


END



GO
/****** Object:  StoredProcedure [v3d].[sp_getDependent]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/22/2011
-- Description:	Returns the dependent series
-- =============================================
CREATE PROCEDURE [v3d].[sp_getDependent]
(
@ID         NVARCHAR(256),              -- Context ID
@UID        NVARCHAR(128) = NULL,       -- Series UID to retrieve
@QUERY      XML = '<dependent />',      -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @WRAP AS BIT
  DECLARE @RC AS INT DECLARE @PROCEDURE AS NVARCHAR(128)

  SELECT @WRAP = 1

  BEGIN TRY

    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the UID was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single STUDY item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @UID IS NULL 
    BEGIN
    
      SET @WRAP = 1
      
      DECLARE @IDOC AS INT

      DECLARE @ORIENTATION      AS NVARCHAR(512)
      DECLARE @PLANE            AS TINYINT
      DECLARE @FROM_DATE        AS DATETIMEOFFSET(7)
      DECLARE @TO_DATE          AS DATETIMEOFFSET(7)
      DECLARE @MODALITY         AS NVARCHAR(512)
      DECLARE @APP              AS NVARCHAR(512)
      DECLARE @TYPE             AS INT
      DECLARE @DESCRIPTION      AS NVARCHAR(512)
      DECLARE @FROM_IMAGE_COUNT AS INT
      DECLARE @TO_IMAGE_COUNT   AS INT
      DECLARE @STATE            AS INT
      DECLARE @SOURCE           AS NVARCHAR(512)
      DECLARE @NUMBER           AS NVARCHAR(64)
      DECLARE @SERIES_UID       AS NVARCHAR(1000)
      DECLARE @STUDY_UID        AS NVARCHAR(256)

      DECLARE @SERIES AS XML

     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      -- Extract the user information 
      SELECT  @SERIES_UID       = [uid],
              @STUDY_UID        = [suid],
              @ORIENTATION      = [ort],
              @PLANE            = [pln],
              @TYPE             = [type],
              @FROM_DATE        = [fromDate],
              @TO_DATE          = [toDate],
              @MODALITY         = [mod],
              @APP              = [app],
              @STATE            = [state],
              @SOURCE           = [src],
              @DESCRIPTION      = [desc],
              @FROM_IMAGE_COUNT = [fromImageCount],
              @TO_IMAGE_COUNT   = [toImageCount],
              @NUMBER           = [num]
            FROM OPENXML(@IDOC, N'/dependent', 1) WITH 
            ( 
              [uid]             nvarchar(1000),
              [suid]            nvarchar(1000),
              [num]             nvarchar(512),
              [fromDate]        datetimeoffset(7) 'date/@from',
              [toDate]          datetimeoffset(7) 'date/@to',
              [ort]             nvarchar(512),
              [mod]             nvarchar(512),
              [desc]            nvarchar(512),
              [type]            int,
              [fromImageCount]  int 'icnt/@from',
              [toImageCount]    int 'icnt/@to',
              [state]           int,
              [src]             nvarchar(512),
              [app]             nvarchar(512),
              [pln]             tinyint
           )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC

    END -- END If @LOGIN IS NULL


    DECLARE @WHERE AS NVARCHAR(1000)
    
    SET @WHERE = ''
    
    IF @UID IS NOT NULL
    BEGIN
    
      -- Make sure the series exists and if not throw exception with VX2021 (StudyNotFOund)
      IF NOT EXISTS ( SELECT * FROM [v3d].[Series] WHERE [uid] = @UID ) 
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Series'
    
    
      SET @WRAP = 0
      
      SET @WHERE = ' [uid] = @UID AND '
      
    END
    ELSE
    BEGIN
    
    
      SET @UID = @SERIES_UID
      
      SET @WHERE = [System].v3d.fn_buildStringComparison(N'@UID', @UID, '[uid]')
   
      SET @WHERE = [System].v3d.fn_buildStringComparison(N'@STUDY_UID', @STUDY_UID, '[suid]')
          
      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_DATE, N'@FROM_DATE', @TO_DATE, N'@TO_DATE', N'[date]')
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@ORIENTATION', @ORIENTATION, N'[ort]')
     
      IF @TYPE IS NOT NULL
        SET @WHERE = @WHERE + ' ( [type] & @TYPE ) = [type] AND '
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@SOURCE', @SOURCE, '[src]')
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@MODALITY', @MODALITY, '[mod]')
              
       IF @STATE IS NOT NULL
          SET @WHERE = @WHERE + ' ( [state] & @STATE ) > 0 AND '
      
      IF @PLANE IS NOT NULL
          SET @WHERE = @WHERE + ' ( [pln] & @PLANE ) > 0 AND '

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@DESCRIPTION', @DESCRIPTION, '[desc]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@APP', @APP, '[app]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@NUMBER', @NUMBER, '[num]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildIntegerComparison(@FROM_IMAGE_COUNT, N'@FROM_IMAGE_COUNT', @TO_IMAGE_COUNT, N'@TO_IMAGE_COUNT', N'[icnt]')

    END
    
    DECLARE @SELECT NVARCHAR(1000)  
    
    SET @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Series'') AS [ver]  FROM [v3d].[DependentSeries] '

    DECLARE @LEN AS INT
     
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      

    IF @WRAP = 1
      SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''series''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    ELSE
      SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''series'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                      N'@FROM_DATE          DATETIMEOFFSET(7), ' +
                      N'@TO_DATE            DATETIMEOFFSET(7), ' +
                      N'@FROM_IMAGE_COUNT   INT, ' +
                      N'@TO_IMAGE_COUNT     INT, '  +
                      N'@NUMBER             NVARCHAR(512), ' +
                      N'@ORIENTATION        NVARCHAR(512), '  +
                      N'@TYPE               INT, ' +
                      N'@MODALITY           NVARCHAR(512), ' +
                      N'@DESCRIPTION        NVARCHAR(255), ' + 
                      N'@STUDY_UID          NVARCHAR(128), ' + 
                      N'@STATE              INT, ' + 
                      N'@PLANE              TINYINT, ' + 
                      N'@UID                NVARCHAR(1000), ' +
                      N'@COUNT              INT OUTPUT, ' +
                      N'@SOURCE             NVARCHAR(512), ' +
                      N'@APP                NVARCHAR(512) '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_DATE          = @TO_DATE,
            @FROM_DATE        = @FROM_DATE,
            @FROM_IMAGE_COUNT = @FROM_IMAGE_COUNT,
            @TO_IMAGE_COUNT   = @TO_IMAGE_COUNT,
            @NUMBER           = @NUMBER,
            @ORIENTATION      = @ORIENTATION,
            @PLANE            = @PLANE,
            @SOURCE           = @SOURCE,
            @STATE            = @STATE,
            @DESCRIPTION      = @DESCRIPTION,
            @APP              = @APP,
            @MODALITY         = @MODALITY,
            @STUDY_UID        = @STUDY_UID,
            @TYPE             = @TYPE,
            @UID              = @UID,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="dependent" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"dependent"} into (/collection)[1]')   
   
   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDependent'
  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getDependent] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDependent] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDicomHeader]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================
-- Author:		  Mark Kontak
-- Create date: 12/5/2011
-- Description:	Gets the Dicom Header XML's for the UID specified
-- ================================================================
CREATE PROCEDURE [v3d].[sp_getDicomHeader]
(
@ID NVARCHAR(256),                      -- Context ID
@UID NVARCHAR(128) = NULL,              -- Study UID
@QUERY XML = '<dicomheader />',         -- Query parameters
@RESULTS XML OUTPUT                     -- XML results    
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @TYPE AS INT
  DECLARe @DICOM AS INT
  DECLARE @IDOC AS INT

  SELECT @DICOM = [System].[v3d].[fn_getMaskBit]('type','dicom')

  BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'
  
    IF @UID IS NULL
    BEGIN
    
      -- ==================
      -- Read the query xml
      -- ==================
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      -- =================================
      -- Extract the query information.
      -- ================================
      SELECT  @UID     = [uid]
            FROM OPENXML(@IDOC, N'//dicomheader', 1) WITH 
            ( 
              [uid]           nvarchar(256)
            )
           
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC
   
    
    END
    
   
    SET @UID = ISNULL(@UID,'')
    
    -- ===================================================================
    -- If no UID was specified then raise the InvalidSeries exception
    -- (900990) Series Uid: Missing of Invalid
    -- ===================================================================
    IF LEN(@UID) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Series Uid'
    
    -- Get the series from the table  
    SELECT @TYPE = [type], @RESULTS = [hdr] FROM [v3d].[Series] WHERE [uid] = @UID
    
    -- ==============================================================================================================
    -- If the series exists then check if it is a DICOM type. If not find the DICOM refernced series and grab that 
    -- header.
    -- =============================================================================================================
    IF @@ROWCOUNT > 0
    BEGIN
      
      -- =======================================================================
      -- If the series is not DICOM then look fror a dicom in the references
      -- ======================================================================
      IF @TYPE <> @DICOM
      BEGIN
      
        -- Returns the first dicom header fro the reference list
        SELECT TOP(1) @RESULTS = [hdr] FROM [v3d].[ReferenceSeries] WHERE [ruid] = @UID AND [type] = @DICOM 
        
      END
  
    
    END
    ELSE
     EXEC [master].[dbo].[sp_throwNotFoundException]  'Series', @UID 
 
    SET @RESULTS = @RESULTS.query('.//data-set')
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDicomHeader'
  
  
  END CATCH




  
END





GO
GRANT EXECUTE ON [v3d].[sp_getDicomHeader] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDicomHeader] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDicomHeaderNative]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================
-- Author:		  Mark Kontak
-- Create date: 12/5/2011
-- Description:	Gets the Dicom Header XML's for the UID specified
-- ================================================================
CREATE PROCEDURE [v3d].[sp_getDicomHeaderNative]
(
@ID NVARCHAR(256),                      -- Context ID
@QUERY XML = '<dicomheader />',         -- Query parameters
@RESULTS NVARCHAR(MAX) OUTPUT                     -- XML results    
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
    EXEC @RC = [v3d].[sp_getDicomHeader]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT

  
    SET @RESULTS = CONVERT(NVARCHAR(MAX),@XML)
    
    RETURN @RC
        
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDicomHeaderNative'
  
  
  END CATCH




  
END





GO
GRANT EXECUTE ON [v3d].[sp_getDicomHeaderNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDicomHeaderNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getDicomServer]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 6/10/2010
-- Description:	Get the dicom server status
--
-- =============================================
CREATE PROCEDURE [v3d].[sp_getDicomServer]
(
@ID         NVARCHAR(256),              -- Context ID
@QUERY      XML = '<dicomserver />',    -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
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

    DECLARE @AETITLE      AS NVARCHAR(128)
    DECLARE @SERVER       AS NVARCHAR(128)

   
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @AETITLE       = [aetitle],
            @SERVER        = [server]
          FROM OPENXML(@IDOC, N'/dicomserver', 1) WITH 
          ( 
            [aetitle]         nvarchar(128),
            [server]          nvarchar(128)
         )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC



    DECLARE @WHERE AS NVARCHAR(1000)
    DECLARE @SELECT NVARCHAR(1000)  
  
    SELECT  @WHERE = [System].v3d.fn_buildStringComparison(N'@AETITLE', @AETITLE, '[aetitle]') + [System].v3d.fn_buildStringComparison(N'@SERVER', @SERVER, '[server]'),
            @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''DicomServers'') AS [ver], ' +
                   '( SELECT *, [v3d].[fn_getVersion](''Receivers'') AS [ver]  FROM  [v3d].[Receivers] ' +
                   '     WHERE [v3d].[Receivers].[aetitle] = [v3d].[DicomServers].[aetitle] FOR XML RAW(''receiver''), TYPE, ROOT(''receivers'') ) ' + 
                   '        FROM [v3d].[DicomServers] '

    DECLARE @LEN AS INT
     
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      
    SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''dicomserver''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                      N'@COUNT              INT OUTPUT, ' +
                      N'@AETITLE            NVARCHAR(128), ' +
                      N'@SERVER             NVARCHAR(128) '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @AETITLE          = @AETITLE,
            @SERVER           = @SERVER,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="dicomserver" />'
    ELSE 
      SET @RESULTS.modify('insert attribute id {"dicomserver"} into (/collection)[1]')   
   
    RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getDicomServer'
  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getDicomServer] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getDicomServer] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the jobs order by the dtae modified
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getJob]
(
@ID AS NVARCHAR(256),                 -- Context Id
@QUERY AS XML = '<job />',            -- Query xml
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
  
    
     
    DECLARE @IDOC AS INT

    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @NAME AS NVARCHAR(512)
    DECLARE @STATE AS INT
    DECLARE @JID AS NVARCHAR(256)
    DECLARE @TO_CREATE_DATE AS DATETIMEOFFSET(3)
    DECLARE @FROM_CREATE_DATE AS DATETIMEOFFSET(3)
    DECLARE @UID AS NVARCHAR(1000)
    DECLARE @APP AS NVARCHAR(32)
    DECLARE @TASK AS NVARCHAR(1000)
    DECLARE @LOGIN AS NVARCHAR(1000)

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY


    SELECT  @JID              = [id],
            @NAME             = [name],
            @STATE            = [state],
            @FROM_CREATE_DATE = [fromCreateDate], 
            @TO_CREATE_DATE   = [toCreateDate],
            @NAME             = [name],
            @APP              = [app],
            @LOGIN            = [login]
            FROM OPENXML(@IDOC, N'/job', 3) 
            WITH 
            (
              [id]              uniqueidentifier,
              [state]           int,
              [name]            nvarchar(1000),
              [app]             nvarchar(32),
              [login]           nvarchar(1000),
              [fromCreateDate]  datetimeoffset(3) 'crdte/@from',
              [toCreateDate]    datetimeoffset(3) 'crdte/@to'
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC


    
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)

    -- ====================================
    -- Initialize dynamic select variables
    -- ====================================
    SET @WHERE = ''      
    SET @SELECT = ''

 
      
    SELECT @WHERE = @WHERE + [System].v3d.fn_buildStringComparison( N'@JID', @JID, N'[v3d].[Jobs].[id]')
    
    IF @STATE IS NOT NULL
      SET @WHERE = @WHERE + ' ( [v3d].[Jobs].[state] & @STATE ) > 0 AND'
            
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison( N'@APP', @APP, N'[v3d].[Jobs].[app]')
       
    SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_CREATE_DATE, N'@FROM_CREATE_DATE', @TO_CREATE_DATE, N'@TO_CREATE_DATE', N'[v3d].[Jobs].[crdte]')
           
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[v3d].[Tasks].[name]')
      
    SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@LOGIN', @LOGIN, '[v3d].[Jobs].[login]')

 
    IF LEN(@NAME) > 0 
      SET @SELECT = 'SELECT [v3d].[Jobs].*, [v3d].[fn_getVersion](''Jobs'') AS [ver] FROM [v3d].[Jobs] '
    ELSE
      SET @SELECT = 'SELECT [v3d].[Jobs].*, [v3d].[fn_getVersion](''Jobs'') AS [ver] FROM [v3d].[Jobs] '
       
    DECLARE @LEN AS INT             
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + ' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3) + ' ORDER BY [v3d].[Jobs].[crdte]  '
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT +  ' FOR XML RAW(''job''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS XML OUTPUT, ' +
                      N'@FROM_CREATE_DATE DATETIMEOFFSET(3), ' + 
                      N'@TO_CREATE_DATE DATETIMEOFFSET(3), ' +
                      N'@NAME NVARCHAR(1000), ' +
                      N'@APP NVARCHAR(32), ' +
                      N'@LOGIN NVARCHAR(1000), ' +
                      N'@STATE INT, ' +
                      N'@COUNT INT OUTPUT, ' +
                      N'@JID AS UNIQUEIDENTIFIER '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_CREATE_DATE     = @TO_CREATE_DATE,
            @FROM_CREATE_DATE   = @FROM_CREATE_DATE,
            @NAME               = @NAME,
            @APP                = @APP,
            @STATE              = @STATE,
            @LOGIN              = @LOGIN,
            @JID                = @JID,
            @COUNT              = @RC OUTPUT,
            @RESULTS            = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="job" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"job"} into (/collection)[1]')   

      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getJob'

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getJobPath]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the jobs order by the dtae modified
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getJobPath]
(
@ID AS NVARCHAR(256),                 -- Context Id
@QUERY AS XML = '<jobpath />',        -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC AS INT
  DECLARE @DICOM AS INT


  RETRY:

  BEGIN TRY
  

    DECLARE @NAME AS NVARCHAR(32)
    DECLARE @JID  AS UNIQUEIDENTIFIER
    DECLARE @INDEX AS INT
    DECLARE @LOCALE AS NVARCHAR(16)
     
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'
  
      
    DECLARE @IDOC AS INT

  
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY


    SELECT  @NAME             = [name],
            @INDEX            = [idx],
            @JID              = [id],
            @LOCALE           = [loc]
              FROM OPENXML(@IDOC, N'/jobpath', 3) 
            WITH 
            (
              [id]              uniqueidentifier,
              [name]            nvarchar(32),
              [idx]             int,
              [loc]             nvarchar(16)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  

    SELECT @DICOM = [System].[v3d].[fn_getMaskBit]('type','dicom'),
           @NAME = ISNULL(@NAME,''),
           @INDEX = ISNULL(@INDEX, 0),
           @LOCALE = ISNULL(@LOCALE,'Remote')

    -- =======================================
    --- (900990) Job Id: Missing or Invalid
    -- =======================================
    IF @JID IS NULL 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Job Id'

    DECLARE @SOURCE_UID NVARCHAR(128)
    DECLARE @DEST_UID NVARCHAR(128)
    DECLARE @VOLUME_NAME NVARCHAR(32)
    DECLARE @CONVERSION_ID NVARCHAR(32)


    SELECT @SOURCE_UID = [src], @DEST_UID = [dst], @VOLUME_NAME = [vol], @CONVERSION_ID = [cid] FROM [v3d].[Jobs] WHERE [id] = @JID

    -- ============================
    -- (900991) Job: Not Found
    -- ============================
    IF @@ROWCOUNT = 0
    BEGIN
      DECLARE @J AS NVARCHAR(256)
      SET @J = CAST(@JID AS NVARCHAR(256))
      EXEC [master].[dbo].[sp_throwNotFoundException]  'Job', @J
    END
    
    DECLARE @SOURCE_PATH NVARCHAR(512)
    DECLARE @DICOM_PATH NVARCHAR(512)
    DECLARE @VOLUME_PATH NVARCHAR(512)
    DECLARE @DEST_PATH NVARCHAR(512)
    DECLARE @SOURCE_FILENAME NVARCHAR(512)
    DECLARE @SOURCE_SERIES_PATH AS NVARCHAR(512)
    
    SELECT @SOURCE_FILENAME = REPLACE(@SOURCE_UID, '.','_'),
           @SOURCE_SERIES_PATH = [Storage].[v3d].[fn_getSeriesPath](@LOCALE,@SOURCE_UID)

    -- ====================================================================================================
    -- Sets Default Paths
    --    SOURCE_PATH : <SOURCE_SERIES_PATH>\<SOURCE_UID>   
    --    VOLUME_PATH : <SOURCE_SERIES_PATH>\volumes                Assumes the source is a DICOM series
    --    DEST_PATH   : <SOURCE_SERIES_PATH>\volumes\<SOURCE_UID>   Assumes 
    -- ====================================================================================================
    SELECT  @SOURCE_PATH = [System].[v3d].[fn_pathCombine]( @SOURCE_SERIES_PATH, @SOURCE_FILENAME, ''),
            @VOLUME_PATH = [System].[v3d].[fn_pathCombine]( @SOURCE_SERIES_PATH, 'volumes',@CONVERSION_ID),
            @DICOM_PATH  = [System].[v3d].[fn_pathCombine]( @SOURCE_SERIES_PATH, 'dicom',''),
            @DEST_PATH   = [System].[v3d].[fn_pathCombine]( @SOURCE_SERIES_PATH, 'volumes',@SOURCE_FILENAME)

    -- ======================================================================================================
    -- If the Destination UID is set then the JOB is not performing a conversion. 
    -- =====================================================================================================
    IF LEN(@DEST_UID) > 0
    BEGIN

       -- Set the DESTINATION PATH
       SELECT @DEST_PATH = [System].[v3d].[fn_pathCombine]( [Storage].[v3d].[fn_getSeriesPath](@LOCALE,@DEST_UID), REPLACE(@DEST_UID, '.','_'), '')

       -- ================================================================================================
       -- If the SOURCE is not a DICOM series then get the DICOM series using the references and set the 
       -- volume path.
       -- 
       --   VOLUME_PATH   :   <DICOM_SERIES_PATH>\volumes ).
       -- =================================================================================================
       IF NOT EXISTS ( SELECT * FROM [Storage].[v3d].[Series] WHERE [uid] =  @SOURCE_UID AND [type] = @DICOM ) 
       BEGIN
          DECLARE @DICOM_SERIES_PATH  AS NVARCHAR(512)

          SELECT @DICOM_SERIES_PATH = [Storage].[v3d].[fn_getSeriesPath](@LOCALE,[ruid]) 
            FROM [Storage].[v3d].[ReferenceSeries] 
            WHERE [uid] = @DEST_UID AND [type] = @DICOM 
                 
          -- ==============================
          -- If no DICOM series was found
          -- ================================ 
          IF @DICOM_SERIES_PATH IS NULL
            EXEC [master].[dbo].[sp_throwNotFoundException]   'Dicom Series'

          SELECT  @VOLUME_PATH = [System].[v3d].[fn_pathCombine]( @DICOM_SERIES_PATH, 'volumes',@CONVERSION_ID),
                  @DICOM_PATH  = [System].[v3d].[fn_pathCombine]( @DICOM_SERIES_PATH, 'dicom','')
                


      END
      ELSE
      BEGIN

        DECLARE @CHARINDEX AS INT
        DECLARE @FILEPATH AS NVARCHAR(512)
  
        SELECT @FILEPATH = [file] FROM [v3d].[Volumes] WHERE [uid] = @SOURCE_UID AND [name] = @VOLUME_NAME

        SELECT @CHARINDEX = CHARINDEX('.', @FILEPATH)

        IF @CHARINDEX > 0 
          SET @FILEPATH = SUBSTRING(@FILEPATH, 0, @CHARINDEX)
        


        SELECT @SOURCE_PATH = [System].[v3d].[fn_pathCombine]( @SOURCE_SERIES_PATH, [System].[v3d].[fn_pathCombine]('volumes',@CONVERSION_ID,''),@FILEPATH)
      END

         
    END

   

    SET @RESULTS = ( SELECT @SOURCE_PATH AS [src], @DEST_PATH AS [dst], @DICOM_PATH AS [dcm], @VOLUME_PATH AS [vol] FOR XML RAW('jobpath') )

  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getJobPath',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getJobPath] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getJobPath] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLaunchList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Creates the launching list
-- =============================================
CREATE PROCEDURE [v3d].[sp_getLaunchList]
@ID AS NVARCHAR(256),               -- Context Id
@QUERY AS XML,                      -- Query Xml
@RESULTS AS XML OUTPUT              -- Launch list results
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON


  -- Conains the Session UID of the session associated with all the dicom uids supplied
  DECLARE @SESSION_UID AS NVARCHAR(128)
  DECLARE @STUDY_UID   AS NVARCHAR(128)

  DECLARE @DESC AS NVARCHAR(256)                    -- Session Dscription
  DECLARE @UIDS AS NVARCHAR(MAX)                    -- List of UIDS to launch
  DECLARE @APP AS NVARCHAR(16)                      -- Application code
  DECLARE @LOGIN AS NVARCHAR(32)                    -- User doing the launching
  DECLARE @SYSTEM AS NVARCHAR(32)                   -- System the launch is generated from
  DECLARE @FORCE_SESSION_CREATION AS BIT            -- Create Session override flag
  DECLARE @TYPE AS INT                              -- Type of series being processed
  DECLARE @UID AS NVARCHAR(128)                     -- Uid of series being processed
  DECLARE @LIST AS NVARCHAR(MAX)                    -- New generated list
  DECLARE @SUID AS NVARCHAR(128)                    -- Study UID of the series ( filled in with the last series study from the specified list )
  DECLARE @LEN AS INT
  DECLARE @FIRST_TIME AS BIT
  DECLARE @UID_COUNT AS INT
  DECLARE @LAUNCH_RULE AS XML
  DECLARE @OPEN AS BIT
  DECLARE @FALSE AS BIT
  DECLARE @TRUE AS BIT
  DECLARE @COUNT AS INT
  DECLARE @IDX AS INT
  DECLARE @SERIES_APP AS NVARCHAR(16)
  DECLARE @USER_LAUNCH_RULES AS XML
  DECLARE @LOCATION AS NVARCHAR(64)
  DECLARE @SESSION AS INT
  DECLARE @DICOM AS INT
  DECLARe @TYPE_NAME AS NVARCHAR(32)

  -- ========================
  -- Launch Rule Variables
  -- ========================
  DECLARE @SESSION_SELECTION  AS NVARCHAR(16)       -- Session selection setting
  DECLARE @DICOM_SELECTION    AS NVARCHAR(16)       -- Dicom series selection setting
  DECLARE @ORIENTATIONS       AS NVARCHAR(1024)     -- Delimited 
  DECLARE @SERIES_TYPES       AS INT                -- Mask of series types
  DECLARE @MAXIMUM_DATASETS   AS INT                -- Max datasets allowed for the application      
  DECLARE @MINIMUM_DATASETS   AS INT                -- Min datasets required for the application      

  -- ================
  -- Set variables
  -- ================
  SELECT  @TRUE  = 1,
          @FALSE = 0,
          @FIRST_TIME = 1,
          @SERIES_APP = '',
          @LOGIN  = [Security].[v3d].[fn_getLogin](@ID)
 
  RETRY:
  
  BEGIN TRY
 

    BEGIN TRANSACTION

    SET @OPEN = 0

     -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'launch'

    DECLARE @IDOC AS INT

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- ===========================================
    -- Extract the get launch list information
    -- ==========================================
    SELECT  @UIDS           = [uids],
            @LOGIN          = [login],
            @APP            = [app],
            @DESC           = [desc],
            @SYSTEM         = [sys],
            @FORCE_SESSION_CREATION = [new]
            FROM OPENXML(@IDOC, N'/launchlist', 1) WITH 
          ( 
            [uids]            nvarchar(MAX),
            [login]           nvarchar(16),
            [app]             nvarchar(32),
            [sys]             nvarchar(32),
            [desc]            nvarchar(256),
            [new]             bit
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

 
    -- ================================================
    -- Missing UIDS (900990) Uids missing or invalid
    -- ================================================
    IF @UIDS IS NULL OR  LEN(@UIDS) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uids' 
      
    -- =====================================================
    -- Missing APP (900990) Application missing or invalid
    -- =====================================================
    IF @APP IS NULL OR  LEN(@APP) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Application' 
 
    -- =====================================
    -- Set parameters to make sure no NULLS
    -- =====================================
    SELECT  @FORCE_SESSION_CREATION = ISNULL(@FORCE_SESSION_CREATION, 0),
            @SESSION        = [System].[v3d].[fn_getMaskBit]('type','session'),
            @DICOM          = [System].[v3d].[fn_getMaskBit]('type','dicom'),
            @DESC           = ISNULL(@DESC,''),
            @LOCATION       = 'remote',
            @SYSTEM         = ISNULL(@SYSTEM,''),
            @APP            = UPPER(@APP),
            @LEN            = LEN(@UIDS)
    

    -- ===========================================================================
    -- If we are launching from the same system then set the location to local
    -- ===========================================================================
    IF SERVERPROPERTY('MachineName') = @SYSTEM 
      SET @LOCATION = 'local'

    -- ================================================================================================
    -- Gets the applications launch rules and make sure that the application exists and installed.
    -- ================================================================================================
    SELECT @LAUNCH_RULE = [launch] FROM [System].[v3d].[Applications] WHERE [id] = @APP AND [inst] = 1
    
    -- =========================================================================================
    -- If the application was not found in the products then (901402) Application:  Not Installed
    -- =========================================================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwException] 900981, @APP
 
 
    -- =======================================================================================================================
    -- Attempt to get the users launch rules from the uset profiles table. This is to get any custom launch rules for the
    -- user.
    -- =======================================================================================================================
    SELECT @USER_LAUNCH_RULES = [profile] FROM [Security].[v3d].[UserProfiles] WHERE [login] = @LOGIN AND [name] = 'launchrules'

    -- =================================================================================================
    -- If the user has launch rules then try to extract the launch rule for the specific application
    -- ================================================================================================
    IF @@ROWCOUNT = 1
    BEGIN
      IF @USER_LAUNCH_RULES.exist('/launchrules/rule[@app=sql:variable("@APP")]') = 1
       SET @LAUNCH_RULE = @USER_LAUNCH_RULES.query('/launchrules/rule[@app=sql:variable("@APP")]')
    END -- END ... If the user has laucnh rules

    -- =========================================================================================
    -- If the application has no launch rules then throw a NOT FOUND exception
    -- =========================================================================================
    IF @LAUNCH_RULE IS NULL
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Launch Rule' 
 
    
     -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @LAUNCH_RULE

    -- ==================================
    -- Extract the launch rul information
    -- ==================================
    SELECT  @MAXIMUM_DATASETS   = [max],
            @MINIMUM_DATASETS   = [min],
            @ORIENTATIONS       = [ort],
            @SERIES_TYPES       = [type],
            @SESSION_SELECTION  = [session],
            @DICOM_SELECTION    = [dicom]
            FROM OPENXML(@IDOC, N'/rule', 1) WITH 
          ( 
            [max]             int,
            [min]             int,
            [ort]             nvarchar(512),
            [type]            int,
            [session]         nvarchar(16),
            [dicom]           nvarchar(16)
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- Set variables
    SELECT @MAXIMUM_DATASETS   = ISNULL(@MAXIMUM_DATASETS,1000),
           @MINIMUM_DATASETS   = ISNULL(@MINIMUM_DATASETS,1),
           @ORIENTATIONS       = ISNULL(@ORIENTATIONS,''),
           @SERIES_TYPES       = ISNULL(@SERIES_TYPES,0),
           @SESSION_SELECTION  = ISNULL(@SESSION_SELECTION, 'LIFO' ),
           @DICOM_SELECTION    = ISNULL(@DICOM_SELECTION, 'FIFO' ),
           @RESULTS            = '',
           @LIST               = '|',
           @UID_COUNT          = 0,
           @UIDS               = '|' + @UIDS + '|'
           FROM [System].[v3d].[Stores] WHERE [name] = 'default'

      -- =======================================
      -- Make sure we found the default store
      -- =======================================
      IF @@ROWCOUNT = 0
        EXEC [master].[dbo].[sp_throwNotFoundException]  'Default Store'

      -- SESSION UID TABLE 
       DECLARE @SESSION_UID_TABLE AS TABLE ( [session_uid] NVARCHAR(128), [session_date] DATETIMEOFFSET(7) )
 

      -- =========================================================
      -- Builds the UID Table from the list of UID's passed in
      -- =========================================================
      DECLARE @UID_TABLE AS TABLE (	[id] INT IDENTITY(1,1) NOT NULL, [uid] [nvarchar](128) NOT NULL )

      INSERT INTO @UID_TABLE ( [uid] ) ( SELECT  SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE
          FROM master..spt_values
          WHERE Type = 'p'
                AND Number <= LEN(@UIDS) - 1
                AND SUBSTRING(@UIDS, Number, 1) = '|' ) 

      -- Actual UID count
      SET @UID_COUNT = @@ROWCOUNT

      -- ==========================================================================
      -- Check to make sure we are not receiving more datasets then we can handle
      -- ==========================================================================
      IF @MAXIMUM_DATASETS < @UID_COUNT
        EXEC [master].[dbo].[sp_throwInvalidDataException] 'Exceeds maximum limit'

      -- ==========================================================================
      -- Create the UID's CURSOR so we can iterate though each UID individually
      -- ==========================================================================
      DECLARE UIDS_CURSOR CURSOR LOCAL
          FOR SELECT [id], [uid] FROM @UID_TABLE

      SET @OPEN = @TRUE

      OPEN UIDS_CURSOR

      -- fetch the first UID
      FETCH NEXT FROM UIDS_CURSOR INTO @IDX, @UID

      -- =============================================
      -- loop while there are records in the cursor
      -- =============================================
      WHILE @@FETCH_STATUS = 0
      BEGIN
        

        -- ============================
        -- Check if the UID is a study
        -- ============================
        IF EXISTS ( SELECT * FROM [v3d].[Studies] WHERE [uid] = @UID )
        BEGIN
      
          -- =================================================================================
          -- Cannot specifiy multiple studies so I check the STUDY_DETECTED flag to see if we
          -- have already processed a study.
          --     (900995) Multi-Study Launch unsupprted
          -- =================================================================================
          IF @STUDY_UID IS NOT NULL
            EXEC [master].[dbo].[sp_throwException] 900995, 'Multi-Study Launch'

          -- =========================================================================================
          -- Cannot specifiy a study and series combination so I check the @LIST for any series
          --    (900995) Study/Series Launch Unsupported
          -- =========================================================================================
          IF @LIST <> '|'
            EXEC [master].[dbo].[sp_throwException] 900995, 'Study/Series Launch'

          -- Set the study detected flag 
          SET @STUDY_UID = @UID
     
          -- ========================================================================================
          -- If we are not forcing a session creation then look for a session in the studies series
          -- ========================================================================================
          IF @FORCE_SESSION_CREATION <> @TRUE
          BEGIN

            -- ===================================================================================
            -- Look for the studies session using either FIFO or LIFO depending on the setting
            -- ===================================================================================
            IF @SESSION_SELECTION = 'LIFO'
              -- Get the last session if there is one
              SELECT TOP(1) @SESSION_UID = [uid] FROM [v3d].[Series] 
                    WHERE [suid] = @UID AND [type] = @SESSION AND [app] = @APP
                    ORDER BY [date] DESC
            ELSE
              -- Get the first session if there is one
              SELECT TOP(1) @SESSION_UID = [uid] FROM [v3d].[Series] 
                    WHERE [suid] = @UID AND [type] = @SESSION AND [app] = @APP
                    ORDER BY [date] ASC

 
          END -- END ... If we are not forcing a sesion creation

           
        
        
        END -- END ... If the UID is a study
      
        -- ==========================================
        -- Else we have what appears to be a series
        -- ==========================================
        ELSE 
        BEGIN
      
          -- =============================================================================================
          -- Cannot specifiy a study and series combination so I check the STUDY_DETECTED flag to see if 
          -- a study was already processed.
          --     (900995) Study/Series Launch Unsupported
          -- =============================================================================================
          IF @STUDY_UID IS NOT NULL
              EXEC [master].[dbo].[sp_throwException] 900995, 'Study/Series Launch' 
 

          -- =================================================================
          -- Check to make sure the series exists and grab the current TYPE
          -- =================================================================
          SELECT  @TYPE = [type], 
                  @TYPE_NAME = [System].[v3d].[fn_getMaskBitName]('type',[type]), 
                  @SUID = [suid], 
                  @SERIES_APP = [app] 
            FROM  [v3d].[Series] WHERE [uid] = @UID

          -- =======================================
          -- Indicate that the series was not found  
          -- =======================================      
          IF @@ROWCOUNT = 0
            EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uid'
    
     
          -- =================================================================================================
          -- If the UID extracted was a session - build the reference list XML to be returned. Only a single 
          -- session or or a list of dicom series are valid. Cannot mix dicom and session series.
          -- =================================================================================================
          IF @TYPE = @SESSION 
          BEGIN
        
            -- =====================================================================
            -- Only a single session can be specified so check for another series
            -- that hase been processed.
            --    (900990) Uids: Missing or invalid
            -- ====================================================================
            IF @LIST <> '|'
              EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uids' 
        
            -- ========================================================================
            -- If we already have a session we cannot launch multiple sessions
            --    (900995) Multi-Session Launch: Unsupported
            -- =======================================================================
            IF @SESSION_UID IS NOT NULL
              EXEC [master].[dbo].[sp_throwException]  900995, 'Multi-Session Launch'


            -- ==========================================================================
            -- If the sessions application does not match what is being launched then 
            -- throw thw 901436 exception.
            --  Session does not match %s application
            -- ==========================================================================
            IF @SERIES_APP <> @APP
              EXEC [master].[dbo].[sp_throwException] 901436, @APP

            --- Sets the SESSION UID, Processing as Session means that only 1 UID should be in the list
            SELECT  @SESSION_UID = @UID
          
          END -- END ... If the UID is a session
        
          -- ===================================================
          -- The only other type of UID I can receive is dicom
          -- ===================================================
          ELSE 
          BEGIN
        
            -- =============================================================
            -- If the session was already specified then we have an error
            --    (900990) Uids: Missing or Invalid
            -- =============================================================
            IF @SESSION_UID IS NOT NULL
              EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uids'
          
            -- =====================================================================================================
            -- If we are not dicom then make sure that the series type is a valid leaf type for that applications
            -- ====================================================================================================
            IF @TYPE <> @DICOM AND ( @TYPE & @SERIES_TYPES ) <> @TYPE 
                   EXEC [master].[dbo].[sp_throwException] 901432, @TYPE_NAME
            

            IF @FORCE_SESSION_CREATION = @FALSE
            BEGIN

              IF @FIRST_TIME = 1
              BEGIN

                INSERT INTO @SESSION_UID_TABLE SELECT [v3d].[DependentSeries].[duid], [v3d].[DependentSeries].[date] 
                                              FROM [v3d].[DependentSeries] 
                                             WHERE [v3d].[DependentSeries].[type] = @SESSION AND 
                                                   [v3d].[DependentSeries].[app] = @APP AND
                                                   [v3d].[DependentSeries].[uid] = @UID AND
                                                   ( SELECT COUNT(*) FROM [v3d].[ReferenceSeries] 
                                                                    WHERE [v3d].[ReferenceSeries].[uid] = [v3d].[DependentSeries].[duid] AND
                                                                          [v3d].[ReferenceSeries].[type] = @TYPE ) = 1

                SET @FIRST_TIME = 0
              END
              ELSE
              BEGIN
            
                -- Delete any session that are not linked to this series
                DELETE FROM @SESSION_UID_TABLE WHERE NOT EXISTS ( SELECT * FROM [v3d].[DependentSeries]
                                                                    WHERE [v3d].[DependentSeries].[duid] = [session_uid] AND 
                                                                          [v3d].[DependentSeries].[uid] = @UID )
            

              END
            END

            -- Creates the list, Processing non-session series
            SELECT  @LIST = @LIST + @UID + '|'
                          
          END -- END ... Else the UID is a non-session
        

        
        END  -- END ... Else we have a series UID

            
        -- This is executed as long as the previous fetch succeeds.
        FETCH NEXT FROM UIDS_CURSOR INTO @IDX, @UID

      END  -- END WHILE
     
      -- Close the CURSOR
      CLOSE UIDS_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE UIDS_CURSOR

      -- CURSOR is closed
      SET @OPEN = @FALSE     

     
      IF @SESSION_SELECTION = 'FIFO' 
        SELECT TOP(1) @SESSION_UID = [session_uid] FROM @SESSION_UID_TABLE ORDER BY [session_date] ASC
      ELSE
        SELECT TOP(1) @SESSION_UID = [session_uid] FROM @SESSION_UID_TABLE ORDER BY [session_date] DESC
      
      -- =====================================================================================
      -- If a session was specified or a study with a session was specified then call the 
      -- sp_getSessionLaunchList to get the dataset list using the launch rules
      -- =====================================================================================
      IF @SESSION_UID IS NOT NULL
      BEGIN
      
        EXEC [v3d].[sp_getSessionLaunchList]
          @ID = @ID, 
          @SESSION_UID = @SESSION_UID,
          @LOCATION = @LOCATION,
          @RULES = @LAUNCH_RULE,
          @RESULTS = @RESULTS OUTPUT
   
      END -- END ... If a session was specified or found in the specified study
      
      -- ===========================================================================================================
      -- If a study was supplied and we are no session was found in the studies series or thje force a new sesion 
      -- flag was true then call sp_getStudyLaunchList. This stored procedure will generate a list of datasets based
      -- on the launch rules.
      -- ===========================================================================================================
      ELSE IF @STUDY_UID IS NOT NULL
      BEGIN
  
       --- Generate a new UID	
	      SET @SESSION_UID = [System].[v3d].[fn_createUid]()

        -- ===========================================================================================================
        -- Insert the session series. There are no references yet as they will be set up in the sp_getStudyLauchList
        -- based on the lauch rules.
        -- ===========================================================================================================
        EXEC [v3d].[sp_insertSeriesInternal]
              @UID          = @SESSION_UID,          
              @STUDY_UID    = @STUDY_UID,           
              @APP          = @APP,
              @MODALITY     = '',
              @SOURCE       = @LOGIN,
              @DESCRIPTION  = @DESC,
              @TYPE         = @SESSION
     
        -- =======================================
        -- Generate the launch list for the study
        -- ======================================
        EXEC [v3d].[sp_getStudyLaunchList]
          @ID           = @ID, 
          @SESSION_UID  = @SESSION_UID,
          @LOCATION     = @LOCATION,
          @STUDY_UID    = @STUDY_UID,
          @RULES        = @LAUNCH_RULE,
          @RESULTS      = @RESULTS OUTPUT


    END -- END ... If a study was speciifed
    
    -- =====================================================================
    -- If we get to this else then we have a list or series to be launched.
    -- =====================================================================
    ELSE
    BEGIN

      --- Generate a new UID	
	    SET @SESSION_UID = [System].[v3d].[fn_createUid]()

      -- ===========================================================================================================
      -- Insert the session series. There are no references yet as they will be set up in the sp_getSeriesLauchList
      -- based on the lauch rules.
      -- ===========================================================================================================
      EXEC [v3d].[sp_insertSeriesInternal]
            @UID          = @SESSION_UID,          
            @STUDY_UID    = @SUID,           
            @APP          = @APP,
            @MODALITY     = '',
            @SOURCE       = @LOGIN,
            @DESCRIPTION  = @DESC,
            @TYPE         = @SESSION

       -- =============================================
        -- Generate the launch list for the series list 
        -- ============================================
        EXEC [v3d].[sp_getSeriesLaunchList]
          @ID          = @ID, 
          @SESSION_UID = @SESSION_UID,
          @LOCATION    = @LOCATION,
          @UIDS        = @LIST,
          @RULES       = @LAUNCH_RULE,
          @RESULTS     = @RESULTS OUTPUT

    END

 
    COMMIT TRANSACTION

	END TRY
	BEGIN CATCH
	
    IF @OPEN = @TRUE
    BEGIN
     -- Close the CURSOR
      CLOSE UIDS_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE UIDS_CURSOR
    END

   IF XACT_STATE() = -1
    ROLLBACK TRANSACTION

   DECLARE @ERROR_CODE AS INT

   EXEC @ERROR_CODE = [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getLaunchList',
      @DEADLOCK = @TRUE
       

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
 

	  RETURN 0
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_getLaunchList] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getLaunchList] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLaunchListNative]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Creates the launching list
-- =============================================
CREATE PROCEDURE [v3d].[sp_getLaunchListNative]
@ID AS NVARCHAR(256),               -- Context Id
@QUERY AS XML,                      -- Query Xml
@RESULTS AS NVARCHAR(MAX) OUTPUT    -- Launch list results
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON


  DECLARE @RC AS INT
  DECLARE @XML AS XML
  
  BEGIN TRY
 


     -- Make sure the context use is authorize
    EXEC @RC = [v3d].[sp_getLaunchList]
      @ID = @ID,
      @QUERY = @QUERY,
      @RESULTS = @XML OUTPUT

    SET @RESULTS = CONVERT(NVARCHAR(MAX),@XML)

    RETURN @RC

	END TRY
	BEGIN CATCH
	
 
   EXEC  [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getLaunchListNative'
 
	  
	END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_getLaunchListNative] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getLaunchListNative] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getLock]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 6/10/2010
-- Description:	Get the lock epcified
--
-- =============================================
CREATE PROCEDURE [v3d].[sp_getLock]
(
@ID         NVARCHAR(256),              -- Context ID
@QUERY      XML = '<lock />',           -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC AS INT
  DECLARE @PROCEDURE AS NVARCHAR(128)

  SELECT @PROCEDURE = OBJECT_NAME(@@PROCID)

  BEGIN TRY

     
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    
      
    DECLARE @IDOC AS INT

    DECLARE @LOGIN            AS NVARCHAR(MAX)
    DECLARE @UID              AS NVARCHAR(MAX)
    DECLARE @TRANSID          AS NVARCHAR(256)


    DECLARE @SERIES AS XML

     
 
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @UID          = [uid],
            @TRANSID      = [transid],
            @LOGIN        = [login]
            FROM OPENXML(@IDOC, N'/lock', 1) WITH 
          ( 
            [uid]         nvarchar(MAX),
            [transid]     nvarchar(256),
            [login]       nvarchar(MAX)
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

      DECLARE @WHERE AS NVARCHAR(1000)
      DECLARE @SELECT NVARCHAR(1000)  
  
      SELECT @WHERE = [System].v3d.fn_buildStringComparison(N'@UID', @UID, '[uid]') + 
                      [System].v3d.fn_buildStringComparison(N'@LOGIN', @LOGIN, '[login]') +
                      [System].v3d.fn_buildStringComparison(N'@TRANSID', @TRANSID, '[transid]'),
            @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Locks'') AS [ver] FROM [v3d].[Locks] '

      DECLARE @LEN AS INT
     
      SET @LEN = LEN(@WHERE)
    
      IF @LEN > 0
        SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      
        SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''lock''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    
   
      DECLARE @PARAMETERS AS NVARCHAR(4000)
    
      SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                        N'@TRANSID            NVARCHAR(256), ' +
                        N'@LOGIN              NVARCHAR(16), ' +
                        N'@UID                NVARCHAR(128), ' +
                        N'@COUNT              INT OUTPUT'

      EXEC sp_executesql 
              @SELECT,
              @PARAMETERS,
              @LOGIN            = @LOGIN,
              @TRANSID          = @TRANSID,
              @UID              = @UID,
              @COUNT            = @RC OUTPUT,
              @RESULTS          = @RESULTS OUTPUT
      
      IF @RESULTS IS NULL
        SET @RESULTS = '<collection id="lock" />'
      ELSE
        SET @RESULTS.modify('insert attribute id {"lock"} into (/collection)[1]')   




   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE
 
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getLock] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getLock] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getMessage]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the jobs order by the dtae modified
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getMessage]
(
@ID AS NVARCHAR(256),                 -- Context Id
@QUERY AS XML = '<message />',        -- Query xml
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
  DECLARE @RC AS INT


 
  RETRY:

  BEGIN TRY
  
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'
  
      
    DECLARE @IDOC AS INT

    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @TO_DATE AS DATETIMEOFFSET(3)
    DECLARE @FROM_DATE AS DATETIMEOFFSET(3)
    DECLARE @APP AS TINYINT
    DECLARE @MID AS INT
    DECLARE @TYPE SMALLINT
    DECLARE @SOURCE AS NVARCHAR(128)
    DECLARE @ORT AS NVARCHAR(16)
 
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY


    SELECT  @MID              = [id],
            @TYPE             = [type],
            @FROM_DATE        = [fromDate], 
            @TO_DATE          = [toDate],
            @APP              = [app],
            @SOURCE           = [src],
            @ORT              = [ort]
            FROM OPENXML(@IDOC, N'/message', 3) 
            WITH 
            (
              [id]              int,
              [type]            smallint,
              [app]             nvarchar(16),
              [src]             nvarchar(128),
              [ort]             nvarchar(16),
              [fromDate]        datetimeoffset(3) 'date/@from',
              [toDate]          datetimeoffset(3) 'date/@to'
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

 
    
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
    IF @MID IS NOT NULL 
    BEGIN
      SET @WHERE = ' [id] = @MID '
    END
    ELSE
    BEGIN

       
      SET @WHERE = ''
      
      IF @TYPE IS NOT NULL
        SET @WHERE = ' [state] = @STATE AND '
 
        
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@APP', @APP, '[app]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@ORT', @ORT, '[ort]')

 
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_DATE, N'@FROM_DATE', @TO_DATE, N'@TO_DATE', N'[date]')
           
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison(N'@SOURCE', @SOURCE, '[src]')
      
 
    END

    SET @SELECT = 'SELECT *, [v3d].[fn_getVersion](''Messages'') AS [ver]  FROM [v3d].[Messages] '
       
    DECLARE @LEN AS INT             
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
    BEGIN
      SET @SELECT = @SELECT + 'WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
    END

    DECLARE @PARAMETERS AS NVARCHAR(2000)

    SET @SELECT = 'SET @RESULTS = (' + @SELECT +  ' ORDER BY [date] ASC FOR XML RAW(''message''), TYPE, ROOT(''collection'')  ); SET @COUNT = @@ROWCOUNT;'

    
    SET @PARAMETERS = N'@RESULTS XML OUTPUT, ' +
                      N'@FROM_DATE DATETIMEOFFSET(3), ' + 
                      N'@TO_DATE DATETIMEOFFSET(3), ' +
                      N'@APP NVARCHAR(16), ' +
                      N'@TYPE SMALLINT, ' +
                       N'@SOURCE NVARCHAR(128), ' +
                      N'@ORT NVARCHAR(16), ' +
                      N'@COUNT INT OUTPUT, ' +
                      N'@MID AS INT '

   
    EXEC  sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_DATE            = @TO_DATE,
            @FROM_DATE          = @FROM_DATE,
            @APP                = @APP,
            @TYPE               = @TYPE,
            @SOURCE             = @SOURCE,
            @MID                = @MID,
            @ORT                = @ORT,
            @COUNT              = @RC OUTPUT,
            @RESULTS            = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL 
      SET @RESULTS = '<collection id="message" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"message"} into (/collection)[1]')   
   
      
    RETURN @RC

  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getMessage',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getMessage] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getMessage] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getNextJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the next job to be processed
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getNextJob]
(
@ID AS NVARCHAR(256),                 -- Context Id
@SYS AS NVARCHAR(32),                 -- System making the request
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @LOCK AS INT
  DECLARE @TRANSACTION AS INT

  RETRY:

  BEGIN TRY
  
    DECLARE @JID AS UNIQUEIDENTIFIER
    DECLARE @PENDING AS INT
    DECLARE @PROCESSING AS INT
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'

    IF @@TRANCOUNT = 0
    BEGIN
      SET @TRANSACTION = 1
      BEGIN TRANSACTION
    END  

 
    SELECT  @PROCESSING = [v3d].[fn_getState]('processing'),
            @PENDING = [v3d].[fn_getState]('pending')

    SELECT TOP(1) @JID = [id] FROM [v3d].[Jobs] WHERE  [state] = @PENDING ORDER BY [crdte]

    IF @@ROWCOUNT = 1
    BEGIN
      -- Update the JOB and get the GUID assigned after inserted    
      UPDATE [v3d].[Jobs]  SET [state] = @PROCESSING, [sys] = @SYS  WHERE [id] = @JID  

      DECLARE @QUERY AS XML

      SET @QUERY = '<job id="' + CONVERT(NVARCHAR(256),@JID) +'"/>'

      EXEC [v3d].[sp_getJob]
        @ID = @ID,
        @QUERY = @QUERY,
        @RESULTS = @RESULTS OUTPUT

        SET @RESULTS = @RESULTS.query('collection/job');

    END
    ELSE
      SET @RESULTS = '<job />'
    
          
 
    IF @TRANSACTION > 0
      COMMIT; 

  END TRY
  BEGIN CATCH


   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getNextJob',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getNextJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getNextJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getNextPacsJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 04/16/2012
-- Description:	Gets the next job to be processed
-- ===========================================================
CREATE PROCEDURE [v3d].[sp_getNextPacsJob]
(
@ID AS NVARCHAR(256),                 -- Context Id
@QUERY AS XML = NULL,                 -- Not used
@RESULTS AS XML OUTPUT                -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @LOCK AS INT
  DECLARE @TRANSCOUNT AS INT
    DECLARE @JID AS INT

  SELECT @TRANSCOUNT = @@TRANCOUNT


  BEGIN TRY
  
    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'
  

      -- Lock Dicom Receiving
    EXEC @LOCK = sp_getapplock 
              @Resource = 'GET_NEXT_PACS_JOB', 
              @LockMode = 'Exclusive', 
              @LockOwner = 'Transaction', 
              @LockTimeout = 60000
                
    --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
      -- Release lock
      EXEC sp_releaseapplock  
                @Resource = 'GET_NEXT_PACS_JOB'

      EXEC [master].[dbo].[sp_throwException] 901401,  'Pacs Queue'
        
    END -- END ... If the lock failed
 
    SELECT TOP(1) @JID = [id] FROM [v3d].[PacsJobs] WHERE [state] = 'Pending' ORDER BY [qdate]

    IF @@ROWCOUNT = 1
    BEGIN
      -- Update the JOB and get the GUID assigned after inserted    
      UPDATE [v3d].[PacsJobs]  SET [state] = 'Processing'  WHERE [id] = @JID  

      EXEC [v3d].[sp_getPacsJob]
        @ID = @ID,
        @JID = @JID,
        @RESULTS = @RESULTS OUTPUT

    END
    ELSE
      SET @RESULTS = '<pacsjob />'
    
          
    -- Release lock
    EXEC sp_releaseapplock  
              @Resource = 'GET_NEXT_PACS_JOB'

    IF @TRANSCOUNT > 0
      COMMIT TRANSACTION 

  END TRY
  BEGIN CATCH

    -- Release lock
    EXEC sp_releaseapplock  
              @Resource = 'GET_NEXT_PACS_JOB'

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION 

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_getNextPacsJob'
    


  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_getNextPacsJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getNextPacsJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getPacsJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Gets the pacs queue entries
-- =============================================
CREATE PROCEDURE [v3d].[sp_getPacsJob]
(
@ID         NVARCHAR(256),              -- Context ID
@JID        INT = NULL,                 -- Job Id
@QUERY      XML = '<pacsjob />',        -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
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

          
    DECLARE @IDOC AS INT


    DECLARE @OP AS SMALLINT
    DECLARE @STATE AS SMALLINT
    DECLARE @HOST AS NVARCHAR(1000)
    DECLARE @FROM_QDATE        AS DATETIMEOFFSET(7)
    DECLARE @TO_QDATE          AS DATETIMEOFFSET(7)


    DECLARE @SERIES AS XML

    
    IF @JID IS NULL 
    BEGIN
     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      -- Extract the user information 
      SELECT  @JID        = [id],
              @OP         = [op],
              @STATE      = [state],
              @HOST       = [host],
              @FROM_QDATE  = [fromQDate],
              @TO_QDATE    = [toQDate]
            FROM OPENXML(@IDOC, N'/pacsjob', 1) WITH 
            ( 
              [id]             int,
              [op]             smallint,
              [state]          smallint,
              [host]           nvarchar(1000),
              [fromQDate]      datetimeoffset(7) 'qdate/@from',
              [toQDate]        datetimeoffset(7) 'qdate/@to'
            )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    END
 
    DECLARE @WHERE AS NVARCHAR(1000)
    
    SET @WHERE = ''
    
    IF @JID IS NOT NULL
    BEGIN   
    
      SET @WRAP = 0
      
      SET @WHERE = ' [id] = @JID AND '
      
    END
    ELSE
    BEGIN
        
      IF @STATE IS NOT NULL
        SET @WHERE = ' [state] = @STATE AND '

      IF @OP IS NOT NULL
         SET @WHERE = ' [op] = @OP AND '
      
      SET @WHERE = @WHERE + [System].v3d.fn_buildStringComparison('@HOST', @HOST, '[host]')

      SET @WHERE = @WHERE + [System].v3d.fn_buildDateComparison(@FROM_QDATE, N'@FROM_QDATE', @TO_QDATE, N'@TO_QDATE', N'[qdate]')
      
    END
    
    DECLARE @SELECT NVARCHAR(1000)  
    
    SET @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''PacsJobs'') AS [ver]  FROM [v3d].[PacsJobs] '

    DECLARE @LEN AS INT
     
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      
     IF @WRAP = 1
      SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''job''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    ELSE
      SET @SELECT =  @SELECT + @WHERE + N' FOR XML RAW(''job'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS      XML OUTPUT, ' +
                      N'@FROM_QDATE    DATETIMEOFFSET(7), ' +
                      N'@TO_QDATE      DATETIMEOFFSET(7), ' +
                      N'@JID          INT, ' +
                      N'@HOST         NVARCHAR(1000), ' +
                      N'@STATE        SMALLINT, '  +
                      N'@OP           SMALLINT, ' +
                      N'@COUNT        INT OUTPUT '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_QDATE          = @TO_QDATE,
            @FROM_QDATE        = @FROM_QDATE,
            @JID              = @JID,
            @STATE            = @STATE,
            @OP               = @OP,
            @HOST             = @HOST,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="pacsjob" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"pacsjob"} into (/collection)[1]')   
   
    RETURN @RC
   
  END TRY
  BEGIN CATCH
     
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_getPacsJobs'

  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getPacsJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getPacsJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getReceivePaths]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================================
-- Author:		  Mark Kontak
-- Create date: 10/23/2013
-- Description:	Determines the DICOM_FILEPATH and THUMNAIL_FILEPATH
-- ===================================================================
CREATE PROCEDURE [v3d].[sp_getReceivePaths]
@SERIES_UID NVARCHAR(128),                    -- Series UID
@INSTANCE_UID NVARCHAR(128),                  -- Instance UID
@CREATE_DIRECTORY BIT,                        -- Flag to indicate that the dicom directory is to be created
@DICOM NVARCHAR(1024)  OUTPUT,                -- DICOM file name with path
@THUMBNAIL NVARCHAR(1024) OUTPUT              -- Thumbnail file name with path
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC           AS INT
  DECLARE @SERIES_PATH  AS NVARCHAR(512)
  DECLARE @DICOM_PATH   AS NVARCHAR(512)
  DECLARE @FILENAME     AS NVARCHAR(256)
    
    
  SELECT  @RC = 0,
          @DICOM = '',
          @THUMBNAIL = ''
     
  -- =================================================
  -- Series path (local since we are on the server)
  -- =================================================    
  SELECT @SERIES_PATH = [System].[v3d].[fn_pathCombine]( [v3d].[fn_getRoot]('local', [v3d].[Series].[st]), [v3d].[Studies].[dir] , [v3d].[Series].[dir] )
        FROM [v3d].[Series] 
          INNER JOIN [v3d].[Studies] ON [v3d].[Series].[suid] = [v3d].[Studies].[uid]
            WHERE [v3d].[Series].[uid] = @SERIES_UID
  
  -- ======================================================================================
  -- Build the DICOM path by just adding a sub-directory of dicom off the series directory
  -- ======================================================================================
  SET @DICOM_PATH = [System].[v3d].[fn_pathCombine](@SERIES_PATH,'dicom','')

  -- ====================================================================================================
  -- Dicom Image files name is the istance uid. It is done here so that the filename can be inserted 
  -- into the database.
  -- ====================================================================================================
  SET @FILENAME = REPLACE(@INSTANCE_UID, '.', '_') + '.dcm'

  -- ===========================================================
  -- Set the images file path to the dicom path created above
  -- ===========================================================
  SET @DICOM = [System].[v3d].[fn_pathCombine](@DICOM_PATH, @FILENAME,'')

  DECLARE @FILE_EXISTS AS INT

  -- ====================================
  -- Check if the file already exists
  -- ====================================
  EXEC @FILE_EXISTS = [v3d].[sp_fileExists] 
                            @FILEPATH = @DICOM

  -- =========================================================================================
  -- If the file does not exists then it is not a duplicate and we need to perform additional 
  -- operations. 
  -- ========================================================================================
  IF @FILE_EXISTS = 0
  BEGIN

    SET @RC = 1

    -- ========================================================================================
    -- Only set the thumnail path and create the dicom sub-directory if the series was added.
    -- ========================================================================================
    IF @CREATE_DIRECTORY = 1
    BEGIN

      -- ===============================
      -- Create the dicom sub-driectory 
      -- ===============================
      EXEC [v3d].[sp_createDirectory] 
            @DIRECTORY = @DICOM_PATH

      -- =========================================
      -- Set the thumnail path to the series path 
      -- =========================================
      SET @THUMBNAIL = [System].[v3d].[fn_pathCombine](@SERIES_PATH, REPLACE(@SERIES_UID,'.','_') + '.png','');

    END

  END   -- END if the image file does not exist

  RETURN @RC

END



GO
GRANT EXECUTE ON [v3d].[sp_getReceivePaths] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getReceivePaths] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getReceiver]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Gets the pacs queue entries
-- =============================================
CREATE PROCEDURE [v3d].[sp_getReceiver]
(
@ID         NVARCHAR(256),              -- Context ID
@QUERY      XML = '<receiver />',       -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
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

    DECLARE @AETITLE AS NVARCHAR(32)
    DECLARE @ASSOCIATION AS NVARCHAR(256)
    DECLARE @SERVER AS NVARCHAR(32)
    DECLARE @UID AS NVARCHAR(128)
    DECLARE @NAME AS NVARCHAR(128)

    DECLARE @SERIES AS XML


 

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @AETITLE        = [aetitle],
            @ASSOCIATION    = [association],
            @UID            = [uid],
            @SERVER         = [server],
            @NAME           = [name]
          FROM OPENXML(@IDOC, N'/receiver', 1) WITH 
          ( 
            [aetitle]       nvarchar(32),
            [association]   nvarchar(256),
            [uid]           nvarchar(128),
            [server]        nvarchar(32),
            [name]          nvarchar(128)
           
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    DECLARE @SELECT NVARCHAR(MAX)   
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @LEN AS INT
  
    SELECT @WHERE = [System].v3d.fn_buildStringComparison('@UID', @UID, '[uid]') +
                    [System].v3d.fn_buildStringComparison('@NAME', @NAME, '[name]') +
                    [System].v3d.fn_buildStringComparison('@SERVER', @SERVER, '[server]') +
                    [System].v3d.fn_buildStringComparison('@AETITLE', @AETITLE, '[aetitle]') +
                    [System].v3d.fn_buildStringComparison('@ASSOCIATION', @ASSOCIATION, '[association]'),
           @SELECT =   N'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Receivers'') AS [ver]  FROM [v3d].[Receivers] '


    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)

    
     
    SELECT @SELECT = @SELECT + @WHERE + 
                    N' FOR XML RAW(''receiver''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS      XML OUTPUT, ' +
                      N'@AETITLE      NVARCHAR(32), ' +
                      N'@ASSOCIATION  NVARCHAR(256), ' +
                      N'@NAME         NVARCHAR(128), ' +
                      N'@SERVER       NVARCHAR(32), ' +
                      N'@UID          NVARCHAR(128), '  +
                      N'@COUNT        INT OUTPUT '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @AETITLE          = @AETITLE,
            @ASSOCIATION      = @ASSOCIATION,
            @NAME             = @NAME,
            @SERVER           = @SERVER,
            @UID              = @UID,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="receiver" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"receiver"} into (/collection)[1]')   
  
    RETURN @RC
   
  END TRY
  BEGIN CATCH
     
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getReceiver'
  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getReceiver] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getReceiver] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getReference]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 07/22/2011
-- Description:	Returns the refernced series list
-- =================================================
CREATE PROCEDURE [v3d].[sp_getReference]
(
@ID         NVARCHAR(256),              -- Context ID
@UID        NVARCHAR(128) = NULL,       -- Series UID to retrieve
@QUERY      XML = '<reference />',      -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @WRAP AS BIT
  DECLARE @RC AS INT

  SELECT @WRAP = 1


  BEGIN TRY

     
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    --============================================================================================================
    -- If the UID was specified then we are ignoring the query if present and processing as a retrieval as a 
    -- single STUDY item. Otherwise the query is processed and an entore collection is returned.
    --============================================================================================================
    IF @UID IS NULL 
    BEGIN
    
      SET @WRAP = 1
      
      DECLARE @IDOC AS INT

      DECLARE @ORIENTATION      AS NVARCHAR(512)
      DECLARE @PLANE            AS TINYINT
      DECLARE @FROM_DATE        AS DATETIMEOFFSET(7)
      DECLARE @TO_DATE          AS DATETIMEOFFSET(7)
      DECLARE @MODALITY         AS NVARCHAR(512)
      DECLARE @APP              AS NVARCHAR(512)
      DECLARE @TYPE             AS INT
      DECLARE @DESCRIPTION      AS NVARCHAR(512)
      DECLARE @FROM_IMAGE_COUNT AS INT
      DECLARE @TO_IMAGE_COUNT   AS INT
      DECLARE @STATE            AS INT
      DECLARE @SOURCE           AS NVARCHAR(512)
      DECLARE @NUMBER           AS NVARCHAR(64)
      DECLARE @SERIES_UID       AS NVARCHAR(1000)
      DECLARE @STUDY_UID        AS NVARCHAR(256)

      DECLARE @SERIES AS XML

     
      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

      -- Extract the user information 
      SELECT  @SERIES_UID       = [uid],
              @STUDY_UID        = [suid],
              @ORIENTATION      = [ort],
              @PLANE            = [pln],
              @TYPE             = [type],
              @FROM_DATE        = [fromDate],
              @TO_DATE          = [toDate],
              @MODALITY         = [mod],
              @APP              = [app],
              @STATE            = [state],
              @SOURCE           = [src],
              @DESCRIPTION      = [desc],
              @FROM_IMAGE_COUNT = [fromImageCount],
              @TO_IMAGE_COUNT   = [toImageCount],
              @NUMBER           = [num]
            FROM OPENXML(@IDOC, N'/reference', 1) WITH 
            ( 
              [uid]             nvarchar(1000),
              [suid]            nvarchar(1000),
              [num]             nvarchar(512),
              [fromDate]        datetimeoffset(7) 'date/@from',
              [toDate]          datetimeoffset(7) 'date/@to',
              [ort]             nvarchar(512),
              [mod]             nvarchar(512),
              [desc]            nvarchar(512),
              [type]            int,
              [fromImageCount]  int 'icnt/@from',
              [toImageCount]    int 'icnt/@to',
              [state]           int,
              [src]             nvarchar(512),
              [app]             nvarchar(512),
              [pln]             tinyint
           )
         
        -- Remove the document
        EXEC sp_xml_removedocument @IDOC

    END -- END If @LOGIN IS NULL


    DECLARE @WHERE AS NVARCHAR(1000)
    
    SET @WHERE = ''
    
    IF @UID IS NOT NULL
    BEGIN
    
      -- ==================================================
      -- Make sure the series exists 
      -- (900990) Series: Not Found
      -- ==================================================
      IF NOT EXISTS ( SELECT * FROM [v3d].[Series] WHERE [uid] = @UID ) 
        EXEC [master].[dbo].[sp_throwNotFoundException] 'Series'
    
    
      SET @WRAP = 0
      
      SET @WHERE = ' [uid] = @UID AND '
      
    END
    ELSE
    BEGIN
    
    
      SET @UID = @SERIES_UID

      IF @TYPE IS NOT NULL
        SET @WHERE = @WHERE + ' ( [type] & @TYPE ) = [type] ) AND '
      
      SELECT @WHERE = [System].v3d.fn_buildStringComparison(N'@UID', @UID, '[uid]') +
                      [System].v3d.fn_buildStringComparison(N'@STUDY_UID', @STUDY_UID, '[suid]') +
                      [System].v3d.fn_buildDateComparison(@FROM_DATE, N'@FROM_DATE', @TO_DATE, N'@TO_DATE', N'[date]') +
                      [System].v3d.fn_buildStringComparison(N'@ORIENTATION', @ORIENTATION, N'[ort]') +
                      [System].v3d.fn_buildStringComparison(N'@SOURCE', @SOURCE, '[src]') +
                      [System].v3d.fn_buildStringComparison(N'@MODALITY', @MODALITY, '[mod]') +
                      [System].v3d.fn_buildStringComparison(N'@DESCRIPTION', @DESCRIPTION, '[desc]') +
                      [System].v3d.fn_buildStringComparison(N'@APP', @APP, '[app]') +
                      [System].v3d.fn_buildStringComparison(N'@NUMBER', @NUMBER, '[num]') +
                      [System].v3d.fn_buildIntegerComparison(@FROM_IMAGE_COUNT, N'@FROM_IMAGE_COUNT', @TO_IMAGE_COUNT, N'@TO_IMAGE_COUNT', N'[icnt]')
          
              
       IF @STATE IS NOT NULL
          SET @WHERE = @WHERE + ' ( [state] & @@STATE ) > 0 AND '
     
      IF @PLANE IS NOT NULL
          SET @WHERE = @WHERE + ' ( [pln] & @PLANE ) > 0 AND '

    END
    
    DECLARE @SELECT NVARCHAR(1000)  
    DECLARE @LEN AS INT
     
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      

    IF @WRAP = 1
      SET @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Series'') AS [ver]  FROM [v3d].[ReferenceSeries] ' + 
                        @WHERE + N' FOR XML RAW(''series''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
    ELSE
      SET @SELECT = 'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Series'') AS [ver]  FROM [v3d].[ReferenceSeries] ' + 
                        @WHERE + N' FOR XML RAW(''series'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                      N'@FROM_DATE          DATETIMEOFFSET(7), ' +
                      N'@TO_DATE            DATETIMEOFFSET(7), ' +
                      N'@FROM_IMAGE_COUNT   INT, ' +
                      N'@TO_IMAGE_COUNT     INT, '  +
                      N'@NUMBER             NVARCHAR(512), ' +
                      N'@ORIENTATION        NVARCHAR(512), '  +
                      N'@TYPE               INT, ' +
                      N'@MODALITY           NVARCHAR(512), ' +
                      N'@DESCRIPTION        NVARCHAR(255), ' + 
                      N'@STUDY_UID          NVARCHAR(128), ' + 
                      N'@STATE              INT, ' + 
                      N'@PLANE              TINYINT, ' + 
                      N'@UID                NVARCHAR(1000), ' +
                      N'@COUNT              INT OUTPUT, ' +
                      N'@SOURCE             NVARCHAR(512), ' +
                      N'@APP                NVARCHAR(512) '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_DATE          = @TO_DATE,
            @FROM_DATE        = @FROM_DATE,
            @FROM_IMAGE_COUNT = @FROM_IMAGE_COUNT,
            @TO_IMAGE_COUNT   = @TO_IMAGE_COUNT,
            @NUMBER           = @NUMBER,
            @ORIENTATION      = @ORIENTATION,
            @PLANE            = @PLANE,
            @SOURCE           = @SOURCE,
            @STATE            = @STATE,
            @DESCRIPTION      = @DESCRIPTION,
            @APP              = @APP,
            @MODALITY         = @MODALITY,
            @STUDY_UID        = @STUDY_UID,
            @TYPE             = @TYPE,
            @UID              = @UID,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
   IF @RESULTS IS NULL
    SET @RESULTS = '<collection id="reference" />'
   ELSE
    SET @RESULTS.modify('insert attribute id {"reference"} into (/collection)[1]')   

   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getReference'
  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getReference] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getReference] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 6/10/2010
-- Description:	Get the series epcified
--
--          uid                 uid
--          mod                 modality
--          nm                  number
--          desc                description
--          orient              orientation
--          type                type
--          imageCount          icnt
--          src                 source
--          pln                 plane
-- =============================================
CREATE PROCEDURE [v3d].[sp_getSeries]
(
@ID         NVARCHAR(256),              -- Context ID
@QUERY      XML = '<series />',         -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @RC AS INT

  RETRY:


  BEGIN TRY
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    
   
    DECLARE @IDOC AS INT

    DECLARE @ORIENTATION      AS NVARCHAR(512)
    DECLARE @PLANE            AS TINYINT
    DECLARE @FROM_DATE        AS DATETIMEOFFSET(7)
    DECLARE @TO_DATE          AS DATETIMEOFFSET(7)
    DECLARE @MODALITY         AS NVARCHAR(512)
    DECLARE @APP              AS NVARCHAR(512)
    DECLARE @TYPE             AS NVARCHAR(512)
    DECLARE @DESCRIPTION      AS NVARCHAR(512)
    DECLARE @STUDY_UID        AS NVARCHAR(MAX)
    DECLARE @UID              AS NVARCHAR(MAX)
    DECLARE @FROM_IMAGE_COUNT AS INT
    DECLARE @TO_IMAGE_COUNT   AS INT
    DECLARE @STATE            AS INT
    DECLARE @SOURCE           AS NVARCHAR(512)
    DECLARE @NUMBER           AS NVARCHAR(64)
    DECLARE @STORE            AS NVARCHAR(32)
    

    DECLARE @SERIES AS XML

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @UID              = [uid],
            @STUDY_UID        = [suid],
            @ORIENTATION      = [ort],
            @PLANE            = [pln],
            @TYPE             = [type],
            @FROM_DATE        = [fromDate],
            @TO_DATE          = [toDate],
            @MODALITY         = [mod],
            @APP              = [app],
            @STATE            = [state],
            @SOURCE           = [src],
            @DESCRIPTION      = [desc],
            @STORE            = [st],
            @FROM_IMAGE_COUNT = [fromImageCount],
            @TO_IMAGE_COUNT   = [toImageCount],
            @NUMBER           = [num]
          FROM OPENXML(@IDOC, N'/series', 1) WITH 
          ( 
            [uid]             nvarchar(1000),
            [suid]            nvarchar(1000),
            [num]             nvarchar(512),
            [fromDate]        datetimeoffset(7) 'date/@from',
            [toDate]          datetimeoffset(7) 'date/@to',
            [ort]             nvarchar(512),
            [mod]             nvarchar(512),
            [desc]            nvarchar(512),
            [type]            nvarchar(512),
            [st]              nvarchar(32),
            [fromImageCount]  int 'icnt/@from',
            [toImageCount]    int 'icnt/@to',
            [state]           int,
            [src]             nvarchar(512),
            [app]             nvarchar(512),
            [pln]             tinyint
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC


    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT NVARCHAR(MAX)  
    DECLARE @ORDER NVARCHAR(MAX)
    DECLARE @STUDY_UID_LEN INT

    SELECT @ORDER = '',
           @STUDY_UID_LEN = LEN(@STUDY_UID)
          
    -- =================================================================
    -- If only 1 study uid was supplied then order the series byt date
    -- ==================================================================
    IF  @STUDY_UID_LEN > 0 AND @STUDY_UID_LEN < 128
      SET @ORDER = ' ORDER BY [v3d].[Series].[date] ASC '

    IF @TYPE IS NOT NULL
      SET @WHERE = @WHERE + ' ( [type] & @TYPE ) = [type] AND '
    
    SELECT @WHERE = [System].v3d.fn_buildStringComparison(N'@UID', @UID, '[uid]') +
                    [System].v3d.fn_buildStringComparison(N'@STUDY_UID', @STUDY_UID, '[suid]') +
                    [System].v3d.fn_buildStringComparison(N'@STORE', @STORE, '[st]') +
                    [System].v3d.fn_buildDateComparison(@FROM_DATE, N'@FROM_DATE', @TO_DATE, N'@TO_DATE', N'[date]') +
                    [System].v3d.fn_buildStringComparison(N'@ORIENTATION', @ORIENTATION, N'[ort]') +
                    [System].v3d.fn_buildStringComparison(N'@SOURCE', @SOURCE, '[src]') +
                    [System].v3d.fn_buildStringComparison(N'@MODALITY', @MODALITY, '[mod]') +
                    [System].v3d.fn_buildStringComparison(N'@DESCRIPTION', @DESCRIPTION, '[desc]') +
                    [System].v3d.fn_buildStringComparison(N'@APP', @APP, '[app]') +
                    [System].v3d.fn_buildStringComparison(N'@NUMBER', @NUMBER, '[num]') +
                    [System].v3d.fn_buildIntegerComparison(@FROM_IMAGE_COUNT, N'@FROM_IMAGE_COUNT', @TO_IMAGE_COUNT, N'@TO_IMAGE_COUNT', N'[icnt]'),
         @SELECT = 'SET @RESULTS = ( SELECT [uid], [suid], [ort], [pln], [date], [dir], [mod], [rcv], [desc],  [type], [msg], [app], [icnt], [num], [src], [state], [on], [st], [proc], [ext], [v3d].[fn_getVersion](''Series'') AS [ver], ' +
                   ' ( SELECT [uid], [cid], [name], [date], [app], [icnt], [file], [ext], [v3d].[fn_getVersion](''Volumes'') AS [ver]  FROM [v3d].[Volumes] WHERE [v3d].[Volumes].[uid] = [v3d].[Series].[uid] FOR XML RAW(''volume''), TYPE, ROOT(''volumes'') ),  ' +
                   ' ( SELECT [uid], [suid], [ort], [pln], [date], [dir], [mod], [rcv], [desc], [type], [app], [icnt], [num], [src], [state], [on], [st], [proc], [ext], [v3d].[fn_getVersion](''Series'') AS [ver]  FROM [v3d].[ReferenceSeries]  ' +
                   ' FOR XML RAW(''series''), TYPE, ROOT(''references'') ) FROM [v3d].[Series] '
 
              
    IF @STATE IS NOT NULL
      SET @WHERE = @WHERE + ' ( [state] & @STATE ) > 0  AMD '
              
    IF @PLANE IS NOT NULL
        SET @WHERE = @WHERE + ' ( [pln] & @PLANE ) > 0 AND '

    DECLARE @LEN AS INT
     
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      
    SET @SELECT =  @SELECT + @WHERE +  + @ORDER + N' FOR XML RAW(''series''), TYPE, ROOT(''collection'') ); SET @COUNT = @@ROWCOUNT;'
   
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = N'@RESULTS            XML OUTPUT, ' +
                      N'@FROM_DATE          DATETIMEOFFSET(7), ' +
                      N'@TO_DATE            DATETIMEOFFSET(7), ' +
                      N'@FROM_IMAGE_COUNT   INT, ' +
                      N'@TO_IMAGE_COUNT     INT, '  +
                      N'@NUMBER             NVARCHAR(512), ' +
                      N'@ORIENTATION        NVARCHAR(512), '  +
                      N'@TYPE               INT, ' +
                      N'@MODALITY           NVARCHAR(512), ' +
                      N'@DESCRIPTION        NVARCHAR(255), ' + 
                      N'@STUDY_UID          NVARCHAR(128), ' + 
                      N'@STATE              INT, ' + 
                      N'@PLANE              TINYINT, ' + 
                      N'@STORE              NVARCHAR(32), ' +
                      N'@UID                NVARCHAR(1000), ' +
                      N'@COUNT              INT OUTPUT, ' +
                      N'@SOURCE             NVARCHAR(512), ' +
                      N'@APP                NVARCHAR(512) '

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            @TO_DATE          = @TO_DATE,
            @FROM_DATE        = @FROM_DATE,
            @FROM_IMAGE_COUNT = @FROM_IMAGE_COUNT,
            @TO_IMAGE_COUNT   = @TO_IMAGE_COUNT,
            @NUMBER           = @NUMBER,
            @ORIENTATION      = @ORIENTATION,
            @PLANE            = @PLANE,
            @SOURCE           = @SOURCE,
            @STATE            = @STATE,
            @STORE            = @STORE,
            @DESCRIPTION      = @DESCRIPTION,
            @APP              = @APP,
            @MODALITY         = @MODALITY,
            @STUDY_UID        = @STUDY_UID,
            @TYPE             = @TYPE,
            @UID              = @UID,
            @COUNT            = @RC OUTPUT,
            @RESULTS          = @RESULTS OUTPUT
      
   IF @RESULTS IS NULL
    SET @RESULTS = '<collection id="series" />'
   ELSE
    SET @RESULTS.modify('insert attribute id {"series"} into (/collection)[1]')   


   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getSeries',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY
  
  END CATCH


END






GO
GRANT EXECUTE ON [v3d].[sp_getSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getSeriesLaunchList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/20/2013
-- Description:	Generates a launch list using the specified series list and launch rules.
-- ===================================================================================
CREATE PROCEDURE [v3d].[sp_getSeriesLaunchList]
@ID           NVARCHAR(256),            -- Context Id
@LOCATION     NVARCHAR(64) = 'remote',  -- Store location remote opr local
@SESSION_UID  NVARCHAR(128),            -- Session (created in sp_getLaunchlist)
@UIDS         NVARCHAR(MAX),            -- Series list
@RULES        XML,                      -- Application launch rule
@RESULTS      XML OUTPUT                -- Session Launch List Results
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON


  BEGIN TRY


    DECLARE @IDX      AS INT
    DECLARE @CID      AS NVARCHAR(32)
    DECLARE @ORT      AS NVARCHAR(32)
    DECLARE @TYPE     AS INT
    DECLARE @UID      AS NVARCHAR(128)
    DECLARE @RUID     AS NVARCHAR(128)
    DECLARE @PATH     AS NVARCHAR(MAX)
    DECLARE @XML      AS XML
    DECLARE @APP      AS NVARCHAR(32)
    DECLARE @STATE    AS BIGINT
    DECLARE @DICOM    AS INT
    DECLARE @SESSION  AS INT
 
    -- ========================
    -- Launch Rule Variables
    -- ========================
    DECLARE @SESSION_SELECTION  AS NVARCHAR(16)       -- Session selection setting
    DECLARE @DICOM_SELECTION    AS NVARCHAR(16)       -- Dicom series selection setting
    DECLARE @ORIENTATIONS       AS NVARCHAR(1024)     -- Delimited 
    DECLARE @SERIES_TYPES       AS INT                -- Mask of series types
    DECLARE @MAXIMUM_DATASETS   AS INT                -- Max datasets allowed for the application      

    DECLARE @IDOC               AS INT

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RULES

    -- ==================================
    -- Extract the launch rule information
    -- ==================================
    SELECT  @APP                = [app],
            @MAXIMUM_DATASETS   = [max],
            @ORIENTATIONS       = [ort],
            @SERIES_TYPES       = [type],
            @SESSION_SELECTION  = [session],
            @DICOM_SELECTION    = [dicom]
            FROM OPENXML(@IDOC, N'/rule', 1) WITH 
          ( 
            [app]             nvarchar(32),
            [max]             int,
            [ort]             nvarchar(512),
            [type]            int,
            [session]         nvarchar(16),
            [dicom]           nvarchar(16)
          )
         
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- Set variables
    SELECT @IDX                = 0,
           @SESSION            = [System].[v3d].[fn_getMaskBit]('type','session'),
           @DICOM              = [System].[v3d].[fn_getMaskBit]('type','dicom'),
           @MAXIMUM_DATASETS   = ISNULL(@MAXIMUM_DATASETS,100),
           @ORIENTATIONS       = ISNULL(@ORIENTATIONS,''),
           @SERIES_TYPES       = ISNULL(@SERIES_TYPES,0),
           @SESSION_SELECTION  = ISNULL(@SESSION_SELECTION, 'LIFO' ),
           @DICOM_SELECTION    = ISNULL(@DICOM_SELECTION, 'FIFO' )

  
 


    DECLARE @SERIES_TABLE AS TABLE (  [id] INT IDENTITY(1,1) NOT NULL, [uid] NVARCHAR(128) NOT NULL, [ruid] NVARCHAR(128) DEFAULT '', [cid] NVARCHAR(32) )


    INSERT INTO @SERIES_TABLE ( [uid], [ruid], [cid] ) ( SELECT     SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE, 
                                                                    SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE2,
                                                                    [v3d].[fn_getVolumeConversionId](SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1)) 
      FROM master..spt_values
      WHERE Type = 'p'
            AND Number <= LEN(@UIDS) - 1
            AND SUBSTRING(@UIDS, Number, 1) = '|' ) 

  
  
    DECLARE SERIES_CURSOR CURSOR LOCAL
        FOR SELECT [id], [uid], [ruid], [cid] FROM @SERIES_TABLE


    OPEN SERIES_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @RUID, @CID


    WHILE @@FETCH_STATUS = 0
    BEGIN

     

      SELECT @TYPE = [type], @STATE = [state] FROM [v3d].[Series] WHERE [uid] = @UID


      -- =========================================================================================
      -- Make sure that the series is not itself is not in a failed state and any refernces or 
      -- dependents.
      -- ========================================================================================
      IF ( [System].[v3d].[fn_isMaskBitSet](@STATE, 'state', 'completed') = 0 OR
           EXISTS ( SELECT * FROM [v3d].[ReferenceSeries] WHERE [uid] = @UID AND [System].[v3d].[fn_isMaskBitSet]([state], 'state', 'completed') = 0 ) OR
           EXISTS ( SELECT * FROM [v3d].[DependentSeries] WHERE [uid] = @UID AND [System].[v3d].[fn_isMaskBitSet]([state], 'state', 'completed') = 0 ) )
        EXEC [master].[dbo].[sp_throwException] 901434, @UID


      IF @TYPE = @DICOM
      BEGIN

        DECLARE @UPDATED AS BIT
        DECLARE @VCID AS NVARCHAR(32)
        DECLARE @VUID AS NVARCHAR(128)
        DECLARE @VTYPE AS INT
        DECLARE @VSTATE AS BIGINT

        SELECT @UPDATED = 0

        -- ============================================================
        -- Make sure the series has been processed for the application
        -- ============================================================
        IF ( @TYPE & @SERIES_TYPES ) <> @TYPE  AND
           NOT EXISTS ( SELECT * FROM [v3d].[DependentSeries] WHERE [uid] = @UID AND ( [type] & @SERIES_TYPES ) = [type] )
          EXEC [master].[dbo].[sp_throwException] 901437, @APP

        DECLARE VOLUME_REFERENCE_CURSOR CURSOR LOCAL FOR SELECT [uid], [type], [state], [cid]
            FROM [v3d].[VolumeReferencedSeries] 
            WHERE [duid] = @UID 
              AND ( [type] & @SERIES_TYPES ) = [type] 
            ORDER BY [date] DESC

        OPEN VOLUME_REFERENCE_CURSOR

        FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

        WHILE @@FETCH_STATUS = 0 AND @UPDATED = 0
        BEGIN

   
            -- ============================================================================================
            -- If the series not in completed state 
            -- ============================================================================================
            IF [System].[v3d].[fn_isMaskBitSet](@VSTATE, 'state', 'completed') = 0
            BEGIN
               EXEC [master].[dbo].[sp_throwException] 901434, @VUID  
            END -- END ... If the series is in a failed state
            ELSE
            BEGIN

              -- Only update the series table's CID (Conversion ID) if it has not already been updated
              UPDATE @SERIES_TABLE SET [cid] = @VCID WHERE [uid] = @UID 

              SELECT @UPDATED = 1

            END
            
            FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

        END

        -- Close the CURSOR
        CLOSE VOLUME_REFERENCE_CURSOR
       
        -- Deallocate the resources
        DEALLOCATE VOLUME_REFERENCE_CURSOR

        EXEC [v3d].[sp_createReferences]
            @DICOM = @UID,
            @SESSION = @SESSION_UID,
            @CID = @VCID
 
      END -- END ... If the series is a DICOM

      -- ================================
      -- Else the series is not a DICOM
      -- ================================
      ELSE
      BEGIN
        
        -- Creates the references for the sesion to the series being launched
        EXEC [v3d].[sp_createSeriesReferences] @SESSION_UID, @UID

        SELECT @RUID = [ruid] FROM [v3d].[ReferenceSeries] WHERE [uid] = @UID  AND [type] = @DICOM 

        IF @@ROWCOUNT > 0
        BEGIN
        
          -- ==========================================================
          -- Cannot luanch two series linked to the same dicom series
          -- ==========================================================
          IF EXISTS ( SELECT * FROM @SERIES_TABLE WHERE [uid] = @RUID )
            EXEC [master].[dbo].[sp_throwException] 901435

          UPDATE @SERIES_TABLE  SET [ruid] = @UID, [uid] = @RUID WHERE [uid] = @UID
        END

      END

 
      -- fetch the first UID
      FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @RUID, @CID

    END
 
    CLOSE SERIES_CURSOR

    -- ==================================================================
    -- Do not DEALLOCATE because it is reopened and used again below
    -- ==================================================================
    -- DEALLOCATE SERIES_CURSOR
    -- ==================================================================

    DECLARE @TRANSID NVARCHAR(256)
    SET @TRANSID = @SESSION_UID

    --- Locks the session
    EXEC [v3d].[sp_lockSeries]
      @ID      = @ID,
      @UID     = @SESSION_UID,
      @TRANSID = @TRANSID OUTPUT

    -- Set the initial results 
    SET @RESULTS = ( SELECT [uid], @TRANSID as [tid], [v3d].[fn_getSeriesPath](@LOCATION,[v3d].[Series].[uid]) AS [url] FROM [v3d].[Series] WHERE [uid] = @SESSION_UID FOR XML RAW('session') )
 
    OPEN SERIES_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @RUID, @CID

    WHILE @@FETCH_STATUS = 0
    BEGIN

      
      SELECT @PATH = [v3d].[fn_getSeriesPath](@LOCATION,@UID) 
    
      SET @XML = ( SELECT LOWER([ort]) AS [ort], [uid], @PATH AS [url],
                          ( SELECT [cid], [name], [System].[v3d].[fn_pathCombine](@PATH,'volumes',[cid]) AS [url]
                                FROM [v3d].[Volumes] NOLOCK
                                WHERE [uid] = @UID AND 
                                      [cid] = @CID 
                                ORDER BY [date] ASC
                                FOR XML RAW('volume'), TYPE ),
                          [v3d].[fn_getDatasetSeries](@RUID, @SESSION_UID, @LOCATION)   
                    FROM [v3d].[Series] NOLOCK
                    WHERE [uid] = @UID  
                      FOR XML RAW('dataset'), TYPE )
      
      SET @RESULTS.modify('insert sql:variable("@XML") into (/session)[1]')

      -- fetch the first UID
      FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @RUID, @CID

    END

    CLOSE SERIES_CURSOR

    DEALLOCATE SERIES_CURSOR



  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler] 
      @PROCEDURE = 'sp_getSeriesLuanchList'

  END CATCH

END



GO
/****** Object:  StoredProcedure [v3d].[sp_getSeriesPath]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 03/30/2012
-- Description: Gets the series path 
-- =============================================
CREATE PROCEDURE [v3d].[sp_getSeriesPath]
(
@ID         NVARCHAR(256),              -- Context ID
@QUERY      XML = '<seriespath />',     -- Query xml
@RESULTS    XML OUTPUT                  -- Result set
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC AS INT
  DECLARE @PROCEDURE AS NVARCHAR(128)

  SELECT @PROCEDURE = OBJECT_NAME(@@PROCID)

  BEGIN TRY

     
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
       @ID = @ID,
      @MASK = 'StorageManagement'

     

    
    DECLARE @UID AS NVARCHAR(128)
    DECLARE @LOC AS NVARCHAR(32)
    DECLARE @IDOC AS INT
   
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- Extract the user information 
    SELECT  @UID    = [uid],
            @LOC    = [loc]
          FROM OPENXML(@IDOC, N'/seriespath', 1) WITH 
          ( 
            [uid]           nvarchar(256),
            [loc]          nvarchar(32)
         )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    -- Remote is the default
    SET @LOC = ISNULL(@LOC,'remote')

    
    IF @UID IS NOT NULL
      SET @RESULTS = ( SELECT [uid], [v3d].[fn_getSeriesPath](@LOC, [uid]) AS [path] FROM [v3d].[Series] WHERE [uid] = @UID FOR XML RAW('seriespath'), TYPE )
    ELSE
      SET @RESULTS = ( SELECT [uid], [v3d].[fn_getSeriesPath](@LOC, [uid]) AS [path] FROM [v3d].[Series] FOR XML RAW('seriespath'),  TYPE, ROOT('seriespaths') )
      
   IF @RESULTS IS NULL
    SET @RESULTS = '<seriespath />'
   
   RETURN @RC
   
  END TRY
  BEGIN CATCH
     
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  @PROCEDURE
  
  END CATCH


END





GO
GRANT EXECUTE ON [v3d].[sp_getSeriesPath] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getSeriesPath] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getService]    Script Date: 3/19/2014 4:44:49 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_getSessionLaunchList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==========================================================
-- Author:		  Mark Kontak 
-- Create date: 11/26/2013
-- Description:	Gets the launch list using the session
-- ==========================================================
CREATE PROCEDURE [v3d].[sp_getSessionLaunchList]
@ID NVARCHAR(256),                    -- Context Id
@LOCATION NVARCHAR(64) = 'remote',    -- Store location remote opr local
@SESSION_UID NVARCHAR(128),           -- Session to launch
@RULES XML,                           -- Application Launch Rule
@RESULTS XML OUTPUT                   -- Session Launch List 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON;


  BEGIN TRY


    -- ========================
    -- Launch Rule Variables
    -- ========================
    DECLARE @SESSION_SELECTION  AS NVARCHAR(16)       -- Session selection setting
    DECLARE @DICOM_SELECTION    AS NVARCHAR(16)       -- Dicom series selection setting
    DECLARE @ORIENTATIONS       AS NVARCHAR(1024)     -- Delimited 
    DECLARE @SERIES_TYPES       AS INT                -- mask of series types
    DECLARE @MAXIMUM_DATASETS   AS INT                -- Max datasets allowed for the application      
    DECLARE @SESSION            AS INT
    DECLARE @DICOM              AS INT


    DECLARE @IDOC               AS INT

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RULES

    -- ==================================
    -- Extract the launch rul information
    -- ==================================
    SELECT  @MAXIMUM_DATASETS   = [max],
            @ORIENTATIONS       = [ort],
            @SERIES_TYPES       = [type],
            @SESSION_SELECTION  = [session],
            @DICOM_SELECTION    = [dicom]
            FROM OPENXML(@IDOC, N'/rule', 1) WITH 
          ( 
            [max]             int,
            [ort]             nvarchar(512),
            [type]            int,
            [session]         nvarchar(16),
            [dicom]           nvarchar(16)
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- Set variables
    SELECT @SESSION            = [System].[v3d].[fn_getMaskBit]('type','session'),
           @DICOM              = [System].[v3d].[fn_getMaskBit]('type','dicom'),
           @MAXIMUM_DATASETS   = ISNULL(@MAXIMUM_DATASETS,100),
           @ORIENTATIONS       = ISNULL(@ORIENTATIONS,''),
           @SERIES_TYPES       = ISNULL(@SERIES_TYPES,0),
           @SESSION_SELECTION  = ISNULL(@SESSION_SELECTION, 'LIFO' ),
           @DICOM_SELECTION    = ISNULL(@DICOM_SELECTION, 'FIFO' )

        
    DECLARE @TRANSID NVARCHAR(256)
    SET @TRANSID = @SESSION_UID

    --- Locks the session
    EXEC [v3d].[sp_lockSeries]
       @ID      = @ID,
       @UID     = @SESSION_UID,
       @TRANSID = @TRANSID OUTPUT
       
    -- Builds the session launch list
    SET @RESULTS = ( SELECT  [v3d].[Series].[uid], @TRANSID as [tid], [v3d].[fn_getSeriesPath](@LOCATION,[v3d].[Series].[uid]) AS [url]
                             FROM [v3d].[Series] 
                            WHERE [v3d].[Series].[uid] = @SESSION_UID
                              FOR XML RAW('session'), TYPE ) 

    DECLARE @SERIES_TABLE AS TABLE (  [uid] NVARCHAR(128) NOT NULL, [ort] NVARCHAR(32) NOT NULL )

    -- ========================================================================================
    -- Only need to iterate through the orientations if they are specified in the launch rule
    -- ========================================================================================
    IF LEN(@ORIENTATIONS) > 0
    BEGIN

     SELECT @ORIENTATIONS = '|' + @ORIENTATIONS + '|'

     DECLARE @ORIENTATIONS_TABLE AS TABLE (	[ort] NVARCHAR(32) NOT NULL )

      INSERT INTO @ORIENTATIONS_TABLE ( [ort] ) ( SELECT  SUBSTRING(@ORIENTATIONS, Number + 1, CHARINDEX('|',  @ORIENTATIONS, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@ORIENTATIONS) - 1
              AND SUBSTRING(@ORIENTATIONS, Number, 1) = '|' ) 

  
      DECLARE @ORT AS NVARCHAR(16)

      DECLARE ORT_CURSOR CURSOR LOCAL
          FOR SELECT [ort] FROM @ORIENTATIONS_TABLE


      OPEN ORT_CURSOR

      -- fetch the first UID
      FETCH NEXT FROM ORT_CURSOR INTO @ORT


      WHILE @@FETCH_STATUS = 0
      BEGIN

 
        -- ================================================
        -- If there is a % in the orientation assume LIKE
        -- ================================================
        IF CHARINDEX('%', @ORT) > 0 
        BEGIN
      
          -- ===================================================================
          -- Either get the first or last dicom based on the launch rue setting
          -- ====================================================================
          IF @DICOM_SELECTION = 'FIFO'
            INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT  TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  LIKE @ORT
                              ORDER BY [date] ASC
          ELSE
            INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT   TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  LIKE @ORT
                              ORDER BY [date] DESC

        END
        ELSE
        BEGIN

          -- ===================================================================
          -- Either get the first or last dicom based on the launch rue setting
          -- ====================================================================
          IF @DICOM_SELECTION = 'FIFO'
            INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT   TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  = @ORT
                              ORDER BY [date] ASC
          ELSE
            INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT   TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  = @ORT
                              ORDER BY [date] DESC

        END



        FETCH NEXT FROM ORT_CURSOR INTO @ORT

      END


            -- Close the CURSOR
      CLOSE ORT_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE ORT_CURSOR

    END
    ELSE
    BEGIN

      IF @DICOM_SELECTION = 'FIFO'
        INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT  TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                              ORDER BY [date] ASC
      ELSE
       INSERT INTO @SERIES_TABLE ( [uid], [ort] ) SELECT TOP(@MAXIMUM_DATASETS) [ruid], [ort]   FROM [v3d].[ReferenceSeries]
                                WHERE [uid] = @SESSION_UID 
                                  AND [type] = @DICOM
                              ORDER BY [date] DESC
  


    END

    DECLARE @XML AS XML
    DECLARE @UID AS NVARCHAR(128)
    DECLARE @PATH AS NVARCHAR(MAX)
    DECLARE @CID AS NVARCHAR(64)
    DECLARE SERIES_CURSOR CURSOR LOCAL
          FOR SELECT [uid] FROM @SERIES_TABLE

    OPEN SERIES_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM SERIES_CURSOR INTO @UID

    WHILE @@FETCH_STATUS = 0
    BEGIN

      
      SELECT @PATH = [v3d].[fn_getSeriesPath](@LOCATION,@UID) 
    
      SET @XML = ( SELECT LOWER([ort]) AS [ort], [uid], @PATH AS [url],
                          ( SELECT [cid], [name], [System].[v3d].[fn_pathCombine](@PATH,'volumes',[cid]) AS [url]
                                FROM [v3d].[VolumeReferences] NOLOCK
                                WHERE [duid] = @UID AND
                                      [uid] = @SESSION_UID
                                FOR XML RAW('volume'), TYPE ),
                          [v3d].[fn_getDatasetSeries]([uid], @SESSION_UID, @LOCATION)   
                    FROM [v3d].[Series] NOLOCK
                    WHERE [uid] = @UID  
                      FOR XML RAW('dataset'), TYPE )
      
      SET @RESULTS.modify('insert sql:variable("@XML") into (/session)[1]')

      -- fetch the first UID
      FETCH NEXT FROM SERIES_CURSOR INTO @UID

    END

    CLOSE SERIES_CURSOR

    DEALLOCATE SERIES_CURSOR

   
  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_getSessionLaunchList'

  END CATCH

END



GO
/****** Object:  StoredProcedure [v3d].[sp_getStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 6/10/2010
-- Description:	Gets the patient/study hierarchacal xml representation using the <study ... /> query paramaters.
--              All attribute names in the xml will be either the column name on the view or short value defined 
--              by greg.
--
--              Short       Column
--              -----       -----------------
--              wgt         weight
--              dob         dob
--              sex         sex
--              mid         medicalId
--              apps        applications
--              desc        description
--              an          accessionNumber
--              id          Id
--              uid         uid
--              grp         group
--              date        scanDate
--              name        name
--              state       state
--              
--            
-- =============================================================================================================
CREATE PROCEDURE [v3d].[sp_getStudy]
@ID NVARCHAR(256),                      -- Context ID
@QUERY XML = '<study />',               -- Query parameters
@RESULTS XML OUTPUT                     -- XML results    
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @IDOC AS INT
  DECLARE @COLLECTION AS NVARCHAR(MAX)
   
  -- ===================================================================================================
  -- Declare all local variables that will be used as parameters in the SELECT against the QueryView 
  -- ===================================================================================================
  DECLARE @NAME             AS NVARCHAR(1000)
  DECLARE @UID              AS NVARCHAR(MAX)
  DECLARE @MID              AS NVARCHAR(512)
  DECLARE @SEX              AS CHAR(1) 
  DECLARE @FROM_WEIGHT      AS DECIMAL(5,2) 
  DECLARE @TO_WEIGHT        AS DECIMAL(5,2)
  DECLARE @FROM_DOB         AS DATE
  DECLARE @TO_DOB           AS DATE
  DECLARE @FROM_SCANDATE    AS DATETIMEOFFSET(3)
  DECLARE @TO_SCANDATE      AS DATETIMEOFFSET(3)
  DECLARE @FROM_RCVDATE     AS DATETIMEOFFSET(3)
  DECLARE @TO_RCVDATE       AS DATETIMEOFFSET(3)
  DECLARE @ACCESSION        AS NVARCHAR(512)
  DECLARE @PHYSICIAN        AS NVARCHAR(512)
  DECLARE @GROUP            AS NVARCHAR(512)
  DECLARE @DESCRIPTION      AS NVARCHAR(512)
  DECLARE @APPS             AS NVARCHAR(128)
  DECLARE @MODS             AS NVARCHAR(128)
  DECLARE @RC               AS INT
  DECLARE @STUDY_ID         AS NVARCHAR(512)
 
 
  RETRY:

  BEGIN TRY
  
     
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'
  
    
    -- ==================
    -- Read the query xml
    -- ==================
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- =================================
    -- Extract the query information.
    -- ================================
    SELECT  @MID            = [mid],
            @NAME           = [name],
            @FROM_DOB       = [fromDob],
            @TO_DOB         = [toDob],
            @SEX            = [sex],
            @FROM_WEIGHT    = [fromWeight],
            @TO_WEIGHT      = [toWeight],
            @UID            = [uid], 
            @ACCESSION      = [an], 
            @GROUP          = [grp],
            @PHYSICIAN      = [phys],
            @FROM_SCANDATE  = [fromScanDate],
            @TO_SCANDATE    = [toScanDate],
            @FROM_RCVDATE   = [fromRcvDate],
            @TO_RCVDATE     = [toRcvDate],
            @APPS           = [apps],
            @MODS           = [mods],
            @STUDY_ID       = [id],
            @DESCRIPTION    = [desc]
          FROM OPENXML(@IDOC, N'//study', 1) WITH 
          ( 
            [mid]           nvarchar(512),
            [name]          nvarchar(1000),
            [fromDob]       date              'dob/@from',
            [toDob]         date              'dob/@to',
            [sex]           char(1),
            [fromWeight]    decimal(5,2)      'wgt/@from',
            [toWeight]      decimal(5,2)      'wgt/@to',
            [uid]           nvarchar(1000),
            [phys]          nvarchar(512),
            [an]            nvarchar(512),
            [grp]           nvarchar(512),
            [id]            nvarchar(512),
            [apps]          nvarchar(128),
            [mods]          nvarchar(128),
            [desc]          nvarchar(512),
            [fromScanDate]  date              'date/@from',
            [toScanDate]    date              'date/@to',
            [fromRcvDate]   date              'rcv/@from',
            [toRcvDate]     date              'rcv/@to'

          )
           
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    
    
    DECLARE @LEN AS INT
    DECLARE @WHERE AS NVARCHAR(MAX)
    DECLARE @SELECT AS NVARCHAR(MAX)

    -- ========================================
    -- Clear of the MODS setting is set to ALL
    -- ========================================
    IF UPPER(@MODS) = 'ALL' 
      SET @MODS = '';
    

    -- ========================================
    -- Clear of the APPS setting is set to ALL
    -- ========================================
    IF UPPER(@APPS) = 'ALL' 
      SET @APPS = '';

    SELECT @WHERE = [System].v3d.fn_buildStringComparison(N'@UID', @UID, '[uid]') +
                    [System].v3d.fn_buildDateComparison(@FROM_DOB, N'@FROM_DOB', @TO_DOB, N'@TO_DOB', N'[dob]') +
                    [System].v3d.fn_buildIntegerComparison(@FROM_WEIGHT, N'@FROM_WEIGHT', @TO_WEIGHT, N'@TO_WEIGHT', N'[wgt]') +
                    [System].v3d.fn_buildStringComparison(N'@MID', @MID, '[mid]') +
                    [System].v3d.fn_buildStringComparison(N'@NAME', @NAME, '[name]') +
                    [System].v3d.fn_buildDateComparison(@FROM_SCANDATE, N'@FROM_SCANDATE', @TO_SCANDATE, N'@TO_SCANDATE', N'[date]') +
                    [System].v3d.fn_buildDateComparison(@FROM_RCVDATE, N'@FROM_RCVDATE', @TO_RCVDATE, N'@TO_RCVDATE', N'[rcv]') +
                    [System].v3d.fn_buildStringComparison(N'@PHYSICIAN', @PHYSICIAN, '[phys]') +
                    [System].v3d.fn_buildStringComparison(N'@APPS', @APPS, '[apps]') +
                    [System].v3d.fn_buildStringComparison(N'@MODS', @MODS, '[mods]') +
                    [System].v3d.fn_buildStringComparison(N'@ACCESSION', @ACCESSION, '[an]') +
                    [System].v3d.fn_buildStringComparison(N'@GROUP', @GROUP, '[grp]') +
                    [System].v3d.fn_buildStringComparison(N'@STUDY_ID', @STUDY_ID, '[id]') +
                    [System].v3d.fn_buildStringComparison(N'@DESCRIPTION', @DESCRIPTION, '[desc]'),
          @SELECT = N'SET @RESULTS = ( SELECT *, [v3d].[fn_getVersion](''Studies'') AS [ver]  FROM [v3d].[Studies] CROSS APPLY  [v3d].[fn_getSeriesBasedStudyColumns]([v3d].[Studies].[uid]) AS EXTRA '

    IF @SEX IS NOT NULL
      SET @WHERE = @WHERE + ' [sex] = @SEX AND '

        
    
    SET @LEN = LEN(@WHERE)
    
    IF @LEN > 0
      SET @WHERE = N' WHERE ' + SUBSTRING(@WHERE,0,@LEN - 3)
      
 
    SET @SELECT = @SELECT + @WHERE + N' FOR XML RAW(''study''), TYPE, ROOT(''collection'')); SET @COUNT = @@ROWCOUNT; '
   
    DECLARE @PARAMETERS AS NVARCHAR(4000)
    
    SET @PARAMETERS = 
                      -- Patient Parameters
                      N'@MID              NVARCHAR(32), ' +
                      N'@NAME             NVARCHAR(64), ' +
                      N'@SEX              NVARCHAR(16), ' + 
                      N'@FROM_WEIGHT      DECIMAL(5,2), ' + 
                      N'@TO_WEIGHT        DECIMAL(5,2), ' +
                      N'@FROM_DOB         DATE, ' +
                      N'@TO_DOB           DATE, ' +
        
                      -- Study parameters
                      N'@FROM_SCANDATE    DATETIMEOFFSET(7), ' + 
                      N'@TO_SCANDATE      DATETIMEOFFSET(7), ' + 
                      N'@FROM_RCVDATE     DATETIMEOFFSET(7), ' + 
                      N'@TO_RCVDATE       DATETIMEOFFSET(7), ' + 
                      N'@STUDY_ID         NVARCHAR(32), ' +
                      N'@GROUP            NVARCHAR(32), ' + 
                      N'@PHYSICIAN        NVARCHAR(64), ' + 
                      N'@ACCESSION        NVARCHAR(64), ' + 
                      N'@DESCRIPTION      NVARCHAR(256), ' + 
                      N'@UID              NVARCHAR(128), ' +
                      N'@APPS             NVARCHAR(128), ' +
                      N'@MODS             NVARCHAR(128), ' +
                      N'@COUNT            INT OUTPUT, ' + 
     
                      N'@RESULTS          XML OUTPUT ' 


    DECLARE @QUERY_CURSOR CURSOR

    EXEC sp_executesql 
            @SELECT,
            @PARAMETERS,
            
            -- Patient
            @MID            = @MID,
            @NAME           = @NAME,
            @SEX            = @SEX,
            @FROM_WEIGHT    = @FROM_WEIGHT,
            @TO_WEIGHT      = @TO_WEIGHT,
            @FROM_DOB       = @FROM_DOB,
            @TO_DOB         = @TO_DOB,

            --- Study          
            @TO_SCANDATE    = @TO_SCANDATE,
            @FROM_SCANDATE  = @FROM_SCANDATE,
            @TO_RCVDATE     = @TO_RCVDATE,
            @FROM_RCVDATE   = @FROM_RCVDATE,
            @PHYSICIAN      = @PHYSICIAN,
            @ACCESSION      = @ACCESSION,
            @DESCRIPTION    = @DESCRIPTION,
            @STUDY_ID       = @STUDY_ID,
            @GROUP          = @GROUP,
            @APPS           = @APPS,
            @MODS           = @MODS,
            @UID            = @UID,
            @COUNT          = @RC OUTPUT,

            -- Results
            @RESULTS        = @RESULTS OUTPUT
           
    IF @RESULTS IS NULL
      SET @RESULTS = '<collection id="study" />'
    ELSE
      SET @RESULTS.modify('insert attribute id {"study"} into (/collection)[1]')   

    RETURN @RC
    
  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getStudy',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY
  
 
  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_getStudy] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_getStudy] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_getStudyLaunchList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/20/2013
-- Description:	Generates a launch list using the specified study and launch rules.
-- ===================================================================================
CREATE PROCEDURE [v3d].[sp_getStudyLaunchList]
@ID           NVARCHAR(256),            -- Context Id
@LOCATION     NVARCHAR(64) = 'remote',  -- Store location remote or local
@SESSION_UID  NVARCHAR(128),            -- Session Uid (Created in sp_getLaunchList)
@STUDY_UID    NVARCHAR(128),            -- Study UID to launch
@RULES        XML,                      -- Launch rule
@RESULTS      XML OUTPUT                -- Results to be returned
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON


  BEGIN TRY


    DECLARE @IDX      AS INT
    DECLARE @CID      AS NVARCHAR(32)
    DECLARE @ORT      AS NVARCHAR(32)
    DECLARE @UID      AS NVARCHAR(128)
    DECLARE @PATH     AS NVARCHAR(MAX)
    DECLARE @XML      AS XML
 
 
    DECLARE @VOLUME_REFERNCE_COUNT AS INT
    DECLARE @VTYPE    AS NVARCHAR(32)
    DECLARE @APP      AS NVARCHAR(32)
    DECLARE @VSTATE   AS BIGINT
    DECLARE @VUID     AS NVARCHAR(128)
    DECLARE @VCID     AS NVARCHAR(32)
    DECLARE @LAST_UID AS NVARCHAR(128)
    DECLARE @UPDATED  AS BIT


    -- ========================
    -- Launch Rule Variables
    -- ========================
    DECLARE @SESSION_SELECTION  AS NVARCHAR(16)       -- Session selection setting
    DECLARE @DICOM_SELECTION    AS NVARCHAR(16)       -- Dicom series selection setting
    DECLARE @ORIENTATIONS       AS NVARCHAR(1024)     -- Delimited 
    DECLARE @SERIES_TYPES       AS INT                -- mask of series types
    DECLARE @MAXIMUM_DATASETS   AS INT                -- Max datasets allowed for the application      
    DECLARE @MINIMUM_DATASETS   AS INT                -- Min datasets required for the application      
    DECLARE @SESSION            AS INT
    DECLARE @DICOM              AS INT

    DECLARE @IDOC               AS INT

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RULES

    -- ==================================
    -- Extract the launch rule information
    -- ==================================
    SELECT  @APP                = [app],
            @MAXIMUM_DATASETS   = [max],
            @ORIENTATIONS       = [ort],
            @SERIES_TYPES       = [type],
            @SESSION_SELECTION  = [session],
            @DICOM_SELECTION    = [dicom]
            FROM OPENXML(@IDOC, N'/rule', 1) WITH 
          ( 
            [app]             nvarchar(32),
            [max]             int,
            [ort]             nvarchar(512),
            [type]            int,
            [session]         nvarchar(16),
            [dicom]           nvarchar(16)
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- Set variables
    -- Set variables
    SELECT @SESSION            = [System].[v3d].[fn_getMaskBit]('type','session'),
           @DICOM              = [System].[v3d].[fn_getMaskBit]('type','dicom'),
           @MAXIMUM_DATASETS   = ISNULL(@MAXIMUM_DATASETS,100),
           @ORIENTATIONS       = ISNULL(@ORIENTATIONS,''),
           @SERIES_TYPES       = ISNULL(@SERIES_TYPES,0),
           @SESSION_SELECTION  = ISNULL(@SESSION_SELECTION, 'LIFO' ),
           @DICOM_SELECTION    = ISNULL(@DICOM_SELECTION, 'FIFO' )

  
    -- Temporary series table
    DECLARE @SERIES_TABLE AS TABLE (  [id] INT IDENTITY(1,1) NOT NULL, 
                                      [uid] NVARCHAR(128) NOT NULL, 
                                      [ort] NVARCHAR(32) NOT NULL,
                                      [cid] NVARCHAR(32) DEFAULT '',
                                      [state] BIGINT DEFAULT 0 )


     -- ========================================================================================
    -- Only need to iterate through the orientations if they are specified in the launch rule
    -- ========================================================================================
    IF LEN(@ORIENTATIONS) > 0
    BEGIN

     SELECT @ORIENTATIONS = '|' + @ORIENTATIONS + '|'

     DECLARE @ORIENTATIONS_TABLE AS TABLE (	[id] INT IDENTITY(1,1) NOT NULL, [ort] NVARCHAR(32) NOT NULL )

     INSERT INTO @ORIENTATIONS_TABLE ( [ort] ) ( SELECT  SUBSTRING(@ORIENTATIONS, Number + 1, CHARINDEX('|',  @ORIENTATIONS, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@ORIENTATIONS) - 1
              AND SUBSTRING(@ORIENTATIONS, Number, 1) = '|' ) 

  
  
      DECLARE ORT_CURSOR CURSOR LOCAL
          FOR SELECT [id], [ort] FROM @ORIENTATIONS_TABLE


      OPEN ORT_CURSOR

      -- fetch the first UID
      FETCH NEXT FROM ORT_CURSOR INTO @IDX, @ORT

      -- =======================================
      -- Loop through all the orientations
      -- ======================================
      WHILE @@FETCH_STATUS = 0
      BEGIN

 
        -- ================================================
        -- If there is a % in the orientation assume LIKE
        -- ================================================
        IF CHARINDEX('%', @ORT) > 0 
        BEGIN
      
          -- ===================================================================
          -- Either get the first or last dicom based on the launch rue setting
          -- ====================================================================
          IF @DICOM_SELECTION = 'FIFO'
            INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid] ) 
                               SELECT TOP(@MAXIMUM_DATASETS) [uid], [ort], [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid]
                                 FROM [v3d].[Series]
                                WHERE [suid] = @STUDY_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  LIKE @ORT
                              ORDER BY [date] ASC
          ELSE
            INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid]  )   
                               SELECT  TOP(@MAXIMUM_DATASETS) [uid], [ort], [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid]
                                 FROM [v3d].[Series]
                                WHERE [suid] = @STUDY_UID 
                                  AND [type] = @DICOM
                                  AND [ort]  LIKE @ORT
                              ORDER BY [date] DESC

        END
        ELSE
        BEGIN

          -- ===================================================================
          -- Either get the first or last dicom based on the launch rue setting
          -- ====================================================================
          IF @DICOM_SELECTION = 'FIFO'
            INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid]  )   
                               SELECT  TOP(@MAXIMUM_DATASETS)  [uid], [ort],  [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid] 
                                 FROM [v3d].[Series]
                                WHERE [suid] = @STUDY_UID
                                  AND [type] = @DICOM
                                  AND [ort] = @ORT
                              ORDER BY [date] ASC
          ELSE
            INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid]  )   
                               SELECT  TOP(@MAXIMUM_DATASETS)  [uid], [ort],  [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid]  
                                FROM [v3d].[Series]
                                WHERE [suid] = @STUDY_UID 
                                  AND [type] = @DICOM
                                  AND [ort] = @ORT
                              ORDER BY [date] DESC

        END



        FETCH NEXT FROM ORT_CURSOR INTO @IDX, @ORT

      END -- END .. While there are orientations in the list


      -- Close the CURSOR
      CLOSE ORT_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE ORT_CURSOR

    END

    -- =================================================================
    -- Else we do not have a orientation odering specified in the rule
    -- =================================================================
    ELSE
    BEGIN

      IF @DICOM_SELECTION = 'FIFO'
        INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid]  )   
                         SELECT TOP(@MAXIMUM_DATASETS) [uid], [ort],  [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid]  
                           FROM [v3d].[Series]
                          WHERE [suid] = @STUDY_UID 
                            AND [type] = @DICOM
                        ORDER BY [date] ASC
      ELSE
       INSERT INTO @SERIES_TABLE ( [uid], [ort], [state], [cid]  )   
                         SELECT TOP(@MAXIMUM_DATASETS) [uid], [ort],  [state], [v3d].[fn_getVolumeConversionId]([uid]) AS [cid]  
                           FROM [v3d].[Series]
                          WHERE [suid] = @STUDY_UID 
                            AND [type] = @DICOM
                        ORDER BY [date] DESC
  


    END -- END ... Else
   
    DECLARE @DISTINT_ORTS  AS INT


    SELECT @DISTINT_ORTS = COUNT(*) FROM @SERIES_TABLE

    DECLARE @UIDS AS NVARCHAR(MAX)
    SET @UIDS = '|'


 
    -- Get the number of distint orientations
    SELECT @DISTINT_ORTS = COUNT(DISTINCT [ort]) FROM @SERIES_TABLE

    -- Cursor to iterate the table
    DECLARE SERIES_CURSOR CURSOR LOCAL
        FOR SELECT [id], [uid], [ort], [cid] FROM @SERIES_TABLE


    OPEN SERIES_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @ORT, @CID

    DECLARE @LAST AS NVARCHAR(32)
    DECLARE @COUNT AS INT
    SELECT @COUNT = 0,
            @LAST = ''

    WHILE @@FETCH_STATUS = 0
    BEGIN

       -- ==============================================================================
      -- If we already have the maximum datasets in the table then delete the remaining
      -- ===============================================================================
      IF @COUNT = @MAXIMUM_DATASETS 
      BEGIN
        DELETE @SERIES_TABLE WHERE [uid] = @UID AND [ort] = @ORT
      END --- END .. If we have reached the maximum limit
      ELSE
      BEGIN 
  
        -- =========================================================================================================
        -- If the orientations were specified in the rule then we need to make sure we have distinct orientations
        -- =========================================================================================================
        IF LEN(@ORIENTATIONS) > 0
        BEGIN
      
          -- ===================================================================================================
          -- If we only have one orientation then oinly save the 1st MAXIMUM_DATASETS count and delete the rest.
          -- ====================================================================================================
          IF @DISTINT_ORTS = 1
          BEGIN

  
            -- ============================================================================================
            -- If there are no volumes for the application specified then delete the series otherwise 
            -- pull information out of thre volume references.
            -- ============================================================================================
            IF NOT EXISTS ( SELECT * FROM [v3d].[Volumes] WHERE [uid] = @UID AND [app] = @APP ) 
            BEGIN
              DELETE @SERIES_TABLE WHERE [uid] = @UID AND [ort] = @ORT
            END
            ELSE
            BEGIN

  
              SELECT @VOLUME_REFERNCE_COUNT = 0,
                      @UPDATED = 0

              DECLARE VOLUME_REFERENCE_CURSOR CURSOR LOCAL FOR SELECT [uid], [type], [state], [cid]
                FROM [v3d].[VolumeReferencedSeries] 
                WHERE [duid] = @UID 
                  AND ( [type] & @SERIES_TYPES ) = [type]
                ORDER BY [date] DESC

              OPEN VOLUME_REFERENCE_CURSOR

              FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

              WHILE @@FETCH_STATUS = 0 AND @UPDATED = 0
              BEGIN

                SET @VOLUME_REFERNCE_COUNT = @VOLUME_REFERNCE_COUNT + 1

                -- ============================================================================================
                -- If the series is not in completed state and it is Required then save so that we can check the 
                -- dependents.
                -- ============================================================================================
                IF [System].[v3d].[fn_isMaskBitSet](@VSTATE, 'state', 'completed') = 0
                BEGIN
              
                    DELETE @SERIES_TABLE WHERE [uid] = @UID
   
                END -- END ... If the series is in a failed state
                ELSE
                BEGIN

                  -- Only update the series table's CID (Conversion ID) if it has not already been updated
                  UPDATE @SERIES_TABLE SET [cid] = @VCID WHERE [uid] = @UID 

                  SELECT @CID = @VCID,
                          @UPDATED = 1

                END
            
                FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

              END -- END ... While there are volume refernces

              -- Close the CURSOR
              CLOSE VOLUME_REFERENCE_CURSOR
       
              -- Deallocate the resources
              DEALLOCATE VOLUME_REFERENCE_CURSOR


              -- =============================================================================================
              -- Only create the references if the series table was updated or there were no volume references
              -- =============================================================================================
              IF @UPDATED = 1 OR @VOLUME_REFERNCE_COUNT = 0
              BEGIN

                EXEC [v3d].[sp_createReferences] 
                  @SESSION = @SESSION_UID, 
                  @DICOM = @UID, 
                  @CID = @CID

                -- Increment the DATASET count
                SET @COUNT = @COUNT + 1
              END -- END ... If the series table was updated or there are not volume references

            END -- END ... Else there are volumes associated with the specified application

 

          END -- END ... If we do not distint orientations

          -- ==================================================================================================================
          -- Else we have more then one type of dataset so we will grab the 1st MAXIMUM_DATASETS number of series with a 
          -- distint orientation or only the maximum number of distint orientations.
          --
          -- Example:       S - Supine, P - Prone, DL - Decub-Left, DR - Decub-Right
          --
          --      MAXIMUM_DATASETS = 4
          --      Series Orientation List:    S1, S2, S3, P, DL
          --      Dataset List:               S1, P, DL
          --
          --      MAXIMUM_DATASETS = 2
          --      Series Orientation List:    S1, S2, S3, S4
          --      Dataset List:               S1, S2
          --
          --      MAXIMUM_DATASETS = 2
          --      Series Orientation List:    S1, S2, P, DL
          --      Dataset List:               S1, P
          --
          -- ==================================================================================================================
          ELSE
          BEGIN 
          
            -- =======================================================================================
            -- If the LAST orientation does not match the new orientation (distinct) and we hove not 
            -- reached the MAXIMUM_DATASETS count then set the new LAST orientation and continue.
            -- =======================================================================================
            IF @LAST <> @ORT 
            BEGIN

              -- ============================================================================================
              -- If there are no volumes for the application specified then delete the series otherwise 
              -- pull information out of thre volume references.
              -- ============================================================================================
              IF NOT EXISTS ( SELECT * FROM [v3d].[Volumes] WHERE [uid] = @UID AND [app] = @APP ) 
              BEGIN
                DELETE @SERIES_TABLE WHERE [uid] = @UID AND [ort] = @ORT
              END
              ELSE
              BEGIN

                SELECT @VOLUME_REFERNCE_COUNT = 0,
                       @UPDATED = 0

                DECLARE VOLUME_REFERENCE_CURSOR CURSOR LOCAL FOR SELECT [uid], [type], [state], [cid]
                  FROM [v3d].[VolumeReferencedSeries] 
                 WHERE [duid] = @UID 
                   AND ( [type] & @SERIES_TYPES ) = [type] 
                 ORDER BY [date] DESC

                OPEN VOLUME_REFERENCE_CURSOR

                FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

                WHILE @@FETCH_STATUS = 0 AND @UPDATED = 0
                BEGIN

                  SET @VOLUME_REFERNCE_COUNT = @VOLUME_REFERNCE_COUNT + 1

                  -- ============================================================================================
                  -- If the series is not in completed state and it is Required then save so that we can check the 
                  -- dependents.
                  -- ============================================================================================
                  IF [System].[v3d].[fn_isMaskBitSet](@VSTATE, 'state', 'completed') = 0
                  BEGIN
               
                    DELETE @SERIES_TABLE WHERE [uid] = @UID
   
                  END -- END ... If the series is in a failed state
                  ELSE
                  BEGIN

                    -- Only update the series table's CID (Conversion ID) if it has not already been updated
                    UPDATE @SERIES_TABLE SET [cid] = @VCID WHERE [uid] = @UID 

                    SELECT @CID = @VCID,
                           @UPDATED = 1

                  END
            
                  FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

                END

                -- Close the CURSOR
                CLOSE VOLUME_REFERENCE_CURSOR
       
                -- Deallocate the resources
                DEALLOCATE VOLUME_REFERENCE_CURSOR

             -- =============================================================================================
              -- Only create the references if the series table was updated or there were no volume references
              -- =============================================================================================
              IF @UPDATED = 1 OR @VOLUME_REFERNCE_COUNT = 0
              BEGIN

                EXEC [v3d].[sp_createReferences] 
                  @SESSION = @SESSION_UID, 
                  @DICOM = @UID, 
                  @CID = @CID

  
                  SELECT @COUNT = @COUNT + 1,
                         @LAST = @ORT

                END -- END ... If the series table was updated or there are not volume references

              END   --- Else if the is volumes associated with the specifie dapplication

            END

            -- ===========================================================================================
            -- Else if the LAST orientation is equal to the current orientation or we have reached the 
            -- MAXIMUM_DATASETS count then delete the series
            -- ===========================================================================================
            ELSE
            BEGIN
              DELETE @SERIES_TABLE WHERE [uid] = @UID AND [ort] = @ORT
            END

          END -- END ... Else
 
        END

        -- ==============================================================================================
        -- Else we do not car about the orientations and their order so we save the 1st MAXIMUM_DATASETS 
        -- series and deleted the rest. We do this in the order that the series are in the table which
        -- should be either FIFO or LIFO.
        -- ==============================================================================================
        ELSE
        BEGIN

  
          -- ============================================================================================
          -- If there are no volumes for the application specified then delete the series otherwise 
          -- pull information out of thre volume references.
          -- ============================================================================================
          IF NOT EXISTS ( SELECT * FROM [v3d].[Volumes] WHERE [uid] = @UID AND [app] = @APP ) 
          BEGIN
            DELETE @SERIES_TABLE WHERE [uid] = @UID AND [ort] = @ORT
          END
          ELSE
          BEGIN

            SELECT @VOLUME_REFERNCE_COUNT = 0,
                    @UPDATED = 0

              DECLARE VOLUME_REFERENCE_CURSOR CURSOR LOCAL FOR SELECT [uid], [type], [state], [cid]
                      FROM [v3d].[VolumeReferencedSeries] 
                      WHERE [duid] = @UID 
                        AND ( [type] & @SERIES_TYPES ) = [type] 
                      ORDER BY [date] DESC

            OPEN VOLUME_REFERENCE_CURSOR

            FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

            WHILE @@FETCH_STATUS = 0 AND @UPDATED = 0
            BEGIN

              SET @VOLUME_REFERNCE_COUNT = @VOLUME_REFERNCE_COUNT + 1

              -- ============================================================================================
              -- If the series is not in completed and it is Required then save so that we can check the 
              -- dependents.
              -- ============================================================================================
              IF [System].[v3d].[fn_isMaskBitSet](@VSTATE, 'state', 'completed') = 0
              BEGIN
              
                DELETE @SERIES_TABLE WHERE [uid] = @UID
   
              END -- END ... If the series is in a failed state
              ELSE
              BEGIN

                -- Only update the series table's CID (Conversion ID) if it has not already been updated
                UPDATE @SERIES_TABLE SET [cid] = @VCID WHERE [uid] = @UID

                SELECT @CID = @VCID,
                        @UPDATED = 1

              END
            
              FETCH NEXT FROM  VOLUME_REFERENCE_CURSOR INTO @VUID, @VTYPE, @VSTATE, @VCID

            END

            -- Close the CURSOR
            CLOSE VOLUME_REFERENCE_CURSOR
       
            -- Deallocate the resources
            DEALLOCATE VOLUME_REFERENCE_CURSOR

            -- =============================================================================================
            -- Only create the references if the series table was updated or there were no volume references
            -- =============================================================================================
            IF @UPDATED = 1 OR @VOLUME_REFERNCE_COUNT = 0
            BEGIN

              EXEC [v3d].[sp_createReferences] 
                @SESSION = @SESSION_UID, 
                @DICOM = @UID, 
                @CID = @CID

              -- Increment the DATASET count
              SET @COUNT = @COUNT + 1
            END -- END ... If the series table was updated or there are not volume references

  
          END

        END -- END ... ELse
    
      END -- END ... Else we have not reach trhe maximum
             
      -- fetch the first UID
      FETCH NEXT FROM SERIES_CURSOR INTO @IDX, @UID, @ORT, @CID

    END -- END ... While there are series in the SERIES_TABLE

        -- Close the CURSOR
    CLOSE SERIES_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE SERIES_CURSOR
    
    -- ==============================================================================================
    -- Make sure we meeet the minimum requirement. If we do not then it usually means we did not find 
    -- datasets with the approiate series that could be launched by the viewer.
    -- ===============================================================================================
    IF ( SELECT COUNT(*) FROM @SERIES_TABLE) < @MINIMUM_DATASETS
     EXEC [master].[dbo].[sp_throwException] 901225, 'Datasets', @MINIMUM_DATASETS

 

    DECLARE @TRANSID NVARCHAR(256)
    SET @TRANSID = @SESSION_UID

    --- Locks the session
    EXEC [v3d].[sp_lockSeries]
      @ID      = @ID,
      @UID     = @SESSION_UID,
      @TRANSID = @TRANSID OUTPUT

    -- Set the initial results 
    SET @RESULTS = ( SELECT [uid], @TRANSID as [tid], [v3d].[fn_getSeriesPath](@LOCATION,[v3d].[Series].[uid]) AS [url] FROM [v3d].[Series] WHERE [uid] = @SESSION_UID FOR XML RAW('session') )
 

     DECLARE SERIES_CURSOR CURSOR LOCAL
          FOR SELECT [uid], [cid] FROM @SERIES_TABLE

    OPEN SERIES_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM SERIES_CURSOR INTO @UID, @CID

    WHILE @@FETCH_STATUS = 0
    BEGIN

      
      SELECT @PATH = [v3d].[fn_getSeriesPath](@LOCATION,@UID) 
    
      SET @XML = ( SELECT LOWER([ort]) AS [ort], [uid], @PATH AS [url],
                          ( SELECT [cid], [name], [System].[v3d].[fn_pathCombine](@PATH,'volumes',[cid]) AS [url]
                                FROM [v3d].[Volumes] NOLOCK
                                WHERE [uid] = @UID AND 
                                      [cid] = @CID 
                                FOR XML RAW('volume'), TYPE ),
                          [v3d].[fn_getDatasetSeries]([uid], @SESSION_UID, @LOCATION)   
                    FROM [v3d].[Series] NOLOCK
                    WHERE [uid] = @UID  
                      FOR XML RAW('dataset'), TYPE )
      
      SET @RESULTS.modify('insert sql:variable("@XML") into (/session)[1]')

      -- fetch the first UID
      FETCH NEXT FROM SERIES_CURSOR INTO @UID, @CID

    END

    CLOSE SERIES_CURSOR

    DEALLOCATE SERIES_CURSOR



  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler] 
      @PROCEDURE = 'sp_getStudyLaunchList'

  END CATCH

END



GO
/****** Object:  StoredProcedure [v3d].[sp_getSystemId]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_getSystemId]
	@systemid [nvarchar](4000) OUTPUT
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_getSystemId]
GO
/****** Object:  StoredProcedure [v3d].[sp_getTransferList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 12/13/2013
-- Description:	Returns the transfer list
-- =============================================
CREATE PROCEDURE [v3d].[sp_getTransferList]
@ID AS NVARCHAR(256),         -- Context Id
@QUERY AS XML,                -- Transfer list query
@RESULTS AS XML OUTPUT        -- Transfer results
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON

  DECLARE @TRUE       AS BIT
  DECLARE @OPEN       AS BIT
  DECLARE @FALSE      AS BIT
  DECLARE @UID_COUNT  AS INT
  DECLARE @NAME       AS NVARCHAR(256)
  DECLARE @TYPE       AS INT
  DECLARE @DESC       AS NVARCHAR(512)
  DECLARE @SERIES_UID AS NVARCHAR(128)
  DECLARE @UID        AS NVARCHAR(128)
  DECLARE @SUID       AS NVARCHAR(128)
  DECLARE @UIDS       AS NVARCHAR(MAX)
  DECLARE @SYSTEM     AS NVARCHAR(64)
  DECLARE @LOGIN      AS NVARCHAR(32)
  DECLARE @LOCATION   AS NVARCHAR(64)
  DECLARE @STUDY_XML  AS XML
  DECLARE @SUBQUERY   AS NVARCHAR(128)
  DECLARE @SERIES_XML AS XML
  DECLARE @STUDY_PROCESSED AS BIT
  DECLARE @SESSION_PROCESSED AS BIT
  DECLARE @PATH AS NVARCHAR(MAX)
  DECLARE @TRANSID UNIQUEIDENTIFIER
  DECLARE @SESSION    AS INT
  DECLARE @DICOM      AS INT

  RETRY:
  
  BEGIN TRY
 

    BEGIN TRANSACTION

    SELECT @SESSION            = [System].[v3d].[fn_getMaskBit]('type','session'),
           @DICOM              = [System].[v3d].[fn_getMaskBit]('type','dicom'),
           @OPEN  = 0,
           @FALSE = 0,
           @TRUE  = 1,
           @NAME  = '',
           @RESULTS = '<transferlist></transferlist>'
 

     -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'transfer'

    DECLARE @IDOC AS INT

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @QUERY

    -- ===========================================
    -- Extract the get launch list information
    -- ==========================================
    SELECT  @UIDS           = [uids],
            @LOGIN          = [login],
            @SYSTEM         = [sys]
            FROM OPENXML(@IDOC, N'/transferlist', 1) WITH 
          ( 
            [uids]            nvarchar(MAX),
            [login]           nvarchar(16),
            [sys]             nvarchar(32)
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

 
    -- ================================================
    -- Missing UIDS (900990) Uids missing or invalid
    -- ================================================
    IF @UIDS IS NULL OR  LEN(@UIDS) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uids' 
      
 
    -- =====================================
    -- Set parameters to make sure no NULLS
    -- =====================================
    SELECT  @TRUE       = 1,
            @SYSTEM     = ISNULL(@SYSTEM,''),
            @UIDS       = '|' + @UIDS + '|',
            @LOCATION   = 'remote'
            FROM [System].[v3d].[Stores] WHERE [name] = 'default'

    -- =======================================
    -- Make sure we found the default store
    -- =======================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException]  'Default Store'


    -- ===========================================================================
    -- If we are launching from the same system then set the location to local
    -- ===========================================================================
    IF SERVERPROPERTY('MachineName') = @SYSTEM 
      SET @LOCATION = 'local'

 
    -- =========================================================
    -- Builds the UID Table from the list of UID's passed in
    -- =========================================================
    DECLARE @UID_TABLE AS TABLE (	[id] INT IDENTITY(1,1) NOT NULL, [uid] [nvarchar](128) NOT NULL )

    INSERT INTO @UID_TABLE ( [uid] ) ( SELECT  SUBSTRING(@UIDS, Number + 1, CHARINDEX('|',  @UIDS, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UIDS) - 1
              AND SUBSTRING(@UIDS, Number, 1) = '|' ) 


    -- Actual UID count
    SET @UID_COUNT = @@ROWCOUNT



    -- ==========================================================================
    -- Create the UID's CURSOR so we can iterate though each UID individually
    -- ==========================================================================
    DECLARE UIDS_CURSOR CURSOR LOCAL
        FOR SELECT [uid] FROM @UID_TABLE

    SET @OPEN = @TRUE

    OPEN UIDS_CURSOR

    -- fetch the first UID
    FETCH NEXT FROM UIDS_CURSOR INTO @UID

    -- =============================================
    -- loop while there are records in the cursor
    -- =============================================
    WHILE @@FETCH_STATUS = 0
    BEGIN
        

      -- ============================
      -- Check if the UID is a study
      -- ============================
      IF EXISTS ( SELECT * FROM [v3d].[Studies] WHERE [uid] = @UID )
      BEGIN
      
 
        --- Locks the study
        EXEC [v3d].[sp_lockStudy]
          @ID      = @ID,
          @UID     = @UID,
          @TRANSID = @TRANSID OUTPUT

        -- ==================
        -- Get the STUDY XML
        -- ==================
        EXEC [v3d].[sp_getTransferStudy]  
          @ID = @ID,
          @UID = @UID,
          @LOCATION = @LOCATION,
          @RESULTS = @STUDY_XML OUTPUT

        SELECT @NAME = T.st.value('@name[1]','nvarchar(256)') FROM @STUDY_XML.nodes('/study') AS T(st)

 
        -- Get all the study series
        DECLARE SERIES_CURSOR CURSOR LOCAL FOR SELECT [uid] FROM [v3d].[Series] WHERE [suid] = @UID

        OPEN SERIES_CURSOR

        -- fetch the first UID
        FETCH NEXT FROM SERIES_CURSOR INTO @SERIES_UID

       
        WHILE @@FETCH_STATUS = 0
        BEGIN

          -- ============================
          -- Gets the SERIES XML
          -- ============================
          EXEC [v3d].[sp_getTransferSeries] 
            @ID = @ID,
            @UID = @SERIES_UID,
            @LOCATION = @LOCATION,
            @RESULTS = @SERIES_XML OUTPUT

          -- ====================================
          -- Insert the series into the study
          -- ====================================
          SET @STUDY_XML.modify('insert sql:variable("@SERIES_XML") into (/study)[1]')
 
 

          -- fetch the first UID
          FETCH NEXT FROM SERIES_CURSOR INTO @SERIES_UID

        END

        CLOSE SERIES_CURSOR

        DEALLOCATE SERIES_CURSOR

        -- ===================================================
        -- Insert the <study> under the <transferlist> node
        -- ====================================================
        SET  @RESULTS.modify('insert sql:variable("@STUDY_XML") into (/transferlist)[1]')      
  
   
        -- Add audit entry for failed action
        EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
             @UID = @UID, 
             @ACTION = N'transfer', 
             @LOGIN = @LOGIN, 
             @NAME = @NAME
   
          
      END -- END ... If the UID is a study
      
      -- ==========================================
      -- Else we have what appears to be a series
      -- ==========================================
      ELSE 
      BEGIN
      
        -- =================================================================
        -- Check to make sure the series exists and grab the current TYPE
        -- =================================================================
        SELECT @TYPE = [type], @SUID = [suid] FROM [v3d].[Series] WHERE [uid] = @UID

        -- =======================================
        -- Indicate that uid was invalid 
        -- =======================================      
        IF @@ROWCOUNT = 0
          EXEC [master].[dbo].[sp_throwInvalidDataException]  'UID'

        -- ================================
        -- Invalid data wrong series types
        -- ================================
        IF @TYPE <> @DICOM AND @TYPE <> @SESSION 
           EXEC [master].[dbo].[sp_throwInvalidDataException] 'Series Type'

        -- ===================
        -- Get the STUDY XML
        -- ===================
        EXEC [v3d].[sp_getTransferStudy]  
          @ID = @ID,
          @UID = @SUID,
          @LOCATION = @LOCATION,
          @RESULTS = @STUDY_XML OUTPUT

        SELECT @NAME = T.st.value('@name[1]','nvarchar(256)') FROM @STUDY_XML.nodes('/study') AS T(st)

 
        -- ===============================================
        -- Gets the SERIES XML
        -- ==============================================
        EXEC [v3d].[sp_getTransferSeries] 
          @ID = @ID,
          @UID = @UID,
          @CIDS = '',
          @LOCATION = @LOCATION,
          @RESULTS = @SERIES_XML OUTPUT
 

        -- ====================================
        -- Insert the series into the study
        -- ====================================
        SET @STUDY_XML.modify('insert sql:variable("@SERIES_XML") into (/study)[1]')
 
        -- ==========================================================
        -- Insert the study into the <transferlist></transferlist>
        -- =========================================================
        SET  @RESULTS.modify('insert sql:variable("@STUDY_XML") into (/transferlist)[1]')      

        -- Add audit entry for failed action
        EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
             @UID = @SUID, 
             @ACTION = N'transfer', 
             @LOGIN = @LOGIN, 
             @NAME = @NAME
   
 
        --- Locks the series
        EXEC [v3d].[sp_lockSeries]
          @ID      = @ID,
          @UID     = @UID,
          @TRANSID = @TRANSID OUTPUT
 
        
        IF @TYPE <> @DICOM
        BEGIN         
        
            
  
          DECLARE @CIDS AS NVARCHAR(MAX)
          DECLARE @RUID AS NVARCHAR(128)
          DECLARE SERIES_CURSOR CURSOR LOCAL FOR SELECT [ruid], [suid] FROM [v3d].[ReferenceSeries] WHERE [uid] = @UID

          SET @CIDS = ''

          SELECT @CIDS = @CIDS + [cid] + '|' FROM [v3d].[VolumeReferences] WHERE [uid] = @UID

          OPEN SERIES_CURSOR

          -- fetch the first UID
          FETCH NEXT FROM SERIES_CURSOR INTO @RUID, @SUID

          -- =============================================
          -- loop while there are records in the cursor
          -- =============================================
          WHILE @@FETCH_STATUS = 0
          BEGIN

            -- ======================
            -- Get the STUDY XML
            -- =====================
            EXEC [v3d].[sp_getTransferStudy]  
              @ID = @ID,
              @UID = @SUID,
              @LOCATION = @LOCATION,
              @RESULTS = @STUDY_XML OUTPUT

              SELECT @NAME = T.st.value('@name[1]','nvarchar(256)') FROM @STUDY_XML.nodes('/study') AS T(st)

            -- ==================
            -- Get the SERIS XML
            -- ==================
            EXEC [v3d].[sp_getTransferSeries] 
              @ID = @ID,
              @UID = @RUID,
              @CIDS = @CIDS,
              @LOCATIOn = @LOCATION,
              @RESULTS = @SERIES_XML OUTPUT
 
 

            IF EXISTS ( SELECT T.st.query('.') FROM @RESULTS.nodes('/transferlist/study[@uid=sql:variable("@SUID")]') T(st) )
            BEGIN
              SET  @RESULTS.modify('insert sql:variable("@SERIES_XML") into (/transferlist/study[@uid=sql:variable("@SUID")])[1]')    
            END
            ELSE
            BEGIN
              -- ====================================
              -- Insert the series into the study
              -- ====================================
              SET @STUDY_XML.modify('insert sql:variable("@SERIES_XML") into (/study)[1]')
 
              -- ==========================================================
              -- Insert the study into the <transferlist></transferlist>
              -- =========================================================
              SET  @RESULTS.modify('insert sql:variable("@STUDY_XML") into (/transferlist)[1]')     
              
              -- Add audit entry for failed action
              EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
                   @UID = @SUID, 
                   @ACTION = N'transfer', 
                   @LOGIN = @LOGIN, 
                   @NAME = @NAME
   
              

            END

            -- fetch the first UID
            FETCH NEXT FROM SERIES_CURSOR INTO @RUID, @SUID

          END

          CLOSE SERIES_CURSOR

          DEALLOCATE SERIES_CURSOR
                      
        END -- END ... Else the UID is a non-session
        

        
      END  -- END ... Else we have a series UID

            
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM UIDS_CURSOR INTO @UID

    END  -- END WHILE
     
    -- Close the CURSOR
    CLOSE UIDS_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE UIDS_CURSOR


    SET  @RESULTS.modify('insert attribute tid{sql:variable("@TRANSID")} into (/transferlist)[1]')      
 
    COMMIT TRANSACTION

	END TRY
	BEGIN CATCH
	
    IF @OPEN = @TRUE
    BEGIN
     -- Close the CURSOR
      CLOSE UIDS_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE UIDS_CURSOR
    END

   IF XACT_STATE() = -1
    ROLLBACK TRANSACTION

   DECLARE @ERROR_CODE AS INT

   EXEC @ERROR_CODE = [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_getTransferList',
      @DEADLOCK = @TRUE
       

 	  WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
 

	  RETURN 0
	  
	END CATCH


END



GO
/****** Object:  StoredProcedure [v3d].[sp_getTransferSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================
-- Author:		  Mark Kontak
-- Create date: 12/18/2013
-- Description:	Returns a single series xml for transfer
-- =========================================================
CREATE PROCEDURE [v3d].[sp_getTransferSeries]
(
@ID NVARCHAR(256),
@UID NVARCHAR(128),
@LOCATION NVARCHAR(16),
@CIDS NVARCHAR(MAX) = NULL,
@RESULTS XML OUTPUT
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @VOLUME_PATH AS NVARCHAR(MAX)
  DECLARE @CID AS NVARCHAR(64)
  DECLARE @NAME AS NVARCHAR(32)
  DECLARE @FILE AS NVARCHAR(256)
  DECLARE @PATH AS NVARCHAR(MAX)
  DECLARE @COUNT AS INT
  DECLARE @QUERY AS XML

  SELECT  @PATH = [v3d].[fn_getSeriesPath](@LOCATION, @UID),
          @QUERY = '<series uid="' + @UID + '" />'


  EXEC [v3d].[sp_getSeries] 
  @ID = @ID,
  @QUERY = @QUERY,
  @RESULTS = @RESULTS OUTPUT
 
  -- ==============================================
  -- Insert the url attribute with the series path
  -- ==============================================
  SET @RESULTS.modify('insert attribute url{sql:variable("@PATH")} into (/collection/series)[1]')
            
  -- ====================================================
  -- Delete the references node from under the series
  -- ====================================================    
  SET @RESULTS.modify('delete (/collection/series/references)')

  -- =============================================
  -- Delete the proc node from under the series
  -- =============================================
  SET @RESULTS.modify('delete (/collection/series/proc)')

 
  -- ==============================================================
  -- Delete any volumes that are noty referenced by this session
  -- ==============================================================
  IF @CIDS IS NOT NULL
    SET @RESULTS.modify('delete (/collection/series/volumes/volume[contains(sql:variable("@CIDS"),@cid) != true()])')


  DECLARE @INDEX AS INT

  SELECT  @PATH = [System].[v3d].[fn_pathCombine](@PATH, 'volumes',''),
          @COUNT = 0

  -- ====================================================================================================================
  -- We now need to insert the url attribute into to the volume xml. We need to do this for each volume under the 
  -- series. I will include the file which may or may not have the extension.
  -- ====================================================================================================================
  DECLARE XML_CURSOR  CURSOR LOCAL FOR SELECT T.vol.value('@cid[1]','nvarchar(64)') AS [cid], 
                                              T.vol.value('@name[1]','nvarchar(32)') AS [name],
                                              T.vol.value('@file[1]','nvarchar(256)') AS [file]
                                          FROM   @RESULTS.nodes('/collection/series/volumes/volume') T(vol)
         
  OPEN XML_CURSOR

  -- fetch the first UID
  FETCH NEXT FROM XML_CURSOR INTO @CID, @NAME, @FILE

  WHILE @@FETCH_STATUS = 0
  BEGIN
  
    --SELECT @INDEX = CHARINDEX('.',@FILE)

    --IF @INDEX > 0 
    --  SELECT @FILE = SUBSTRING(@FILE,1,@INDEX-1)

    -- =============================================
    -- Count the volumes and set the volume path
    -- =============================================
    SELECT  @COUNT = @COUNT + 1,
            @VOLUME_PATH =  [System].[v3d].[fn_pathCombine](@PATH,@CID,'')
              
    SET @RESULTS.modify('insert attribute url{sql:variable("@VOLUME_PATH")} into  (/collection/series/volumes/volume[@cid=sql:variable("@CID") and @name=sql:variable("@NAME")])[1]')
 
              
    -- fetch the first UID
    FETCH NEXT FROM XML_CURSOR INTO @CID, @NAME, @FILE

  END 


  CLOSE XML_CURSOR

  DEALLOCATE XML_CURSOR

  -- ===============================================
  -- If we have no volumes remove the volumes root
  -- ===============================================
  IF @COUNT = 0 
    SET @RESULTS.modify('delete (/collection/series/volumes)')

  -- ==============================================================================================================================
  -- Pull out the series node from the <collection> since we need to insert them as a list of series under the <transferlist> node
  -- ==============================================================================================================================
  SELECT @RESULTS = T.c.query('.') FROM   @RESULTS.nodes('/collection/series') T(c)

END



GO
/****** Object:  StoredProcedure [v3d].[sp_getTransferStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================
-- Author:		  Mark Kontak
-- Create date: 12/18/2013
-- Description:	Returns a single series xml for transfer
-- =========================================================
CREATE PROCEDURE [v3d].[sp_getTransferStudy]
(
@ID NVARCHAR(256),
@UID NVARCHAR(128),
@LOCATION NVARCHAR(16),
@RESULTS XML OUTPUT
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @PATH AS NVARCHAR(MAX)
  DECLARE @QUERY AS XML


  SELECT @PATH = [v3d].[fn_getStudyPath](@LOCATION, @UID),
          @QUERY = '<study uid="' + @UID + '" />'

  EXEC [v3d].[sp_getStudy]
    @ID = @ID,
    @QUERY = @QUERY,
    @RESULTS = @RESULTS OUTPUT

  -- ===========================================
  -- Insert the url (PATH) for the study xml
  -- ===========================================
  SET @RESULTS.modify('insert attribute url{sql:variable("@PATH")} into (/collection/study)[1]')

  -- ==============================================
  -- Pull the study node out from the <collection>
  -- =============================================
  SELECT @RESULTS = T.c.query('.') FROM   @RESULTS.nodes('/collection/study') T(c)


END



GO
/****** Object:  StoredProcedure [v3d].[sp_insertDicomServer]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/14/2011
-- Description: Cleans up the server entries
-- =============================================
CREATE PROCEDURE [v3d].[sp_insertDicomServer]
(
@ID       NVARCHAR(256),      -- Context Id
@RECORD   XML OUTPUT
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

   DECLARE @AETITLE  AS NVARCHAR(32)
  DECLARE @SERVER   AS NVARCHAR(32)
  DECLARE @PORT     AS INT
  DECLARE @IDOC     AS INT



  BEGIN TRY
 
    
    -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
       @ID = @ID,
      @MASK = 'StorageManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @AETITLE         = [aetitle],
            @SERVER          = [server],
            @PORT            = [port]
           FROM OPENXML(@IDOC, N'/dicomserver', 1) WITH 
          ( 
            [aetitle]   nvarchar(64),
            [server]    nvarchar(32),
            [port]      int
          )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC


    SET @AETITLE  = ISNULL(@AETITLE,'')
    SET @SERVER   = ISNULL(@SERVER,'')
    SET @PORT     = ISNULL(@PORT,0)

    -- ==================================================================
    -- An AETitle must be supplied (900990) AETitle is missing or invalid
    -- =================================================================
    IF LEN(@AETITLE) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'AETitle'

    -- ======================================================
    -- Make sure that the the server does not already exist
    -- ======================================================
    IF EXISTS ( SELECT * FROM [v3d].[DicomServers] WHERE [aetitle] = @AETITLE )
      UPDATE [v3d].[DicomServers] SET [date] = SYSDATETIMEOFFSET() WHERE [aetitle] = @AETITLE
    ELSE
      INSERT INTO [v3d].[DicomServers] ( [aetitle], [server], [port] ) VALUES ( @AETITLE, @SERVER, @PORT )
 
    RETURN @@ROWCOUNT

  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertDicomServer'

  END CATCH

 
END


  






GO
GRANT EXECUTE ON [v3d].[sp_insertDicomServer] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertDicomServer] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ========================================================================
-- Author:		  Mark Kontak
-- Create date: 04/17/2012
-- Description:	Creates the job from the parameters supplied. The initial
--              state is alwasy pending.
-- ========================================================================
CREATE PROCEDURE [v3d].[sp_insertJob]
(
@ID NVARCHAR(256),          -- Authentication token
@RECORD XML = NULL,         -- Record
@UID NVARCHAR(256) = NULL,  -- Series uid
@APP NVARCHAR(32) = NULL,   -- Application
@TID NVARCHAR(32) = NULL,   -- Template id
@VOL NVARCHAR(128) = NULL,  -- Volume name
@STATE INT = NULL           -- Initial state
)
AS
BEGIN

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON;

  DECLARE @TYPE AS INT
  DECLARE @PROCESS AS XML

  DECLARE @RC AS INT
  DECLARE @ORT AS NVARCHAR(32)
  DECLARE @MOD AS NVARCHAR(16)
  DECLARE @NAME AS NVARCHAR(128)
  DECLARE @DESC AS NVARCHAR(256)
  DECLARE @SUID AS NVARCHAR(128)
  DECLARE @PENDING AS BIGINT
  DECLARE @PAUSED AS BIGINT
  DECLARE @RECEIVING AS BIGINT
  DECLARE @DEFAULT_APP AS NVARCHAR(32)
  DECLARE @ICNT AS INT
  DECLARE @IDOC AS INT
  DECLARE @TASKS AS XML
  DECLARE @INDEX AS INT
  DECLARE @TRANSCOUNT AS INT
  DECLARE @SESSION    AS INT
  DECLARE @DICOM      AS INT
 

  SELECT @TRANSCOUNT = @@TRANCOUNT
 

  BEGIN TRY

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION;

    -- ============================================================================================
    -- Determines if the user specified in the context supplied is authoried to perform this task.
    -- ============================================================================================
    EXEC [Security].[v3d].[sp_isAuthorized]
        @ID = @ID,
        @MASK = 'ProcessingManagement'

   
   -- ==============================================================
   -- If the XML record was supplied then use thos parameters
   -- ==============================================================
   IF @RECORD IS NOT NULL
   BEGIN

      -- Read the license xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


      SELECT  @STATE            = [state],
              @TID              = [tid],
              @APP              = [app],
              @VOL              = [vol],
              @UID              = [uid]
              FROM OPENXML(@IDOC, N'/job', 3) 
              WITH 
              (
                [state]         int,
                [vol]           nvarchar(128),
                [tid]           nvarchar(32),
                [app]           nvarchar(32),
                [uid]           nvarchar(256)
              )
       
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC
      
    END -- END ... If the RECORD was supplied

    SELECT  @SESSION      = [System].[v3d].[fn_getMaskBit]('type','session'),
            @DICOM        = [System].[v3d].[fn_getMaskBit]('type','dicom'),
            @PENDING      = [v3d].[fn_getState]('pending'),
            @PAUSED       = [v3d].[fn_getState]('paused'),
            @RECEIVING    = [v3d].[fn_getState]('receiving'),
            @DEFAULT_APP  = [v3d].[fn_getDefaultApplication]();



    -- Initialize variable
    SELECT  @TASKS    = '', 
            @STATE    = ISNULL(@STATE,@PENDING),
            @APP      = ISNULL(@APP,''),
            @TID      = ISNULL(@TID,''),
            @VOL      = ISNULL(@VOL,''),
            @INDEX    = 1




    -- ===========================================================================
    -- Raise exception indicating that the the series uid was missing or invalid
    -- ===========================================================================
    IF @UID IS NULL OR LEN(@UID) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Series Uid'
 
    DECLARE @SERIES_APP AS NVARCHAR(32)

    -- =================================
    -- Get information from the series
    -- =================================
    SELECT  @NAME       = [name],  
            @ORT        = [ort],  
            @TYPE       = [type], 
            @MOD        = [mod],
            @SERIES_APP = [app],
            @ICNT       = [icnt], 
            @PROCESS    = [proc],
            @SUID       = [suid],
            @DESC       = [Storage].[v3d].[Series].[desc] 
          FROM [Storage].[v3d].[Series] 
          INNER JOIN [Storage].[v3d].[Studies] ON [Storage].[v3d].[Studies].[uid] = [Storage].[v3d].[Series].[suid] 
          WHERE [Storage].[v3d].[Series].[uid] = @UID 

    -- ======================================
    -- If the Series specified does not exist
    -- ======================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Series'

    -- ===================================================================================
    -- If the initial state is not PAUSED OR RECEIVING then set it PENDING
    -- ===================================================================================
    IF @STATE <> @PAUSED AND @STATE <> @RECEIVING
      SET @STATE = @PENDING

    -- =======================================================================================================
    -- If NO template was specified and the source series is not DICOM then we need to set the application. 
    -- The following tree shows the presidence of the applications.
    --
    --  SPECIFIED
    --    SERIES
    --      VOLUME
    --        PROCESS (Extract From XML)
    --          DEFAULT ( All else fails )
    -- ========================================================================================================
    IF LEN(@TID) = 0 AND LEN(@APP) = 0 
    BEGIN

      
      -- =========================================================================================================
      -- If we have a DICOM series with with no template and application specified so we are doing a conversion
      -- =========================================================================================================
      IF @TYPE = @DICOM
      BEGIN

        SET @TASKS = [System].[v3d].[fn_getSetting]('ConversionTask',NULL);

        -- =================================================
        -- If there no conversion task we have a problem
        --
        -- 900990 - Missing or invalid conversion task
        -- ================================================
        IF  @TASKS IS NULL
          EXEC [master].[dbo].[sp_throwInvalidDataException] 'Conversion Task'

      END

 
      -- ==============================================================================
      -- If the series contains no application and a volume name was specified then 
      -- get the application from the volume
      -- =============================================================================
      IF LEN(@SERIES_APP) = 0 AND LEN(@VOL) > 0
        SELECT @APP = [app] FROM [v3d].[Volumes] WHERE [name] = @VOL AND [uid] = @UID


      -- ================================================================================================
      -- If the process in the series was set then extract the index of the process and the application. 
      -- Otherwise we need to get the application from the Filters if possible.
      -- ================================================================================================
      IF LEN(@APP) = 0 AND @PROCESS IS NOT NULL
      BEGIN
        SELECT  @INDEX  = ( @PROCESS.value('(proc/task/@idx)[1]', 'int') + 1 ),
                @APP    =  @PROCESS.value('(proc/task/@app)[1]', 'nvarchar(16)')
      END

      -- =======================================================
      -- If all else fails then set to the default application
      -- ======================================================
      IF LEN(@APP) = 0 
        SET @APP = @DEFAULT_APP

 
         
    END -- END ... If the template and applications was not specfied

 

    -- ========================================================================================
    -- If we have not set the TASKS yet we need to extract the information from the Templates
    -- ========================================================================================
    IF @TASKS IS NULL
    BEGIN
        
        -- ========================================================================
        -- If the template was specified then look up the template using the id
        -- =========================================================================
        IF LEN(@TID) > 0 
        BEGIN

          -- Get the default processing template for the application
          SELECT @TASKS = [tasks] FROM [System].[v3d].[Templates] WHERE [id] = @TID

          -- ===========================================================
          -- Raise a NotFound exception for the  teamplate
          -- ==========================================================
          IF @@ROWCOUNT = 0
             EXEC [master].[dbo].[sp_throwNotFoundException] 'Template', @TID
 

        END -- END ... If the template was specified

        -- ===========================================
        -- Look up the template using the application
        -- ==========================================
        ELSE
        BEGIN

          -- Get the default processing template for the application
          SELECT @TASKS = [tasks] FROM [System].[v3d].[Templates] WHERE [app] = @APP AND [default] = 1

          -- ==================================================================================================
          -- If there is not DEFAULT template for the application then we will use the first one from the list,
          -- but a warning will be logged to the ErrorMessages.
          -- ==================================================================================================
          IF @@ROWCOUNT = 0 
          BEGIN

            DECLARE @DATABASE AS NVARCHAR(64)
            DECLARE @MESSAGE AS NVARCHAR(1024)

            SELECT  @DATABASE = DB_NAME(),
                    @MESSAGE = 'No default template for ' + @APP + ' was found, defaulting to the first template entry for the application'

            EXEC [master].[dbo].[sp_logErrorMessage] @DATABASE, 'sp_insertJob', 0, @MESSAGE

       
            -- Get the first row from the applicatios template list
            SELECT TOP(1) @TASKS = [tasks] FROM [System].[v3d].[Templates] WHERE [app] = @APP 

            -- ===========================================================
            -- Raise a NotFound exception for the application teamplates
            -- ==========================================================
            IF @@ROWCOUNT = 0
               EXEC [master].[dbo].[sp_throwNotFoundException] 'Aplication Templates', @APP

          END -- END ... If the there is no default application template

        END  -- END ... Else no template was specified

    END


    -- ====================================
    -- Only insert the job if we have tasks
    -- ====================================
    IF @TASKS IS NOT NULL 
    BEGIN

       -- Delete all tasks with an index != to the current process 
      SET @TASKS.modify('delete task[@idx!=sql:variable(''@INDEX'')]')

 
      -- ==========================================
      -- Do not create a JOB if there are no tasks
      -- =========================================
      IF @TASKS.exist('task[@name]') = 1
      BEGIN
    
  
        -- Insert the JOB and get the GUID assigned after inserted    
        INSERT INTO [v3d].[Jobs]  
                ( [login], [src], [app], [state], [name], [ort], [mod], [desc], [task], [study], [icnt], [tid] ) VALUES
                ( [Security].[v3d].[fn_getLogin](@ID), @UID,  @APP, @STATE, @NAME, @ORT, @MOD, @DESC, @TASKS, @SUID, @ICNT, ISNULL(@TID,'') ) 

        SET @RC = @@ROWCOUNT

        -- =================================================================
        -- If a job was inserted and the initial state was pending then 
        -- raise the pending jobs event
        -- =================================================================
        IF @RC = 1 AND @STATE = @PENDING
          EXEC [v3d].[sp_raisePendingJobsEvent]

 
      END -- END ... If no additional tasks are found

    END --- END ... If tasks weere found 

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN @RC

  END TRY
  BEGIN CATCH
 
   IF XACT_STATE() = -1
      ROLLBACK TRANSACTION;
 
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertJob'

  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_insertJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2010
-- Description:	Adds the specified series to the system. Will get the storage devive from the list of stores 
--              by checking the LargestSeriesSize against the available storage and if there is enough room 
--              the the store is returned.
--  
--              V3D2000   No uid was specified
--              V3D2001   No study uid was specified
--
-- ============================================================================================================
CREATE PROCEDURE [v3d].[sp_insertSeries] 
(
@ID AS NVARCHAR(256),               -- Context Id
@AUTHORIZE AS BIT = 1,              -- authorize flag (used in the insertImage procedure)
@RECORD XML OUTPUT                  -- Series
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON


  DECLARE @IDOC               AS INT
  DECLARE @STUDY_UID          AS NVARCHAR(128)
  DECLARE @ORIENTATION        AS NVARCHAR(32)
  DECLARE @PLANE              AS TINYINT
  DECLARE @DATE               AS DATETIMEOFFSET(7)
  DECLARE @UID                AS NVARCHAR(256)
  DECLARE @MODALITY           AS NVARCHAR(16)
  DECLARE @TYPE               AS INT
  DECLARE @DESCRIPTION        AS NVARCHAR(256)
  DECLARE @IMAGE_COUNT        AS INT
  DECLARE @STATE              AS BIGINT
  DECLARE @SOURCE             AS NVARCHAR(288)
  DECLARE @NUMBER             AS NVARCHAR(64)
  DECLARE @EXT                AS XML
  DECLARE @HEADER             AS XML
  DECLARE @RC                 AS INT
  DECLARE @PROC               AS XML
  DECLARE @DIR                AS NVARCHAR(512)
  DECLARE @APP                AS NVARCHAR(10)
  DECLARE @STORE              AS NVARCHAR(32)
  DECLARE @REFS               AS XML
  DECLARE @VOLS               AS XML
  DECLARE @TRANSCOUNT         AS INT


  BEGIN TRY


    IF @@TRANCOUNT = 0 
    BEGIN
      SET @TRANSCOUNT = 1
      BEGIN TRANSACTION  
    END

    SELECT  @RC = -1

    -- ====================================
    -- Make sure the user can add a study
    -- ====================================
    IF @AUTHORIZE = 1
      EXEC [Security].[v3d].[sp_isAuthorized]
        @ID = @ID,
        @MASK = 'StorageManagement'

 

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @UID                = [uid],
            @STUDY_UID          = [suid],
            @ORIENTATION        = [ort],
            @PLANE              = [pln],
            @TYPE               = [type],
            @DATE               = [date],
            @MODALITY           = [mod],
            @STATE              = [state],
            @SOURCE             = [src],
            @APP                = [app],
            @DESCRIPTION        = [desc],
            @IMAGE_COUNT        = [icnt],
            @NUMBER             = [num],
            @DIR                = [dir],
            @STORE              = [st],
            @PROC               = [proc],
            @HEADER             = [hdr],
            @EXT                = [ext],
            @REFS               = [references],
            @VOLS               = [volumes]
          FROM OPENXML(@IDOC, N'/series', 1) WITH 
          ( 
            [uid]         nvarchar(128),
            [suid]        nvarchar(128),
            [num]         nvarchar(32),
            [date]        datetimeoffset(7),
            [ort]         nvarchar(32),
            [mod]         nvarchar(16),
            [desc]        nvarchar(256),
            [type]        int,
            [icnt]        int,
            [state]       nvarchar(32),
            [app]         nvarchar(10),
            [src]         nvarchar(288),
            [pln]         tinyint,
            [dir]         nvarchar(512),
            [st]          nvarchar(32),
            [proc]        xml    './proc',
            [hdr]         xml    './hdr',
            [ext]         xml    './ext',
            [references]  xml    './references',
            [volumes]     xml    './volumes'
         )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    

    -- ====================================================================================================
    -- If the Extended Properties were included I need to extract the inner xml to load into the database
    -- ====================================================================================================
    IF @EXT IS NOT NULL
      SET @EXT = @EXT.query('/ext/*')

     -- ====================================================================================================
    -- If the header was included I need to extract the inner xml to load into the database
    -- ====================================================================================================
    IF @HEADER IS NOT NULL
      SET @HEADER = @HEADER.query('/hdr/*')
  
    -- ====================================================================================================
    -- If the process was included I need to extract the inner xml to load into the database
    -- ====================================================================================================
    IF @PROC IS NOT NULL
      SET @PROC = @HEADER.query('/proc/*')


    EXEC @RC = [v3d].[sp_insertSeriesInternal]
        @UID = @UID,
        @STUDY_UID = @STUDY_UID,
        @ORIENTATION = @ORIENTATION,
        @PLANE = @PLANE,
        @DATE = @DATE,
        @MODALITY = @MODALITY,
        @TYPE = @TYPE,
        @DESCRIPTION = @DESCRIPTION,
        @COUNT = @IMAGE_COUNT,
        @STATE = @STATE,
        @SOURCE = @SOURCE,
        @NUMBER = @NUMBER,
        @EXT = @EXT,
        @HEADER = @HEADER,
        @PROC = @PROC,
        @REFS = @REFS,
        @VOLS = @VOLS,
        @STORE = @STORE,
        @DIR = @DIR,
        @APP = @APP
      

           
   IF @TRANSCOUNT >0 
    COMMIT TRANSACTION

    RETURN @RC
  
  END TRY
  BEGIN CATCH

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertSeries'

  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_insertSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertSeriesInternal]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2010
-- Description:	Adds the specified series to the system. Will get the storage device from the list of stores 
--              by checking the LargestSeriesSize against the available storage and if there is enough room 
--              the the store is returned. No transaction is created assuming the calling procedure maintains 
--              the transaction.
--  
--              V3D2000   No uid was specified
--              V3D2001   No study uid was specified
--
-- ============================================================================================================
CREATE PROCEDURE [v3d].[sp_insertSeriesInternal] 
(
@UID                NVARCHAR(128),
@STUDY_UID          NVARCHAR(128),
@ORIENTATION        NVARCHAR(32) = NULL,
@PLANE              TINYINT = NULL,
@DATE               DATETIMEOFFSET(7) = NULL,
@MODALITY           NVARCHAR(16) = NULL,
@TYPE               INT = NULL,
@DESCRIPTION        NVARCHAR(256) = NULL,
@COUNT              INT = 0,
@STATE              BIGINT = 0,
@SOURCE             NVARCHAR(288) = NULL,
@NUMBER             NVARCHAR(64) = NULL,
@EXT                XML = NULL,
@HEADER             XML = NULL,
@PROC               XML = NULL,
@REFS               XML = NULL,
@VOLS               XML = NULL,
@STORE              NVARCHAR(32)  = NULL,
@DIR                NVARCHAR(512) = NULL,
@APP                NVARCHAR(10) = NULL
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @RC                 AS INT
  DECLARE @TYPE_NAME          AS NVARCHAR(32)
  
 
  RETRY:

  BEGIN TRY
  

    -- Set varaibles making sure no  NULLS
    SELECT @RC        = 0,
           @TYPE_NAME = [System].[v3d].[fn_getMaskBitName]('type',@TYPE),
           @STORE     = ISNULL(@STORE,''),
           @DIR       = ISNULL(@DIR,''),
           @DATE      = ISNULL(@DATE, sysdatetimeoffset())
    
    -- ============================================
    -- Make sure that the series uid was specified
    -- (900990) Uid missing or invalid
    -- ============================================
    IF @UID IS NULL OR  LEN(@UID) = 0
  	  EXEC [master].[dbo].[sp_throwInvalidDataException]  'Series Uid' 
   	   	  
    -- ============================================
    -- Make sure that the study uid was specified
    -- ============================================
    IF @STUDY_UID IS NULL OR LEN(@STUDY_UID) = 0 
  	  EXEC [master].[dbo].[sp_throwInvalidDataException]  'Study Uid' 

     -- =============================================
     -- Make sure the series does not already exist
     -- ============================================
     IF NOT EXISTS ( SELECT * FROM [v3d].[Series] WHERE uid = @UID )
     BEGIN

  
        
      -- ========================================================
      -- If the SERIES directory was not specified then build it
      -- ========================================================
      IF @DIR IS NULL OR LEN(@DIR) = 0 
      BEGIN
    
        DECLARE @CDATE AS DATETIME
        DECLARE @INCLUDE_ORT AS NVARCHAR(32)
      
        SELECT  @INCLUDE_ORT = UPPER([System].[v3d].[fn_getSetting]('IncludeOrientationInDirectory','False')),
                @CDATE = GETDATE()

        -- ==========================================================================================
        -- If the orientation is to be included in the directory and the orientation is present
        -- then used it.
        --
        -- Without Orientation:
        --    TYPE_YYYMMDDhhmmssff
        --
        --    Where TYPE is VOLUME, DICOM, PREPROCESSED, CAD
        --
        -- With Orientation
        --    ORIENTATION_TYPE_YYYMMDDhhmmssff
        --
        --    Where TYPE is the same as above.
        --
        --    Where ORIENTATION is HFS, FFS, HFP, FFP, FFDL, HFDL, FFDR, HFDR, Supine, Prone
        --                         Decubitu-Left and Decubitus-Right.
        --    
        -- =========================================================================================
        IF @INCLUDE_ORT = 'TRUE' AND LEN(@ORIENTATION) > 0
        BEGIN
          SELECT @DIR = UPPER(@ORIENTATION) + '_' + 
                      UPPER(@TYPE_NAME) + '_' +
                        CONVERT(NVARCHAR(4),DATEPART(YYYY,@CDATE)) + 
                          CONVERT(NVARCHAR(2),DATEPART(MM,@CDATE)) + 
                            CONVERT(NVARCHAR(2),DATEPART(DD,@CDATE)) + 
                              CONVERT(NVARCHAR(2),DATEPART(hh,@CDATE)) + 
                                CONVERT(NVARCHAR(2),DATEPART(mi, @CDATE)) + 
                                  CONVERT(NVARCHAR(2),DATEPART(ss,@CDATE)) + 
                                    CONVERT(NVARCHAR(3),DATEPART(ms,@CDATE))
      
        END
        ELSE
        BEGIN
          SELECT @DIR = UPPER(@TYPE_NAME) + '_' + 
                      CONVERT(NVARCHAR(4),DATEPART(YYYY,@CDATE)) + 
                        CONVERT(NVARCHAR(2),DATEPART(MM,@CDATE)) + 
                          CONVERT(NVARCHAR(2),DATEPART(DD,@CDATE)) + 
                            CONVERT(NVARCHAR(2),DATEPART(hh,@CDATE)) + 
                              CONVERT(NVARCHAR(2),DATEPART(mi, @CDATE)) + 
                                CONVERT(NVARCHAR(2),DATEPART(ss,@CDATE)) + 
                                  CONVERT(NVARCHAR(3),DATEPART(ms,@CDATE))
        END
                              
      END -- END ... If the directory was not specified
    
    
      -- =========================================================================
      -- If the STORE was not already specified then check for an available store 
      -- =========================================================================
      IF @STORE IS NULL OR LEN(@STORE) = 0 
        EXEC [System].[v3d].[sp_getAvailableStore]
          @STORE = @STORE OUTPUT
    
      IF LEN(@STORE) = 0
        SET @STORE = 'default'  
    
      SET @MODALITY = ISNULL(@MODALITY,'')

      -- =======================================================================
      -- If the date has not time zone then set it to the the systems time-zone
      -- =======================================================================
      IF DATEPART(tz,@DATE) = 0 
        SET @DATE = TODATETIMEOFFSET(@DATE, [v3d].[fn_getTimeZoneOffset]())
    
      -- ===================
      -- Perform the INSERT
      -- ===================
      INSERT INTO [v3d].[Series] ( [uid], [suid], [num], [date], [ort], [mod], [desc], [type], [icnt], 
                                   [state], [app], [ext], [pln], [src], [dir], [st], [proc], [hdr] )
             VALUES              (  @UID, 
                                    @STUDY_UID,  
                                    ISNULL(@NUMBER,''), 
                                    ISNULL(@DATE, SYSDATETIMEOFFSET()),
                                    ISNULL(@ORIENTATION,''),
                                    ISNULL(@MODALITY,''), 
                                    ISNULL(@DESCRIPTION,''), 
                                    ISNULL(@TYPE,0),  
                                    ISNULL(@COUNT,0), 
                                    ISNULL(@STATE,0), 
                                    ISNULL(@APP,''),
                                    @EXT, 
                                    ISNULL(@PLANE,0), 
                                    ISNULL(@SOURCE,''),
                                    @DIR,
                                    @STORE,
                                    @PROC,
                                    @HEADER )

      SET @RC = @@ROWCOUNT

    
      -- ======================================================================================================================
      -- If the references were included in the series then we add then to the database.This will be the case during 
      -- importing.
      -- ======================================================================================================================
      IF @REFS IS NOT NULL
      BEGIN

        -- ===============================
        -- Insert references from the xml
        -- ===============================
        INSERT INTO [v3d].[SeriesReferences] ( [uid], [ruid] )  
        SELECT @UID as [uid], T.c.query('.').value('(series/@uid)[1]','nvarchar(128)') AS ruid
                FROM   @REFS.nodes('/references/series') T(c)

      END

      -- ======================================================================================================================
      -- If the volumes were included in the series then we add then to the database.This will be the case during 
      -- importing.
      -- ======================================================================================================================
      IF @VOLS IS NOT NULL
      BEGIN

        -- ===============================
        -- Insert volumes from the xml
        -- ===============================
        INSERT INTO [v3d].[Volumes] ( [uid], [name], [icnt],  [app], [file], [date], [ext] )  
        SELECT @UID as  [uid], 
                        T.c.query('.').value('(volume/@name)[1]','nvarchar(128)') AS [name], 
                        T.c.query('.').value('(volume/@icnt)[1]','int') AS [icnt],
                        T.c.query('.').value('(volume/@app)[1]','nvarchar(16)') AS [app], 
                        T.c.query('.').value('(volume/@file)[1]','nvarchar(512)') AS [file], 
                        T.c.query('.').value('(volume/@date)[1]','datetimeoffset(7)') AS [date],
                        T.c.query('./ext/*') AS [ext]
                FROM   @VOLS.nodes('/volumes/volume') T(c)

      END


      -- ============================================
      -- Serialize the series
      -- ===========================================
      EXEC [v3d].[sp_serializeSeries]
            @UID = @UID

    END  -- END ... If the series does not already exist

  
 
    RETURN @RC
  
  END TRY
  BEGIN CATCH

 
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_insertSeriesInternal',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY
    

  END CATCH
  
END






GO
GRANT EXECUTE ON [v3d].[sp_insertSeriesInternal] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertSeriesInternal] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertSeriesReference]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/11/2009
-- Description:	Adds the UID-RUID reference as well as all the dependents of RUID as references to UID.
-- =============================================================================================================
CREATE PROCEDURE [v3d].[sp_insertSeriesReference]
(
@ID NVARCHAR(256),
@RECORD AS XML OUTPUT
)
AS
BEGIN

   DECLARE @IDOC    AS INT
   DECLARE @UID     AS NVARCHAR(256)
   DECLARE @RUID    AS NVARCHAR(256)

  -- ====================================
  -- Make sure the user can add a study
  -- ====================================
  EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @UID            = [uid],
            @RUID           = [ruid]
          FROM OPENXML(@IDOC, N'/series', 1) WITH 
          ( 
            [uid]     nvarchar(128),
            [ruid]    nvarchar(128)
         )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  -- =================================================
  -- Makes sure the refernce does not already exist 
  -- =================================================
  IF NOT EXISTS ( SELECT * FROM [v3d].[SeriesReferences] WHERE [uid] = @UID AND [ruid] = @RUID )
  BEGIN

    -- =======================================================================================================
    -- Check for the reference series existence here. The FK contraint is added here to allow for smoother 
    -- deletes.
    -- ======================================================================================================
    IF EXISTS ( SELECT * FROM [v3d].[Series] WHERE [uid] = @RUID )
    BEGIN
    
      -- Insert all the referened series depenedents
      INSERT INTO [v3d].[SeriesReferences]  ( [uid], [ruid] ) SELECT @UID, [v3d].[DependentSeries].[uid] FROM [v3d].[DependentSeries] WHERE [v3d].[DependentSeries].[duid] = @RUID 
 
      -- Insert the new refernce
      INSERT INTO [v3d].[SeriesReferences] ([uid], [ruid]) VALUES ( @UID, @RUID )
       
    END

  END
  
END





GO
GRANT EXECUTE ON [v3d].[sp_insertSeriesReference] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertSeriesReference] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2010
-- Description:	Adds the specified study tp the system. Since this includes patient information the
--              context id is required so that auditing can be performed.
--  
--              V3D2001   No study uid specified
--
-- =====================================================================================================
CREATE PROCEDURE [v3d].[sp_insertStudy] 
(
@ID AS NVARCHAR(256),           -- Context ID (Used for auditing purposes)
@AUTHORIZE AS BIT = 1,          -- Flag indicating that authorization is required (Only 0 when called from sp_insertImage)
@RECORD XML OUTPUT              -- Record to be added and returned
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @IDOC AS INT
  DECLARE @RC AS INT

  DECLARE @UID          AS NVARCHAR(128)
  DECLARE @SCANDATE     AS DATETIMEOFFSET(7)
  DECLARE @ACCESSION    AS NVARCHAR(64)
  DECLARE @PHYSICIAN    AS NVARCHAR(64)
  DECLARE @GROUP        AS NVARCHAR(32)
  DECLARE @DESCRIPTION  AS NVARCHAR(256)
  DECLARE @STUDY_ID     AS NVARCHAR(32)
  DECLARE @EXT          AS XML
  DECLARE @DIR          AS NVARCHAR(512)
  DECLARE @MEDICAL_ID   AS NVARCHAR(32)
  DECLARE @NAME         AS NVARCHAR(128)
  DECLARE @SEX          AS NVARCHAR(16) 
  DECLARE @WEIGHT       AS DECIMAL(5,2)
  DECLARE @DOB          AS DATE
  DECLARE @TRANSCOUNT   AS INT
  DECLARE @CONTEXT_LOGIN      AS NVARCHAR(16)
  
  SELECT  @RC = 0,
          @CONTEXT_LOGIN = [Security].[v3d].[fn_getLogin](@ID)

 
  BEGIN TRY

      SET XACT_ABORT ON

      IF @@TRANCOUNT = 0 
      BEGIN
        SET @TRANSCOUNT = 1
        BEGIN TRANSACTION
      END
  
 
    -- =======================================================================================================
    -- Make sure the user can add a study if the authorization flag is set. The flag is set by default but
    -- there is no need to perform multilple authroization when called from another stored procedure.
    -- =======================================================================================================
    IF @AUTHORIZE = 1
      EXEC [Security].[v3d].[sp_isAuthorized]
        @ID = @ID,
        @MASK = 'StorageManagement'

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

      SELECT  @UID          = [uid], 
              @MEDICAL_ID   = [mid],
              @NAME         = [name],
              @ACCESSION    = [an], 
              @GROUP        = [grp],
              @PHYSICIAN    = [phys],
              @SCANDATE     = [date],
              @STUDY_ID     = [id],
              @DOB          = [dob],
              @SEX          = [sex],
              @DIR          = [dir],
              @WEIGHT       = [wgt],
              @DESCRIPTION  = [desc]
              FROM OPENXML(@IDOC, N'/study', 3) 
              WITH 
              (
                [uid]             nvarchar(128),
                [mid]             nvarchar(64),
                [name]            nvarchar(128),
                [dob]             date,
                [sex]             nvarchar(10),
                [wgt]             decimal(5,2),
                [phys]            nvarchar(64),
                [an]              nvarchar(64),
                [grp]             nvarchar(32),
                [id]              nvarchar(32),
                [apps]            nvarchar(128),
                [mods]            nvarchar(128),
                [dir]             nvarchar(512),
                [desc]            nvarchar(256),
                [date]            datetimeoffset(7)
              )
   

    -- Remove the document
    EXEC sp_xml_removedocument @IDOC
    
    -- ====================================================================================================
    -- If no study uid is supplied we cannot add (900990) Study Uid: Missing or invalid
    -- ====================================================================================================
    IF @UID IS NULL OR  LEN(@UID) = 0 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Study Uid'
    
    -- ====================================================================================================
    -- If the Extended Properties were included I need to extract the inner xml to load into the database
    -- ====================================================================================================
    IF @EXT IS NOT NULL
      SET @EXT = @EXT.query('/ext/*')
            
  
    EXEC @RC = [v3d].[sp_insertStudyInternal]
      @UID          = @UID,
      @SCANDATE     = @SCANDATE,
      @PHYSICIAN    = @PHYSICIAN,
      @ID           = @STUDY_ID,
      @ACCESSION    = @ACCESSION,
      @DESCRIPTION  = @DESCRIPTION,
      @GROUP        = @GROUP,
      @NAME         = @NAME,
      @SEX          = @SEX,
      @WEIGHT       = @WEIGHT,
      @MEDICAL_ID   = @MEDICAL_ID,
      @DOB          = @DOB, 
      @DIR          = @DIR,
      @LOGIN        = @CONTEXT_LOGIN,
      @EXT          = @EXT
 
 
    IF @TRANSCOUNT > 0
      COMMIT TRANSACTION;

    RETURN @RC

  END TRY
  BEGIN CATCH
  

    IF  XACT_STATE() = -1
      ROLLBACK TRANSACTION;

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_insertStudy'


  END CATCH
  
 
END





GO
GRANT EXECUTE ON [v3d].[sp_insertStudy] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertStudy] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_insertStudyInternal]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2013
-- Description: Internal study insert. No transaction is started here assuming that the calling procedure has a transaction.
-- =============================================================================================================================
CREATE PROCEDURE [v3d].[sp_insertStudyInternal]
@UID              AS NVARCHAR(128),
@NAME             AS NVARCHAR(64),
@SCANDATE         AS DATETIMEOFFSET(7) = NULL,
@ID               AS NVARCHAR(32) = NULL,
@ACCESSION        AS NVARCHAR(64) = NULL,
@DESCRIPTION      AS NVARCHAR(256) = NULL,
@PHYSICIAN        AS NVARCHAR(64) = NULL,
@MEDICAL_ID       AS NVARCHAR(64) = NULL,
@DOB              AS NVARCHAR(8) = NULL,
@SEX              AS NVARCHAR(3) = NULL,
@WEIGHT           AS FLOAT = NULL,
@DIR              AS NVARCHAR(512) = NULL,
@GROUP            AS NVARCHAR(32) = NULL,
@LOGIN            AS NVARCHAR(32) = '',
@EXT              AS XML = NULL
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON 

  RETRY:

  DECLARE @RC           AS INT
 
  SELECT  @RC = -1



  BEGIN TRY

    BEGIN TRANSACTION INSERT_STUDY;

    -- ============================================
    -- Make sure that the study uid was specified
    -- (900990) Study Uid missing or invalid
    -- ============================================
    IF @UID IS NULL OR  LEN(@UID) = 0
  	  EXEC [master].[dbo].[sp_throwInvalidDataException] 'Study Uid' 
  
    -- ============================================
    -- Make sure that the patient name was specified
    -- (900990) Patient name missing or invalid
    -- ============================================
    IF @NAME IS NULL OR  LEN(@UID) = 0
  	  EXEC [master].[dbo].[sp_throwInvalidDataException] 'Patient Name'


    -- ===================================================
    -- Only insert if the study does not already exist
    -- ===================================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[Studies] WHERE [uid] = @UID )
    BEGIN




      -- Make sure 19000101 is not set as a date of birth
      IF @DOB IS NOT NULL AND ( @DOB = '19000101'OR ISDATE(@DOB) = 0 )
        SET @DOB = NULL;

      SELECT  @SCANDATE = ISNULL(@SCANDATE, sysdatetimeoffset()), 
              @SEX      = UPPER(ISNULL(@SEX,''))
    
      -- ========================================================
      -- If the SEX is not a single character then it is invalid
      -- so we just clear it.
      -- ========================================================
      IF LEN(@SEX) > 1
        SET @SEX = ''

 
      -- ===================================================================================================
      -- If the medical id is not supplied or blank then onlt the study uid is used otherwise we use the 
      -- <MEDICALID_UID> format
      -- ===================================================================================================   
      IF @DIR IS NULL OR LEN(@DIR) = 0 
          SET @DIR = REPLACE(@UID,'.','_')

      -- =======================================================================
      -- If the date has not time zone then set it to the the systems time-zone
      -- =======================================================================
      IF @SCANDATE IS NULL 
        SET @SCANDATE = SYSDATETIMEOFFSET()
      ELSE IF DATEPART(tz,@SCANDATE) = 0 
        SET @SCANDATE = TODATETIMEOFFSET(@SCANDATE, [v3d].[fn_getTimeZoneOffset]())


      INSERT INTO [v3d].[Studies] 
              ( [uid], [date], [phys], [id], [an], [desc], [name], [sex], [wgt], [mid], [dob], [ext], [dir], [grp] )
      VALUES  (   @UID, @SCANDATE, 
                  [v3d].[fn_formatPersonName](ISNULL(@PHYSICIAN,'')), 
                  ISNULL(@ID,''),
                  ISNULL(@ACCESSION,''), 
                  ISNULL(@DESCRIPTION,''), 
                  [v3d].[fn_formatPersonName](@NAME),
                  @SEX, 
                  ISNULL(@WEIGHT,0.0), 
                  ISNULL(@MEDICAL_ID,''), 
                  @DOB, @EXT, @DIR, 
                  ISNULL(@GROUP,'') ) 


      IF @@ROWCOUNT = 1
      BEGIN

        -- Add audit entry for failed action
        EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
             @UID = @UID, 
             @ACTION = N'add', 
             @LOGIN = @LOGIN, 
             @NAME = @NAME

      END

 
 
      -- =========================================
      -- Serialize if the study was inserted
      -- ========================================
      EXEC [v3d].[sp_serializeStudy]
          @UID = @UID
 
    END

    COMMIT TRANSACTION INSERT_STUDY;

    RETURN @RC;


  END TRY
  BEGIN CATCH

    IF XACT_STATE() = -1
      ROLLBACK TRANSACTION;
 
    --DECLARE @ERROR_MESSAGE NVARCHAR(4000);
 
    --SELECT @ERROR_MESSAGE     = ERROR_MESSAGE()

    ------ Add audit entry for failed action
    ----EXEC [Auditing].[v3d].[sp_insertStudyHistory]  
    ----     @UID = @UID, 
    ----     @ACTION = N'add', 
    ----     @LOGIN = @LOGIN, 
    ----     @NAME = @NAME,
    ----     @MESSAGE = @ERROR_MESSAGE

   
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_insertStudyInternal',
      @DEADLOCK = 1

    WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY
  
  END CATCH


END




GO
GRANT EXECUTE ON [v3d].[sp_insertStudyInternal] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_insertStudyInternal] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_lockSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2010
-- Description:	Locks a series 
-- ============================================================================================================
CREATE PROCEDURE [v3d].[sp_lockSeries] 
(
@ID  AS NVARCHAR(256),
@UID AS NVARCHAR(MAX),              -- Series UID
@DESC AS NVARCHAR(256) = '',
@TRANSID AS NVARCHAR(256) OUTPUT
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @IDOC           AS INT
  DECLARE @RC             AS INT
  DECLARE @OPEN           AS BIT
  DECLARE @LOGIN          AS NVARCHAR(32)
  DECLARE @TYPE           AS INT
  DECLARE @MSG            AS NVARCHAR(512)
  DECLARE @SUID           AS NVARCHAR(128)
  DECLARE @TRANSCOUNT     AS INT


  RETRY:


  BEGIN TRY
    

    -- Initialize variables
    SELECT  @RC = 0,
            @OPEN = 0,
            @TRANSCOUNT = @@TRANCOUNT,
            @LOGIN = [Security].[v3d].[fn_getLogin](@ID),
            @TRANSID = ISNULL(@TRANSID, CONVERT(NVARCHAR(256),NEWID()))

    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION;
  
    -- ============================================================================
    -- Cleanup any expired locks or locks that do not have an associated context
    -- ============================================================================
    EXEC [v3d].[sp_cleanupLocks]

  
    -- ========================================================================
    -- Make sure that the uid was specified (900990) Uid is missing or invalid
    -- =========================================================================
    IF @UID IS NULL OR  LEN(@UID) = 0 
  	  EXEC [master].[dbo].[sp_throwException] 900990,  'Uid' 
     
    IF RIGHT(@UID,1) <> '|'
      SET @UID =  @UID + '|'

    IF LEFT(@UID,1) <> '|'
      SET @UID = '|' + @UID 


    DECLARE @TABLE AS TABLE (	[uid] [nvarchar](128) NOT NULL )

    INSERT INTO @TABLE ( [uid] ) ( SELECT  SUBSTRING(@UID, Number + 1, CHARINDEX('|',  @UID, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UID) - 1
              AND SUBSTRING(@UID, Number, 1) = '|' ) 


    DECLARE @LOCK_UID AS NVARCHAR(128)
    DECLARE INSERT_LOCK_CURSOR CURSOR FOR  SELECT [uid] FROM @TABLE

    OPEN INSERT_LOCK_CURSOR
      
      
    -- Cursor is open
    SET @OPEN = 1
      
    -- fetch the first record
    FETCH NEXT FROM INSERT_LOCK_CURSOR INTO @LOCK_UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN
        
      DECLARE @TYPE_NAME AS NVARCHAR(32)

      -- ===========================================
      -- Check if the UID to be locked is a series
      -- ===========================================
      SELECT  @SUID = [suid], 
              @TYPE = [type], 
              @TYPE_NAME = [System].[v3d].[fn_getMaskBitName]('type',[type]) 
        FROM  [v3d].[Series] WHERE [uid] = @LOCK_UID


      -- ==================================================================
      -- If the UID was found in the series table then lock it
      -- ==================================================================
      IF @@ROWCOUNT = 1
      BEGIN

        
        SELECT @MSG = 'Series(' + @LOCK_UID + ' - ' + @TYPE_NAME + ')'

        -- =============================================================================================
        -- Check if the series specified is locked or any of its references are locked or any of its
        -- dependents are locked.
        --
        -- (900994) Series(): Locked
        -- ==============================================================================================
        IF  EXISTS ( SELECT * FROM [v3d].[Locks] WHERE [uid] = @LOCK_UID AND [tid] <> @TRANSID ) OR
            EXISTS ( SELECT * FROM [v3d].[Locks] INNER JOIN [v3d].[SeriesReferences] 
                                ON [v3d].[SeriesReferences].[ruid] = [v3d].[Locks].[uid] 
                             WHERE [v3d].[SeriesReferences].[uid] = @LOCK_UID AND [tid] <> @TRANSID ) OR
            EXISTS ( SELECT * FROM [v3d].[Locks] INNER JOIN [v3d].[SeriesReferences] 
                                ON [v3d].[SeriesReferences].[uid] = [v3d].[Locks].[uid] 
                             WHERE [v3d].[SeriesReferences].[ruid] = @LOCK_UID AND [tid] <> @TRANSID  ) 
            EXEC [master].[dbo].[sp_throwException] 900994, @MSG
    

  

        -- =====================================
        -- Insert all LOCKS for all referneces 
        -- ====================================
        INSERT INTO [v3d].[Locks] ( [uid], [suid], [ctx], [tid], [desc], [login]  ) 
              SELECT [uid], [suid], @ID, @TRANSID, @DESC, @LOGIN 
                FROM [v3d].[ReferenceSeries] 
               WHERE [ruid] = @LOCK_UID AND
                     NOT EXISTS ( SELECT * 
                                    FROM [v3d].[Locks] l
                                    WHERE  [uid] = l.[uid])

         SET @RC = @RC + @@ROWCOUNT

        -- =======================================
        -- Insert all LOCKS for all dependents
        -- ========================================
        INSERT INTO [v3d].[Locks] ( [uid], [suid], [ctx], [tid], [desc], [login]  ) 
              SELECT [uid], [suid], @ID, @TRANSID, @DESC, @LOGIN 
                FROM [v3d].[DependentSeries] 
               WHERE [duid] = @LOCK_UID AND
                     NOT EXISTS ( SELECT * 
                                    FROM [v3d].[Locks] l
                                    WHERE  [uid] = l.[uid])

  
         SET @RC = @RC + @@ROWCOUNT

        -- ===========================
        -- Insert the LOCK for series
        -- ===========================
        IF NOT EXISTS ( SELECT * FROM [v3d].[Locks] WHERE [uid] = @LOCK_UID AND [tid] = @TRANSID )
        BEGIN
          INSERT INTO [v3d].[Locks] ( [uid], [suid], [tid], [desc], [ctx], [login] ) 
                  VALUES ( @LOCK_UID, @SUID, @TRANSID, @DESC, @ID, @LOGIN );
          SET @RC = @RC + @@ROWCOUNT
        END

      END -- END ... If series
  
         
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM INSERT_LOCK_CURSOR INTO @LOCK_UID

    END  -- END WHILE
     
    -- Close the CURSOR
    CLOSE INSERT_LOCK_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE INSERT_LOCK_CURSOR

    SELECT @OPEN = 0

 
    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN @RC
  
  END TRY
  BEGIN CATCH

     
    -- ====================================================================
    -- If the cursor is still open then close and deallocte the resources
    -- ====================================================================
    IF @OPEN = 1
    BEGIN
      -- Close the CURSOR
      CLOSE INSERT_LOCK_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE INSERT_LOCK_CURSOR
    END

    IF XACT_STATE() = -1
     ROLLBACK TRANSACTION;


   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_lockSeries',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_lockSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_lockSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_lockStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 06/11/2010
-- Description:	Losck the specified studies
-- ============================================================================================================
CREATE PROCEDURE [v3d].[sp_lockStudy] 
(
@ID  AS NVARCHAR(256),
@UID AS NVARCHAR(MAX),              -- Study or Series UID
@DESC AS NVARCHAR(256) = '',
@TRANSID AS UNIQUEIDENTIFIER OUTPUT -- Returned transaction id  
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  DECLARE @IDOC         AS INT
  DECLARE @RC           AS INT
  DECLARE @OPEN         AS BIT
  DECLARE @MSG          AS NVARCHAR(256)
  DECLARE @SUID         AS NVARCHAR(128)
  DECLARE @TRANSCOUNT   AS INT

  RETRY:


  BEGIN TRY

  

    SELECT  @RC = 0,
            @OPEN = 0,
            @TRANSCOUNT = @@TRANCOUNT,
            @TRANSID = ISNULL(@TRANSID,NEWID())
 
    IF @TRANSCOUNT = 0
      BEGIN TRANSACTION
     
    -- ============================================================================
    -- Cleanup any expired locks or locks that do not have an associated context
    -- ============================================================================
    EXEC [v3d].[sp_cleanupLocks]

    -- ========================================================================
    -- Make sure that the uid was specified (900990) Uid is missing or invalid
    -- =========================================================================
    IF @UID IS NULL OR  LEN(@UID) = 0 
  	  EXEC [master].[dbo].[sp_throwInvalidDataException]  'Uid'
     
    IF RIGHT(@UID,1) <> '|'
      SET @UID =  @UID + '|'

    IF LEFT(@UID,1) <> '|'
      SET @UID = '|' + @UID 


    DECLARE @TABLE AS TABLE (	[uid] [nvarchar](128) NOT NULL )

    INSERT INTO @TABLE ( [uid] ) ( SELECT  SUBSTRING(@UID, Number + 1, CHARINDEX('|',  @UID, Number + 1) - Number -1) AS VALUE
        FROM master..spt_values
        WHERE Type = 'p'
              AND Number <= LEN(@UID) - 1
              AND SUBSTRING(@UID, Number, 1) = '|' ) 


    DECLARE @LOCK_UID AS NVARCHAR(128)
    DECLARE INSERT_LOCK_CURSOR CURSOR FOR  SELECT [uid] FROM @TABLE

    OPEN INSERT_LOCK_CURSOR
      
      
    -- Cursor is open
    SET @OPEN = 1
      
    -- fetch the first record
    FETCH NEXT FROM INSERT_LOCK_CURSOR INTO @LOCK_UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN
        
      
      -- ==================================
      -- Only lock the study if it exists
      -- ==================================
      IF EXISTS ( SELECT * FROM [v3d].[Studies] WHERE [uid] = @LOCK_UID )
      BEGIN
         
 
        SET @MSG = 'Study(' + @LOCK_UID + ')' 

        DECLARE @NUMBER_OF_LOCKED_SERIES AS INT
        DECLARE @SERIES_UIDS AS NVARCHAR(MAX)
        SET @SERIES_UIDS = '|'

        -- ===============================================
        -- Get the list of series for the specified study
        -- ===============================================
        SELECT @SERIES_UIDS = @SERIES_UIDS + [uid] + '|' FROM [v3d].[Series] WHERE [suid] = @LOCK_UID

   
        EXEC @NUMBER_OF_LOCKED_SERIES = [v3d].[sp_lockSeries]  
          @ID             = @ID,
          @UID            = @SERIES_UIDS,
          @TRANSID        = @TRANSID,
          @DESC           = @DESC


        SET @RC = @RC + @NUMBER_OF_LOCKED_SERIES

      END


         
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM INSERT_LOCK_CURSOR INTO @LOCK_UID

    END  -- END WHILE
     
    -- Close the CURSOR
    CLOSE INSERT_LOCK_CURSOR
       
    -- Deallocate the resources
    DEALLOCATE INSERT_LOCK_CURSOR

    SET @OPEN = 0

    IF @TRANSCOUNT = 0
      COMMIT TRANSACTION

    RETURN @RC
  
  END TRY
  BEGIN CATCH

    
    -- ====================================================================
    -- If the cursor is still open then close and deallocte the resources
    -- ====================================================================
    IF @OPEN = 1
    BEGIN
      -- Close the CURSOR
      CLOSE INSERT_LOCK_CURSOR
       
      -- Deallocate the resources
      DEALLOCATE INSERT_LOCK_CURSOR
    END

    IF XACT_STATE() = -1 
      ROLLBACK TRANSACTION;

   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_lockStudy',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_lockStudy] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_lockStudy] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_purgeDatabase]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
/*********************************************************************************************
  * Procedure Name:	purgeDatabase			  
  * Description:		Pyrges all processing data from the database
  ******************************************************************************************************/
CREATE PROCEDURE [v3d].[sp_purgeDatabase] 
@ID AS NVARCHAR(256)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

   
  DECLARE @UID AS NVARCHAR(256)
  DECLARE @COUNT AS INT
  DECLARE @OPEN AS BIT
  
  SET @COUNT = 0
  SET @OPEN = 0
  
  -- Delet Study Cursor
  DECLARE DELETE_STUDY_CURSOR CURSOR FOR SELECT [uid] FROM [v3d].[Studies]
    

  BEGIN TRY

    DECLARE @CONTEXT_LOGIN AS NVARCHAR(32)
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID
      
 
    -- Processing Tables
    DELETE [v3d].[Messages];
    DELETE [v3d].[Jobs];

    -- Storage Tables
    DELETE [v3d].[DicomServers];
    DELETE [v3d].[Receivers];
    DELETE [v3d].[SeriesReferences];
    DELETE [v3d].[VolumeReferences];
    DELETE [v3d].[Locks];
    DELETE [v3d].[Volumes];

      
    -- open the study cursor
    OPEN DELETE_STUDY_CURSOR
    
    -- Flag cursor as open
    SET @OPEN = 1
    
    -- fetch the first record
    FETCH NEXT FROM DELETE_STUDY_CURSOR INTO @UID

    -- loop while there are records in the cursor
    WHILE @@FETCH_STATUS = 0
    BEGIN
    
      SET @COUNT = @COUNT + 1
      
      EXEC [v3d].[sp_deleteStudy]
        @ID = @ID,
        @UID = @UID,
        @FORCE = 1
     
      -- This is executed as long as the previous fetch succeeds.
      FETCH NEXT FROM DELETE_STUDY_CURSOR INTO @UID

    END  -- END WHILE

    -- Close the CURSOR
    CLOSE DELETE_STUDY_CURSOR
   
    -- Deallocate the resources
    DEALLOCATE DELETE_STUDY_CURSOR

    -- Flas curor as closed
    SET @OPEN = 0
    
    RETURN @COUNT
    
  END TRY
  BEGIN CATCH
    
    -- =================================
    -- Close the cursor if it is open
    -- =================================
    IF @OPEN = 1
    BEGIN
      -- Close the CURSOR
      CLOSE DELETE_STUDY_CURSOR
     
      -- Deallocate the resources
      DEALLOCATE DELETE_STUDY_CURSOR
    END
     
   
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_purgeDatabase'
  
  END CATCH
     
END





GO
GRANT EXECUTE ON [v3d].[sp_purgeDatabase] TO [v3dsa] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_raisePendingJobsEvent]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 07/18/2013
-- Description:	Raise pending jobs event
-- =============================================
CREATE PROCEDURE [v3d].[sp_raisePendingJobsEvent]
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  BEGIN TRY

    DECLARE @EVENT AS NVARCHAR(128)

    -- Get the JobsPending event name
    SET @EVENT = [System].[v3d].[fn_getSetting]('PendingJobsEvent', '')


    IF LEN(@EVENT) = 0
    BEGIN

      DECLARE @DATABASE AS NVARCHAR(64)


      SELECT @DATABASE = DB_NAME()

      EXEC [master].[dbo].[sp_logErrorMessage] @DATABASE, 'sp_raisePendingJobsEven', 0, 'PendingJobsEvent was not found'

      RETURN 0

    END

    -- Trigger the named event
    EXEC [v3d].[sp_setNamedEvent] 
        @name = @EVENT

  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_raisePendingJobsEvent'


  END CATCH

END




GO
GRANT EXECUTE ON [v3d].[sp_raisePendingJobsEvent] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_raisePendingJobsEvent] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_receiveImage]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/11/2009
-- Description:	Receives the dicom image into the system and returned the dicom file path the system should
--              write the file to. Also the thumnail file will be returned only upon the intial receipt and
--              blank for all subsequent indicting that the first image is used as a thumnail for the series.
-- =============================================================================================================
CREATE PROCEDURE [v3d].[sp_receiveImage]
(
@ID AS NVARCHAR(256) = NULL,    -- Context ID (Used for auditing )
@RECORD AS XML,                 -- Record contains receiving information and dicom header
@PATHS AS XML OUTPUT            -- Paths to be returns
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	
	DECLARE @IDOC               AS INT
  DECLARE @MESSAGE_SOURCE     AS NVARCHAR(32)     -- Used for logging messages
	DECLARE @RC                 AS INT              -- Return Code
	DECLARE @RETURN             AS INT              -- Return value
	DECLARE @LOCK               AS INT              -- Lock flag
	DECLARE @FILENAME           AS NVARCHAR(512)     -- Used to store DICOM file name    
  DECLARE @FILEPATH           AS NVARCHAR(MAX)    -- Dicom Filename with full path  (Returned)
  DECLARE @THUMBNAIL          AS NVARCHAR(MAX)    -- Thumbnail filename with full path (Returned)
  DECLARE @SERVER             AS NVARCHAR(32)     -- Computer name the dicom server is running on
  DECLARE @CALLED_AETITLE     AS NVARCHAR(32)     -- Our servers AETItle        
  DECLARE @CALLING_AETITLE    AS NVARCHAR(32)     -- AE Title of the device sending the dicom
  DECLARE @ASSOCIATION        AS NVARCHAR(256)    -- Association/Receiver ID
  DECLARE @HEADER             AS XML              -- Dicom Header
  DECLARE @SERIES_ADDED       AS INT
  DECLARE @STATE              AS BIGINT           -- Holds the state of receiving for inserting the series
  DECLARE @LOCK_RESOURCE      AS NVARCHAR(128)
  DECLARE @TRUE               AS BIT
  DECLARE @FALSE              AS BIT
  DECLARE @DICOM              AS INT
 
 	
  RETRY:

  SELECT  @RC               = 0, 
          @RETURN           = 0, 
          @LOCK             = 0, 
          @SERIES_ADDED     = 0,
          @TRUE             = 1,
          @FALSE            = 0,
          @THUMBNAIL        = '', 
          @FILEPATH         = '',  
          @DICOM            = [System].[v3d].[fn_getMaskBit]('type','dicom'),
          @LOCK_RESOURCE    = 'DICOM_RECEIVING',
          @MESSAGE_SOURCE   = 'DicomServer',
          @STATE            = [v3d].[fn_getState]('receiving')
  

  -- Sets the ID to internal usage context
  --SET @ID = ISNULL(@ID, 'V3D-A43257B8-DA39-4F3A-9FFC-71A54BCA9893|Receiver' )
   

  -- =============================================================================================
  -- Wrap the remaining process in a try catch so that upon an exception the transaction can be
  -- rolled back.
  -- =============================================================================================
  BEGIN TRY
 
    SET XACT_ABORT ON                                              

    BEGIN TRANSACTION;

      -- Make sure we are authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

      -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @SERVER             = [server],
            @CALLED_AETITLE     = [calledAETitle],
            @CALLING_AETITLE    = [callingAETitle],
            @ASSOCIATION        = [assoc],
            @HEADER             = [hdr]
          FROM OPENXML(@IDOC, N'/image', 1) WITH 
          ( 
            [server]          nvarchar(32),
            [calledAETitle]   nvarchar(64),
            [callingAETitle]  nvarchar(64),
            [assoc]           nvarchar(256),
            [hdr]             xml                 './hdr'
            
         )


   -- Remove the document
   EXEC sp_xml_removedocument @IDOC

   -- Set variables accounting for NULL values
   SELECT   @SERVER = ISNULL(@SERVER,''), 
            @CALLED_AETITLE  = ISNULL(@CALLED_AETITLE,''), 
            @CALLING_AETITLE = ISNULL(@CALLING_AETITLE, ''), 
            @ASSOCIATION = ISNULL(@ASSOCIATION,'')

     
    -- Lock Dicom Receiving
    EXEC @LOCK = sp_getapplock 
              @Resource = @LOCK_RESOURCE, 
              @LockMode = 'Exclusive', 
              @LockOwner = 'Transaction', 
              @LockTimeout = 60000
                
    --=================================================================================
    -- If the application lock returns a code < 0 then the lock could be retreieved so
    -- an exception will be thrown. 
    --=================================================================================
    IF @LOCK < 0
    BEGIN
      
        -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE

      EXEC [master].[dbo].[sp_throwException] 901401, 'Receive Image'
        
    END -- END ... If the lock failed
      
   -- ==================
    -- Local variables
    -- ==================
    DECLARE @ORIENTATION  AS NVARCHAR(16)
    DECLARE @OMASK        AS BIGINT
    DECLARE @PLANE        AS NVARCHAR(32)
    DECLARE @PMASK        AS BIGINT
    DECLARE @SCAN_DATE    AS DATETIMEOFFSET(3)
    DECLARE @TIMEZONE     AS NVARCHAR(10)     -- Time zone
  
    
    -- ==============
    -- Patient data
    -- ===============
    DECLARE @PATIENT_NAME       AS NVARCHAR(64)
    DECLARE @PATIENT_MEDICAL_ID AS NVARCHAR(64)
    DECLARE @PATIENT_DOB        AS NVARCHAR(8)
    DECLARE @SEX                AS NVARCHAR(3)
    DECLARE @PATIENT_WEIGHT     AS NVARCHAR(12)
   

    
    -- ===========
    -- Study data
    -- ===========
    DECLARE @STUDY_UID              AS NVARCHAR(128)
    DECLARE @STUDY_DATE             AS NVARCHAR(8)
    DECLARE @STUDY_TIME             AS NVARCHAR(16)
    DECLARE @STUDY_ID               AS NVARCHAR(32)
    DECLARE @STUDY_ACCESSION_NUMBER AS NVARCHAR(64)
    DECLARE @STUDY_DESCRIPTION      AS NVARCHAR(256)
    DECLARE @STUDY_PHYSICIAN        AS NVARCHAR(64)
    
    
    -- =============
    --- Series data
    -- =============
    DECLARE @SERIES_UID               AS NVARCHAR(128)
    DECLARE @SERIES_DATE              AS NVARCHAR(8)
    DECLARE @SERIES_TIME              AS NVARCHAR(16)
    DECLARE @SERIES_NUMBER            AS NVARCHAR(32)
    DECLARE @SERIES_MODALITY          AS NVARCHAR(16)
    DECLARE @SERIES_DESCRIPTION       AS NVARCHAR(256)
    
    -- ==================
    -- Acquisition data
    -- ===================    
    DECLARE @ACQUISITION_DATE       AS NVARCHAR(8)
    DECLARE @ACQUISITION_TIME       AS NVARCHAR(16)

    -- ==================
    -- Image data
    -- ===================    
    DECLARE @IMAGE_DATE         AS NVARCHAR(8)
    DECLARE @IMAGE_TIME         AS NVARCHAR(16)
    DECLARE @INSTANCE_UID       AS NVARCHAR(128)
    DECLARE @IMAGE_ORIENTATION  AS NVARCHAR(128)
    

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @HEADER

    -- Extract the user information 
    SELECT  @PATIENT_NAME           = [patientName],
            @PATIENT_MEDICAL_ID     = [patientMedicalId], 
            @SEX                    = [patientSex],
            @PATIENT_DOB            = [patientDob],
            @PATIENT_WEIGHT         = [patientWeight],

            @STUDY_UID              = [studyUid],
            @STUDY_DATE             = [studyDate],
            @STUDY_TIME             = [studyTime],
            @STUDY_ID               = [studyId],
            @STUDY_ACCESSION_NUMBER = [accessionNumber],
            @STUDY_DESCRIPTION      = [studyDescription],
            @STUDY_PHYSICIAN        = [studyPhysician],

            @ACQUISITION_DATE       = [acquisitionDate],
            @ACQUISITION_TIME       = [acquisitionTime],

            @SERIES_UID             = [seriesUid],
            @SERIES_DATE            = [seriesDate],
            @SERIES_TIME            = [seriesTime],
            @SERIES_NUMBER          = [seriesNumber],
            @SERIES_MODALITY        = [seriesModality],
            @SERIES_DESCRIPTION     = [seriesDescription],
            
            @IMAGE_DATE             = [imageDate],
            @IMAGE_TIME             = [imageTime],
            @INSTANCE_UID           = [instanceUid],
            @IMAGE_ORIENTATION      = [imageOrientation],

            @TIMEZONE               = [timezone]
            
         
        FROM OPENXML(@IDOC, N'hdr/data-set', 1) WITH 
        ( 
        
          -- =============
          -- Patient data
          -- ==============
          [patientName]       nvarchar(128)   './element[@tag="0010,0010"]',
          [patientMedicalId]  nvarchar(64)    './element[@tag="0010,0020"]',
          [patientSex]        nvarchar(10)    './element[@tag="0010,0040"]',
          [patientDob]        nvarchar(8)     './element[@tag="0010,0030"]',
          [patientWeight]     nvarchar(10)    './element[@tag="0010,1030"]',

          -- ===========
          -- Study data
          -- ===========
          [studyUid]          nvarchar(128)   './element[@tag="0020,000d"]',
          [studyDate]         nvarchar(8)     './element[@tag="0008,0020"]',
          [studyTime]         nvarchar(16)    './element[@tag="0008,0030"]',
          [studyId]           nvarchar(32)    './element[@tag="0020,0010"]',
          [accessionNumber]   nvarchar(64)    './element[@tag="0008,0050"]',
          [studyDescription]  nvarchar(256)   './element[@tag="0008,1030"]',
          [studyPhysician]    nvarchar(64)    './element[@tag="0008,0090"]',

          [acquisitionDate]   nvarchar(8)     './element[@tag="0008,0022"]',
          [acquisitionTime]   nvarchar(16)    './element[@tag="0008,0032"]',
     
          [seriesUid]         nvarchar(128)   './element[@tag="0020,000e"]',
          [seriesDate]        nvarchar(8)     './element[@tag="0008,0021"]',
          [seriesTime]        nvarchar(16)    './element[@tag="0008,0031"]',
          [seriesNumber]      nvarchar(32)    './element[@tag="0020,0011"]',
          [seriesModality]    nvarchar(16)    './element[@tag="0008,0060"]',
          [seriesDescription] nvarchar(256)   './element[@tag="0008,103e"]',
          
          [imageDate]         nvarchar(8)     './element[@tag="0008,0023"]',
          [imageTime]         nvarchar(8)     './element[@tag="0008,0033"]',
          [instanceUid]       nvarchar(128)   './element[@tag="0008,0018"]',
          [imageOrientation]  nvarchar(128)   './element[@tag="0020,0037"]',

          [timezone]          nvarchar(10)    './element[@tag="0002,0201"]'

       )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    
    -- ========================================================================================================
    -- Check for missing patient name (901426) Missing dicom field (Patient Name)
    -- ========================================================================================================
    IF @PATIENT_NAME IS NULL 
      EXEC [master].[dbo].[sp_throwException]  901400,  'Patient Name'

    -- ========================================================================================================
    -- Check for missing study UID (901400) Missing dicom field (Study Uid)
    -- ========================================================================================================
    IF @STUDY_UID IS NULL
      EXEC [master].[dbo].[sp_throwException]  901400, 'Study Uid' 
 
    -- ========================================================================================================
    -- Check for missing series UID (901400) Missing dicom field (Series Uid)
    -- ========================================================================================================
    IF @SERIES_UID IS NULL
      EXEC [master].[dbo].[sp_throwException] 901400,  'Series Uid'

    -- ========================================================================================================
    -- Check for missing instance UID (901400) Missing dicom field (Instance Uid)
    -- ========================================================================================================
    IF @INSTANCE_UID IS NULL
      EXEC [master].[dbo].[sp_throwException] 901400, 'Instance Uid'
  
    -- ==========================================================================================
    -- If the receiving entry does not exists then add it otherwise increment the image count
    -- ==========================================================================================
    IF NOT EXISTS ( SELECT * FROM [v3d].[Receivers] WHERE [aetitle] = @CALLED_AETITLE AND [uid] = @SERIES_UID AND [association] = @ASSOCIATION )
    BEGIN
   
      DECLARE @SERIES_DATETIMEOFFET   AS DATETIMEOFFSET(7)
    
      -- Extract the image orientation and the plane from the COSINES
      SELECT  @ORIENTATION            = [orientation], 
              @OMASK                  = [omask],
              @PLANE                  = [plane],
              @PMASK                  = [pmask],
              @SERIES_DATETIMEOFFET   = ISNULL([v3d].[fn_convertDateTime](@SERIES_DATE,@SERIES_TIME, @TIMEZONE), sysdatetimeoffset()),
              @SCAN_DATE              = [v3d].[fn_getScanDate](@STUDY_DATE, @STUDY_TIME, @SERIES_DATE, @SERIES_TIME, @ACQUISITION_DATE, @ACQUISITION_TIME, @IMAGE_DATE, @IMAGE_TIME, @TIMEZONE )
              FROM [v3d].[fn_getOrientationInformation](@IMAGE_ORIENTATION)

  
      -- ==================================================================
      -- Handles the case when the DOB element is present with no value
      -- =================================================================
      IF ISDATE(@PATIENT_DOB) = @TRUE  
        SET @PATIENT_DOB = NULL   
  
      -- ====================================
      -- Check for an invalid patient weight
      -- ====================================
      IF ISNUMERIC(@PATIENT_WEIGHT) = @TRUE
        SET @PATIENT_WEIGHT = '0.0'
  
      -- =============================================================================================================
      -- Format the STUDY xml used to the sp_insertStudy based on the DOB. If NULL them dont include in XML otherwise
      -- set the values received. Also I check for 19000101 since that is the value that the date will be set to if 
      -- the element exists but contains no value. I want to make sure that the value is NULL in the DB.
      -- =============================================================================================================
      IF @PATIENT_DOB IS NOT NULL AND @PATIENT_DOB = '19000101'
        SET @PATIENT_DOB = NULL

   
      -- =============
      -- insert study
      -- =============
      EXEC @RC = [v3d].[sp_insertStudyInternal]
        @UID          = @STUDY_UID,
        @SCANDATE     = @SCAN_DATE,
        @PHYSICIAN    = @STUDY_PHYSICIAN,
        @ID           = @STUDY_ID,
        @ACCESSION    = @STUDY_ACCESSION_NUMBER,
        @DESCRIPTION  = @STUDY_DESCRIPTION,
        @NAME         = @PATIENT_NAME,
        @SEX          = @SEX,
        @WEIGHT       = @PATIENT_WEIGHT,
        @MEDICAL_ID   = @PATIENT_MEDICAL_ID,
        @DOB          = @PATIENT_DOB
           
 
      -- ================
      -- Insert Series
      -- =================
      EXEC @SERIES_ADDED = [v3d].[sp_insertSeriesInternal]  
        @UID          = @SERIES_UID,
        @STUDY_UID    = @STUDY_UID,
        @DATE         = @SERIES_DATETIMEOFFET,
        @ORIENTATION  = @ORIENTATION,
        @MODALITY     = @SERIES_MODALITY,
        @PLANE        = @PMASK,
        @NUMBER       = @SERIES_NUMBER,
        @DESCRIPTION  = @SERIES_DESCRIPTION,
        @STATE        = @STATE,
        @TYPE         = @DICOM,
        @SOURCE       = @CALLING_AETITLE,
        @HEADER       = @HEADER  

      EXEC @RC = [v3d].[sp_getReceivePaths]
        @SERIES_UID       = @SERIES_UID,
        @INSTANCE_UID     = @INSTANCE_UID,
        @CREATE_DIRECTORY = @SERIES_ADDED,
        @DICOM            = @FILEPATH OUTPUT,
        @THUMBNAIL        = @THUMBNAIL OUTPUT

 
      -- ============================
      -- insert the receiver record
      -- ============================
      INSERT [v3d].[Receivers] ( [aetitle], [uid], [association], [name], [ort], [server], [calling], [new]) 
        VALUES ( @CALLED_AETITLE, @SERIES_UID, @ASSOCIATION, @PATIENT_NAME, @ORIENTATION, @SERVER, @CALLING_AETITLE, 1 ) 

    END -- END ... If the receiver record exists for this series

    -- ==================================================================================================
    -- Else the receiver record exists for this series so we do not need to insert the stuyd and series
    -- ==================================================================================================
    ELSE
    BEGIN

      
      EXEC @RC = [v3d].[sp_getReceivePaths]
        @SERIES_UID       = @SERIES_UID,
        @INSTANCE_UID     = @INSTANCE_UID,
        @CREATE_DIRECTORY = 0,
        @DICOM            = @FILEPATH OUTPUT,
        @THUMBNAIL        = @THUMBNAIL OUTPUT

      -- ===========================================
      -- We only need to update the receiver record
      -- ===========================================
      IF @RC = 1
        UPDATE [v3d].[Receivers] SET [cnt] = [cnt] + 1, [new] = [new] + 1
          WHERE [aetitle] = @CALLED_AETITLE AND [uid] = @SERIES_UID AND [association] = @ASSOCIATION
      ELSe
        UPDATE [v3d].[Receivers] SET [cnt] = [cnt] + 1 
          WHERE [aetitle] = @CALLED_AETITLE AND [uid] = @SERIES_UID AND [association] = @ASSOCIATION
 
    END
 

    -- ===============================================
    -- We must release the lock if locking is enabled
    -- ================================================   
    IF @LOCK > 0
    BEGIN
      -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE
    END -- END ... If locking is enabled then release the lock

    -- We return the paths in the xml
    SET @PATHS = ( SELECT @FILEPATH AS [dicom], @THUMBNAIL AS [thumbnail] FOR XML RAW('paths') )


    COMMIT TRANSACTION;

    RETURN @RC

  END TRY
  BEGIN CATCH
    
    IF  XACT_STATE() = -1 
     ROLLBACK TRANSACTION;
  
    -- ===============================================
    -- We must release the lock if locking is enabled
    -- ================================================   
    IF @LOCK > 0
    BEGIN
      -- Lock Dicom Receiving
      EXEC sp_releaseapplock  
                @Resource = @LOCK_RESOURCE
    END -- END ... If locking is enabled then release the lock


    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_receiveImage',
      @DEADLOCK = 1

    WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
	  GOTO RETRY -- Go to Label RETRY
	
  END CATCH

END -- END ... sp_receiveDicomImage





GO
GRANT EXECUTE ON [v3d].[sp_receiveImage] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_receiveImage] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_reindexTable]    Script Date: 3/19/2014 4:44:49 PM ******/
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
/****** Object:  StoredProcedure [v3d].[sp_resetJobs]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================
-- Author:		  Mark Kontak
-- Create date: 04/19/2012
-- Description:	Resets all jobs/tasks marked as processing back to Pending
-- ========================================================================
CREATE PROCEDURE [v3d].[sp_resetJobs]
(
@ID NVARCHAR(256)       -- Context Id
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
  DECLARE @PENDING AS INT
  DECLARE @PROCESSING AS INT
	
 

  BEGIN TRY
  
   
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'



    SET @PENDING = [v3d].[fn_getState]('pending')

    SET @PROCESSING = [v3d].[fn_getState]('processing')
	

    -- =======================================
    -- Updates all series that are processing
    -- ==========================================
    UPDATE [Storage].[v3d].[Series] SET  [state] = 0 WHERE [state] = @PROCESSING
  
    -- =======================================================    
    -- Updates all jobs that have a state of PROCESSING
    --
    --    Sets
    --      State         = PENDING
    --      Start Date    = NULL (Not started)
    --      System        = BLANK
    --
    -- =======================================================
    UPDATE [v3d].[Jobs] SET [state] = @PENDING, [sys] = '', [stdte] = NULL  WHERE [state] = @PROCESSING

    RETURN @@ROWCOUNT


  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_resetJobs'

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_resetJobs] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_resetJobs] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_resetNamedEvent]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_resetNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_resetNamedEvent]
GO
/****** Object:  StoredProcedure [v3d].[sp_resetPacsJobs]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =======================================================================
-- Author:		  Mark Kontak
-- Create date: 04/19/2012
-- Description:	Resets all pacsjobs marked as processing back to Pending
-- ========================================================================
CREATE PROCEDURE [v3d].[sp_resetPacsJobs]
(
@ID NVARCHAR(256)       -- Context Id
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	
  DECLARE @PENDING AS NVARCHAR(32)
  DECLARE @PROCESSING AS NVARCHAR(32)
	


  BEGIN TRY
  
   
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'



    SET @PENDING = 'Pending'
    SET @PROCESSING = 'Processing'
	
  
    -- =======================================================    
    -- Updates all processing jobs and sets them to pending
    -- =======================================================
    UPDATE [v3d].[PacsJobs] SET [state] = @PENDING WHERE [state] = @PROCESSING

    RETURN @@ROWCOUNT


  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_resetPacsJobs'
  

  END CATCH
	              
	              
END





GO
GRANT EXECUTE ON [v3d].[sp_resetPacsJobs] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_resetPacsJobs] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_serializeSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/09/2013
-- Description:	Serializes series 
-- ======================================================================================
CREATE PROCEDURE [v3d].[sp_serializeSeries]
@UID NVARCHAR(128) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @XML AS XML


  RETRY:


  
  BEGIN TRY

    SET @XML = (  SELECT [uid], [suid], [ort], [pln], [date], [dir], [mod], [rcv], [desc], [type], [app], [icnt], [num], [src], 
                    [state], [msg], [on], [st], [proc], [ext], [v3d].[fn_getSeriesPath]('local', [uid] ) AS [loc],
                    [v3d].[fn_getVersion]('Series') AS [ver], 
              ( SELECT  [uid], [cid], [name], [date], [app], [icnt], [file], [ext], [v3d].[fn_getVersion]('Volumes') AS [ver]  
                        FROM [v3d].[Volumes] 
                        WHERE [v3d].[Volumes].[uid] = [v3d].[Series].[uid] FOR XML RAW('volume'), TYPE, ROOT('volumes') ),  
                  ( SELECT [uid], [suid], [ort], [pln], [date], [dir], [mod], [rcv], [desc], [type], [app], [icnt], [num], [src], 
                          [state], [on], [st], [proc], [ext], [v3d].[fn_getVersion]('Series') AS [ver]  
                          FROM [v3d].[ReferenceSeries] WHERE [v3d].[ReferenceSeries].[ruid] = [v3d].[Series].[uid]
                          FOR XML RAW('series'), TYPE, ROOT('references') ) FROM [v3d].[Series] WHERE [v3d].[Series].[uid] = @UID
                              FOR XML RAW('series'), ROOT('collection') )

    EXEC [v3d].[sp_serializeXml]
      @XML = @XML

  END TRY
  BEGIN CATCH

 
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_serializeSeries'
 
  
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_serializeSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_serializeSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_serializeStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/09/2013
-- Description:	Serializes studies contained in the uid list. This list is delimited 
--              by '|'
-- ======================================================================================
CREATE PROCEDURE [v3d].[sp_serializeStudy]
@UID NVARCHAR(128) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
  SET XACT_ABORT ON
  
   
  DECLARE @XML AS XML
 
  
  BEGIN TRY

     -- Get XML   
    SET @XML = ( SELECT [uid], [date], [mid], [name], [dob], [sex] [id], [wgt], 
                        [an], [desc], [grp], [phys],
                        [v3d].[fn_getVersion]('Studies') AS [ver],
                        [System].[v3d].[fn_pathCombine]([v3d].[fn_getRoot]('local','default'),  [dir], '' ) AS [loc],
                        [ext]
                      FROM [v3d].[Studies] WHERE [uid] = @UID
                        FOR XML RAW('study'), ROOT('collection') );
    
    -- Serialize the xml to the specified location
    EXEC [v3d].[sp_serializeXml] 
      @XML = @XML
 
  END TRY
  BEGIN CATCH

    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_serializeStudy'
 
     
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_serializeStudy] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_serializeStudy] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_serializeXml]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_serializeXml]
	@xml [xml]
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_serializeXml]
GO
/****** Object:  StoredProcedure [v3d].[sp_serverStarted]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================
-- Author:		  Mark Kontak
-- Create date: 07/14/20111
-- Description:	Adds the dicom server information to the table
-- ===============================================================
CREATE PROCEDURE [v3d].[sp_serverStarted]
@AETITLE NVARCHAR(32),      -- AE Title of the server
@SERVER NVARCHAR(32),       -- Name of the server/machine the server is running
@PORT INT                   -- Port the server is lisetning on
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- ======================================================
  -- Make sure that the the server does not already exist
  -- ======================================================
  IF EXISTS ( SELECT * FROM [v3d].[DicomServers] WHERE [aetitle] = @AETITLE )
  BEGIN
    UPDATE [v3d].[DicomServers] SET [date] = SYSDATETIMEOFFSET() WHERE [aetitle] = @AETITLE
  END
  ELSE
  BEGIN
    INSERT INTO [v3d].[DicomServers] ( [aetitle], [server], [port] ) VALUES ( @AETITLE, @SERVER, @PORT )
  END
  
  
  RETURN @@ROWCOUNT
  
END





GO
GRANT EXECUTE ON [v3d].[sp_serverStarted] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_serverStarted] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_setNamedEvent]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE PROCEDURE [v3d].[sp_setNamedEvent]
	@name [nvarchar](4000)
WITH EXECUTE AS CALLER
AS
EXTERNAL NAME [Viatronix.Sql.Storage].[StoredProcedures].[sp_setNamedEvent]
GO
/****** Object:  StoredProcedure [v3d].[sp_setSeriesState]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ========================================================
-- Author:		  Mark Kontak
-- Create date: 10/24/2013
-- Description:	Sets the series state (Checks the Jobs )
-- ========================================================
CREATE PROCEDURE [v3d].[sp_setSeriesState]
@UID AS NVARCHAR(128),              -- Series UID
@STATE AS BIGINT = 0                -- State to set series to
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @JOB_STATES AS BIGINT

  
  -- =========================================================================================================
  -- Extract the STATE from the SOURCE and DESTINTAION is not set which means this JOB is a conversion and 
  -- the SOURCE is the DICOM series.
  -- =========================================================================================================
  SELECT @STATE = @STATE | [state] FROM [v3d].[Jobs] WHERE [src] = @UID AND [dst] = '' 

 
  -- ============================================================
  -- If the UID is not in the SOURCE then check the DESTINATION
  -- ============================================================
  IF @@ROWCOUNT = 0
    SELECT @STATE = @STATE | [state] FROM [v3d].[Jobs] WHERE [dst] = @UID


  UPDATE [v3d].[Series]
     SET [state] = @STATE
   WHERE [uid] = @UID
   
 
END



GO
/****** Object:  StoredProcedure [v3d].[sp_startJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description:	Starts the job
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_startJob]
(
@JOBID NVARCHAR(256)
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
	DECLARE @RC AS INT

	BEGIN TRY
    


    DECLARE @JID      AS UNIQUEIDENTIFIER
    DECLARE @APP      AS NVARCHAR(16)
    DECLARE @PROCESSING AS INT
    DECLARE @PENDING    AS INT
  

     
    -- ======================================
    -- Makes sure the JOB id was specified
    -- (900990) Job Id: Missing or Invalid
    -- =======================================
    IF @JOBID IS NULL OR LEN(@JOBID) = 0
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Job Id'

    SET @JID = CONVERT(UNIQUEIDENTIFIER, @JOBID)

    DECLARE @SRC    AS NVARCHAR(128)
    DECLARE @DST    AS NVARCHAR(128)
    DECLARE @UID    AS NVARCHAR(128)

    -- Extract data from the current job (will be used to seed the next job)
    SELECT  @DST          = [dst],                  -- Destination UID
            @SRC          = [src],                  -- Source UID
            @PENDING      = [v3d].[fn_getState]('pending'),
            @PROCESSING   = [v3d].[fn_getState]('processing')
            FROM [v3d].[Jobs] WHERE [id] = @JID


    -- ========================================================
    -- If the JOB is not found then throw a NOT FOUND EXECTION
    -- ========================================================
    IF @@ROWCOUNT = 0
      EXEC [master].[dbo].[sp_throwNotFoundException] 'Job', @JOBID

   
    -- ======================================================================
    -- Sets the JOBS state to processing
    -- =====================================================================
    UPDATE [v3d].[jobs] SET   [state]  = @PROCESSING, 
                              [prog]   = 0,
                              [stage]  = '',
                              [stdte]  = SYSDATETIMEOFFSET()
                        WHERE [id] = @JID
 
    -- ============================================================================================
    -- If the DST (Destination UID) is not set then we are doing a conversion and the UID is the 
    -- DICOM series.
    -- =============================================================================================
    IF LEN(@DST) = 0 
      SET @UID = @SRC
    ELSE
      SET @UID = @DST
      
    -- =============================================  
    -- Update the series stsate
    --   Set to PROCESSING
    --   Unset PENDING
    -- ==========================================       
    UPDATE [v3d].[Series] 
      SET [state] = ( [state] | @PENDING | @PROCESSING ) ^ @PENDING 
    WHERE [uid] = @UID
         
    -- ============================
    -- Serialize the updated series
    -- ============================
    EXEC [v3d].[sp_serializeSeries]
      @UID = @UID
   
    RETURN 1 
        
    
  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_startJob'
    

  END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_startJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_startJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description:	Updates the job 
--
--        Can only update
--          STATE
--          PROGRESS
--          STAGE
--          
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_updateJob]
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
	DECLARE @RC AS INT

	BEGIN TRY
    
 

   -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'


    --DECLARE @LOGIN AS NVARCHAR(16)
    DECLARE @STATE    AS INT
    DECLARE @JID      AS UNIQUEIDENTIFIER
    DECLARE @PROG     AS FLOAT
    DECLARE @STAGE    AS NVARCHAR(64)
    DECLARE @MESSAGE  AS NVARCHAR(512)
    DECLARE @APP      AS NVARCHAR(16)
    DECLARE @TID      AS NVARCHAR(32)
    DECLARE @SYS      AS NVARCHAR(32)
    DECLARE @COMPLETED  AS INT
    DECLARE @FAILED     AS INT
    DECLARE @PROCESSING AS INT
    DECLARE @PENDING    AS INT
    DECLARE @ABORTED    AS INT

                
 
     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @JID              = [id],
            @STATE            = [state],
            @STAGE            = [stage],
            @TID              = [tid],
            @PROG             = [prog],
            @SYS              = [sys],
            @MESSAGE          = [msg]
            FROM OPENXML(@IDOC, N'/job', 3) 
            WITH 
            (
              [id]              uniqueidentifier,
              [stage]           nvarchar(64),
              [app]             nvarchar(16),
              [state]           int,
              [prog]            float,
              [tid]             nvarchar(32),
              [sys]             nvarchar(32),
              [msg]             nvarchar(512)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
    -- ======================================
    -- Makes sure the JOB id was specified
    -- (900990) Job Id: Missing or Invalid
    -- =======================================
    IF @JID IS NULL 
      EXEC [master].[dbo].[sp_throwInvalidDataException] 'Job Id'



     -- ======================================================
    -- Extract the states masks from the Systems.Masks table
    -- ======================================================
    SELECT  @COMPLETED    = [v3d].[fn_getState]('completed'),
            @FAILED       = [v3d].[fn_getState]('failed'),
            @PROCESSING   = [v3d].[fn_getState]('processing'),
            @PENDING      = [v3d].[fn_getState]('pending'),
            @ABORTED      = [v3d].[fn_getState]('aborted')

    -- =========================
    -- JOB is completed
    -- ========================
    IF @STATE = @COMPLETED 
    BEGIN

      EXEC [v3d].[sp_completeJob]
        @JOBID = @JID


    END -- END ... If the jobs state is being set to completed

    -- ==============
    -- JOB Failed
    -- ==============
    ELSE IF @STATE = @FAILED 
    BEGIN

         
     EXEC [v3d].[sp_failJob]
        @JOBID = @JID,
        @MESSAGE = @MESSAGE

    END

    -- ==============
    -- JOB Processing
    -- ==============
    ELSE IF @STATE = @PROCESSING 
    BEGIN
   
      EXEC [v3d].[sp_startJob]  
        @JOBID = @JID

    END
    


    -- =====================
    -- JOB Progress Changed
    -- =====================
    ELSE  
    BEGIN

        
        
      -- ========================================================================
      -- Updates the job and only set the TID if it is not blank and not null
      -- ========================================================================
      IF LEN(@TID) = 0 
         UPDATE [v3d].[jobs] SET   [state]  = ISNULL(@STATE,[state]), 
                                   [prog]   = ISNULL(@PROG,[prog]),
                                   [stage]  = ISNULL(@STAGE,[stage]),
                                   [tid]    = ISNULL(@TID,[tid]),
                                   [sys]    = ISNULL(@SYS,[sys])
                             WHERE [id] = @JID
      ELSE
        UPDATE [v3d].[jobs] SET    [state]  = ISNULL(@STATE,[state]), 
                                   [prog]   = ISNULL(@PROG,[prog]),
                                   [stage]  = ISNULL(@STAGE,[stage]),
                                   [sys]    = ISNULL(@SYS,[sys])
                             WHERE [id] = @JID
  
 
    END


    RETURN 1 
        
    
  END TRY
  BEGIN CATCH
  
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateJob' 

  END CATCH

END






GO
GRANT EXECUTE ON [v3d].[sp_updateJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updatePacsJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 08/26/2009
-- Description:	Updates the pacs job
-- ====================================================================
CREATE PROCEDURE [v3d].[sp_updatePacsJob]
(
@ID NVARCHAR(256),             -- Context ID
@RECORD XML OUTPUT             -- Task to be returned    
) 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;
	

  DECLARE @IDOC AS INT
	DECLARE @RC AS INT
	
	BEGIN TRY
  
    
    -- Make sure the context use is authorize
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'


    DECLARE @STATE AS NVARCHAR(16)
    DECLARE @JID AS INT
 

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD


    SELECT  @JID              = [id],
            @STATE            = [state]
            FROM OPENXML(@IDOC, N'/pacsjob', 3) 
            WITH 
            (
              [id]              int,
              [state]           nvarchar(16)
            )
       
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC    
    
    -- ===========================================
    -- Makes sure the JOB id was specified
    -- (9000990) Job Id: Missing or invalid
    -- ===========================================
    IF @JID IS NULL 
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Job Id'
 

       
    -- Updates the job
    UPDATE [v3d].[Pacsjobs] SET  [state]  = ISNULL(@STATE,[state])
                        WHERE [id] = @JID

    SET @RC = @@ROWCOUNT


     



    RETURN @RC 
        
    
  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]
      @PROCEDURE = 'sp_updatePacsJob'
 
  END CATCH

END





GO
GRANT EXECUTE ON [v3d].[sp_updatePacsJob] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updatePacsJob] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateSession]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/25/2011
-- Description:	Updates the session
-- =============================================
CREATE PROCEDURE [v3d].[sp_updateSession]
@ID NVARCHAR(256),
@RECORD XML
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  BEGIN TRANSACTION

 
  DECLARE @WRAP AS BIT
  DECLARE @RC AS INT
  DECLARE @OPEN AS INT

  SET @WRAP = 1
  SET @OPEN = [v3d].[fn_getState]('open')

  BEGIN TRY

     
    -- ========================================
    -- Make sure the context use is authorize
    -- ========================================
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'StorageManagement'

          
    DECLARE @IDOC AS INT

    DECLARE @UID AS NVARCHAR(256)
    DECLARE @STATE AS INT


    DECLARE @SERIES AS XML

     
    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @UID        = [uid],
            @STATE      = [state]
          FROM OPENXML(@IDOC, N'/session', 1) WITH 
          ( 
            [uid]            nvarchar(256),
            [state]          int
          )
         
      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

    SET @STATE = ISNULL(@STATE,0)

    -- =============================================
    -- Make sure the UID was specified
    -- (900990) Series Uid: Missing or invalid
    -- =============================================
    IF @UID IS NULL OR ( @UID IS NOT NULL AND LEN(@UID) = 0 )
      EXEC [master].[dbo].[sp_throwInvalidDataException]  'Series Uid'
 
    -- Updates the series state with the new state. Will mask out the open bit if present and remask so that if the 
     UPDATE [v3d].[Series] SET [state] =  @STATE WHERE [uid] = @UID AND [type] = [System].[v3d].[fn_getMaskBit]('type','session')

     
   
    RETURN @@ROWCOUNT
   
  END TRY
  BEGIN CATCH
     
   EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateSession'
  
  END CATCH
  
END





GO
GRANT EXECUTE ON [v3d].[sp_updateSession] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateSession] TO [v3duser] AS [dbo]
GO
/****** Object:  StoredProcedure [v3d].[sp_updateWorkQueue]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================================
-- Author:		  Mark Kontak
-- Create date: 07/09/2013
-- Description:	Updates the conversion process by adding the volumes contained
--              in the @RECORD (Conversion Results) paremeter.
--            
--              
-- =============================================================================
CREATE PROCEDURE [v3d].[sp_updateWorkQueue]
@ID     NVARCHAR(256),     -- Context ID
@RECORD XML                -- Conversion results 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;


  RETRY:

  BEGIN TRY
 
    
    -- Make sure that the the user is authorized
    EXEC [Security].[v3d].[sp_isAuthorized]
      @ID = @ID,
      @MASK = 'ProcessingManagement'

    DECLARE @IDOC     AS INT
    DECLARE @TASK     AS NVARCHAR(32)
    DECLARE @CID      AS NVARCHAR(32)
    DECLARE @JID      AS NVARCHAR(256)
    DECLARE @APP      AS NVARCHAR(16)
    DECLARE @SUB      AS NVARCHAR(32)
    DECLARE @RESULTS  AS XML
    DECLARE @VOLUME   AS XML
    DECLARE @NAME     AS NVARCHAR(128)
    DECLARE @COUNT    AS INT
    DECLARE @UID      AS NVARCHAR(128)
    DECLARe @FILE     AS NVARCHAR(512)
    DECLARE @DESC     AS NVARCHAR(512)
    DECLARE @EXT      AS XML
    DECLARE @OPEN     AS BIT

    SELECT @OPEN = 0

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @RECORD

    -- Extract the user information 
    SELECT  @TASK     = LOWER([task]),
            @APP      = [app],
            @JID      = [jid],
            @RESULTS  = [results]
          FROM OPENXML(@IDOC, N'/workqueue', 1) WITH 
          ( 
            [task]    nvarchar(128),
            [app]     nvarchar(128),
            [jid]     nvarchar(256),
            [results] xml './pipeline'
          )
         
    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

    -- ===================================================================
    -- If no task was specified we do not know how to process the results
    -- ===================================================================
    IF @TASK IS NULL OR LEN(@TASK) = 0 
  	  EXEC [master].[dbo].[sp_throwInvalidDataException]  'Task'
 

    -- ========================================================================================
    -- Only the conversion has results at this moment, however other entries may be needed 
    -- =========================================================================================
    IF @TASK = 'conversion'
    BEGIN
     
    -- Cursor to hold the conversions volume xml 
    DECLARE  VOLUME_CURSOR CURSOR LOCAL FOR SELECT n.query('.') FROM @RESULTS.nodes('pipeline/volumes/volume') AS a(n)

    -- Open cursor
    OPEN VOLUME_CURSOR

    -- Set to open
    SET @OPEN = 1

    -- Get the forst row
    FETCH NEXT FROM VOLUME_CURSOR INTO @VOLUME


    -- ========================================
    -- Loop until all rows have been processed
    -- ========================================
    WHILE @@FETCH_STATUS = 0 
    BEGIN
  
      -- Read the volume xml
      EXEC sp_xml_preparedocument @IDOC OUTPUT, @VOLUME

      SELECT  @CID      = [id],
              @NAME     = [name], 
              @APP      = [app],
              @SUB      = [sub],
              @COUNT    = [icnt], 
              @UID      = [uid],
              @FILE     = [file],
              @DESC     = [desc],
              @EXT      = [ext]
              FROM OPENXML(@IDOC, N'/volume', 3) 
              WITH 
              (
                [id]              nvarchar(32),
                [name]            nvarchar(32),
                [sub]             nvarchar(64),
                [app]             nvarchar(16),
                [icnt]            int,
                [uid]             nvarchar(128),
                [file]            nvarchar(512),
                [desc]            nvarchar(128),
                [ext]             xml  './ext'

              )
   

      -- Remove the document
      EXEC sp_xml_removedocument @IDOC

      -- Make sure there is a conversion id
      IF @CID IS NULL
        EXEC [master].[dbo].[sp_throwInvalidDataException] 'Conversion Id'

      -- Make sure there is a volume name
      IF @NAME IS NULL
        EXEC [master].[dbo].[sp_throwInvalidDataException] 'Volume Name'

      -- Make sure there is a series uid
      IF @UID IS NULL
        EXEC [master].[dbo].[sp_throwInvalidDataException] 'Uid'

      -- =====================================================
      -- If the volume exists then we will update them
      -- =====================================================
      IF EXISTS ( SELECT * FROM [v3d].[Volumes] WHERE [cid] = @CID AND [uid] = @UID AND [name] = @NAME )
      BEGIN
        
        -- ====================================
        -- Updates the volumes current values
        -- ===================================
        UPDATE [v3d].[Volumes] 
           SET [icnt] = @COUNT,
               [file] = @FILE,
               [desc] = @DESC,
               [ext]  = @EXT,
               [sub]  = ISNULL(@SUB,''),
               [app]  = ISNULL(@APP,'')
        WHERE  [cid]  = @CID AND [uid] = @UID AND [name] = @NAME 

      END

      -- ========================================================
      -- Else if the volue does not exist then insert a new one
      -- =======================================================
      ELSE
      BEGIN

        -- ==================
        -- Insert new volume
        -- ==================
        INSERT 
          INTO [v3d].[Volumes] 
               ( [cid], [uid], [name], [app], [sub], [icnt], [file], [ext], [desc] )
        VALUES ( @CID, @UID, @NAME, ISNULL(@APP,''), ISNULL(@SUB,''), @COUNT, @FILE, @EXT, @DESC )

      END


      FETCH NEXT FROM VOLUME_CURSOR INTO @VOLUME


    END -- END .. While there are volume rows to process


    CLOSE VOLUME_CURSOR
    DEALLOCATE VOLUME_CURSOR

    SET @OPEN = 0

    --- Sets the CID in the job
    UPDATE [v3d].[Jobs] SET [cid] = ISNULL(@CID,[cid]) WHERE [id] = @JID 
 
    -- =========================================
    -- Only serialize if the UID contains data
    -- =========================================
    IF @UID IS NOT NULL AND LEN(@UID) > 0
       EXEC [v3d].[sp_serializeSeries]
          @UID = @UID

    END


  END TRY
  BEGIN CATCH
  
	
    EXEC [v3d].[sp_errorHandler]    
      @PROCEDURE =  'sp_updateWorkQueue',
      @DEADLOCK = 1

		WAITFOR DELAY '00:00:00.05' -- Wait for 5 ms
		GOTO RETRY -- Go to Label RETRY

  END CATCH
END




GO
GRANT EXECUTE ON [v3d].[sp_updateWorkQueue] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[sp_updateWorkQueue] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildPatientDirectory]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 06/15/2011
-- Description: Creates the patient directory
-- =============================================
CREATE FUNCTION [v3d].[fn_buildPatientDirectory] 
(
@LNAME AS NVARCHAR(64),   -- Last name
@FNAME AS NVARCHAR(64),   -- First name
@MID AS NVARCHAR(64)      -- Medical Id
)
RETURNS NVARCHAR(128)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @DIR AS NVARCHAR(128)

  SET @DIR = @LNAME
    
  IF LEN(@FNAME) > 0
  BEGIN
    IF LEN(@DIR) > 0
      SET @DIR = @DIR + '_' + @FNAME
    ELSE
      SET @DIR = @FNAME
  END
  
  IF LEN(@MID) > 0
  BEGIN
    IF LEN(@DIR) > 0
      SET @DIR = @DIR + '_' + @MID
    ELSE
      SET @DIR = @MID
  END  

	-- Return the result of the function
	RETURN REPLACE(REPLACE(REPLACE(REPLACE(@DIR,'''',''),'`',''),'.','_'),' ','_')
	

END





GO
GRANT EXECUTE ON [v3d].[fn_buildPatientDirectory] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildPatientDirectory] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildPatientName]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
/*****************************************************************************************************************************************************************
 * Function:  PATIENTNAME()
 *
 * Description:  Converts the LAST_NAME, FIRST_NAME and MIDDLE_NAME into a formatted PATIENT_NAME. 
 *                      Format:   LAST_NAME, FIRST_NAME MIDDLE_NAME.
 *
 *	           If the FIRST_NAME is blank then only the LAST_NAME will appear.
 *****************************************************************************************************************************************************************/
CREATE FUNCTION [v3d].[fn_buildPatientName] 
(
@LAST_NAME nvarchar(256), 
@FIRST_NAME nvarchar(256), 
@MIDDLE_NAME nvarchar(256)
) 
RETURNS nvarchar(256) AS  
BEGIN
  DECLARE @PATIENT_NAME AS NVARCHAR(256);
 
  IF  LEN(@FIRST_NAME) > 0 
    IF LEN(@MIDDLE_NAME) > 0 
      SET @PATIENT_NAME = @LAST_NAME + ', ' + @FIRST_NAME + ' ' + @MIDDLE_NAME
    ELSE
      SET @PATIENT_NAME = @LAST_NAME + ', ' + @FIRST_NAME
  ELSE
    SET @PATIENT_NAME = @LAST_NAME
 
  RETURN @PATIENT_NAME;	   
END





GO
GRANT EXECUTE ON [v3d].[fn_buildPatientName] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildPatientName] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_buildSessionLaunchList]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Builds the session launch list
-- =============================================
CREATE FUNCTION [v3d].[fn_buildSessionLaunchList]
(
@SESSION_UID AS NVARCHAR(128),
@RULES AS XML,
@TRANSID AS NVARCHAR(256)
)
RETURNS XML
AS
BEGIN
  
  -- Returns value
  DECLARE @XML AS XML
  DECLARE @BASE_PATH AS NVARCHAR(512)
  

	-- Builds the session launch list
  SET @XML = ( SELECT  [v3d].[Series].[uid], @TRANSID as [tid], [v3d].[fn_getSeriesPath]('remote',[v3d].[Series].[uid]) AS [path],
                          [v3d].[fn_getDatasets](@SESSION_UID, @RULES)
                           FROM [v3d].[Series] 
                          WHERE [v3d].[Series].[uid] = @SESSION_UID 
                            AND [v3d].[Series].[type] = [System].[v3d].[fn_getMaskBit]('type','session') 
                            FOR XML RAW('session'), TYPE ) 
                          
	-- Return the result of the function
	RETURN @XML

END





GO
GRANT EXECUTE ON [v3d].[fn_buildSessionLaunchList] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_buildSessionLaunchList] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_combineStrings]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 08/15/2013
-- Description:	Combines the first parameter with the second putting a comma between
-- ====================================================================================
CREATE FUNCTION [v3d].[fn_combineStrings]
(
@VALUES NVARCHAR(MAX),
@VALUE NVARCHAR(16)
)
RETURNS NVARCHAR(MAX)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @RETURN AS NVARCHAR(MAX)

  SELECT  @VALUES = ISNULL(@VALUES,''),
          @VALUE = ISNULL(@VALUE,'')

  IF LEN(@VALUES) = 0
    SET @RETURN = @VALUE
  ELSE 
  BEGIN
    IF PATINDEX('%|' + @VALUE + '%', @VALUES) = 0 AND
       PATINDEX('%' + @VALUE + '|%', @VALUES) = 0 AND
       @VALUES <> @VALUE AND 
       LEN(@VALUE) > 0
      SET @RETURN  = @VALUES + '|' + @VALUE
    ELSE
      SET @RETURN = @VALUES
  END

	-- Return the result of the function
	RETURN @RETURN

END





GO
GRANT EXECUTE ON [v3d].[fn_combineStrings] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_combineStrings] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_convertDateTime]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================================
-- Author:		  Mark Kontak
-- Create date: 06/15/2010
-- Description: Formats a datetimeoffset type from a dicom date and time string
-- ==============================================================================
CREATE FUNCTION [v3d].[fn_convertDateTime]
(
@DATE AS NVARCHAR(8),
@TIME AS NVARCHAR(10),
@TIMEZONE AS NVARCHAR(10)
)
RETURNS DATETIMEOFFSET(3)
AS
BEGIN
	DECLARE @DATETIME AS DATETIMEOFFSET(3)
	
	SET @DATETIME = NULL


  IF @TIMEZONE IS NULL 
    SET @TIMEZONE = [v3d].[fn_getTimeZoneOffset]()
  ELSE
    SET @TIMEZONE = SUBSTRING(@TIMEZONE,1,3) + ':' + SUBSTRING(@TIMEZONE,3,2)

  -- ==============================
  -- Make sure the date is valid
  -- ==============================
  IF ISDATE(@DATE) = 1 AND LEN(@DATE) =  8 
  BEGIN
    IF @TIME IS NULL OR ( @TIME IS NOT NULL AND LEN(@TIME) < 6 ) 
    BEGIN
      SET @TIME = '000000'
    END
    
    DECLARE @TMP_DATE NVARCHAR(32)
    DECLARE @HOUR     NVARCHAR(2)
    DECLARE @MIN      NVARCHAR(2)
    DECLARE @SEC      NVARCHAR(2)
    DECLARE @TMP      INT

    SET @HOUR = '00'
    SET @MIN = '00'
    SET @SEC = '00'
    
    SET @HOUR = SUBSTRING(@TIME,1,2)
    IF ISNUMERIC(@HOUR) = 1
    BEGIN
    
      SET @TMP = CONVERT(INT,@HOUR)
      
      IF @HOUR < 25
      BEGIN
        SET @MIN = SUBSTRING(@TIME,3,2)
        IF ISNUMERIC(@MIN) = 1
        BEGIN
          SET @TMP= CONVERT(INT,@MIN)
          IF @MIN < 60
          BEGIN
            SET @SEC = SUBSTRING(@TIME,5,2)
            IF ISNUMERIC(@SEC) = 1
            BEGIN
              SET @TMP = CONVERT(INT,@SEC)
              
              IF @TMP > 59
                SET @SEC = '00'
              
            END
            ELSE
              SET @SEC = '00'
          END
          ELSE
            SET @MIN = '00'
        END
        ELSE
          SET @MIN = '00'
      END
        SET @HOUR = '00'  
    END
    ELSE
      SET @HOUR = '00'
    
   
     
    SET @TMP_DATE = SUBSTRING(@DATE,1,4) + '-' + SUBSTRING(@DATE,5,2) + '-' + SUBSTRING(@DATE,7,2) + 'T' + 
                    @HOUR + ':' + @MIN + ':' + SUBSTRING(@TIME,5,2) + @TIMEZONE
                    
    SET @DATETIME = CONVERT(DATETIMEOFFSET(7),@TMP_DATE);
    
  END
  
  
  RETURN  @DATETIME
  
END





GO
GRANT EXECUTE ON [v3d].[fn_convertDateTime] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_convertDateTime] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_formatName]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/09/2010
-- Description:	Formats a name from the parameters passed in ( LAST, FIRST MIDDLE )
-- ====================================================================================
CREATE FUNCTION [v3d].[fn_formatName]
(
@LAST NVARCHAR(64),
@FIRST NVARCHAR(64),
@MIDDLE NVARCHAR(64)
)
RETURNS NVARCHAR(128)
AS
BEGIN

	DECLARE @NAME NVARCHAR(128)

  SET @NAME = ''
  
	-- Format the name
	IF LEN(@LAST) > 0 
	BEGIN
	  SET @NAME = @LAST 
	  
	  IF LEN(@FIRST) > 0 
	  BEGIN
	    SET @NAME = @NAME + ', ' + @FIRST + ' ' + @MIDDLE
	    SET @NAME = RTRIM(@NAME)
	  END
	    
	END
	ELSE
	BEGIN
	  SET @NAME = @FIRST + ' ' + @MIDDLE
	  SET @NAME = LTRIM(@NAME)
	END
	
	

	-- Return the name
	RETURN @NAME

END





GO
GRANT EXECUTE ON [v3d].[fn_formatName] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_formatName] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_formatPersonName]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Reformats the DICOM name to Last, First M
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_formatPersonName] 
( 
@NAME NVARCHAR(128) 
)
RETURNS NVARCHAR(512) 
BEGIN

  DECLARE @NAME_TABLE AS TABLE
  (
  [lastName]    NVARCHAR(64),
  [firstName]   NVARCHAR(64),
	[middleName]  NVARCHAR(64),
	[prefix]      NVARCHAR(32),
	[suffix]      NVARCHAR(32)
  )


  DECLARE @FORMATTED_NAME NVARCHAR(512)  
  DECLARE @LAST AS NVARCHAR(64)
  DECLARE @FIRST AS NVARCHAR(64)
  DECLARE @MIDDLE AS NVARCHAR(64)
  
  DECLARE @INDEX AS INT
  DECLARE @START AS INT
  
  SET @LAST = ''
  SET @FIRST = ''
  SET @MIDDLE = ''

  -- Strip off MOD: is received by one customer
  SET @NAME = REPLACE(@NAME,'MOD:','') 

  
  --============================================================
  -- Handle the dicom format or LAST^FIRST^MIDDLE^SUFFIX^PREFIX
  --=============================================================
  IF PATINDEX('%^%',@NAME) > 0 
  BEGIN

    -- Find the third '.' in the string
    SET @INDEX = CHARINDEX('^',@NAME)
      
    IF @INDEX = 0
    BEGIN
      SET @LAST = @NAME
    END
    ELSE
    BEGIN
      SET @LAST = SUBSTRING(@NAME,1,@INDEX - 1)
      SET @START = @INDEX + 1
      SET @NAME = SUBSTRING(@NAME,@START, LEN(@NAME) - @INDEX)
    END
    
    SET @NAME = ' ' + REVERSE(REPLACE(@NAME,'^','.')) + ' '
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    SELECT @LAST AS LAST_NAME, 
            LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME,1)),''))) AS FIRST_NAME, 
              LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 2)),''))) AS MIDDLE_NAME,
                 LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 3)),''))) AS PREFIX,
                   LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 4)),''))) AS SUFFIX

  END
  ELSE IF PATINDEX('%,%',@NAME) > 0 
  BEGIN

    -- Find the third '.' in the string
    SET @INDEX = CHARINDEX(',',@NAME)
    
    IF @INDEX = 0
    BEGIN
      SET @LAST = @NAME
    END
    ELSE
    BEGIN
      
      SET @FIRST = ''
      SET @MIDDLE = ''
      
      SET @LAST = SUBSTRING(@NAME,1,@INDEX - 1)
      
      SET @START = @INDEX + 1
      SET @NAME = LTRIM(SUBSTRING(@NAME,@START, LEN(@NAME) - @INDEX))
      
      -- Find the third '.' in the string
      SET @INDEX = CHARINDEX(' ',@NAME)
        
      IF @INDEX = 0
      BEGIN
        SET @FIRST = @NAME
      END
      ELSE
      BEGIN
        SET @FIRST = SUBSTRING(@NAME,1,@INDEX - 1)
        
        SET @START = @INDEX + 1
        SET @MIDDLE = SUBSTRING(@NAME, @START, LEN(@NAME) - @INDEX)
        
      END
      
    END
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    VALUES ( LTRIM(RTRIM(@LAST)), LTRIM(RTRIM(@FIRST)), LTRIM(RTRIM(@MIDDLE)), '', '' )


  END
  ELSE
  BEGIN
    
    SET @LAST = @NAME
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    VALUES ( LTRIM(RTRIM(@LAST)), LTRIM(RTRIM(@FIRST)), LTRIM(RTRIM(@MIDDLE)), '', '' )

  END
   
  	

  SET @FORMATTED_NAME = ''
  
  SELECT @LAST = lastName, @FIRST = firstName, @MIDDLE = middleName FROM @NAME_TABLE
  
	-- Format the name
	IF LEN(@LAST) > 0 
	BEGIN
	  SET @FORMATTED_NAME = @LAST 
	  
	  IF LEN(@FIRST) > 0 
	  BEGIN
	    SET @FORMATTED_NAME = @FORMATTED_NAME + ', ' + @FIRST + ' ' + @MIDDLE
	    SET @FORMATTED_NAME = RTRIM(@FORMATTED_NAME)
	  END
	    
	END
	ELSE
	BEGIN
	  SET @FORMATTED_NAME = @FIRST + ' ' + @MIDDLE
	  SET @FORMATTED_NAME = LTRIM(@FORMATTED_NAME)
	END

  RETURN @FORMATTED_NAME

END





GO
GRANT EXECUTE ON [v3d].[fn_formatPersonName] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_formatPersonName] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getCoordinates]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z codinates from the image orientation dicom field
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_getCoordinates]
(
@IMAGE_ORIENTATION NVARCHAR(32)
)
RETURNS @COORDINATE TABLE 
(
  [x] FLOAT,
  [y] FLOAT,
	[z] FLOAT
)

BEGIN
  
  
  SET @IMAGE_ORIENTATION = REPLACE(@IMAGE_ORIENTATION,'.',',')

  IF PATINDEX('%/%', @IMAGE_ORIENTATION) > 0 
  BEGIN
    SET @IMAGE_ORIENTATION = REPLACE(@IMAGE_ORIENTATION,'/','.') 
  END
  ELSE
  BEGIN
    SET @IMAGE_ORIENTATION = REPLACE(@IMAGE_ORIENTATION,'\','.') 
  END

  INSERT INTO @COORDINATE ( z, y, x ) 
  SELECT REPLACE(PARSENAME(@IMAGE_ORIENTATION,1),',','.') AS Z,
         REPLACE(PARSENAME(@IMAGE_ORIENTATION,2),',','.') AS Y, 
         REPLACE(PARSENAME(@IMAGE_ORIENTATION,3),',','.') AS X
 

  
  RETURN




END





GO
GRANT SELECT ON [v3d].[fn_getCoordinates] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[fn_getCoordinates] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getCosines]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z cosines for the row and columns
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_getCosines]
(
@COSINES NVARCHAR(128)
)
RETURNS @COSINES_TABLE TABLE 
(
  [row_x] FLOAT,
  [row_y] FLOAT,
	[row_z] FLOAT,
	[col_x] FLOAT,
	[col_y] FLOAT,
	[col_z] FLOAT
)
BEGIN
  
  DECLARE @INDEX AS INT

  IF PATINDEX('%/%',@COSINES) > 0 
  BEGIN
    -- Find the third '/' in the string
    SET @INDEX = CHARINDEX('/',@COSINES,CHARINDEX('/',@COSINES,CHARINDEX('/',@COSINES,1) + 1) + 1)
  END
  ELSE
  BEGIN
    -- Find the third '\' in the string
    SET @INDEX = CHARINDEX('\',@COSINES,CHARINDEX('\',@COSINES,CHARINDEX('\',@COSINES,1) + 1) + 1)
  END

  DECLARE @ROW AS NVARCHAR(32)
  DECLARE @COLUMN AS NVARCHAR(32)
  
  SET @ROW = SUBSTRING(@COSINES,1,@INDEX - 1)
  SET @COLUMN = SUBSTRING(@COSINES, @INDEX + 1, LEN(@COSINES) - @INDEX)
  
  DECLARE @ROW_X AS FLOAT
  DECLARE @ROW_Y AS FLOAT
  DECLARE @ROW_Z AS FLOAT
  DECLARE @COL_X AS FLOAT
  DECLARE @COL_Y AS FLOAT
  DECLARE @COL_Z AS FLOAT
  
  SELECT @ROW_X = x, @ROW_Y = y, @ROW_Z = z  FROM [v3d].[fn_getCoordinates](@ROW)  
  
  SELECT @COL_X = x, @COL_Y = y, @COL_Z = z FROM [v3d].[fn_getCoordinates](@COLUMN) 
 

  INSERT @COSINES_TABLE ( row_x, row_y, row_z, col_x, col_y, col_z ) VALUES 
                      ( @ROW_X, @ROW_Y, @ROW_Z, @COL_X, @COL_Y, @COL_Z )

  RETURN




END





GO
GRANT SELECT ON [v3d].[fn_getCosines] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[fn_getCosines] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDatabase]    Script Date: 3/19/2014 4:44:49 PM ******/
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

  SET @NAME = 'Storage'
  
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
/****** Object:  UserDefinedFunction [v3d].[fn_getDatasets]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Generates the referenced series launching list
-- ===========================================================
CREATE FUNCTION [v3d].[fn_getDatasets]
(
@UID   AS NVARCHAR(128),
@RULES AS XML
)
RETURNS XML
AS
BEGIN

  DECLARE @INNER AS NVARCHAR(MAX)
  DECLARE @ORT AS NVARCHAR(32)
  DECLARE @DICOM_UID AS NVARCHAR(128)
  DECLARE @PATH AS NVARCHAR(MAX)
  DECLARE @XML AS XML

  SET @INNER = ''
  
  
  -- Gets the list of unique dicom orientations in the series references 
  DECLARE DATASETS_CURSOR CURSOR LOCAL FOR SELECT DISTINCT [ruid], LOWER([ort])
                                         FROM [v3d].[ReferenceSeries]
                                        WHERE [uid] = @UID 
                                          AND [type] = [System].[v3d].[fn_getMaskBit]('type','dicom')

            

  -- open the study cursor
  OPEN DATASETS_CURSOR

  -- fetch the first record
  FETCH NEXT FROM DATASETS_CURSOR INTO @DICOM_UID, @ORT

  -- =============================================
  -- loop while there are records in the cursor
  -- =============================================
  WHILE @@FETCH_STATUS = 0
  BEGIN
    

    SELECT @PATH = [v3d].[fn_getSeriesPath]('remote',@DICOM_UID) 
    
    SET @XML = ( SELECT LOWER([ort]) AS [ort], [uid], @PATH AS [path], 
                       ( SELECT [name], [System].[v3d].[fn_pathCombine](@PATH,'volumes','') AS [path]
                              FROM [v3d].[Volumes] NOLOCK
                             WHERE [uid] = @DICOM_UID FOR XML RAW('volume'), TYPE ),
                        [v3d].[fn_getDatasetSeries]([uid], @UID, 'remote')   
                  FROM [v3d].[Series] NOLOCK
                 WHERE [uid] = @DICOM_UID  
                   FOR XML RAW('dataset'), TYPE )
      
    SET @INNER = @INNER + CONVERT(NVARCHAR(MAX),@XML)
    
    -- fetch the first record
    FETCH NEXT FROM DATASETS_CURSOR INTO @DICOM_UID, @ORT
    
  END

  CLOSE DATASETS_CURSOR
  DEALLOCATE DATASETS_CURSOR

  SET @XML = @INNER
  
  RETURN @XML
  
END





GO
GRANT EXECUTE ON [v3d].[fn_getDatasets] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getDatasets] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDatasetSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ============================================================
-- Author:		  Mark Kontak
-- Create date: 10/17/2011
-- Description:	Generates the referenced series launching list
-- ===========================================================
CREATE FUNCTION [v3d].[fn_getDatasetSeries]
(
@UID AS NVARCHAR(128),
@RUID AS NVARCHAR(128),
@LOCATIOIN AS NVARCHAR(64)
)
RETURNS XML
AS
BEGIN

  DECLARE @INNER AS NVARCHAR(MAX)
  DECLARE @TYPE AS INT
  DECLARE @TYPE_NAME AS NVARCHAR(32)
  DECLARE @SUID AS NVARCHAR(128)
  DECLARE @XML AS XML
  DECLARE @DICOM AS INT
  DECLARE @SESSION AS INT
  
  SELECT @INNER = '',
         @DICOM = [System].[v3d].[fn_getMaskBit]('type','dicom'),
         @SESSION = [System].[v3d].[fn_getMaskBit]('type','session')

  
  SELECT @TYPE = [type], @TYPE_NAME = [System].[v3d].[fn_getMaskBitName]('type',[type]) FROM [v3d].[Series] WHERE [uid] = @UID



  IF @TYPE = @DICOM
  BEGIN
  
    -- define the study selecte cursor
    DECLARE DEPENDENT_SERIES_CURSOR CURSOR LOCAL FOR SELECT [v3d].[DependentSeries].[duid], [v3d].[DependentSeries].[type], [System].[v3d].[fn_getMaskBitName]('type',[v3d].[DependentSeries].[type]) 
                                           FROM [v3d].[DependentSeries]
                                          WHERE [uid] = @UID 
                                            AND [System].[v3d].[fn_isMaskBitSet]([state],'state','failed') <> 1 
                                            AND EXISTS ( SELECT * FROM [v3d].[SeriesReferences] 
                                                                 WHERE [uid] = @RUID  AND [ruid] = [v3d].[DependentSeries].[duid]  )
                                            AND [v3d].[DependentSeries].[type] <> @SESSION

    -- open the study cursor
    OPEN DEPENDENT_SERIES_CURSOR

    -- fetch the first record
    FETCH NEXT FROM DEPENDENT_SERIES_CURSOR INTO @SUID, @TYPE, @TYPE_NAME

    -- ============================================
    -- loop while there are records in the cursor
    -- ============================================
    WHILE @@FETCH_STATUS = 0
    BEGIN
    
     SET @XML = ( SELECT [uid], [v3d].[fn_getSeriesPath](@LOCATIOIN,@SUID) AS [url]
                     FROM [v3d].[Series] NOLOCK
                    WHERE [uid] = @SUID
                      FOR XML RAW('series'), TYPE )
    
    
      SET @INNER = @INNER + REPLACE(CONVERT(NVARCHAR(1024),@XML),'<series','<' + @TYPE_NAME)
    
      -- fetch the first record
      FETCH NEXT FROM DEPENDENT_SERIES_CURSOR INTO @SUID, @TYPE, @TYPE_NAME
    
    END

    CLOSE DEPENDENT_SERIES_CURSOR
    DEALLOCATE DEPENDENT_SERIES_CURSOR

  END
  ELSE
  BEGIN


     -- define the study selecte cursor
    DECLARE REFERNECE_SERIES_CURSOR CURSOR LOCAL FOR SELECT [v3d].[ReferenceSeries].[ruid], [v3d].[ReferenceSeries].[type], [System].[v3d].[fn_getMaskBitName]('type',[v3d].[ReferenceSeries].[type]) 
                                           FROM [v3d].[ReferenceSeries]
                                          WHERE [uid] = @UID 
                                            AND [System].[v3d].[fn_isMaskBitSet]([state],'state','failed') <> 1 
                                            AND EXISTS ( SELECT * FROM [v3d].[SeriesReferences] 
                                                                 WHERE [uid] = @RUID  AND [ruid] = [v3d].[ReferenceSeries].[uid]  )
                                            AND [v3d].[ReferenceSeries].[type] <> @SESSION AND [v3d].[ReferenceSeries].[type] <> @DICOM
                                              

    -- open the study cursor
    OPEN REFERNECE_SERIES_CURSOR

    -- fetch the first record
    FETCH NEXT FROM REFERNECE_SERIES_CURSOR INTO @SUID, @TYPE, @TYPE_NAME

    -- ============================================
    -- loop while there are records in the cursor
    -- ============================================
    WHILE @@FETCH_STATUS = 0
    BEGIN
    
     SET @XML = ( SELECT [uid], [v3d].[fn_getSeriesPath](@LOCATIOIN,@SUID) AS [url]
                     FROM [v3d].[Series] NOLOCK
                    WHERE [uid] = @SUID
                      FOR XML RAW('series'), TYPE )
    
    
      SET @INNER = @INNER + REPLACE(CONVERT(NVARCHAR(1024),@XML),'<series','<' + @TYPE_NAME)
    
      -- fetch the first record
      FETCH NEXT FROM REFERNECE_SERIES_CURSOR INTO @SUID, @TYPE, @TYPE_NAME
    
    END

    CLOSE REFERNECE_SERIES_CURSOR
    DEALLOCATE REFERNECE_SERIES_CURSOR

    SELECT @TYPE = [type], @TYPE_NAME = [System].[v3d].[fn_getMaskBitName]('type',[type])  FROM [v3d].[Series] WHERE [uid] = @UID 

    IF @@ROWCOUNT > 0 AND @TYPE <> @DICOM 
    BEGIN
      SET @XML = ( SELECT [uid], [v3d].[fn_getSeriesPath](@LOCATIOIN,@UID) AS [url]
                       FROM [v3d].[Series] NOLOCK
                      WHERE [uid] = @UID
                        FOR XML RAW('series'), TYPE )
    
    
        SET @INNER = @INNER + REPLACE(CONVERT(NVARCHAR(1024),@XML),'<series','<' + @TYPE_NAME)


    END
    
  END


  SET @XML = @INNER
  
  RETURN @XML
  
END





GO
GRANT EXECUTE ON [v3d].[fn_getDatasetSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getDatasetSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getDefaultApplication]    Script Date: 3/19/2014 4:44:49 PM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_getGender]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 06/16/2010
-- Description:	Returns the Gender using the dicom SEX value
---
--                  M   Male
--                  F   Female
--                  O   Other
-- ==============================================================
CREATE FUNCTION [v3d].[fn_getGender]
(
@SEX AS NVARCHAR(4)
)
RETURNS NVARCHAR(10)
AS
BEGIN
	DECLARE @GENDER AS NVARCHAR(10)

  -- Make sure the SEX parameter is not NULL
  IF @SEX IS NULL
    SET @SEX = ''
    
  -- Set to upper case
  SET @SEX = UPPER(@SEX)
  
  -- Male
  IF @SEX = 'M'
    SET @GENDER = 'Male'
  
  -- Female
  ELSE IF @SEX = 'F'
    SET @GENDER = 'Female'
    
  -- Other
  ELSE IF @SEX = 'O'
    SET @GENDER = 'Other'
    
  -- Unknown
  ELSE 
    SET @GENDER = ''

	-- Return the result of the function
	RETURN @GENDER

END





GO
GRANT EXECUTE ON [v3d].[fn_getGender] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getGender] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getJobProcess]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE FUNCTION [v3d].[fn_getJobProcess] 
(
@STATE nvarchar(32), 
@PROCESS nvarchar(32), 
@NEXT_PROCESS nvarchar(32), 
@SERIES_PROCESS nvarchar(32)
)  
RETURNS nvarchar(32) AS  
BEGIN 

  DECLARE @JOBPROCESS AS NVARCHAR(32)

  SET   @JOBPROCESS = @NEXT_PROCESS  

  IF @STATE = 'Starting'
    SET @JOBPROCESS = @SERIES_PROCESS
  ELSE IF @STATE = '' OR @STATE = 'Completed'
    SET @JOBPROCESS = @PROCESS
  

  RETURN @JOBPROCESS
  
END





GO
GRANT EXECUTE ON [v3d].[fn_getJobProcess] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getJobProcess] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getNumberOfDependentSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 06/22/2011
-- Description: Gets the number of dependent series for the UID specified
-- =========================================================================
CREATE FUNCTION [v3d].[fn_getNumberOfDependentSeries]
(
@UID AS NVARCHAR(128)
)
RETURNS INT AS  
BEGIN
  DECLARE @COUNT AS INT
  
  SELECT @COUNT = COUNT(*) FROM [v3d].[SeriesReferences] WHERE [ruid] = @UID
  
  RETURN @COUNT
END





GO
GRANT EXECUTE ON [v3d].[fn_getNumberOfDependentSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getNumberOfDependentSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getNumberOfReferenceSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
-- =========================================================================
-- Author:		  Mark Kontak
-- Create date: 06/22/2011
-- Description: Gets the number of references series for the UID specified
-- =========================================================================
CREATE FUNCTION [v3d].[fn_getNumberOfReferenceSeries]
(
@UID AS NVARCHAR(128)
)
RETURNS INT AS  
BEGIN
  DECLARE @COUNT AS INT
  
  SELECT @COUNT = COUNT(*) FROM [v3d].[SeriesReferences] WHERE [uid] = @UID
  
  RETURN @COUNT
END





GO
GRANT EXECUTE ON [v3d].[fn_getNumberOfReferenceSeries] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getNumberOfReferenceSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getOrientationInformation]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z cosines for the row and columns
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_getOrientationInformation]
(
@COSINES NVARCHAR(128)
)
RETURNS @ORIENTATION_PLANE_TABLE TABLE 
(
  [orientation] NVARCHAR(32),
  [omask] INT,
  [plane] NVARCHAR(32),
  [pmask] INT
)
BEGIN
  
  DECLARE @ORIENTATION AS NVARCHAR(32)
  DECLARE @PLANE AS NVARCHAR(32)
  DECLARE @ROW_X AS FLOAT
  DECLARE @ROW_Y AS FLOAT
  DECLARE @ROW_Z AS FLOAT
  DECLARE @COL_X AS FLOAT
  DECLARE @COL_Y AS FLOAT
  DECLARE @COL_Z AS FLOAT
  
  SET @ORIENTATION = ''
  SET @PLANE = ''
  
  IF @COSINES IS NOT NULL AND LEN(@COSINES) > 0 
  BEGIN

    SELECT @ROW_X = row_x, @ROW_Y = row_y, @ROW_Z = row_z, @COL_X = col_x, @COL_Y = col_y, @COL_Z = col_z FROM [v3d].[fn_getCosines](@COSINES) 
    
    -- ============================
    -- HFP  (Head First Prone)
    --    -1\0\0  : 0\-1\0
    -- =============================
    IF ( @ROW_X < 0 AND @COL_Y < 0  )
    BEGIN
      SET @ORIENTATION = 'HFP'
    END
    -- =============================
    -- FFP (Feet First Prone)
    --    1\0\0  : 0\-1\0   
    -- =============================
    ELSE IF ( @ROW_X > 0 AND @COL_Y < 0 ) 
    BEGIN
      SET @ORIENTATION = 'FFP'
    END  
    -- ==========================
    -- HFS (Head First Supine)
    --    1\0\0  : 0\1\0    
    -- ==========================
    ELSE IF ( @ROW_X > 0 AND @COL_Y > 0 )
    BEGIN
      SET @ORIENTATION = 'HFS'
    END
    -- ==========================
    -- FFS (Heet First Supine)
    --    -1\0\0  : 0\1\0    
    -- ==========================
    ELSE IF ( @ROW_X < 0 AND @COL_Y > 0 )
    BEGIN
      SET @ORIENTATION = 'FFS'
    END
    -- =====================================
    -- HFDL   (Head First Decubitus-Left)
    --    0\-1\0 : 1\0\0    
    -- =====================================
    ELSE IF ( @ROW_Y < 0 AND @COL_X > 0 ) 
    BEGIN
      SET @ORIENTATION = 'HFDL'
    END
    -- =====================================
    -- FFDL   (Feet First Decubitus-Left)
    --    0\1\0  : 1\0\0    
    -- =====================================
    ELSE IF ( @ROW_Y > 0 AND @COL_X > 0 ) 
    BEGIN
      SET @ORIENTATION = 'FFDL'
    END
    -- =====================================
    -- HFDR   (Head First Decubitus-Right)
    --    0\1\0  : -1\0\0       
    -- =====================================
    ELSE IF ( @ROW_Y > 0 AND @COL_X < 0 )
    BEGIN
      SET @ORIENTATION = 'HFDR'
    END
    -- =====================================
    -- FFDR   (Feet First Decubitus-Right)
    --    0\-1\0 : -1\0\0          
    -- =====================================
    ELSE IF ( @ROW_Y < 0 AND @COL_X < 0 )
    BEGIN
      SET @ORIENTATION = 'FFDR'
    END
    ELSE
    BEGIN
      SET @ORIENTATION = ''
    END

    IF ( @ROW_Z = 0 AND @COL_Z = 0 ) 
    BEGIN
      SET @PLANE = 'axial';
    END
    ELSE IF ( @ROW_X = 0 AND @COL_X = 0 )
    BEGIN
      SET @PLANE  = 'sagittal';
    END
    ELSE IF ( @ROW_Y = 0 AND @COL_Y = 0 )
    BEGIN
      SET @PLANE  = 'coronal';
    END
    ELSE
    BEGIN
      SET @PLANE = ''
    END
  
  END

  INSERT @ORIENTATION_PLANE_TABLE ( [omask], [orientation], [pmask], [plane] ) 
         VALUES ( [System].[v3d].[fn_getMaskBit]('orientation',@ORIENTATION), @ORIENTATION, 
                  [System].[v3d].[fn_getMaskBit]('plane',@PLANE) , @PLANE )

  RETURN

END





GO
GRANT SELECT ON [v3d].[fn_getOrientationInformation] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[fn_getOrientationInformation] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getPersonName]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the DICOM person name field into a LAST_NAME, FIRST_NAME and MIDDLE_NAME
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_getPersonName] ( @NAME NVARCHAR(64) )
RETURNS @NAME_TABLE TABLE 
(
  [lastName]    NVARCHAR(64),
  [firstName]   NVARCHAR(64),
	[middleName]  NVARCHAR(64),
	[prefix]      NVARCHAR(32),
	[suffix]      NVARCHAR(32)
)

BEGIN

  
  DECLARE @LAST AS NVARCHAR(64)
  DECLARE @FIRST AS NVARCHAR(64)
  DECLARE @MIDDLE AS NVARCHAR(64)
  
  DECLARE @INDEX AS INT
  DECLARE @START AS INT
  
  SET @LAST = ''
  SET @FIRST = ''
  SET @MIDDLE = ''

  -- Strip off MOD: is received by one customer
  SET @NAME = REPLACE(@NAME,'MOD:','') 

  
  --============================================================
  -- Handle the dicom format or LAST^FIRST^MIDDLE^SUFFIX^PREFIX
  --=============================================================
  IF PATINDEX('%^%',@NAME) > 0 
  BEGIN

    -- Find the third '.' in the string
    SET @INDEX = CHARINDEX('^',@NAME)
    
    IF @INDEX = 0
    BEGIN
      SET @LAST = @NAME
    END
    ELSE
    BEGIN
      SET @LAST = SUBSTRING(@NAME,1,@INDEX - 1)
      SET @START = @INDEX + 1
      SET @NAME = SUBSTRING(@NAME,@START, LEN(@NAME) - @INDEX)
    END
    
    SET @NAME = ' ' + REVERSE(REPLACE(@NAME,'^','.')) + ' '
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    SELECT @LAST AS LAST_NAME, 
            LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME,1)),''))) AS FIRST_NAME, 
              LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 2)),''))) AS MIDDLE_NAME,
                 LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 3)),''))) AS PREFIX,
                   LTRIM(RTRIM(ISNULL(REVERSE(PARSENAME(@NAME, 4)),''))) AS SUFFIX

  END
  ELSE IF PATINDEX('%,%',@NAME) > 0 
  BEGIN

    -- Find the third '.' in the string
    SET @INDEX = CHARINDEX(',',@NAME)
    
    IF @INDEX = 0
    BEGIN
      SET @LAST = @NAME
    END
    ELSE
    BEGIN
      
      SET @FIRST = ''
      SET @MIDDLE = ''
      
      SET @LAST = SUBSTRING(@NAME,1,@INDEX - 1)
      
      SET @START = @INDEX + 1
      SET @NAME = LTRIM(SUBSTRING(@NAME,@START, LEN(@NAME) - @INDEX))
      
      -- Find the third '.' in the string
      SET @INDEX = CHARINDEX(' ',@NAME)
        
      IF @INDEX = 0
      BEGIN
        SET @FIRST = @NAME
      END
      ELSE
      BEGIN
        SET @FIRST = SUBSTRING(@NAME,1,@INDEX - 1)
        
        SET @START = @INDEX + 1
        SET @MIDDLE = SUBSTRING(@NAME, @START, LEN(@NAME) - @INDEX)
        
      END
      
    END
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    VALUES ( LTRIM(RTRIM(@LAST)), LTRIM(RTRIM(@FIRST)), LTRIM(RTRIM(@MIDDLE)), '', '' )


  END
  ELSE
  BEGIN
    
    SET @LAST = @NAME
    
    INSERT @NAME_TABLE ( lastName, firstName, middleName, prefix, suffix ) 
    VALUES ( LTRIM(RTRIM(@LAST)), LTRIM(RTRIM(@FIRST)), LTRIM(RTRIM(@MIDDLE)), '', '' )

  END
   
  RETURN

END





GO
GRANT SELECT ON [v3d].[fn_getPersonName] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[fn_getPersonName] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getPlaneCoordinate]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 01/07/2011
-- Description:
-- =============================================
CREATE FUNCTION [v3d].[fn_getPlaneCoordinate]
(
@PLANE AS INT,
@POS AS NVARCHAR(512)
)
RETURNS FLOAT
AS
BEGIN
	
	DECLARE @X AS FLOAT
	DECLARE @Y AS FLOAT
	DECLARE @Z AS FLOAT
	DECLARE @RETURN AS FLOAT
	
	SELECT @X = X, @Y = Y, @Z = Z FROM [v3d].[fn_getCoordinates](@POS)

  IF [System].[v3d].[fn_getMaskBit]('plane','coronal' ) = @PLANE  
    SET @RETURN =  @Y
  ELSE IF [System].[v3d].[fn_getMaskBit]('plane','saggital' ) = @PLANE  
    SET @RETURN =  @X
  ELSE
    SET @RETURN = @Z

  RETURN @RETURN
  
END





GO
GRANT EXECUTE ON [v3d].[fn_getPlaneCoordinate] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getPlaneCoordinate] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getPowerOf]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================
-- Author:		  Mark Kontak
-- Create date: 01/07/2011
-- Description:	Get the what power value was used on the 2 to
--              get the value
-- =============================================================
CREATE FUNCTION [v3d].[fn_getPowerOf]
(
@VALUE AS INT
)
RETURNS INT
AS
BEGIN

  DECLARE @POWER AS INT
  SET @POWER = 0


  WHILE ( @VALUE > 1 )
  BEGIN

    SET @VALUE = @VALUE / 2
    
    SET @POWER = @POWER + 1 
    
  END
  
  
	-- Return the result of the function
	RETURN @POWER

END





GO
GRANT EXECUTE ON [v3d].[fn_getPowerOf] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getPowerOf] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getRoot]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================================
-- Author:		  Mark Kontak 
-- Create date: 06/17/2010
-- Description:	Gets the root using the storgae device and the type. The 
--              default type is remote assuming remote systems are accessing the
--              system. Otherwise local for serever specific operations.
-- =================================================================================
 CREATE FUNCTION [v3d].[fn_getRoot]
(
@LOCATION NVARCHAR(8) = 'remote',      -- Remote is the default
@STORE    NVARCHAR(16)
)
RETURNS NVARCHAR(32)
AS
BEGIN

  DECLARE @ROOT AS NVARCHAR(32)
  
  SET @ROOT = ''
  
  IF LOWER(@LOCATION) = 'local' 
  BEGIN
    SELECT @ROOT = [local] FROM [System].[v3d].[Stores] NOLOCK WHERE [name] = @STORE
  END
  ELSE
  BEGIN
    SELECT @ROOT = [remote] FROM [System].[v3d].[Stores] NOLOCK WHERE [name] = @STORE
  END
  
	-- Return the result of the function
	RETURN @ROOT

END





GO
GRANT EXECUTE ON [v3d].[fn_getRoot] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getRoot] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getScanDate]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================================
-- Author:		  Mark Kontak
-- Create date: 06/15/2010
-- Description:	Returns the scan date using the dates supplied
-- =============================================================
CREATE FUNCTION [v3d].[fn_getScanDate]
(
@STUDY_DATE       NVARCHAR(8),
@STUDY_TIME       NVARCHAR(10),
@SERIES_DATE      NVARCHAR(8),
@SERIES_TIME      NVARCHAR(10),
@ACQUISITION_DATE NVARCHAR(8),
@ACQUISITION_TIME NVARCHAR(10),
@IMAGE_DATE       NVARCHAR(8),
@IMAGE_TIME       NVARCHAR(10),
@TIMEZONE         NVARCHAR(10)
)
RETURNS DATETIMEOFFSET(3)
AS
BEGIN

	DECLARE @SCAN_DATE AS DATETIMEOFFSET(3)

  -- Study date/time
  SET @SCAN_DATE = [v3d].[fn_convertDateTime](@STUDY_DATE, @STUDY_TIME, @TIMEZONE )
  
  IF @SCAN_DATE IS NULL 
  BEGIN
  
    --- Series date/time
    SET @SCAN_DATE = [v3d].[fn_convertDateTime](@SERIES_DATE, @SERIES_TIME, @TIMEZONE )
    
    IF @SCAN_DATE IS NULL
    BEGIN
    
      -- Acquistion date/time
      SET @SCAN_DATE = [v3d].[fn_convertDateTime](@ACQUISITION_DATE, @ACQUISITION_TIME, @TIMEZONE )
      
      IF @SCAN_DATE IS NULL
      BEGIN
      
        -- Image date/time
        SET @SCAN_DATE = [v3d].[fn_convertDateTime](@IMAGE_DATE, @IMAGE_TIME, @TIMEZONE )
        
    
        IF @SCAN_DATE IS NULL
        BEGIN
        
          -- Current date/time
          SET @SCAN_DATE = SYSDATETIMEOFFSET()
          
        END -- If the image date/time is invalid or missing
        
      END -- If the acquisition date/time is invalid or missing
      
    END -- If the series date/time is invalid or missing
    
  END -- If the study date/time is invalid or missinf
  
  
  RETURN @SCAN_DATE
  
  
END





GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSeriesBasedStudyColumns]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Gets the extra study columns ( [mods], [apps] and [state] )
-- =================================================================================================
CREATE FUNCTION [v3d].[fn_getSeriesBasedStudyColumns]
(
@UID NVARCHAR(128)
)
RETURNS @STUDYCOLUMNS TABLE 
(
  [state] BIGINT,
  [mods]  NVARCHAR(128),
	[apps]  NVARCHAR(128)
)

BEGIN
  

  DECLARE @STATE AS BIGINT
  DECLARE @APPS AS NVARCHAR(128)
  DECLARE @MODS AS NVARCHAR(128)
  
  SELECT @STATE = 0,
         @APPS = '',
         @MODS = ''

  SELECT  @STATE = @STATE | [state],
          @APPS = [v3d].[fn_combineStrings](@APPS,[app]),
          @MODS = [v3d].[fn_combineStrings](@MODS,[mod])
    FROM [v3d].[Series] 
    WHERE [suid] = @UID 

   SELECT @APPS = [v3d].[fn_combineStrings](@APPS,[v3d].[Volumes].[app])
    FROM [v3d].[Volumes] 
    INNER JOIN [v3d].[Series] ON [v3d].[Volumes].[uid] = [v3d].[Series].[uid] 
    WHERE [suid] = @UID 

  INSERT INTO @STUDYCOLUMNS ( [state], [apps], [mods] ) 
  SELECT @STATE, @APPS, @MODS
 

  
  RETURN




END





GO
GRANT SELECT ON [v3d].[fn_getSeriesBasedStudyColumns] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[fn_getSeriesBasedStudyColumns] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getSeriesPath]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2010
-- Description:	Retreives the series path including the default store
-- ====================================================================
CREATE FUNCTION [v3d].[fn_getSeriesPath]
(
@LOCATION AS NVARCHAR(16),    -- 'local' or 'remote'
@UID      AS NVARCHAR(128)     -- uid of series or study you want the location for
)
RETURNS NVARCHAR(MAX)
AS
BEGIN

	-- Declare the return variable here
	DECLARE @RESULTS AS NVARCHAR(MAX)
	
	SELECT @RESULTS = [System].[v3d].[fn_pathCombine]( [v3d].[fn_getRoot](@LOCATION, [v3d].[Series].[st]), [v3d].[Studies].[dir] , [v3d].[Series].[dir] )
      FROM [v3d].[Series] 
        INNER JOIN [v3d].[Studies] ON [v3d].[Series].[suid] = [v3d].[Studies].[uid]
          WHERE [v3d].[Series].[uid] = @UID
  
	-- Return the result of the function
	RETURN @RESULTS

END





GO
GRANT EXECUTE ON [v3d].[fn_getSeriesPath] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getSeriesPath] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getState]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 06/06/2012
-- Description: Gets the state
-- =============================================
CREATE FUNCTION [v3d].[fn_getState]
(
@NAME NVARCHAR(32)
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @MASK AS BIGINT

	SET @MASK = [System].[v3d].[fn_getMaskBit]('state',@NAME)

	-- Return the result of the function
	RETURN @MASK

END





GO
GRANT EXECUTE ON [v3d].[fn_getState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getState] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getStudyApplications]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/28/2013
-- Description:	Gets the studies state
-- =============================================
CREATE FUNCTION [v3d].[fn_getStudyApplications]
(
@UID NVARCHAR(256)        -- Study UID
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @APPS AS BIGINT

  SET @APPS = 0

 -- Or all the volume applications together
	SELECT @APPS = @APPS | [System].[v3d].[Applications].[mask] 
    FROM [v3d].[Series]
    INNER JOIN [System].[v3d].[Applications] 
            ON [System].[v3d].[Applications].[id] = [app] 
    WHERE [suid] = @UID AND LEN([app]) > 0
 
 -- Or all the volume applications together
 SELECT @APPS = @APPS | [System].[v3d].[Applications].[mask]
    FROM [v3d].[Volumes]
    INNER JOIN [v3d].[Series] ON [v3d].[Volumes].[uid] = [v3d].[Series].[uid]
    INNER JOIN [System].[v3d].[Applications] 
           ON [System].[v3d].[Applications].[id] = [v3d].[Volumes].[app] 
    WHERE [v3d].[Series].[suid] = @UID AND LEN([v3d].[Volumes].[app]) > 0

 	-- Return the result of the function

	RETURN @APPS

END




GO
GRANT EXECUTE ON [v3d].[fn_getStudyApplications] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getStudyApplications] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getStudyPath]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================
-- Author:		  Mark Kontak
-- Create date: 08/11/2010
-- Description:	Retreives the study path including the default store
-- ====================================================================
CREATE FUNCTION [v3d].[fn_getStudyPath]
(
@LOCATION AS NVARCHAR(16),    -- 'local' or 'remote'
@UID      AS NVARCHAR(128)     -- uid of series or study you want the location for
)
RETURNS NVARCHAR(MAX)
AS
BEGIN

	-- Declare the return variable here
	DECLARE @RESULTS AS NVARCHAR(MAX)

  SET @RESULTS = ''

  DECLARE @PATH_TABLE AS TABLE ( [path] NVARCHAR(512) )

  INSERT INTO @PATH_TABLE ( [path] ) 
    ( SELECT [System].[v3d].[fn_pathCombine]( [v3d].[fn_getRoot](@LOCATION, [v3d].[Series].[st]), [v3d].[Studies].[dir], '') AS [s_path]
        FROM [v3d].[Series] 
        INNER JOIN [v3d].[Studies] ON [v3d].[Series].[suid] = [v3d].[Studies].[uid]
           WHERE [v3d].[Series].[suid] = @UID )
 
	
	SELECT DISTINCT @RESULTS = @RESULTS + [path] + '|'
      FROM @PATH_TABLE

  SET @RESULTS = SUBSTRING(@RESULTS, 1 , LEN(@RESULTS) - 1 )
  
	-- Return the result of the function
	RETURN @RESULTS

END





GO
GRANT REFERENCES ON [v3d].[fn_getStudyPath] TO [v3dsa] AS [dbo]
GO
GRANT REFERENCES ON [v3d].[fn_getStudyPath] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getStudyState]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/28/2013
-- Description:	Gets the studies state
-- =============================================
CREATE FUNCTION [v3d].[fn_getStudyState]
(
@UID NVARCHAR(256)        -- Study UID
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @STATE AS BIGINT

  SET @STATE = 0

	-- Or all the series states together
	SELECT @STATE = @STATE | [state] FROM [v3d].[Series] WHERE [suid] = @UID

	-- Return the result of the function
	RETURN @STATE

END




GO
GRANT EXECUTE ON [v3d].[fn_getStudyState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getStudyState] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getTimeZoneOffset]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 04/05/2012
-- Description:	Gets the local timezone offset
-- =============================================
CREATE FUNCTION [v3d].[fn_getTimeZoneOffset]
()
RETURNS NVARCHAR(10)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @OFFSET AS NVARCHAR(64)
	

  SET @OFFSET = CONVERT(NVARCHAR(64),sysdatetimeoffset())
  SET @OFFSET = SUBSTRING(@OFFSET, LEN(@OFFSET) - 5, 6)



	-- Return the result of the function
	RETURN @OFFSET

END





GO
GRANT EXECUTE ON [v3d].[fn_getTimeZoneOffset] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getTimeZoneOffset] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_getVersion]    Script Date: 3/19/2014 4:44:49 PM ******/
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
/****** Object:  UserDefinedFunction [v3d].[fn_getVolumeConversionId]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =========================================================================================
-- Author:		  Mark Kontak
-- Create date: 11/18/2013
-- Description:	Gets the conversion id for the last volumes created from the series specified
-- =========================================================================================
CREATE FUNCTION [v3d].[fn_getVolumeConversionId]
(
@UID AS NVARCHAR(128)
)
RETURNS NVARCHAR(64)
AS
BEGIN
	-- Declare the return variable here
	DECLARE @CID AS NVARCHAR(64)
  DECLARE @TYPE AS INT
  DECLARE @DATE AS DATETIMEOFFSET(7)

	SELECT @CID = '', @TYPE = [type] FROM [v3d].[Series] WHERE [uid] = @UID

  IF @@ROWCOUNt > 0
  BEGIN

    IF @TYPE = [System].[v3d].[fn_getMaskBit]('type','dicom')
	    SELECT TOP(1) @CID = [cid], @DATE = [date] 
            FROM [v3d].[Volumes] 
          WHERE [uid] = @UID 
          ORDER BY [date] DESC
    ELSE
      SELECT @CID = [cid] FROM [v3d].[VolumeReferences] WHERE [uid] = @UID
  END

	-- Return the result of the function
	RETURN  @CID

END



GO
GRANT EXECUTE ON [v3d].[fn_getVolumeConversionId] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_getVolumeConversionId] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_isState]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =================================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Determines if the specified bit is set for the state
-- =================================================================
CREATE FUNCTION [v3d].[fn_isState]
(
@VALUE AS BIGINT,              -- Mask value
@NAME AS NVARCHAR(16)          -- Bit name             
)
RETURNS  BIT
AS
BEGIN
	
	DECLARE @RETURN AS BIT
	
	SET @RETURN = [System].[v3d].[fn_isMaskBitSet](@VALUE, 'state', @NAME)

  RETURN @RETURN


END





GO
GRANT EXECUTE ON [v3d].[fn_isState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_isState] TO [v3duser] AS [dbo]
GO
/****** Object:  UserDefinedFunction [v3d].[fn_setState]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==================================================
-- Author:		  Mark Kontak
-- Create date: 08/12/2010
-- Description:	Sets the specified bit in the mask
-- ==================================================
CREATE FUNCTION [v3d].[fn_setState]
(
@VALUE AS BIGINT,              -- Mask
@NAME AS NVARCHAR(16)          -- Bit Name
)
RETURNS BIGINT
AS
BEGIN
	-- Declare the return variable here
	DECLARE @BIT AS BIGINT
	
	SET @BIT = [System].[v3d].[fn_setMaskBit](@VALUE, 'state', @NAME)

	-- Return the result of the function
	RETURN ( @VALUE | @BIT )

END





GO
GRANT EXECUTE ON [v3d].[fn_setState] TO [v3dsa] AS [dbo]
GO
GRANT EXECUTE ON [v3d].[fn_setState] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[DeletePaths]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[DeletePaths](
	[id] [uniqueidentifier] NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[path] [nvarchar](1024) NOT NULL
) ON [PRIMARY]

GO
/****** Object:  Table [v3d].[DicomServers]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[DicomServers](
	[aetitle] [nvarchar](32) NOT NULL,
	[server] [nvarchar](32) NOT NULL,
	[port] [int] NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
 CONSTRAINT [PK_DicomServers] PRIMARY KEY CLUSTERED 
(
	[aetitle] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[DicomServers] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[DicomServers] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[DicomServers] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[DicomServers] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[DicomServers] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[DicomServers] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[DicomServers] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[DicomServers] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[DirectoryErrors]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[DirectoryErrors](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[type] [nvarchar](32) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[path] [nvarchar](1024) NOT NULL,
	[msg] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_DirectoryErrors_1] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[DirectoryErrors] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[DirectoryErrors] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[DirectoryErrors] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[DirectoryErrors] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[DirectoryErrors] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[DirectoryErrors] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Jobs]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Jobs](
	[id] [uniqueidentifier] NOT NULL,
	[app] [nvarchar](6) NOT NULL,
	[login] [nvarchar](32) NOT NULL,
	[name] [nvarchar](64) NOT NULL,
	[ort] [nvarchar](32) NOT NULL,
	[mod] [nvarchar](10) NOT NULL,
	[sys] [nvarchar](32) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
	[dst] [nvarchar](128) NOT NULL,
	[src] [nvarchar](128) NOT NULL,
	[study] [nvarchar](128) NOT NULL,
	[crdte] [datetimeoffset](7) NOT NULL,
	[stdte] [datetimeoffset](7) NULL,
	[task] [xml] NOT NULL,
	[state] [int] NOT NULL,
	[stage] [nvarchar](32) NOT NULL,
	[prog] [float] NOT NULL,
	[cid] [nvarchar](32) NOT NULL,
	[vol] [nvarchar](32) NOT NULL,
	[tid] [nvarchar](128) NOT NULL,
	[icnt] [int] NOT NULL,
	[msg] [nvarchar](1024) NOT NULL,
 CONSTRAINT [PK_Jobs] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Jobs] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Jobs] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Jobs] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Jobs] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[Jobs] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[Jobs] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[Jobs] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Jobs] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Locks]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Locks](
	[uid] [nvarchar](128) NOT NULL,
	[suid] [nvarchar](128) NOT NULL,
	[ctx] [nvarchar](256) NOT NULL,
	[tid] [nvarchar](256) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
	[date] [datetime] NOT NULL,
	[login] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_Locks] PRIMARY KEY NONCLUSTERED 
(
	[uid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Locks] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Locks] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Locks] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Locks] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[Locks] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[Locks] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[Locks] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Locks] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Messages]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Messages](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[type] [smallint] NOT NULL,
	[date] [datetime] NOT NULL,
	[app] [nvarchar](32) NOT NULL,
	[name] [nvarchar](64) NOT NULL,
	[src] [nvarchar](128) NOT NULL,
	[msg] [nvarchar](max) NOT NULL,
	[ort] [nvarchar](16) NOT NULL,
 CONSTRAINT [MESSAGES_PK] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Messages] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Messages] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Messages] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Messages] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[Messages] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[Messages] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[Messages] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[PacsJobs]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[PacsJobs](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[qdate] [datetimeoffset](7) NOT NULL,
	[sdate] [datetimeoffset](7) NULL,
	[cdate] [datetimeoffset](7) NULL,
	[op] [nvarchar](16) NOT NULL,
	[prog] [int] NOT NULL,
	[msg] [nvarchar](256) NOT NULL,
	[state] [nvarchar](10) NOT NULL,
	[suid] [nvarchar](128) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[host] [nvarchar](32) NOT NULL,
	[dest] [nvarchar](64) NOT NULL,
	[query] [xml] NULL,
 CONSTRAINT [PK_PacsJobs] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
/****** Object:  Table [v3d].[Receivers]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Receivers](
	[aetitle] [nvarchar](32) NOT NULL,
	[association] [nvarchar](256) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[server] [nvarchar](32) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[cnt] [int] NOT NULL,
	[new] [int] NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[ort] [nvarchar](16) NOT NULL,
	[calling] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_Receiving_1] PRIMARY KEY CLUSTERED 
(
	[aetitle] ASC,
	[association] ASC,
	[uid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Receivers] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Receivers] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Receivers] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Receivers] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Receivers] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[SerializeErrors]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[SerializeErrors](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[type] [nvarchar](16) NOT NULL,
	[msg] [nvarchar](2048) NOT NULL,
 CONSTRAINT [PK_SerializationErrors_1] PRIMARY KEY NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[SerializeErrors] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[SerializeErrors] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[SerializeErrors] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SerializeErrors] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[SerializeErrors] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[SerializeErrors] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Series]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Series](
	[uid] [nvarchar](128) NOT NULL,
	[suid] [nvarchar](128) NOT NULL,
	[ort] [nvarchar](16) NOT NULL,
	[pln] [tinyint] NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[dir] [nvarchar](128) NOT NULL,
	[mod] [nvarchar](16) NOT NULL,
	[rcv] [datetimeoffset](7) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
	[type] [int] NOT NULL,
	[app] [nvarchar](32) NOT NULL,
	[icnt] [int] NOT NULL,
	[num] [nvarchar](32) NOT NULL,
	[src] [nvarchar](32) NOT NULL,
	[state] [int] NOT NULL,
	[on] [datetimeoffset](7) NOT NULL,
	[st] [nvarchar](32) NOT NULL,
	[proc] [xml] NULL,
	[hdr] [xml] NULL,
	[ext] [xml] NULL,
	[msg] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_Series] PRIMARY KEY NONCLUSTERED 
(
	[uid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[Series] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Series] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Series] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Series] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[Series] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[Series] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[Series] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Series] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[SeriesReferences]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[SeriesReferences](
	[uid] [nvarchar](128) NOT NULL,
	[ruid] [nvarchar](128) NOT NULL,
 CONSTRAINT [PK_SeriesReferences] PRIMARY KEY CLUSTERED 
(
	[uid] ASC,
	[ruid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[SeriesReferences] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[SeriesReferences] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[SeriesReferences] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SeriesReferences] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[SeriesReferences] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[SeriesReferences] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[SeriesReferences] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[SeriesReferences] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Studies]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [v3d].[Studies](
	[uid] [nvarchar](128) NOT NULL,
	[mid] [nvarchar](64) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[rcv] [datetimeoffset](7) NOT NULL,
	[phys] [nvarchar](64) NOT NULL,
	[id] [nvarchar](32) NOT NULL,
	[an] [nvarchar](64) NOT NULL,
	[desc] [nvarchar](256) NOT NULL,
	[grp] [nvarchar](16) NOT NULL,
	[dir] [nvarchar](128) NOT NULL,
	[name] [nvarchar](256) NOT NULL,
	[sex] [char](1) NOT NULL,
	[dob] [date] NULL,
	[wgt] [decimal](5, 2) NOT NULL,
	[ext] [xml] NULL,
 CONSTRAINT [PK_Studies] PRIMARY KEY NONCLUSTERED 
(
	[uid] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING ON
GO
GRANT DELETE ON [v3d].[Studies] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[Studies] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[Studies] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Studies] TO [v3dsa] AS [dbo]
GO
GRANT DELETE ON [v3d].[Studies] TO [v3duser] AS [dbo]
GO
GRANT INSERT ON [v3d].[Studies] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[Studies] TO [v3duser] AS [dbo]
GO
GRANT UPDATE ON [v3d].[Studies] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Versions]    Script Date: 3/19/2014 4:44:49 PM ******/
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
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 70) ON [PRIMARY]
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
/****** Object:  Table [v3d].[VolumeReferences]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[VolumeReferences](
	[uid] [nvarchar](128) NOT NULL,
	[duid] [nvarchar](128) NOT NULL,
	[cid] [nvarchar](32) NOT NULL,
	[name] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_VolumeReferences_1] PRIMARY KEY CLUSTERED 
(
	[uid] ASC,
	[duid] ASC,
	[cid] ASC,
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
GRANT DELETE ON [v3d].[VolumeReferences] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[VolumeReferences] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[VolumeReferences] TO [v3dsa] AS [dbo]
GO
GRANT UPDATE ON [v3d].[VolumeReferences] TO [v3dsa] AS [dbo]
GO
GRANT INSERT ON [v3d].[VolumeReferences] TO [v3duser] AS [dbo]
GO
GRANT SELECT ON [v3d].[VolumeReferences] TO [v3duser] AS [dbo]
GO
/****** Object:  Table [v3d].[Volumes]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [v3d].[Volumes](
	[uid] [nvarchar](128) NOT NULL,
	[cid] [nvarchar](32) NOT NULL,
	[name] [nvarchar](32) NOT NULL,
	[date] [datetimeoffset](7) NOT NULL,
	[app] [nvarchar](16) NOT NULL,
	[sub] [nvarchar](32) NOT NULL,
	[file] [nvarchar](512) NOT NULL,
	[icnt] [int] NOT NULL,
	[ext] [xml] NULL,
	[desc] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_Volumes_1] PRIMARY KEY NONCLUSTERED 
(
	[uid] ASC,
	[cid] ASC,
	[name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
/****** Object:  View [v3d].[DependentSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [v3d].[DependentSeries]
AS
SELECT        v3d.SeriesReferences.ruid AS uid, v3d.Series.uid AS duid, v3d.Series.suid, v3d.Series.ort, v3d.Series.pln, v3d.Series.date, v3d.Series.dir, v3d.Series.mod, v3d.Series.[desc], v3d.Series.type, v3d.Series.app, 
                         v3d.Series.num, v3d.Series.src, v3d.Series.[proc], v3d.Series.state, v3d.Series.hdr, v3d.Series.rcv, v3d.Series.icnt, v3d.Series.[on], v3d.Series.ext, v3d.Series.st, v3d.Series.msg
FROM            v3d.SeriesReferences INNER JOIN
                         v3d.Series ON v3d.SeriesReferences.uid = v3d.Series.uid



GO
GRANT SELECT ON [v3d].[DependentSeries] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[DependentSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  View [v3d].[ReferenceSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [v3d].[ReferenceSeries]
AS
SELECT        v3d.SeriesReferences.uid, v3d.Series.uid AS ruid, v3d.Series.suid, v3d.Series.ort, v3d.Series.pln, v3d.Series.date, v3d.Series.dir, v3d.Series.mod, v3d.Series.[desc], v3d.Series.type, v3d.Series.app, 
                         v3d.Series.num, v3d.Series.src, v3d.Series.[proc], v3d.Series.state, v3d.Series.hdr, v3d.Series.rcv, v3d.Series.icnt, v3d.Series.[on], v3d.Series.ext, v3d.Series.st, v3d.Series.msg
FROM            v3d.SeriesReferences INNER JOIN
                         v3d.Series ON v3d.SeriesReferences.ruid = v3d.Series.uid



GO
GRANT SELECT ON [v3d].[ReferenceSeries] TO [v3dsa] AS [dbo]
GO
GRANT SELECT ON [v3d].[ReferenceSeries] TO [v3duser] AS [dbo]
GO
/****** Object:  View [v3d].[VolumeReferencedSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [v3d].[VolumeReferencedSeries]
AS
SELECT        v3d.Series.uid, v3d.Series.suid, v3d.Series.ort, v3d.Series.pln, v3d.Series.date, v3d.Series.dir, v3d.Series.mod, v3d.Series.rcv, v3d.Series.[desc], v3d.Series.type, v3d.Series.app, v3d.Series.icnt, 
                         v3d.Series.num, v3d.Series.src, v3d.Series.state, v3d.Series.[on], v3d.Series.st, v3d.Series.[proc], v3d.Series.hdr, v3d.Series.ext, v3d.Series.msg, v3d.VolumeReferences.cid, v3d.VolumeReferences.name, 
                         v3d.VolumeReferences.duid
FROM            v3d.VolumeReferences INNER JOIN
                         v3d.Series ON v3d.VolumeReferences.uid = v3d.Series.uid



GO
ALTER TABLE [v3d].[DeletePaths] ADD  CONSTRAINT [DF_Paths_uid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[DeletePaths] ADD  CONSTRAINT [DF_Directories_directory]  DEFAULT ('') FOR [path]
GO
ALTER TABLE [v3d].[DicomServers] ADD  CONSTRAINT [DF_DicomServers_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[DirectoryErrors] ADD  CONSTRAINT [DF_DirectoryErrors_id]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[DirectoryErrors] ADD  CONSTRAINT [DF_DirectoryErrors_type]  DEFAULT ('series') FOR [type]
GO
ALTER TABLE [v3d].[DirectoryErrors] ADD  CONSTRAINT [DF_DirectoryErrors_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[DirectoryErrors] ADD  CONSTRAINT [DF_DirectoryErrors_directory]  DEFAULT ('') FOR [path]
GO
ALTER TABLE [v3d].[DirectoryErrors] ADD  CONSTRAINT [DF_DirectoryErrors_message]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_id]  DEFAULT (newid()) FOR [id]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_app]  DEFAULT ('') FOR [app]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_login]  DEFAULT ('') FOR [login]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_ort]  DEFAULT ('') FOR [ort]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_mod]  DEFAULT ('') FOR [mod]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_sys]  DEFAULT ('') FOR [sys]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_uid]  DEFAULT ('') FOR [dst]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_suid]  DEFAULT ('') FOR [src]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_study]  DEFAULT ('') FOR [study]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Table_1_date]  DEFAULT (sysdatetimeoffset()) FOR [crdte]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_tsk]  DEFAULT ('') FOR [task]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_st]  DEFAULT ('') FOR [state]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_stage]  DEFAULT ('') FOR [stage]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_JobsNew_prog]  DEFAULT ((0.0)) FOR [prog]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_conv]  DEFAULT ('') FOR [cid]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_volume]  DEFAULT ((-1)) FOR [vol]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_tid]  DEFAULT ('') FOR [tid]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_icnt]  DEFAULT ((0)) FOR [icnt]
GO
ALTER TABLE [v3d].[Jobs] ADD  CONSTRAINT [DF_Jobs_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Locks] ADD  CONSTRAINT [DF_Locks_suid]  DEFAULT ('') FOR [suid]
GO
ALTER TABLE [v3d].[Locks] ADD  CONSTRAINT [DF_SERIES_LOCK_LOCK_USER]  DEFAULT ('') FOR [ctx]
GO
ALTER TABLE [v3d].[Locks] ADD  CONSTRAINT [DF_Locks_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Locks] ADD  CONSTRAINT [DF_Locks_date]  DEFAULT (getdate()) FOR [date]
GO
ALTER TABLE [v3d].[Locks] ADD  CONSTRAINT [DF_Locks_login]  DEFAULT ('') FOR [login]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_Messages_type]  DEFAULT ('Pending') FOR [type]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_MODIFIED]  DEFAULT (getdate()) FOR [date]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_STUDY_TYPE]  DEFAULT ('') FOR [app]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_PATIENT_FIRST_NAME]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_SOURCE]  DEFAULT (N'''') FOR [src]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_MSG]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Messages] ADD  CONSTRAINT [DF_MESSAGES_PATIENT_ORIENTATION]  DEFAULT ('') FOR [ort]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsQueue_date]  DEFAULT (sysdatetimeoffset()) FOR [qdate]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsQueue_op]  DEFAULT ('') FOR [op]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsQueue_prog]  DEFAULT ((0)) FOR [prog]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsQueue_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsQueue_state]  DEFAULT ('') FOR [state]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsJobs_suid]  DEFAULT ('') FOR [suid]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsJobs_uid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsJobs_aetitle]  DEFAULT ('') FOR [host]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsJobs_dest]  DEFAULT ('') FOR [dest]
GO
ALTER TABLE [v3d].[PacsJobs] ADD  CONSTRAINT [DF_PacsJobs_query]  DEFAULT (NULL) FOR [query]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_receiver]  DEFAULT ('') FOR [association]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_seriesUid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_server]  DEFAULT ('') FOR [server]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_cnt]  DEFAULT ((1)) FOR [cnt]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receivers_new]  DEFAULT ((0)) FOR [new]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receiving_pname]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receivers_ort]  DEFAULT ('') FOR [ort]
GO
ALTER TABLE [v3d].[Receivers] ADD  CONSTRAINT [DF_Receivers_calling]  DEFAULT ('') FOR [calling]
GO
ALTER TABLE [v3d].[SerializeErrors] ADD  CONSTRAINT [DF_SerializationErrors_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[SerializeErrors] ADD  CONSTRAINT [DF_SerializationErrors_type]  DEFAULT (N'series') FOR [type]
GO
ALTER TABLE [v3d].[SerializeErrors] ADD  CONSTRAINT [DF_SerializationErrors_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_UID]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_ORIENTATION]  DEFAULT ('') FOR [ort]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_pln]  DEFAULT ((0)) FOR [pln]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_SERIES_DATE]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_LOCATION]  DEFAULT ('') FOR [dir]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_MODALITY]  DEFAULT ('OT') FOR [mod]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_DATE_RECEIVED]  DEFAULT (sysdatetimeoffset()) FOR [rcv]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_DESCRIPTION]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_tmp]  DEFAULT ((0)) FOR [type]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_SUB_TYPE]  DEFAULT (N'UNKNOWN') FOR [app]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_IMAGE_COUNT]  DEFAULT ((0)) FOR [icnt]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_DICOM_SERIES_NUMBER]  DEFAULT ('') FOR [num]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_SENDING_MODALITY]  DEFAULT ('') FOR [src]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_STATE]  DEFAULT ((0)) FOR [state]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_procOn]  DEFAULT (sysdatetimeoffset()) FOR [on]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_str]  DEFAULT ('default') FOR [st]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_SERIES_PROCESS]  DEFAULT (NULL) FOR [proc]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_hdr]  DEFAULT (NULL) FOR [hdr]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_ext]  DEFAULT (NULL) FOR [ext]
GO
ALTER TABLE [v3d].[Series] ADD  CONSTRAINT [DF_Series_msg]  DEFAULT ('') FOR [msg]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_UID]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_mid]  DEFAULT ('') FOR [mid]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_SCAN_DATE]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_rcv]  DEFAULT (sysdatetimeoffset()) FOR [rcv]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_REFERRING_PHYSICIAN]  DEFAULT ('') FOR [phys]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_DICOM_STUDY_ID]  DEFAULT ('') FOR [id]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_ACCESSSION_NUMBER]  DEFAULT ('') FOR [an]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_DESCRIPTION]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_GROUP]  DEFAULT ('') FOR [grp]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_STUDIES_DIRECTIRY_NAME]  DEFAULT ('') FOR [dir]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_sex]  DEFAULT ('') FOR [sex]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_wgt]  DEFAULT ((0.0)) FOR [wgt]
GO
ALTER TABLE [v3d].[Studies] ADD  CONSTRAINT [DF_Studies_ext]  DEFAULT ('') FOR [ext]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_SeriesVolumes_uid]  DEFAULT ('') FOR [uid]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_cnvid]  DEFAULT ('') FOR [cid]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Table_1_volume]  DEFAULT (N'Volume file associated with the dicom series') FOR [name]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_date]  DEFAULT (sysdatetimeoffset()) FOR [date]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_app]  DEFAULT ('') FOR [app]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_sub]  DEFAULT ('') FOR [sub]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_filepath]  DEFAULT ('') FOR [file]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_icnt]  DEFAULT ((0)) FOR [icnt]
GO
ALTER TABLE [v3d].[Volumes] ADD  CONSTRAINT [DF_Volumes_desc]  DEFAULT ('') FOR [desc]
GO
ALTER TABLE [v3d].[Series]  WITH CHECK ADD  CONSTRAINT [FK_Series_Studies] FOREIGN KEY([suid])
REFERENCES [v3d].[Studies] ([uid])
GO
ALTER TABLE [v3d].[Series] CHECK CONSTRAINT [FK_Series_Studies]
GO
/****** Object:  Trigger [v3d].[tr_deleteDicomServer]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE TRIGGER [v3d].[tr_deleteDicomServer]
   ON  [v3d].[DicomServers]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 BEGIN TRY
 
 
    DELETE [v3d].[Receivers] FROM DELETED WHERE [v3d].[Receivers].[aetitle] = DELETED.[aetitle]


  END TRY
  BEGIN CATCH
  
    EXEC [v3d].[sp_errorHandler]  
      @PROCEDURE = 'tr_deleteDicomServer',
      @RAISERROR = 0

 
    

  END CATCH

END




GO
/****** Object:  Trigger [v3d].[tr_deleteJob]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak
-- Create date: 08/28/2013
-- Description:	Trigger for job deletion
-- =============================================
CREATE TRIGGER [v3d].[tr_deleteJob]
   ON  [v3d].[Jobs]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @COMPLETED AS BIGINT
  DECLARE @RECEIVING AS BIGINT
  DECLARE @PENDING   AS BIGINT


  SELECT  @COMPLETED  = [System].[v3d].[fn_getMaskBit]('state','completed'),
          @RECEIVING  = [System].[v3d].[fn_getMaskBit]('state','receiving'),
          @PENDING    = [System].[v3d].[fn_getMaskBit]('state','pending')


    

  -- ===============================
  -- The SRC column should be DICOM
  -- ===============================

   -- =========================================================================================
  -- Unsets the JOBS states bits in the series state so that the series has the proper state. 
  -- If the series only has one associated job then the state will be 0 so the next UPDATE 
  -- is necessary.
  -- ========================================================================================
  UPDATE [v3d].[Series] SET [v3d].[Series].[state] = ( [v3d].[Series].[state] | DELETED.[state] ) ^ DELETED.[state] 
    FROM DELETED 
   WHERE [v3d].[Series].[uid] = DELETED.[src] AND 
         DELETED.[state] = @RECEIVING OR 
         DELETED.[state] = @PENDING

 -- =========================================================================================
  -- Sets all source series to a COMPLETED state if the above update has set it to 0
  -- ========================================================================================
  UPDATE [v3d].[Series] SET [v3d].[Series].[state] = @COMPLETED 
    FROM DELETED 
   WHERE [v3d].[Series].[uid] = DELETED.[src] AND 
         [v3d].[Series].[state] = 0



END



GO
/****** Object:  Trigger [v3d].[tr_deleteLock]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================
-- Author:		  Mark Kontak
-- Create date: 08/10/2012
-- Description:	Resets the locked bit on the series
-- ===================================================
CREATE TRIGGER [v3d].[tr_deleteLock]
   ON  [v3d].[Locks]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Unsets the lock bit for the series state 
  UPDATE [v3d].[Series] SET [state] = [System].[v3d].[fn_unsetMaskBit]([state],'state', 'locked') FROM DELETED WHERE [v3d].[Series].[uid] = DELETED.[uid]


END



GO
/****** Object:  Trigger [v3d].[tr_insertLock]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================
-- Author:		  Mark Kontak
-- Create date: 08/10/2012
-- Description:	Sets the locked bit on the series
-- ===================================================
CREATE TRIGGER [v3d].[tr_insertLock]
   ON  [v3d].[Locks]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Set the locked bit on the series 
  UPDATE [v3d].[Series] SET [state] = [System].[v3d].[fn_setMaskBit]([state],'state', 'locked') FROM INSERTED WHERE [v3d].[Series].[uid] = INSERTED.[uid]


END



GO
/****** Object:  Trigger [v3d].[tr_deleteSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===============================================================
-- Author:		  Mark Kontak
-- Create date: 04/30/2012
-- Description:	Deletes any messages associated with the series
-- ===============================================================
CREATE TRIGGER [v3d].[tr_deleteSeries] 
   ON  [v3d].[Series]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Delete associated messages
  DELETE [v3d].[Messages] 
          FROM DELETED 
          WHERE [v3d].[Messages].[src] = DELETED.[uid]

  -- Delete associated locks
  DELETE [v3d].[Locks]
          FROM DELETED  
          WHERE [v3d].[Locks].[uid] = DELETED.[uid]

  -- Delete associated volumes
  DELETE [v3d].[Volumes]
          FROM DELETED  
          WHERE [v3d].[Volumes].[uid] = DELETED.[uid]

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[DirectoryErrors] FROM deleted WHERE [v3d].[DirectoryErrors].[uid] = deleted.[uid]

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[SerializeErrors] FROM deleted WHERE [v3d].[SerializeErrors].[uid] = deleted.[uid]


END



GO
/****** Object:  Trigger [v3d].[tr_insertSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 1/24/2012
-- Description:	Clears any directories if the series is add/re-added
-- ====================================================================
CREATE TRIGGER [v3d].[tr_insertSeries]
   ON  [v3d].[Series]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[DirectoryErrors] FROM INSERTED WHERE [v3d].[DirectoryErrors].[uid] = inserted.[uid]

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[SerializeErrors] FROM INSERTED WHERE [v3d].[SerializeErrors].[uid] = inserted.[uid]

END



GO
/****** Object:  Trigger [v3d].[tr_updateSeries]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/*********************************************************************************************
 * Trigger Name:	UPDATE_SERIES_MESSAGES			  
 * Description:	Deletes the corresponding MESSAGES entries for the DATASET_LOCATION
 *                	being deleted iff the entry exits
 **********************************************************************************************/
CREATE TRIGGER [v3d].[tr_updateSeries] 
ON [v3d].[Series]
FOR UPDATE 
AS 
BEGIN 

	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 	IF UPDATE([state])
	BEGIN
		-- ======================================================
		-- Delete any error messages associated with the series
		-- ======================================================
		DELETE [v3d].[Messages] 
			  FROM INSERTED 
			  WHERE ( [v3d].[Messages].[src] = INSERTED.[uid] AND [v3d].[Messages].[type] > 0 )

	END
 


 
  
 END



GO
/****** Object:  Trigger [v3d].[tr_referenceDeleted]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/16/2011
-- Description:	Set off the series serialization trigger by updating the series item
-- =====================================================================================
CREATE TRIGGER [v3d].[tr_referenceDeleted]
   ON  [v3d].[SeriesReferences]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Update to force triggers
  UPDATE [v3d].[Series] SET [v3d].[Series].[num] = [v3d].[Series].[num] FROM  DELETED WHERE [v3d].[Series].[uid] = DELETED.[uid]

END





GO
/****** Object:  Trigger [v3d].[tr_referenceInserted]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =====================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/16/2011
-- Description:	Set off the series serialization trigger by updating the series item
-- =====================================================================================
CREATE TRIGGER [v3d].[tr_referenceInserted]
   ON  [v3d].[SeriesReferences]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Update to force triggers
  UPDATE [v3d].[Series] SET [v3d].[Series].[num] = [v3d].[Series].[num] FROM  INSERTED WHERE [v3d].[Series].[uid] = INSERTED.[uid]

END





GO
/****** Object:  Trigger [v3d].[tr_deleteStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 1/24/2012
-- Description:	Clears any directories if the study is add/re-added
-- ====================================================================
CREATE TRIGGER [v3d].[tr_deleteStudy]
   ON  [v3d].[Studies]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[DirectoryErrors] FROM deleted WHERE [v3d].[DirectoryErrors].[uid] = deleted.[uid]

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[SerializeErrors] FROM deleted WHERE [v3d].[SerializeErrors].[uid] = deleted.[uid]

END



GO
/****** Object:  Trigger [v3d].[tr_insertStudy]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ===================================================================
-- Author:		  Mark Kontak
-- Create date: 1/24/2012
-- Description:	Clears any directories if the series is add/re-added
-- ====================================================================
CREATE TRIGGER [v3d].[tr_insertStudy]
   ON  [v3d].[Studies]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[DirectoryErrors] FROM INSERTED WHERE [v3d].[DirectoryErrors].[uid] = inserted.[uid]

  -- Delete any old directory entries associated with this series
  DELETE [v3d].[SerializeErrors] FROM INSERTED WHERE [v3d].[SerializeErrors].[uid] = inserted.[uid]

END



GO
/****** Object:  Trigger [v3d].[tr_deleteVolume]    Script Date: 3/19/2014 4:44:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =================================================
-- Author:		  Mark Kontak
-- Create date: 10/28/2013
-- Description:	 Cleanup the referneces upon delete
-- =================================================
CREATE TRIGGER [v3d].[tr_deleteVolume]
   ON  [v3d].[Volumes]
   AFTER DELETE
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- ==================================================
  -- Delete all references to the volume being deleted
  -- ==================================================
  DELETE [v3d].[VolumeReferences] FROM [deleted] 
   WHERE [v3d].[VolumeReferences].[cid] = deleted.[cid] AND 
         [v3d].[VolumeReferences].[duid] = deleted.[uid]   

END



GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study or series uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DeletePaths', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the transaction list of paths that are to be deleted' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DeletePaths'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of DICOM servers that are running' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomServers'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DicomServers'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Id associated wth this path' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Date of Error' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date of the directory exception event' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Path' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'path'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Path to be deleted' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'path'
GO
EXEC sys.sp_addextendedproperty @name=N'ColumnHeader', @value=N'Error Message' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Error message/reason for the entry in this table' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains DirectoryErrors that we had a problem deleting from within the database and need to be deleted again.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'DirectoryErrors'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Job Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Application id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'User creating job' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'login'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Modified date' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'System doing the processing' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'sys'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Destination UID' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'dst'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Source UID' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'src'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study UID' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'study'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the job was created' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'crdte'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the job was started' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'stdte'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Task name in template (currently being processed)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'task'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'State of job (Pending, Processing, Completed, Failed)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'state'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Conversion id that the colume is associated with' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'stage'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Progress of the job' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'prog'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Template id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'tid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Error message if the job failed (only used if the job was not deleted)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of JOBS that are to be processed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'6' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Jobs'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Locks', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study Uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Locks', @level2type=N'COLUMN',@level2name=N'suid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Context holding the lock' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Locks', @level2type=N'COLUMN',@level2name=N'ctx'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the series locks within the system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Locks'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Locks'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Information = 0, Warning = 1, Error = 2' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages', @level2type=N'COLUMN',@level2name=N'type'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'First name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Source of data' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages', @level2type=N'COLUMN',@level2name=N'src'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Source of the message ( can be a tag name or a UID  and is used for cleanup )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Datatsets orientation' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages', @level2type=N'COLUMN',@level2name=N'ort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of messages for receiving and processing of DICOM' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Messages'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Operation ( store, retreive, query )' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs', @level2type=N'COLUMN',@level2name=N'op'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs', @level2type=N'COLUMN',@level2name=N'suid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Pacs server name in the host table' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs', @level2type=N'COLUMN',@level2name=N'host'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Destintaion AETitle' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs', @level2type=N'COLUMN',@level2name=N'dest'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of JOBS to be processed by the Pacs Queue Server' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'PacsJobs'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'AE Title of the server the receiver belongs to' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'server'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date we started receiving ' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the total number of images received' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'cnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the number of actual new images' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'new'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient name being received' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Orientation or series being receved' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'ort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Calling AE Title' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers', @level2type=N'COLUMN',@level2name=N'calling'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of associations the DICOM server has up and receiving' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Receivers'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SerializeErrors', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of series/studies that had problems serializing out and need to be serialized' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SerializeErrors'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'5' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SerializeErrors'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series UID' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study ID' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'suid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Orientation' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'ort'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series Date' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series Location' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'dir'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Modality' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'mod'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date Received' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'rcv'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Series Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Application Identifier' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Image Count' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'icnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Dicom Series Number' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'num'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Sending Modality' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'src'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date Processed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'on'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Store name this series resides on' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'st'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Process used to generate this series (Task xml including parameters)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'proc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'First images header used in filtering' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'hdr'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Extended properties' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'ext'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains additional state information' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series', @level2type=N'COLUMN',@level2name=N'msg'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all the DICOM series we have received and processed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'6' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Series'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Lists all the references for a series' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SeriesReferences'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'4' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'SeriesReferences'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study Uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Medical Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'mid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Scan/Study date' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the study was received or entered our system' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'rcv'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Physician' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'phys'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Study Id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'id'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Accession Number' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'an'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Description' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'desc'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Group' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'grp'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Directory' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'dir'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient name' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient gender' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'sex'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient date of birth' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'dob'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Patient weight' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'wgt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Extended Properties' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies', @level2type=N'COLUMN',@level2name=N'ext'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all the DICOM studies we have received and processed' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'6' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Studies'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains a list of object version used to return the ver attribute in the get procedures.' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Versions'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Dicom uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'VolumeReferences', @level2type=N'COLUMN',@level2name=N'duid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Links the columes to the processed data' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'VolumeReferences'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'0' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'VolumeReferences'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Dicom series uid' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'uid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Conversion id' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'cid'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Name of volume (must be unique within the series)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'name'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Date the volume was created' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'date'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Application code' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'app'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Application sub type' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'sub'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Volume file (can be full path)' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'file'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Image count for this volume' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'icnt'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Extended properties' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes', @level2type=N'COLUMN',@level2name=N'ext'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains the list of volumes for a DICOM series' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes'
GO
EXEC sys.sp_addextendedproperty @name=N'ServicePermissions', @value=N'7' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'TABLE',@level1name=N'Volumes'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[29] 4[33] 2[10] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "SeriesReferences (v3d)"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 110
               Right = 198
            End
            DisplayFlags = 280
            TopColumn = 0
         End
         Begin Table = "Series (v3d)"
            Begin Extent = 
               Top = 6
               Left = 281
               Bottom = 125
               Right = 441
            End
            DisplayFlags = 280
            TopColumn = 18
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
      Begin ColumnWidths = 24
         Width = 284
         Width = 5130
         Width = 5250
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 3660
         Table = 3195
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'DependentSeries'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'DependentSeries'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "SeriesReferences (v3d)"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 110
               Right = 198
            End
            DisplayFlags = 280
            TopColumn = 0
         End
         Begin Table = "Series (v3d)"
            Begin Extent = 
               Top = 6
               Left = 236
               Bottom = 125
               Right = 396
            End
            DisplayFlags = 280
            TopColumn = 18
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
      Begin ColumnWidths = 22
         Width = 284
         Width = 5055
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 2115
         Table = 3030
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'ReferenceSeries'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'ReferenceSeries'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "VolumeReferences (v3d)"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 208
            End
            DisplayFlags = 280
            TopColumn = 0
         End
         Begin Table = "Series (v3d)"
            Begin Extent = 
               Top = 6
               Left = 246
               Bottom = 136
               Right = 416
            End
            DisplayFlags = 280
            TopColumn = 0
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
      Begin ColumnWidths = 25
         Width = 284
         Width = 3150
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
         Width = 1500
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 900
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'VolumeReferencedSeries'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'v3d', @level1type=N'VIEW',@level1name=N'VolumeReferencedSeries'
GO
