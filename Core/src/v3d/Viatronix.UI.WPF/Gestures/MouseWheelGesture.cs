using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace Viatronix.UI.Gestures
{

  #region enums

  /// <summary>
  /// Mouse Wheel Action enumeratoer
  /// </summary>
  public enum MouseWheelAction
  {
    AllMovement,        // All movement (Up.Down)
    WheelUp,            // Only Up 
    WheelDown           // Only Down
  } // enum MouseWheelAction

  #endregion


  #region class MouseWheelGesture

  /// <summary>
  /// Implements a mouse wheel gesture used to determine if the wheel was moved up or down.
  /// </summary>
  public class MouseWheelGesture : MouseGesture
  {
    #region fields

    /// <summary>
    /// Mouse wheel action
    /// </summary>
    private MouseWheelAction m_mouseWheelAction;

    #endregion

    #region constructors

 
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="action"></param>
    public MouseWheelGesture() : base(MouseAction.WheelClick)
    {
      m_mouseWheelAction = MouseWheelAction.AllMovement;
    } // MouseWheelGesture()

    #endregion

    #region properties

    /// <summary>
    /// Gets/Sets the mouse wheel action for this object
    /// </summary>
    public MouseWheelAction MouseWheelAction
    {
      get { return m_mouseWheelAction; }
      set { m_mouseWheelAction = value; }
    } // MouseWheelAction

    #endregion

    #region methods

    /// <summary>
    /// Overrides the Matches method to determine of the event matches the gesture
    /// </summary>
    /// <param name="targetElement">Target element </param>
    /// <param name="inputEventArgs">Inpur arguments (MouseWheelEventArgs)</param>
    /// <returns></returns>
    public override bool Matches(object targetElement, InputEventArgs inputEventArgs)
    {
      // ========================================================
      // Make sure the input binding matches the gesture/action
      // ========================================================
      if (base.Matches(targetElement, inputEventArgs))
      {

        //
        MouseWheelEventArgs mouseWheelEventArgs = inputEventArgs as MouseWheelEventArgs;

        // ======================================
        // Make sure the arguement is not NULL
        // =====================================
        if (mouseWheelEventArgs != null)
        {
          // =======================================================================================
          // Check for a match of the wheel event and the mouse wheel action set for this gesture
          // =======================================================================================
          if ( MouseWheelAction == MouseWheelAction.AllMovement ||
              (MouseWheelAction == MouseWheelAction.WheelDown && mouseWheelEventArgs.Delta < 0) ||
              (MouseWheelAction == MouseWheelAction.WheelUp && mouseWheelEventArgs.Delta > 0))
          {
              return true;
          } // END ... If the action matches the wheel event

        } // END .. If the mouse wheel arguements are not null

      } // END ... If the gesture matches the binding

      /// Doesn't match
      return false;

    } // Matches(object targetElement, InputEventArgs inputEventArgs)

    #endregion

  };  // MouseWheelGesture

  #endregion

} // namespace Viatronix.Utilities.Dicom.Viewer.Gesture

