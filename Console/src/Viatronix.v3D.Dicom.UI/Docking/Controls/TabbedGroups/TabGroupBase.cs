// $Id: TabGroupBase.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public abstract class TabGroupBase
  {
    public enum NotifyCode
    {
      StyleChanged,
      ProminentChanged,
      MinimumSizeChanged,
      ResizeBarVectorChanged,
      ResizeBarColorChanged,
      DisplayTabMode,
      ImageListChanging,
      ImageListChanged
    }

    // Class fields
    protected static int m_count = 0;
	    
    // Instance fields
    protected int m_unique;
    protected object m_tag;
    protected Size m_minSize;
    protected Decimal m_space;
    protected TabGroupBase m_parent;
    protected TabbedGroups m_tabbedGroups;
	
    public TabGroupBase(TabbedGroups tabbedGroups)
    {
      InternalConstruct(tabbedGroups, null);
    }
        
    public TabGroupBase(TabbedGroups tabbedGroups, TabGroupBase parent)
    {
      InternalConstruct(tabbedGroups, parent);
    }
		
    protected void InternalConstruct(TabbedGroups tabbedGroups, TabGroupBase parent)
    {
      // Assign initial values
      m_tabbedGroups = tabbedGroups;
      m_parent = parent;
      m_unique = m_count++;
		    
      // Defaults
      m_tag = null;
      m_space = 100m;
      m_minSize = new Size(m_tabbedGroups.DefaultGroupMinimumWidth,
        m_tabbedGroups.DefaultGroupMinimumHeight);
    }

    public Decimal Space
    {
      get 
      {
        TabGroupLeaf prominent = m_tabbedGroups.ProminentLeaf;
                
        // Are we in prominent mode?
        if (prominent != null)
        {
          // If we are a child of the root sequence
          if (m_parent.Parent == null)
          {
            // Then our space is determined by the containment of the prominent leaf
            if (this.ContainsProminent(true))
              return 100m;
            else
              return 0m;
          }
          else
          {
            // Else, if we are inside a sequence that contains prominent leaf
            if (m_parent.ContainsProminent(true))
            {
              // Then we need to decide on all or nothing allocation
              if (this.ContainsProminent(true))
                return 100m;
              else
                return 0m;
            }
            else
            {
              // Otherwise, we will already be shrunk
              return m_space;                        
            }
          }
        }
        else
          return m_space; 
      }
            
      set { m_space = value; }
    }

    internal Decimal RealSpace
    {
      get { return m_space; }
      set { m_space = value; }
    }

    public Size MinimumSize
    {
      get { return m_minSize; }
            
      set
      {
        if (!m_minSize.Equals(value))
        {
          m_minSize = value;
                    
          // Inform parent it might need to resize its children
          if (m_parent != null)
            m_parent.Notify(NotifyCode.MinimumSizeChanged);
        }
      }
    }

    public TabGroupBase Parent 
    {
      get { return m_parent; }
    }

    internal void SetParent(TabGroupBase tgb)
    {
      m_parent = tgb;
    }
        
    public TabbedGroups TabbedGroups 
    {
      get { return m_tabbedGroups; }
    }

    public object Tag
    {
      get { return m_tag; }
      set { m_tag = value; }
    }
        
    public int Unique
    {
      get { return m_unique; }
    }
        
    // Common Properties not implemented
    public abstract int Count               { get; }
    public abstract bool IsLeaf             { get; }
    public abstract bool IsSequence         { get; }
    public abstract Control GroupControl    { get; }
        
    // Common methods not implemented
    public abstract void Notify(NotifyCode code); 
    public abstract bool ContainsProminent(bool recurse);
    public abstract void SaveToXml(XmlTextWriter xmlOut);
    public abstract void LoadFromXml(XmlTextReader xmlIn);
  }
}


#region revision history

// $Log: TabGroupBase.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/TabGroupBase.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: TabGroupBase.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

