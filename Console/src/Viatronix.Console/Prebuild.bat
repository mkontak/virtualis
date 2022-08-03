IF "%1"=="Release" GOTO RELEASE

GOTO DEBUG

:RELEASE
REGSVR32 /S "..\..\libs\com\ReportLib.dll"
XCOPY "..\..\libs\vx\bin\*.dll" %3 /Y /R
GOTO DONE

:DEBUG
XCOPY "..\..\..\CodeBase\libs\vx\bin\*.dll" %3 /Y /R
GOTO DONE

:DONE
XCOPY %2 %3 /Y /R