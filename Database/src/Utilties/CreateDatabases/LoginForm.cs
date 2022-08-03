using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.Utilities.Database
{
  public partial class LoginForm : Form
  {
    public LoginForm(string server)
    {

      InitializeComponent();

      m_textBoxServer.Text = server;
    }


 
    public string Password
    {
      get { return m_textBoxPassword.Text; }
    }

    private void OnLoginClick(object sender, EventArgs e)
    {
      if (string.IsNullOrEmpty(m_textBoxPassword.Text))
      {
        MessageBox.Show(this, "No password entered", "Invalid Password", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxPassword.Text = string.Empty;
        m_textBoxPassword.Focus();
        return;
      }
      else
      {
        string connectionString = "Data Source=" + m_textBoxServer.Text  + ";Initial Catalog=master;User Id=sa;Password=" + m_textBoxPassword.Text + ";";


        using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(connectionString))
        {
          try
          {
            connection.Open();
          }
          catch (Exception ex)
          {
            MessageBox.Show(this, ex.Message, "Login Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
            m_textBoxPassword.Text = string.Empty;
            m_textBoxPassword.Focus();
            return;
          }

        }

      }

      DialogResult = System.Windows.Forms.DialogResult.OK;
    }

    private void OnCancelClick(object sender, EventArgs e)
    {
      DialogResult = System.Windows.Forms.DialogResult.Cancel;
    }
  }
}
