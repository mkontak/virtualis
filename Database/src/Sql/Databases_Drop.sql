
-- =======================
-- Drop Licensing database
-- =======================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Licensing')
  PRINT 'Dropping [Licensing] Database'
  DROP DATABASE [Licensing]
GO

-- =======================
-- Drop Auditing database
-- =======================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Auditing')
  PRINT 'Dropping [Auditing] Database'
  DROP DATABASE [Auditing]
GO

-- ==============================
-- Drop the Security Database
-- ==============================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Security')
  PRINT 'Dropping [Security] Database'
  DROP DATABASE [Security]
GO

-- ==============================
-- Drop the Storage Database
-- ==============================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Storage')
  PRINT 'Dropping [Storage] Database'
  DROP DATABASE [Storage]
GO

-- ==============================
-- Drop the System Database
-- ==============================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'System')
  PRINT 'Dropping [System] Database'
  DROP DATABASE [System]
GO

-- ==============================
-- Drop the Procesisng Database
-- ==============================
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'Processing')
  PRINT 'Dropping [Processing] Database'
  DROP DATABASE [Processing]
GO