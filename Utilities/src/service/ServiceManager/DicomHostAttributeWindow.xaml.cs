using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using Viatronix.Enterprise;
using Viatronix.Utilities.Service.ServiceManager.ViewModels;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Interaction logic for DicomHostAttributeWindow.xaml
  /// </summary>
  public partial class DicomHostAttributeWindow : Window
  {

    #region fields

    /// <summary>
    /// The editing mode.
    /// </summary>
    private EditingModes m_editingMode = EditingModes.Insert;

    /// <summary>
    /// The dicom host.
    /// </summary>
    private DicomHostViewModel m_dicomHost = null;

    /// <summary>
    /// The attribute we are working with.
    /// </summary>
    private DicomHostAttributeViewModel m_attribute = new DicomHostAttributeViewModel();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DicomHostAttributeWindow()
    {
      InitializeComponent();

      DataContext = m_attribute;
    } // DicomHostAttributeWindow()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the editing mode.
    /// </summary>
    public EditingModes EditingMode
    {
      get { return m_editingMode; }
      set 
      {
        m_editingMode = value;
        m_nameTextBox.IsEnabled = (value == EditingModes.Insert);
      }
    } // EditingMode


    /// <summary>
    /// Gets or sets the dicom host.
    /// </summary>
    public DicomHostViewModel DicomHost
    {
      get { return m_dicomHost; }
      set { m_dicomHost = value; }
    } // DicomHost


    /// <summary>
    /// Gets or sets the attribute.
    /// </summary>
    public DicomHostAttributeViewModel Attribute
    {
      get { return m_attribute; }
      set
      {
        m_attribute = value;
        DataContext = m_attribute;
      }
    } // Attribute

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the ok button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnOKClickHandler(object sender, EventArgs args)
    {
      try
      {
        foreach (DicomHostAttributeViewModel attribute in DicomHost.Attributes)
        {
          if (attribute.Name == m_attribute.Name && EditingMode != EditingModes.Update)
          {
            throw new ViatronixException("An attribute with that name already exists.", ErrorCodes.None);
          }
        }

        if(m_attribute.Name.Length < 1)
        {
          throw new ViatronixException("The attribute\'s name must not be empty.", ErrorCodes.None);
        }
        else if(m_attribute.Name.Length > 0 && m_attribute.Name[0] >= '0' && m_attribute.Name[0] <= '9')
        {
          throw new ViatronixException("Attribute\'s name must start with a lowercase/uppercase letter.", ErrorCodes.None);
        }

        for (int i = 0; i < m_attribute.Name.Length;++i )
        {
          char character = m_attribute.Name[i];
          if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '-' || character == '_'))
          {
            // If character not in the accepted characters group then fail.
            throw new ViatronixException(string.Format("Character \"{0}\" cannot appear in the attribute\'s name.  Please remove and try again.", character), ErrorCodes.None);
          }
        }

        for (int i = 0; i < m_attribute.Value.Length; ++i)
        {
          char character = m_attribute.Value[i];
          if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '-' || character == '_'))
          {
            // If character not in the accepted characters group then fail.
            throw new ViatronixException(string.Format("Character \"{0}\" cannot appear in the attribute\'s value.  Please remove and try again.", character), ErrorCodes.None);
          }
        }

        if (EditingMode == EditingModes.Insert)
        {
          DicomHost.AddAttribute(Attribute);
        }
        else if (EditingMode == EditingModes.Update)
        {
          DicomHost.UpdateAttribute(Attribute);
        }

        DialogResult = true;
      }
      catch (ViatronixException e)
      {
        if (EditingMode == EditingModes.Insert)
        {
          MessageBox.Show(string.Format("Could not insert host. {0}", e.Message), "Insert Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
        }
        else if (EditingMode == EditingModes.Update)
        {
          MessageBox.Show(string.Format("Could not update host. {0}", e.Message), "Update Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
        }
      }
    } // OnOKClickHandler( sender, args )


    /// <summary>
    /// Handles the cancel button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCancelClickHandler(object sender, EventArgs args)
    {
      DialogResult = false;
    } // OnCancelClickHandler( sender, args )

    #endregion

  } // class DicomHostAttributeWindow
} // namespace Viatronix.Utilities.Service.ServiceManager
