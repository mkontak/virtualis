using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI
{
  /// <summary>
  /// Summary description for SaveFiles.
  /// </summary>
  public class SaveFilesDialog : System.Windows.Forms.Form
  {
    private string m_path;
    private Viatronix.v3D.UI.Button m_buttonOk;
    private Viatronix.v3D.UI.Button m_buttonCancel;
    private System.Windows.Forms.Label m_labelPath;
    private System.Windows.Forms.TextBox m_textBoxPath;
    private System.Windows.Forms.FolderBrowserDialog m_folderBrowserDialog;
    private Viatronix.v3D.UI.Button m_buttonBrowsePath;
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.ComponentModel.IContainer components;

    #region initialization

    /// <summary>
    /// Constructor
    /// </summary>
    public SaveFilesDialog()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      m_path = "";
    }



    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }

    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.m_labelPath = new System.Windows.Forms.Label();
      this.m_textBoxPath = new System.Windows.Forms.TextBox();
      this.m_buttonOk = new Viatronix.v3D.UI.Button();
      this.m_buttonCancel = new Viatronix.v3D.UI.Button();
      this.m_buttonBrowsePath = new Viatronix.v3D.UI.Button();
      this.m_folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.SuspendLayout();
      // 
      // m_labelPath
      // 
      this.m_labelPath.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelPath.ForeColor = System.Drawing.Color.Navy;
      this.m_labelPath.Location = new System.Drawing.Point(8, 16);
      this.m_labelPath.Name = "m_labelPath";
      this.m_labelPath.Size = new System.Drawing.Size(72, 24);
      this.m_labelPath.TabIndex = 0;
      this.m_labelPath.Text = "Destination:";
      // 
      // m_textBoxPath
      // 
      this.m_textBoxPath.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxPath.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxPath.Location = new System.Drawing.Point(96, 16);
      this.m_textBoxPath.Name = "m_textBoxPath";
      this.m_textBoxPath.Size = new System.Drawing.Size(448, 21);
      this.m_textBoxPath.TabIndex = 1;
      this.m_textBoxPath.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxPath, "Enter the destination path to expand the dicom file image to.");
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_buttonOk.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonOk.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonOk.Location = new System.Drawing.Point(216, 48);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.Size = new System.Drawing.Size(72, 24);
      this.m_buttonOk.TabIndex = 12;
      this.m_buttonOk.Text = "Ok";
      this.m_toolTip.SetToolTip(this.m_buttonOk, "Click OK to save file image.");
      this.m_buttonOk.Click += new System.EventHandler(this.OkClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                       null,
                                                                       null,
                                                                       null};
      this.m_buttonCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonCancel.Location = new System.Drawing.Point(328, 48);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(72, 24);
      this.m_buttonCancel.TabIndex = 13;
      this.m_buttonCancel.Text = "Cancel";
      this.m_toolTip.SetToolTip(this.m_buttonCancel, "Click cancel to not save file image.");
      this.m_buttonCancel.Click += new System.EventHandler(this.CancelClick);
      // 
      // m_buttonBrowsePath
      // 
      this.m_buttonBrowsePath.CustomImages = new System.Drawing.Bitmap[] {
                                                                           null,
                                                                           null,
                                                                           null};
      this.m_buttonBrowsePath.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_buttonBrowsePath.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonBrowsePath.Location = new System.Drawing.Point(552, 16);
      this.m_buttonBrowsePath.Name = "m_buttonBrowsePath";
      this.m_buttonBrowsePath.Size = new System.Drawing.Size(48, 24);
      this.m_buttonBrowsePath.TabIndex = 14;
      this.m_buttonBrowsePath.Text = "...";
      this.m_toolTip.SetToolTip(this.m_buttonBrowsePath, "Click to browse for the destination folder.");
      this.m_buttonBrowsePath.Click += new System.EventHandler(this.OnPathBrowseClick);
      // 
      // m_folderBrowserDialog
      // 
      this.m_folderBrowserDialog.Description = "Destination Path for File(s)";
      // 
      // SaveFilesDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.ClientSize = new System.Drawing.Size(616, 78);
      this.Controls.Add(this.m_buttonBrowsePath);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonOk);
      this.Controls.Add(this.m_textBoxPath);
      this.Controls.Add(this.m_labelPath);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "SaveFilesDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Save File(s)";
      this.m_toolTip.SetToolTip(this, "Save the dicom file image into the destination path specified.");
      this.ResumeLayout(false);

    }
    #endregion

    #endregion

    #region properties

    /// <summary>
    /// File name property 
    /// </summary>
    public string Path
    {
      get 
      { 
        return m_path; 
      }

      set
      {
        m_path = value;
        m_textBoxPath.Text = m_path;
      }
    } // Path

    #endregion

    #region events

    /// <summary>
    /// OK button click event
    /// </summary>
    /// <param name="sender">Object reference of sender</param>
    /// <param name="e">Event arguements</param>
    private void OkClick(object sender, System.EventArgs e)
    {
      this.DialogResult = DialogResult.OK;
      m_path = m_textBoxPath.Text;
      this.Close();
    } // m_buttonOk_Click()

    /// <summary>
    /// Cancel button click event
    /// </summary>
    /// <param name="sender">Object reference of sender</param>
    /// <param name="e">Event arguements</param>
    private void CancelClick(object sender, System.EventArgs e)
    {
      this.DialogResult = DialogResult.Cancel;
      m_path = "";
      this.Close();
    } // m_buttonCancel_Click()

    #endregion

    private void OnPathBrowseClick(object sender, System.EventArgs e)
    {
      if ( m_folderBrowserDialog.ShowDialog(this) == DialogResult.OK )
      {
        m_textBoxPath.Text = m_folderBrowserDialog.SelectedPath;
      }
    }
  }
}
