using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// The attribute view model.
  /// </summary>
  public class DicomHostAttributeViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The name of the attributes.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The value.
    /// </summary>
    private string m_value = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DicomHostAttributeViewModel()
    {
    } // DicomHostAttributeViewModel()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="name">The name of the attribute.</param>
    /// <param name="value">The value.</param>
    public DicomHostAttributeViewModel(string name, string value)
    {
      m_name = name;
      m_value = value;
    } // DicomHostAttributeViewModel( name, value )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set
      {
        m_name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the value.
    /// </summary>
    public string Value
    {
      get { return m_value; }
      set
      {
        m_value = value;
        RaisePropertyChanged("Value");
      }
    } // Value

    #endregion

  } // class DicomHostAttributeViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
