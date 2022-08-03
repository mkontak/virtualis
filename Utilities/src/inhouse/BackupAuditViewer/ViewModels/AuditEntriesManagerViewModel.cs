// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.IO;
using System.Data;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;
using System.Linq;
using System.Text;

using Excel = Microsoft.Office.Interop.Excel;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

namespace Viatronix.Utilities.BackupAuditViewer.ViewModels
{
  /// <summary>
  /// A manager for the audit entries.
  /// </summary>
  public class AuditEntriesManagerViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The server that we are connecting to.
    /// </summary>
    private string m_connectedServer = string.Empty;

    /// <summary>
    /// List of audit entries that conform to the search specifications.
    /// </summary>
    private ObservableCollection<AuditEntryViewModel> m_entries = new ObservableCollection<AuditEntryViewModel>();

    /// <summary>
    /// The start date.
    /// </summary>
    private DateTime m_fromDate = new DateTime(2000, 1, 1);

    /// <summary>
    /// The end date.
    /// </summary>
    private DateTime m_toDate = new DateTime(2099, 12, 31);

    /// <summary>
    /// The type that we are looking for.
    /// </summary>
    private string m_typeQuery = string.Empty;

    /// <summary>
    /// The system that we are looking for.
    /// </summary>
    private string m_systemQuery = string.Empty;

    /// <summary>
    /// The month to purge.
    /// </summary>
    private int m_purgeMonth = 0;

    /// <summary>
    /// The corresponding year.
    /// </summary>
    private string m_purgeYear = "2000";

    /// <summary>
    /// Search terms for the type.
    /// </summary>
    private ObservableCollection<string> m_typeSearchTerms = (ObservableCollection<string>)System.Configuration.ConfigurationManager.GetSection("types");

    /// <summary>
    /// Search terms for the system.
    /// </summary>
    private ObservableCollection<string> m_systemSearchTerms = (ObservableCollection<string>)System.Configuration.ConfigurationManager.GetSection("systems");

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public AuditEntriesManagerViewModel()
    {
      SearchCommand = new RelayCommand(() => Search(), () => true);
      ReportCommand = new RelayCommand(() => CreateReport(), () => true);
      PurgeCommand = new RelayCommand(() => Purge(), () => true);
      SearchPurgeCommand = new RelayCommand(() => SearchPurge(), () => true);

      Database db = DatabaseFactory.CreateDatabase("Auditing");
      ConnectedServer = db.ConnectionStringWithoutCredentials.Split(';')[0].Remove(0, 7);
    } // AuditEntriesManagerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the server we're connected to.
    /// </summary>
    public string ConnectedServer
    {
      get { return m_connectedServer; }
      set
      {
        m_connectedServer = value;
        RaisePropertyChanged("ConnectedServer");
      }
    } // ConnectedServer


    /// <summary>
    /// Gets or sets the entries.
    /// </summary>
    public ObservableCollection<AuditEntryViewModel> Entries
    {
      get { return m_entries; }
      set 
      { 
        m_entries = value;
        RaisePropertyChanged("Entries");
      }
    } // Entries


    /// <summary>
    /// Gets or sets the from date.
    /// </summary>
    public DateTime FromDate
    {
      get { return m_fromDate; }
      set
      {
        m_fromDate = value;
        RaisePropertyChanged("FromDate");
      }
    } // FromDate


    /// <summary>
    /// Gets or sets the to date.
    /// </summary>
    public DateTime ToDate
    {
      get { return m_toDate; }
      set
      {
        m_toDate = value;
        RaisePropertyChanged("ToDate");
      }
    } // ToDate


    /// <summary>
    /// Gets or sets the type query.
    /// </summary>
    public string TypeQuery
    {
      get { return m_typeQuery; }
      set
      {
        m_typeQuery = value;
        RaisePropertyChanged("TypeQuery");
      }
    } // TypeQuery


    /// <summary>
    /// Gets or sets the system query.
    /// </summary>
    public string SystemQuery
    {
      get { return m_systemQuery; }
      set
      {
        m_systemQuery = value;
        RaisePropertyChanged("SystemQuery");
      }
    } // SystemQuery


    /// <summary>
    /// Gets or sets the purge month.
    /// </summary>
    public int PurgeMonth
    {
      get { return m_purgeMonth; }
      set
      {
        m_purgeMonth = value;
        RaisePropertyChanged("PurgeMonth");
      }
    } // PurgeMonth


    /// <summary>
    /// Gets or sets the purge year.
    /// </summary>
    public string PurgeYear
    {
      get { return m_purgeYear; }
      set
      {
        m_purgeYear = value;
        RaisePropertyChanged("PurgeYear");
      }
    } // PurgeYear


    /// <summary>
    /// Gets or sets defalult terms for the type.
    /// </summary>
    public ObservableCollection<string> TypeSearchTerms
    {
      get { return m_typeSearchTerms; }
      set
      {
        m_typeSearchTerms = value;
        RaisePropertyChanged("TypeSearchTerms");
      }
    } // TypeSearchTerms


    /// <summary>
    /// Gets or sets defalult terms for the system.
    /// </summary>
    public ObservableCollection<string> SystemSearchTerms
    {
      get { return m_systemSearchTerms; }
      set
      {
        m_systemSearchTerms = value;
        RaisePropertyChanged("SystemSearchTerms");
      }
    } // SystemSearchTerms

    #endregion

    #region commands

    /// <summary>
    /// Command to retrieve results from the database and filter out by the search parameters.
    /// </summary>
    public RelayCommand SearchCommand
    {
      get;
      set;
    } // SearchCommand


    /// <summary>
    /// Command to write out a report to the specified file.
    /// </summary>
    public RelayCommand ReportCommand
    {
      get;
      set;
    } // GenerateReportCommand

    
    /// <summary>
    /// Comand to wipe out the database table's records.
    /// </summary>
    public RelayCommand PurgeCommand
    {
      get;
      set;
    } // PurgeCommand


    /// <summary>
    /// Command to search the purge query.
    /// </summary>
    public RelayCommand SearchPurgeCommand
    {
      get;
      set;
    } // SearchPurgeCommand

    #endregion

    #region methods

    /// <summary>
    /// Performs the search and updates the gui.
    /// </summary>
    private void Search()
    {
      Entries.Clear();
      List<AuditEntryViewModel> entries = MatchesSearchQuery();

      foreach(AuditEntryViewModel entry in entries)
      {
        Entries.Add(entry);
      }
    } // Search()


    /// <summary>
    /// Creates the report.
    /// </summary>
    private void CreateReport()
    {
      Search();

      GenerateReport(Entries.ToList());
    } // CreateReport()


    /// <summary>
    /// Purges the database.
    /// </summary>
    public void Purge()
    {
      int year = -1;
      try
      {
        year = Convert.ToInt32(PurgeYear);
      }
      catch (Exception)
      {
        MessageBox.Show("A valid year for purging must be entered.");
        return;
      }

      List<AuditEntryViewModel> purging = MatchesPurgeQuery(PurgeMonth + 1, year);
      if (purging.Count == 0)
      {
        MessageBox.Show("No entries exist at the selected month and year to be removed.");
        return;
      }

      MessageBoxResult result = MessageBox.Show(string.Format("This will delete all {0} entries in the search criteria.\n                     First we'll save a report.\n          Are you sure you want to continue?", purging.Count), "Purge Database", System.Windows.MessageBoxButton.YesNo, MessageBoxImage.Warning, MessageBoxResult.No);
      if (result == MessageBoxResult.Yes)
      {
        if (!GenerateReport(purging, string.Format("{0}{1}Report.xlsx", MonthAsString(PurgeMonth + 1), year)))
        {
          MessageBox.Show("Saving report was cancelled.  Purge will not be performed.");
          return;
        }

        try
        {
          Database database = DatabaseFactory.CreateDatabase("Auditing");
          using (DbConnection conn = database.CreateConnection())
          {
            foreach (AuditEntryViewModel entry in purging)
            {
              DbCommand cmd = database.GetSqlStringCommand(string.Format("DELETE FROM [AuditLog] WHERE [AuditLog].[type] = \'{0}\'" +
                "AND [AuditLog].[subType] = \'{1}\' AND [AuditLog].[server] = \'{2}\' AND [AuditLog].[system] = \'{3}\' AND [AuditLog].[source] = \'{4}\'" +
                "AND [AuditLog].[started] = \'{5}\' AND [AuditLog].[completed] = \'{6}\' AND [AuditLog].[time] = \'{7}\' AND [AuditLog].[file] = \'{8}\'" +
                "AND [AuditLog].[message] = \'{9}\'", entry.Type, entry.SubType, entry.Server, entry.System, entry.Source, entry.Started, entry.Completed,
                entry.Time, entry.File, entry.Message));

              database.ExecuteNonQuery(cmd);
            }
          }

          Entries.Clear();
        }
        catch (Exception e)
        {
          MessageBox.Show(string.Format("An error occured purging the selected results from the database.  Error: {0}", e.Message));
        }
      }
    } // Purge()


    /// <summary>
    /// Searches with the purge parameters.
    /// </summary>
    public void SearchPurge()
    {
      Entries.Clear();

      int year = -1;
      try
      {
        year = Convert.ToInt32(PurgeYear);
      }
      catch (Exception)
      {
        MessageBox.Show("A valid year for searching must be entered.");
        return;
      }

      List<AuditEntryViewModel> purging = MatchesPurgeQuery((PurgeMonth + 1), year);
      foreach (AuditEntryViewModel entry in purging)
      {
        Entries.Add(entry);
      }
    } // SearchPurge()


    /// <summary>
    /// Retrieves the entries from the database.
    /// </summary>
    public List<AuditEntryViewModel> GetDatabaseEntries()
    {
      List<AuditEntryViewModel> entries = new List<AuditEntryViewModel>();

      try
      {
        Database database = DatabaseFactory.CreateDatabase("Auditing");
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM AuditLog ORDER BY [AuditLog].[started]");
          DataSet ds = database.ExecuteDataSet(cmd);

          if (ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
          {
            foreach (DataRow row in ds.Tables[0].Rows)
            {
              AuditEntryViewModel viewModel = new AuditEntryViewModel();

              viewModel.Type = row["type"].ToString();
              viewModel.SubType = row["subtype"].ToString();
              viewModel.Server = row["server"].ToString();
              viewModel.System = row["system"].ToString();
              viewModel.Source = row["source"].ToString();
              viewModel.Started = ((DateTimeOffset)row["started"]).ToLocalTime();
              viewModel.Completed = ((DateTimeOffset)row["completed"]).ToLocalTime();
              viewModel.Time = row["time"].ToString();
              viewModel.File = row["file"].ToString();
              viewModel.Status = row["status"].ToString();
              viewModel.Message = row["message"].ToString();

              entries.Add(viewModel);
            }
          }
        }
      }
      catch (Exception e)
      {
        MessageBox.Show(string.Format("Cannot access the database.  Error: {0}", e.Message));
      }

      return entries;
    } // GetDatabaseEntries()


    /// <summary>
    /// Generates a report.
    /// </summary>
    /// <param name="entries">The list of entries to write out to the report.</param>
    private void GenerateReport(List<AuditEntryViewModel> entries)
    {
      GenerateReport(entries, string.Empty);
    } // GenerateReport( entries )


    /// <summary>
    /// Generates the report.
    /// </summary>
    /// <param name="entries">The list of entries to write out to the report.</param>
    /// <param name="defaultFile">The file to use as a default.  If "" or null this step is ignored.</param>
    private bool GenerateReport(List<AuditEntryViewModel> entries, string defaultFile)
    {
      Microsoft.Win32.SaveFileDialog dialog = new Microsoft.Win32.SaveFileDialog();
      dialog.Filter = "Excel Spreadsheet (*.xlsx)|*.xlsx";

      if (!string.IsNullOrEmpty(defaultFile))
      {
        dialog.FileName = defaultFile;
      }

      bool notCancelled = dialog.ShowDialog() == true ? true : false;
      if (notCancelled)
      {
        Excel.Application app = null;
        Excel.Workbook workbook = null;
        Excel.Worksheet worksheet = null;

        try
        {
          if (File.Exists(dialog.FileName))
          {
            File.Delete(dialog.FileName);
          }

          // -------------------------------------------------------------------------------------------------
          // Uses the office interop to access excel and write out the report.
          // -------------------------------------------------------------------------------------------------

          app = new Excel.Application();
          workbook = app.Workbooks.Add();
          worksheet = (Excel.Worksheet)workbook.Worksheets.get_Item(1);

          worksheet.Cells[1, 1] = "type";
          worksheet.Cells[1, 2] = "subtype";
          worksheet.Cells[1, 3] = "server";
          worksheet.Cells[1, 4] = "system";
          worksheet.Cells[1, 5] = "source";
          worksheet.Cells[1, 6] = "started";
          worksheet.Cells[1, 7] = "completed";
          worksheet.Cells[1, 8] = "file";
          worksheet.Cells[1, 9] = "message";

          for (int i = 0; i < entries.Count; ++i)
          {
            worksheet.Cells[i + 2, 1] = entries[i].Type;
            worksheet.Cells[i + 2, 2] = entries[i].SubType;
            worksheet.Cells[i + 2, 3] = entries[i].Server;
            worksheet.Cells[i + 2, 4] = entries[i].System;
            worksheet.Cells[i + 2, 5] = entries[i].Source;
            worksheet.Cells[i + 2, 6] = entries[i].Started.ToString();
            worksheet.Cells[i + 2, 7] = entries[i].Completed.ToString();
            worksheet.Cells[i + 2, 8] = entries[i].File;
            worksheet.Cells[i + 2, 9] = entries[i].Message;
          }

          workbook.SaveAs(dialog.FileName);
          workbook.Close();
          app.Quit();
        }
        catch (Exception e)
        {
          MessageBox.Show(string.Format("An error occurred while writting out the report to Microsoft Excel.  Error: ", e.Message));
        }
        finally
        {
          // Remove our objects.
          if (worksheet != null)
          {
            ReleaseObject(worksheet);
          }

          if (workbook != null)
          {
            ReleaseObject(workbook);
          }

          if (app != null)
          {
            ReleaseObject(app);
          }
        }
      }

      return notCancelled;
    } // GenerateReport( entries )


    /// <summary>
    /// Determines if the entry matches the search terms.
    /// </summary>
    /// <param name="viewModel">The entry we're checking.</param>
    /// <returns>Whether or not the entry should be part of the viewable collection.</returns>
    private List<AuditEntryViewModel> MatchesSearchQuery()
    {
      List<AuditEntryViewModel> retrival = new List<AuditEntryViewModel>();
      List<AuditEntryViewModel> entries = GetDatabaseEntries();

      foreach (AuditEntryViewModel entry in entries)
      {
        if (((FromDate < entry.Started && entry.Started < ToDate) || (FromDate < entry.Completed && entry.Completed < ToDate)) &&
          (TypeQuery == string.Empty || (TypeQuery != string.Empty && entry.Type == TypeQuery)) &&
          (SystemQuery == string.Empty || (SystemQuery != string.Empty && entry.System == SystemQuery)))
        {
          retrival.Add(entry);
        }
      }

      return retrival;
    } // MatchesQuery( viewModel )


    /// <summary>
    /// Returns the entries that match the purge query.
    /// </summary>
    /// <param name="month">The month to retrieve.</param>
    /// <param name="year">The year to retrieve.</param>
    /// <returns>Entries that conform to the month/year pair.</returns>
    private List<AuditEntryViewModel> MatchesPurgeQuery(int month, int year)
    {
      List<AuditEntryViewModel> purging = new List<AuditEntryViewModel>();
      List<AuditEntryViewModel> entries = GetDatabaseEntries();

      foreach (AuditEntryViewModel entry in entries)
      {
        if (entry.Started.Month == month && entry.Started.Year == year)
        {
          purging.Add(entry);
        }
      }

      return purging;
    } // MatchesPurgeQuery( month, year )


    /// <summary>
    /// Releases a COM Object.
    /// </summary>
    /// <param name="obj"></param>
    private void ReleaseObject(object obj)
    {
      try
      {
        System.Runtime.InteropServices.Marshal.ReleaseComObject(obj);
        obj = null;
      }
      catch (Exception ex)
      {
        obj = null;
        MessageBox.Show("Exception Occured while releasing object " + ex.ToString());
      }
      finally
      {
        GC.Collect();
      }
    } // ReleaseObject( obj )


    /// <summary>
    /// Converts the int to a string.  Months of the year.
    /// </summary>
    /// <param name="month">The month from 1 to 12.</param>
    /// <returns>The month as a string.</returns>
    private string MonthAsString(int month)
    {
      switch (month)
      {
        case 1: return "January";
        case 2: return "February";
        case 3: return "March";
        case 4: return "April";
        case 5: return "May";
        case 6: return "June";
        case 7: return "July";
        case 8: return "August";
        case 9: return "September";
        case 10: return "October";
        case 11: return "November";
        case 12: return "December";
        default: return "Undefined";
      }
    } // MonthAsString( month )

    #endregion

  } // class AuditEntriesManagerViewModel
} // namespace Viatronix.Utilities.BackupAuditViewer.ViewModels
