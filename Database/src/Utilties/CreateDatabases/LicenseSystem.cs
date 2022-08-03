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

  public partial class LicenseSystem : Form
  {


    #region constructirs

    public LicenseSystem()
    {

      InitializeComponent();
    }

    #endregion


    #region properties


    /// <summary>
    /// Gets the V3D Licesning Systems Instance name
    /// </summary>
    public string Password
    {
      get { return m_textBoxPassword.Text; }
    } // Password

    /// <summary>
    /// Gets the database server name SERVER\INSTANCE
    /// </summary>
    public string Server
    {
      get { return ( m_textBoxServer.Text.Length > 0 && m_textBoxInstance.Text.Length > 0  ? 
                      string.Format("{0}\\{1}",m_textBoxServer.Text,m_textBoxInstance.Text) : 
                      string.Empty ); }
    } // DatabaseSystemName

    #endregion


    #region event handlers

    private void OnCancelClick(object sender, EventArgs e)
    {
      DialogResult = System.Windows.Forms.DialogResult.Cancel;
    }

    private void OnOkClick(object sender, EventArgs e)
    {

      if (string.IsNullOrEmpty(this.Server))
      {
        MessageBox.Show(this, "No License server information has been entered, click Cancel if this is intended", "License Server", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        m_textBoxServer.Focus();
        return;
      }

      DialogResult = System.Windows.Forms.DialogResult.OK;

    
    }

    #endregion

    private void OnTestClick(object sender, EventArgs e)
    {

      if (string.IsNullOrEmpty(this.Server))
      {
        MessageBox.Show(this, "No License server information has been entered", "License Server", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxServer.Focus();
      }
      else if (string.IsNullOrEmpty(m_textBoxPassword.Text))
      {
        MessageBox.Show(this, "No password entered", "Invalid Password", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxPassword.Text = string.Empty;
        m_textBoxPassword.Focus();
      }
      else
      {
        string connectionString = "Data Source=" + m_textBoxServer.Text + "\\" + m_textBoxInstance.Text + ";Initial Catalog=master;User Id=sa;Password=" + m_textBoxPassword.Text + ";";


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
          }

        }

      }

    }

  }
}
