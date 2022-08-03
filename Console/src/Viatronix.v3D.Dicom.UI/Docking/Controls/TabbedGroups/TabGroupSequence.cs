// $Id: TabGroupSequence.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public class TabGroupSequence : TabGroupBase, IResizeSource
  {
    // Class fields
    protected const int SPACE_PRECISION = 3;
	
    // Instance fields
    protected Control m_control;
    protected Direction m_direction;
    protected TabGroupBaseCollection m_children;
        
    public TabGroupSequence(TabbedGroups tabbedGroups)
      : base(tabbedGroups)
    {
      // Root instance always defaults to being horizontal
      InternalConstruct(tabbedGroups, Direction.Horizontal);
    }
    
    public TabGroupSequence(TabbedGroups tabbedGroups, TabGroupBase parent)
      : base(tabbedGroups, parent)
    {
      InternalConstruct(null, Direction.Horizontal);
    }

    public TabGroupSequence(TabbedGroups tabbedGroups, TabGroupBase parent, Direction direction)
      : base(tabbedGroups, parent)
    {
      InternalConstruct(null, direction);
    }

    protected void InternalConstruct(Control control, Direction direction)
    {
      // Do we need to create our own window?
      if (control == null) 
      {
        // Yes, use a simple panel for organizing children onto
        m_control = new Panel();
      }
      else
      {
        // No, use the constructor provided one
        m_control = control;
      }
            
      // Hook into control events
      m_control.Resize += new EventHandler(OnControlResize);
            
      // Assign initial values
      m_direction = direction;
            
      // Create collection to remember our child objects
      m_children = new TabGroupBaseCollection();
    }
        
    public override int Count               
    { 
      get { return m_children.Count; } 
    }
            
    public override bool IsLeaf             
    { 
      get { return false; } 
    }
        
    public override bool IsSequence         
    { 
      get { return true; } 
    }
        
    public override Control GroupControl    
    { 
      get { return m_control; } 
    }
        
    public Direction Direction
    {
      get { return m_direction; }
            
      set
      {
        if (m_direction != value)
        {
          m_direction = value;
          RepositionChildren();
        }
      }
    }
        
    public VisualStyle Style        { get { return m_tabbedGroups.Style;             } }        
    public int ResizeBarVector      { get { return m_tabbedGroups.ResizeBarVector;   } }
    public Color ResizeBarColor     { get { return m_tabbedGroups.ResizeBarColor;    } }        
    public Color BackgroundColor    { get { return m_tabbedGroups.BackColor;         } }

    public TabGroupLeaf AddNewLeaf()
    {
      // Create a new leaf instance with correct back references
      TabGroupLeaf tgl = new TabGroupLeaf(m_tabbedGroups, this);
            
      // Add into the collection
      Add(tgl);
            
      // Return its position in collection
      return tgl;
    }

    public TabGroupLeaf InsertNewLeaf(int index)
    {
      // Range check index
      if (index < 0)
        throw new ArgumentOutOfRangeException("index", index, "Insert index must be at least 0");
                
      if (index >= m_children.Count)
        throw new ArgumentOutOfRangeException("index", index, "Cannot insert after end of current entries");

      // Create a new leaf instance with correct back references
      TabGroupLeaf tgl = new TabGroupLeaf(m_tabbedGroups, this);
            
      // Insert into correct collection position
      Insert(index, tgl);
            
      // Return its position in collection
      return tgl;                           
    }
            
    public void Remove(TabGroupBase group)
    {
      // Convert from reference to index to use existing RemoveAt implementation
      RemoveAt(m_children.IndexOf(group));
    }

    public void RemoveAt(int index)
    {
      // Range check index
      if (index < 0)
        throw new ArgumentOutOfRangeException("index", index, "RemoveAt index must be at least 0");
                
      if (index >= m_children.Count)
        throw new ArgumentOutOfRangeException("index", index, "Cannot remove entry after end of list");

      // Is the removed item the active leaf?
      if (m_children[index] == m_tabbedGroups.ActiveLeaf)
      {
        // Then request movement of the active leaf
        m_tabbedGroups.MoveActiveToNearestFromLeaf(m_children[index]);
      }

      // Inform control that a group is removed, so it can track number of leafs 
      m_tabbedGroups.GroupRemoved(m_children[index]);

      // Is this the only Window entry?
      if (m_children.Count == 1)
      {
        // Remove Window from appearance

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(m_control.Controls, 0);
      }
      else
      {
        int pos = 0;

        // Calculate position of Window to remove				
        if (index != 0)
          pos = index * 2 - 1;

        // Remove Window and bar 

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(m_control.Controls, pos);
        ControlHelper.RemoveAt(m_control.Controls, pos);
      }

      // How much space is removed entry taking up?
      Decimal space = m_children[index].Space;

      // Remove child from collection
      m_children.RemoveAt(index);

      // Redistribute space to other groups
      RemoveWindowSpace(space);

      // Update child layout to reflect new proportional spacing values
      RepositionChildren();

      // Last page removed?            
      if (m_children.Count == 0)
      {
        // All pages removed, do we need to compact?
        if (m_tabbedGroups.AutoCompact)
          m_tabbedGroups.Compact();
      }
            
      // Give control chance to enfore leaf policy
      m_tabbedGroups.EnforceAtLeastOneLeaf();
            
      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }
        
    public int IndexOf(TabGroupBase group)
    {
      return m_children.IndexOf(group);
    }
        
    public void Clear()
    {
      // Do we contain the active leaf?
      if (m_children.IndexOf(m_tabbedGroups.ActiveLeaf) != 0)
      {
        // Then request movement of the active leaf to different group
        m_tabbedGroups.MoveActiveToNearestFromSequence(this);
      }
        
      // Remove all child controls
      ControlHelper.RemoveAll(m_control);
            
      // Remove all child group references
      m_children.Clear();

      m_control.Invalidate();

      // All pages removed, do we need to compact?
      if (m_tabbedGroups.AutoCompact)
        m_tabbedGroups.Compact();

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }
        
    public TabGroupBase this[int index]
    {
      get { return m_children[index]; }
    }

    public override void Notify(NotifyCode code)
    {
      // Handle codes of interest
      switch(code)
      {
        case NotifyCode.ProminentChanged:
        case NotifyCode.MinimumSizeChanged:
          // Must reposition to take account of change
          RepositionChildren();
          break;
        case NotifyCode.StyleChanged:
          // Inform each resize bar of change in style
          foreach(Control c in m_control.Controls)
            if (c is ResizeBar)
              (c as ResizeBar).Style = m_tabbedGroups.Style;
                
          // Reposition the children based on new resize bar size
          RepositionChildren();
          break;
        case NotifyCode.ResizeBarVectorChanged:
          // Recalculate layout of childreen
          RepositionChildren();
          break;
        case NotifyCode.ResizeBarColorChanged:
          // If we are showing at least one resize bar
          if (m_children.Count > 1)
          {
            // Then must repaint in new color
            m_control.Invalidate();
          }
          break;
      }
            
      // Pass notification to children
      foreach(TabGroupBase child in m_children)
        child.Notify(code);
    }
    
    public void Rebalance(bool recurse)
    {
      if (m_children.Count > 0)
      {
        // Calculate how much space to give each child
        Decimal newSpace = Decimal.Round(100m / m_children.Count, SPACE_PRECISION);

        // Assign equal space to all entries        
        foreach(TabGroupBase group in m_children)
          group.Space = newSpace;

        Decimal totalSpace = 100m - (newSpace * m_children.Count);
                 
        // Allocate rounding errors to last child
        if (totalSpace != 0)
          m_children[m_children.Count - 1].Space += 100m - totalSpace;
                        
        // Propogate effect into child sequences?
        if (recurse)
        {
          foreach(TabGroupBase group in m_children)
            if (group.IsSequence)
              (group as TabGroupSequence).Rebalance(recurse);
        }
      }
            
      // Update child layout to reflect new proportional spacing values
      RepositionChildren();
    }

    public override bool ContainsProminent(bool recurse)
    {
      // Cache the currently selected prominent group
      TabGroupLeaf prominent = m_tabbedGroups.ProminentLeaf;
            
      // If not defined then we cannot contain it!
      if (prominent == null)
        return false;
                
      // Check our own leaf nodes first
      foreach(TabGroupBase group in m_children)
        if (group.IsLeaf)
          if (group == prominent)
            return true;

      // Need to check sub-sequences?
      if (recurse)
      {
        // Check our child sequences 
        foreach(TabGroupBase group in m_children)
          if (group.IsSequence)
            if (group.ContainsProminent(recurse))
              return true;
      }
                                    
      // Not found
      return false;                            
    }

    public override void SaveToXml(XmlTextWriter xmlOut)
    {
      // Output standard values appropriate for all Sequence instances
      xmlOut.WriteStartElement("Sequence");
      xmlOut.WriteAttributeString("Count", m_children.Count.ToString());
      xmlOut.WriteAttributeString("Unique", m_unique.ToString());
      xmlOut.WriteAttributeString("Space", m_space.ToString());
      xmlOut.WriteAttributeString("Direction", m_direction.ToString());

      // Output each sub element
      foreach(TabGroupBase tgb in m_children)
        tgb.SaveToXml(xmlOut);
                
      xmlOut.WriteEndElement();
    }

    public override void LoadFromXml(XmlTextReader xmlIn)
    {
      // Grab the expected attributes
      string rawCount = xmlIn.GetAttribute(0);
      string rawUnique = xmlIn.GetAttribute(1);
      string rawSpace = xmlIn.GetAttribute(2);
      string rawDirection = xmlIn.GetAttribute(3);

      // Convert to correct types
      int count = System.Convert.ToInt32(rawCount);
      int unique = System.Convert.ToInt32(rawUnique);
      Decimal space = System.Convert.ToDecimal(rawSpace);
      Direction direction = (rawDirection == "Horizontal" ? Direction.Horizontal : 
        Direction.Vertical);
            
      // Update myself with new values
      m_unique = unique;
      m_space = space;
      m_direction = direction;
            
      // Load each of the children
      for(int i=0; i<count; i++)
      {
        // Read the next Element
        if (!xmlIn.Read())
          throw new ArgumentException("An element was expected but could not be read in");

        TabGroupBase newElement = null;

        // Is it another sequence?
        if (xmlIn.Name == "Sequence")
          newElement = new TabGroupSequence(m_tabbedGroups, this);
        else if (xmlIn.Name == "Leaf")
          newElement = new TabGroupLeaf(m_tabbedGroups, this);
        else
          throw new ArgumentException("Unknown element was encountered");
            
        bool expectEndElement = !xmlIn.IsEmptyElement;

        // Load its config
        newElement.LoadFromXml(xmlIn);
                   
        // Add new element to the collection
        Add(newElement);

        // Do we expect and end element to occur?
        if (expectEndElement)
        {
          // Move past the end element
          if (!xmlIn.Read())
            throw new ArgumentException("Could not read in next expected node");

          // Check it has the expected name
          if (xmlIn.NodeType != XmlNodeType.EndElement)
            throw new ArgumentException("EndElement expected but not found");
        }
      }
    }

    public void Compact()
    {
      Compact(m_tabbedGroups.CompactOptions);
    }

    public void Compact(TabbedGroups.CompactFlags flags)
    {
      // Compact each child sequence
      foreach(TabGroupBase tgb in m_children)
        if (tgb.IsSequence)
          (tgb as TabGroupSequence).Compact(flags);
        
      // Remove dangling entries
      CompactRemoveEmptyTabLeafs(flags);
      CompactRemoveEmptyTabSequences(flags);
            
      // Integrate single entries
      CompactReduceSingleEntries(flags);
            
      // Integrate sub-sequences which run in same direction
      CompactReduceSameDirection(flags);
    }
        
    public void Reposition()
    {
      // Update child layout to reflect new proportional spacing values
      RepositionChildren();
    }

    protected void CompactRemoveEmptyTabLeafs(TabbedGroups.CompactFlags flags)
    {
      // Should we check for empty leaf nodes?
      if ((flags & Controls.TabbedGroups.CompactFlags.RemoveEmptyTabLeaf) != 0)
      {
        int count = m_children.Count;
                
        for(int index=0; index<count; index++)
        {
          // Only interested in leaf entries
          if (m_children[index].IsLeaf)
          {
            TabGroupLeaf tgl = (TabGroupLeaf)m_children[index];
                        
            // Is this an empty leaf node?
            if (tgl.Count == 0)
            {
              // Update active leaf setting
              if (m_tabbedGroups.ActiveLeaf == tgl)
              {
                TabGroupLeaf newLeaf = m_tabbedGroups.NextLeaf(tgl);
                                
                if (newLeaf == null)
                  newLeaf = m_tabbedGroups.PreviousLeaf(tgl);
                                    
                m_tabbedGroups.ActiveLeaf = newLeaf;
              }

              // Need to remove the redundant entry
              RemoveAt(index);
                            
              // Reduce number of entries left to check
              count--;
                            
              // Move backwards so the next increment stays on same index
              index--;
                        
              // Mark layout as dirty
              if (m_tabbedGroups.AutoCalculateDirty)
                m_tabbedGroups.Dirty = true;
            }
          }
        }
      }
    }

    protected void CompactRemoveEmptyTabSequences(TabbedGroups.CompactFlags flags)
    {
      // Should we check for empty sequence nodes?
      if ((flags & Controls.TabbedGroups.CompactFlags.RemoveEmptyTabSequence) != 0)
      {
        int count = m_children.Count;
                
        for(int index=0; index<count; index++)
        {
          // Only interested in sequence entries
          if (m_children[index].IsSequence)
          {
            TabGroupSequence tgs = (TabGroupSequence)m_children[index];
                        
            // Is this an empty sequence node?
            if (tgs.Count == 0)
            {
              // Need to remove the redundant entry
              RemoveAt(index);
                            
              // Reduce number of entries left to check
              count--;
                            
              // Move backwards so the next increment stays on same index
              index--;

              // Mark layout as dirty
              if (m_tabbedGroups.AutoCalculateDirty)
                m_tabbedGroups.Dirty = true;
            }
          }
        }
      }
    }

    protected void CompactReduceSingleEntries(TabbedGroups.CompactFlags flags)
    {
      bool changed = false;
        
      // Should we check for single instance nodes?
      if ((flags & Controls.TabbedGroups.CompactFlags.ReduceSingleEntries) != 0)
      {
        int count = m_children.Count;
                
        for(int index=0; index<count; index++)
        {
          // Only interested in sequence entries
          if (m_children[index].IsSequence)
          {
            TabGroupSequence tgs = (TabGroupSequence)m_children[index];
                        
            // Does this entry only have a single child
            if (tgs.Count == 1)
            {
              // Remember how much space the base entry occupies
              Decimal temp = tgs.RealSpace;
                            
              // Get reference to only child
              TabGroupBase child = tgs[0];
                            
              // Update parentage
              child.SetParent(this);
                            
              // Find the child control to be replaced
              int childPos = m_control.Controls.IndexOf(tgs.GroupControl);
                            
              // Remove it
              ControlHelper.RemoveAt(m_control.Controls, childPos);
                            
              // Add new child control in its place
              m_control.Controls.Add(child.GroupControl);
              m_control.Controls.SetChildIndex(child.GroupControl, childPos);
                            
              // Replace the middle object with the child
              m_children.RemoveAt(index);
              m_children.Insert(index, child);
                            
              // Restore its correct spacing
              child.RealSpace = temp;

              // Need controls repositioned
              changed = true;                

              // Mark layout as dirty
              if (m_tabbedGroups.AutoCalculateDirty)
                m_tabbedGroups.Dirty = true;
            }
          }
        }
      }

      // Change in contents requires entries to be repositioned
      if (changed)
        RepositionChildren();
    }
        
    protected void CompactReduceSameDirection(TabbedGroups.CompactFlags flags)
    {
      bool changed = false;
        
      // Should we check for same direction sub-sequences?
      if ((flags & Controls.TabbedGroups.CompactFlags.ReduceSameDirection) != 0)
      {
        int count = m_children.Count;
                
        for(int index=0; index<count; index++)
        {
          // Only interested in sequence entries
          if (m_children[index].IsSequence)
          {
            TabGroupSequence tgs = (TabGroupSequence)m_children[index];
                        
            // Does it run in same direction as ourself?
            if (m_direction == tgs.Direction)
            {
              // Remember how much space the base entry occupies
              Decimal temp = tgs.RealSpace;

              // Find the child control to be replaced
              int childPos = m_control.Controls.IndexOf(tgs.GroupControl);

              // Need to remove a resize bar before the control?
              if (childPos > 0)
                ControlHelper.RemoveAt(m_control.Controls, childPos);
                                
              // Remove the actual control
              ControlHelper.RemoveAt(m_control.Controls, childPos);

              // Remove the intermediate group
              m_children.RemoveAt(index);
                            
              // Reflect change in size
              count--;

              Decimal totalAllocated = 0m;

              // Add in each sub group in turn
              int subCount = tgs.Count;
                            
              bool firstInsert = true;
                
              for(int subIndex=0; subIndex<subCount; subIndex++)
              {
                TabGroupBase tgb = tgs[subIndex];
                            
                // What percentage of original space did it have?
                Decimal orig = tgb.RealSpace;
                                
                // Give it the same proportion of new space
                Decimal update = Decimal.Round(temp / 100 * orig, SPACE_PRECISION);
                            
                // Keep total actually allocated
                totalAllocated += update;
                            
                // Use new proportion
                tgb.RealSpace = update;
                                
                // Update parentage
                tgb.SetParent(this);

                // Does new child control need a resizing bar?                            
                if ((childPos > 0) && !firstInsert)
                {
                  // Create a resizing bar
                  ResizeBar bar = new ResizeBar(m_direction, this);

                  m_control.Controls.Add(bar);
                  m_control.Controls.SetChildIndex(bar, childPos++);
                }
                            
                // Add new child control in its place
                m_control.Controls.Add(tgb.GroupControl);
                m_control.Controls.SetChildIndex(tgb.GroupControl, childPos++);

                // Insert at current position
                m_children.Insert(index, tgb);
                                
                // Adjust variables to reflect increased size
                index++;
                count++;
                firstInsert = false;
              }
                            
              // Assign any remainder to last group     
              m_children[index-1].RealSpace +=  temp - totalAllocated;      
                            
              // Need controls repositioned
              changed = true;                

              // Mark layout as dirty
              if (m_tabbedGroups.AutoCalculateDirty)
                m_tabbedGroups.Dirty = true;
            }
          }
        }
      }
            
      // Change in contents requires entries to be repositioned
      if (changed)
        RepositionChildren();
    }

    protected TabGroupBase Add(TabGroupBase group)
    {
      // Remember reference
      m_children.Add(group);

      // First group added to sequence?
      if (m_children.Count == 1) 
      {
        // Add new child control
        m_control.Controls.Add(group.GroupControl);
      }
      else
      {
        // Create a resizing bar
        ResizeBar bar = new ResizeBar(m_direction, this);
                
        // Append resize bar between existing entries and new entry
        m_control.Controls.Add(bar);
                
        // Append new group control
        m_control.Controls.Add(group.GroupControl);
      }
            
      if (!m_tabbedGroups.Initializing)
      {
        // Allocate space for the new child
        AllocateSpace(group);
                
        // Update child layout to reflect new proportional spacing values
        RepositionChildren();
      }
            
      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;

      return group;
    }
    
    protected TabGroupBase Insert(int index, TabGroupBase group)
    {
      // Range check index
      if (index < 0)
        throw new ArgumentOutOfRangeException("index", index, "Insert index must be at least 0");
                
      if (index >= m_children.Count)
        throw new ArgumentOutOfRangeException("index", index, "Cannot insert after end of current entries");
        
      // Remember reference
      m_children.Insert(index, group);

      // Create a resizing bar
      ResizeBar bar = new ResizeBar(m_direction, this);
                
      // Append resize bar between existing entries and new entry
      m_control.Controls.Add(bar);
                
      // Append new group control
      m_control.Controls.Add(group.GroupControl);

      // Inserting at start of collection?
      if (index == 0)
      {
        // Reposition the bar and group to start of collection
        m_control.Controls.SetChildIndex(bar, 0);
        m_control.Controls.SetChildIndex(group.GroupControl, 0);
      }
      else
      {
        // Find correct index taking into account number of resize bars
        int	pos = index * 2 - 1;

        // Reposition the bar and Window to correct relative ordering
        m_control.Controls.SetChildIndex(bar, pos++);
        m_control.Controls.SetChildIndex(group.GroupControl, pos);
      }
            
      // Allocate space for the new child
      AllocateSpace(group);
            
      // Update child layout to reflect new proportional spacing values
      RepositionChildren();

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;

      return group;
    }

    internal void Replace(TabGroupBase orig, TabGroupBase replace)
    {
      // Find array position of old item
      int origPos = m_children.IndexOf(orig);
            
      // Transfer across the space occupied
      replace.RealSpace = orig.RealSpace;
            
      // Is this the only Window entry?
      if (m_children.Count == 1)
      {
        // Remove Window from appearance

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(m_control.Controls, 0);
      }
      else
      {
        int pos = 0;

        // Calculate position of Window to remove				
        if (origPos != 0)
          pos = origPos * 2 - 1;

        // Remove Window and bar 

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(m_control.Controls, pos);
        ControlHelper.RemoveAt(m_control.Controls, pos);
      }
            
      // Inserting at start of collection?
      if (origPos == 0)
      {
        if (m_children.Count > 1)
        {
          // Create a resizing bar
          ResizeBar bar = new ResizeBar(m_direction, this);
                
          // Append resize bar between existing entries and new entry
          m_control.Controls.Add(bar);

          // Reposition the bar and group to start of collection
          m_control.Controls.SetChildIndex(bar, 0);
        }            

        // Append new group control
        m_control.Controls.Add(replace.GroupControl);

        // Reposition the bar and group to start of collection
        m_control.Controls.SetChildIndex(replace.GroupControl, 0);
      }
      else
      {
        // Create a resizing bar
        ResizeBar bar = new ResizeBar(m_direction, this);
                
        // Append resize bar between existing entries and new entry
        m_control.Controls.Add(bar);

        // Append new group control
        m_control.Controls.Add(replace.GroupControl);
                    
        // Find correct index taking into account number of resize bars
        int	pos = origPos * 2 - 1;

        // Reposition the bar and Window to correct relative ordering
        m_control.Controls.SetChildIndex(bar, pos++);
        m_control.Controls.SetChildIndex(replace.GroupControl, pos);
      }
            
      // Update parentage
      replace.SetParent(this);
            
      // Replace the entry
      m_children[origPos] = replace;
            
      // Update child layout to reflect new proportional spacing values
      RepositionChildren();

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }

    protected void AllocateSpace(TabGroupBase newGroup)
    {
      // Is this the only group?
      if (m_children.Count == 1)
      {
        // Give it all the space
        newGroup.Space = 100m;
      }
      else
      {
        // Calculate how much space it should have
        Decimal newSpace = 100m / m_children.Count;

        // How much space should we steal from each of the others
        Decimal reduceSpace = newSpace / (m_children.Count - 1);

        // Actual space acquired so far 
        Decimal allocatedSpace = 0m;

        foreach(TabGroupBase group in m_children)
        {
          // Only process existing entries, not the new one
          if (group != newGroup)
          {
            // How much space does the group currently have
            Decimal currentSpace = group.Space;

            // How much space to steal from it
            Decimal xferSpace = reduceSpace;

            // If group has less space then we want, just steal all it has
            if (currentSpace < xferSpace)
              xferSpace = currentSpace;

            // Transfer the space across
            currentSpace -= xferSpace;
                        
            // Round the sensible number of decimal places
            currentSpace = Decimal.Round(currentSpace, SPACE_PRECISION);
                        
            // Update window with new space allocation
            group.Space = currentSpace;

            // Total up total space of all entries except new one
            allocatedSpace += currentSpace;
          }
        }

        // Assign all remaining space to new entry
        newGroup.Space = 100m - allocatedSpace;
      }
    }            
        
    protected void RemoveWindowSpace(Decimal space)
    {
      // Are there any children to process?
      if (m_children.Count != 0)
      {
        // Is there only a single group left?
        if (m_children.Count == 1)
        {
          // Give it all the space
          m_children[0].Space = 100m;
        }
        else
        {
          // Is there any space to reallocate?
          if (space > 0)
          {
            // Total up all the values
            Decimal totalAllocated = 0m;

            // How much space should we add to each of the others
            Decimal freedSpace = space / m_children.Count;

            foreach(TabGroupBase group in m_children)
            {
              // We only retain a sensible level of precision
              Decimal newSpace = Decimal.Round(group.Space + freedSpace, SPACE_PRECISION);

              // Assign back new space
              group.Space = newSpace;

              // Total up all space so far 
              totalAllocated += newSpace;
            }

            // Look for minor errors because not all fractions can be accurately represented in binary!
            if (totalAllocated > 100m)
            {
              // Remove from first entry
              m_children[0].Space -= totalAllocated - 100m;
            }
            else if (totalAllocated < 100m)
            {
              // Add to first entry
              m_children[0].Space += 100m - totalAllocated;
            }
          }
        }
      }
    }
                
    protected void RepositionChildren()
    {
      // Area available for repositioning
      Rectangle clientRect = m_control.ClientRectangle;

      // Is there anything to reposition?
      if (m_children.Count > 0)
      {
        // Space available for allocation
        int space;
			
        // Starting position of first control 
        int delta;

        // Values depend on sequence direction
        if (m_direction == Direction.Vertical)
        {
          space = clientRect.Height;
          delta = clientRect.Top;
        }
        else
        {
          space = clientRect.Width;
          delta = clientRect.Left;
        }

        // Ensure this is not negative
        if (space < 0)
          space = 0;

        int barSpace = 0;
        int allocation = space;

        // Create temporary array of working values
        int[] positions = new int[m_control.Controls.Count];

        // Pass 1, allocate all the space needed for each ResizeBar and the 
        //         minimal amount of space that each group requests. 
        AllocateMandatorySizes(ref positions, ref barSpace, ref space);

        // Is there any more space left?
        if (space > 0)
        {
          // Pass 2, allocate any space left over according to the requested
          //         percent space that each group would like to achieve.
          AllocateRemainingSpace(ref positions, space);
        }

        // Pass 3, reposition the controls according to allocated values.
        RepositionChildren(ref positions, clientRect, delta);
      }
    }

    protected void AllocateMandatorySizes(ref int[] positions, ref int barSpace, ref int space)
    {
      // Process each control
      for(int index=0, child=0; index<m_control.Controls.Count; index++)
      {
        ResizeBar bar = m_control.Controls[index] as ResizeBar;

        // Is this a resize bar control?
        if (bar != null)
        {
          // Length needed is dependant on direction 
          positions[index] = bar.Length;

          // Add up how much space was allocated to ResizeBars
          barSpace += positions[index];
        }
        else
        {
          Size minimal = m_children[child++].MinimumSize;

          // Length needed is depends on direction 
          if (m_direction == Direction.Vertical)
            positions[index] = minimal.Height;
          else
            positions[index] = minimal.Width;
        }

        // Reduce available space by that just allocated
        space -= positions[index];
      }			
    }

    protected void AllocateRemainingSpace(ref int[] positions, int windowSpace)
    {
      // Space allocated so far
      int allocated = 0;

      // Process each control
      for(int index=0, childIndex=0; index<m_control.Controls.Count; index++)
      {
        Control c = m_control.Controls[index];
                
        bool isResizeBar = (c is ResizeBar);

        // We do not allocate any more space for resize bars
        if (!isResizeBar)
        {
          int extra;
					
          // How much of remaining space does the group request to have?
          extra = (int)(windowSpace / 100m * m_children[childIndex++].Space);

          // Is this the last group to be positioned?
          if (childIndex == m_children.Count)
          {
            // Use up all the remaining space, this handles the case of the above 
            // vector calculation giving rounding errors so that the last element 
            // needs adusting to fill exactly all the available space
            extra = windowSpace - allocated;
          }

          // Add the extra space to any existing space it has
          positions[index] += extra;

          // Keep count of all allocated so far
          allocated += extra;
        }
      }
    }

    protected void RepositionChildren(ref int[] positions, Rectangle clientRect, int delta)
    {
      // Process each control 
      for(int index=0; index<m_control.Controls.Count; index++)
      {
        // Delta length for this particular control
        int newDelta = positions[index];

        ResizeBar bar = m_control.Controls[index] as ResizeBar;

        if (bar != null)
        {
          if (m_direction == Direction.Vertical)
          {
            // Set new position
            bar.Location = new Point(clientRect.X, delta);
            bar.Width = clientRect.Width;
            bar.Height = newDelta;

            // Move delta down to next position
            delta += newDelta;
          }
          else
          {
            // Set new position
            bar.Location = new Point(delta, clientRect.Y);
            bar.Height = clientRect.Height;
            bar.Width = newDelta;

            // Move delta across to next position
            delta += newDelta;
          }
        }
        else
        {
          Control c = m_control.Controls[index];

          if (c != null)
          {
            if (newDelta == 0)
              c.Hide();
            else
            {
              // Set new position/size based on direction
              if (m_direction == Direction.Vertical)
              {
                c.Location = new Point(clientRect.X, delta);
                c.Width = clientRect.Width;
                c.Height = newDelta;
              }
              else
              {
                c.Location = new Point(delta, clientRect.Y);
                c.Height = clientRect.Height;
                c.Width = newDelta;
              }

              if (!c.Visible)
                c.Show();

              // Move delta to next position
              delta += newDelta;
            }
          }
        }
      }			
    }
        
    protected void OnControlResize(object sender, EventArgs e)
    {
      // Change the layout of the children to match new size
      RepositionChildren();
    }
        
    public bool CanResize(ResizeBar bar)
    {
      // Cannot resize when in prominent mode
      if (!m_tabbedGroups.ResizeBarLock && (m_tabbedGroups.ProminentLeaf == null))
      {
        // Find position of this ResizeBar in the Controls collection
        int barIndex = m_control.Controls.IndexOf(bar);
                
        // Convert from control to children indexing
        int beforeIndex = (barIndex - 1) / 2;

        TabGroupBase before = m_children[beforeIndex];
        TabGroupBase after = m_children[beforeIndex + 1];

        // If groups on both sides have no space then cannot resize there relative positions
        if (((before.Space <= 0m) && (after.Space <= 0m)))
          return false;
        else
          return true;
      }
      else
      {
        // Must exit prominent mode before resize can occur
        return false;
      }
    }

    public bool StartResizeOperation(ResizeBar bar, ref Rectangle screenBoundary)
    {
      // Find position of this ResizeBar in the Controls collection
      int barIndex = m_control.Controls.IndexOf(bar);
            
      // Convert from control to children indexing
      int beforeIndex = (barIndex - 1) / 2;

      // Get groups before and after the resize bar
      TabGroupBase before = m_children[beforeIndex];
      TabGroupBase after = m_children[beforeIndex + 1];

      // Resizing boundary is defaulted to whole control area
      screenBoundary = m_control.RectangleToScreen(m_control.ClientRectangle);

      // Find screen rectangle for the groups either side of the bar
      Rectangle rectBefore = before.GroupControl.RectangleToScreen(before.GroupControl.ClientRectangle);
      Rectangle rectAfter = after.GroupControl.RectangleToScreen(after.GroupControl.ClientRectangle);

      // Reduce the boundary in the appropriate direction
      if (m_direction == Direction.Vertical)
      {
        screenBoundary.Y = rectBefore.Y + before.MinimumSize.Height;
        screenBoundary.Height -= screenBoundary.Bottom - rectAfter.Bottom;
        screenBoundary.Height -= after.MinimumSize.Height;
      }
      else
      {
        screenBoundary.X = rectBefore.X + before.MinimumSize.Width;
        screenBoundary.Width -= screenBoundary.Right - rectAfter.Right;
        screenBoundary.Width -= after.MinimumSize.Width;
      }

      // Allow resize operation to occur
      return true;
    }
        
    public void EndResizeOperation(ResizeBar bar, int delta)
    {
      // Find position of this ResizeBar in the Controls collection
      int barIndex = m_control.Controls.IndexOf(bar);
            
      // Convert from control to children indexing
      int beforeIndex = (barIndex - 1) / 2;

      // The Window relating to this bar must be the one before it in the collection
      TabGroupBase before = m_children[beforeIndex];

      // Is the Window being expanded
      DeltaGroupSpace(before, delta);
    }

    protected void DeltaGroupSpace(TabGroupBase group, int vector)
    {
      Rectangle clientRect = m_control.ClientRectangle;

      // Space available for allocation
      int space;

      // New pixel length of the modified group
      int newLength = vector;
			
      if (m_direction == Direction.Vertical)
      {
        space = clientRect.Height;

        // New pixel size is requested change plus original 
        // height minus the minimal size that is always added
        newLength += group.GroupControl.Height;
        newLength -= group.MinimumSize.Height;
      }
      else
      {
        space = clientRect.Width;

        // New pixel size is requested change plus original 
        // width minus the minimal size that is always added
        newLength += group.GroupControl.Width;
        newLength -= group.MinimumSize.Width;
      }

      int barSpace = 0;

      // Create temporary array of working values
      int[] positions = new int[m_control.Controls.Count];

      // Pass 1, allocate all the space needed for each ResizeBar and the 
      //         minimal amount of space that each Window requests. 
      AllocateMandatorySizes(ref positions, ref barSpace, ref space);

      // What is the new percentage it needs?
      Decimal newPercent = 0m;

      // Is there any room to allow a percentage calculation
      if ((newLength > 0) && (space > 0))
        newPercent = (Decimal)newLength / (Decimal)space * 100m;

      // What is the change in area
      Decimal reallocate = newPercent - group.Space;

      // Find the group after this one
      TabGroupBase nextGroup = m_children[m_children.IndexOf(group) + 1];

      if ((nextGroup.Space - reallocate) < 0m)
        reallocate = nextGroup.Space;
	
      // Modify the Window in question
      group.Space += reallocate;

      // Reverse modify the Window afterwards
      nextGroup.Space -= reallocate;
			
      // Update the visual appearance
      RepositionChildren();
    }
        
    internal Control.ControlCollection ChildControls
    {
      get { return m_control.Controls; }
    }
  }
}


#region revision history

// $Log: TabGroupSequence.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/TabGroupSequence.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: TabGroupSequence.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

