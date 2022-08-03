// $Id: DicomHeaderSearchDialoig.Designer.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark KOntak] [mkontak@viatronix.com]


namespace Viatronix.v3D.Dicom.UI
{

  /// <summary>
  /// Search dialog for the dicom header
  /// </summary>
  partial class DicomHeaderSearchDialog
  {

    #region component model

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    #endregion

    #region finalizer

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    } //  Dispose(bool disposing)

    #endregion

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_listViewSearchResults = new System.Windows.Forms.ListView();
      this.m_buttonSearchCancel = new Viatronix.UI.Button();
      this.m_columnHeaderId = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderDescription = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderType = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderValue = new System.Windows.Forms.ColumnHeader();
      this.m_labelSearch = new System.Windows.Forms.Label();
      this.m_textBoxSearch = new System.Windows.Forms.TextBox();
      this.m_buttonCancel = new Viatronix.UI.Button();
      this.SuspendLayout();
      // 
      // m_listViewSearchResults
      // 
      this.m_listViewSearchResults.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.m_columnHeaderId,
            this.m_columnHeaderDescription,
            this.m_columnHeaderType,
            this.m_columnHeaderValue});
      this.m_listViewSearchResults.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_listViewSearchResults.ForeColor = System.Drawing.Color.Navy;
      this.m_listViewSearchResults.FullRowSelect = true;
      this.m_listViewSearchResults.Location = new System.Drawing.Point(-2, 55);
      this.m_listViewSearchResults.Name = "m_listViewSearchResults";
      this.m_listViewSearchResults.Size = new System.Drawing.Size(692, 233);
      this.m_listViewSearchResults.TabIndex = 0;
      this.m_listViewSearchResults.UseCompatibleStateImageBehavior = false;
      this.m_listViewSearchResults.View = System.Windows.Forms.View.Details;
      // 
      // m_buttonSearchCancel
      // 
      this.m_buttonSearchCancel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonSearchCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonSearchCancel.Location = new System.Drawing.Point(600, 12);
      this.m_buttonSearchCancel.Name = "m_buttonSearchCancel";
      this.m_buttonSearchCancel.OldStockButton = false;
      this.m_buttonSearchCancel.Size = new System.Drawing.Size(75, 23);
      this.m_buttonSearchCancel.TabIndex = 1;
      this.m_buttonSearchCancel.Text = "Cancel";
      this.m_buttonSearchCancel.UseVisualStyleBackColor = true;
      this.m_buttonSearchCancel.Click += new System.EventHandler(this.OnClickHandler);
      // 
      // m_columnHeaderId
      // 
      this.m_columnHeaderId.Text = "Id";
      this.m_columnHeaderId.Width = 128;
      // 
      // m_columnHeaderDescription
      // 
      this.m_columnHeaderDescription.Text = "Description";
      this.m_columnHeaderDescription.Width = 330;
      // 
      // m_columnHeaderType
      // 
      this.m_columnHeaderType.Text = "Type";
      this.m_columnHeaderType.Width = 57;
      // 
      // m_columnHeaderValue
      // 
      this.m_columnHeaderValue.Text = "Value";
      this.m_columnHeaderValue.Width = 173;
      // 
      // m_labelSearch
      // 
      this.m_labelSearch.AutoSize = true;
      this.m_labelSearch.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_labelSearch.ForeColor = System.Drawing.Color.AliceBlue;
      this.m_labelSearch.Location = new System.Drawing.Point(9, 15);
      this.m_labelSearch.Name = "m_labelSearch";
      this.m_labelSearch.Size = new System.Drawing.Size(51, 15);
      this.m_labelSearch.TabIndex = 2;
      this.m_labelSearch.Text = "Search:";
      this.m_labelSearch.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_textBoxSearch
      // 
      this.m_textBoxSearch.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_textBoxSearch.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxSearch.Location = new System.Drawing.Point(66, 12);
      this.m_textBoxSearch.Name = "m_textBoxSearch";
      this.m_textBoxSearch.Size = new System.Drawing.Size(528, 20);
      this.m_textBoxSearch.TabIndex = 3;
      this.m_textBoxSearch.TextChanged += new System.EventHandler(this.OnSearchTextChangedHandler);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_buttonCancel.ForeColor = System.Drawing.Color.Navy;
      this.m_buttonCancel.Location = new System.Drawing.Point(600, 12);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.OldStockButton = false;
      this.m_buttonCancel.Size = new System.Drawing.Size(75, 23);
      this.m_buttonCancel.TabIndex = 1;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.UseVisualStyleBackColor = true;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnClickHandler);
      // 
      // DicomHeaderSearchDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(687, 291);
      this.Controls.Add(this.m_listViewSearchResults);
      this.Controls.Add(this.m_buttonSearchCancel);
      this.Controls.Add(this.m_labelSearch);
      this.Controls.Add(this.m_textBoxSearch);
      this.Name = "DicomHeaderSearchDialog";
      this.Text = "Dicom Header Search";
      this.Controls.SetChildIndex(this.m_textBoxSearch, 0);
      this.Controls.SetChildIndex(this.m_labelSearch, 0);
      this.Controls.SetChildIndex(this.m_buttonSearchCancel, 0);
      this.Controls.SetChildIndex(this.m_listViewSearchResults, 0);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion


    #region fields

    /// <summary>
    /// List view search results
    /// </summary>
    private System.Windows.Forms.ListView m_listViewSearchResults;

    /// <summary>
    /// Colume header for the id
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_columnHeaderId;

    /// <summary>
    /// Column header for the description
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_columnHeaderDescription;

    /// <summary>
    /// Search button
    /// </summary>
    private Viatronix.UI.Button m_buttonSearchCancel;

    /// <summary>
    /// Column header for the tag
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_columnHeaderType;

    /// <summary>
    /// Column header for the value
    /// </summary>
    private System.Windows.Forms.ColumnHeader m_columnHeaderValue;


    /// <summary>
    /// Label for search
    /// </summary>
    private System.Windows.Forms.Label m_labelSearch;

    /// <summary>
    /// Text box for search
    /// </summary>
    private System.Windows.Forms.TextBox m_textBoxSearch;

    /// <summary>
    /// Cancel button
    /// </summary>
    private Viatronix.UI.Button m_buttonCancel;

    #endregion

  } // class DicomHeaderSearchDialog

} // namespace Viatronix.v3D.Dicom.UI

#region revision history

// $Log: DicomHeaderSearchDialoig.Designer.cs,v $
// Revision 1.2  2007/03/08 21:56:13  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/DicomHeaderSearchDialoig.Designer.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
// $Id: DicomHeaderSearchDialoig.Designer.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $

#endregion

