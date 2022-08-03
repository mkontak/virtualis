/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:Viatronix.Console.Tests.App.ViewModel"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
  
  OR (WPF only):
  
  xmlns:vm="clr-namespace:Viatronix.Console.Tests.App.ViewModel"
  DataContext="{Binding Source={x:Static vm:ViewModelLocatorTemplate.ViewModelNameStatic}}"
*/

using System;
using System.Collections.Generic;

namespace Viatronix.UI.ViewModels
{
  /// <summary>
  /// This class contains static references to all the view models in the
  /// application and provides an entry point for the bindings.
  /// <para>
  /// Use the <strong>mvvmlocatorproperty</strong> snippet to add ViewModels
  /// to this locator.
  /// </para>
  /// <para>
  /// In Silverlight and WPF, place the ViewModelLocatorTemplate in the App.xaml resources:
  /// </para>
  /// <code>
  /// &lt;Application.Resources&gt;
  ///     &lt;vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:Viatronix.Console.Tests.App.ViewModel"
  ///                                  x:Key="Locator" /&gt;
  /// &lt;/Application.Resources&gt;
  /// </code>
  /// <para>
  /// Then use:
  /// </para>
  /// <code>
  /// DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
  /// </code>
  /// <para>
  /// You can also use Blend to do all this with the tool's support.
  /// </para>
  /// <para>
  /// See http://www.galasoft.ch/mvvm/getstarted
  /// </para>
  /// <para>
  /// In <strong>*WPF only*</strong> (and if databinding in Blend is not relevant), you can delete
  /// the Main property and bind to the ViewModelNameStatic property instead:
  /// </para>
  /// <code>
  /// xmlns:vm="clr-namespace:Viatronix.Console.Tests.App.ViewModel"
  /// DataContext="{Binding Source={x:Static vm:ViewModelLocatorTemplate.ViewModelNameStatic}}"
  /// </code>
  /// </summary>
  public class ViewModelLocator
  {

    #region fields

    private static Dictionary< string, ViewModelBase > m_stringMap = new Dictionary< string, ViewModelBase >();

    #endregion

    #region construction

    /// <summary>
    /// Initializes a new instance of the ViewModelLocator class.
    /// </summary>
    public ViewModelLocator()
    {
      ////if (ViewModelBase.IsInDesignModeStatic)
      ////{
      ////    // Create design time view models
      ////}
      ////else
      ////{
      ////    // Create run time view models
      ////}
    } // ViewModelLocator()

    #endregion

    #region properties

    public ViewModelBase this[ string id ]
    {
      get
      {
        ViewModelBase viewModel = null;
        m_stringMap.TryGetValue( id, out viewModel );

        return viewModel;
      }
    } // Indexer

    #endregion

    #region methods

    public static T Get<T>( string id ) where T : ViewModelBase
    {
      ViewModelBase viewModel = null;
      m_stringMap.TryGetValue( id, out viewModel );

      return viewModel as T;
    } // Get<T>( id )


    public static ViewModelBase Get( string id )
    {
      ViewModelBase viewModel = null;
      m_stringMap.TryGetValue( id, out viewModel );

      return viewModel;
    } // Get( id )



    public static void Add( string name, ViewModelBase viewModel )
    {
      if( !m_stringMap.ContainsKey( name ))
        m_stringMap.Add( name, viewModel );
    } // Add( viewModel )


    /// <summary>
    /// Cleans up all the resources.
    /// </summary>
    public static void Cleanup()
    {
      foreach( ViewModelBase viewModel in m_stringMap.Values )
        viewModel.Cleanup();

      m_stringMap.Clear();
    } // Cleanup()

    #endregion
 
  } // class ViewModelLocator
} // namespace Viatronix.Console