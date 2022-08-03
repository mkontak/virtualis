using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Viatronix.Enterprise.Entities;


namespace Viatronix.v3D.Core
{
  public class PasswordHint
  {

    #region fields

    private string m_id = string.Empty;

    private string m_condition = string.Empty;

    private string m_transDescription = string.Empty;

    private string m_origDescription = string.Empty;

    #endregion


    #region methods


    #region properties


    public string Id
    { get { return m_id;  } }

    public string Condition
    { get { return m_condition;  } }

    public string Description
    { get { return m_transDescription; } }


    #endregion

    /// <summary>
    /// Creates the password hint from the xml
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns></returns>
    public static PasswordHint Create(XmlNode node)
    {

      PasswordHint hint = new PasswordHint();

      if (hint == null)
        throw new OutOfMemoryException("Failed to allocate PasswordHint");


      hint.m_id = node.Attributes["id"].Value;
      hint.m_condition = node.Attributes["condition"].Value;
      hint.m_origDescription = hint.m_transDescription = node.Attributes["desc"].Value;

      return hint;

    } // Create(XmlNode node)


    /// <summary>
    /// Determines if the condition meets the conditions specified using the setting. If so then the translated description will be set.
    /// </summary>
    /// <param name="settings"></param>
    /// <returns></returns>
    public bool MeetsCondition(List<Setting> settings)
    {
      bool pass = (m_condition.Length == 0 );
      Setting setting = new Setting();

      char[] del = { ' ' };

      if (!pass)
      {

        string[] fields = m_condition.Split(del);

        string settingName = fields[0].Substring(2, fields[0].Length - 3);
        string condition = fields[1];
        string value = fields[2];
 
        if (settings.TryFind(settingName, ref setting))
        {

          string settingValue = setting.Value;

          try
          {


            if (condition == "==")
              pass = (settingValue == value);
            else if (condition == ">")
              pass = (Int32.Parse(settingValue) > Int32.Parse(value));
            else if (condition == "<")
              pass = (Int32.Parse(settingValue) < Int32.Parse(value));
          }
          catch (Exception)
          {
            Logging.Log.Warning("Invalid comparison not correct type", "PasswordHint", "MeetsCondition");
          }


        }
      }

      if (pass)
      {

        StringBuilder translated = new StringBuilder();

         string[] fields = m_origDescription.Split(del);

        for (int i = 0; i < fields.Length; ++i)
        {

          if (fields[i].StartsWith("${"))
          {

            bool containsPeriod = fields[i].EndsWith(".");
            string settingName = fields[i].Substring(2, fields[i].Length - (containsPeriod ? 4 : 3));

            if (! settings.TryFind(settingName, ref setting))
            {
              Logging.Log.Warning("Could not find macro [" + settingName + "] in system settings", "PasswordHintsConfigSectionHandler", "Create");
              continue;
            }
            else
              fields[i] = setting.Value + (containsPeriod ? "." : "");


          }

          translated.Append(fields[i] + " ");

        }

        m_transDescription = translated.ToString();
      }



      return pass;

    }




    #endregion


   
  }
}
