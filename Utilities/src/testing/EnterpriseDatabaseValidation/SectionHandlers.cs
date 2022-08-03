// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql
{

  #region ValidationTestSectionHandler

  /// <summary>
  /// Gets a list of database validation tests.
  /// </summary>
  public class ValidationTestSectionHandler : IConfigurationSectionHandler
  {

    /// <summary>
    /// Creates the section handler.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="section">The section we should handle.</param>
    /// <returns>A list of names that need to be checked for existence.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<IDatabaseValidationTest> entries = new List<IDatabaseValidationTest>();

      foreach (XmlNode node in section.SelectNodes("validationTest"))
      {
        IDatabaseValidationTest entry = (IDatabaseValidationTest)Activator.CreateInstance(Type.GetType(node.Attributes["type"].Value));
        entries.Add(entry);
      }

      return entries;
    } // Create( parent, configContext, section )

  } // class ValidationTestSectionHandler

  #endregion

  #region ObjectsSectionHandler

  /// <summary>
  /// A section handler to handle the items that need to be checked for existance section.
  /// </summary>
  public class ObjectsSectionHandler : IConfigurationSectionHandler
  {

    /// <summary>
    /// Creates the section handler.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="section">The section we should handle.</param>
    /// <returns>A list of names that need to be checked for existence.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<string> entries = new List<string>();

      foreach (XmlNode entry in section.SelectNodes("object"))
      {
        entries.Add(entry.Attributes["name"].Value);
      }

      return entries;
    } // Create( parent, configContext, section )

  } // class ObjectsSectionHandler

  #endregion

  #region DatabasesSectionHandler

  /// <summary>
  /// Handles the databases configuration section.
  /// </summary>
  public class DatabasesSectionHandler : IConfigurationSectionHandler
  {

    /// <summary>
    /// Creates the section handler.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="section">The section we should handle.</param>
    /// <returns>A list of names that need to be checked for existence.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<string> entries = new List<string>();

      foreach (XmlNode entry in section.SelectNodes("database"))
      {
        entries.Add(entry.Attributes["name"].Value);
      }

      return entries;
    } // Create( parent, configContext, section )

  } // class DatabasesSectionHandler

  #endregion

  #region VersionedObjectsSectionHandler

  /// <summary>
  /// Section handler for the VersionedObjects section.
  /// </summary>
  public class VersionedObjectsSectionHandler : IConfigurationSectionHandler
  {

    /// <summary>
    /// Creates the section handler.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="section">The section we should handle.</param>
    /// <returns>A list of names paired with versiosn that need to be checked for existence.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      Dictionary<string, string> entries = new Dictionary<string, string>();

      foreach (XmlNode entry in section.SelectNodes("versionedObject"))
      {
        entries.Add(entry.Attributes["name"].Value, entry.Attributes["version"].Value);
      }

      return entries;
    } // Create( parent, configContext, section )

  } // class VersionedObjectsSectionHandler

  #endregion

  #region DBSettingsSectionHandler

  /// <summary>
  /// Handles the databse settings config section.
  /// </summary>
  public class DBSettingsSectionHandler : IConfigurationSectionHandler
  {

    /// <summary>
    /// Creates the section handler.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="section">The section we should handle.</param>
    /// <returns>A list of tuples having subsystem, name and value.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<Tuple<string, string, string>> entries = new List<Tuple<string, string, string>>();

      foreach (XmlNode entry in section.SelectNodes("dbSetting"))
      {
        entries.Add(new Tuple<string, string, string>(entry.Attributes["subsystem"].Value, entry.Attributes["name"].Value, entry.Attributes["value"].Value));
      }

      return entries;
    } // Create( parent, configContext, section )

  } // class DBSettingsSectionHandler

  #endregion

} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql
