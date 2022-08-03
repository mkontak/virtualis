// $Id: WizardPage.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [Designer(typeof(ParentControlDesigner))]
  public class WizardPage : Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage
  {
    // Instance fields
    protected bool m_fullPage;
    protected string m_subTitle;
    protected string m_captionTitle;
        
    // Instance events
    public event EventHandler FullPageChanged;
    public event EventHandler SubTitleChanged;
    public event EventHandler CaptionTitleChanged;
    
    public WizardPage()
    {
      m_fullPage = false;
      m_subTitle = "(Page Description not defined)";
      m_captionTitle = "(Page Title)";
    }
		
    public bool FullPage
    {
      get { return m_fullPage; }
		    
      set 
      {
        if (m_fullPage != value)
        {
          m_fullPage = value;
          OnFullPageChanged(EventArgs.Empty);
        }
      }
    }
		
    public string SubTitle
    {
      get { return m_subTitle; }

      set 
      {
        if (m_subTitle != value)
        {
          m_subTitle = value;
          OnSubTitleChanged(EventArgs.Empty);
        }
      }
    }
		
    public string CaptionTitle
    {
      get { return m_captionTitle; }
		    
      set
      {
        if (m_captionTitle != value)
        {
          m_captionTitle = value;
          OnCaptionTitleChanged(EventArgs.Empty);
        }
      }
    }
		
    public virtual void OnFullPageChanged(EventArgs e)
    {
      if (FullPageChanged != null)
        FullPageChanged(this, e);
    }
    
    public virtual void OnSubTitleChanged(EventArgs e)
    {
      if (SubTitleChanged != null)
        SubTitleChanged(this, e);
    }

    public virtual void OnCaptionTitleChanged(EventArgs e)
    {
      if (CaptionTitleChanged != null)
        CaptionTitleChanged(this, e);
    }
  }
}

#region revision history

// $Log: WizardPage.cs,v $
// Revision 1.1  2005/10/11 14:42:12  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/WizardControl/WizardPage.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
// $Id: WizardPage.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $

#endregion

