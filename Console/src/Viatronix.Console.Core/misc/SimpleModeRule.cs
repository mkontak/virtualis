// $Id: SimpleModeRule.cs,v 1.1.2.6 2008/01/16 14:54:09 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{

  #region SimpleModeRule class

  public class SimpleModeRule
  {
    #region fields

    /// <summary>
    /// sub type
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// Series type
    /// </summary>
    private string m_seriesTtype = string.Empty;

    /// <summary>
    /// Preprocessed as
    /// </summary>
    private string m_preprocessedAs = string.Empty;

    /// <summary>
    /// Viewable flafg
    /// </summary>
    private bool m_viewable = true;

    /// <summary>
    /// References
    /// </summary>
    private string m_references = string.Empty;

    /// <summary>
    /// Dependents
    /// </summary>
    private string m_dependents = string.Empty;

    #endregion


    #region constructors

    /// <summary>
    /// Constructpr
    /// </summary>
    public SimpleModeRule()
    {
    } // SimpleModeRule()

    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the references
    /// </summary>
    public string References
    {
      get { return m_references; }
      set { m_references = value; }
    }

    /// <summary>
    /// Sets/Get the dependents
    /// </summary>
    public string Dependents
    {
      get { return m_dependents; }
      set { m_dependents = value; }
    }

    /// <summary>
    /// Set/Get the sub type
    /// </summary>
    public string Application
    {
      get { return m_application; }
      set { m_application = value; }
    }

    /// <summary>
    /// Set/Get the series type 
    /// </summary>
    public string SeriesType
    {
      get { return m_seriesTtype; }
      set { m_seriesTtype = value; }
    }

    /// <summary>
    /// Sets/Gets the preprocess as value
    /// </summary>
    public string PreprocessedAs
    {
      get { return m_preprocessedAs; }
      set { m_preprocessedAs = value; }
    }

    /// <summary>
    /// Stes/gets the viewable flag
    /// </summary>
    public bool Viewable
    {
      get { return m_viewable; }
      set { m_viewable = value; }
    }

    #endregion

    #region methods

    public override int GetHashCode()
    {
      return base.GetHashCode();
    }

    /// <summary>
    /// Determine equality
    /// </summary>
    /// <param name="rule">Rule to compare against</param>
    /// <returns></returns>
    public bool Equals(SimpleModeRule rule)
    {
      return (string.Compare(rule.Application, m_application, true) == 0 && string.Compare(rule.PreprocessedAs, m_preprocessedAs, true) == 0 && string.Compare(rule.SeriesType, m_seriesTtype, true) == 0);
    }

    /// <summary>
    /// Determine equality
    /// </summary>
    /// <param name="obj">Objhect to compare against</param>
    /// <returns></returns>
    public override bool Equals(object obj)
    {
      return Equals((SimpleModeRule)obj);
    }

 
    /// <summary>
    /// Creates a SimpleMideRUle from the xml node
    /// </summary>
    /// <param name="node">Simple Mode Rule</param>
    /// <returns>SimpleModeRule</returns>
    public static SimpleModeRule FromXml(XmlNode node)
    {
      SimpleModeRule rule = new SimpleModeRule();

      rule.Application = node.Attributes["app"].Value.ToString();
      rule.SeriesType = node.Attributes["seriesType"].Value.ToString();
      rule.PreprocessedAs = (node.Attributes["preprocessedAs"] != null ? node.Attributes["preprocessedAs"].Value.ToString() : string.Empty);
      rule.References = (node.Attributes["references"] != null ? node.Attributes["references"].Value.ToString() : string.Empty);
      rule.Dependents = (node.Attributes["dependents"] != null ? node.Attributes["dependents"].Value.ToString() : string.Empty);

      rule.Viewable = System.Convert.ToBoolean(node.Attributes["viewable"].Value);

      return rule;
    }

    #endregion



  } // class SimpleModeRule

  #endregion


  #region SimpleModeRuleCollection class

  /// <summary>
  /// Implementation of a SimpleModeRule collection
  /// </summary>
  public class SimpleModeRuleCollection : CollectionBase
  {

    #region construction

    /// <summary>
    /// Creates a new instance of a SimpleModeRuleCollection.
    /// </summary>
    public SimpleModeRuleCollection()
    {
    } // SimpleModeRuleCollection()

    #endregion

    #region properties

    /// <summary>
    /// Gets a SimpleModeRule object at the specified index in the collection.
    /// </summary>
    public SimpleModeRule this[int index]
    {
      get { return (SimpleModeRule)List[index]; }
    } // Indexer

    #endregion

    #region typed methods


    /// <summary>
    /// Determines if the suplied series is viewable based on the rules
    /// </summary>
    /// <param name="series">Series to check</param>
    /// <returns>TRue if viewable, false othwerwise</returns>
    public bool IsViewable(Series series)
    {

      bool viewable = true;
      
      ////////////////////////////////////////////////////
      // Loop through all the rules to check the series
      ////////////////////////////////////////////////////
      foreach( SimpleModeRule rule in List )
      {

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // Make sure the series matches the rule using the subtype, series type and processed as fields
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if ((string.IsNullOrEmpty(rule.Application) || string.Compare(rule.Application, series.Application, true) == 0) &&
           // TODO: REST  Removed since the AS has been removed
           //(string.IsNullOrEmpty(rule.PreprocessedAs) || string.Compare(rule.PreprocessedAs, series.PreprocessAs, true) == 0) &&
           //
           (string.IsNullOrEmpty(rule.SeriesType) || string.Compare(rule.SeriesType, series.Type.ToString(), true) == 0) )
        {

          ////////////////////////////////////////////////////////////////////////////////////////////////////////
          // If there are references defined then look at then otherwise set the return code to the viewable flag
          ////////////////////////////////////////////////////////////////////////////////////////////////////////
          if ( !string.IsNullOrEmpty(rule.References))
          {
            foreach (Series refSeries in series.References)
            {
              ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              // If the series contains a reference of the type specified in the rule then set the return code the viewable flag
              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              if (string.Compare(refSeries.Type.ToString(), rule.References, true) == 0)
              {
                viewable = rule.Viewable;
                break;
              } // END ... If the series type is referenced
            } // END ... For each referenced series

          }
          else
          {
             viewable = rule.Viewable;
             break;
          }
        } // END ... If the series matches the rule
      } // END ... for each rule

       return viewable;
    }


    /// <summary>
    /// Finds the SimpleModeRule specified in the list
    /// </summary>
    /// <param name="SimpleModeRule">SimpleModeRule to search for in collection</param>
    /// <returns>SimpleModeRule if found, null otherwise</returns>
    public SimpleModeRule Find(SimpleModeRule SimpleModeRule)
    {
      foreach (SimpleModeRule p in List)
      {
        if (p.Equals(SimpleModeRule))
        {
          return p;
        }
      }

      return null;

    } // Find(SimpleModeRule SimpleModeRule)

    /// <summary>
    /// Removes the SimpleModeRule specified
    /// </summary>
    /// <param name="SimpleModeRule">SimpleModeRule to be removed</param>
    public void Remove(SimpleModeRule SimpleModeRule)
    {
      for (int i = 0; i < List.Count; ++i)
      {
        SimpleModeRule p = (SimpleModeRule)List[i];
        if (p.Equals(SimpleModeRule))
        {
          RemoveAt(i);
          break;
        }
      }

    } // Remove(SimpleModeRule SimpleModeRule)

    /// <summary>
    /// Add SimpleModeRule to collection
    /// </summary>
    /// <param name="SimpleModeRule">SimpleModeRule to add</param>
    public void Add(SimpleModeRule SimpleModeRule)
    {
      /////////////////////////////////////////////
      // Only add the SimpleModeRule if it does not exist
      ////////////////////////////////////////////
      if (!Contains(SimpleModeRule))
      {
        List.Add(SimpleModeRule);
      } // END ... If the SimpleModeRule does not exists in the collection
    } // Add( SimpleModeRule SimpleModeRule )

    /// <summary>
    /// Adds all SimpleModeRules from the collection to this collection
    /// </summary>
    /// <param name="SimpleModeRules">SimpleModeRule collection to add</param>
    public void Add(SimpleModeRuleCollection SimpleModeRules)
    {
      SimpleModeRule[] array = new SimpleModeRule[SimpleModeRules.Count];
      SimpleModeRules.CopyTo(array, 0);
      Add(array);
    } // Add( SimpleModeRuleCollection SimpleModeRules )

    /// <summary>
    /// Adds all the SimpleModeRules in the array to this collection
    /// </summary>
    /// <param name="SimpleModeRules">SimpleModeRules to be added</param>
    public void Add(SimpleModeRule[] SimpleModeRules)
    {
      foreach (SimpleModeRule SimpleModeRule in SimpleModeRules)
      {
        Add(SimpleModeRule);
      }
    } // Add( SimpleModeRule [] SimpleModeRules )

    /// <summary>
    /// Copies the collection to the supplied array
    /// </summary>
    /// <param name="array">Array to copy to</param>
    /// <param name="index">Index ti start at</param>
    public void CopyTo(Array array, int index)
    {
      List.CopyTo(array, index);
    } // CopyTo( Array array, int index )


    /// <summary>
    /// Determines if the SimpleModeRule is in the collection
    /// </summary>
    /// <param name="SimpleModeRule">SimpleModeRule to check</param>
    /// <returns>True if exists, flase otherwise</returns>
    public bool Contains(SimpleModeRule SimpleModeRule)
    {
      foreach (SimpleModeRule p in List)
      {
        if (p.Equals(SimpleModeRule))
        {
          return true;
        }
      }

      return false;
    } // Contains( SimpleModeRule SimpleModeRule )




    #endregion

    #region override methods

    /// <summary>
    /// Performs additional custom processes when validating a value.
    /// </summary>
    /// <param name="value">The object to validate.</param>
    protected override void OnValidate(object value)
    {
      if (!(value is SimpleModeRule))
        throw new ArgumentException("The specified value is not of type SimpleModeRule.");
    } // OnValidate( value )

    #endregion

  } // class SimpleModeRuleCollection

  #endregion


}


#region revision history

// $Log: SimpleModeRule.cs,v $
// Revision 1.1.2.6  2008/01/16 14:54:09  mkontak
// no message
//
// Revision 1.1.2.5  2008/01/10 18:30:24  mkontak
// Added new features
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/SimpleModeRule.cs,v 1.1.2.6 2008/01/16 14:54:09 mkontak Exp $
// $Id: SimpleModeRule.cs,v 1.1.2.6 2008/01/16 14:54:09 mkontak Exp $

#endregion
