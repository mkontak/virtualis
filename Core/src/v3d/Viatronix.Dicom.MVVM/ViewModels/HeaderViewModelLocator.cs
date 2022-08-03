/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:HeaderViewer.ViewModel"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
*/

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Ioc;
using Microsoft.Practices.ServiceLocation;
using Viatronix.Dicom.MVVM.Model;


namespace Viatronix.Dicom.MVVM.ViewModels
{
  /// <summary>
  /// This class contains static references to all the view models in the
  /// application and provides an entry point for the bindings.
  /// <para>
  /// See http://www.galasoft.ch/mvvm
  /// </para>
  /// </summary>
  public class HeaderViewModelLocator
  {
    static HeaderViewModelLocator()
    {
      ServiceLocator.SetLocatorProvider(() => SimpleIoc.Default);

      if (ViewModelBase.IsInDesignModeStatic)
      {
        SimpleIoc.Default.Register<IDataService, Design.DesignDataService>();
      }
      else
      {
        SimpleIoc.Default.Register<IDataService, DataService>();
      }

      SimpleIoc.Default.Register<HeaderViewModel>();
    }

    /// <summary>
    /// Gets the Main property.
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
        "CA1822:MarkMembersAsStatic",
        Justification = "This non-static member is needed for data binding purposes.")]
    public HeaderViewModel Main
    {
      get
      {
        return ServiceLocator.Current.GetInstance<HeaderViewModel>();
      }
    }

    /// <summary>
    /// Cleans up all the resources.
    /// </summary>
    public static void Cleanup()
    {
    }
  }
}