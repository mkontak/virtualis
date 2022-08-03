using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Collections.Generic;
using System.Data.Common;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Tests the get licenses stored proc.
  /// </summary>
  [TestClass]
  public class GetLicenseTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      LicensingGateway.RegisterLicenseDatabase();
    } // TestInitialize()


    /// <summary>
    /// Cleans up after each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Adds two licenses and then verifies they were added, using the get licenses stored proc.
    /// </summary>
    [TestMethod]
    public void GetLic_TestSuccess()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(LicensingGateway.GetLicense("<license />"));

      Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "");
    } // GetLic_TestSuccess()


    /// <summary>
    /// Adds two licenses, marks them as disabled, then attempts to query on disabled licenses.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByDisabledState()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses();
        foreach (Tuple<string, LicensingGateway.LicenseStates> license in licenses)
        {
          LicensingGateway.DisableLicense(license.Item1);
        }

        string xml = LicensingGateway.GetLicense(string.Format("<license state=\"{0}\" />", (int)LicensingGateway.LicenseStates.Disabled));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes(string.Format("/collection[@id=\"license\"]/license[@state=\"{0}\"]", (int)LicensingGateway.LicenseStates.Disabled)).Count, "The get licenses could not retrieve the disabled licenses correctly.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByDisabledState()


    /// <summary>
    /// Adds two licenses, marks them as locked, then attempts to query on locked licenses.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByLockedState()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses();
        foreach (Tuple<string, LicensingGateway.LicenseStates> license in licenses)
        {
          LicensingGateway.LockLicense(license.Item1);
        }

        string xml = LicensingGateway.GetLicense(string.Format("<license state=\"{0}\" />", (int)LicensingGateway.LicenseStates.Locked));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes(string.Format("/collection[@id=\"license\"]/license[@state=\"{0}\"]", (int)LicensingGateway.LicenseStates.Locked)).Count, "The get licenses could not retrieve the locked licenses correctly.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByLockedState()


    /// <summary>
    /// Tests acquiring licenses that are in use.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByInUseState()
    {
      string licenseKey01 = string.Empty;
      string licenseKey02 = string.Empty;

      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        licenseKey01 = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
        licenseKey02 = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

        string xml = LicensingGateway.GetLicense(string.Format("<license state=\"{0}\" />", (int)LicensingGateway.LicenseStates.InUse));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes(string.Format("/collection[@id=\"license\"]/license[@state=\"{0}\"]", (int)LicensingGateway.LicenseStates.InUse)).Count, "The get licenses could not retrieve the inuse licenses correctly.");
      }
      finally
      {
        LicensingGateway.ReleaseLicense(licenseKey01);
        LicensingGateway.ReleaseLicense(licenseKey02);

        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByInUseState()


    /// <summary>
    /// Tests querying by the added date.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByAddedDate()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        // Test an inclusive search just covers the time frme around the licenses being added.
        string query = string.Format("<license><add from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.Now.Subtract(new TimeSpan(0, 0, 1, 0)).ToString("o"), DateTimeOffset.Now.AddMinutes(1).ToString("o"));
        string xml = LicensingGateway.GetLicense(query);

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query around the added date did not produce the expected results.");

        // Test an exclusive search that covers the time before the licenses were added.
        query = string.Format("<license><add from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.MinValue.ToString("o"), DateTimeOffset.Now.Subtract(new TimeSpan(0, 1, 0)).ToString("o"));
        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query before the added date did not produce the expected results.");

        // Test an exclusive search that covers the time after the licenses were added.
        query = string.Format("<license><add from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.Now.AddDays(1).ToString("o"), DateTimeOffset.MaxValue.ToString("o"));
        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query after the added date did not produce the expected results.");

        // Test an inclusive search that covers all time.
        query = string.Format("<license><add from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.MinValue.ToString("o"), DateTimeOffset.MaxValue.ToString("o"));
        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query from the beginning of time to the end of time did not produce the expected results.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByAddedDate()


    /// <summary>
    /// Tests querying by the issued date.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByIssuedDate()
    {
      try
      {
        DateTimeOffset issuedDate = DateTimeOffset.Now.Subtract(new TimeSpan(45, 0, 0, 0));
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, issuedDate, "kchalupa", 2, 120);

        // Test an inclusive search just covers the time frme around the licenses being added.
        string query = string.Format("<license><iss from=\"{0}\" to=\"{1}\" /></license>",
          issuedDate.Subtract(new TimeSpan(0, 1, 0)).ToString("o"), issuedDate.AddMinutes(1).ToString("o"));

        string xml = LicensingGateway.GetLicense(query);

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query around the issued date did not produce the expected results.");

        // Test an exclusive search that covers the time before the licenses were added.
        query = string.Format("<license><iss from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.MinValue.ToString("o"),
          issuedDate.Subtract(new TimeSpan(0, 1, 0)).ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query before the issued date did not produce the expected results.");

        // Test an exclusive search that covers the time after the licenses were added.
        query = string.Format("<license><iss from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.Now.AddMinutes(1).ToString("o"), DateTimeOffset.MaxValue.ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query after the issued date did not produce the expected results.");

        // Test an inclusive search that covers all time.
        query = string.Format("<license><iss from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.MinValue.ToString("o"), DateTimeOffset.MaxValue.ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query from the beginning of time to the end of time did not produce the expected results.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByIssuedDate()


    /// <summary>
    /// Tests querying by the expiration date.
    /// </summary>
    [TestMethod]
    public void GetLic_TestQueryByExpirationDate()
    {
      try
      {
        DateTimeOffset expirationDate = DateTimeOffset.Now.AddDays(120);
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        // Test an inclusive search just covers the time frme around the licenses' expired.
        string query = string.Format("<license><exp from=\"{0}\" to=\"{1}\" /></license>",
          expirationDate.Subtract(new TimeSpan(0, 0, 1, 0)).ToString("o"), expirationDate.AddMinutes(1).ToString("o"));

        string xml = LicensingGateway.GetLicense(query);

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query around the expiration date did not produce the expected results.");

        // Test an exclusive search that covers the time before the licenses were added.
        query = string.Format("<license><exp from=\"{0}\" to=\"{1}\" /></license>",
          DateTimeOffset.MinValue.ToString("o"), expirationDate.Subtract(new TimeSpan(0, 1, 0)).ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query prior to the expiration date did not produce the expected results.");

        // Test an exclusive search that covers the time after the licenses were added.
        query = string.Format("<license><exp from=\"{0}\" to=\"{1}\" /></license>", expirationDate.AddMinutes(1).ToString("o"),
          DateTimeOffset.MaxValue.ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(0, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query after the expiration date did not produce the expected results.");

        // Test an inclusive search that covers all time.
        query = string.Format("<license><exp from=\"{0}\" to=\"{1}\" /></license>", DateTimeOffset.MinValue.ToString("o"),
          DateTimeOffset.MaxValue.ToString("o"));

        xml = LicensingGateway.GetLicense(query);

        doc = new XmlDocument();
        doc.LoadXml(xml);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count, "Performing a query from the beginning of time to the end of time did not produce the expected results.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestQueryByExpirationDate()

    #endregion

  } // class GetLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
