using System.Linq;
using System.Windows.Data;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using DicomRIAService.Web.Services;

namespace DicomRIAService.ViewModels
{
  /// <summary>
  /// This class contains properties that a View can data bind to.
  /// <para>
  /// Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel.
  /// </para>
  /// <para>
  /// You can also use Blend to data bind with the tool's support.
  /// </para>
  /// <para>
  /// See http://www.galasoft.ch/mvvm/getstarted
  /// </para>
  /// </summary>
  public class SendViewModel : ViewModelBase
  {

    #region fields

    private DatasetViewModel m_dataset = null;

    private ProgressViewModel m_progress = null;

    #endregion

    #region construction

    /// <summary>
    /// Initializes a new instance of the SendDatasetViewModel class.
    /// </summary>
    public SendViewModel( DatasetViewModel dataset, ProgressViewModel progress )
    {
      m_dataset = dataset;
      m_progress = progress;
    }

    ////public override void Cleanup()
    ////{
    ////    // Clean own resources if needed

    ////    base.Cleanup();
    ////}

    #endregion

    #region properties

    public DatasetViewModel Dataset
    {
      get { return m_dataset; }
    }


    public ProgressViewModel Progress
    {
      get { return m_progress; }
    }


    #endregion

  }
}