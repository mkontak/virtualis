using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise
{

      #region enum

    /// <summary>
    /// Net Join Status
    /// </summary>
    public enum NetJoinStatus
    {
      NetSetupUnknownStatus = 0,
      NetSetupUnjoined,
      NetSetupWorkgroupName,
      NetSetupDomainName
    }

    #endregion

  /// <summary>
  /// Domain Info structure contains the domain/workspace name and the 
  /// status
  /// </summary>
  public struct DomainInfo
  {
    public DomainInfo(string name, NetJoinStatus status)
    { Name = name; Status = status; }

    // Domain or workspace name
    public string Name;

    // Join status
    public NetJoinStatus Status;
  } // struct DomainInfo

  public static class DomainInformation
  {



    #region contants

    // Win32 Result Code Constant  
    const int ErrorSuccess = 0;    

    #endregion


    #region interop

    /// <summary>
    /// Fress the NET Buffers
    /// </summary>
    /// <param name="Buffer"></param>
    /// <returns></returns>
    [DllImport("Netapi32.dll")]
    static extern int NetApiBufferFree(IntPtr Buffer);

    /// <summary>
    /// Gets the Join information
    /// </summary>
    /// <param name="server">Server name</param>
    /// <param name="domain">Dmain information</param>
    /// <param name="status">Join status</param>
    /// <returns></returns>
    [DllImport("Netapi32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern int NetGetJoinInformation(string server, out IntPtr domain, out NetJoinStatus status);

    #endregion



    #region methods



    /// <summary>
    /// Returns the domain information such as the name and the status
    /// </summary>
    /// <returns></returns>
    public static DomainInfo GetDomainInfo()
    {

      int result = 0;
      string domain = null;
      IntPtr pDomain = IntPtr.Zero;
      NetJoinStatus status = NetJoinStatus.NetSetupUnknownStatus;

      try
      {
        // get domain information
        result = NetGetJoinInformation(null, out pDomain, out status);

        // ============================
        // Make sure we did not error
        // ============================
        if (result == ErrorSuccess)
        {
          switch (status)
          {
            case NetJoinStatus.NetSetupDomainName:
            case NetJoinStatus.NetSetupWorkgroupName:
              domain = Marshal.PtrToStringAuto(pDomain);
              break;
          }

        } // END ... If the information was returned
      }
      finally
      {
        //=============================================================
        // Make sure the domain returned is not null before freeing
        //=============================================================
        if (pDomain != IntPtr.Zero)
          NetApiBufferFree(pDomain);
      }

      // If the domain is null return blank string
      if (domain == null)
        domain = "";

      return new DomainInfo(domain, status);

    } // GetDomainName()




    #endregion

  } // class DomainInformation

} // namespace Viatronix.Enterprise