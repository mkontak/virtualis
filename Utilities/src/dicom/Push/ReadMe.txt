

Command:      Push

Description:  Performs a C-MOVE DICOM action which essentially queries the specified PACS using any of the query specific args that appear below. 
              If no data is found you will receive a "unable to process" response from the PACS. Any data match will be sent to the destination 
              specified.  
              

Arguments:  

<-a AE title>             AE Title of this program which is required.
                          Default may be set in the app.config file.

<-d destination>          Destination AE Title (This is sent to the PACS to tell it were to push the data to),
                          Default may be set in the app.config file.


Query specific args
--------------------

[-study UID]              Study uid.
[-series UID]             Series uid.
[-instance UID]           Instance/Image uid.
[-i Patient Id]           Patient Id
[-n Patient Name]         Patient Name

Connection specific args
-------------------------

[-h Hostname]             PACS servers host name that appears in the DICOM's host.xml file

[-s Server/IP]            PACS Server name or IP
[-sa Servers AE Title]    AE Title of the PACS server
[-p Port]                 Port to connect to

