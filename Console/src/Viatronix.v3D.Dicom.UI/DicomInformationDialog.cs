using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using vxUI = Viatronix.v3D.UI;
using vxDicom = Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class DicomInformationDialog : vxUI.Dialog
	{
    private Viatronix.v3D.Dicom.UI.DicomInformationControl m_dicomInformationControl;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public DicomInformationDialog()
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
      this.m_dicomInformationControl = new Viatronix.v3D.Dicom.UI.DicomInformationControl();
      this.SuspendLayout();
      // 
      // m_dicomInformationControl
      // 
      this.m_dicomInformationControl.AllowDrop = true;
      this.m_dicomInformationControl.AllowElementSaving = false;
      this.m_dicomInformationControl.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_dicomInformationControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
      this.m_dicomInformationControl.ForeColor = System.Drawing.Color.Navy;
      this.m_dicomInformationControl.Location = new System.Drawing.Point(0, 0);
      this.m_dicomInformationControl.Name = "m_dicomInformationControl";
      this.m_dicomInformationControl.Size = new System.Drawing.Size(520, 568);
      this.m_dicomInformationControl.TabIndex = 0;
      // 
      // DicomInformationDialog
      // 
      this.AllowDrop = true;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(514, 568);
      this.Controls.Add(this.m_dicomInformationControl);
      this.Name = "DicomInformationDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Dicom Information";
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
			Application.Run(new DicomInformationDialog());
		}

    public void SetElements(string dicomFile)
    {
      m_dicomInformationControl.SetElements(dicomFile);
    }

    private void OnResize(object sender, System.EventArgs e)
    {

      int height = this.Size.Height;
    
    }

    public void SetElements(vxDicom.ElementList elementList)
    {
      m_dicomInformationControl.SetElements(elementList);
    }
	}
}
