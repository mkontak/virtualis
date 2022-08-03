USE [master]
GO


IF NOT EXISTS (SELECT name FROM master.sys.server_principals WHERE name = 'v3duser') 
BEGIN
  CREATE LOGIN [v3duser] WITH PASSWORD=N'qwe321$', DEFAULT_DATABASE=[master], DEFAULT_LANGUAGE=[us_english], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF;
  
  ALTER LOGIN [v3duser] ENABLE
END
GO


IF NOT EXISTS (SELECT name FROM master.sys.server_principals WHERE name = 'v3dsa') 
BEGIN
  CREATE LOGIN [v3dsa] WITH PASSWORD=N'jkl789@', DEFAULT_DATABASE=[master], DEFAULT_LANGUAGE=[us_english], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF;
  
  ALTER LOGIN [v3dsa] ENABLE
END
GO

