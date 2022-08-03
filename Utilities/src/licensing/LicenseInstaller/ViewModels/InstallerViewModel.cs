using System;
using System.IO;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Windows.Forms;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Logging;
using Viatronix.Licensing;

namespace Viatronix.Utilities.Licensing.LicenseInstaller.ViewModels
{
  /// <summary>
  /// The view model for the installer.
  /// </summary>
  public class InstallerViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The filename containing the license instructions.
    /// </summary>
    private string m_licenseFilename = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public InstallerViewModel()
    {
      DisplayLicenseFileBrowserCommand = new RelayCommand(DisplayLicenseFileBrowser);
      PerformInstallationCommand = new RelayCommand(PerformInstallation);
    } // InstallerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the license filename.
    /// </summary>
    public string LicenseFilename
    {
      get { return m_licenseFilename; }
      set
      {
        m_licenseFilename = value;
        RaisePropertyChanged("LicenseFilename");
      }
    } // LicenseFilename

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the command.
    /// </summary>
    public RelayCommand DisplayLicenseFileBrowserCommand
    {
      get;
      private set;
    } // DisplayLicenseFileBrowserCommand


    /// <summary>
    /// Gets or sets the command.
    /// </summary>
    public RelayCommand PerformInstallationCommand
    {
      get;
      private set;
    } // PerformInstallationCommand

    #endregion

    #region methods

    /// <summary>
    /// Browse for the license file path.
    /// </summary>
    private void DisplayLicenseFileBrowser()
    {
      OpenFileDialog dialog = new OpenFileDialog();
      dialog.DefaultExt = "";
      dialog.Multiselect = false;

      if (dialog.ShowDialog() == DialogResult.OK)
      {
        LicenseFilename = dialog.FileName;
      }
    } // DisplayLicenseFileBrowser()


    /// <summary>
    /// Performs the installation.
    /// </summary>
    private void PerformInstallation()
    {
      LicenseFileFormatter formatter = null;
      if (!string.IsNullOrEmpty(LicenseFilename))
      {
        try
        {
          using(TextReader reader = new StreamReader(LicenseFilename))
          {
            formatter = LicenseFileFormatter.ReadFormattedString(Viatronix.Encryption.Crypto.Decrypt(reader.ReadLine()));
          }
        }
        catch (Exception ex)
        {
          Log.Error(string.Format("An error occurred reading the license file.  [ERROR=\"{0}\"]", ex.Message), "InstallerViewModel", "PerformInstallation");
          MessageBox.Show(Messages.GetMessage("VX001"), Messages.GetMessage("VX002"), MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }

        try
        {
          UpdateSystem(formatter.SystemId, formatter.TransactionId, formatter.ShouldResetSystemTimeToLiveDate, formatter.SystemTimeToLiveDate, formatter.ShouldInstallLicenses, formatter.NumberLicenses, formatter.DateIssued, formatter.IssuedBy, formatter.LicenseDuration, formatter.LicenseType, formatter.Comments);

          MessageBox.Show(Messages.GetMessage("VX003"), Messages.GetMessage("VX004"), MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        catch (SystemIdMismatchException e)
        {
          Log.Error(string.Format("The system id specified in the license file does not match the installed value in the database.  [ERROR=\"{0}\"]", e.Message), "InstallerViewModel", "PerformInstallation");
          MessageBox.Show(Messages.GetMessage("VX009"), Messages.GetMessage("VX010"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        catch (InstallationAlreadyPerformedException e)
        {
          Log.Error(string.Format("This license has already been installed on this system.  [ERROR=\"{0}\"]", e.Message), "InstallerViewModel", "PerformInstallation");
          MessageBox.Show(Messages.GetMessage("VX011"), Messages.GetMessage("VX012"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        catch (Exception e)
        {
          Log.Error(string.Format("Failed to perform the installation.  [ERROR=\"{0}\"]", e.Message), "InstallerViewModel", "PerformInstallation");
          MessageBox.Show(Messages.GetMessage("VX005"), Messages.GetMessage("VX006"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
      }
      else
      {
        // License file path was not specified.
        MessageBox.Show(Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    } // PerformInstallation()


    /// <summary>
    /// Performs the update of the system.
    /// </summary>
    /// <param name="systemId">The system id.</param>
    /// <param name="transactionId">The transaction id.</param>
    /// <param name="shouldResetSystemTTL">Should we reset the system ttl?</param>
    /// <param name="systemTTLDate">The system time-to-live date.</param>
    /// <param name="shouldInstallLicenses">Should we install licenses?</param>
    /// <param name="licenseCount">The number of licenses to install.</param>
    /// <param name="dateIssued">The date issued.</param>
    /// <param name="issuedBy">Who's issuing the licenses.</param>
    /// <param name="licenseTTL">The license time to live.</param>
    /// <param name="licenseType">The license type.</param>
    /// <param name="message">The message stating comments included in the license file.</param>
    private void UpdateSystem(string systemId, Guid transactionId, bool shouldResetSystemTTL, DateTime systemTTLDate, bool shouldInstallLicenses, int licenseCount, DateTime dateIssued, string issuedBy, int licenseTTL, string licenseType, string message)
    {
      try
      {
        Database database = new SqlDatabase(Viatronix.Encryption.Crypto.Decrypt(ConfigurationManager.ConnectionStrings["System"].ConnectionString));
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetStoredProcCommand("[System].[v3d].[sp_updateSystem]");

          database.AddInParameter(cmd, "@ID", DbType.AnsiString, transactionId.ToString());
          database.AddInParameter(cmd, "@SYSID", DbType.AnsiString, systemId);

          if (shouldResetSystemTTL)
          {
            database.AddInParameter(cmd, "@ENABLED", DbType.Boolean, SqlBoolean.True);
          }
          else
          {
            database.AddInParameter(cmd, "@ENABLED", DbType.Boolean, null);
          }

          if (shouldResetSystemTTL)
          {
            database.AddInParameter(cmd, "@EXP", DbType.DateTimeOffset, systemTTLDate);
          }
          else
          {
            database.AddInParameter(cmd, "@EXP", DbType.DateTimeOffset, null);
          }

          database.AddInParameter(cmd, "@COUNT", DbType.Int32, shouldInstallLicenses ? licenseCount : 0);
          database.AddInParameter(cmd, "@ISSUED", DbType.DateTimeOffset, dateIssued);
          database.AddInParameter(cmd, "@BY", DbType.AnsiString, issuedBy);
          database.AddInParameter(cmd, "@TTL", DbType.Int32, licenseTTL);
          database.AddInParameter(cmd, "@TYPE", DbType.AnsiString, licenseType);
          database.AddInParameter(cmd, "@MESSAGE", DbType.AnsiString, message);

          database.ExecuteNonQuery(cmd);
        }
      }
      catch (Exception e)
      {
        if (e.Message.StartsWith("VX"))
        {
          int errorCode = Int32.Parse(e.Message.Substring(2, 4));

          switch (errorCode)
          {
            case 9006:
              throw new InstallationAlreadyPerformedException("This license file has already been installed on this system.");
            case 9001:
              throw new SystemIdMismatchException("The specified license file is invalid for this system.");
          }
        }

        throw;
      }
    } // UpdateSystem( systemId, transactionId, systemTTLDate, licenseCount, dateIssued, issuedBy, licenseTTL, licenseType, message )

    #endregion

  } // class InstallerViewModel
} // namespace Viatronix.Utilities.Licensing.LicenseInstaller.ViewModels
