@echo off
TITLE test

echo Tool to create a directory for delivery to a CAD vendor

set output="Viatronix CAD API"
set binaries="..\..\..\bin"
set tlbdir="..\..\..\bin\com"


echo making output directory
IF NOT EXIST %output% MD %output%

echo copying vxProcessor\CAD
echo ______________________________________________________________
IF NOT EXIST %output%\Test md %output%\Test
COPY %binaries%\vxProcessor\Viatronix.v3D.Processor.Plugins.CAD.exe %output%\Test\ /Y
COPY %binaries%\vxProcessor\Viatronix.v3D.Processor.Plugins.CAD.exe.config %output%\Test\ /Y
COPY %binaries%\vxProcessor\vxBase32.dll %output%\Test\ /Y
COPY %binaries%\vxProcessor\vxBlockVolume32.dll %output%\Test\ /Y
COPY %binaries%\vxProcessor\vxLogger32.dll %output%\Test\ /Y
COPY %tlbdir%\vxComCad.tlb %output%\ /Y


echo copying vxUtilities
echo ______________________________________________________________
IF NOT EXIST %output%\Convert md %output%\Convert
COPY %binaries%\vxUtilities\CreateVolume.exe %output%\Convert\ /Y
COPY %binaries%\vxUtilities\vxLogger32.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\vxBase32.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\vxBlockVolume32.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\vxViewer32.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\msiCOM3.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.Visualization.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.Logger.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.Core.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.UI.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.Dicom.dll %output%\Convert\ /Y
COPY %binaries%\vxUtilities\Viatronix.v3D.Utilities.dll %output%\Convert\ /Y

echo copying sample projects
echo ______________________________________________________________
IF NOT EXIST %output%\sample_projects md %output%\sample_projects
XCOPY vs2003 %output%\sample_projects\vs2003\ /Y /R /S /F
XCOPY vc6 %output%\sample_projects\vc6\ /Y /R /S /F

echo copying documentation
echo ______________________________________________________________
COPY *.doc %output% /Y
