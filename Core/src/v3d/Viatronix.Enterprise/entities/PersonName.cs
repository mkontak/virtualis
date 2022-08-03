using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Runtime.Serialization;

namespace Viatronix.Enterprise.Entities
{

  #region enumerators

  /// <summary>
  /// Name field identifiers
  /// </summary>
  [Serializable]
  public enum PersonNameFields
  {
    lname,
    fname,
    mname,
    pfx,
    sfx
  };  // enum PErsonNameFields

  #endregion

  /// <summary>
  /// Implements a person name seperating out all the parts of a name last, first, middle, prefix and suffix
  /// </summary>
  [Serializable]
  public class PersonName : ISerializable, ICloneable, IComparable
  {

    #region constants

    /// <summary>
    /// Default xml tag
    /// </summary>
    private const string m_defaultXmlTag = "name";

    /// <summary>
    /// Maximum field allowed
    /// </summary>
    private const int m_maxFields = 10;

    /// <summary>
    /// Allowed delimiters for the name field. The order of the delimiters is the order in which they are
    /// checked so that a ' ' can be used in a name like O Brien using a delimiter that appears prior to 
    /// the space.
    /// </summary>
    private static char[] allowedDelimiters = new char[] { '^', '_', ',', ' ', '*' };

    /// <summary>
    /// Maximum name field (See NameField)
    /// </summary>
    private const int m_maxNameFields = 5;


    #endregion

    #region fields

    /// <summary>
    /// Field count
    /// </summary>
    private int m_fieldCount = 0;

    /// <summary>
    /// Name fields (At most allow for 10)
    /// </summary>
    private string[] m_nameFields = new string[m_maxFields];

    #endregion

    #region constructors and finalizers

    /// <summary>
    /// Default constructor
    /// </summary>
    public PersonName()
      : this(string.Empty)
    {
      for (int i = 0; i < m_nameFields.Length; ++i)
      {
        if (m_nameFields[i] == null)
          m_nameFields[i] = string.Empty;
      }
    } // PersonName

    /// <summary>
    /// Constructs a person name using the name field passed in
    /// </summary>
    /// <param name="name">Delimited name</param>
    public PersonName(string name)
    {
      /// Initialize
      for (int i = 0; i < m_maxFields; m_nameFields[i++] = string.Empty) ;

      ///////////////////////////////////
      // Make sure the name has a value
      ///////////////////////////////////
      if (name.Length > 0)
      {

        //////////////////////////////////////////////////////////
        // Ignore the MOD: prepended to the NAME for some scanners
        ///////////////////////////////////////////////////////////
        string modName = name.Replace("*", " ").Trim().ToUpper();
        if (String.Compare(modName, 0, "MOD:", 0, 4, true) == 0)
        {
          modName = modName.Substring(4);
        } // END ... Ignore the MOD:

        /// Default handles a name with no delimiters
        m_nameFields[0] = modName;
        m_fieldCount = 1;

        //////////////////////////////////////////////////////////////////////////////////////////////
        // I use only one delimiter at a time so that another delimiter can be used from within
        // the name field
        ///////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < allowedDelimiters.Length; ++i)
        {
          char[] del = { allowedDelimiters[i] };

          string[] fields = modName.Split(del);

          /////////////////////////////////////////
          // If this delimiter is found then us it
          ////////////////////////////////////////
          if (fields.Length > 1)
          {
            m_fieldCount = (fields.Length < m_maxFields ? fields.Length : m_maxFields);
            for (int j = 0; j < m_fieldCount; ++j)
            {
              m_nameFields[j] = fields[j].Trim();
            }
            break;
          }

        } // END ... For each allowed name delimiter

      } // END ... If the Dicom NAME is not blank

    } // PersonName(string name)

    /// <summary>
    /// Constructs a person name using the first and last name specified
    /// </summary>
    /// <param name="lastName">Persons last name</param>
    /// <param name="firstName">Persons first name</param>
    public PersonName(string lastName, string firstName)
      : this()
    {
      m_nameFields[(int)PersonNameFields.fname] = firstName;
      m_nameFields[(int)PersonNameFields.lname] = lastName;
      m_fieldCount = 2;
    } // PersonName(string lastName, string firstName)

    /// <summary>
    /// Deserialization constructor
    /// </summary>
    /// <param name="info">sertialization info</param>
    /// <param name="context">context stream</param>
    public PersonName(SerializationInfo info, StreamingContext context)
    {
      m_nameFields[(int)PersonNameFields.lname] = info.GetString(PersonNameFields.lname.ToString());
      m_nameFields[(int)PersonNameFields.fname] = info.GetString(PersonNameFields.fname.ToString());
      m_nameFields[(int)PersonNameFields.mname] = info.GetString(PersonNameFields.mname.ToString());
      m_nameFields[(int)PersonNameFields.pfx] = info.GetString(PersonNameFields.pfx.ToString());
      m_nameFields[(int)PersonNameFields.sfx] = info.GetString(PersonNameFields.sfx.ToString());

      m_fieldCount = info.GetInt32("FIELD_COUNT");
    } // PersonName(SerializationInfo info, StreamingContext context)

    #endregion

    #region properties

    /// <summary>
    /// Gets empty flag (true if the name is empty)
    /// </summary>
    public bool Empty
    {
      get
      {
        int total = 0;
        for (int i = 0; i < m_nameFields.Length; total += m_nameFields[i++].Length) ;
        return (total == 0);
      }
    } // Empty

    /// <summary>
    /// Gets the last, first and middle name
    /// </summary>
    public string LastFirstMiddle
    {
      get { return this.LastFirst + ((this.MiddleName == string.Empty) ? string.Empty : " " + this.MiddleName); }
    } // LastFirstMiddle

    /// <summary>
    /// Gets the last, first middle name only
    /// </summary>
    public string LastFirst
    {
      get
      {
        string name = string.Empty;

        if (m_nameFields[(int)PersonNameFields.lname].Length > 0)
        {
          name = m_nameFields[(int)PersonNameFields.lname];

          if (m_nameFields[(int)PersonNameFields.fname].Length > 0)
          {
            name = name + ", " + m_nameFields[(int)PersonNameFields.fname];
          }

        }
        else
        {
          if (m_nameFields[(int)PersonNameFields.fname].Length > 0)
          {
            name = m_nameFields[(int)PersonNameFields.fname];
          }
        }

        return name;

      } // get

    } // LastFirst

    /// <summary>
    /// Gets the Full Name PREFIX FIRST MIDDLE LAST SUFFIX
    /// </summary>
    public string FullName
    {
      get
      {
        StringBuilder fullName = new StringBuilder();

        fullName.Append(m_nameFields[(int)PersonNameFields.pfx]);

        if (String.Compare(m_nameFields[(int)PersonNameFields.fname], String.Empty) != 0)
        {
          fullName.Append((fullName.Length != 0 ? String.Concat(" ", m_nameFields[(int)PersonNameFields.fname]) : m_nameFields[(int)PersonNameFields.fname]));
        }

        if (String.Compare(m_nameFields[(int)PersonNameFields.mname], String.Empty) != 0)
        {
          fullName.Append((fullName.Length != 0 ? String.Concat(" ", m_nameFields[(int)PersonNameFields.mname]) : m_nameFields[(int)PersonNameFields.mname]));
        }

        if (String.Compare(m_nameFields[(int)PersonNameFields.lname], String.Empty) != 0)
        {
          fullName.Append((fullName.Length != 0 ? String.Concat(" ", m_nameFields[(int)PersonNameFields.lname]) : m_nameFields[(int)PersonNameFields.lname]));
        }

        if (String.Compare(m_nameFields[(int)PersonNameFields.sfx], String.Empty) != 0)
        {
          fullName.Append((fullName.Length != 0 ? String.Concat(" ", m_nameFields[(int)PersonNameFields.sfx]) : m_nameFields[(int)PersonNameFields.sfx]));
        }

        return fullName.ToString();
      }
    } // FullName

    /// <summary>
    /// Sets/Gets the Last name
    /// </summary>
    public string LastName
    {
      get { return m_nameFields[(int)PersonNameFields.lname]; }
      set { m_nameFields[(int)PersonNameFields.lname] = value; if (value.Length > 0) SetFieldCount(PersonNameFields.lname); }
    } // LastName

    /// <summary>
    /// Sets/Gets the First Name
    /// </summary>
    public string FirstName
    {
      get { return m_nameFields[(int)PersonNameFields.fname]; }
      set { m_nameFields[(int)PersonNameFields.fname] = value; if (value.Length > 0) SetFieldCount(PersonNameFields.fname); }
    } // FirstName

    /// <summary>
    /// Sets/Gets the Middle Name
    /// </summary>
    public string MiddleName
    {
      get { return m_nameFields[(int)PersonNameFields.mname]; }
      set { m_nameFields[(int)PersonNameFields.mname] = value; if (value.Length > 0) SetFieldCount(PersonNameFields.mname); }
    } // MiddleName

    /// <summary>
    /// Sets/Gets the Name Prefix
    /// </summary>
    public string Prefix
    {
      get { return m_nameFields[(int)PersonNameFields.pfx]; }
      set { m_nameFields[(int)PersonNameFields.pfx] = value; if (value.Length > 0) SetFieldCount(PersonNameFields.pfx); }
    } // Prefix

    /// <summary>
    /// Sets/Gets the Name Suffix
    /// </summary>
    public string Suffix
    {
      get { return m_nameFields[(int)PersonNameFields.sfx]; }
      set { m_nameFields[(int)PersonNameFields.sfx] = value; if (value.Length > 0) SetFieldCount(PersonNameFields.sfx); }
    } // Suffix

    #endregion

    #region methods

    /// <summary>
    /// Serializes object
    /// </summary>
    /// <param name="s">serialzation info</param>
    /// <param name="c">context stream</param>
    public void GetObjectData(SerializationInfo s, StreamingContext c)
    {
      s.SetType(this.GetType());
      s.AddValue("FIELD_COUNT", m_fieldCount);
      s.AddValue(PersonNameFields.lname.ToString(), m_nameFields[(int)PersonNameFields.lname]);
      s.AddValue(PersonNameFields.fname.ToString(), m_nameFields[(int)PersonNameFields.fname]);
      s.AddValue(PersonNameFields.mname.ToString(), m_nameFields[(int)PersonNameFields.mname]);
      s.AddValue(PersonNameFields.pfx.ToString(), m_nameFields[(int)PersonNameFields.pfx]);
      s.AddValue(PersonNameFields.sfx.ToString(), m_nameFields[(int)PersonNameFields.sfx]);

    } // GetObjectData(SerializationInfo s, StreamingContext c) 

    /// <summary>
    /// Returns the hash code for the person name string
    /// </summary>
    /// <returns>Hash code</returns>
    public override int GetHashCode()
    {
      return ToString().GetHashCode();
    } // GetHashCode()


    /// <summary>
    /// Determines if the person name supplied equals the current name
    /// </summary>
    /// <param name="name">Name to compare against</param>
    /// <returns>True is equal</returns>
    public override bool Equals(object obj)
    {
      if (obj is PersonName)
      {
        return Equals((PersonName)obj);
      }
      else
      {
        throw new ArgumentException("Invalid type, expecting PersonName");
      }
    } // Equals(object obj)

    /// <summary>
    /// Determines if the person name supplied equals the current name
    /// </summary>
    /// <param name="name">Name to compare against</param>
    /// <returns>True is equal</returns>
    public bool Equals(PersonName name)
    {
      return (String.Compare(this.ToDicom(), name.ToDicom()) == 0);
    } // Equals(PersonName name)

    /// <summary>
    /// Operator equality
    /// </summary>
    /// <param name="name1">Name1 to compare</param>
    /// <param name="name2">Name2 to compare Name1 to</param>
    /// <returns>True if the two names are equal</returns>
    public static bool op_Equality(PersonName name1, PersonName name2)
    {
      return name1.Equals(name2);
    } // op_Equality()

    /// <summary>
    /// Operator Inequality
    /// </summary>
    /// <param name="name1">Name1 to compare</param>
    /// <param name="name2">Name2 to compare Name1 to</param>
    /// <returns>True if the two names are not equal</returns>
    public static bool op_Inequality(PersonName name1, PersonName name2)
    {
      return !name1.Equals(name2);
    } // op_Inequality()


    /// <summary>
    /// Returns the string representation of the name
    /// </summary>
    /// <returns>String representation of object</returns>
    public override string ToString()
    {
      return ToString(string.Empty);
    } // ToString()


    /// <summary>
    /// Converts the value of this instance to its equivalent string representation using the specified format. If no formta is specified then 
    /// the default format for the ToString()is the.
    /// 
    /// ************* DO NOT CHANGE THE DEFAULT ToString() FORMAT UNLESS YOU TALK TO MARK *****************************
    /// 
    /// </summary>
    /// <param name="format">A format string.</param>
    /// <returns>A string representation of value of this instance as specified by format.</returns>
    public string ToString(string format)
    {
      switch (format.ToUpper())
      {
        case "LFM": return LastFirstMiddle;
        case "LF": return LastFirst;
        case "W": return FullName;
        case "L": return LastName;
        case "F": return FirstName;
        case "M": return MiddleName;
        case "P": return Prefix;
        case "S": return Suffix;

        ////////////////////////////////////////////////////////
        // Default string format is DICOM please do not change 
        ////////////////////////////////////////////////////////
        default:
          return this.ToDicom();
      }
    } // ToString( format )


    /// <summary>
    /// Returns the person name as a dicom string
    /// </summary>
    /// <returns>Dicom string representation of the name</returns>
    public string ToDicom()
    {
      return String.Join("^", m_nameFields, 0, m_fieldCount);
    } // ToDicom()

    /// <summary>
    /// Returns the XML for this name with the default tag of Name
    /// </summary>
    /// <returns>Xml representation of the name</returns>
    public string ToXml()
    {
      return ToXml(m_defaultXmlTag);
    } // ToXml()

    /// <summary>
    /// Returns the XML for this name with the default tag of Name
    /// </summary>
    /// <returns>Xml representation of the name</returns>
    public string ToXml(string xmlTag)
    {
      using (MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter(new StreamWriter(stream));

        writer.WriteStartElement(xmlTag);

        writer.WriteAttributeString(PersonNameFields.lname.ToString(), this.LastName);
        writer.WriteAttributeString(PersonNameFields.fname.ToString(), this.FirstName);
        writer.WriteAttributeString(PersonNameFields.mname.ToString(), this.MiddleName);
        writer.WriteAttributeString(PersonNameFields.pfx.ToString(), this.Prefix);
        writer.WriteAttributeString(PersonNameFields.sfx.ToString(), this.Suffix);

        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString(stream.ToArray());
      }
    } // ToXml()


    /// <summary>
    /// Sets the field count based on the field
    /// </summary>
    /// <param name="field">field</param>
    private void SetFieldCount(PersonNameFields field)
    {
      int count = (int)field + 1;
      m_fieldCount = (m_fieldCount < count ? count : m_fieldCount);
    }

    #endregion

    #region ICloneable Members

    /// <summary>
    /// Creates a new person name object that is a copy of the current instance.
    /// </summary>
    /// <returns>A new object that is a copy of this instance.</returns>
    public object Clone()
    {
      return new PersonName(this.ToDicom());
    } // Clone()

    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a PersonName object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a PersonName object.</param>
    /// <returns>A PersonName object.</returns>
    public static PersonName Create(string name)
    {
      PersonName personName = new PersonName(name);

      if (personName == null)
        throw new OutOfMemoryException("Failed to allocate PersonName");



      return personName;
    } 

    /// <summary>
    /// Creates a new instance of a PersonName object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a PersonName object.</param>
    /// <returns>A PersonName object.</returns>
    public static PersonName Create(XmlNode node)
    {
      PersonName personName = new PersonName();

      if (personName == null)
        throw new OutOfMemoryException("Failed to allocate PersonName");

      XmlAttribute version = node.Attributes["ver"];
      if (version != null && version.Value == "2.0")
      {
        personName.LastName = node.Attributes[PersonNameFields.lname.ToString()].Value;
        personName.FirstName = node.Attributes[PersonNameFields.fname.ToString()].Value;
        personName.MiddleName = node.Attributes[PersonNameFields.mname.ToString()].Value;
        personName.Prefix = node.Attributes[PersonNameFields.pfx.ToString()].Value;
        personName.Suffix = node.Attributes[PersonNameFields.sfx.ToString()].Value;
      }
      else
      {
        personName.LastName = node.Attributes["LAST_NAME"].Value;
        personName.FirstName = node.Attributes["FIRST_NAME"].Value;
        personName.MiddleName = node.Attributes["MI"].Value;
        personName.Prefix = node.Attributes["PREFIX"].Value;
        personName.Suffix = node.Attributes["SUFFIX"].Value;
      }

      return personName;
    } // FromXml( node )

    #endregion

    #region IComparable Members

    /// <summary>
    /// Compares the object specified to this object
    /// </summary>
    /// <param name="obj">Object to compare to</param>
    /// <returns></returns>
    public int CompareTo(object obj)
    {
      PersonName other = (PersonName)obj;

      int compare = this.LastName.CompareTo(other.LastName);
      if (compare == 0)
      {
        compare = this.FirstName.CompareTo(other.FirstName);
        if (compare == 0)
        {
          compare = this.MiddleName.CompareTo(other.MiddleName);
          if (compare == 0)
          {
            compare = this.Prefix.CompareTo(other.Prefix);
            if (compare == 0)
              compare = this.Suffix.CompareTo(other.Suffix);
          }
        }
      }

      return compare;
    } // CompareTo( obj )

    #endregion

  } // class PersonName
} // namespace Viatronix.Data
