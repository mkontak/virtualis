using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using Viatronix.UI;
using Viatronix.Dicom;

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class DicomHeaderDialog : System.Windows.Forms.Form
	{
    private Viatronix.v3D.Dicom.UI.DicomHeaderControl m_dicomHeaderControl;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    #region initialization

    public DicomHeaderDialog()
		{
		
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(DicomHeaderDialog));
      this.m_dicomHeaderControl = new Viatronix.v3D.Dicom.UI.DicomHeaderControl();
      this.SuspendLayout();
      // 
      // m_dicomHeaderControl
      // 
      this.m_dicomHeaderControl.AllowDrop = true;
      this.m_dicomHeaderControl.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_dicomHeaderControl.Cursor = System.Windows.Forms.Cursors.Default;
      this.m_dicomHeaderControl.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_dicomHeaderControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
      this.m_dicomHeaderControl.ForeColor = System.Drawing.Color.Navy;
      this.m_dicomHeaderControl.Location = new System.Drawing.Point(0, 0);
      this.m_dicomHeaderControl.Name = "m_dicomHeaderControl";
      this.m_dicomHeaderControl.Size = new System.Drawing.Size(810, 303);
      this.m_dicomHeaderControl.TabIndex = 0;
      // 
      // DicomHeaderDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(810, 303);
      this.Controls.Add(this.m_dicomHeaderControl);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "DicomHeaderDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Dicom Header";
      this.Resize += new System.EventHandler(this.OnResize);
      this.ResumeLayout(false);

    }
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new DicomHeaderDialog());
    }

    #endregion

    #region properties

    public bool EnableContextMenu
    {
      get { return m_dicomHeaderControl.EnableContextMenu; }
      set { m_dicomHeaderControl.EnableContextMenu = value; }
    }
    #endregion

    #region methods

    public void SetElements(string dicomFile)
    {
      m_dicomHeaderControl.SetElements(dicomFile);
    }

    private void OnResize(object sender, System.EventArgs e)
    {

      int height = this.Size.Height;
    
    }

    public void SetElements(Viatronix.Dicom.Image dataset)
    {
      m_dicomHeaderControl.SetElements(dataset);
    }

    #endregion

  }
}
