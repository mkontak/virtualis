using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix
{

  #region enum AgentJobTypes

  [Flags]
  public enum AgentJobTypes
  {
    SQL = 0,
    EXE = 1
  };

  #endregion


  #region enum JobFrequency

  public enum JobFrequency
  {
    None      = 0,
    Daily     = 1,
    Weekly    = 2,
    Monthly   = 3
  };

  #endregion


  #region enum Days

  /// <summary>
  /// Days enumerator for AgentJobs
  /// </summary>
  [Flags]
  public enum Days
  {
    None      = 0x0000,
    Sunday    = 0x0001,
    Monday    = 0x0002,
    Tuesday   = 0x0004,
    Wednesday = 0x0008,
    Thursday  = 0x0010,
    Friday    = 0x0020,
    Satuday   = 0x0040
  };  

  #endregion

  #region enum Applications

  [Flags]
  public enum Applications
  {
    None    = 0x00000000,
    VC      = 0x00000001,         // Virtual Colonoscopy  
    CAR     = 0x00000002,         // Cardiac
    XP      = 0x00000004,         // Explorer
    VAS     = 0x00000008,         // Vascular
    ANG     = 0x00000010,         // Angio
    FN      = 0x00000020,         // Pet Fusion
    CS      = 0x00000040          // Calcium Scoring

  }

  #endregion

  #region enum DicomServiceTypes

  [Flags]
  public enum DicomServiceTypes
  {
    None          = 0x00000000,
    Query         = 0x00000001,       // C-FIND 
    Store         = 0x00000002,       // C-STORE
    Retrieve      = 0x00000004,       // C-MOVE
    Print         = 0x00000008,       // C-PRINT
    QueryRetrieve = Query | Retrieve,
    All           = DicomServiceTypes.QueryRetrieve | DicomServiceTypes.Print | DicomServiceTypes.Store
  }

  #endregion

  #region enum Location

  public enum Location
  {
    Local,
    Remote
  } // Location

  #endregion

 
  #region enum MessageTypes

  /// <summary>
  /// Defines the message types
  /// </summary>
  public enum MessageTypes
  {
    Information = 0,
    Warning = 1,
    Error = 2
  }

  #endregion

  #region enum ServerStates

  /// <summary>
  /// State of the server
  /// </summary>
  public enum ServerStates
  {
    Idle = 0,
    Running = 1,
    Aborted = 2,
    Failed = 3
  };

 
  #endregion

  #region enum Orientations

  [Flags]
  public enum Orientations : int
  {
    None = 0x00000000,
    HeadFirstSupine = 0x00000001,
    FeetFirstSupine = 0x00000002,
    HeadFirstProne = 0x00000004,
    FeetFirstProne = 0x00000008,
    HeadFirstDecubitusLeft = 0x00000010,
    FeetFirstDecubitusLeft = 0x00000020,
    HeadFirstDecubitusRight = 0x00000040,
    FeetFirstDecubitusRight = 0x00000080,

    Supine = HeadFirstSupine | FeetFirstSupine,
    Prone = HeadFirstProne | FeetFirstProne,
    DecubitusLeft = HeadFirstDecubitusLeft | FeetFirstDecubitusLeft,
    DecubitusRight = HeadFirstDecubitusRight | FeetFirstDecubitusRight,
  }

  #endregion

  #region enum Gender

  public enum Genders 
  {
    Unknown,
    Male,
    Female,
    Other,
  } // Gender

  #endregion

  #region enum Permissions

  [Flags]
  public enum Permissions : int
  {
    None = 0x00000000,
    Administer = 0x00000001,             // Allows administration to the V3D system
    Delete = 0x00000002,             // Allows deletion within the V3D system
    Process = 0x00000004,             // Allows processing within the V3D system
    Launch = 0x00000008,             // Allows launching within the V3D system
    Transfer = 0x00000010,             // Allows tranfer within the V3D system
    Save = 0x00000020,             // AllowS session saving within the V3D system
    Complete = 0x00000040,             // Allows session completion within the V3D system
    Auditing = 0x40000000,             // Allows auditing within the V3D system
    Service = 0x20000000,             // Allows service within the V3D system
    Add = 0x00000000,             // Allows add within the V3D system
  };

  #endregion

  #region enum SeriesTypes

  [Flags]
  public enum SeriesTypes : int
  {
    None          = 0x00000000,
    Dicom         = 0x00000001,
    Preprocessed  = 0x00000002,
    Cad           = 0x00000004,
    Session       = 0x00000008
  }

  #endregion

  #region enum States

  [Flags]
  public enum States : int
  {
    None        = 0x00000000,         // No state
    Warning     = 0x00000001,        
    Receiving   = 0x00000002,
    Processing  = 0x00000004,
    Converting  = 0x00000008,
    Completed   = 0x00000010,
    Pending     = 0x00000020,
    Failed      = 0x00000040,
    Aborted     = 0x00000080,
    Limited     = 0X00000100,
    Locked      = 0x00001000,
    Paused      = 0x00002000    // Paused (USed when a job is added and we do not want to run it now )
  }


  #endregion

  #region enum UserStates

  [Flags]
  public enum UserStates : int
  {
    None            = 0x00000000,                    // User is Active
    Locked          = 0x00000001,                  // User is locked
    Expired         = 0x00000002,                 // User has expied
    Disabled        = 0x00000004,
    PasswordChange  = 0x00000100            // Password change is required
  }

  #endregion

  #region enum LicenseStates

  [Flags]
  public enum LicenseStates : int
  {
    Available  = 0x00000001,      // License is available
    Inuse      = 0x00000001,      // License is inuse
    Dsiabled   = 0x00000002,      // License is disabled
    Expired    = 0x00000004,      // License has expired
    Locked     = 0x00000100       // License is locked
  }

  #endregion


  #region enum Settings

  [Flags]
  public enum Settings : int
  {
    None            = 0x00000000,   //  Default
    System          = 0x00000001,   //  System Entity 
    Privledeged     = 0x00000002,   //  Privledged   (User - Allows override of FREQUECY password setting)
    Viewing         = 0x00000016,   //  Allow Viewing (Role - Allows the role to be viewed by none service accounts ) 
    Inserting       = 0x00000032,   //  Not used
    Deleting        = 0X00000064    //  Not used
  }

  #endregion


  #region enum SubSystems

  /// <summary>
  /// Settings Subsystems
  /// </summary>
  public enum Subsystems
  {
    System,
    Security,
    Processing,
    Launching,
    Maintenace,
    Locking,
    Pacs,
    Receiving,
    Debug
  };


  #endregion

  #region enum Planes

  [Flags]
  public enum Planes : short
  {
    None      = 0x0000,
    Axial     = 0x0001,
    Coronal   = 0x0002,
    Sagittal  = 0x0004,
    Oblique   = 0x0008

  }

  #endregion

 
  #region enum StatusCode

  public enum StatusCode : int
  {
    None = 0,
  }

  #endregion

}
