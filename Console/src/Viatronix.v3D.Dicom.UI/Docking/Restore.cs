// $Id: Restore.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Xml;
using System.Drawing;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class Restore
  {
    // Instance fields
    protected Restore m_child;

    public Restore()
    {
      // Default state
      m_child = null;
    }

    public Restore(Restore child)
    {
      // Remember parameter
      m_child = child;
    }

    public Restore Child
    {
      get { return m_child; }
      set { m_child = value; }
    }

    public virtual void PerformRestore(DockingManager dm) {}
    public virtual void PerformRestore(Window w) {}
    public virtual void PerformRestore(Zone z) {}
    public virtual void PerformRestore() {}

    public virtual void Reconnect(DockingManager dm)
    {
      if (m_child != null)
        m_child.Reconnect(dm);
    }

    public virtual void SaveToXml(XmlTextWriter xmlOut)
    {
      // Must define my type so loading can recreate my instance
      xmlOut.WriteAttributeString("Type", this.GetType().ToString());

      SaveInternalToXml(xmlOut);

      // Output the child object			
      xmlOut.WriteStartElement("Child");

      if (m_child == null)
        xmlOut.WriteAttributeString("Type", "null");
      else
        m_child.SaveToXml(xmlOut);

      xmlOut.WriteEndElement();
    }

    public virtual void LoadFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "Child")
        throw new ArgumentException("Node 'Child' expected but not found");

      string type = xmlIn.GetAttribute(0);
			
      if (type != "null")
        m_child = CreateFromXml(xmlIn, false, formatVersion);

      // Move past the end element
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");
		
      // Check it has the expected name
      if (xmlIn.NodeType != XmlNodeType.EndElement)
        throw new ArgumentException("EndElement expected but not found");
    }

    public virtual void SaveInternalToXml(XmlTextWriter xmlOut) {}
    public virtual void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion) {}

    public static Restore CreateFromXml(XmlTextReader xmlIn, bool readIn, int formatVersion)
    {
      if (readIn)
      {
        // Move to next xml node
        if (!xmlIn.Read())
          throw new ArgumentException("Could not read in next expected node");
      }
			
      // Grab type name of the object to create
      string attrType = xmlIn.GetAttribute(0);

      // Convert from string to a Type description object
      Type newType = Type.GetType(attrType);

      // Create an instance of this object which must derive from Restore base class
      Restore newRestore = newType.Assembly.CreateInstance(attrType) as Restore;

      // Ask the object to load itself
      newRestore.LoadFromXml(xmlIn, formatVersion);

      return newRestore;
    }
  }

  public class RestoreContent : Restore
  {
    // Instance fields
    protected String m_title;
    protected Content m_content;

    public RestoreContent()
      : base()
    {
      // Default state
      m_title = "";
      m_content = null;
    }

    public RestoreContent(Content content)
      : base()
    {
      // Remember parameter
      m_title = content.Title;
      m_content = content;
    }

    public RestoreContent(Restore child, Content content)
      : base(child)
    {
      // Remember parameter
      m_title = content.Title;
      m_content = content;
    }

    public override void Reconnect(DockingManager dm)
    {
      // Connect to the current instance of required content object
      m_content = dm.Contents[m_title];

      base.Reconnect(dm);
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      xmlOut.WriteStartElement("Content");
      xmlOut.WriteAttributeString("Name", m_content.Title);
      xmlOut.WriteEndElement();				
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "Content")
        throw new ArgumentException("Node 'Content' expected but not found");

      // Grab type name of the object to create
      m_title = xmlIn.GetAttribute(0);
    }
  }
	
  public class RestoreContentState : RestoreContent
  {
    // Instance fields
    protected State m_state;

    public RestoreContentState()
      : base()
    {
    }

    public RestoreContentState(State state, Content content)
      : base(content)
    {
      // Remember parameter
      m_state = state;
    }

    public RestoreContentState(Restore child, State state, Content content)
      : base(child, content)
    {
      // Remember parameter
      m_state = state;
    }

    public override void PerformRestore(DockingManager dm)
    {
      // Use the existing DockingManager method that will create a Window appropriate for 
      // this Content and then add a new Zone for hosting the Window. It will always place
      // the Zone at the inner most level
      dm.AddContentWithState(m_content, m_state);				
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      xmlOut.WriteStartElement("State");
      xmlOut.WriteAttributeString("Value", m_state.ToString());
      xmlOut.WriteEndElement();				
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "State")
        throw new ArgumentException("Node 'State' expected but not found");

      // Grab type state of the object to create
      string attrState = xmlIn.GetAttribute(0);

      // Convert from string to enumeration value
      m_state = (State)Enum.Parse(typeof(State), attrState);
    }
  }
	
  public class RestoreAutoHideState : RestoreContentState
  {
    // Instance fields
	    
    public RestoreAutoHideState()
      : base()
    {
    }
        
    public RestoreAutoHideState(State state, Content content)
      : base(state, content)
    {
    }

    public RestoreAutoHideState(Restore child, State state, Content content)
      : base(child, state, content)
    {
    }
    
    public override void PerformRestore(DockingManager dm)
    {
      // Create collection of Contents to auto hide
      ContentCollection cc = new ContentCollection();
            
      // In this case, there is only one
      cc.Add(m_content);
        
      // Add to appropriate AutoHidePanel based on m_state
      dm.AutoHideContents(cc, m_state);
    }
  }

  public class RestoreAutoHideAffinity : RestoreAutoHideState
  {
    // Instance fields
    protected StringCollection m_next;
    protected StringCollection m_previous;
    protected StringCollection m_nextAll;
    protected StringCollection m_previousAll;

    public RestoreAutoHideAffinity()
      : base()
    {
      // Must always point to valid reference
      m_next = new StringCollection();
      m_previous = new StringCollection();
      m_nextAll = new StringCollection();
      m_previousAll = new StringCollection();
    }

    public RestoreAutoHideAffinity(Restore child, 
      State state,
      Content content, 
      StringCollection next,
      StringCollection previous,
      StringCollection nextAll,
      StringCollection previousAll)
      : base(child, state, content)
    {
      // Remember parameters
      m_next = next;				
      m_previous = previous;	
      m_nextAll = nextAll;				
      m_previousAll = previousAll;	
    }

    public override void PerformRestore(DockingManager dm)
    {   
      // Get the correct target panel from state
      AutoHidePanel ahp = dm.AutoHidePanelForState(m_state);
            
      ahp.AddContent(m_content, m_next, m_previous, m_nextAll, m_previousAll);
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      m_next.SaveToXml("Next", xmlOut);
      m_previous.SaveToXml("Previous", xmlOut);
      m_nextAll.SaveToXml("NextAll", xmlOut);
      m_previousAll.SaveToXml("PreviousAll", xmlOut);
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);
      m_next.LoadFromXml("Next", xmlIn);
      m_previous.LoadFromXml("Previous", xmlIn);
      m_nextAll.LoadFromXml("NextAll", xmlIn);
      m_previousAll.LoadFromXml("PreviousAll", xmlIn);
    }
  }

  public class RestoreContentDockingAffinity : RestoreContentState
  {
    // Instance fields
    protected Size m_size;
    protected Point m_location;
    protected StringCollection m_best;
    protected StringCollection m_next;
    protected StringCollection m_previous;
    protected StringCollection m_nextAll;
    protected StringCollection m_previousAll;

    public RestoreContentDockingAffinity()
      : base()
    {
      // Must always point to valid reference
      m_best = new StringCollection();
      m_next = new StringCollection();
      m_previous = new StringCollection();
      m_nextAll = new StringCollection();
      m_previousAll = new StringCollection();
    }

    public RestoreContentDockingAffinity(Restore child, 
      State state, 
      Content content, 
      StringCollection best,
      StringCollection next,
      StringCollection previous,
      StringCollection nextAll,
      StringCollection previousAll)
      : base(child, state, content)
    {
      // Remember parameters
      m_best = best;
      m_next = next;
      m_previous = previous;
      m_nextAll = nextAll;
      m_previousAll = previousAll;
      m_size = content.DisplaySize;
      m_location = content.DisplayLocation;
    }

    public override void PerformRestore(DockingManager dm)
    {
      int count = dm.Container.Controls.Count;

      int min = -1;
      int max = count;

      if (dm.InnerControl != null)
        min = dm.Container.Controls.IndexOf(dm.InnerControl);

      if (dm.OuterControl != null)
        max = dm.OuterControlIndex();

      int beforeIndex = -1;
      int afterIndex = max;
      int beforeAllIndex = -1;
      int afterAllIndex = max;

      // Create a collection of the Zones in the appropriate direction
      for(int index=0; index<count; index++)
      {
        Zone z = dm.Container.Controls[index] as Zone;

        if (z != null)
        {
          StringCollection sc = ZoneHelper.ContentNames(z);
					
          if (m_state == z.State)
          {
            if (sc.Contains(m_best))
            {
              // Can we delegate to a child Restore object
              if (m_child != null)
                m_child.PerformRestore(z);
              else
              {
                // Just add an appropriate Window to start of the Zone
                dm.AddContentToZone(m_content, z, 0);
              }
              return;
            }

            // If the WindowContent contains a Content previous to the target
            if (sc.Contains(m_previous))
            {
              if (index > beforeIndex)
                beforeIndex = index;
            }
						
            // If the WindowContent contains a Content next to the target
            if (sc.Contains(m_next))
            {
              if (index < afterIndex)
                afterIndex = index;
            }
          }
          else
          {
            // If the WindowContent contains a Content previous to the target
            if (sc.Contains(m_previousAll))
            {
              if (index > beforeAllIndex)
                beforeAllIndex = index;
            }
						
            // If the WindowContent contains a Content next to the target
            if (sc.Contains(m_nextAll))
            {
              if (index < afterAllIndex)
                afterAllIndex = index;
            }
          }
        }
      }

      dm.Container.SuspendLayout();

      // Create a new Zone with correct State
      Zone newZ = dm.CreateZoneForContent(m_state);

      // Restore the correct content size/location values
      m_content.DisplaySize = m_size;
      m_content.DisplayLocation = m_location;

      // Add an appropriate Window to start of the Zone
      dm.AddContentToZone(m_content, newZ, 0);

      // Did we find a valid 'before' Zone?
      if (beforeIndex != -1)
      {
        // Try and place more accurately according to other edge Zones
        if (beforeAllIndex > beforeIndex)
          beforeIndex = beforeAllIndex;

        // Check against limits
        if (beforeIndex >= max)
          beforeIndex = max - 1;

        dm.Container.Controls.SetChildIndex(newZ, beforeIndex + 1);
      }
      else
      {
        // Try and place more accurately according to other edge Zones
        if (afterAllIndex < afterIndex)
          afterIndex = afterAllIndex;

        // Check against limits
        if (afterIndex <= min)
          afterIndex = min + 1;
				
        if (afterIndex > min)
          dm.Container.Controls.SetChildIndex(newZ, afterIndex);
        else
        {
          // Set the Zone to be the least important of our Zones
          dm.ReorderZoneToInnerMost(newZ);
        }
      }

      dm.Container.ResumeLayout();
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      xmlOut.WriteStartElement("Position");
      xmlOut.WriteAttributeString("Size", ConversionHelper.SizeToString(m_size));
      xmlOut.WriteAttributeString("Location", ConversionHelper.PointToString(m_location));
      xmlOut.WriteEndElement();				
      m_best.SaveToXml("Best", xmlOut);
      m_next.SaveToXml("Next", xmlOut);
      m_previous.SaveToXml("Previous", xmlOut);
      m_nextAll.SaveToXml("NextAll", xmlOut);
      m_previousAll.SaveToXml("PreviousAll", xmlOut);
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "Position")
        throw new ArgumentException("Node 'Position' expected but not found");

      // Grab raw position information
      string attrSize = xmlIn.GetAttribute(0);
      string attrLocation = xmlIn.GetAttribute(1);

      // Convert from string to proper types
      m_size = ConversionHelper.StringToSize(attrSize);
      m_location = ConversionHelper.StringToPoint(attrLocation);

      m_best.LoadFromXml("Best", xmlIn);
      m_next.LoadFromXml("Next", xmlIn);
      m_previous.LoadFromXml("Previous", xmlIn);
      m_nextAll.LoadFromXml("NextAll", xmlIn);
      m_previousAll.LoadFromXml("PreviousAll", xmlIn);
    }
  }

  public class RestoreContentFloatingAffinity : RestoreContentState
  {
    // Instance fields
    protected Size m_size;
    protected Point m_location;
    protected StringCollection m_best;
    protected StringCollection m_associates;

    public RestoreContentFloatingAffinity()
      : base()
    {
      // Must always point to valid reference
      m_best = new StringCollection();
      m_associates = new StringCollection();
    }

    public RestoreContentFloatingAffinity(Restore child, 
      State state, 
      Content content, 
      StringCollection best,
      StringCollection associates)
      : base(child, state, content)
    {
      // Remember parameters
      m_best = best;
      m_associates = associates;
      m_size = content.DisplaySize;
      m_location = content.DisplayLocation;

      // Remove target from collection of friends
      if (m_best.Contains(content.Title))
        m_best.Remove(content.Title);

      // Remove target from collection of associates
      if (m_associates.Contains(content.Title))
        m_associates.Remove(content.Title);
    }

    public override void PerformRestore(DockingManager dm)
    {
      // Grab a list of all floating forms
      Form[] owned = dm.Container.FindForm().OwnedForms;

      FloatingForm target = null;

      // Find the match to one of our best friends
      foreach(Form f in owned)
      {
        FloatingForm ff = f as FloatingForm;

        if (ff != null)
        {
          if (ZoneHelper.ContentNames(ff.Zone).Contains(m_best))
          {
            target = ff;
            break;
          }
        }
      }

      // If no friends then try associates as second best option
      if (target == null)
      {
        // Find the match to one of our best friends
        foreach(Form f in owned)
        {
          FloatingForm ff = f as FloatingForm;

          if (ff != null)
          {
            if (ZoneHelper.ContentNames(ff.Zone).Contains(m_associates))
            {
              target = ff;
              break;
            }
          }
        }
      }

      // If we found a friend/associate, then restore to it
      if (target != null)
      {
        // We should have a child and be able to restore to its Zone
        m_child.PerformRestore(target.Zone);
      }
      else
      {
        // Restore its location/size
        m_content.DisplayLocation = m_location;
        m_content.DisplaySize = m_size;

        // Use the docking manage method to create us a new Floating Window at correct size/location
        dm.AddContentWithState(m_content, State.Floating);
      }
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      xmlOut.WriteStartElement("Position");
      xmlOut.WriteAttributeString("Size", ConversionHelper.SizeToString(m_size));
      xmlOut.WriteAttributeString("Location", ConversionHelper.PointToString(m_location));
      xmlOut.WriteEndElement();				
      m_best.SaveToXml("Best", xmlOut);
      m_associates.SaveToXml("Associates", xmlOut);
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "Position")
        throw new ArgumentException("Node 'Position' expected but not found");

      // Grab raw position information
      string attrSize = xmlIn.GetAttribute(0);
      string attrLocation = xmlIn.GetAttribute(1);

      // Convert from string to proper types
      m_size = ConversionHelper.StringToSize(attrSize);
      m_location = ConversionHelper.StringToPoint(attrLocation);

      m_best.LoadFromXml("Best", xmlIn);
      m_associates.LoadFromXml("Associates", xmlIn);
    }
  }

  public class RestoreZoneAffinity : RestoreContent
  {
    // Instance fields
    protected Decimal m_space;
    protected StringCollection m_best;
    protected StringCollection m_next;
    protected StringCollection m_previous;

    public RestoreZoneAffinity()
      : base()
    {
      // Default state
      m_space = 50m;

      // Must always point to valid reference
      m_best = new StringCollection();
      m_next = new StringCollection();
      m_previous = new StringCollection();
    }

    public RestoreZoneAffinity(Restore child, 
      Content content, 
      StringCollection best,
      StringCollection next,
      StringCollection previous)
      : base(child, content)
    {
      // Remember parameters
      m_best = best;				
      m_next = next;				
      m_previous = previous;	
			
      if (content.Visible)			
        m_space = content.ParentWindowContent.ZoneArea;
      else
        m_space = 50m;
    }

    public override void PerformRestore(Zone z)
    {
      int count = z.Windows.Count;
      int beforeIndex = - 1;
      int afterIndex = count;
		
      // Find the match to one of our best friends
      for(int index=0; index<count; index++)
      {
        WindowContent wc = z.Windows[index] as WindowContent;

        if (wc != null)
        {
          // If this WindowContent contains a best friend, then add ourself here as well
          if (wc.Contents.Contains(m_best))
          {
            if (m_child == null)
            {
              // If we do not have a Restore object for the Window then just add
              // into the WindowContent at the end of the existing Contents
              wc.Contents.Add(m_content);
            }
            else
            {
              // Get the child to restore as best as possible inside WindowContent
              m_child.PerformRestore(wc);
            }

            return;
          }

          // If the WindowContent contains a Content previous to the target
          if (wc.Contents.Contains(m_previous))
          {
            if (index > beforeIndex)
              beforeIndex = index;
          }
					
          // If the WindowContent contains a Content next to the target
          if (wc.Contents.Contains(m_next))
          {
            if (index < afterIndex)
              afterIndex = index;
          }
        }
      }

      // If we get here then we did not find any best friends, this 
      // means we need to create a new WindowContent to host the Content.
      Window newW =  z.DockingManager.CreateWindowForContent(m_content);

      ZoneSequence zs = z as ZoneSequence;

      // If this is inside a ZoneSequence instance
      if (zs != null)
      {
        // Do not reposition the Windows on the .Insert but instead ignore the
        // reposition and let it happen in the .ModifyWindowSpace. This reduces
        // the flicker that would occur otherwise
        zs.SuppressReposition();
      }

      // Need to find the best place in the order for the Content, start by
      // looking for the last 'previous' content and place immediately after it
      if (beforeIndex >= 0)
      {
        // Great, insert after it
        z.Windows.Insert(beforeIndex + 1, newW);
      }
      else
      {
        // No joy, so find the first 'next' content and place just before it, if
        // none are found then just add to the end of the collection.
        z.Windows.Insert(afterIndex, newW);
      }

      // If this is inside a ZoneSequence instance
      if (zs != null)
      {
        // We want to try and allocate the correct Zone space
        zs.ModifyWindowSpace(newW, m_space);
      }
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      xmlOut.WriteStartElement("Space");
      xmlOut.WriteAttributeString("Value", m_space.ToString());
      xmlOut.WriteEndElement();				
      m_best.SaveToXml("Best", xmlOut);
      m_next.SaveToXml("Next", xmlOut);
      m_previous.SaveToXml("Previous", xmlOut);
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);

      // Move to next xml node
      if (!xmlIn.Read())
        throw new ArgumentException("Could not read in next expected node");

      // Check it has the expected name
      if (xmlIn.Name != "Space")
        throw new ArgumentException("Node 'Space' expected but not found");

      // Grab raw position information
      string attrSpace = xmlIn.GetAttribute(0);

      // Convert from string to proper type
      m_space = Decimal.Parse(attrSpace);

      m_best.LoadFromXml("Best", xmlIn);
      m_next.LoadFromXml("Next", xmlIn);
      m_previous.LoadFromXml("Previous", xmlIn);
    }
  }

  public class RestoreWindowContent : RestoreContent
  {
    // Instance fields
    protected bool m_selected;
    protected StringCollection m_next;
    protected StringCollection m_previous;

    public RestoreWindowContent()
      : base()
    {
      // Must always point to valid reference
      m_selected = false;
      m_next = new StringCollection();
      m_previous = new StringCollection();
    }

    public RestoreWindowContent(Restore child, 
      Content content, 
      StringCollection next, 
      StringCollection previous,
      bool selected)
      : base(child, content)
    {
      // Remember parameters
      m_selected = selected;
      m_next = next;
      m_previous = previous;
    }

    public override void PerformRestore(Window w)
    {
      // We are only ever called for a WindowContent object
      WindowContent wc = w as WindowContent;

      int bestIndex = -1;

      foreach(String s in m_previous)
      {
        if (wc.Contents.Contains(s))
        {
          int previousIndex = wc.Contents.IndexOf(wc.Contents[s]);

          if (previousIndex > bestIndex)
            bestIndex = previousIndex;
        }
      }

      // Did we find a previous Content?
      if (bestIndex >= 0)
      {
        // Great, insert after it
        wc.Contents.Insert(bestIndex + 1, m_content);
      }
      else
      {
        bestIndex = wc.Contents.Count;

        foreach(String s in m_next)
        {
          if (wc.Contents.Contains(s))
          {
            int nextIndex = wc.Contents.IndexOf(wc.Contents[s]);

            if (nextIndex < bestIndex)
              bestIndex = nextIndex;
          }
        }

        // Insert before the found entry (or at end if non found)
        wc.Contents.Insert(bestIndex, m_content);
      }
			
      // Should this content become selected?
      if (m_selected)
        m_content.BringToFront();
    }

    public override void SaveInternalToXml(XmlTextWriter xmlOut)
    {
      base.SaveInternalToXml(xmlOut);
      m_next.SaveToXml("Next", xmlOut);
      m_previous.SaveToXml("Previous", xmlOut);
        
      xmlOut.WriteStartElement("Selected");
      xmlOut.WriteAttributeString("Value", m_selected.ToString());
      xmlOut.WriteEndElement();				
    }

    public override void LoadInternalFromXml(XmlTextReader xmlIn, int formatVersion)
    {
      base.LoadInternalFromXml(xmlIn, formatVersion);
      m_next.LoadFromXml("Next", xmlIn);
      m_previous.LoadFromXml("Previous", xmlIn);
        
      // m_selected added in version 4 format
      if (formatVersion >= 4)
      {
        // Move to next xml node
        if (!xmlIn.Read())
          throw new ArgumentException("Could not read in next expected node");

        // Check it has the expected name
        if (xmlIn.Name != "Selected")
          throw new ArgumentException("Node 'Selected' expected but not found");

        // Convert attribute value to boolean value
        m_selected = System.Convert.ToBoolean(xmlIn.GetAttribute(0));
      }
    }
  }
}


#region revision history

// $Log: Restore.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Restore.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Restore.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

