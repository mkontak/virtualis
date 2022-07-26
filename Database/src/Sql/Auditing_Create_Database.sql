USE [master]
GO

-- =========================
-- Drop the database first
-- =========================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Auditing')
  PRINT 'Dropping [Auditing] Database'
  DROP DATABASE [Auditing]
GO

-- ===============================
-- Create the Auditing Database
-- ==============================

PRINT 'Dropping [Auditing] Database'

-- Create
CREATE DATABASE [Auditing]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'Auditing', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.V3DENT\MSSQL\DATA\Auditing.mdf' , SIZE = 11264KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )
 LOG ON 
( NAME = N'Auditing_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.V3DENT\MSSQL\DATA\Auditing_log.LDF' , SIZE = 10944KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)
GO

EXEC [Auditing].sys.sp_addextendedproperty @name=N'Viatronix', @value=N'True' 
GO
EXEC [Auditing].sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Contains all auditing logs for the V3D Enterprise System' 
GO

ALTER DATABASE [Auditing] SET COMPATIBILITY_LEVEL = 100
GO

IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [Auditing].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO

ALTER DATABASE [Auditing] SET ANSI_NULL_DEFAULT OFF 
GO

ALTER DATABASE [Auditing] SET ANSI_NULLS OFF 
GO

ALTER DATABASE [Auditing] SET ANSI_PADDING OFF 
GO

ALTER DATABASE [Auditing] SET ANSI_WARNINGS OFF 
GO

ALTER DATABASE [Auditing] SET ARITHABORT OFF 
GO

ALTER DATABASE [Auditing] SET AUTO_CLOSE ON 
GO

ALTER DATABASE [Auditing] SET AUTO_CREATE_STATISTICS ON 
GO

ALTER DATABASE [Auditing] SET AUTO_SHRINK ON 
GO

ALTER DATABASE [Auditing] SET AUTO_UPDATE_STATISTICS ON 
GO

ALTER DATABASE [Auditing] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO

ALTER DATABASE [Auditing] SET CURSOR_DEFAULT  GLOBAL 
GO

ALTER DATABASE [Auditing] SET CONCAT_NULL_YIELDS_NULL OFF 
GO

ALTER DATABASE [Auditing] SET NUMERIC_ROUNDABORT OFF 
GO

ALTER DATABASE [Auditing] SET QUOTED_IDENTIFIER OFF 
GO

ALTER DATABASE [Auditing] SET RECURSIVE_TRIGGERS OFF 
GO

ALTER DATABASE [Auditing] SET  ENABLE_BROKER 
GO

ALTER DATABASE [Auditing] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO

ALTER DATABASE [Auditing] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO

ALTER DATABASE [Auditing] SET TRUSTWORTHY OFF 
GO

ALTER DATABASE [Auditing] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO

ALTER DATABASE [Auditing] SET PARAMETERIZATION SIMPLE 
GO

ALTER DATABASE [Auditing] SET READ_COMMITTED_SNAPSHOT OFF 
GO

ALTER DATABASE [Auditing] SET HONOR_BROKER_PRIORITY OFF 
GO

ALTER DATABASE [Auditing] SET RECOVERY SIMPLE 
GO

ALTER DATABASE [Auditing] SET  MULTI_USER 
GO

ALTER DATABASE [Auditing] SET PAGE_VERIFY TORN_PAGE_DETECTION  
GO

ALTER DATABASE [Auditing] SET DB_CHAINING OFF 
GO

ALTER DATABASE [Auditing] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO

ALTER DATABASE [Auditing] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO

ALTER DATABASE [Auditing] SET  READ_WRITE 
GO

