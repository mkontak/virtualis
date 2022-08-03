// $Id: Toolbar.cs,v 1.8.2.4 2009/03/27 21:37:11 gdavidson Exp $
//
// Copyright (c) 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;

namespace Viatronix.UI
{
  #region enumerations

  /// <summary>
  /// Enumeration of all toolbar button styles.
  /// </summary>
  public enum ToolbarButtonStyle
  {
    PushButton,
    ToggleButton,
    TwoPart,
    Separator,
  }

  /// <summary>
  /// Enurmation for the pieces that make up a two-part button
  /// </summary>
  enum TwoPartButtonPiece
  {
    Button,
    Menu,
  }

  #endregion

  #region Toolbar class

  /// <summary>
  /// A toolbar control.
  /// </summary>
  public class Toolbar : System.Windows.Forms.Control
	{

    #region fields

    /// <summary>
    /// Collection of ToolbarButtons.
    /// </summary>
    private ToolbarButtonCollection m_buttons;

    /// <summary>
    /// The display size for a ToolbarButton.
    /// </summary>
    private Size m_buttonSize = new Size( 24, 24 );

    /// <summary>
    /// The display size for a ToolbarButton.
    /// </summary>
    private Size m_twoPartButtonSize = new Size( 36, 24 );

    /// <summary>
    /// Reference to the active hover button.
    /// </summary>
    private ToolbarButton m_hoverButton = null;

    /// <summary>
    /// Reference to the active down button.
    /// </summary>
    private ToolbarButton m_downButton = null;

    /// <summary>
    /// The state of the down button prior to it being active.
    /// </summary>
    private bool m_downButtonPrevState = false;

    /// <summary>
    /// Tooltip control.
    /// </summary>
    private System.Windows.Forms.ToolTip m_tooltip = new System.Windows.Forms.ToolTip();

    /// <summary>
    /// The mouse down handler for the ToolbarButtons.
    /// </summary>
    private System.Windows.Forms.MouseEventHandler m_labelMouseDownHandler;

    /// <summary>
    /// The mouse enter handler for the ToolbarButtons.
    /// </summary>
    private System.EventHandler m_labelMouseEnterHandler;

    /// <summary>
    /// The mouse leave handler for the ToolbarButtons.
    /// </summary>
    private System.EventHandler m_labelMouseLeaveHandler;

    /// <summary>
    /// The mouse up handler for the ToolbarButtons.
    /// </summary>
    private System.Windows.Forms.MouseEventHandler m_labelMouseUpHandler;

    /// <summary>
    /// Determines if transparency is used for the background.
    /// </summary>
    private bool m_isTransparentBackground = false;

    /// <summary>
    /// The amount of padding between buttons.
    /// </summary>
    private const int m_padding = 4;
    
    /// <summary>
    /// Controls that were added from the config file.
    /// </summary>
    private List< KeyValuePair< Control, string >> m_addedControls = new List< KeyValuePair< Control, string >>();

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of the Toolbar control without transparency.
    /// </summary>
    public Toolbar() : this( false )
    {
    } // Toolbar()


    /// <summary>
    /// Creates a new instance of the toolbar control with the specified transparency.
    /// </summary>
    /// <param name="transparentBackground">true if the background is transparent; otherwise false</param>
    public Toolbar(bool transparentBackground)
    {
      m_isTransparentBackground = transparentBackground;

      SetStyle( System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( System.Windows.Forms.ControlStyles.DoubleBuffer, true );
      SetStyle( System.Windows.Forms.ControlStyles.UserPaint, true );
      SetStyle( System.Windows.Forms.ControlStyles.SupportsTransparentBackColor, true );

      m_buttons = new ToolbarButtonCollection( this );

      m_labelMouseDownHandler = new System.Windows.Forms.MouseEventHandler( this.OnLabelMouseDownHandler );
      m_labelMouseEnterHandler = new System.EventHandler( this.OnLabelMouseEnterHandler );
      m_labelMouseLeaveHandler = new System.EventHandler( this.OnLabelMouseLeaveHandler );
      m_labelMouseUpHandler = new System.Windows.Forms.MouseEventHandler( this.OnLabelMouseUpHandler );

      m_tooltip.InitialDelay = 500;
      m_tooltip.ReshowDelay = 500;
    } // Toolbar( transparentBackground )

    
    #endregion

    #region properties

    /// <summary>
    /// Returns a collection of ToolbarButtons.
    /// </summary>
    public ToolbarButtonCollection Buttons
    {
      get { return m_buttons; }
    } // Buttons


    /// <summary>
    /// Gets or sets the size of the ToolbarButtons.
    /// </summary>
    public Size ButtonSize
    {
      get { return m_buttonSize; }
      set { m_buttonSize = value; }
    } // ButtonSize


    /// <summary>
    /// Gets or sets if the background is transparent.
    /// </summary>
    public bool IsTransparentBackground
    {
      get { return m_isTransparentBackground; }
      set { m_isTransparentBackground = value; }
    } // IsTransparentBackground


    /// <summary>
    /// Returns the amount of padding between ToolbarButtons.
    /// </summary>
    public new int Padding
    {
      get { return m_padding; }
    } // Padding

    #endregion

    #region methods

    /// <summary>
    /// Adds the control to the toolbar
    /// </summary>
    /// <param name="control">Child control</param>
    /// <param name="tooltip">tooltip message</param>
    public void AddControl( Control control, string tooltip )
    {
      m_addedControls.Add( new KeyValuePair< Control, string >( control, tooltip ));
    } // AddControl( control, tooltip )

    #endregion

    #region override methods

    /// <summary>
    /// This member overrides Control.OnPaint.
    /// </summary>
    /// <param name="args">A PaintEventArgs that contains the event data.</param>
    protected override void OnPaint( System.Windows.Forms.PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      if( this.IsTransparentBackground )
        this.BackColor = Color.Transparent;
      else
      {
        gfx.FillRectangle( Theme.ToolbarMiddleBrush, 0, 0, this.Width, this.Height );

        gfx.DrawImage( Theme.ToolbarLeftImage, 0, 0 );
        gfx.DrawImage( Theme.ToolbarRightImage, this.Width - Theme.ToolbarRightImage.Width, 0 );
      }
      
      int x = m_padding;
      int y = m_padding;
      int offset = 2;

      // enumerate of all the 
      foreach( ToolbarButton button in m_buttons )
      {
        if( !button.Visible )
          continue;

        // draw separator
        if( button.Style == ToolbarButtonStyle.Separator )
        {
          int seperatorHeight = m_buttonSize.Height - ( offset * 2 );
          //int seperatorWidth = 1;
          gfx.DrawLine( Pens.Black, x, y + offset, x + 1, y + offset );
          gfx.DrawLine( Pens.Black, x, y + offset, x, y + offset + seperatorHeight );
          gfx.DrawLine( Pens.White, x, y + offset + seperatorHeight, x + 1, y + offset + seperatorHeight );
          gfx.DrawLine( Pens.White, x + 1, y + offset, x + 1, y + offset + seperatorHeight );

          x += 2 + m_padding;
          continue;
        }

        // get the width and height of the button based on the style
        int width = button.Style == ToolbarButtonStyle.TwoPart? m_twoPartButtonSize.Width : m_buttonSize.Width;
        int height = m_buttonSize.Height;

        if( button.Enabled && this.Enabled )
        {
          // draw the pushed toolbar style
          if( button.Pushed || ( button == m_downButton && button == m_hoverButton ))
          {
            Brush fillBrush = ( button == m_hoverButton ) ? Theme.ToolbarMiddleBrush : Theme.DownBrush;

            gfx.FillRectangle(fillBrush, x, y, m_buttonSize.Width, m_buttonSize.Height);

            gfx.DrawLine( Pens.Black, x, y, x + width, y );
            gfx.DrawLine( Pens.Black, x, y, x, y + height );
            gfx.DrawLine( Pens.White, x, y + height, x + width, y + height );
            gfx.DrawLine( Pens.White, x + width, y, x + width, y + height );
          }
          
          // draw the hover style
          else if (button == m_hoverButton)
          {
            gfx.FillRectangle(Theme.SelectedBrush, x, y, width, height);
            gfx.DrawRectangle(Theme.SelectedPen, x, y, width, height);

            // if it is a two-part button draw a seprator
            if (button.Style == ToolbarButtonStyle.TwoPart)
              gfx.DrawLine(Theme.SelectedPen, x + m_buttonSize.Width, y, x + m_buttonSize.Width, y + height);
          }
          else
          {
            if(button is BlinkingToolbarButton)
            {
              using (Brush brush = new SolidBrush(((BlinkingToolbarButton)button).CurrentColor))
              {
                gfx.FillRectangle(brush, x, y, width, height);
              }
            }
          }

          gfx.DrawImage( button.Image, x + offset, y + offset, button.Image.Width, button.Image.Height );

          // draw the drop down arrow if it is a two part button
          if( button.Style == ToolbarButtonStyle.TwoPart )
          {
            const int arrowSize = 5;
            
            int diff = m_twoPartButtonSize.Width - m_buttonSize.Width;

            float startx = x + m_buttonSize.Width + ( ( diff - arrowSize ) / 2.0F );
            float starty = y + m_twoPartButtonSize.Height / 2.0F  - ( arrowSize / 2.0F );
            
            gfx.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            PointF[ ] arrow = new PointF[ 3 ];
            arrow[ 0 ] = new PointF( startx, starty );
            arrow[ 1 ] = new PointF( startx + arrowSize, starty );
            arrow[ 2 ] = new PointF( startx + arrowSize / 2.0F, starty + arrowSize );
            gfx.FillPolygon( Brushes.Black, arrow );
          }
        }
        else
          System.Windows.Forms.ControlPaint.DrawImageDisabled( gfx, button.Image, x + offset, y + offset, Color.Transparent );


        x += width + m_padding;
      }

      foreach( KeyValuePair< Control, string > pair in m_addedControls )
      {
        /// Draw a seperator before each control.
        int seperatorHeight = m_buttonSize.Height - (offset * 2);
        int seperatorWidth = 1;
        gfx.DrawLine(Pens.Black, x, y + offset, x + 1, y + offset);
        gfx.DrawLine(Pens.Black, x, y + offset, x, y + offset + seperatorHeight);
        gfx.DrawLine(Pens.White, x, y + offset + seperatorHeight, x + 1, y + offset + seperatorHeight);
        gfx.DrawLine(Pens.White, x + 1, y + offset, x + 1, y + offset + seperatorHeight);
        x += 2 + m_padding;
        x += pair.Key.Size.Width + m_padding;
      }
    } // OnPaint( args )


    /// <summary>
    /// Provides additional funtionality on button clicks.
    /// </summary>
    /// <param name="args">A ToolbarButtonClickEventArgs that contains the event data.</param>
    protected virtual void OnButtonClick( ToolbarButtonClickEventArgs args )
    {
    } // OnButtonClick( args )

    
    /// <summary>
    /// This member overrides Control.OnResize.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data. </param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Positions the labels containing the event handlers beneath the Toolbarbuttons.
    /// </summary>
    internal protected void RecalculateLayout()
    {   
      int width = 0;
      foreach( ToolbarButton button in m_buttons )
      {
        if( !button.Visible )
          continue;

        if( button.Style == ToolbarButtonStyle.TwoPart )
          width += m_twoPartButtonSize.Width + m_padding;
        else
          width += m_buttonSize.Width + m_padding;
      }

      // TODO: Fix tooltips
      m_tooltip.RemoveAll();
      this.Controls.Clear();

      int x = m_padding;
      int y = m_padding;
      //int offset = 2;

      foreach( ToolbarButton button in m_buttons )
      {
        if( !button.Visible )
          continue;

        if( button.Style == ToolbarButtonStyle.Separator )
        {
          x += 2 + m_padding;
          continue;
        }

        System.Windows.Forms.Label label = new System.Windows.Forms.Label();
        
        if( button.Style == ToolbarButtonStyle.TwoPart )
          label.Size = m_twoPartButtonSize;
        else
          label.Size = m_buttonSize;
        
        label.Location = new Point( x, y );
        label.BackColor = Color.Transparent;
        label.MouseDown += m_labelMouseDownHandler;
        label.MouseEnter += m_labelMouseEnterHandler;
        label.MouseLeave += m_labelMouseLeaveHandler;
        label.MouseUp += m_labelMouseUpHandler;
        
        this.Controls.Add( label );
        
        m_tooltip.SetToolTip( label, button.Tooltip );

        if( button.Style == ToolbarButtonStyle.TwoPart )
          x += m_twoPartButtonSize.Width + m_padding;
        else
          x += m_buttonSize.Width + m_padding;
      }

      foreach( KeyValuePair< Control, string > pair in m_addedControls )
      {
        width += pair.Key.Size.Width + m_padding;
        m_tooltip.SetToolTip( pair.Key, pair.Value );

        pair.Key.Location = new System.Drawing.Point( x, m_padding );

        this.Controls.Add( pair.Key );
      }

      int height = m_buttonSize.Height + 2 * m_padding;
      this.Size = new Size( width , height );

    } // RecalculateLayout()

    
    /// <summary>
    /// Determines which Toolbarbutton is clicked.
    /// </summary>
    /// <returns>The ToolbarButton that is clicked; otherwise null</returns>
    private ToolbarButton HitTest()
    {
      Point position = PointToClient( System.Windows.Forms.Control.MousePosition );

      Rectangle bounds = new Rectangle( m_padding, m_padding, m_buttonSize.Width, m_buttonSize.Height );
      
      foreach( ToolbarButton button in m_buttons )
      {
        if( !button.Visible )
          continue;

        if( button.Style == ToolbarButtonStyle.Separator )
        {
          bounds.Offset( 2 + m_padding, 0 );
          continue;
        }

        if( button.Style == ToolbarButtonStyle.TwoPart )
          bounds.Size = m_twoPartButtonSize;
        else
          bounds.Size = m_buttonSize;


        if( bounds.Contains( position ) )
        {
          if( button.Enabled )
            return button;
          break;
        }

        if( button.Style == ToolbarButtonStyle.TwoPart )
          bounds.Offset( m_twoPartButtonSize.Width + m_padding, 0 );
        else
          bounds.Offset( m_buttonSize.Width + m_padding, 0 );
      }
      return null;
    } // HitTest()


    /// <summary>
    /// Gets the piece of a two part button that was cliecked
    /// </summary>
    /// <param name="bounds">Button bounds</param>
    /// <returns>Piece that was clicked</returns>
    private TwoPartButtonPiece GetTwoPartPiece( Rectangle bounds )
    {
      int diff = m_twoPartButtonSize.Width - m_buttonSize.Width;

      Point pos = PointToClient( System.Windows.Forms.Control.MousePosition );
      if( pos.X >= ( bounds.Right - diff ) && pos.X < bounds.Right )
        return TwoPartButtonPiece.Menu;

      return TwoPartButtonPiece.Button;
    }
    
    
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the mouse down event.
    /// </summary>
    /// <param name="sender">The sender of the event.</param>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    private void OnLabelMouseDownHandler( object sender, System.Windows.Forms.MouseEventArgs args  )
    {
      ToolbarButton button = null;

      // if there isn't a hover button, check if there was a clicked button
      if( m_hoverButton == null )
      {
        button = HitTest();
        if( button == null )
          return;

        m_hoverButton = button;
      }
      
      m_downButton = m_hoverButton;
      
      if( m_downButton.Style == ToolbarButtonStyle.TwoPart )
      {
        Rectangle bounds = ((System.Windows.Forms.Label)sender).Bounds;

        if( GetTwoPartPiece( bounds ) == TwoPartButtonPiece.Menu )
        {
          m_downButtonPrevState = m_downButton.Pushed;
          m_downButton.Pushed = true;
          Refresh();

          System.Windows.Forms.Label label = (System.Windows.Forms.Label) sender;
          try
          {
            label.MouseEnter -= m_labelMouseEnterHandler;
            label.MouseLeave -= m_labelMouseLeaveHandler;
            
            m_downButton.DropDownMenu.Show( this,  new Point( bounds.Right, bounds.Bottom ), ContextMenuAlignment.Top | ContextMenuAlignment.Right );
          }
          finally
          {
            if( !m_downButtonPrevState )
            {
              m_downButtonPrevState = true;
              m_downButton.Pushed = false;
            }
            m_downButton = null;
            if( !label.Bounds.Contains( PointToClient( System.Windows.Forms.Control.MousePosition ) ) )
              m_hoverButton = null;
            Refresh();
            
            label.MouseEnter += m_labelMouseEnterHandler;
            label.MouseLeave += m_labelMouseLeaveHandler;
          }
        }
        else
        {
          m_downButtonPrevState = m_downButton.Pushed;
          m_downButton.Pushed = !m_downButtonPrevState;
          Refresh();
        }
      }
      else
      {
        m_downButtonPrevState = m_downButton.Pushed;
        m_downButton.Pushed = !m_downButtonPrevState;

        Refresh();
      }
    
    } // OnLabelMouseDownHandler( sender, args )


    /// Handles the mouse enter event.
    /// </summary>
    /// <param name="sender">The sender of the event.</param>
    /// <param name="args">A EventArgs that contains the event data.</param>
    private void OnLabelMouseEnterHandler( object sender, EventArgs args )
    {    
      ToolbarButton button = HitTest();

      // reset the down buttons pushed state
      if( m_downButton != null && button == m_downButton )
      {
        m_downButton.Pushed = !m_downButtonPrevState;
        m_hoverButton = null;
      }
      else
        m_hoverButton = button;

      Invalidate();
    } // OnLabelMouseEnterHandler( sender, args )


    /// <summary>
    /// Handles the mouse leave event.
    /// </summary>
    /// <param name="sender">The sender of the event.</param>
    /// <param name="args">A EventArgs that contains the event data.</param>
    private void OnLabelMouseLeaveHandler( object sender, EventArgs args )
    {
      ToolbarButton button = HitTest();

      if( m_downButton != null && button == m_downButton )
      {

        if(  m_downButton.Style == ToolbarButtonStyle.TwoPart )
        {
        }
        else
        {   
          m_downButton.Pushed = m_downButtonPrevState;
        }
      }

      m_hoverButton = null;

      Invalidate();
    } // OnLabelMouseLeaveHandler( sender, args )


    /// <summary>
    /// Handles the mouse up event.
    /// </summary>
    /// <param name="sender">The sender of the event.</param>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    private void OnLabelMouseUpHandler( object sender, System.Windows.Forms.MouseEventArgs args )
    {
      try
      {
        if( m_downButton != null )
        {
          ToolbarButton button = HitTest();

          if( button == m_downButton )
          {
            OnButtonClick( new ToolbarButtonClickEventArgs( m_downButton ));
            if( m_downButton.Style == ToolbarButtonStyle.PushButton )
            {
              m_downButton.Pushed = false;
            }
          }
          else
            m_downButton.Pushed = m_downButtonPrevState;
        }
      }
      finally
      {
        m_downButton = null;
        m_downButtonPrevState = false;
        Invalidate();
      }
    } // OnLabelMouseUpHandler( sender, args )


  
    
    #endregion

  } // class Toolbar

  #endregion
  
  #region ToolbarButton class
  
  /// <summary>
  /// ToolbarButton object displayed by the Toolbar control.
  /// </summary>
  public class ToolbarButton
  {

    #region fields

    /// <summary>
    /// The parent of this button.
    /// </summary>
    private Control m_parent = null;

    /// <summary>
    /// Style displayed by the button.
    /// </summary>
    private ToolbarButtonStyle m_style = ToolbarButtonStyle.PushButton;

    /// <summary>
    /// The button's image.
    /// </summary>
    private Image m_image;

    /// <summary>
    /// The button's tooltip.
    /// </summary>
    private string m_tooltip = string.Empty;

    /// <summary>
    /// Determines if the button is in the pushed state.
    /// </summary>
    private bool m_pushed = false;

    /// <summary>
    /// Data that can be associated with the button.
    /// </summary>
    private object m_data = null;

    /// <summary>
    /// Determines if the button is enabled.
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// associated drop down menu
    /// </summary>
    private Viatronix.UI.ContextMenu m_dropDownMenu = null;

    /// <summary>
    /// button identifier
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// visibility of the button
    /// </summary>
    private bool m_visible = true;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a ToolbarButton with the specified style.
    /// </summary>
    /// <param name="style">The button's style.</param>
    public ToolbarButton( ToolbarButtonStyle style )
    {
      m_style = style;
    } // ToolbarButton( style )


    /// <summary>
    /// Creates anew instance of a ToolbarButton.
    /// </summary>
    public ToolbarButton()
    {
    } // ToolbarButton()

    #endregion

    #region properties

    /// <summary>
    /// The parent of this button.
    /// </summary>
    public Control Parent
    {
      get { return m_parent;  }
      set { m_parent = value; }
    } // Parent


    /// <summary>
    /// Gets or sets the ToolbarButtonStyle.
    /// </summary>
    public ToolbarButtonStyle Style
    {
      get { return m_style; }
      set { m_style = value; }
    } // Style


    /// <summary>
    /// Gets or sets the button's image.
    /// </summary>
    public Image Image
    {
      get { return m_image; }
      set { m_image = value; }
    } // Image


    /// <summary>
    /// Gets or sets the pushed state of the button.
    /// </summary>
    public bool Pushed
    {
      get { return m_pushed; }
      set { m_pushed = value; }
    } // Pushed

    
    /// <summary>
    /// gets or set the button's tooltip.
    /// </summary>
    public string Tooltip
    {
      get { return m_tooltip; }
      set { m_tooltip = value; }
    } // Tooltip

    
    /// <summary>
    /// Gets or sets the additional information associated with the button.
    /// </summary>
    public object Tag
    {
      get { return m_data; }
      set { m_data = value; }
    } // Tag


    /// <summary>
    /// Gets or set if the button is enabled.
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    } // Enabled

    
    /// <summary>
    /// Gets or sets the drop down menu
    /// </summary>
    public Viatronix.UI.ContextMenu DropDownMenu
    {
      get { return m_dropDownMenu; }
      set { m_dropDownMenu = value; }
    } // DropDownMenu


    /// <summary>
    /// Gets or sets the id
    /// </summary>
    public string ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID


    /// <summary>
    /// Gets or sets the visibility
    /// </summary>
    public bool Visible
    {
      get { return m_visible; }
      set { m_visible = value; }
    } // Visible
    
    #endregion

  } // class ToolbarButton

  #endregion

  #region BlinkingToolbarButton class

  /// <summary>
  /// Represents a toolbar button that blinks at a specified interval.
  /// </summary>
  public class BlinkingToolbarButton : ToolbarButton
  {

    #region fields

    /// <summary>
    /// The period between blinks (in milliseconds.)
    /// </summary>
    private int m_period = 350;

    /// <summary>
    /// The time the last blink went off.
    /// </summary>
    private DateTime m_lastBlinked = DateTime.Now;

    /// <summary>
    /// The initial color that shows up when the button is shown.
    /// </summary>
    private Color m_firstColor = Color.Transparent;

    /// <summary>
    /// The color that shows up after the blink event until the period ends.
    /// </summary>
    private Color m_secondColor = Color.Orange;

    /// <summary>
    /// The current color that is being shown.
    /// </summary>
    private Color m_currentColor = Color.Transparent;

    /// <summary>
    /// The timer that goes off every period to change the current color.
    /// </summary>
    private Timer m_timer = new Timer();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public BlinkingToolbarButton()
    {
      m_timer.Tick += new EventHandler( OnTimerTick );
      m_timer.Interval = m_period;
      m_timer.Start();
    } // BlinkingToolbarButton()

    #endregion

    #region properties

    /// <summary>
    /// Period between color changes (in milliseconds.)
    /// </summary>
    public int Period
    {
      get { return m_period;  }
      set { m_period = value; }
    } // Period


    /// <summary>
    /// The time the last blink occurred.
    /// </summary>
    public DateTime LastBlink
    {
      get { return m_lastBlinked;  }
      set { m_lastBlinked = value; }
    } // LastBlink


    /// <summary>
    /// The first color to be displayed.
    /// </summary>
    public Color FirstColor
    {
      get { return m_firstColor;  }
      set { m_firstColor = value; }
    } // FirstColor


    /// <summary>
    /// The color that is currently being displayed.
    /// </summary>
    public Color CurrentColor
    {
      get { return m_currentColor;  }
      set { m_currentColor = value; }
    } // CurrentColor

    #endregion

    #region methods

    /// <summary>
    /// Starts the blinking of the button.
    /// </summary>
    public void StartBlinking()
    {
      m_timer.Start();
    } // StartBlinking()

    /// <summary>
    /// Tells the timer to stop and changes the color back to the inital color.
    /// </summary>
    public void StopBlinking()
    {
      m_timer.Stop();

      m_currentColor = m_firstColor;

      if (Parent != null)
      {
        Parent.Invalidate();
      }
    } // StopBlinking()
    
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the timer tick event.
    /// </summary>
    /// <param name="sender">Origin of the signal.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTimerTick(object sender, EventArgs args)
    {
      m_lastBlinked = DateTime.Now;

      if (m_currentColor == m_firstColor)
      {
        m_currentColor = m_secondColor;
      }
      else
      {
        m_currentColor = m_firstColor;
      }

      if (Parent != null)
      {
        Parent.Invalidate();
      }
    } // OnTimerTick()

    #endregion

  } // BlinkingToolbarButton

  #endregion

  #region ToolbarButtonCollection class

  /// <summary>
  /// A colelctino of ToolbarButtons.
  /// </summary>
  public class ToolbarButtonCollection : CollectionBase
  {

    #region fields
    
    /// <summary>
    /// A reference to the parent toolbar.
    /// </summary>
    private Toolbar m_toolbar;
    
    #endregion

    #region construction
    
    /// <summary>
    /// Creates a new instance of the ToolbarButtonCollection with the specified parent Toolbar.
    /// </summary>
    /// <param name="toolbar">The parent Toolbar control.</param>
    public ToolbarButtonCollection( Toolbar toolbar )
    {
      m_toolbar = toolbar;
    } // ToolbarButtonCollection( toolbar )

    #endregion
    
    #region properties

    /// <summary>
    /// Indexer over all elements in the collection.
    /// </summary>
    public ToolbarButton this[ int index ]
    {
      get { return this.List[ index ] as ToolbarButton; }
      set { this.List[ index ] = value; }
    } // Indexer


    /// <summary>
    /// Indexer over all elements in the collection.
    /// </summary>
    public ToolbarButton this[ string id ]
    {
      get
      {
        foreach( ToolbarButton button in this.List )
        {
          if( button.ID == id )
            return button;
        }

        return null;
      }
    } // Indexer

    #endregion

    #region methods

    /// <summary>
    /// Adds a new ToolbarButton to the collection.
    /// </summary>
    /// <param name="image">The button's image.</param>
    /// <param name="tooltip">The button's tooltip.</param>
    /// <returns>The index in the collection.</returns>
    public int Add( Image image, string tooltip )
    {
      ToolbarButton button = new ToolbarButton();
      button.Image = image;
      button.Tooltip = tooltip;
      return Add( button );
    } // Add( image, tooltip )
    

    /// <summary>
    /// Adds a new ToolbarButton to the collection.
    /// </summary>
    /// <param name="button">The ToolbarButton.</param>
    /// <returns>The index in the collection.</returns>
    public int Add( ToolbarButton button )
    {
      return List.Add( button );
    } // Add( button )


    /// <summary>
    /// Adds a collection of ToolbarButtons.
    /// </summary>
    /// <param name="collection">The collection of ToolbarButtons.</param>
    public void AddRange( ICollection collection )
    {
      foreach( object obj in collection )
        this.List.Add( obj );
    } // AddRange( collection )


    /// <summary>
    /// Inserts the ToolbarButton at the specified index.
    /// </summary>
    /// <param name="index">The index to insert into.</param>
    /// <param name="button">The button to insert.</param>
    public void Insert(int index, ToolbarButton button)
    {
      List.Insert(index, button);
    } // Insert(int index, ToolbarButton button)

    
    #endregion

    #region override methods

    /// <summary>
    /// Verifies that the object being added is a ToolbarButton.
    /// </summary>
    /// <param name="value">The object to validate. </param>
    protected override void OnValidate( object value )
    {
      base.OnValidate( value );

      if( !( value is ToolbarButton ))
        throw new ArgumentException( "The specified object is not a Toolbarbutton." );
    } // OnValidate( value )


    /// <summary>
    /// Recalculate the layout of the parent toolbar control when all items are removed.
    /// </summary>
    protected override void OnClearComplete()
    {
      base.OnClearComplete();
      m_toolbar.RecalculateLayout();
    } // OnClearComplete()


    /// <summary>
    /// Recalculate the layout of the parent toolbar control when a new item is added.
    /// </summary>
    /// <param name="index">The zero-based index at which to insert value.</param>
    /// <param name="value">The new value of the element at index.</param>
    protected override void OnInsertComplete( int index, object value )
    {
      base.OnInsertComplete( index, value );
      m_toolbar.RecalculateLayout();
    } // OnInsertComplete( index, value )

    #endregion

  } // class ToolbarButtonCollection

  #endregion

  #region ToolbarButtonClickEventArgs class


  /// <summary>
  /// Provides data for the ButtonClick event.
  /// </summary>
  public class ToolbarButtonClickEventArgs : System.EventArgs
  {

    #region fields

    /// <summary>
    /// The ToolbarButton raising the event.
    /// </summary>
    ToolbarButton m_button;
    
    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a ToolbarButtonEventArgs with the specified ToolbarButton.
    /// </summary>
    /// <param name="button">The ToolbarButton which raised the event.</param>
    public ToolbarButtonClickEventArgs( ToolbarButton button )
    {
      m_button = button;
    } // ToolbarButtonClickEventArgs( button )

    
    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the ToolbarButton which raised the event.
    /// </summary>
    public ToolbarButton Button
    {
      get { return m_button; }
      set { m_button = value; }
    } // Button

    #endregion

  } // class ToolbarButtonClickEventArgs

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: Toolbar.cs,v $
// Revision 1.8.2.4  2009/03/27 21:37:11  gdavidson
// Added tool tips
//
// Revision 1.8.2.3  2009/03/25 17:27:13  kchalupa
// Added code to create blinking toolbar buttons.
//
// Revision 1.8.2.2  2009/03/24 20:45:33  kchalupa
// Added ability to add System.Windows.Forms.Control objects along with the buttons.
//
// Revision 1.8.2.1  2009/01/29 18:11:20  kchalupa
// Issue #6211: Findings button should not show up in XP or others, only Cardiac.
//
// Revision 1.8  2007/06/15 21:08:56  gdavidson
// Modified the RecalculateLayout method
//
// Revision 1.7  2007/05/17 18:17:00  gdavidson
// Added ID and Visible properties to ToolbarButton
//
// Revision 1.6  2005/11/18 21:08:03  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.2  2005/08/16 18:46:53  geconomos
// Fixed click issue
//
// Revision 1.5.2.1  2005/08/11 19:51:13  geconomos
// Added "two part" button style
//
// Revision 1.5  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4.2.3  2005/02/28 16:22:55  gdavidson
// Added comments
//
// Revision 1.4.2.2  2005/02/10 17:04:23  geconomos
// Issue #3921: Fixed issues with disabled toolbar buttons.
//
// Revision 1.4.2.1  2005/02/10 16:12:42  gdavidson
// Issue #3890: Added mouse enter and leave events and changed functionality for mouse move and mouse up events.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Toolbar.cs,v 1.8.2.4 2009/03/27 21:37:11 gdavidson Exp $
// $Id: Toolbar.cs,v 1.8.2.4 2009/03/27 21:37:11 gdavidson Exp $

#endregion