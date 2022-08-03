using System;
using System.Threading;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using Viatronix.Utilities.General.AdministerHostsTable.ViewModels;
using Viatronix.Utilities.General.AdministerHostsTable.ValidationRules;

namespace Viatronix.Utilities.General.AdministerHostsTable
{
  /// <summary>
  /// Interaction logic for AttributeWindow.xaml
  /// </summary>
  public partial class AttributeWindow : Window
  {

    #region fields

    /// <summary>
    /// Indicates the name has an error in binding.
    /// </summary>
    private bool m_nameError = false;

    /// <summary>
    /// Indicates the value has an error in binding.
    /// </summary>
    private bool m_valueError = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public AttributeWindow()
    {
      InitializeComponent();
    } // AttributeWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the ok button's click event
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnOKClickHandler(object sender, EventArgs args)
    {
      AttributeViewModel model = (AttributeViewModel)this.DataContext;

      FreeformValidationRule rule = new FreeformValidationRule();
      ValidationResult result = rule.Validate(model.Name, Thread.CurrentThread.CurrentCulture);
      if (!result.IsValid)
      {
        MessageBox.Show(Messages.GetMessage("VX025") + result.ErrorContent.ToString(), Messages.GetMessage("VX026"));
        return;
      }

      result = rule.Validate(model.Value, Thread.CurrentThread.CurrentCulture);
      if (!result.IsValid)
      {
        MessageBox.Show(Messages.GetMessage("VX027") + result.ErrorContent.ToString(), Messages.GetMessage("VX028"));
        return;
      }

      KeyValuePair<string, ObservableCollection<AttributeViewModel>> keyPair = (KeyValuePair<string, ObservableCollection<AttributeViewModel>>)this.Tag;

      if (keyPair.Key != string.Empty)
      {
        foreach (AttributeViewModel viewModel in keyPair.Value)
        {
          if (keyPair.Key != viewModel.Name && model.Name.ToLower() == viewModel.Name.ToLower())
          {
            // Attribute exists with that name.
            MessageBox.Show(Messages.GetMessage("VX015"), Messages.GetMessage("VX016"));
            return;
          }
        }
      }
      else
      {
        foreach (AttributeViewModel viewModel in keyPair.Value)
        {
          if (viewModel.Name.ToLower() == model.Name.ToLower())
          {
            // Attribute exists with that name.
            MessageBox.Show(Messages.GetMessage("VX015"), Messages.GetMessage("VX016"));
            return;
          }
        }
      }


      DialogResult = true;
    } // OnOKClickHandler( sender, args )


    /// <summary>
    /// Handles the cancel button's click event
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCancelClickHandler(object sender, EventArgs args)
    {
      DialogResult = false;
    } // OnCancelClickHandler( sender, args )

    #endregion

  } // class AttributeWindow
} // namespace Viatronix.Utilities.General.AdministerHostsTable
