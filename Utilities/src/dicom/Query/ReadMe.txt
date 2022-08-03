Query [-e] [-study uid] [-series uid] [-instance uid] [-physician physician] [-patientName name] [-patientId id]
      [-m modalities] [-l level] [-stusdyDate date] [-seriesDate date] [-studyTime time]
      [-studyId id] [-seriesTime time] [-dob date of borth] [-tob time of birth]
      <-s server name/ip> <-p port> <-a aetitle> <-sa servers aetitle>

Sends a DICOM C-FIND command to the server specified by the -s, -p and -sa commands. The server will
send the images specified to the destination (-d) aetitle specified. The destination aetitle must be
known by the server.


Where:
<-s server name/ip>          Specifies the servers name or ip address.

<-p port>                    Specifies the port the server is listening on.

<-sa servers aetitle>        Specifies the server aetitle.

<-a aetitle>                 Specifies the aetitle of this application.

[-l level]                     Specifies the level of te query 'PATIENT', 'STUDY', 'SERIES' or
                               'IMAGE' to retrieve. (Default is 'SERIES')

[-e]                           Forces the application to wait for a ENTER key to be pressed at the end of the 
                               process. Helps detect errors if they occur. (Default is off)

[-patientName name/name/...]   Specifies the patient names to retrieve. (A single name ncan be specified and '*' 
                             is used a wildcard character)

[-patientId id]                Specifies the medical id to retrieve.

[-dob patient dob <from-to>]    Specifies the from/to patient dob to retrieve. (Can be a single date)

[-tob Time of birth <from-to>]  Specifies the from/to patient tob to retrieve. (Can be a single time)

[-study uid/...]               Specifies the list of study uids to retrieve. (A single uid can be specified)

[-studyId id]                  Specifies the study id to retrieve.

[-studyDate date <from-to>]     Specifies the from/to study date to retrieve. (Can be a single date)

[-studyTime time <from-to>]     Specifies the from/to study time to retrieve. (Can be a single time)

[-series uid/...]              Specifies the list of series uid to retrieve. (A single uid can be specified)

[-seriesDate date <from-to>]    Specifies the from/to series date to retrieve. (Can be a single date)

[-seriesTime time <from-to>]    Specifies the from/to series time to retrieve. (Can be a single time)

[-m modality/...]              Specifies the list modalities to retrieve. (A single modality can be specified)

[-instance uid/...]            Specifies the list instance uid to retrieve. (A single uid can be specified)

[-physician  physician]         Specifies the physician to retrieve.


