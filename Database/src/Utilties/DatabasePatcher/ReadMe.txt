Application:  DatabasePatcher
Description:  Patches the database using the supplied/specified sql scripts.

Options:

[-c]              Disables the ability for the application to be canceled or closed.

[-l <LOGFILE>]    Specifies the log file path to be used if logging is required (Default is no logging)

[-f <SCRIPTFILE>] Specifies a file containing the lsit of sql scripts to be processed. This file can contain fulle paths or can be used 
                  in conjunction with the -d option. The order or the scripts in the file is the order in which they are processed.

                  Example File:
                     <scripts>
                       <script sql="DROP_ASSEMBLIES_SYSTEM.SQL"/>
                       <script sql="D:\Temp\DataPatch04_3.5.1.sql"/>
                    </scripts>

                    DROP_ASSEMBLIES_SYSTEM.SQL      The DROP_ASSEMBLIES_SYSTEM.SQL will have the -d directory prepended to it. If no -d is supplied then sql script is assumed to be a full path.
                    D:\Temp\DataPatch04_3.5.1.sql   The fulle path is specified to the -d will be ignored for this entry.


[-d <DIRECTORY>]  Specifies the SQL scripts directory.        

[-a <DIRECTORY>]  Speifies the assemblies directory (Required if an assembly is being patched).   

[-p <PASSWORD>]   Specifies the sa password to be used.

[-i <INSTANCE>]   Specifies the database instance server to patch (Defualt is in the application config and should
                  be (local)/V3D )

[-s <SCRIPT>]     Specifies a single script file. This option can contain a sql script with a full path or be used in conjunction with the -d option and just contain a file name.
                  Multiple -s options are allowed and the scripts will be processed in the order they are read from the command line. Can also be used with the -f option 
                  
              

Examples:


DatabasePatcher

Launching the database patcher with no options will launch the GUI in interactive mode allowing for a single script patch.


DatabasePatcher  -d "D:\Installs\Patch04\Sql" -s "SCRIPT01.sql" -s"SCRIPT02.sql"

The DatabasePatcher will require the installer to enter the instance and sa password to perform the patch. Clicking the [PATCH] button will process D:\Installs\Patch04\Sql\SCRIPT01.sql and 
D:\Installs\Patch04\Sql\SCRIPT02.sql (The order they apear on the command line).

DatabasePatcher  -d "D:\Installs\Patch04\Sql" -f -d "D:\Installs\Patch04\patch04.config" -c

The database patcher will process all the script files contained in the path04.config file using the -d option if necessary. The -c option indicates that the patcher cannot be canceled or closed so
the you can nly perform the patch. 

patch04.config
<scripts>
    <script sql="SCRIPT01.SQL"/>
    <script sql="D:\Installs\Patch04\Sql\Update\SCRIPT02.sql"/>
</scripts>

The patcher will process the follwoing scripts from the patch04.config file above:

D:\Installs\Patch04\Sql\SCRIPT01.sql          The -d options path is prepended.
D:\Installs\Patch04\Sql\Update\SCRIPT02.sql   The -d option is ignored and the full path supplied is used.

