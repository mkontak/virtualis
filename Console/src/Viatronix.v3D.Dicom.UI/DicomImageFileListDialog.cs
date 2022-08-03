using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Specialized;
using vxDicom = Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Summary description for DicomFileListDialog.
	/// </summary>
	public class DicomImageFileListDialog : Viatronix.v3D.UI.Dialog
	{
    private System.Windows.Forms.ListView m_listViewFiles;
    private System.Windows.Forms.Label m_label;
    private System.Windows.Forms.Button m_buttonOK;
    private System.Windows.Forms.Label m_lableDicomFile;
    private System.Windows.Forms.TextBox m_textBoxDicomFile;
    private System.Windows.Forms.ColumnHeader m_columnHeaderFileName;
    private ColumnHeader m_columnHeaderPath;
    private ColumnHeader m_columnHeaderPiece;
    private ColumnHeader m_columnHeaderSize;
    private ColumnHeader m_columnHeaderTotalSize;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    public DicomImageFileListDialog(DicomImage image)
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      System.Collections.Generic.List<Viatronix.v3D.Dicom.FileImageInfo> list = image.GetFileList();
      
      for ( int i = 0; i < list.Count; ++i )
      {
        ListViewItem lv = new ListViewItem(list[i].FileName);

        lv.SubItems.Add(new ListViewItem.ListViewSubItem(lv,list[i].FilePath));
        lv.SubItems.Add(new ListViewItem.ListViewSubItem(lv,list[i].Piece.ToString() + " of " + list[i].TotalPieces.ToString()));
        lv.SubItems.Add(new ListViewItem.ListViewSubItem(lv,list[i].PieceSize.ToString()));
        lv.SubItems.Add(new ListViewItem.ListViewSubItem(lv,list[i].TotalSize.ToString()));


        m_listViewFiles.Items.Add(lv);
      }

      m_textBoxDicomFile.Text = image.DicomFile;
        
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
      this.m_listViewFiles = new System.Windows.Forms.ListView();
      this.m_columnHeaderFileName = new System.Windows.Forms.ColumnHeader();
      this.m_label = new System.Windows.Forms.Label();
      this.m_buttonOK = new System.Windows.Forms.Button();
      this.m_lableDicomFile = new System.Windows.Forms.Label();
      this.m_textBoxDicomFile = new System.Windows.Forms.TextBox();
      this.m_columnHeaderPiece = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderSize = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderTotalSize = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderPath = new System.Windows.Forms.ColumnHeader();
      this.SuspendLayout();
      // 
      // m_listViewFiles
      // 
      this.m_listViewFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.m_columnHeaderFileName,
            this.m_columnHeaderPath,
            this.m_columnHeaderPiece,
            this.m_columnHeaderSize,
            this.m_columnHeaderTotalSize});
      this.m_listViewFiles.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_listViewFiles.ForeColor = System.Drawing.Color.Navy;
      this.m_listViewFiles.GridLines = true;
      this.m_listViewFiles.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.m_listViewFiles.Location = new System.Drawing.Point(2, 65);
      this.m_listViewFiles.MultiSelect = false;
      this.m_listViewFiles.Name = "m_listViewFiles";
      this.m_listViewFiles.Size = new System.Drawing.Size(852, 255);
      this.m_listViewFiles.TabIndex = 0;
      this.m_listViewFiles.UseCompatibleStateImageBehavior = false;
      this.m_listViewFiles.View = System.Windows.Forms.View.Details;
      // 
      // m_columnHeaderFileName
      // 
      this.m_columnHeaderFileName.Text = "File name";
      this.m_columnHeaderFileName.Width = 400;
      // 
      // m_label
      // 
      this.m_label.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_label.ForeColor = System.Drawing.Color.Navy;
      this.m_label.Location = new System.Drawing.Point(8, 39);
      this.m_label.Name = "m_label";
      this.m_label.Size = new System.Drawing.Size(408, 23);
      this.m_label.TabIndex = 1;
      this.m_label.Text = "File list from the dicom image.";
      // 
      // m_buttonOK
      // 
      this.m_buttonOK.Location = new System.Drawing.Point(394, 325);
      this.m_buttonOK.Name = "m_buttonOK";
      this.m_buttonOK.Size = new System.Drawing.Size(75, 23);
      this.m_buttonOK.TabIndex = 2;
      this.m_buttonOK.Text = "Ok";
      this.m_buttonOK.Click += new System.EventHandler(this.OnOkClick);
      // 
      // m_lableDicomFile
      // 
      this.m_lableDicomFile.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
      this.m_lableDicomFile.Location = new System.Drawing.Point(8, 8);
      this.m_lableDicomFile.Name = "m_lableDicomFile";
      this.m_lableDicomFile.Size = new System.Drawing.Size(72, 23);
      this.m_lableDicomFile.TabIndex = 3;
      this.m_lableDicomFile.Text = "Dicom File:";
      // 
      // m_textBoxDicomFile
      // 
      this.m_textBoxDicomFile.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxDicomFile.Location = new System.Drawing.Point(88, 8);
      this.m_textBoxDicomFile.Name = "m_textBoxDicomFile";
      this.m_textBoxDicomFile.ReadOnly = true;
      this.m_textBoxDicomFile.Size = new System.Drawing.Size(766, 21);
      this.m_textBoxDicomFile.TabIndex = 4;
      // 
      // m_columnHeaderPiece
      // 
      this.m_columnHeaderPiece.DisplayIndex = 1;
      this.m_columnHeaderPiece.Text = "Piece";
      // 
      // m_columnHeaderSize
      // 
      this.m_columnHeaderSize.DisplayIndex = 2;
      this.m_columnHeaderSize.Text = "Piece Size";
      this.m_columnHeaderSize.Width = 100;
      // 
      // m_columnHeaderTotalSize
      // 
      this.m_columnHeaderTotalSize.DisplayIndex = 3;
      this.m_columnHeaderTotalSize.Text = "Total Size";
      this.m_columnHeaderTotalSize.Width = 100;
      // 
      // m_columnHeaderPath
      // 
      this.m_columnHeaderPath.DisplayIndex = 4;
      this.m_columnHeaderPath.Text = "Path";
      this.m_columnHeaderPath.Width = 200;
      // 
      // DicomImageFileListDialog
      // 
      this.AcceptButton = this.m_buttonOK;
      this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
      this.ClientSize = new System.Drawing.Size(857, 360);
      this.Controls.Add(this.m_textBoxDicomFile);
      this.Controls.Add(this.m_lableDicomFile);
      this.Controls.Add(this.m_label);
      this.Controls.Add(this.m_buttonOK);
      this.Controls.Add(this.m_listViewFiles);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ForeColor = System.Drawing.Color.Navy;
      this.Name = "DicomImageFileListDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Dicom Image File List";
      this.Controls.SetChildIndex(this.m_listViewFiles, 0);
      this.Controls.SetChildIndex(this.m_buttonOK, 0);
      this.Controls.SetChildIndex(this.m_label, 0);
      this.Controls.SetChildIndex(this.m_lableDicomFile, 0);
      this.Controls.SetChildIndex(this.m_textBoxDicomFile, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion

    private void OnOkClick(object sender, System.EventArgs e)
    {
      this.Close();
    }
	}
}
