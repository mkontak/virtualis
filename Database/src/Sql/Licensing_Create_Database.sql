USE [master]
GO


-- ==================================
-- Drop the Licensing database first
-- ===================================
IF EXISTS (SELECT * FROM master.dbo.sysdatabases WHERE name = N'Licensing')
  PRINT 'Dropping [Licensing] Database'
  DROP DATABASE [Licensing]
GO


-- ===============================
-- Create the Licensing database
-- ===============================

PRINT 'Creating [Licensing] Database'

-- Create
CREATE DATABASE [Licensing]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'Licensing', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.V3DENT\MSSQL\DATA\Licensing.mdf' , SIZE = 11264KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )
 LOG ON 
( NAME = N'Licensing_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.V3DENT\MSSQL\DATA\Licensing_log.ldf' , SIZE = 10944KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)
GO

EXEC [Licensing].sys.sp_addextendedproperty @name=N'Viatronix', @value=N'True' 
GO
EXEC [Licensing].sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all licensing information for the V3D Enterprise System' 
GO

ALTER DATABASE [Licensing] SET COMPATIBILITY_LEVEL = 110
GO

IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [Licensing].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO

ALTER DATABASE [Licensing] SET ANSI_NULL_DEFAULT ON 
GO

ALTER DATABASE [Licensing] SET ANSI_NULLS ON 
GO

ALTER DATABASE [Licensing] SET ANSI_PADDING ON 
GO

ALTER DATABASE [Licensing] SET ANSI_WARNINGS ON 
GO

ALTER DATABASE [Licensing] SET ARITHABORT ON 
GO

ALTER DATABASE [Licensing] SET AUTO_CLOSE OFF 
GO

ALTER DATABASE [Licensing] SET AUTO_CREATE_STATISTICS ON 
GO

ALTER DATABASE [Licensing] SET AUTO_SHRINK OFF 
GO

ALTER DATABASE [Licensing] SET AUTO_UPDATE_STATISTICS ON 
GO

ALTER DATABASE [Licensing] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO

ALTER DATABASE [Licensing] SET CURSOR_DEFAULT  LOCAL 
GO

ALTER DATABASE [Licensing] SET CONCAT_NULL_YIELDS_NULL ON 
GO

ALTER DATABASE [Licensing] SET NUMERIC_ROUNDABORT OFF 
GO

ALTER DATABASE [Licensing] SET QUOTED_IDENTIFIER ON 
GO

ALTER DATABASE [Licensing] SET RECURSIVE_TRIGGERS OFF 
GO

ALTER DATABASE [Licensing] SET  DISABLE_BROKER 
GO

ALTER DATABASE [Licensing] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO

ALTER DATABASE [Licensing] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO

ALTER DATABASE [Licensing] SET TRUSTWORTHY ON 
GO

ALTER DATABASE [Licensing] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO

ALTER DATABASE [Licensing] SET PARAMETERIZATION SIMPLE 
GO

ALTER DATABASE [Licensing] SET READ_COMMITTED_SNAPSHOT OFF 
GO

ALTER DATABASE [Licensing] SET HONOR_BROKER_PRIORITY OFF 
GO

ALTER DATABASE [Licensing] SET RECOVERY FULL 
GO

ALTER DATABASE [Licensing] SET  MULTI_USER 
GO

ALTER DATABASE [Licensing] SET PAGE_VERIFY NONE  
GO

ALTER DATABASE [Licensing] SET DB_CHAINING OFF 
GO

ALTER DATABASE [Licensing] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO

ALTER DATABASE [Licensing] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO

ALTER DATABASE [Licensing] SET  READ_WRITE 
GO

