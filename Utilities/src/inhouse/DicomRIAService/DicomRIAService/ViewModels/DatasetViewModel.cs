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
  public class DatasetViewModel : ViewModelBase
  {

    #region fields

    private Dataset m_dataset = null;

    private bool m_selected = false;

    #endregion

    #region construction

    /// <summary>
    /// Initializes a new instance of the DatasetViewModel class.
    /// </summary>
    public DatasetViewModel( Dataset dataset )
    {
      m_dataset = dataset;
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

    public string UID
    {
      get { return m_dataset.uid; }
      set 
      {
        if( !string.Equals( m_dataset.uid, value ))
        {
          m_dataset.uid = value;
          RaisePropertyChanged( "UID" );
        }
      }
    }


    public string Name
    {
      get { return m_dataset.name; }
      set 
      {
        if( !string.Equals( m_dataset.name, value ))
        {
          m_dataset.name = value;
          RaisePropertyChanged( "Name" );
        }
      }
    }


    public string Modality
    {
      get { return m_dataset.mod; }
      set 
      {
        if( !string.Equals( m_dataset.mod, value ))
        {
          m_dataset.mod = value;
          RaisePropertyChanged( "Modality" );
        }
      }
    }


    public string Manufacturer
    {
      get { return m_dataset.man; }
      set 
      {
        if( !string.Equals( m_dataset.man, value ))
        {
          m_dataset.man = value;
          RaisePropertyChanged( "Manufacturer" );
        }
      }
    }


    public string Model
    {
      get { return m_dataset.model; }
      set 
      {
        if( !string.Equals( m_dataset.model, value ))
        {
          m_dataset.model = value;
          RaisePropertyChanged( "Model" );
        }
      }
    }


    public string Orientation
    {
      get { return m_dataset.ort; }
      set 
      {
        if( !string.Equals( m_dataset.ort, value ))
        {
          m_dataset.ort = value;
          RaisePropertyChanged( "Orientation" );
        }
      }
    }


    public bool IsQA
    {
      get { return m_dataset.qa; }
      set 
      {
        if( m_dataset.qa != value )
        {
          m_dataset.qa = value;
          RaisePropertyChanged( "IsQA" );
        }
      }
    }


    public bool HasPrivateData
    {
      get { return m_dataset.cpd; }
      set 
      {
        if( m_dataset.cpd != value )
        {
          m_dataset.cpd = value;
          RaisePropertyChanged( "HasPrivateData" );
        }
      }
    }


    public string Description
    {
      get { return m_dataset.desc; }
      set 
      {
        if( !string.Equals( m_dataset.desc, value ))
        {
          m_dataset.desc = value;
          RaisePropertyChanged( "Description" );
        }
      }
    }


    public int ImageCount
    {
      get { return m_dataset.icnt; }
      set 
      {
        if( m_dataset.icnt != value )
        {
          m_dataset.icnt = value;
          RaisePropertyChanged( "ImageCount" );
        }
      }
    }


    public string Application
    {
      get { return m_dataset.app; }
      set 
      {
        if( !string.Equals( m_dataset.app, value ))
        {
          m_dataset.app = value;
          RaisePropertyChanged( "Application" );
        }
      }
    }


    public string Type
    {
      get { return m_dataset.type; }
      set 
      {
        if( !string.Equals( m_dataset.type, value ))
        {
          m_dataset.type = value;
          RaisePropertyChanged( "Type" );
        }
      }
    }


    public string Plane
    {
      get { return m_dataset.pln; }
      set 
      {
        if( !string.Equals( m_dataset.pln, value ))
        {
          m_dataset.pln = value;
          RaisePropertyChanged( "Plane" );
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