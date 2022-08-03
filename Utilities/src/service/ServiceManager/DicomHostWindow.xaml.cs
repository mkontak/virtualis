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
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Service.ServiceManager.ViewModels;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Interaction logic for HostWindow.xaml
  /// </summary>
  public partial class DicomHostWindow : Window
  {

    #region fields

    /// <summary>
    /// The dicom host.
    /// </summary>
    private DicomHostViewModel m_dicomHost = new DicomHostViewModel();

    /// <summary>
    /// The editing mode.
    /// </summary>
    private EditingModes m_mode = EditingModes.Insert;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DicomHostWindow()
    {
      InitializeComponent();

      DataContext = m_dicomHost;
    } // DicomHostWindow()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the dicom host.
    /// </summary>
    public DicomHostViewModel DicomHost
    {
      get { return m_dicomHost; }
      set
      {
        m_dicomHost = value;
        DataContext = m_dicomHost;
      }
    } // DicomHost


    /// <summary>
    /// The editing mode.
    /// </summary>
    public EditingModes EditingMode
    {
      get { return m_mode; }
      set 
      {
        m_mode = value;
        m_nameTextBox.IsEnabled = value == EditingModes.Insert;
      }
    } // EditingMode
    
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
        foreach(DicomHost dicomHost in Global.GetDicomHosts())
        {
          if(dicomHost.Name == DicomHost.Name && EditingMode != EditingModes.Update)
          {
            throw new ViatronixException("A host with that name already exists.", ErrorCodes.None);
          }
        }

        if(m_dicomHost.Name.Length < 1)
        {
          throw new ViatronixException("The host\'s name must not be empty.", ErrorCodes.None);
        }
        else if (m_dicomHost.Name.Length > 0 && m_dicomHost.Name[0] >= '0' && m_dicomHost.Name[0] <= '9')
        {
          throw new ViatronixException("Host\'s name must start with a lowercase/uppercase letter.", ErrorCodes.None);
        }

        for (int i = 0; i < m_dicomHost.Name.Length; ++i)
        {
          char character = m_dicomHost.Name[i];
          if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '-' || character == '_'))
          {
            // If character not in the accepted characters group then fail.
            throw new ViatronixException(string.Format("Character \"{0}\" cannot appear in the host\'s name.  Please remove and try again.", character), ErrorCodes.None);
          }
        }

        if(m_dicomHost.Server.Length < 1)
        {
          throw new ViatronixException("The host\'s server must not be empty.", ErrorCodes.None);
        }

        for (int i = 0; i < m_dicomHost.Server.Length; ++i)
        {
          char character = m_dicomHost.Server[i];
          if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '-' || character == '_'))
          {
            // If character not in the accepted characters group then fail.
            throw new ViatronixException(string.Format("Character \"{0}\" cannot appear in the host\'s server.  Please remove and try again.", character), ErrorCodes.None);
          }
        }

        if(m_dicomHost.AETitle.Length < 1)
        {
          throw new ViatronixException("The host\'s AETitle must not be empty.", ErrorCodes.None);
        }
        if (m_dicomHost.AETitle.Length > 0 && m_dicomHost.AETitle[0] >= '0' && m_dicomHost.AETitle[0] <= '9')
        {
          throw new ViatronixException("Host\'s AETitle must start with a lowercase/uppercase letter.", ErrorCodes.None);
        }

        for (int i = 0; i < m_dicomHost.AETitle.Length; ++i)
        {
          char character = m_dicomHost.AETitle[i];
          if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '-' || character == '_'))
          {
            // If character not in the accepted characters group then fail.
            throw new ViatronixException(string.Format("Character \"{0}\" cannot appear in the host\'s aetitle.  Please remove and try again.", character), ErrorCodes.None);
          }
        }

        if(EditingMode == EditingModes.Insert)
        {
          Global.InsertDicomHost(DicomHost.DicomHost);
        }
        else if(EditingMode == EditingModes.Update)
        {
          Global.UpdateDicomHost(DicomHost.DicomHost);
        }

        DialogResult = true;
      }
      catch(ViatronixException e)
      {
        if (EditingMode == EditingModes.Insert)
        {
          MessageBox.Show(string.Format("Could not insert host. {0}", e.Message), "Insert Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
        }
        else if(EditingMode == EditingModes.Update)
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

  } // class DicomHostWindow
} // namespace Viatronix.Utilities.Service.ServiceManager
