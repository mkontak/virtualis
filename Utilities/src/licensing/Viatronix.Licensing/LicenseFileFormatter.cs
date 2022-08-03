using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Licensing
{
  /// <summary>
  /// The license formatter.
  /// </summary>
  public class LicenseFileFormatter
  {

    #region fields

    /// <summary>
    /// The system id.
    /// </summary>
    private string m_systemId = string.Empty;

    /// <summary>
    /// Comments made by the customer service department.
    /// </summary>
    private string m_comments = string.Empty;

    /// <summary>
    /// Should we update the database expiration date?
    /// </summary>
    private bool m_resetSystemTimeToLiveDate = true;

    /// <summary>
    /// The system time to live date.
    /// </summary>
    private DateTime m_systemTimeToLiveDate = DateTime.Now;

    /// <summary>
    /// Should we install licenses.
    /// </summary>
    private bool m_shouldInstallLicenses = true;

    /// <summary>
    /// The transaction id.
    /// </summary>
    private Guid m_transactionId = Guid.NewGuid();

    /// <summary>
    /// The date issued.
    /// </summary>
    private DateTime m_dateIssued = DateTime.Now;

    /// <summary>
    /// Who issued the licenses.
    /// </summary>
    private string m_issuedBy = string.Empty;

    /// <summary>
    /// The number of licenses.
    /// </summary>
    private int m_numberLicenses = 0;

    /// <summary>
    /// The time to live on the licenses.
    /// </summary>
    private int m_licenseDuration = 0;

    /// <summary>
    /// The license type.
    /// </summary>
    private string m_licenseType = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the system id.
    /// </summary>
    public string SystemId
    {
      get { return m_systemId; }
      set { m_systemId = value; }
    } // SystemId


    /// <summary>
    /// Gets or sets the comments that ride along with the license file.
    /// </summary>
    public string Comments
    {
      get { return m_comments; }
      set { m_comments = value; }
    } // Comments


    /// <summary>
    /// Gets or sets whether we should reset the system time to live date.
    /// </summary>
    public bool ShouldResetSystemTimeToLiveDate
    {
      get { return m_resetSystemTimeToLiveDate; }
      set { m_resetSystemTimeToLiveDate = value; }
    } // ShouldResetSystemTimeToLiveDate


    /// <summary>
    /// Gets or sets the system ttl date.
    /// </summary>
    public DateTime SystemTimeToLiveDate
    {
      get { return m_systemTimeToLiveDate; }
      set { m_systemTimeToLiveDate = value; }
    } // SystemTimeToLiveDate


    /// <summary>
    /// Gets or sets whether we should install licenses.
    /// </summary>
    public Boolean ShouldInstallLicenses
    {
      get { return m_shouldInstallLicenses; }
      set { m_shouldInstallLicenses = value; }
    } // ShouldInstallLicenses


    /// <summary>
    /// Gets or sets the transaction id.
    /// </summary>
    public Guid TransactionId
    {
      get { return m_transactionId; }
      set { m_transactionId = value; }
    } // TransactionId


    /// <summary>
    /// Gets or sets the date issued.
    /// </summary>
    public DateTime DateIssued
    {
      get { return m_dateIssued; }
      set { m_dateIssued = value; }
    } // DateIssued


    /// <summary>
    /// Gets or sets the issued by.
    /// </summary>
    public string IssuedBy
    {
      get { return m_issuedBy; }
      set { m_issuedBy = value; }
    } // IssuedBy


    /// <summary>
    /// Gets or sets the number of licenses.
    /// </summary>
    public int NumberLicenses
    {
      get { return m_numberLicenses; }
      set { m_numberLicenses = value; }
    } // NumberLicenses


    /// <summary>
    /// Gets or sets the license duration.
    /// </summary>
    public int LicenseDuration
    {
      get { return m_licenseDuration; }
      set { m_licenseDuration = value; }
    } // LicenseDuration


    /// <summary>
    /// Gets or sets the license type.
    /// </summary>
    public string LicenseType
    {
      get { return m_licenseType; }
      set { m_licenseType = value; }
    } // LicenseType

    #endregion

    #region methods

    /// <summary>
    /// Returns a formatted string containing all of the necessary information to generate the licenses.
    /// </summary>
    public string ToFormattedString()
    {
      return string.Join("|", SystemId, Comments, ShouldResetSystemTimeToLiveDate, SystemTimeToLiveDate, ShouldInstallLicenses, TransactionId, DateIssued, IssuedBy, NumberLicenses, LicenseDuration, LicenseType);
    } // ToFormattedString()


    /// <summary>
    /// Reads the formated string to reconstitute the original object's values with a new instance.
    /// </summary>
    public static LicenseFileFormatter ReadFormattedString(string data)
    {
      try
      {
        string[] segments = data.Split(new char[] { '|' }, StringSplitOptions.None);
        if (segments.Length != 11)
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        LicenseFileFormatter formatter = new LicenseFileFormatter();

        if (segments[0].Trim() != string.Empty)
        {
          formatter.SystemId = segments[0];
        }
        else
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        formatter.Comments = segments[1];

        if (segments[2].Trim() != string.Empty)
        {
          formatter.ShouldResetSystemTimeToLiveDate = Convert.ToBoolean(segments[2]);
        }
        else
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        if (formatter.ShouldResetSystemTimeToLiveDate)
        {
          if (segments[3].Trim() != string.Empty)
          {
            formatter.SystemTimeToLiveDate = Convert.ToDateTime(segments[3]);
          }
          else
          {
            throw new LicenseFileMalformedException("Data string did not meet expectations.");
          }
        }

        if (segments[4].Trim() != string.Empty)
        {
          formatter.ShouldInstallLicenses = Convert.ToBoolean(segments[4]);
        }
        else
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        if (segments[5].Trim() != string.Empty)
        {
          formatter.TransactionId = Guid.Parse(segments[5]);
        }
        else
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        if (segments[6].Trim() != string.Empty)
        {
          formatter.DateIssued = Convert.ToDateTime(segments[6]);
        }
        else
        {
          throw new LicenseFileMalformedException("Data string did not meet expectations.");
        }

        if (formatter.ShouldInstallLicenses)
        {
          if (segments[7].Trim() != string.Empty)
          {
            formatter.IssuedBy = segments[7];
          }
          else
          {
            throw new LicenseFileMalformedException("Data string did not meet expectations.");
          }

          if (segments[8].Trim() != string.Empty)
          {
            formatter.NumberLicenses = Convert.ToInt32(segments[8]);
          }
          else
          {
            throw new LicenseFileMalformedException("Data string did not meet expectations.");
          }

          if (segments[9].Trim() != string.Empty)
          {
            formatter.LicenseDuration = Convert.ToInt32(segments[9]);
          }
          else
          {
            throw new LicenseFileMalformedException("Data string did not meet expectations.");
          }

          if (segments[10].Trim() != string.Empty)
          {
            formatter.LicenseType = segments[10];
          }
          else
          {
            throw new LicenseFileMalformedException("Data string did not meet expectations.");
          }
        }

        return formatter;
      }
      catch (Exception)
      {
        throw new LicenseFileMalformedException("Data string did not meet expectations.");
      }
    } // ReadFormattedString( data )

    #endregion

  } // class LicenseFileFormatter
} // namespace Viatronix.Licensing
