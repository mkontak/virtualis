using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI
{
  /// <summary>
	/// Summary description for SaveAs.
	/// </summary>
	public class ImageSaveAsDialog : System.Windows.Forms.Form
	{
    private string m_fileName = string.Empty;
    private string m_extension = string.Empty;

    private System.Windows.Forms.Label m_labelFile;
    private System.Windows.Forms.TextBox m_textBoxFile;
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.SaveFileDialog m_saveFileDialog;
    private Viatronix.v3D.UI.Button m_buttonSaveFile;
    private Viatronix.v3D.UI.Button m_buttonCancel;
    private Viatronix.v3D.UI.Button m_buttonOk;
    private System.ComponentModel.IContainer components;

    #region constructor and desctructors

    /// <summary>
    /// Constructor
    /// </summary>
		public ImageSaveAsDialog(string extension)
		{


			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      m_extension = extension;

      m_fileName = "";
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ImageSaveAsDialog));
      this.m_labelFile = new System.Windows.Forms.Label();
      this.m_textBoxFile = new System.Windows.Forms.TextBox();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_saveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.m_buttonSaveFile = new Viatronix.v3D.UI.Button();
      this.m_buttonOk = new Viatronix.v3D.UI.Button();
      this.m_buttonCancel = new Viatronix.v3D.UI.Button();
      this.SuspendLayout();
      // 
      // m_labelFile
      // 
      this.m_labelFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelFile.ForeColor = System.Drawing.Color.Navy;
      this.m_labelFile.Location = new System.Drawing.Point(8, 16);
      this.m_labelFile.Name = "m_labelFile";
      this.m_labelFile.Size = new System.Drawing.Size(72, 24);
      this.m_labelFile.TabIndex = 0;
      this.m_labelFile.Text = "File Name:";
      // 
      // m_textBoxFile
      // 
      this.m_textBoxFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_textBoxFile.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxFile.Location = new System.Drawing.Point(96, 16);
      this.m_textBoxFile.Name = "m_textBoxFile";
      this.m_textBoxFile.Size = new System.Drawing.Size(488, 21);
      this.m_textBoxFile.TabIndex = 1;
      this.m_textBoxFile.Text = "";
      this.m_toolTip.SetToolTip(this.m_textBoxFile, "Enter the file name for the image to be saved, the extension determines the forma" +
        "t.");
      // 
      // m_buttonSaveFile
      // 
      this.m_buttonSaveFile.CustomImages = new System.Drawing.Bitmap[] {
                                                                         null,
                                                                         null,
                                                                         null};
      this.m_buttonSaveFile.ForeColor = System.Drawing.Color.Black;
      this.m_buttonSaveFile.Location = new System.Drawing.Point(592, 16);
      this.m_buttonSaveFile.Name = "m_buttonSaveFile";
      this.m_buttonSaveFile.OldStockButton = false;
      this.m_buttonSaveFile.Size = new System.Drawing.Size(32, 23);
      this.m_buttonSaveFile.TabIndex = 2;
      this.m_buttonSaveFile.Text = "...";
      this.m_buttonSaveFile.Click += new System.EventHandler(this.OnClickSave);
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.CustomImages = new System.Drawing.Bitmap[] {
                                                                   null,
                                                                   null,
                                                                   null};
      this.m_buttonOk.ForeColor = System.Drawing.Color.Black;
      this.m_buttonOk.Location = new System.Drawing.Point(231, 48);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.OldStockButton = false;
      this.m_buttonOk.TabIndex = 3;
      this.m_buttonOk.Text = "Ok";
      this.m_buttonOk.Click += new System.EventHandler(this.OnOkClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                       null,
                                                                       null,
                                                                       null};
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Black;
      this.m_buttonCancel.Location = new System.Drawing.Point(335, 48);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.TabIndex = 4;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // ImageSaveAsDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.ClientSize = new System.Drawing.Size(640, 85);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonOk);
      this.Controls.Add(this.m_buttonSaveFile);
      this.Controls.Add(this.m_textBoxFile);
      this.Controls.Add(this.m_labelFile);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ImageSaveAsDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Save Image";
      this.m_toolTip.SetToolTip(this, "Save dicom image in another format.");
      this.ResumeLayout(false);

    }
		#endregion

    #endregion

    #region properties

    /// <summary>
    /// File name property 
    /// </summary>
    public string Filename
    {
      get 
      { 
        return m_fileName; 
      }

      set
      {
        m_fileName = value;
        m_textBoxFile.Text = m_fileName;
      }
    } // Filename

    #endregion

    #region events

    /// <summary>
    /// OK button click event
    /// </summary>
    /// <param name="sender">Object reference of sender</param>
    /// <param name="e">Event arguements</param>
    private void OnOkClick(object sender, System.EventArgs e)
    {
      this.DialogResult = DialogResult.OK;
      m_fileName = m_textBoxFile.Text;
      this.Close();
    } // m_buttonOk_Click()

    /// <summary>
    /// Cancel button click event
    /// </summary>
    /// <param name="sender">Object reference of sender</param>
    /// <param name="e">Event arguements</param>
    private void OnCancelClick(object sender, System.EventArgs e)
    {
      this.DialogResult = DialogResult.Cancel;
      m_fileName = "";
      this.Close();
    } // m_buttonCancel_Click()

    #endregion

    #region event handlers

    private void OnClickSave(object sender, System.EventArgs e)
    {
      m_saveFileDialog.RestoreDirectory = true;
      m_saveFileDialog.Title = "Save File";
      m_saveFileDialog.AddExtension = true;
      m_saveFileDialog.CheckPathExists = true;
      m_saveFileDialog.DefaultExt = m_extension;
      m_saveFileDialog.Filter = m_extension.Substring(1,m_extension.Length - 1) + " file |*" + m_extension;

      if ( m_saveFileDialog.ShowDialog(this) == DialogResult.OK )
      {
        m_textBoxFile.Text = m_saveFileDialog.FileName;
      } 
    }

    #endregion


	}
}
