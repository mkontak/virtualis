using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Viatronix.Launching
{

  /// <summary>
  /// Implements the p-invoke win32 functions used in the launching
  /// </summary>
  public static class Win32
  {
    public const uint  STANDARD_RIGHTS_REQUIRED = 0x000F0000;

    [Flags]

    public enum TOKEN_ACCESS : uint
    {
     TOKEN_ASSIGN_PRIMARY    = (0x0001),
     TOKEN_DUPLICATE         = (0x0002),
     TOKEN_IMPERSONATE       = (0x0004),
     TOKEN_QUERY             = (0x0008),
     TOKEN_QUERY_SOURCE      = (0x0010),
     TOKEN_ADJUST_PRIVILEGES = (0x0020),
     TOKEN_ADJUST_GROUPS     = (0x0040),
     TOKEN_ADJUST_DEFAULT    = (0x0080),
     TOKEN_ADJUST_SESSIONID  = (0x0100),
     TOKEN_ALL_ACCESS        =  ( STANDARD_RIGHTS_REQUIRED  |
                                  TOKEN_ASSIGN_PRIMARY      |
                                  TOKEN_DUPLICATE           |
                                  TOKEN_IMPERSONATE         |
                                  TOKEN_QUERY               |
                                  TOKEN_QUERY_SOURCE        |
                                  TOKEN_ADJUST_PRIVILEGES   |
                                  TOKEN_ADJUST_GROUPS       |
                                  TOKEN_ADJUST_DEFAULT      |
                                  TOKEN_ADJUST_SESSIONID )
    }

    #region const 

    public const int NO_ERROR = 0;

    public const int ERROR_INSUFFICIENT_BUFFER = 122;

    public const uint SYNCHRONIZE = 0x00100000;

    public const uint PROCESS_ALL_ACCESS = STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFFF;

    public const uint GENERIC_ALL = 0x10000000;

    public const UInt32 INFINITE = 0xFFFFFFFF;

    public const UInt32 WAIT_FAILED = 0xFFFFFFFF;

    public const UInt32 WAIT_TIMEOUT = 0x00000102;

    #endregion

    /// <summary>
    /// Process Creation Flags
    /// </summary>
    [Flags]
    public enum CreationFlags : uint
    {
       DEBUG_PROCESS                    = 0x00000001,
       DEBUG_ONLY_THIS_PROCESS          = 0x00000002,
       CREATE_SUSPENDED                 = 0x00000004,
       DETACHED_PROCESS                 = 0x00000008,

       CREATE_NEW_CONSOLE               = 0x00000010,
       NORMAL_PRIORITY_CLASS            = 0x00000020,
       IDLE_PRIORITY_CLASS              = 0x00000040,
       HIGH_PRIORITY_CLASS              = 0x00000080,

       REALTIME_PRIORITY_CLASS          = 0x00000100,
       CREATE_NEW_PROCESS_GROUP         = 0x00000200,
       CREATE_UNICODE_ENVIRONMENT       = 0x00000400,
       CREATE_SEPARATE_WOW_VDM          = 0x00000800,

       CREATE_SHARED_WOW_VDM            = 0x00001000,
       CREATE_FORCEDOS                  = 0x00002000,
       BELOW_NORMAL_PRIORITY_CLASS      = 0x00004000,
       ABOVE_NORMAL_PRIORITY_CLASS      = 0x00008000,

       INHERIT_PARENT_AFFINITY          = 0x00010000,
       INHERIT_CALLER_PRIORITY          = 0x00020000,    // Deprecated
       CREATE_PROTECTED_PROCESS         = 0x00040000,
       EXTENDED_STARTUPINFO_PRESENT     = 0x00080000,

       PROCESS_MODE_BACKGROUND_BEGIN    = 0x00100000,
       PROCESS_MODE_BACKGROUND_END      = 0x00200000,

       CREATE_BREAKAWAY_FROM_JOB        = 0x01000000,
       CREATE_PRESERVE_CODE_AUTHZ_LEVEL = 0x02000000,
       CREATE_DEFAULT_ERROR_MODE        = 0x04000000,
       CREATE_NO_WINDOW                 = 0x08000000,

       PROFILE_USER                     = 0x10000000,
       PROFILE_KERNEL                   = 0x20000000,
       PROFILE_SERVER                   = 0x40000000,
       CREATE_IGNORE_SYSTEM_DEFAULT     = 0x80000000

    }


    /// <summary>
    /// Windows Terminal Server Connection States
    /// </summary>
    public enum WTS_CONNECTSTATE_CLASS
    {
      WTSActive,              // User logged on to WinStation
      WTSConnected,           // WinStation connected to client
      WTSConnectQuery,        // In the process of connecting to client
      WTSShadow,              // Shadowing another WinStation
      WTSDisconnected,        // WinStation logged on without client
      WTSIdle,                // Waiting for client to connect
      WTSListen,              // WinStation is listening for connection
      WTSReset,               // WinStation is being reset
      WTSDown,                // WinStation is down due to error
      WTSInit,                // WinStation in initialization
    } ;


    /// <summary>
    /// Windows Terminal Server Session Info
    /// </summary>
    public struct WTS_SESSION_INFO
    {
      public uint SessionId;                // session id
      public string pWinStationName;        // name of WinStation this session is
      public WTS_CONNECTSTATE_CLASS State;  // State
    };

    /// <summary>
    /// TOKEN Types
    /// </summary>
    public enum TOKEN_TYPE
    {
      TokenPrimary = 1,
      TokenImpersonation

    }

    /// <summary>
    /// TOKEN_USER struct
    /// </summary>
    public struct TOKEN_USER
    {
      public SID_AND_ATTRIBUTES User;
    }

    /// <summary>
    /// SID_AND_ATTRIBUTES strucur
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SID_AND_ATTRIBUTES
    {

      public IntPtr Sid;
      public int Attributes;
    } 


    /// <summary>
    /// Token information types used in GetToeknInformation
    /// </summary>
    public enum TOKEN_INFORMATION_CLASS
    {
      /// <summary>
      /// The buffer receives a TOKEN_USER structure that contains the user account of the token.
      /// </summary>
      TokenUser = 1,

      /// <summary>
      /// The buffer receives a TOKEN_GROUPS structure that contains the group accounts associated with the token.
      /// </summary>
      TokenGroups,

      /// <summary>
      /// The buffer receives a TOKEN_PRIVILEGES structure that contains the privileges of the token.
      /// </summary>
      TokenPrivileges,

      /// <summary>
      /// The buffer receives a TOKEN_OWNER structure that contains the default owner security identifier (SID) for newly created objects.
      /// </summary>
      TokenOwner,

      /// <summary>
      /// The buffer receives a TOKEN_PRIMARY_GROUP structure that contains the default primary group SID for newly created objects.
      /// </summary>
      TokenPrimaryGroup,

      /// <summary>
      /// The buffer receives a TOKEN_DEFAULT_DACL structure that contains the default DACL for newly created objects.
      /// </summary>
      TokenDefaultDacl,

      /// <summary>
      /// The buffer receives a TOKEN_SOURCE structure that contains the source of the token. TOKEN_QUERY_SOURCE access is needed to retrieve this information.
      /// </summary>
      TokenSource,

      /// <summary>
      /// The buffer receives a TOKEN_TYPE value that indicates whether the token is a primary or impersonation token.
      /// </summary>
      TokenType,

      /// <summary>
      /// The buffer receives a SECURITY_IMPERSONATION_LEVEL value that indicates the impersonation level of the token. If the access token is not an impersonation token, the function fails.
      /// </summary>
      TokenImpersonationLevel,

      /// <summary>
      /// The buffer receives a TOKEN_STATISTICS structure that contains various token statistics.
      /// </summary>
      TokenStatistics,

      /// <summary>
      /// The buffer receives a TOKEN_GROUPS structure that contains the list of restricting SIDs in a restricted token.
      /// </summary>
      TokenRestrictedSids,

      /// <summary>
      /// The buffer receives a DWORD value that indicates the Terminal Services session identifier that is associated with the token. 
      /// </summary>
      TokenSessionId,

      /// <summary>
      /// The buffer receives a TOKEN_GROUPS_AND_PRIVILEGES structure that contains the user SID, the group accounts, the restricted SIDs, and the authentication ID associated with the token.
      /// </summary>
      TokenGroupsAndPrivileges,

      /// <summary>
      /// Reserved.
      /// </summary>
      TokenSessionReference,

      /// <summary>
      /// The buffer receives a DWORD value that is nonzero if the token includes the SANDBOX_INERT flag.
      /// </summary>
      TokenSandBoxInert,

      /// <summary>
      /// Reserved.
      /// </summary>
      TokenAuditPolicy,

      /// <summary>
      /// The buffer receives a TOKEN_ORIGIN value. 
      /// </summary>
      TokenOrigin,

      /// <summary>
      /// The buffer receives a TOKEN_ELEVATION_TYPE value that specifies the elevation level of the token.
      /// </summary>
      TokenElevationType,

      /// <summary>
      /// The buffer receives a TOKEN_LINKED_TOKEN structure that contains a handle to another token that is linked to this token.
      /// </summary>
      TokenLinkedToken,

      /// <summary>
      /// The buffer receives a TOKEN_ELEVATION structure that specifies whether the token is elevated.
      /// </summary>
      TokenElevation,

      /// <summary>
      /// The buffer receives a DWORD value that is nonzero if the token has ever been filtered.
      /// </summary>
      TokenHasRestrictions,

      /// <summary>
      /// The buffer receives a TOKEN_ACCESS_INFORMATION structure that specifies security information contained in the token.
      /// </summary>
      TokenAccessInformation,

      /// <summary>
      /// The buffer receives a DWORD value that is nonzero if virtualization is allowed for the token.
      /// </summary>
      TokenVirtualizationAllowed,

      /// <summary>
      /// The buffer receives a DWORD value that is nonzero if virtualization is enabled for the token.
      /// </summary>
      TokenVirtualizationEnabled,

      /// <summary>
      /// The buffer receives a TOKEN_MANDATORY_LABEL structure that specifies the token's integrity level. 
      /// </summary>
      TokenIntegrityLevel,

      /// <summary>
      /// The buffer receives a DWORD value that is nonzero if the token has the UIAccess flag set.
      /// </summary>
      TokenUIAccess,

      /// <summary>
      /// The buffer receives a TOKEN_MANDATORY_POLICY structure that specifies the token's mandatory integrity policy.
      /// </summary>
      TokenMandatoryPolicy,

      /// <summary>
      /// The buffer receives the token's logon security identifier (SID).
      /// </summary>
      TokenLogonSid,

      /// <summary>
      /// The maximum value for this enumeration
      /// </summary>
      MaxTokenInfoClass
    }


    /// <summary>
    /// Security Information use parameter
    /// </summary>
    public enum SID_NAME_USE
    {
      SidTypeUser = 1,
      SidTypeGroup,
      SidTypeDomain,
      SidTypeAlias,
      SidTypeWellKnownGroup,
      SidTypeDeletedAccount,
      SidTypeInvalid,
      SidTypeUnknown,
      SidTypeComputer
    }

    /// <summary>
    /// Security Impersonation Levels
    /// </summary>
    public enum SECURITY_IMPERSONATION_LEVEL
    {
      SecurityAnonymous,
      SecurityIdentification,
      SecurityImpersonation,
      SecurityDelegation
    } 

    /// <summary>
    /// Process Startup 
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct STARTUPINFO
		{
			public int cb;
			public String lpReserved;
			public String lpDesktop;
			public String lpTitle;
			public uint dwX;
			public uint dwY;
			public uint dwXSize;
			public uint dwYSize;
			public uint dwXCountChars;
			public uint dwYCountChars;
			public uint dwFillAttribute;
			public uint dwFlags;
			public short wShowWindow;
			public short cbReserved2;
			public IntPtr lpReserved2;
			public IntPtr hStdInput;
			public IntPtr hStdOutput;
			public IntPtr hStdError;
		}

    /// <summary>
    /// Process Information
    /// </summary>
		[StructLayout(LayoutKind.Sequential)] 
    public struct PROCESS_INFORMATION
		{
			public IntPtr hProcess;
			public IntPtr hThread;
			public uint   dwProcessId;
			public uint   dwThreadId;
		};

    /// <summary>
    /// Security Attributes
    /// </summary>
		[StructLayout(LayoutKind.Sequential)] 
    public struct SECURITY_ATTRIBUTES
		{
			public int    Length;
			public IntPtr lpSecurityDescriptor;
			public bool   bInheritHandle;
		};

    /// <summary>
    /// Closes a handle
    /// </summary>
    /// <param name="handle"></param>
    /// <returns></returns>
		[DllImport("kernel32.dll", EntryPoint="CloseHandle", SetLastError=true, CharSet=CharSet.Auto)]
		public extern static bool CloseHandle(IntPtr handle);

    /// <summary>
    /// Closes the handle checking the return code and logging an error message
    /// </summary>
    /// <param name="hHandle"></param>
    /// <param name="type"></param>
    public static void CloseHandle(System.IntPtr hHandle, string type)
    {

      Logging.Log.Debug(string.Format("Attempting to close {0} handle", type), "Win32", "CloseHandle");

      if (hHandle != IntPtr.Zero)
      {
        bool result = Win32.CloseHandle(hHandle);

        if (result == false)
          Logging.Log.Error(string.Format("Failed to close {0} handle : {1}", type, Marshal.GetLastWin32Error()), "Win32", "CloseHandle");
      }
      else
        Logging.Log.Warning(string.Format("{0} handle is already closed", type), "Win32", "CloseHandle");

      hHandle = IntPtr.Zero;

    } // CloseHandle(System.IntPtr hHandle, string type)


    /// <summary>
    /// Gets the processes exit code
    /// </summary>
    /// <param name="hProcess">Process handle</param>
    /// <param name="exitCode">Exit code</param>
    /// <returns></returns>
    [DllImport("kernel32.dll", EntryPoint="CloseHandle", SetLastError=true, CharSet=CharSet.Auto)]
    public extern static bool GetExitCodeProcess( IntPtr hProcess, ref int exitCode );

    /// <summary>
    /// Creates a process as user 
    /// </summary>
    /// <param name="hToken"></param>
    /// <param name="lpApplicationName"></param>
    /// <param name="lpCommandLine"></param>
    /// <param name="lpProcessAttributes"></param>
    /// <param name="lpThreadAttributes"></param>
    /// <param name="bInheritHandle"></param>
    /// <param name="dwCreationFlags"></param>
    /// <param name="lpEnvironment"></param>
    /// <param name="lpCurrentDirectory"></param>
    /// <param name="lpStartupInfo"></param>
    /// <param name="lpProcessInformation"></param>
    /// <returns></returns>
		[DllImport("advapi32.dll", EntryPoint="CreateProcessAsUser", SetLastError=true, CharSet=CharSet.Ansi)]
		public extern static bool CreateProcessAsUser(IntPtr hToken, 
                                                  String lpApplicationName, 
                                                  String lpCommandLine, 
                                                  ref SECURITY_ATTRIBUTES lpProcessAttributes, 
			                                            ref SECURITY_ATTRIBUTES lpThreadAttributes, 
                                                  bool bInheritHandle, 
                                                  uint dwCreationFlags, 
                                                  IntPtr lpEnvironment,
			                                            String lpCurrentDirectory, 
                                                  ref STARTUPINFO lpStartupInfo, 
                                                  out PROCESS_INFORMATION lpProcessInformation);

    /// <summary>
    /// Duplicates the security token
    /// </summary>
    /// <param name="hExistingTokenHandle">Token to be duplicates</param>
    /// <param name="dwDesiredAccess">Desired access code</param>
    /// <param name="lpThreadAttributes">Thread security attributes</param>
    /// <param name="TokenType">Token type</param>
    /// <param name="ImpersonationLevel">Impersonation level</param>
    /// <param name="hDuplicateTokenHandle">New duplicated handle</param>
    /// <returns></returns>
		[DllImport("advapi32.dll", EntryPoint="DuplicateTokenEx")]
		public extern static bool DuplicateTokenEx( IntPtr hExistingTokenHandle, 
                                                uint dwDesiredAccess, 
			                                          ref SECURITY_ATTRIBUTES lpThreadAttributes, 
                                                int TokenType,
			                                          int ImpersonationLevel,
                                                out  IntPtr hDuplicateTokenHandle);


    #region WTS Functions


    /// <summary>
    /// Windows terminal Service gets the active session id
    /// </summary>
    /// <returns></returns>
    [DllImport("Kernel32", CharSet = CharSet.Auto)]
    public extern static uint WTSGetActiveConsoleSessionId();

    /// <summary>
    /// Windows Terminal Server opens the server
    /// </summary>
    /// <param name="pServerName"></param>
    /// <returns></returns>
    [DllImport("wtsapi32.dll", SetLastError = true)]
    public static extern IntPtr WTSOpenServer(String pServerName);

    /// <summary>
    /// Windows Terminal Server close server
    /// </summary>
    /// <param name="hServer"></param>
    [DllImport("wtsapi32.dll")]
    public static extern void WTSCloseServer(IntPtr hServer);

    /// <summary>
    /// Windows Terminal Server enumerate through all sessions
    /// </summary>
    /// <param name="hServer">Current terminal server</param>
    /// <param name="Reserved">NULL</param>
    /// <param name="Version">Version</param>
    /// <param name="ppSessionInfo">Session Info pointer</param>
    /// <param name="Count">Count of sessions</param>
    /// <returns></returns>
    [DllImport("wtsapi32.dll", SetLastError = true)]
    public static extern int WTSEnumerateSessions(IntPtr hServer,
                                                  int Reserved,
                                                  int Version,
                                                  ref IntPtr ppSessionInfo,
                                                  ref int Count);

    /// <summary>
    /// Enumerates the sessions removing the Reserved and Version as they are hard coded.
    /// </summary>
    /// <param name="hServer"></param>
    /// <param name="ppSessionInfo"></param>
    /// <param name="pCount"></param>
    /// <returns></returns>
    public static int WTSEnumerateSessions(System.IntPtr hServer,
                                              ref IntPtr ppSessionInfo,
                                              ref int pCount)
    {
      return WTSEnumerateSessions(hServer, (int)0, (int)1, ref ppSessionInfo, ref pCount);
    }


    /// <summary>
    /// Windows Terminal Server frees the memory of objects allocated by other WTS calls
    /// </summary>
    /// <param name="pMemory"></param>
    [DllImport("wtsapi32.dll", CharSet = CharSet.Auto)]
    public static extern void WTSFreeMemory(IntPtr pMemory);
    
    
    /// <summary>
    /// Windows Terminal Server query for the user's security token of the specified session id
    /// </summary>
    /// <param name="sessionId">Session id</param>
    /// <param name="hToken">Returned security token</param>
    /// <returns></returns>
    [DllImport("wtsapi32.dll", CharSet = CharSet.Auto)]
    public static extern bool WTSQueryUserToken(uint sessionId, out IntPtr hToken);


    #endregion

    /// <summary>
    /// Opens a process by the id
    /// </summary>
    /// <param name="dwDesiredAccess">Access</param>
    /// <param name="bInheritHandle">Inherits handle flag</param>
    /// <param name="dwProcessId">Process id</param>
    /// <returns>Process Handle</returns>
    [DllImport("Kernel32", CharSet = CharSet.Auto)]
    public extern static IntPtr OpenProcess(uint dwDesiredAccess, bool bInheritHandle, uint dwProcessId);

    /// <summary>
    /// Opens a process security token
    /// </summary>
    /// <param name="hProcess">Process handle</param>
    /// <param name="dwDesiredAccess">Access</param>
    /// <param name="hToken">Returned token</param>
    /// <returns></returns>
    [DllImport("Advapi32", CharSet = CharSet.Auto)]
    public extern static bool OpenProcessToken(IntPtr hProcess, uint dwDesiredAccess, out IntPtr hToken);

    /// <summary>
    /// Terminates  the specified process
    /// </summary>
    /// <param name="hProcess">Process handle</param>
    /// <param name="uExitCode">Exit code</param>
    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool TerminateProcess(IntPtr hProcess, uint uExitCode);


    /// <summary>
    /// Gets the specified tokens information
    /// </summary>
    /// <param name="TokenHandle"></param>
    /// <param name="TokenInformationClass"></param>
    /// <param name="TokenInformation"></param>
    /// <param name="TokenInformationLength"></param>
    /// <param name="ReturnLength"></param>
    /// <returns></returns>
    [DllImport("advapi32.dll", SetLastError = true)]
    public static extern bool GetTokenInformation(  IntPtr TokenHandle,
                                                    TOKEN_INFORMATION_CLASS TokenInformationClass,
                                                    IntPtr TokenInformation,
                                                    uint TokenInformationLength,
                                                    out uint ReturnLength);

    /// <summary>
    /// Looks up the account SID 
    /// </summary>
    /// <param name="lpSystemName">System name</param>
    /// <param name="Sid">Sid</param>
    /// <param name="lpName">User name</param>
    /// <param name="cchName">Size of user name</param>
    /// <param name="ReferencedDomainName">Domain name</param>
    /// <param name="cchReferencedDomainName">size of domain name</param>
    /// <param name="peUse">SID Name User</param>
    /// <returns></returns>
    [DllImport("advapi32.dll", CharSet=CharSet.Auto, SetLastError = true)]
    public static extern bool LookupAccountSid(string lpSystemName,
                                               IntPtr Sid,
                                               System.Text.StringBuilder lpName,
                                               ref uint cchName,
                                               System.Text.StringBuilder ReferencedDomainName,
                                               ref uint cchReferencedDomainName,
                                               out SID_NAME_USE peUse);        


    
    /// <summary>
    /// Converts the SID to a string.
    /// 
    ///  NOTE: Using IntPtr for pSID instead of Byte[]
    /// </summary>
    /// <param name="pSID"></param>
    /// <param name="ptrSid"></param>
    /// <returns></returns>
    [DllImport("advapi32", CharSet = CharSet.Auto, SetLastError = true)]
    public static extern bool ConvertSidToStringSid( IntPtr pSID, out IntPtr ptrSid);


    /// <summary>
    /// Frees local memory
    /// </summary>
    /// <param name="hMem"></param>
    /// <returns></returns>
    [DllImport("kernel32.dll")]
    public static extern IntPtr LocalFree(IntPtr hMem);

    /// <summary>
    /// Waist for a single object to be signaled
    /// </summary>
    /// <param name="hHandle"></param>
    /// <param name="dwMilliseconds"></param>
    /// <returns></returns>
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds );


  };

}
