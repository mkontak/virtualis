using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Viatronix.Launching
{
  /// <summary>
  /// Encapsulate information about a Windows Terminal Session including the ID, USER and Users Security Token
  /// </summary>
  public class WindowsSession : IDisposable
  {

    #region fields

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// User token
    /// </summary>
    private IntPtr m_token = IntPtr.Zero;

    /// <summary>
    /// Session id
    /// </summary>
    private uint m_id = 0;

    /// <summary>
    /// Active flag
    /// </summary>
    private bool m_active = false;

    /// <summary>
    /// User name
    /// </summary>
    private string m_username = string.Empty;

    #endregion


    #region constructors/destructors
    protected WindowsSession() { }

    #endregion


    /// <summary>
    /// Disposes the object
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Disposes of the windows session information
    /// </summary>
    /// <param name="disposing"></param>
    protected void Dispose(bool disposing)
    {

      Logging.Log.Debug(string.Format("Disposing of Window Session ({0})", Id), "WindowsSession", "Dispose");

      if (!m_disposed)
      {
        // Close the user token
        Win32.CloseHandle(m_token, "User Access Token");

        m_disposed = true;

      } // END ... If not already disposed
    } // Dispose(bool disposing)

    #region methods

    /// <summary>
    /// Creates a session information object from the Session info
    /// </summary>
    /// <param name="si"></param>
    /// <returns></returns>
    public static WindowsSession Create(Win32.WTS_SESSION_INFO si)
    {
      WindowsSession info = new WindowsSession();

      if (info == null)
        throw new OutOfMemoryException("Failed to allocate WindowsSession object");

      info.m_id = si.SessionId;
      info.m_active = (si.State == Win32.WTS_CONNECTSTATE_CLASS.WTSActive);

      IntPtr hToken = IntPtr.Zero;

      // ===========================================================
      // We need to query the session to get the user  token
      // ==========================================================
      if (!Win32.WTSQueryUserToken(si.SessionId, out hToken))
        throw new LaunchException(string.Format("Failed to query user token [{0}]!", Marshal.GetLastWin32Error()));

      info.m_token = hToken;


      // ===============================================================
      // We now get the users SID associated with the access token 
      // ==============================================================

      uint TokenInfLength = 0;
      bool result;

      // First call gets length of TokenInformation
      result = Win32.GetTokenInformation(hToken, Win32.TOKEN_INFORMATION_CLASS.TokenUser, IntPtr.Zero, TokenInfLength, out TokenInfLength);

      int errorCode = Marshal.GetLastWin32Error();

      // ===============================================================================================================================
      // Make sure the first pass did not error without and INSUFFICENT BUFFER ERROR which is what we expect. This call just gets the 
      // required buffer size.
      // ================================================================================================================================
      if (result == false && errorCode != Win32.ERROR_INSUFFICIENT_BUFFER)
        throw new LaunchException(string.Format("Failed to get token information (first call), error {0}", errorCode));

      // Allocate the 
      IntPtr TokenInformation = Marshal.AllocHGlobal((int)TokenInfLength);

      // Next call get the actual token information
      if (Win32.GetTokenInformation(hToken, Win32.TOKEN_INFORMATION_CLASS.TokenUser, TokenInformation, TokenInfLength, out TokenInfLength))
      {

        // Remaps the memory to a TOEKN_USER
        Win32.TOKEN_USER tokenUser = (Win32.TOKEN_USER)Marshal.PtrToStructure(TokenInformation, typeof(Win32.TOKEN_USER));

        // User name
        StringBuilder name = new StringBuilder(64);
        uint cchName = (uint)name.Capacity;

        // Domain name
        StringBuilder referencedDomainName = new StringBuilder(64);
        uint cchReferencedDomainName = (uint)referencedDomainName.Capacity;

        // Name identifier enum
        Win32.SID_NAME_USE sidUse;

        // ==========================================================================================================================================
        // Initial lookup will attempt to get the user name associated with the SID, but if the 64 characters is not enough then the the amount of
        // memory is returned and I change the StringBuilder's capacity to account for the required memory.
        // =========================================================================================================================================
        if (!Win32.LookupAccountSid(null, tokenUser.User.Sid, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
        {

          // ==================================================================================
          // Get the last error so that I can check if we received INSUFFICIENT BUFFER error
          // ==================================================================================
          int lastError = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
          if (lastError == Win32.ERROR_INSUFFICIENT_BUFFER)
          {
            name.EnsureCapacity((int)cchName);
            referencedDomainName.EnsureCapacity((int)cchReferencedDomainName);
            lastError = Win32.NO_ERROR;
            if (!Win32.LookupAccountSid(null, tokenUser.User.Sid, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
              lastError = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
          }

          if (lastError != Win32.NO_ERROR)
            throw new LaunchException(String.Format("Failed account lookup,  error {0}", lastError));

        }

        info.m_username = name.ToString();

      }
      else
        throw new LaunchException(String.Format("Failed to get token information (second call), error {0}", Marshal.GetLastWin32Error()));



      return info;
    } // Create()

 
    /// <summary>
    /// Determines if the session is valid based on the user. If the user is blank then we only
    /// check if the session is active. Otherwise we check the user.
    /// </summary>
    /// <param name="users">List of valid user session that can be used</param>
    /// <returns></returns>
    public bool CanUse(List<string> users )
    {
      return ( m_active && ( users.Count == 0 || users.Contains(m_username, true)));
    }


    #endregion


    #region properties

    /// <summary>
    /// Gets the active flag
    /// </summary>
    public bool IsActive
    {
      get { return m_active;  }
    }

    /// <summary>
    /// Gets the session id
    /// </summary>
    public uint Id
    {
      get { return m_id; }
    }

    /// <summary>
    /// Gets the user toekn associated with this session
    /// </summary>
    public IntPtr UserToken
    {
      get { return m_token; }
    }

    /// <summary>
    /// Gets the user associated with this session
    /// </summary>
    public string UserName
    {
      get { return m_username; }
    }

    #endregion

  }; // class WindowsSession
}
