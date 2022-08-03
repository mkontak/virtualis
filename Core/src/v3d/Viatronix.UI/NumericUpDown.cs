// $Id: NumericUpDown.cs,v 1.2 2001/01/02 03:44:25 frank Exp $
//
// Copyright (c) 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Globalization;

namespace Viatronix.UI
{
	/// <summary>
	/// NumericUpDown control with validation.
	/// </summary>
	public class NumericUpDown : System.Windows.Forms.NumericUpDown
	{

    #region events

    /// <summary>
    /// Occurs when the control is valid.
    /// </summary>
    public event EventHandler Valid = null;

    /// <summary>
    /// Occurs when the change to the control is valid.
    /// </summary>
    public event EventHandler ValidChange = null;

    /// <summary>
    /// Occurs when the control is invalid.
    /// </summary>
    public event EventHandler Invalid = null;

    /// <summary>
    /// Occurs when the change to the control is invalid.
    /// </summary>
    public event EventHandler InvalidChange = null;

    #endregion

    #region fields

    /// <summary>
    /// Determines if the control is valid.
    /// </summary>
    private bool m_isValid = true;

    #endregion

    #region properties

    /// <summary>
    /// Return if the control is valid.
    /// </summary>
    public bool IsValid
    {
      get { return m_isValid; }
    } // IsValid


    /// <summary>
    /// Returns the text for the control.
    /// </summary>
    private new string Text 
    {
      get { return ((UpDownBase) this).Text; }
    } // Text


    /// <summary>
    /// Returns the current value of the control.
    /// </summary>
    private decimal CurrentValue
    {
      get { return decimal.Parse( this.Text, this.NumberStyles ); }
    } // CurrentValue


    /// <summary>
    /// Gets or sets the style of the numbers.
    /// </summary>
    private NumberStyles NumberStyles
    {
      get
      {
        if( this.DecimalPlaces == 0 )
          return NumberStyles.Integer & NumberStyles.AllowThousands;
        else
          return NumberStyles.Number;
      }
    } // NumberStyles

    #endregion

    #region override methods
  
    /// <summary>
    /// This member overrides Control.OnKeyUp.
    /// </summary>
    /// <param name="args">A KeyEventArgs that contains the event data.</param>
    protected override void OnKeyUp( KeyEventArgs args )
    {
      m_isValid = CheckValue( true );

      base.OnKeyUp( args );

      if( this.IsValid )
        OnValidChange();
      else
        OnInvalidChange();      
    } // OnKeyUp( args )


    /// <summary>
    /// This member overrides Control.OnValidating.
    /// </summary>
    /// <param name="args">A CancelEventArgs that contains the event data.</param>
    protected override void OnValidating( CancelEventArgs args )
    {
      m_isValid = CheckValue( false );
      args.Cancel = !m_isValid;

      base.OnValidating( args );
    } // OnValidating( args )


    /// <summary>
    /// This member overrides Control.OnLeave.
    /// </summary>
    /// <param name="args">A EventArgs that contains the event data.</param>
    protected override void OnLeave( EventArgs args )
    {
      base.OnLeave( args );

      // raise the Valid or Invalid event
      if( this.IsValid && this.Valid != null )
        this.Valid( this, EventArgs.Empty );
      else if( !this.IsValid && this.Invalid != null )
        this.Invalid( this, EventArgs.Empty );
    }


    /// <summary>
    /// Adds validation to the base.ValidateEditText.
    /// </summary>
    protected override void ValidateEditText()
    {
      m_isValid = CheckValue( false );

      base.ValidateEditText();
    } // ValidateEditText()


    #endregion

    #region virtual methods

    /// <summary>
    /// Raises the ValidChange event.
    /// </summary>
    protected virtual void OnValidChange()
    {
      m_isValid = true;
      if( this.ValidChange != null )
        this.ValidChange( this, EventArgs.Empty );
    } // OnValidChange()


    /// <summary>
    /// Raises the InvalidChange event.
    /// </summary>
    protected virtual void OnInvalidChange()
    {
      m_isValid = false;
      if( this.InvalidChange != null )
        this.InvalidChange( this, EventArgs.Empty );
    } // OnInvalidChange()


    /// <summary>
    /// Raises the Valid event.
    /// </summary>
    protected virtual void OnValid()
    {
      m_isValid = true;
      if( this.Valid != null )
        this.Valid( this, EventArgs.Empty );
    } // OnValid()


    /// <summary>
    /// Raises the Invalid event.
    /// </summary>
    protected virtual void OnInvalid()
    {
      m_isValid = false;
      if( this.Invalid != null )
        this.Invalid( this, EventArgs.Empty );
    } // OnInvalid()

    #endregion

    #region private methods

    /// <summary>
    /// Checks that the controls value is valid.
    /// </summary>
    /// <param name="setValue">true if the controls value should be set; otherwise false</param>
    /// <returns>true if the value is valid; otherwise false</returns>
    private bool CheckValue( bool setValue )
    {
      try
      {
        decimal num = this.CurrentValue;
        if( num >= this.Minimum && num <= this.Maximum )
        {
          if( setValue )
            this.Value = num;

          return true;
        }

        return false;
      }
      catch( Exception )
      {
        return false;
      }
    } // CheckValue( setValue )

    #endregion

	} // class NumericUpDown
} // namespace Viatronix.UI

#region revision history

// $Log: NumericUpDown.cs,v $
// Revision 1.2  2001/01/02 03:44:25  frank
// initial version
//
// Revision 1.1.2.3  2005/02/28 18:35:13  gdavidson
// Added comments
//
// Revision 1.1.2.2  2005/02/09 17:10:18  gdavidson
// Issue #3893: Modified control to update state as the text changes.
//
// Revision 1.1.2.1  2005/02/08 19:02:07  gdavidson
// Issue #3893: New control with bounds checking
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/NumericUpDown.cs,v 1.2 2001/01/02 03:44:25 frank Exp $
// $Id: NumericUpDown.cs,v 1.2 2001/01/02 03:44:25 frank Exp $

#endregion
