
=================================================================================================================================================================
DeepCopy	<Source> <Destination>

Copies the source directory to the destination including all sub-directories and files.

where:		<Source>		Source directoiry you want to copy.
		<Destincation>		Destination of the source directory

Examples:	

DeepCopy 	C:\Temp\TestDir		D:\Temp

Creates a dub-directory D:\Temp\TestDir and copies the contents of C:\Temp\TestDir to D:\Temp\TextDir. 

DeepCopy 	C:\Temp\NewDir		D:\Temp\OldDir\.

Copies the contexts of C:\Temp\NewDir to D:\Temp\OldDir. Does not create a D:\Temp\OldDir\NewDir because of the \. at the end of thr destination.

=================================================================================================================================================================

GetHardwareInfo		

Gui allowing you to look at various hardware information.


=================================================================================================================================================================

FolderMonitoringService

Folder monitoring service used to monitor specified folders so that modifictions to files can be detected and status can be distributed by email and or logging. This service is currently being used to detect the JIRA backup.

=================================================================================================================================================================


SendEmail [-server SERVER NAME] [-port PORT] [-user USERNAME] [-password PASSWORD] [-ssl] 
	  [-from FROM] <-body BODY> <-to TO_1,TO_2,...> [-subject SUBJECT] [-a ATTACHMENT_1, ATTACHMENT_2, ... ]

Send a single email to the list addresses in the to parameter.

where:

	[-server SERVER NAME]			Identifies the SMTP server to connect to. The deafult is extracted from the config file.
	[-port PORT]				Identifies the SMTP port to connect to. The default is extracted from the config file.
	[-user USERNAME]			Identifies the user name to use to log into the SMTP server with if required. Default
						is extracted from the config file.
	[-password PASSWORD]			Identifies the user names password used to log into the SMTP server if required. Default is 
						extracted from the config file.
	[-ssl]					Identifies if the SMTP is using SSL. Default is extracted from the config file.
	[-from FROM]				Identifies the FROM field for the email, must be valid user. Default is extracted from the config file.
	<-body BODY>				Specifies body of the email.
	<-to TO_1,TO_2,...>			Specifies the list of users to send the email to. 
	[-subject SUBJECT]			Subject of email. Default is extracted from the config file.
	[-a ATTACHMENT_1, ATTACHMENT_2, ... ]	List of attachments to be included in the email.

=================================================================================================================================================================
