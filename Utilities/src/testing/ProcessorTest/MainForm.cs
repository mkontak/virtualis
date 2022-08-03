using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.Utilities.Tests.ProcessorTest
{
  /// <summary>
  /// The main form into the test.
  /// </summary>
  public partial class MainForm : Form
  {

    #region fields

    /// <summary>
    /// The process that will execute.
    /// </summary>
    Process m_process = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm()
    {
      InitializeComponent();

      m_process = new Process();
      m_process.StartInfo.FileName = "Processor\\Processor.exe";
      m_process.StartInfo.CreateNoWindow = true;
      m_process.StartInfo.UseShellExecute = false;
      m_process.StartInfo.RedirectStandardInput = true;
      m_process.StartInfo.RedirectStandardError = true;
      m_process.ErrorDataReceived += new DataReceivedEventHandler(OnErrorDataReceivedHandler);
      m_process.Exited += new EventHandler(OnProcessExitedHandler);
    } // MainForm()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the start button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStartClickHandler(object sender, EventArgs args)
    {
      m_startButton.Enabled = false;
      m_stopButton.Enabled = true;
      m_process.Start();

      m_process.StandardInput.WriteLine(m_taskText.Text);
      m_process.StandardInput.WriteLine(m_sourceText.Text);
      m_process.StandardInput.WriteLine(m_seriesText.Text);
    } // OnStartClickHandler( sender, args )


    /// <summary>
    /// Handles the stop button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStopClickHandler(object sender, EventArgs args)
    {
      m_process.StandardInput.WriteLine("exit");
    } // OnStopClickHandler( sender, args )


    /// <summary>
    /// Handles an error coming from the process.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnErrorDataReceivedHandler(object sender, DataReceivedEventArgs args)
    {
      if (args.Data != string.Empty)
      {
        MessageBox.Show(args.Data, "Error Received From Process");
      }
    } // OnErrorDataReceivedHandler( sender, args )


    /// <summary>
    /// Handles the process's Exited event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnProcessExitedHandler(object sender, EventArgs args)
    {
      m_startButton.Enabled = true;
      m_stopButton.Enabled = false;
    } // OnProcessExitedHandler( sender, args )

    #endregion

  } // class MainForm
} // namespace Viatronix.Utilities.Tests.ProcessorTest
