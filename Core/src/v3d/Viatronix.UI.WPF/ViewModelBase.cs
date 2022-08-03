using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight.Messaging;

namespace Viatronix.UI
{
  public class ViewModelBase : GalaSoft.MvvmLight.ViewModelBase
  {
    /// <summary>
    /// Default constructor
    /// </summary>
    public ViewModelBase() : base( new Messenger() )
    {
    }


    #region properties

    public IMessenger LocalMessenger
    {
      get { return this.MessengerInstance; }
    }

    #endregion


    /// <summary>
    /// Helper method which changes the value of a property and raises it's PropertyChanged event
    /// </summary>
    /// <typeparam name="T">property type</typeparam>
    /// <param name="property">property's name</param>
    /// <param name="newValue">new value</param>
    /// <param name="oldValue">current value</param>
    protected bool SetProperty< T >( string property, T newValue, ref T currentValue )
    {
      if( !object.Equals( currentValue, newValue ))
      {
        currentValue = newValue;
        RaisePropertyChanged( property );
        return true;
      }

      return false;
    } // SetProperty< T >( property, newValue, ref currentValue )


    /// <summary>
    /// Helper method which resolves Services
    /// </summary>
    /// <typeparam name="T">Service type</typeparam>
    /// <returns>service</returns>
    protected T GetService< T >() where T : class
    {
      return ServiceContainer.Instance.GetService< T >();
    } // GetService< T >()

  } // class ViewModelBase
} // namespace Viatronix.Console
