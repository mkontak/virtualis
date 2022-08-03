// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// pragmas
#pragma once

// namespace
using namespace System;
using namespace System::Drawing;

OPEN_ANMZN_NS 

// foreard declarations
ref class AnonymizationInformation;

/**
 * FieldEditor Allows the user to modify default anonymization information values.
 */
public ref class FieldEditor : public Viatronix::UI::Dialog
{

	public:

    ///Constructor
		FieldEditor();
    
    ///Constructor with args
    FieldEditor( AnonymizationInformation ^  anmznInfo , System::String ^ dcmSource );

   
  private: 

    ///Flag indicates the change in text
    bool m_bTextChanged;

    ///Keeps the last row
    int m_lastRow;

    ///Keeps the old value
    Object ^ m_pOldValue;
    
    ///DataGrid which shows the anon values
    System::Windows::Forms::DataGrid ^  m_fieldViewerGrid;

    ///OK button object
    Viatronix::UI::Button ^  m_ok;

    ///Cancel button object
    Viatronix::UI::Button ^  m_cancel;

    ///DataTable object
    System::Data::DataTable ^  m_dataTable;

    ///Anonymization Information Object.
    AnonymizationInformation ^  m_anmznInfo;

    ///Dicom Source file path
    System::String ^ m_dcmSource;

    ///Dicom Element List reads from the source DICOM.
    Viatronix::Dicom::IImage ^ m_pIImage;

    ///Dicom Element List holds the replacement values.
    Viatronix::Dicom::Image ^ m_pNewIImage;

    /// Create table style for dataGrid. 
    void CreateTableStyle();

    ///load the values
    void LoadValues ();

    /// Text changed event handler
    void OnTextChangedHandler(Object ^ sender, System::EventArgs ^ args);

    ///Handles the OK button click event.
    System::Void OnOkClickHandler(System::Object ^  pSender, System::EventArgs ^  pArgs );

    ///Handles the cancel click event.
    System::Void OnCancelClickHandler(System::Object ^  pSender, System::EventArgs ^  pArgs );

    ///Handles the cell changed event
    System::Void OnCurrentCellChangedHandler(System::Object ^  pSender, System::EventArgs ^  pArgs );

    /// Validates the mdified cell
    bool ValidateValue();

	private:

		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->m_fieldViewerGrid = gcnew System::Windows::Forms::DataGrid();
      this->m_ok = gcnew Viatronix::UI::Button();
      this->m_cancel = gcnew Viatronix::UI::Button();
      (safe_cast<System::ComponentModel::ISupportInitialize ^  >(this->m_fieldViewerGrid))->BeginInit();
      this->SuspendLayout();
      // 
      // m_fieldViewerGrid
      // 
      this->m_fieldViewerGrid->AlternatingBackColor = System::Drawing::SystemColors::InactiveBorder;
      this->m_fieldViewerGrid->BackgroundColor = System::Drawing::SystemColors::Window;
      this->m_fieldViewerGrid->CaptionBackColor = System::Drawing::SystemColors::ActiveBorder;
      this->m_fieldViewerGrid->CaptionFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
      this->m_fieldViewerGrid->CaptionVisible = false;
      this->m_fieldViewerGrid->DataMember = "";
      this->m_fieldViewerGrid->Font = gcnew System::Drawing::Font("Arial", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
      this->m_fieldViewerGrid->HeaderFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
      this->m_fieldViewerGrid->HeaderForeColor = System::Drawing::SystemColors::ControlText;
      this->m_fieldViewerGrid->Location = System::Drawing::Point(4, 0);
      this->m_fieldViewerGrid->Name = "m_fieldViewerGrid";
      this->m_fieldViewerGrid->PreferredRowHeight = 9;
      this->m_fieldViewerGrid->Size = System::Drawing::Size(526, 231);
      this->m_fieldViewerGrid->TabIndex = 0;
      this->m_fieldViewerGrid->CurrentCellChanged += gcnew System::EventHandler(this, &Viatronix::v3D::Anonymization::FieldEditor::OnCurrentCellChangedHandler);
      // 
      // m_ok
      // 
      array<System::Drawing::Bitmap ^> ^ __mcTemp__1 = gcnew array<System::Drawing::Bitmap ^>(3);
      __mcTemp__1[0] = nullptr;
      __mcTemp__1[1] = nullptr;
      __mcTemp__1[2] = nullptr;
      this->m_ok->CustomImages = __mcTemp__1;
      this->m_ok->ForeColor = System::Drawing::Color::Black;
      this->m_ok->Location = System::Drawing::Point(368, 243);
      this->m_ok->Name = "m_ok";
      this->m_ok->OldStockButton = false;
      this->m_ok->Size = System::Drawing::Size(70, 26);
      this->m_ok->TabIndex = 1;
      this->m_ok->Text = "OK";
      this->m_ok->Click += gcnew System::EventHandler(this, &Viatronix::v3D::Anonymization::FieldEditor::OnOkClickHandler);
      // 
      // m_cancel
      // 
      array<System::Drawing::Bitmap ^> ^  __mcTemp__2 = gcnew array<System::Drawing::Bitmap ^>(3);
      __mcTemp__2[0] = nullptr;
      __mcTemp__2[1] = nullptr;
      __mcTemp__2[2] = nullptr;
      this->m_cancel->CustomImages = __mcTemp__2;
      this->m_cancel->ForeColor = System::Drawing::Color::Black;
      this->m_cancel->Location = System::Drawing::Point(445, 243);
      this->m_cancel->Name = "m_cancel";
      this->m_cancel->OldStockButton = false;
      this->m_cancel->Size = System::Drawing::Size(70, 26);
      this->m_cancel->TabIndex = 2;
      this->m_cancel->Text = "Cancel";
      this->m_cancel->Click += gcnew System::EventHandler(this, &Viatronix::v3D::Anonymization::FieldEditor::OnCancelClickHandler);
      // 
      // FieldEditor
      // 
      this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
      this->ClientSize = System::Drawing::Size(550, 276);
      this->Controls->Add(this->m_cancel);
      this->Controls->Add(this->m_ok);
      this->Controls->Add(this->m_fieldViewerGrid);
      this->MaximizeBox = false;
      this->MinimizeBox = false;
      this->Name = "FieldEditor";
      this->ShowInTaskbar = false;
      this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Hide;
      this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
      this->Text = "Anonymization Field Editor";
      this->Controls->SetChildIndex(this->m_fieldViewerGrid, 0);
      this->Controls->SetChildIndex(this->m_ok, 0);
      this->Controls->SetChildIndex(this->m_cancel, 0);
      (safe_cast<System::ComponentModel::ISupportInitialize ^  >(this->m_fieldViewerGrid))->EndInit();
      this->ResumeLayout(false);

    }		


}; //public ref class FieldEditor : public Viatronix::UI::Dialog




/**
 ^ This class provides a ReadOnly Text Box Column which can be assocciated with a Grid Cell.
 */
public ref class ReadOnlyDataGridTextBoxColumn : public System::Windows::Forms::DataGridTextBoxColumn
{
  
  public: 
    

  // Constructor
  ReadOnlyDataGridTextBoxColumn()
	{
    //Set the Text Field as Read only
    this->ReadOnly = true;
	} // ReadOnlyDataGridTextBoxColumn


  protected:  
    
  /**
   ^ If Selects paint the cell with Blue else White. 
   *
   ^ @param gfx           Graphics Object
   ^ @param bounds        Bounds of the Column
   ^ @param source        CurrencyManager
   ^ @param rowNum        row number
   ^ @param backBrush     background brush
   ^ @param foreBrush     foreground brush
   ^ @param alignToRight  aliugn to right flag
   */

  void Paint(System::Drawing::Graphics ^ gfx, System::Drawing::Rectangle ^ bounds, System::Windows::Forms::CurrencyManager ^ source, int  rowNum, System::Drawing::Brush ^ backBrush, System::Drawing::Brush ^ foreBrush, bool alignToRight)
  {
    Brush ^ brushBack = ( this->DataGridTableStyle->DataGrid->CurrentRowIndex == rowNum ) ? gcnew SolidBrush(Color::FromArgb(49,106,197)):backBrush;
    Brush ^ brushFront = ( this->DataGridTableStyle->DataGrid->CurrentRowIndex == rowNum ) ? gcnew SolidBrush(Color::White):foreBrush;
  } // Paint(System::Drawing::Graphics ^ gfx, System::Drawing::Rectangle ^ bounds, System::Windows::Forms::CurrencyManager ^ source, int  rowNum, System::Drawing::Brush ^ backBrush, System::Drawing::Brush ^ foreBrush, bool alignToRight)

};//ReadOnlyDataGridTextBoxColumn
 
CLOSE_ANMZN_NS


// $Log: AnonymizationFieldSelector.h,v $
// Revision 1.15  2007/03/12 15:24:37  mkontak
// coding standards
//
// Revision 1.14  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.13  2006/09/19 19:05:43  mkontak
// Issue 4907
//
// Revision 1.12  2006/06/05 18:28:44  gdavidson
// ported to vs2005
//
// Revision 1.11  2005/11/01 15:04:28  romy
// changed the windows defualt location to center parent.
//
// Revision 1.10  2005/10/29 02:43:01  romy
// theme added
//
// Revision 1.9  2005/10/20 15:14:00  romy
// modified the row height
//
// Revision 1.8  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationFieldSelector.h,v 1.15 2007/03/12 15:24:37 mkontak Exp $
// $Id: AnonymizationFieldSelector.h,v 1.15 2007/03/12 15:24:37 mkontak Exp $