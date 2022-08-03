using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Configuration;
using System.Data.SqlClient;
using System.Data;

namespace Viatronix.Sql.Filtering
{


  public class Filter
  {


    #region fields


    /// <summary>
    /// Default flag
    /// </summary>
    private bool m_default = false;
 
    /// <summary>
    /// Value
    /// </summary>
    private string m_value = string.Empty;


    /// <summary>
    /// Rules list
    /// </summary>
    private List<Rule> m_rules = new List<Rule>();


    #endregion


    #region constructors




    #endregion



    #region properties

    /// <summary>
    /// Gets the value for this filter
    /// </summary>
    public string Value
    {
      get { return m_value; }
    } // Value


    /// <summary>
    /// Gets the default flag
    /// </summary>
    public bool Default
    {
      get { return m_default; }
    } // Default
 
    #endregion


    #region methods

    /// <summary>
    /// Dtermines of the header matches the filter
    /// </summary>
    /// <param name="header">Dicom header</param>
    /// <returns>True if a match is detected, false otherwise</returns>
    public bool IsMatch( XmlNode header )
    {

      // Aaume no match
      bool isMatch = false;

      // =======================================================================================================
      // Check all the rules for a match. Rules listed in the same level are considered OR opperations so the
      // first true that is hit we break out since there is no need to check any further.
      // =======================================================================================================
      foreach (Rule rule in m_rules)
      {

 
        if ((isMatch |= rule.IsMatch(header)))
          break;

      }
     


      return isMatch;
    }


    /// <summary>
    /// Load the rules
    /// </summary>
    /// <param name="series">Series</param> 
    /// <param name="task">task xml string</param>
    /// <param name="rules">filter xml node</param>
    public static Filter Create(XmlNode node)
    {


      Filter filter = new Filter();


      // Get the app associated with this filter
      filter.m_value   = node.Attributes["value"].Value.ToString();
      filter.m_default = (node.Attributes["default"] != null ? Boolean.Parse(node.Attributes["default"].Value.ToString()) : false);


      XmlNode rules = node.SelectSingleNode("rules");

      // =============================
      // Add all rules to the list
      // =============================
      foreach (XmlNode rule in rules.SelectNodes("rule"))
      {
        filter.m_rules.Add(Rule.Create(rule));
      }   // END ... For each 


      return filter;
    }  // Create(XmlNode rules)

    #endregion

  }
}
