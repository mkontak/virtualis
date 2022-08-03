using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.Console
{
  public class ViewGroups : System.Windows.Forms.UserControl,  Viatronix.UI.IView
  {
    #region IView

    /// <summary>
    /// Called when the view is initially created.
    /// </summary>
    /// <param name="xml">Additional xml configuration data</param>
    public void Initialize( string xml )
    {
    } // Initialize( string xml )

    #endregion

    #region fields
    
    /// <summary>
    /// component container
    /// </summary>
    private System.ComponentModel.Container components = null;

    #endregion

    #region methods

    /// <summary>
    /// Initializes a ViewGroups instance with default values
    /// </summary>
    public ViewGroups()
    {
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      
      InitializeComponent();
    } // ViewGroups()

		
    /// <summary>
    /// Releases unmanaged and optionally managed resources
    /// </summary>
    /// <param name="disposing">true if managed resources are to be released</param>
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
    } // Dispose( bool disposing )

    
    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      components = new System.ComponentModel.Container();
    }//InitializeComponent()

    #endregion
  } // class ViewGroups
} // namespace Viatronix.Console
