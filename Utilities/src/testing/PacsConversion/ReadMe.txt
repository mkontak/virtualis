acsConversion  [-e] [-study uid] [-series uid] [-instance uid] [-n patient name]
                [i patient id] <-s server name/ip> <-p port> <-a aetitle> 
                <-sa servers aetitle> <-d destination aetitle> <-dp destination port>
                [-w volume file]

Starts an internal DICOM server using the destination aetitle and the destination port. Sends a DICOM C-MOVE
argv[i] to the PACS server specfied by the -s, -p and -sa parameteres. This tell the PACS to send the images 
specified by the uid or patient parameters to the destination. The destination aetitle must be known by the server
on the PACS.

\nWhere:

[-e]                     Forces the appication to wait for a ENTER key to be pressed at the end of the                          
                         process. Helps detect errors if they occur. (Default is off)

[-w volume file]         Indicates that the volume is to be written to the file specified

[-study uid]             Specifies the study uid to retrieve.

[-series uid]            Specifies the series uid to retrieve.

[-instance uid]          Specifies the instnace uid to retrieve.

[-i patient id]          Specifies the patient id to retrieve.

[-n patient name]        Specifies the patient name to retrieve.

<-s server name/ip>      Specifies the servers name or ip address.

<-p port>                Specifies the port the server is listening on.

<-sa servers aetitle>    Specifies the server aetitle.

<-a aetitle>             Specifies the aetitle of this application.

<-d destination aetitle> Specifies the server's aetitle that will be started for retreival and is
                         destination aetitle send to the PACS to indicate where the images are to be
                         pushed.

<-dp destination port>   Specifies the server's port that will be started for the retreival.



Example:


PacsConversion -a V3D_SCU -s GRUMPY -sa CONQUEST -p 5678 -e -d V3D_APOLLO -dp 104 -study 1.2.840.113619.2.55.1.1762882978.2182.1046869733.677 -series 1.2.840.113619.2.55.1.1762882978.2182.1046869734.67

This command will open a client connection to the CONQUEST server running on GRUMPY listening on port 5678 using the AE title of V3D_SCU. The retrieval 
launch a DICOM server using AE Title V3D_APOLO listening on prot 104. A request to push the study/series, specified by the uid's above, to V3D_APOLLO 
will be send to the CONQUEST pacs. 

Notes:


-study 1.2.840.113619.2.55.1.1762882978.2182.1046869733.677     Specifies the AppleCores datasets study UID
-series 1.2.840.113619.2.55.1.1762882978.2182.1046869734.67      Specifies the prone series of the AppleCore dataset

The -d and -dp specifiy the internal DICOM servers aetitle/port and also the destinations aetitle specified for the push. This
aetitle/port must be known on the CONQUEST server for the operation to work. 

If the aetitle/port does not exist in the known server list on the PACS or the uid's specified are not in the PACS the you 
will receive an "unable to process" error.

