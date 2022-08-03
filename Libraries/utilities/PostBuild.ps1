#
# Script.ps1
#

if ( $args.Count -ne 2 )
{
    Write-Error "Missing parameters"
    exit -1
}

$source = $args[0]
$binaries = $args[1]
$info = $binaries + "\Viatronix\Info\"

Write-Output("Source Path:   " + $source);
Write-Output("Binaries Path: " + $binaries);
Write-Output("Info Path:     " + $info);
Write-Output("");


$tf = "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\tf.exe"

if ( ! [System.IO.Directory]::Exists($info) )
{
    MkDir($info)
}

$subdirs   = @("\Libraries","\Core","\Processor\src\Services","\Processor\src\Processing", "\Console","\Database","\Utilities","\VC","\V3DViewer")
$infofiles = @("libraries.info","core.info","services.info","processing.info", "console.info","database.info","utilities.info","vc.info","v3dviewer.info")


$i = 0

foreach ( $dir  in $subdirs) 
{
    $src = $source + $dir


    if  ( [System.IO.Directory]::Exists($src) )
    {
        $dst = $info + $infofiles[$i]

        Write-Output($src + " ---> " + $dst)

 
        & $tf info "$src" > "$dst" 
    }

    $i++
}

$src = $source + "\Data"
$dst = $binaries + "\Viatronix\Data"

Write-Output("");
Write-Output("Copying " + $src + "  to " + $dst)
Write-Output("");

xcopy /S /I /Y $src $dst