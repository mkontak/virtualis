CreateUser [-msg 1|0] [-create] [-chgpwd 0|1] [-u USERNAME/LOGIN] [-p PASSWORD] [-r ROLE]  [-n NAME]

Allows the creation of a user in the V3D system. All users created using this utility use the Viatronix authentication. If the -create parameter is not
specified then only user/password validation is performed. Any errors will generate a non-zero error code. If no parameters are specified then the GUI is 
invoked allowing you to enter your information in their corresponding fields. In order to gain access to the GUI you will need to know the valid service 
password.

where:

[-create]                Specifies that the user is to be created, otherwise only validation occurs
[-chgpwd 0|1]            Specifies that the user will be required to change their password upon first login (Default is 1)
[-u USERTNAME/LOGIN]     Specifies the users login
[-p PASSWORD]            Specifies the users password
[-r ROLE]                Specifies the users Role (Default is Physician)
[-n NAME]                Specifies the users name
[-msg 0|1]               Specifies whether messages will be displayed (Default is 0)