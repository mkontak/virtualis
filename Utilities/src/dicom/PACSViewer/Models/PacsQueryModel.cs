using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Dicom;

namespace Viatronix.Utilities.Dicom.PACSViewer.Models
{
  /// <summary>
  /// The model to connect and search the pacs.
  /// </summary>
  public class PacsQueryModel
  {

    #region events

    /// <summary>
    /// Notifies that the query has started.
    /// </summary>
    public event ClientEventHandler Started;

    /// <summary>
    /// Notifies that progress occured.  ClientEventArgs should contain a dataset definition.
    /// </summary>
    public event ClientEventHandler Progress;

    /// <summary>
    /// Notifies that the process has been completed successfully.
    /// </summary>
    public event ClientEventHandler Completed;

    /// <summary>
    /// Notifies that the process has failed.
    /// </summary>
    public event ClientEventHandler Failed;

    #endregion

    #region methods

    /// <summary>
    /// Performs the query and feeds the upper levels with the events.
    /// </summary>
    /// <param name="connection">The server to connect to.</param>
    /// <param name="parameters">The parameters to pass to the query.</param>
    public void PerformQuery(Viatronix.Dicom.Host server, SearchParametersModel parameters)
    {
      CFind finder = CFind.Create("V3D", server);
      finder.AddParameters(parameters.GetParameters());
      finder.Started   += new ClientEventHandler(OnFindStartedHandler);
      finder.Progress  += new ClientEventHandler(OnFindProgressHandler);
      finder.Completed += new ClientEventHandler(OnFindCompletedHandler);
      finder.Failed    += new ClientEventHandler(OnFindFailedHandler);

      finder.Start();
    } // PerformQuery( connection, parameters )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the finder's started event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFindStartedHandler(object sender, ClientEventArgs args)
    {
      if (Started != null)
      {
        Started(sender, args);
      }
    } // OnFindStartedHandler( sender, args )


    /// <summary>
    /// Handles the finder's progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFindProgressHandler(object sender, ClientEventArgs args)
    {
      if (Progress != null)
      {
        Progress(sender, args);
      }
    } // OnFindProgressHandler( sender, args )


    /// <summary>
    /// Handles the finder's completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFindCompletedHandler(object sender, ClientEventArgs args)
    {
      if (Completed != null)
      {
        Completed(sender, args);
      }
    } // OnFindCompletedHandler( sender, args )


    /// <summary>
    /// Handles the finder's failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFindFailedHandler(object sender, ClientEventArgs args)
    {
      if (Failed != null)
      {
        Failed(sender, args);
      }
    } // OnFindFailedHandler( sender, args )

    #endregion

  } // class PacsQueryModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.Models
