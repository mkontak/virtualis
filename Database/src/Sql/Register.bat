@ECHO OFF
osql.exe -S %1 -d %2 -U sa -P %3 -Q "EXEC [v3d].[sp_register] @EXPTTL = %4, @EXPUNITS = %5"