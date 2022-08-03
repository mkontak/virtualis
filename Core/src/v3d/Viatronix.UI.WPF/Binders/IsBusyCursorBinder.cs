using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Drawing;

namespace Viatronix.UI.Binders
{

  public class IsBusyCursorBinder
  {

    public static readonly DependencyProperty BindTargetProperty =
        DependencyProperty.RegisterAttached("BindTarget", typeof(bool), typeof(IsBusyCursorBinder), new PropertyMetadata(false, OnBindTargetChanged));

    /// <summary>
    /// Gets the binding target
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    public static bool GetBindTarget(DependencyObject obj) 
    { 
      return (bool)obj.GetValue(BindTargetProperty); 
    }

    /// <summary>
    /// Sets the binding target
    /// </summary>
    /// <param name="obj"></param>
    /// <param name="value"></param>
    public static void SetBindTarget(DependencyObject obj, bool value) 
    { 
      obj.SetValue(BindTargetProperty, value); 
    }

    /// <summary>
    /// Event handler for the bind target changed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private static void OnBindTargetChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
    {
      // sender is the FrameworkElement
      FrameworkElement element = sender as FrameworkElement;

      // ================================================================
      // Make sure the element is not null before settingt the cursor
      //  The event args NewValue is the IsBusy flag and if true
      //  the cursor should be a wait cursor otherwise it is the
      //  default arrow.
      // ===============================================================
      if (element != null)
        element.Cursor = ((bool)e.NewValue ? System.Windows.Input.Cursors.Wait : System.Windows.Input.Cursors.Arrow);
     

 
      
    }
  }
}
