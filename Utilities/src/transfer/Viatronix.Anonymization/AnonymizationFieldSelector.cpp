// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// includes
#include "StdAfx.h"
#include "AnonymizationInformation.h"
#include "AnonymizationFieldSelector.h"
#include "Exceptions.h"

// namesaces
using namespace System;
using namespace System::Collections::Generic;
using namespace Viatronix::UI;
using namespace Viatronix::Logging;
using namespace System::Windows::Forms;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Data;
using namespace System::Drawing;

USING_ANMZN_NS

/* 
* Constructor
*/
FieldEditor::FieldEditor(void)
{
  InitializeComponent();
  this->m_fieldViewerGrid->Font = Theme::FormFont;
  
  m_lastRow = 0;
  m_bTextChanged = false;
  m_pOldValue = nullptr;
  m_anmznInfo = nullptr;
  m_dataTable = nullptr;
  m_pIImage = nullptr;

} // FieldEditor(void)

/* 
 ^ Constructor with args
 ^ 
 ^ @param pInfo          Anonymization information
 ^ @param sDicomSource   Diocom source
 */
FieldEditor::FieldEditor( AnonymizationInformation ^  pInfo , String ^ sDicomSource )
{
  InitializeComponent();

  this->m_fieldViewerGrid->Font = Theme::FormFont;
    
  m_lastRow = 0;
  m_bTextChanged = false;
  m_pOldValue = nullptr;

  m_pIImage = nullptr;
  m_dcmSource = sDicomSource;
  m_anmznInfo = pInfo;
  m_dataTable = gcnew DataTable("Control");
  CreateTableStyle();
  LoadValues();
} // FieldEditor( AnonymizationInformation ^  pInfo , String ^ sDicomSource )


/*
 ^ Handles the OK click event
 *
 ^ @param pSender       Sending object
 ^ @param pArgs         Evenet args
 */
Void FieldEditor::OnOkClickHandler( Object ^  pSender, System::EventArgs ^ pArgs )
{

  if ( ! ValidateValue() )
  {
    return;
  }

  
  Viatronix::Dicom::ElementCollection ^ pElements(m_pNewIImage->GetElements());

 
  for each ( DataRow ^ mpRow in m_dataTable->Rows )
  {

    int index = 0;


    //Iterate through the newValue Tags and Its Values Find the equivalent of the gcnew Value from the Hash Table.   
    for each ( Viatronix::Dicom::Element ^ mpElement in pElements )
    {
     
                                                                                
      try
      {
        // Compare row IDs and set the values.
        if ( mpElement->Id == Int32::Parse(mpRow[0]->ToString()) )
        {
          m_pNewIImage->SetValue<String ^>( mpElement->GroupId, mpElement->ElementId, mpRow[2]->ToString());

          break;
        }
      }
      catch(Viatronix::Dicom::DicomException ^ ex )
      {
        Log::Error ( String::Concat ( "Ignoring" ,ex->Message), "FieldEditor", "OnOkClickHandler" );
      }
    }//for
  }//while

  m_anmznInfo->AddSearchAndReplaceObjects( m_pIImage );

  this->DialogResult = System::Windows::Forms::DialogResult::OK;
  this->Close();

} // OnOkClickHandler( Object ^  pSender, EventArgs ^ pArgs )


/* 
 ^ Creates DataGrid Table Style
 */
void FieldEditor::CreateTableStyle()
{
  try
  {
    //Create the DataTable with 3 Columns as in the Grid
    m_dataTable = gcnew DataTable("Control");

    DataGridTableStyle ^ gridTableStyle  = gcnew DataGridTableStyle();
    gridTableStyle->MappingName          = "Control";
    gridTableStyle->RowHeadersVisible    = true;
   // gridTableStyle->PreferredRowHeight   = 8;
    gridTableStyle->RowHeaderWidth       = 10;

    m_dataTable->Columns->Add( "Field" );


    //Column which holds the Option Value.
    ReadOnlyDataGridTextBoxColumn ^ fieldCol = gcnew ReadOnlyDataGridTextBoxColumn(); 
    fieldCol->MappingName     = "Field";
    fieldCol->HeaderText      = "Field";
    fieldCol->Width           = ( m_fieldViewerGrid->Width/3 )- 12;
    fieldCol->Alignment       = HorizontalAlignment::Left;

    gridTableStyle->GridColumnStyles->Add( fieldCol );

    m_dataTable->Columns->Add( "Current Value" );
    //Column which holds the Current Value.
    ReadOnlyDataGridTextBoxColumn ^ valueCol = gcnew ReadOnlyDataGridTextBoxColumn(); 
    valueCol->MappingName     = "Current Value";
    valueCol->HeaderText      = "Current Value";
    valueCol->Width           = ( m_fieldViewerGrid->Width/3 )- 12;
    valueCol->Alignment       = HorizontalAlignment::Left;

    gridTableStyle->GridColumnStyles->Add( valueCol );

    m_dataTable->Columns->Add( "New Value" );
    //Column which holds the gcnew Value.
    DataGridTextBoxColumn ^ newValueCol = gcnew DataGridTextBoxColumn(); 
    newValueCol->MappingName     = "New Value";
    newValueCol->HeaderText      = "New Value";
    newValueCol->Width           = ( m_fieldViewerGrid->Width/3 )- 13; 
    newValueCol->Alignment       = HorizontalAlignment::Left;
    newValueCol->TextBox->TextChanged += gcnew System::EventHandler(this, &FieldEditor::OnTextChangedHandler);

    gridTableStyle->GridColumnStyles->Add( newValueCol );


    m_dataTable->Columns->Add( "Group" );
    DataGridTextBoxColumn ^ groupCol = gcnew DataGridTextBoxColumn(); 
    //Column which holds the gcnew Value.
    groupCol->MappingName     = "Group";
    groupCol->HeaderText      = "Group";
    groupCol->Width           = 0;
    groupCol->Alignment       = HorizontalAlignment::Left;
    gridTableStyle->GridColumnStyles->Add( groupCol );

    m_dataTable->Columns->Add( "Element" );
    DataGridTextBoxColumn ^ elementCol = gcnew DataGridTextBoxColumn(); 
    //Column which holds the gcnew Value.
    elementCol->MappingName     = "Element";
    elementCol->HeaderText      = "Element";
    elementCol->Width           = 0;
    elementCol->Alignment       = HorizontalAlignment::Left;
    gridTableStyle->GridColumnStyles->Add( elementCol );


    //Donot allow adding gcnew Rows.
    m_dataTable->DefaultView->AllowNew = false;
    ////Bind the dataTable to the Grid.
    m_fieldViewerGrid->DataSource = m_dataTable;
    m_fieldViewerGrid->TableStyles->Add( gridTableStyle );
  }
  catch ( Exception ^ ex )
  {
    Log::Error ( ex->Message, "FieldEditor", "CreateTableStyle" );
    throw ex;
  }

}//CreateTableStyle


/*
 ^ Load values on DataGrid.
 */
void FieldEditor::LoadValues()
{
  //Get the gcnew Element List which was read from the Configuration XML File.
  m_pNewIImage = (Viatronix::Dicom::Image ^)m_anmznInfo->AnonymizationElementList; 

  //Get the DICOM Element List
  m_pIImage = Viatronix::Dicom::ImageFactory::Create( m_dcmSource, Viatronix::Dicom::ParameterType::HEADER );

  for each ( Viatronix::Dicom::Element ^ mpElement in m_pNewIImage->GetElements() )
  {
    
    try
    {
      DataRow ^ row(m_dataTable->NewRow());

      row[0] = mpElement->Name;

      String ^ msValue(String::Empty);
      m_pIImage->TryGetValue( mpElement->GroupId, mpElement->ElementId,  msValue);

      row[1] = msValue;

      msValue = String::Empty;
      m_pNewIImage->TryGetValue( mpElement->GroupId, mpElement->ElementId, msValue );
      row[2] = msValue;

      row[3] = String::Format("{0:X4}",mpElement->GroupId);
      row[4] = String::Format("{0:X4}",mpElement->ElementId);


      m_dataTable->Rows->Add(row);
    }
    catch(Viatronix::Dicom::DicomException ^ ex )
    {
      Log::Error ( String::Concat ( "Ignoring" ,ex->Message), "FieldEditor", "LoadValue" );
    }
  }

  m_dataTable->DefaultView->AllowNew = false;
  //Bind the dataTable to the Grid.
  m_fieldViewerGrid->DataSource = m_dataTable;

}//LoadValues()


/*
 ^ Handles the text changed event.
 ^ 
 ^ @param pSender       Sending object
 ^ @param pArgs         event args
 */
void FieldEditor::OnTextChangedHandler(Object ^ pSender, System::EventArgs ^ pArgs)
{
  //Since the 1st cell is Read Only,Set the 2nd Cell as the Current cell on a cell click.
  if( m_fieldViewerGrid->CurrentCell.ColumnNumber == 2  )
  {
    m_bTextChanged = true;
  }
} // OnTextChangedHandler(Object ^ pSender, EventArgs ^ pArgs)

/*
 ^ Handles the currentcellchanged event
 ^ 
 ^ @param pSender       Sending object
 ^ @param pArgs         event args
 */
Void FieldEditor::OnCurrentCellChangedHandler( Object ^ pSender, System::EventArgs ^  pArgs )
{
  //Since the 1st cell is Read Only,Set the 2nd Cell as the Current cell on a cell click.
  if( m_fieldViewerGrid->CurrentCell.ColumnNumber == 0 || m_fieldViewerGrid->CurrentCell.ColumnNumber == 1 )
    m_fieldViewerGrid->CurrentCell = DataGridCell( m_fieldViewerGrid->CurrentRowIndex,2 );

 
  ValidateValue();

}//OnCurrentCellChangedHandler( Object ^  sender, EventArgs ^  e)


/**
 ^ Validates any changed value by looking at the last cell
 *
 ^ @return Returns true if valid, false if not
 */
bool FieldEditor::ValidateValue()
{
  bool bReturn(true);

  int currentRow(m_fieldViewerGrid->CurrentRowIndex);
  DataRow ^ pCurrentRow(m_dataTable->Rows[currentRow]);
  DataRow ^ pLastRow(m_dataTable->Rows[m_lastRow]);

  String ^ sDesc(pLastRow["Field"]->ToString());
  Object ^ pCurrentValue(pCurrentRow["New Value"]);
  Object ^ pLastValue(pLastRow["New Value"]);

  Viatronix::Dicom::Tag ^ pTag = gcnew Viatronix::Dicom::Tag(Convert::ToUInt16(pLastRow["Group"]->ToString()), 
                                                            Convert::ToUInt16(pLastRow["Element"]->ToString()));

  if ( m_bTextChanged )
  {
    if (  ! m_pOldValue->Equals(pLastValue) )
    {
      //
      // TODO  We need to validate the 
      // 
      if ( ! Viatronix::Dicom::Utility::IsValid(pTag->Group, pTag->Element, String::Empty, pLastValue->ToString()) )
      {
        Viatronix::UI::MessageBox::ShowError(this, String::Format("Invalid value entered for {0}, please re-enter", sDesc));
        pLastRow["New Value"] = m_pOldValue;
        m_fieldViewerGrid->CurrentCell = DataGridCell( m_lastRow,2 );
        bReturn = false;

      }
    }
  }
  else
  {
    m_lastRow = currentRow;
    m_bTextChanged = false;
    m_pOldValue = pLastValue;
  }

  return bReturn;
} // ValidateValue()

/*
 ^ Handles the cancelc click event
 ^ 
 ^ @param pSender       Sending object
 ^ @param pArgs         event args
 */
Void FieldEditor::OnCancelClickHandler( Object ^  pSender, System::EventArgs ^  pArgs )
{
  this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
  this->Close();
}//OnCancelClick(Object ^ pSender, EventArgs ^  sArgs)



// $Log: AnonymizationFieldSelector.cpp,v $
// Revision 1.14  2007/03/12 15:24:37  mkontak
// coding standards
//
// Revision 1.13  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.12  2006/09/19 19:05:43  mkontak
// Issue 4907
//
// Revision 1.11  2006/07/26 15:16:14  mkontak
// Fixed problem in description for anomymization override
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationFieldSelector.cpp,v 1.14 2007/03/12 15:24:37 mkontak Exp $
// $Id: AnonymizationFieldSelector.cpp,v 1.14 2007/03/12 15:24:37 mkontak Exp $