RemoveViewer <-app APPLICATION> <-server SERVER> <-port PORT> <-user USERNAME> [-pwd PASSWORD]
             [-desc DESCRIPTION] [-cid CONTEXT][-new 1 or 0][-nomsg]

Will attempt to connect to the V3D Viewer Service to perform a remot launch of the specified viewer application.

where:

<-app APPLICATION>         Specifies the application to be launched ( VC, CAR, ... )
<-server SERVER>           Specifies the name of the server running the V3D Viewer Service
<-port PORT>               Specifies the port the service is listening on
<-user USERNAME>           Specifies either a valid V3D user name or and alias
[-pwd PASSWORD]            Required only if the -user is a V3D User
[-desc DESCRIPTION]        Allows the session description to be specified, only used if a new session is created
[-cid CONTEXT]             Specifies a V3D Authentication context token (Not Used)
[-new 1 or 0]              If 1 then a new session is created. If 0 a new session is only created if one does not already exists (default 0)
[-nomsg]                   Indicates that no Message Boxes are to be displayed
