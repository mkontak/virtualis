using System;
using GalaSoft.MvvmLight;
using DicomRIAService.Web.Models;

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
  public class ProgressViewModel : ViewModelBase
  {

    #region fields

    private Progress m_progress = null;

    private bool m_selected = false;

    #endregion

    #region construction

    /// <summary>
    /// Initializes a new instance of the DatasetViewModel class.
    /// </summary>
    public ProgressViewModel( Progress progress )
    {
      m_progress = progress;
      ////if (IsInDesignMode)
      ////{
      ////    // Code runs in Blend --> create design time data.
      ////}
      ////else
      ////{
      ////    // Code runs "for real": Connect to service, etc...
      ////}
    }

    ////public override void Cleanup()
    ////{
    ////    // Clean own resources if needed

    ////    base.Cleanup();
    ////}

    #endregion

    #region properties


    public Progress Entity
    {
      get { return m_progress; }
    }


    public string UID
    {
      get { return m_progress.uid; }
      set 
      {
        if( !string.Equals( m_progress.uid, value ))
        {
          m_progress.uid = value;
          RaisePropertyChanged( "UID" );
        }
      }
    }


    public string AETitle
    {
      get { return m_progress.aetitle; }
      set 
      {
        if( !string.Equals( m_progress.aetitle, value ))
        {
          m_progress.aetitle = value;
          RaisePropertyChanged( "AETitle" );
        }
      }
    }


    public int Port
    {
      get { return m_progress.port; }
      set 
      {
        if( m_progress.port != value )
        {
          m_progress.port = value;
          RaisePropertyChanged( "Port" );
        }
      }
    }


    public int Progress
    {
      get { return m_progress.progress1; }
      set 
      {
        if( m_progress.progress1 != value )
        {
          m_progress.progress1 = value;
          RaisePropertyChanged( "Progress" );
        }
      }
    }


    public int Total
    {
      get { return m_progress.total; }
      set 
      {
        if( m_progress.total != value )
        {
          m_progress.total = value;
          RaisePropertyChanged( "Total" );
        }
      }
    }


    public string Server
    {
      get { return m_progress.server; }
      set 
      {
        if( !string.Equals( m_progress.server, value ))
        {
          m_progress.server = value;
          RaisePropertyChanged( "Server" );
        }
      }
    }


    public string Message
    {
      get { return m_progress.message; }
      set 
      {
        if( !string.Equals( m_progress.message, value ))
        {
          m_progress.message = value;
          RaisePropertyChanged( "Message" );
        }
      }
    }


    public string Name
    {
      get { return m_progress.name; }
      set 
      {
        if( !string.Equals( m_progress.name, value ))
        {
          m_progress.name = value;
          RaisePropertyChanged( "Name" );
        }
      }
    }


    public DateTime Date
    {
      get { return m_progress.date; }
      set 
      {
        if( !DateTime.Equals( m_progress.date, value ))
        {
          m_progress.date = value;
          RaisePropertyChanged( "Date" );
        }
      }
    }


    public string State
    {
      get { return m_progress.state; }
      set 
      {
        if( !string.Equals( m_progress.state, value ))
        {
          m_progress.state = value;
          RaisePropertyChanged( "State" );
        }
      }
    }


    public bool IsSelected
    {
      get { return m_selected; }
      set 
      {
        if( m_selected != value )
        {
          m_selected = value;
          RaisePropertyChanged( "IsSelected" );
        }
      }
    }

    #endregion
    
  }
}