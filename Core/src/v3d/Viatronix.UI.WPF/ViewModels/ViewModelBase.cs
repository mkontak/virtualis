using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Linq.Expressions;
using System.ComponentModel;
using GalaSoft.MvvmLight.Messaging;

namespace Viatronix.UI.ViewModels
{

    /// <summary>
    /// Base class for all ViewModel classes in the application.
    /// It provides support for property change notifications 
    /// and has a DisplayName property.  This class is abstract.
    /// </summary>
    public abstract class ViewModelBase : GalaSoft.MvvmLight.ViewModelBase, IDisposable
    {
        #region Constructor

        protected ViewModelBase() : base(new Messenger())
        {
        }

        #endregion // Constructor


        /// <summary>
        /// Helper method which resolves Services
        /// </summary>
        /// <typeparam name="T">Service type</typeparam>
        /// <returns>service</returns>
        protected T GetService<T>() where T : class
        {
          return ServiceContainer.Instance.GetService<T>();
        } // GetService< T >()

        #region DisplayName

        /// <summary>
        /// Returns the user-friendly name of this object.
        /// Child classes can set this property to a new value,
        /// or override it to determine the value on-demand.
        /// </summary>
        public virtual string DisplayName { get; protected set; }

        #endregion // DisplayName

        #region properties

        public IMessenger LocalMessenger
        {
          get { return this.MessengerInstance; }
        }

        #endregion


        #region IDisposable Members

        /// <summary>
        /// Invoked when this object is being removed from the application
        /// and will be subject to garbage collection.
        /// </summary>
        public void Dispose()
        {
            this.OnDispose();
        }

        /// <summary>
        /// Child classes can override this method to perform 
        /// clean-up logic, such as removing event handlers.
        /// </summary>
        protected virtual void OnDispose()
        {
        }

#if DEBUG
        /// <summary>
        /// Useful for ensuring that ViewModel objects are properly garbage collected.
        /// </summary>
        ~ViewModelBase()
        {
            string msg = string.Format("{0} ({1}) ({2}) Finalized", this.GetType().Name, this.DisplayName, this.GetHashCode());
            System.Diagnostics.Debug.WriteLine(msg);
        }
#endif

        #endregion // IDisposable Members
    }
}