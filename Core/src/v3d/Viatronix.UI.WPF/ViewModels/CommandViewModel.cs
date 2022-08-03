using System;
using System.Windows.Input;
using System.Windows.Media;
using System.Drawing;
using System.Windows;
using GalaSoft.MvvmLight.Command;
using Viatronix.UI;

namespace Viatronix.UI.ViewModels
{
    /// <summary>
    /// Represents an actionable item displayed by a View.
    /// </summary>
    public class CommandViewModel : ViewModelBase
    {
        public CommandViewModel(string displayName, ICommand command, Bitmap bitmap):
            this(displayName, command, Utilities.ToBitmapSource(bitmap))
        {
            TextVisibility = Visibility.Visible;
        }

        public CommandViewModel(string displayName, ICommand command, ImageSource image):this(displayName,command)
        {
            SmallImageSource = image;
        }

        public CommandViewModel(string displayName, ICommand command)
        {
            if (command == null)
                throw new ArgumentNullException("command");

            base.DisplayName = displayName;
            this.Command = command;
        }

        public virtual string ToolTipTitle { get; protected set; }

        public virtual string ToolTipDescription { get; protected set; }
        
        public virtual ImageSource SmallImageSource { get; protected set; }
        
        public virtual ImageSource LargeImageSource { get; protected set; }
        public virtual Visibility TextVisibility { get; set; }
        
        public ICommand Command { get; private set; }
    }
}