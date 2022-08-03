using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Viatronix.Database.Sql
{

  class Win32
  {
    [DllImport("advapi32.dll", EntryPoint = "LogonUser")]
    public static extern bool LogonUser(string lpszUserName, string lpszDomain, string lpszPassword, uint dwLogonType, uint dwLogonProvider, out IntPtr pHToken);

    [DllImport("Netapi32.dll", CharSet=CharSet.Unicode, SetLastError=true)] 
    public static extern int NetGetJoinInformation(string server, out IntPtr domain, out NetJoinStatus status); 
 
    [DllImport("Netapi32.dll")] 
    public static extern int NetApiBufferFree(IntPtr Buffer);

    [DllImport("kernel32", CharSet = CharSet.Auto, SetLastError = true, ExactSpelling = true)]
    public static extern int CloseHandle(IntPtr handle);

    /// <summary>
    /// Logon Types
    /// </summary>
    public enum LogonTypes
    {
      Interactive = 2,
      Network,
      Batch,
      Service,
      NetworkCleartext = 8,
      NewCredentials
    };

    /// <summary>
    /// LogonProviders
    /// </summary>
    public enum LogonProviders
    {
      Default = 0, // default for platform (use this!)
      WinNT35,     // sends smoke signals to authority
      WinNT40,     // uses NTLM
      WinNT50      // negotiates Kerb or NTLM
    };

    /// <summary>
    /// Net join status
    /// </summary>
    public enum NetJoinStatus
    {
      Unknown = 0,          // The status is unknown
      Unjoined,             // The computer is not joined
      WorkgroupName,        // The computer is joined to a workgroup
      DomainName            // The computer is joined to a domain
    }; 



  } // class Win32
}
