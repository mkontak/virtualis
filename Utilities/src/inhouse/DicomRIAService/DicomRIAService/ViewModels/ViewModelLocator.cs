/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocator xmlns:vm="clr-namespace:DicomRIAService.Views"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
*/

using DicomRIAService.ViewModels;

namespace DicomRIAService
{
  /// <summary>
  /// This class contains static references to all the view models in the
  /// application and provides an entry point for the bindings.
  /// <para>
  /// Use the <strong>mvvmlocatorproperty</strong> snippet to add ViewModels
  /// to this locator.
  /// </para>
  /// <para>
  /// In Silverlight and WPF, place the ViewModelLocator in the App.xaml resources:
  /// </para>
  /// <code>
  /// &lt;Application.Resources&gt;
  ///     &lt;vm:ViewModelLocator xmlns:vm="clr-namespace:DicomRIAService.Views"
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
  /// </summary>
  public class ViewModelLocator
  {
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
    }

    public static void Cleanup()
    {
       ClearBrowserViewModel();
    }


    private static BrowserViewModel m_browserViewModel;

    /// <summary>
    /// Gets the BrowserViewModel property.
    /// </summary>
    public static BrowserViewModel BrowserViewModelStatic
    {
      get
      {
        if( m_browserViewModel == null )
        {
          CreateBrowserViewModel();
        }

        return m_browserViewModel;
      }
    }

    /// <summary>
    /// Gets the BrowserViewModel property.
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage( "Microsoft.Performance",
        "CA1822:MarkMembersAsStatic",
        Justification = "This non-static member is needed for data binding purposes." )]
    public BrowserViewModel BrowserViewModel
    {
      get
      {
        return BrowserViewModelStatic;
      }
    }

    /// <summary>
    /// Provides a deterministic way to delete the BrowserViewModel property.
    /// </summary>
    public static void ClearBrowserViewModel()
    {
      m_browserViewModel.Cleanup();
      m_browserViewModel = null;
    }

    /// <summary>
    /// Provides a deterministic way to create the BrowserViewModel property.
    /// </summary>
    public static void CreateBrowserViewModel()
    {
      if( m_browserViewModel == null )
      {
        m_browserViewModel = new BrowserViewModel();
      }
    }


  }
}