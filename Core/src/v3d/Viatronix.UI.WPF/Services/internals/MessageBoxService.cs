// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace Viatronix.UI.Services
{
	internal class MessageBoxService : IMessageBoxService
	{

    #region IMessageBoxService Members

    public MessageBoxResult Show( string message )
    {
      return MessageBox.Show( Application.Current.MainWindow, message );
    } // Show( message )


    public void ShowError( string message, string caption )
    {
      MessageBox.Show( Application.Current.MainWindow, message, caption, MessageBoxButton.OK, MessageBoxImage.Error );
    }


    public bool ShowQuestion( string message, string caption )
    {
      MessageBoxResult result = MessageBox.Show( Application.Current.MainWindow, message, caption, MessageBoxButton.YesNo, MessageBoxImage.Question );
      return ( result == MessageBoxResult.Yes );
    }

    #endregion

  } // class MessageBoxService
} // namespace Viatronix.UI.Services
