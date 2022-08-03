using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ErrorViewer
{

  /// <summary>
  /// Error Form 
  /// </summary>
  public partial class ErrorForm : Form
  {
    public ErrorForm(string title, string errorText)
    {

 
      InitializeComponent();

      this.Text = title;

      m_textBoxError.Text = errorText;

    }

    private void OnButtonClick(object sender, EventArgs e)
    {
      this.Close();
    }
  }
}
