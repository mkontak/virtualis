using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  /// <summary>
  /// Dialog for displaying the selected directories for output.
  /// </summary>
  public partial class DirectoriesConfirmationDialog : Form
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="directories">The directories to display.</param>
    public DirectoriesConfirmationDialog(string[] directories)
    {
      InitializeComponent();
      m_directoriesListBox.Items.AddRange(directories);
    } // DirectoriesConfirmationDialog( directories )

    #endregion

  } // class DirectoriesConfirmationDialog
} // namespace Viatronix.Utilities.SelectiveManifestGenerator
