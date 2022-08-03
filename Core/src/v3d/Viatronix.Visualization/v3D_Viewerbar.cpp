// $Id: v3D_Viewerbar.cpp,v 1.19.8.1 2009/03/23 15:13:44 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_Viewerbar.h"
#include "v3D_Viewer.h"
#include "v3D_ViewerbarButtonCollection.h"

// namespaces
using namespace Viatronix::Visualization;
using namespace Viatronix::UI;
using namespace System::Windows::Forms;
//using namespace System::Drawing;

// resolves problem with vxBase typedef'd Point
#ifdef Point
#undef Point
#endif
#ifdef MessageBox
#undef MessageBox
#endif

// constants


// defines
#define FILE_REVISION "$REVISION 3.0$"



/** 
 * Initializes a Viewerbar object with the specified Viewer object.
 *
 * @param mpViewer parent viewer
 */
Viewerbar::Viewerbar( Viewer ^ mpViewer ) : 
 m_mpViewer( mpViewer ),
 m_bExpanded( true )
{
  m_mpTimer = nullptr;

  // enable double buffering
  SetStyle( ControlStyles::AllPaintingInWmPaint, true );
  SetStyle( ControlStyles::UserPaint, true );
  SetStyle( ControlStyles::DoubleBuffer, true );
  
  // create button collection
  m_mpButtons = gcnew Viatronix::Visualization::ViewerbarButtonCollection( this );

  // create ToolTip control
  m_mpToolTip = gcnew System::Windows::Forms::ToolTip();
  
  // 
  this->BackColor = System::Drawing::Color::Black;
  this->Size = System::Drawing::Size( 0 , 0 );
  this->Font = gcnew System::Drawing::Font( L"Lucida Console", 8 );
  
  m_iTrackItem = -1;

  m_mpViewer->Controls->Add( this );

  m_mpTimerDelgate = gcnew System::EventHandler( this, &Viewerbar::TimerCallbackDelegate );
  
} // constructor


/**
 * Recalculates the required size of the viewerbar, the size and location of the buttons
 */
void Viewerbar::RecalculateLayout()
{
  // remove all child controls
  this->Controls->Clear();
  
  // clear out the tooltips
  m_mpToolTip->RemoveAll();
  
  int height = m_mpToolBarLeftImage->Height;
  // do we have at least one button?
  if ( m_mpButtons->Count > 0 )
  {
    if( m_bExpanded )
    {
      int x = m_mpToolBarLeftImage->Width;
      for( int iIndex( 0 ); iIndex < m_mpButtons->Count; ++iIndex )
      {
        ViewerbarButton ^ mpButton = safe_cast< ViewerbarButton ^ >( m_mpButtons[ iIndex ]);

        mpButton->Viewer = m_mpViewer;

        // create a label control to act as a button
        System::Windows::Forms::Label ^ mpLabel = gcnew System::Windows::Forms::Label();

        // give the label a transparent background
        mpLabel->BackColor = System::Drawing::Color::Transparent;

        // don't process an invisible button
        if( !mpButton->Visible )
        {
          mpLabel->Visible = false;
          this->Controls->Add( mpLabel );
          continue;
        }

        // wire up the mouse event handlers
        mpLabel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler( this, &Viewerbar::OnMouseMoveHandler );
        mpLabel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler( this, &Viewerbar::OnMouseDownHandler );
        mpLabel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler( this, &Viewerbar::OnMouseUpHandler );
        this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler( this, &Viewerbar::OnKeyDownHandler );

        // set the bounds fo the label control
        const int4 iButtonSize = m_mpToolBarLeftImage->Height - 4;
        const int buttonY = m_mpToolBarLeftImage->Height / 2 - iButtonSize / 2;
        mpLabel->SetBounds( x, buttonY, iButtonSize, iButtonSize );
        mpButton->Bounds = mpLabel->Bounds;

        // set the tooltip for the button
        m_mpToolTip->SetToolTip( mpLabel, mpButton->Tooltip );

        // add the label to this control
        this->Controls->Add( mpLabel );
        
        // next button x location
        const int iButtonGap = 2;
        x += iButtonSize + iButtonGap;
      }
      
      // set the size and location for the viewerbar
      this->Location = System::Drawing::Point( 0, m_mpViewer->Height - height );
      const int4 iShowLessRightImage = 6;
      this->Size = System::Drawing::Size( x + m_mpToolBarRightImage->Width - iShowLessRightImage, height );
    }
    else
    {
      this->Location = System::Drawing::Point( 0, m_mpViewer->Height - height );
      this->Size = System::Drawing::Size( m_mpToolBarExpandImage->Width, height );
    }
  } // ( m_mpButtons->Count > 0 )
  else
  {
    this->Size = System::Drawing::Size( 0, 0 );
  }
} // RecalculateLayout()


/**
 * Called when the viewerbar needs to paint itself
 *
 * @param mpArgs paint arguments
 */
void Viewerbar::OnPaint( System::Windows::Forms::PaintEventArgs ^ mpArgs )
{
  // only paint if there are button
  if ( m_mpButtons->Count > 0 )
  {      
    System::Drawing::Graphics ^ gfx = mpArgs->Graphics;

    // highest quality painting
    gfx->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::HighQuality;

    if( m_bExpanded )
    {
      gfx->DrawImage( m_mpToolBarLeftImage, 0, 0 );

      int x = m_mpToolBarLeftImage->Width;
      for( ; x < this->Width - m_mpToolBarRightImage->Width; x += m_mpToolBarMiddleImage->Width )
        gfx->DrawImage( m_mpToolBarMiddleImage, x, 0 );

      gfx->DrawImage( m_mpToolBarRightImage, this->Width - m_mpToolBarRightImage->Width, 0 );
      
      // draw each button
      for ( int iIndex(0); iIndex < m_mpButtons->Count; ++iIndex )
      {
        // get associated button and bounding rectangle
        ViewerbarButton ^ mpButton = safe_cast< ViewerbarButton ^ > ( m_mpButtons[ iIndex ] );

        // skip invisible buttons
        if( !mpButton->Visible )
          continue;
        
        System::Drawing::Rectangle bounds = mpButton->Bounds;
        
        // draw the highlight around the mpButton

        if( m_iTrackItem == iIndex )
        {
          gfx->FillRectangle( Theme::SelectedBrush, bounds );
          gfx->DrawRectangle( Theme::SelectedPen, bounds );
        }

        System::Drawing::Image ^ mpImage = mpButton->Image;
        if( mpButton->IsDropSource )
          mpImage = mpButton->GetDropSourceImage();

        // center the image
        const int4 iImageX = bounds.X + bounds.Width / 2 - mpImage->Width / 2;
        const int4 iImageY = bounds.Y + bounds.Height / 2 - mpImage->Height / 2;
        
        // is the item selected?
        if ( m_iTrackItem == iIndex )
          gfx->DrawImage( mpImage, iImageX + 1, iImageY + 1, mpButton->Image->Width, mpButton->Image->Height );      
        else
          gfx->DrawImage( mpImage, iImageX, iImageY, mpButton->Image->Width, mpButton->Image->Height );      
          
        if( mpButton->IsDropSource )
          delete mpImage;
      
      } // for each button
    } //  
    else
    {
      gfx->DrawImage( m_mpToolBarExpandImage, 0, 0, m_mpToolBarExpandImage->Width, this->Height );
    }
    
    if( m_mpViewer->BorderWidth > 1.0F )
    {
      /*System::Drawing::Pen * mpPen = new System::Drawing::Pen( Viatronix::UI::Theme::SelectedViewerBorderColor, m_mpViewer->BorderWidth );
      
      gfx->DrawLine( mpPen, 0, 0, 0, this->Height - 1 );
      gfx->DrawLine( mpPen, 0, this->Height-1, this->Width-1, this->Height-1 );
      
      mpPen->Dispose(); 
    
      mpPen = new System::Drawing::Pen( Viatronix::UI::Theme::SelectedViewerBorderColor, 1.0F );
      
      gfx->DrawLine( mpPen, 0,              0,  this->Width-1,  0 );
      gfx->DrawLine( mpPen, this->Width-1,   0,  this->Width-1, this->Height-1 );
      
      mpPen->Dispose(); */
    }

  } // m_button->Count > 0
} // OnPaint()


/**
 * Thread timer callback funtion. Checks if mouse is still over control
 *
 * @param mpState state object
 */
void Viewerbar::OnTimerCallbackHandler( System::Object ^ mpState )
{
 this->Invoke( m_mpTimerDelgate );
} // OnTimerCallbackHandler()


/**
 * Callback for w
 */
void Viewerbar::TimerCallbackDelegate( System::Object ^ mpSender, System::EventArgs ^ mpArgs )
{
    // get the bounds of this control in screen coordinates
  System::Drawing::Rectangle bounds = this->RectangleToScreen( this->ClientRectangle );

  // check if the mouse has moved outside of this control
  if ( !bounds.Contains( this->MousePosition ) )
  {
    // kill the timer
    if ( m_mpTimer != nullptr )
    {
      delete m_mpTimer;
      m_mpTimer = nullptr;
    }

    // reset current;y tracked item
    m_iTrackItem = -1;
    Invalidate();
  }

} // TimerCallbackDelegate()


/**
 * Mouse enter handler for child label controls.
 *
 * @param mpSender sender of event
 * @param mpArgs mouse event arguments
 */
void Viewerbar::OnMouseMoveHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs )
{
  // create a timer to check when mouse moves outside this control
  if ( m_mpTimer == nullptr )
  {
    // create the callback delegate
    System::Threading::TimerCallback ^ mpCallback = gcnew System::Threading::TimerCallback( this, &Viewerbar::OnTimerCallbackHandler );
  
    // start the timer
    m_mpTimer = gcnew System::Threading::Timer( mpCallback, nullptr, 0, 100 );
  }
  
  // set the track item to the sender's associated index within the control array
  m_iTrackItem = this->Controls->IndexOf( safe_cast< System::Windows::Forms::Control ^ > ( mpSender ) );
  
  Refresh();
} // OnMouseEnterHandler()


/**
 * Mouse down handler for child label controls.
 *
 * @param mpSender sender of event
 * @param mpArgs mouse event arguments
 */
void Viewerbar::OnMouseDownHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs )
{
  this->InvokeOnClick( this, System::EventArgs::Empty );
  
  // get the index of the label in the control array
  int iIndex = this->Controls->IndexOf( safe_cast< System::Windows::Forms::Control ^ > ( mpSender ) );
  if( iIndex < 0 || iIndex >= m_mpButtons->Count )
    return;

  // associated viewerbar button
  ViewerbarButton ^ mpButton = safe_cast< ViewerbarButton ^ >( m_mpButtons[ iIndex ] );

  // does this item display a context menu?
  if( mpButton->HasContextMenu && mpArgs->Button == System::Windows::Forms::MouseButtons::Left )
  {
      Viatronix::UI::ContextMenu ^ mpContextMenu = mpButton->GetContextMenu();
    
      System::Drawing::Point location = mpButton->Bounds.Location;
      mpContextMenu->Show( this, location,(Viatronix::UI::ContextMenuAlignment)( Viatronix::UI::ContextMenuAlignment::Left | Viatronix::UI::ContextMenuAlignment::Bottom ));
      
      System::Windows::Forms::Control ^ mpLabel = safe_cast< System::Windows::Forms::Control ^ >( mpSender );

      mpLabel->Focus();
      m_iTrackItem = -1;
      Refresh();
  }
  else if( mpButton->IsDropSource && mpArgs->Button == System::Windows::Forms::MouseButtons::Right )
  {
    IDropSource ^ mpDropSource =  mpButton->GetDropSource();
    DoDragDrop(gcnew DataObject( "Viatronix.Viusalization.IDropSource", mpDropSource ), (DragDropEffects)(DragDropEffects::Copy | DragDropEffects::Link ));
    m_iTrackItem = -1;
    Refresh();
  }
} // OnMouseDownHandler()



/**
 *
 */
void Viewerbar::OnMouseDown( System::Windows::Forms::MouseEventArgs ^ mpArgs )
{
  __super::OnMouseDown( mpArgs );
  
  if( !m_bExpanded )
  {
    m_bExpanded = true;
    RecalculateLayout();
  }
  else 
  {
    if( mpArgs->X < m_mpToolBarLeftImage->Width )
    {
      m_bExpanded = false;
      RecalculateLayout();
    }
  }
  Invalidate();
}


/**
 * Event handler for the viewerbar's key down
 */
void Viewerbar::OnKeyDownHandler( System::Object ^ mpSender, System::Windows::Forms::KeyEventArgs ^ mpArgs )
{
  ::SendMessage( static_cast< HWND >( this->Parent->Handle.ToPointer() ), WM_KEYDOWN, static_cast< WPARAM >( mpArgs->KeyCode ), 0 );
}

/**
 * Mouse up handler for child label controls.
 *
 * @param mpSender sender of event
 * @param mpArgs mouse event arguments
 */
void Viewerbar::OnMouseUpHandler( System::Object ^ mpSender, System::Windows::Forms::MouseEventArgs ^ mpArgs )
{
  int iIndex(this->Controls->IndexOf( safe_cast< System::Windows::Forms::Control ^ > ( mpSender ) ));
  if( iIndex <0 || iIndex >= m_mpButtons->Count )
    return;


  // associated viewerbar button
  ViewerbarButton ^ mpButton = safe_cast< ViewerbarButton ^ >( m_mpButtons[ iIndex ] );

  if( !mpButton->HasContextMenu || ( mpButton->IsDropSource && mpArgs->Button != System::Windows::Forms::MouseButtons::Right ) )
  {
    mpButton->OnClick( mpArgs->Button );
  }
} // OnMouseUpHandler()



#undef FILE_REVISION


// Revision History:
// $Log: v3D_Viewerbar.cpp,v $
// Revision 1.19.8.1  2009/03/23 15:13:44  gdavidson
// Added the ablility to changed the visibility of a viewerbar button
//
// Revision 1.19  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.18  2006/01/31 17:07:54  geconomos
// corrected access of control from within timer
//
// Revision 1.17  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.16  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.15  2005/05/25 15:30:37  frank
// worked on displaying the full viewerbar skin image - it was being cut off
//
// Revision 1.14  2005/05/17 14:24:40  frank
// moved the buttons around to center them better
//
// Revision 1.13  2005/05/17 13:23:25  frank
// moved the collapse viewerbar button to the left
//
// Revision 1.12  2005/05/09 19:33:26  frank
// defaulted viewerbar to expanded
//
// Revision 1.11  2004/11/17 13:29:14  geconomos
// code cleanup
//
// Revision 1.10  2004/11/12 16:17:16  geconomos
// Defaulted the viewebar to be collapsed
//
// Revision 1.9  2004/11/02 20:44:20  geconomos
// fixed bug with not calling click method on button
//
// Revision 1.8  2004/10/18 02:50:37  geconomos
// reworked selected viewer's border color
//
// Revision 1.7  2004/10/15 17:08:33  geconomos
// defaulted toolbars to "expanded"
//
// Revision 1.6  2004/10/13 16:07:19  geconomos
// Added the ability to collapse the viewerbar.
//
// Revision 1.5  2004/10/12 00:14:07  geconomos
// now skinning the toolbar
//
// Revision 1.4  2004/07/12 15:04:54  geconomos
// modified to not show a popup if it doesn't have any menu items
//
// Revision 1.3  2004/07/09 18:41:15  geconomos
// Updating the "Bounds" property on the ViewerbarButton interface a button is positioned.
//
// Revision 1.2  2004/04/07 20:33:06  geconomos
// Removed NotifyParent method.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.26  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.25.2.1  2004/02/12 21:12:59  geconomos
// Changed made to viatronix.v3d.ui
//
// Revision 1.25  2004/01/26 12:37:11  geconomos
// Passed along key presses to viewer when mouse is over the viewer bar.
//
// Revision 1.24  2003/12/16 13:37:32  michael
// added FILE_REVISION for logging purposes
//
// Revision 1.23  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.22  2003/06/26 13:27:06  geconomos
// Fixed crash with viewebar button out of range.
//
// Revision 1.21  2003/06/06 12:32:43  frank
// Checked for null
//
// Revision 1.20  2003/06/02 13:19:10  geconomos
// Set font to a fixed point for popup menu.
//
// Revision 1.19  2003/05/27 19:10:18  geconomos
// Made viewer active when clicking the viewerbar.
//
// Revision 1.18  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.17  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.16  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.15  2003/04/23 15:03:09  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.14  2003/04/16 18:50:22  geconomos
// Issue #3110:  Buttons not refreshing after drop event.
//
// Revision 1.13  2003/04/08 14:54:52  geconomos
// Issue #2398: Tooltips are now available for viewerbar buttons.
//
// Revision 1.12  2003/04/07 18:09:00  geconomos
// Changed color White to RGB( 250, 250, 250 ).
//
// Revision 1.11  2003/04/07 15:52:36  michael
// transition from BackColor to BackgroundColor
//
// Revision 1.10  2003/03/20 13:45:19  geconomos
// Inclusion of v3D_Viewer.h
//
// Revision 1.9  2003/02/19 13:34:38  geconomos
// Changed selection color.
//
// Revision 1.8  2003/01/22 22:09:49  ingmar
// disabled typedef for Point
//
// Revision 1.7  2002/11/27 21:35:45  geconomos
// Handled case for viewerbar with no buttons.
//
// Revision 1.6  2002/11/26 18:11:06  geconomos
// added call to IViewerbar::Click on MouseDown
//
// Revision 1.5  2002/11/25 13:55:35  geconomos
// Default background color of viewerbar.
//
// Revision 1.4  2002/11/20 13:49:11  geconomos
// implemented click on ViewerbarButton
//
// Revision 1.3  2002/11/19 20:39:48  geconomos
// drawing refinments
//
// Revision 1.2  2002/11/18 21:01:06  geconomos
// Fixed bug with viewerbar not redrawing after a popup is displayed.
//
// Revision 1.1  2002/11/18 19:59:22  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Viewerbar.cpp,v 1.19.8.1 2009/03/23 15:13:44 gdavidson Exp $
// $Id: v3D_Viewerbar.cpp,v 1.19.8.1 2009/03/23 15:13:44 gdavidson Exp $
