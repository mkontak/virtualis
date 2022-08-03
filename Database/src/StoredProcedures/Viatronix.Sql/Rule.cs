using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Text.RegularExpressions;


namespace Viatronix.Sql.Filtering
{

  /// <summary>
  /// Implements a rule used for determine a dataset type
  /// </summary>
  public class Rule
  {

    #region fields

    /// <summary>
    /// Dicom Element tag
    /// </summary>
    private string m_tag = string.Empty;

    /// <summary>
    /// Pattern
    /// </summary>
    private string m_pattern = string.Empty;

    /// <summary>
    /// Value
    /// </summary>
    private string m_value = string.Empty;

    /// <summary>
    /// Ignore case
    /// </summary>
    private bool m_ignoreCase = true;

    /// <summary>
    /// Sub rules 
    /// </summary>
    private List<Rule> m_rules = new List<Rule>();

    #endregion


    #region properties


    /// <summary>
    /// Gets the dicom element tag
    /// </summary>
    public string Tag
    {
      get { return m_tag; }
    } // Tag

    /// <summary>
    /// Gets the regular expression pattern
    /// </summary>
    public string Pattern
    {
      get { return m_pattern; }
    } // Pattern


    /// <summary>
    /// Ignore case
    /// </summary>
    public bool IgnoreCase
    {
      get { return m_ignoreCase; }
    }// IgnoreCase


    #endregion


    #region methods

    /// <summary>
    /// Creates the rule
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Rule Create(XmlNode node)
    {
      Rule rule = new Rule();

      rule.m_pattern = ( node.Attributes["pattern"] == null ? string.Empty : node.Attributes["pattern"].Value.ToString() );
      rule.m_value = (node.Attributes["value"] == null ? string.Empty : node.Attributes["value"].Value.ToString());
      rule.m_ignoreCase = (node.Attributes["ignoreCase"] == null ? false : Boolean.Parse(node.Attributes["ignoreCase"].Value));
      rule.m_tag = node.Attributes["tag"].Value.ToString();
 
      if (rule.m_pattern.Length > 0 && rule.m_value.Length > 0)
        throw new System.ApplicationException("Invalid rule : Can only have one a pattern or value");

      foreach (XmlNode sub in node.SelectNodes("rule"))
      {
        rule.m_rules.Add(Rule.Create(sub));
      }

      return rule;
    } //  Create(XmlNode node)

    /// <summary>
    /// 
    /// </summary>
    /// <param name="element"></param>
    /// <returns></returns>
    public bool IsMatch(XmlNode header)
    {

      bool isMatch = false;

      string xpath = string.Format("/element[@tag='{0}']", m_tag);

      XmlNode element = header.SelectSingleNode(xpath);

      if (element != null)
      {
        if (element.ChildNodes.Count > 0)
        {

          /// Get the value of the dicom element
          string value = element.FirstChild.Value.ToString();

          if (m_pattern.Length > 0)
          {
            /// Create the regualt expression 
            Regex rgx = (m_ignoreCase ? new Regex(m_pattern, RegexOptions.IgnoreCase) : new Regex(m_pattern));

            /// Check the value againast the regular expression for a match
            isMatch = rgx.IsMatch(value);
          }
          else
          {

            char[] separator = { '\\' };
            string[] values = m_value.Split(separator);

            // ================
            // For each value
            // ================
            foreach (string val in values)
            {
              if ( val.StartsWith("|") )
                isMatch |= (string.Compare(value, val.Substring(1), m_ignoreCase) == 0 ? true : false);
              if ( val.StartsWith("&") )
                isMatch &= (string.Compare(value, val.Substring(1), m_ignoreCase) == 0 ? true : false);
              else
                isMatch |= (string.Compare(value, val, m_ignoreCase) == 0 ? true : false);

            } // END ... For each value

          }

          // ==================
          // Check the match
          // ==================
          if (isMatch)
          {

            // ================================================
            // Loop through all the rules and check for a match
            // ================================================
            foreach (Rule rule in m_rules)
            {

              isMatch = isMatch && rule.IsMatch(header);

              /// Once we do not have a match there is no need to check anymore.
              if (isMatch)
                break;

            } // END ... For each rule

          } // END ... If the dicom header matches the rule
        }
      }

      return isMatch;

    } // IsMatch(XmlNode header)


    #endregion

  } // class Rule

} // namespace Viatronix.Enterprise.Filtering
