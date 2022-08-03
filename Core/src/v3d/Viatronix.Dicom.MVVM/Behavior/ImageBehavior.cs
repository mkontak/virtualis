using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;


namespace Viatronix.Dicom.MVVM.Behavior
{

  public static class ImageBehavior 
  {

    #region KeyDown Command

    /// <summary>
    /// KeyDown COmmand property
    /// </summary>
    public static readonly DependencyProperty KeyDownCommandProperty =
            DependencyProperty.RegisterAttached
            (
                "KeyDownCommand",
                typeof(ICommand),
                typeof(ImageBehavior),
                new PropertyMetadata(OnKeyDown)
            );

    /// <summary>
    /// Gets the KeyDown Command for the specified UIElement
    /// </summary>
    /// <param name="obj">UIElement</param>
    /// <returns>KeyDown COmmand</returns>
    public static ICommand GetKeyDownCommand(UIElement obj)
    {
      return (ICommand)obj.GetValue(KeyDownCommandProperty);
    } //  GetKeyDownCommand(UIElement obj)

    /// <summary>
    /// Sets the KeyDown command for the specified UIElement
    /// </summary>
    /// <param name="obj">UIElement</param>
    /// <param name="command">KeyDown Command</param>
    public static void SetKeyDownCommand(UIElement obj, ICommand command)
    {
      obj.SetValue(KeyDownCommandProperty, command);
    } // SetKeyDownCommand(UIElement obj, ICommand command)

    /// <summary>
    /// KeyDown Command Handler
    /// </summary>
    /// <param name="d">Dependency Object</param>
    /// <param name="e">Evenet Args</param>
    private static void OnKeyDown(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
      UIElement obj = d as UIElement;
      ICommand command = GetKeyDownCommand(obj);

      obj.KeyDown += (sender, e1) =>
      {
        command.Execute(e1);

        e1.Handled = true;
      };
    } // OnKeyDown(DependencyObject d, DependencyPropertyChangedEventArgs e)

    #endregion


    #region KeyUp Command

    /// <summary>
    /// Dependency property for the KeyUp Command
    /// </summary>
    public static readonly DependencyProperty KeyUpCommandProperty =
         DependencyProperty.RegisterAttached
         (
             "KeyUpCommand",
             typeof(ICommand),
             typeof(ImageBehavior),
             new PropertyMetadata(OnKeyUp)
         );

    /// <summary>
    /// Gets the KeyUpCommand for the specified UIElement
    /// </summary>
    /// <param name="obj">UIElement</param>
    /// <returns></returns>
    public static ICommand GetKeyUpCommand(UIElement obj)
    {
      return (ICommand)obj.GetValue(KeyUpCommandProperty);
    } // GetKeyUpCommand(UIElement obj)

    /// <summary>
    /// Sets the KeyUpCommand for the specified UIElement
    /// </summary>
    /// <param name="obj">UIElement</param>
    /// <param name="command">KeyUp Command</param>
    public static void SetKeyUpCommand(UIElement obj, ICommand command)
    {
      obj.SetValue(KeyUpCommandProperty, command);
    } // SetKeyUpCommand(UIElement obj, ICommand command)



    /// <summary>
    /// KeyUp Handler
    /// </summary>
    /// <param name="d">Dependency object</param>
    /// <param name="e">Changed evenet args</param>
    private static void OnKeyUp(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
      UIElement obj = d as UIElement;
      ICommand command = GetKeyUpCommand(obj);

      obj.KeyUp += (sender, e1) =>
      {
        command.Execute(e1);
        e1.Handled = true;
      };
    } // OnKeyUp(DependencyObject d, DependencyPropertyChangedEventArgs e)


    #endregion


  } // class ImageBehavior




} // namespace Viatronix.Utilities.Dicom.Viewer.Behavior
