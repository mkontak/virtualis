USE [Datasets]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_uid]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_name]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_mod]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_man]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_model]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_ort]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_pln]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_qa]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_cpd]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_desc]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_icnt]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_app]
GO
ALTER TABLE [dbo].[Datasets] DROP CONSTRAINT [DF_Datasets_type]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_uid]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_aetitle]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_server]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_port]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_date]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_state]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_name]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_progress]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Table_1_tot]
GO
ALTER TABLE [dbo].[Progress] DROP CONSTRAINT [DF_Progress_message]
GO
/****** Object:  StoredProcedure [dbo].[sp_addImage]    Script Date: 09/15/2010 15:53:48 ******/
DROP PROCEDURE [dbo].[sp_addImage]
GO
/****** Object:  StoredProcedure [dbo].[sp_getImages]    Script Date: 09/15/2010 15:53:48 ******/
DROP PROCEDURE [dbo].[sp_getImages]
GO
/****** Object:  StoredProcedure [dbo].[sp_purgeDatabase]    Script Date: 09/15/2010 15:53:48 ******/
DROP PROCEDURE [dbo].[sp_purgeDatabase]
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getOrientationInformation]    Script Date: 09/15/2010 15:53:48 ******/
DROP FUNCTION [dbo].[fn_getOrientationInformation]
GO
/****** Object:  Table [dbo].[Images]    Script Date: 09/15/2010 15:53:48 ******/
DROP TABLE [dbo].[Images]
GO
/****** Object:  StoredProcedure [dbo].[sp_updateProgress]    Script Date: 09/15/2010 15:53:48 ******/
DROP PROCEDURE [dbo].[sp_updateProgress]
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getCosines]    Script Date: 09/15/2010 15:53:48 ******/
DROP FUNCTION [dbo].[fn_getCosines]
GO
/****** Object:  Table [dbo].[Progress]    Script Date: 09/15/2010 15:53:48 ******/
DROP TABLE [dbo].[Progress]
GO
/****** Object:  Table [dbo].[Datasets]    Script Date: 09/15/2010 15:53:48 ******/
DROP TABLE [dbo].[Datasets]
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getCoordinates]    Script Date: 09/15/2010 15:53:48 ******/
DROP FUNCTION [dbo].[fn_getCoordinates]
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getCoordinates]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z codinates from the image orientation dicom field
-- =================================================================================================
CREATE FUNCTION [dbo].[fn_getCoordinates]
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
/****** Object:  Table [dbo].[Datasets]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Datasets](
	[uid] [nvarchar](128) NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[mod] [nvarchar](4) NOT NULL,
	[man] [nvarchar](64) NOT NULL,
	[model] [nvarchar](64) NOT NULL,
	[ort] [nvarchar](32) NOT NULL,
	[pln] [nvarchar](32) NOT NULL,
	[qa] [bit] NOT NULL,
	[cpd] [bit] NOT NULL,
	[desc] [nvarchar](1000) NOT NULL,
	[icnt] [int] NOT NULL,
	[app] [nvarchar](6) NOT NULL,
	[type] [nvarchar](64) NOT NULL,
 CONSTRAINT [PK_Datasets] PRIMARY KEY NONCLUSTERED 
(
	[uid] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Progress]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Progress](
	[uid] [nvarchar](128) NOT NULL,
	[aetitle] [nvarchar](64) NOT NULL,
	[server] [nvarchar](64) NOT NULL,
	[port] [int] NOT NULL,
	[date] [datetime] NOT NULL,
	[state] [nvarchar](10) NOT NULL,
	[name] [nvarchar](128) NOT NULL,
	[progress] [int] NOT NULL,
	[total] [int] NOT NULL,
	[message] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_Progress] PRIMARY KEY CLUSTERED 
(
	[uid] ASC,
	[aetitle] ASC,
	[server] ASC,
	[port] ASC,
	[date] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getCosines]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z cosines for the row and columns
-- =================================================================================================
CREATE FUNCTION [dbo].[fn_getCosines]
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
  
  SELECT @ROW_X = x, @ROW_Y = y, @ROW_Z = z  FROM fn_getCoordinates(@ROW)  
  
  SELECT @COL_X = x, @COL_Y = y, @COL_Z = z FROM fn_getCoordinates(@COLUMN) 
 

  INSERT @COSINES_TABLE ( row_x, row_y, row_z, col_x, col_y, col_z ) VALUES 
                      ( @ROW_X, @ROW_Y, @ROW_Z, @COL_X, @COL_Y, @COL_Z )

  RETURN




END
GO
/****** Object:  StoredProcedure [dbo].[sp_updateProgress]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		  Mark Kontak 
-- Create date: 09/13/2010
-- Description:	Add progress
-- =============================================
CREATE PROCEDURE [dbo].[sp_updateProgress]
(
@UID AS NVARCHAR(128),            -- Dataset UID 
@AETITLE AS NVARCHAR(64),         -- AE Title
@SERVER AS NVARCHAR(64),          -- Server
@PORT AS INT,                     -- Port
@PROGRESS AS INT,                 -- Progress
@STATE AS NVARCHAR(10),           -- State 
@DATE AS DATETIMEOFFSET(7),       -- Date and time of send
@MESSAGE AS NVARCHAR(512) = ''    -- Error Message
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  DECLARE @CNT AS INT
  DECLARE @NAME AS NVARCHAR(128)
  
  SELECT @CNT = [icnt], @NAME = [name] FROM [Datasets]  WHERE [uid] = @UID

  IF ( SELECT COUNT(*) FROM [Progress] WHERE [uid] = @UID AND [aetitle] = @AETITLE AND [server] = @SERVER AND [port] = @PORT AND [date] = @DATE ) = 0 
  BEGIN
    
    INSERT INTO [Progress] ( [uid], [aetitle], [server], [port], [progress], [total], [message], [name], [state], [date] ) 
            VALUES ( @UID, @AETITLE, @SERVER, @PORT, @PROGRESS, @CNT, @MESSAGE, @NAME, @STATE, @DATE )
  END
  ELSE
  BEGIN
   
   UPDATE [Progress] SET [progress] = @PROGRESS, [message] = @MESSAGE, [state] = @STATE WHERE [uid] = @UID AND [aetitle] = @AETITLE AND [port] = @PORT AND [date] = @DATE
    
  END
  
  RETURN 1
END
GO
/****** Object:  Table [dbo].[Images]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Images](
	[suid] [nvarchar](128) NOT NULL,
	[uid] [nvarchar](128) NOT NULL,
	[file] [nvarchar](512) NOT NULL,
 CONSTRAINT [PK_Images] PRIMARY KEY CLUSTERED 
(
	[suid] ASC,
	[uid] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Trigger [InsertImageTrigger]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE TRIGGER [dbo].[InsertImageTrigger]
   ON  [dbo].[Images]
   AFTER INSERT
AS 
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  UPDATE [Datasets] SET [icnt] = [icnt] + 1 FROM INSERTED WHERE [Datasets].[uid] = [suid] 

END
GO
/****** Object:  UserDefinedFunction [dbo].[fn_getOrientationInformation]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================================================
-- Author:		  Mark Kontak
-- Create date: 07/29/2009
-- Description:	Splits out the X, Y, Z cosines for the row and columns
-- =================================================================================================
CREATE FUNCTION [dbo].[fn_getOrientationInformation]
(
@COSINES NVARCHAR(128)
)
RETURNS @ORIENTATION_PLANE_TABLE TABLE 
(
  [orientation] NVARCHAR(32),
  [plane] NVARCHAR(32)
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

    SELECT @ROW_X = row_x, @ROW_Y = row_y, @ROW_Z = row_z, @COL_X = col_x, @COL_Y = col_y, @COL_Z = col_z FROM fn_getCosines(@COSINES) 
    
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
    ELSE IF ( @ROW_X > 1 AND @COL_Y < 0 ) 
    BEGIN
      SET @ORIENTATION = 'FFP'
    END  
    -- ==========================
    -- HFS (Head First Supine)
    --    1\0\0  : 0\1\0    
    -- ==========================
    ELSE IF ( @ROW_X < 0 AND @COL_Y > 0 )
    BEGIN
      SET @ORIENTATION = 'HFS'
    END
    -- ==========================
    -- FFS (Heet First Supine)
    --    -1\0\0  : 0\1\0    
    -- ==========================
    ELSE IF ( @ROW_X > 0 AND @COL_Y > 0 )
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
    ELSE IF ( @ROW_Y > 0 AND @COL_X < 1 )
    BEGIN
      SET @ORIENTATION = 'HFDR'
    END
    -- =====================================
    -- FFDR   (Feet First Decubitus-Right)
    --    0\-1\0 : -1\0\0          
    -- =====================================
    ELSE IF ( @ROW_Y < 0 AND @COL_X < 1 )
    BEGIN
      SET @ORIENTATION = 'FFDR'
    END
    ELSE
    BEGIN
      SET @ORIENTATION = ''
    END

    IF ( @ROW_Z = 0 AND @COL_Z = 0 ) 
    BEGIN
      SET @PLANE = 'Axial';
    END
    ELSE IF ( @ROW_X = 0 AND @COL_X = 0 )
    BEGIN
      SET @PLANE  = 'Sagittal';
    END
    ELSE IF ( @ROW_Y = 0 AND @COL_Y = 0 )
    BEGIN
      SET @PLANE  = 'Coronal';
    END
    ELSE
    BEGIN
      SET @PLANE = ''
    END
  
  END

  INSERT @ORIENTATION_PLANE_TABLE ( [orientation], [plane] ) VALUES ( @ORIENTATION, @PLANE )

  RETURN




END
GO
/****** Object:  StoredProcedure [dbo].[sp_purgeDatabase]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ================================================================
-- Author:		  Mark Kontak
-- Create date: 09/13/2010
-- Description:	Deletes all images and reset the image count 0.
-- ================================================================
CREATE PROCEDURE [dbo].[sp_purgeDatabase]
(
@ALL AS BIT = 0       -- If true then everything is purged, 0 only images are purged
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  -- Delete all progress
  DELETE FROM [Progress]
  
  -- Delete all images
  DELETE FROM [Images]
  
  -- Reset the image count back to 0
  UPDATE [Datasets] SET [icnt] = 0
  
END
GO
/****** Object:  StoredProcedure [dbo].[sp_getImages]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ==============================================================
-- Author:		  Mark Kontak
-- Create date: 09/13/2010
-- Description: Retreives all the files for th assocated series
-- ===============================================================
CREATE PROCEDURE [dbo].[sp_getImages]
(
@UID AS NVARCHAR(128),     -- Series UID
@RESULTS AS XML OUTPUT     -- Results
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

  SET @RESULTS = ( SELECT [file] FROM [Images] WHERE [suid] = @UID FOR XML RAW('image'), TYPE, ROOT('images') )
  
END
GO
/****** Object:  StoredProcedure [dbo].[sp_addImage]    Script Date: 09/15/2010 15:53:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- ====================================================================================
-- Author:		  Mark Kontak
-- Create date: 09/13/2010
-- Description:	Adds dicom image to the datasets
-- ====================================================================================
CREATE PROCEDURE [dbo].[sp_addImage]
(
@XML AS XML,            -- Dicom Header
@NAME AS NVARCHAR(64),  -- Directory name
@APP AS NVARCHAR(6),    -- Application
@FILE AS NVARCHAR(512)  -- File Path to store the DICOM into
)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

 
	DECLARE @RC AS INT
	DECLARE @RETURN AS INT

  SET @RETURN = 0
  
  -- =============================================================================================
  -- Wrap the remaining process in a try catch so that upon an exception the transaction can be
  -- rolled back.
  -- =============================================================================================
  BEGIN TRY
  
    -- ==================
    -- Local variables
    -- ==================
    DECLARE @IDOC AS INT
    DECLARE @ORIENTATION AS NVARCHAR(32)
    DECLARE @PLANE AS NVARCHAR(32)
    DECLARE @SCAN_DATE AS DATETIMEOFFSET(3)

    
    -- ==============
    -- Patient data
    -- ===============
    DECLARE @PATIENT_NAME AS NVARCHAR(64)
    DECLARE @PATIENT_MEDICAL_ID AS NVARCHAR(64)
    DECLARE @PATIENT_DOB AS NVARCHAR(8)
    DECLARE @SEX AS NVARCHAR(16)
    DECLARE @PATIENT_WEIGHT AS INT
    DECLARE @PATIENT_ADDRESS AS NVARCHAR(256)
    DECLARE @PATIENT_ID AS INT
   

    
    -- ===========
    -- Study data
    -- ===========
    DECLARE @STUDY_UID AS NVARCHAR(128)
    DECLARE @STUDY_DATE AS NVARCHAR(8)
    DECLARE @STUDY_TIME AS NVARCHAR(8)
    DECLARE @STUDY_ID AS NVARCHAR(32)
    DECLARE @STUDY_ACCESSION_NUMBER AS NVARCHAR(64)
    DECLARE @STUDY_DESCRIPTION AS NVARCHAR(256)
    DECLARE @STUDY_PHYSICIAN AS NVARCHAR(64)
    
    
    -- =============
    --- Series data
    -- =============
    DECLARE @SERIES_UID AS NVARCHAR(128)
    DECLARE @SERIES_DATE AS NVARCHAR(8)
    DECLARE @SERIES_TIME AS NVARCHAR(8)
    DECLARE @SERIES_NUMBER AS NVARCHAR(32)
    DECLARE @SERIES_MODALITY AS NVARCHAR(16)
    DECLARE @SERIES_DESCRIPTION AS NVARCHAR(256)
    DECLARE @SERIES_PATIENT_POSITION AS NVARCHAR(16)
    DECLARE @MANUFACTURER AS NVARCHAR(64)
    DECLARE @MODEL AS NVARCHAR(64)
    
    -- ==================
    -- Acquisition data
    -- ===================    
    DECLARE @ACQUISITION_DATE AS NVARCHAR(8)
    DECLARE @ACQUISITION_TIME AS NVARCHAR(8)
    DECLARE @ACQUISITION_NUMBER AS NVARCHAR(32)

    -- ==================
    -- Image data
    -- ===================    
    DECLARE @IMAGE_DATE AS NVARCHAR(8)
    DECLARE @IMAGE_TIME AS NVARCHAR(8)
    DECLARE @INSTANCE_UID AS NVARCHAR(128)
    DECLARE @IMAGE_COSINES AS NVARCHAR(128)
    DECLARE @IMAGE_NUMBER AS INT
    DECLARE @IMAGE_ORIENTATION AS NVARCHAR(128)
    DECLARE @RESCALE_SLOPE AS FLOAT
    DECLARE @RESCALE_INTERCEPT AS FLOAT
    DECLARE @FRAME_COUNT AS INT
    DECLARE @ROWS AS INT
    DECLARE @COLS AS INT
    

    -- Read the license xml
    EXEC sp_xml_preparedocument @IDOC OUTPUT, @XML

    -- Extract the user information 
    SELECT  @PATIENT_NAME           = patientName,
            @PATIENT_MEDICAL_ID     = patientMedicalId, 
            @SEX                    = patientSex,
            @PATIENT_DOB            = patientDob,
            @PATIENT_WEIGHT         = patientWeight,

            @STUDY_UID              = studyUid,
            @STUDY_DATE             = studyDate,
            @STUDY_TIME             = studyTime,
            @STUDY_ID               = studyId,
            @STUDY_ACCESSION_NUMBER = accessionNumber,
            @STUDY_DESCRIPTION      = studyDescription,
            @STUDY_PHYSICIAN        = studyPhysician,

            @ACQUISITION_DATE       = acquisitionDate,
            @ACQUISITION_TIME       = acquisitionTime,
            @ACQUISITION_NUMBER     = acquisitionNumber,

            @SERIES_UID             = seriesUid,
            @SERIES_DATE            = seriesDate,
            @SERIES_TIME            =   seriesTime,
            @SERIES_NUMBER          = seriesNumber,
            @SERIES_MODALITY        = seriesModality,
            @SERIES_DESCRIPTION     = seriesDescription,
            @MANUFACTURER           = manufacturer,
            @MODEL                  = model,
            
            @IMAGE_DATE             = imageDate,
            @IMAGE_TIME             = imageTime,
            @INSTANCE_UID           = instanceUid,
            @IMAGE_ORIENTATION      = imageOrientation,
            @IMAGE_COSINES          = imageCosines,
            
            @IMAGE_NUMBER           = imageNumber,
            @RESCALE_SLOPE          = rescaleSlope,
            @RESCALE_INTERCEPT      = rescaleIntercept,
            @FRAME_COUNT            = frameCount,
            @ROWS                   = [rows],
            @COLS                   = [columns]
        
        FROM OPENXML(@IDOC, N'/dicomHeader/data-set', 1) WITH 
        ( 
        
          -- =============
          -- Patient data
          -- ==============
          patientName       nvarchar(64)    './element[@tag="0010,0010"]',
          patientMedicalId  nvarchar(64)    './element[@tag="0010,0020"]',
          patientSex        nvarchar(10)    './element[@tag="0010,0040"]',
          patientDob        nvarchar(8)     './element[@tag="0010,0030"]',
          patientWeight     nvarchar(8)     './element[@tag="0010,1030"]',

          -- ===========
          -- Study data
          -- ===========
          studyUid          nvarchar(128) './element[@tag="0020,000d"]',
          studyDate         nvarchar(8) './element[@tag="0008,0020"]',
          studyTime         nvarchar(8) './element[@tag="0008,0030"]',
          studyId           nvarchar(32) './element[@tag="0020,0010"]',
          accessionNumber   nvarchar(64) './element[@tag="0008,0050"]',
          studyDescription  nvarchar(256) './element[@tag="0008,1030"]',
          studyPhysician    nvarchar(64) './element[@tag="0008,0090"]',

          acquisitionDate   nvarchar(8)     './element[@tag="0008,0022"]',
          acquisitionTime   nvarchar(8)     './element[@tag="0008,0032"]',
          acquisitionNumber nvarchar(32)    './element[@tag="0020,0012"]',
     
          seriesUid         nvarchar(128)   './element[@tag="0020,000e"]',
          seriesDate        nvarchar(8)     './element[@tag="0008,0021"]',
          seriesTime        nvarchar(8)     './element[@tag="0008,0031"]',
          seriesNumber      nvarchar(32)    './element[@tag="0020,0011"]',
          seriesModality    nvarchar(16)    './element[@tag="0008,0060"]',
          seriesDescription nvarchar(256)   './element[@tag="0008,103e"]',
          manufacturer      nvarchar(64)    './element[@tag="0008,0070"]',
          model             nvarchar(64)    './element[@tag="0008,1090"]',
          
          imageDate         nvarchar(8)     './element[@tag="0008,0023"]',
          imageTime         nvarchar(8)     './element[@tag="0008,0033"]',
          instanceUid       nvarchar(128)   './element[@tag="0008,0018"]',
          imageCosines      nvarchar(128)   './element[@tag="0020,0037"]',
          imageNumber       int             './element[@tag="0020,0013"]',
          imageOrientation  nvarchar(128)   './element[@tag="0020,0032"]',
          [rows]            int             './element[@tag="0028,0010"]',
          [columns]         int             './element[@tag="0028,0011"]',
          rescaleIntercept  float           './element[@tag="0028,1052"]',
          rescaleSlope      float           './element[@tag="0028,1053"]',
          frameCount        int             './element[@tag="0054,0053"]'

       )


    -- Remove the document
    EXEC sp_xml_removedocument @IDOC

  
    
    -- ========================================================================================================
    -- Check for missing instance UID
    -- ========================================================================================================
    IF @INSTANCE_UID IS NULL
    BEGIN
      
      
      RAISERROR('Invalid dicom, instance uid is missing', 1, 16)
      
      RETURN 0
      
    END -- END If the instance uid is missing

    
    -- Extract the image orientation from the COSINSE
    SELECT  @ORIENTATION = [orientation], @PLANE = [plane] FROM fn_getOrientationInformation(@IMAGE_COSINES)

    
    IF ( SELECT COUNT(*) FROM [Datasets] WHERE [uid] = @SERIES_UID ) = 0
    BEGIN
     INSERT INTO [Datasets] ( [uid], [model], [man], [mod], [ort], [name], [app], [pln] ) 
            VALUES ( @SERIES_UID, @MODEL, @MANUFACTURER, @SERIES_MODALITY, @ORIENTATION, @NAME, @APP, @PLANE )
    END
    
    IF ( SELECT COUNT(*) FROM [Images] WHERE [suid] = @SERIES_UID AND [uid] = @INSTANCE_UID ) = 0
    BEGIN
      INSERT INTO [Images] ( [suid], [uid], [file] ) 
              VALUES ( @SERIES_UID, @INSTANCE_UID, @FILE )
    END
    
    

  END TRY
  BEGIN CATCH
    
    DECLARE @ErrorMessage NVARCHAR(4000);
    DECLARE @ErrorSeverity INT;
    DECLARE @ErrorState INT;

    SELECT @ErrorMessage  = ERROR_MESSAGE(),
           @ErrorSeverity = ERROR_SEVERITY(),
           @ErrorState    = ERROR_STATE();

    RAISERROR (@ErrorMessage, @ErrorSeverity, @ErrorState );
    
    RETURN 0

  END CATCH
  
  RETURN 1

END -- END ... sp_receiveDicomImage
GO
/****** Object:  Default [DF_Datasets_uid]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_uid]  DEFAULT ('') FOR [uid]
GO
/****** Object:  Default [DF_Datasets_name]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_name]  DEFAULT ('') FOR [name]
GO
/****** Object:  Default [DF_Datasets_mod]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_mod]  DEFAULT ('') FOR [mod]
GO
/****** Object:  Default [DF_Datasets_man]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_man]  DEFAULT ('') FOR [man]
GO
/****** Object:  Default [DF_Datasets_model]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_model]  DEFAULT ('') FOR [model]
GO
/****** Object:  Default [DF_Datasets_ort]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_ort]  DEFAULT ('') FOR [ort]
GO
/****** Object:  Default [DF_Datasets_pln]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_pln]  DEFAULT ('') FOR [pln]
GO
/****** Object:  Default [DF_Datasets_qa]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_qa]  DEFAULT ((0)) FOR [qa]
GO
/****** Object:  Default [DF_Datasets_cpd]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_cpd]  DEFAULT ((0)) FOR [cpd]
GO
/****** Object:  Default [DF_Datasets_desc]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_desc]  DEFAULT ('') FOR [desc]
GO
/****** Object:  Default [DF_Datasets_icnt]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_icnt]  DEFAULT ((0)) FOR [icnt]
GO
/****** Object:  Default [DF_Datasets_app]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_app]  DEFAULT ('') FOR [app]
GO
/****** Object:  Default [DF_Datasets_type]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Datasets] ADD  CONSTRAINT [DF_Datasets_type]  DEFAULT ('') FOR [type]
GO
/****** Object:  Default [DF_Progress_uid]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_uid]  DEFAULT ('') FOR [uid]
GO
/****** Object:  Default [DF_Progress_aetitle]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_aetitle]  DEFAULT ('') FOR [aetitle]
GO
/****** Object:  Default [DF_Progress_server]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_server]  DEFAULT ('') FOR [server]
GO
/****** Object:  Default [DF_Progress_port]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_port]  DEFAULT ((0)) FOR [port]
GO
/****** Object:  Default [DF_Progress_date]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_date]  DEFAULT (getdate()) FOR [date]
GO
/****** Object:  Default [DF_Progress_state]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_state]  DEFAULT ('') FOR [state]
GO
/****** Object:  Default [DF_Progress_name]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_name]  DEFAULT ('') FOR [name]
GO
/****** Object:  Default [DF_Progress_progress]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_progress]  DEFAULT ((0)) FOR [progress]
GO
/****** Object:  Default [DF_Table_1_tot]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Table_1_tot]  DEFAULT ((0)) FOR [total]
GO
/****** Object:  Default [DF_Progress_message]    Script Date: 09/15/2010 15:53:48 ******/
ALTER TABLE [dbo].[Progress] ADD  CONSTRAINT [DF_Progress_message]  DEFAULT ('') FOR [message]
GO
