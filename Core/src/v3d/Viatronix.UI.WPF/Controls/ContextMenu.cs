using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows;
using System.Windows.Markup;

namespace Viatronix.UI
{
  public class ContextMenu : System.Windows.Controls.ContextMenu, INameScope
  {

    #region fields

    private Dictionary<string, object> m_items = new Dictionary<string, object>();

    #endregion

    #region INameScope Members

    object INameScope.FindName( string name ) 
    { 
      return m_items[name]; 
    } 
 
    void INameScope.RegisterName( string name, object scopedElement ) 
    { 
      m_items.Add(name, scopedElement); 
    } 
 
    void INameScope.UnregisterName( string name ) 
    { 
      m_items.Remove(name); 
    } 
 
    #endregion

  }
}
