using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using System.Windows.Threading;

using GalaSoft.MvvmLight.Threading;

using Viatronix.Logging;

namespace Viatronix.Utilities.Transfer
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : System.Windows.Application
  {

    #region fields

    /// <summary>
    /// The uid argument.
    /// </summary>
    private static string m_inputUid = string.Empty;

    /// <summary>
    /// The user argument.
    /// </summary>
    private static string m_inputUser = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the uid provided in the arguments.
    /// </summary>
    public static string Uid
    {
      get { return m_inputUid; }
      private set { m_inputUid = value; }
    } // Uid


    /// <summary>
    /// Gets or sets the user parameters provided in the arguments.
    /// </summary>
    public static string User
    {
      get { return m_inputUser; }
      private set { m_inputUser = value; }
    } // User

    #endregion

    #region methods

    /// <summary>
    /// Handles the startup.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnStartup(StartupEventArgs args)
    {
      base.OnStartup(args);

      DispatcherHelper.Initialize();
      Log.Initialize();

      // ------------------------------------------------
      // Handle the login.
      // ===============================================

      for (int i = 0; i < args.Args.Length; ++i)
      {
        if (args.Args[i] == "/user" && i + 1 < args.Args.Length)
        {
          User = args.Args[i + 1];
        }
        else if (args.Args[i] == "/uid" && i + 1 < args.Args.Length)
        {
          Uid = args.Args[i + 1];
        }
      }
    } // OnStartup( args )

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Transfer
