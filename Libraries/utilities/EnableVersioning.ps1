#
# Script.ps1
#
param([string] $path)

dir D:\Builds -r *.* | % { $_.fullname } | % { attrib -r $_ } 


$file = $path + "\Libraries\utilities\Versioning.exe.config";
$tmp = $path + "\Libraries\utilities\Old_Versioning.exe.config";

get-content $file | Out-File $tmp

get-content $tmp | ForEach-Object { $_ -replace 'key="enabled" value="false"','key="enabled" value="true"' } | Out-File $file -Encoding ascii;

remove-item $tmp;