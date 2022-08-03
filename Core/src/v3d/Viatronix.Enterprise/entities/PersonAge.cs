// $Id: PersonAge.cs,v 1.16.2.2 2009/02/06 19:19:12 kchalupa Exp $
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
using System.Text;
using System.Xml;
using System.Runtime.Serialization;


namespace Viatronix.Enterprise.Entities
{

  #region enumerations

  /// <summary>
  /// Age Units Enumerator
  /// </summary>
  [Serializable]
  public enum AgeUnit 
  { 
    DAY, 
    WEEK, 
    MONTH, 
    YEAR 
  };  // enum AgeUnit


  /// <summary>
  /// PersonAge fields
  /// </summary>
  [Serializable]
  public enum PersonAgeFields
  {
    AGE,
    AGE_UNITS
  };  // enum PersonAgeFields

  #endregion

	/// <summary>
	/// Person Age implementation
	/// </summary>
  [Serializable]
	public class PersonAge : ISerializable, ICloneable
	{
    #region constants

    /// <summary>
    /// Default Xml tag
    /// </summary>
    private string m_defaultXmlTag = "age";

    /// <summary>
    /// Used to determine numerical values
    /// </summary>
    private const string m_numeric = "0123456789";

    /// Valid dicom age unit Id's
    private static string [] m_validAgeUnits = { "D", "W", "M", "Y" };

    #endregion

    #region fields

    /// <summary>
    /// Person age
    /// </summary>
    private int m_age = 0;

    /// <summary>
    /// Ages units default to YEAR
    /// </summary>
    private AgeUnit m_units = AgeUnit.YEAR;

    #endregion

    #region constructors and finalizers

    /// <summary>
    /// Constructor
    /// </summary>
		public PersonAge()
		{
		} // PersonAge()

    /// <summary>
    /// Constructor with initial values
    /// </summary>
    /// <param name="age">Age</param>
    /// <param name="units">Age unit</param>
    public PersonAge(int age, AgeUnit units)
    {
      Age = age;
      m_units = units;
    } // PersonAge(int age, AgeUnit units)


    /// <summary>
    /// Constrictor with just hte age assumes the units is years
    /// </summary>
    /// <param name="age">Age</param>
    public PersonAge(int age)
    {
      Age = age;
    } // PersonAge(int age)

    /// <summary>
    /// Constructor with the age as a dicom string
    /// </summary>
    /// <param name="age">Age string in dicom format</param>
    public PersonAge(string age)
    {
      if ( age.Length > 1 )
      {


        // Assume no units
        string ageField = age;

        // Get the last character
        string unitsField = age.Substring(age.Length - 1, 1);


        ////////////////////////////////////////////////////////////////////////
        // Check if the units fields is numeric indicating that the it is 
        // not supplied and YEAR is assumed
        ///////////////////////////////////////////////////////////////////////
        if ( m_numeric.IndexOf(unitsField) == -1 )
        {
          // The last character is alway the units
          ageField  = age.Substring(0,age.Length - 1);

          int units = 0;
          if ( (units = Array.IndexOf(m_validAgeUnits,units)) == -1 )
          {
            //Log.Warning("Invalid age units: " + unitsField + ", setting to units Y");
          }
          else
          {
            m_units = (AgeUnit)units;
          }
        }

        
        try
        {
          Age = System.Convert.ToInt32(ageField);
        }
        catch ( Exception  )
        {
          //Log.Warning("Invalid patient age: " + ageField + ", setting to age 0");
        }

      } // END ... If the patient age has a length greater then 2
    } // PersonAge(string age)

    /// <summary>
    /// Deserialization constructor
    /// </summary>
    /// <param name="info">serialization info</param>
    /// <param name="context">context stream</param>
    public PersonAge(SerializationInfo info, StreamingContext context)  
    {
      m_age = info.GetInt32(PersonAgeFields.AGE.ToString());
      m_units = (AgeUnit)info.GetInt32(PersonAgeFields.AGE_UNITS.ToString());
    } // PersonAge(SerializationInfo info, StreamingContext context)  

    #endregion
    
    #region properties

    /// <summary>
    /// Sets/Gets the age
    /// </summary>
    public int Age
    {
      get { return m_age; }
      set 
      {
        if ( value < 0 )
        {
          throw new System.Exception("Invalid age, cannot be less then 0");
        } // END ... If the age is less then 0

        m_age = value; 
      }
    } // Age

    /// <summary>
    /// Sets/Gets the units
    /// </summary>
    public AgeUnit Units
    {
      get { return m_units; }
      set { m_units = value; }
    } // Units

    #endregion

    #region methods

    /// <summary>
    /// Serializes object
    /// </summary>
    /// <param name="s">serialization info</param>
    /// <param name="c">context stream</param>
    public void GetObjectData(SerializationInfo s, StreamingContext c) 
    {
      s.SetType(this.GetType());
      s.AddValue(PersonAgeFields.AGE.ToString(), m_age);
      s.AddValue(PersonAgeFields.AGE_UNITS.ToString(), (int)m_units);
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
    /// Returns the string representation of the age
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return ToDicom();
    } // ToString()



    /// <summary>
    /// Returns the dicom format of the age
    /// </summary>
    /// <returns>Dicom formatted age</returns>
    public string ToDicom()
    {
      return String.Format("{0:000}{1}",m_age,ConvertToString(m_units));
    } // ToDicom()


    /// <summary>
    /// Returns the XML for this name with the default tag of Name
    /// </summary>
    /// <returns>Xml representation of the name</returns>
    public string ToXml()
    {
      using( MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter( new StreamWriter( stream ));

        writer.WriteStartElement( m_defaultXmlTag );

        writer.WriteAttributeString( PersonAgeFields.AGE.ToString(), Age.ToString() );
        writer.WriteAttributeString( PersonAgeFields.AGE_UNITS.ToString(), Units.ToString() );
      
        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString( stream.ToArray() );
      }
    } // ToXml()


    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="obj">Object to check equality to</param>
    /// <returns>TRue if equal</returns>
    public override bool Equals(object obj)
    {
      PersonAge age = (PersonAge)obj;
      return ( age.ToString() == ToString() ? true : false);
    } //  Equals(object obj)



    /// <summary>
    /// OPerator overload (==)
    /// </summary>
    /// <param name="age1">age1 to be used in the comparison</param>
    /// <param name="age2">age2 to be used in the comparison</param>
    /// <returns>True if euqual</returns>
    public static  bool op_Equality(PersonAge age1, PersonAge age2)
    {
      return age1.Equals(age2);
    } // op_Equality(PersonAge age1, PersonAge age2)


    /// <summary>
    /// Operator overload !=
    /// </summary>
    /// <param name="age1">age1 to be used in the comparison</param>
    /// <param name="age2">age2 to be used in the comparison</param>
    /// <returns>TRue if not equal</returns>
    public static  bool op_Inequality(PersonAge age1, PersonAge age2)
    {
      return ! age1.Equals(age2);

    } // op_Inequality(PersonAge age1, PersonAge age2)

    /// <summary>
    /// Operator overload >
    /// </summary>
    /// <param name="age1">age1 to be used in the comparison</param>
    /// <param name="age2">age2 to be used in the comparison</param>
    /// <returns>True if age1 > age2</returns>
    public static  bool op_GreaterThan(PersonAge age1, PersonAge age2)
    {
      return  ((age1.Units >  age2.Units) && ( age1.Age != 0 && age2.Age != 0 ) ||
                (age1.Units == age2.Units) && ( age1.Age > age2.Age));
    } // op_GreaterThan(PersonAge age1, PersonAge age2)

    /// <summary>
    /// Operator overload <=
    /// </summary>
    /// <param name="age1">age1 to be used in the comparison</param>
    /// <param name="age2">age2 to be used in the comparison</param>
    /// <returns>True if age1 less then or equal to age2</returns>
    public static  bool op_LessThanOrEqual(PersonAge age1, PersonAge age2)
    {
      return  ((age1.Units <  age2.Units) && ( age1.Age != 0 && age2.Age != 0 ) ||
                (age1.Units == age2.Units) && ( age1.Age <= age2.Age));
    } // op_LessThanOrEqual(PersonAge age1, PersonAge age2)

    /// <summary>
    /// Operator overload >=
    /// </summary>
    /// <param name="age1">age1 to be used in the comparison</param>
    /// <param name="age2">age2 to be used in the comparison</param>
    /// <returns>True if age1 >= age2</returns>
    public static  bool op_GreaterThanOrEqual(PersonAge age1, PersonAge age2)
    {
      return  ((age1.Units >  age2.Units) && ( age1.Age != 0 && age2.Age != 0 ) ||
                (age1.Units == age2.Units) && ( age1.Age >= age2.Age));
    }  // op_GreaterThanOrEqual(PersonAge age1, PersonAge age2)
   

    #endregion

    #region ICloneable Members

    /// <summary>
    /// Creates a new object that is a copy of the current instance.
    /// </summary>
    /// <returns>A new object that is a copy of this instance.</returns>
    public object Clone()
    {
      return new PersonAge(this.ToDicom());
    } // Clone()

    #endregion

    #region static methods

    /// <summary>
    /// Gets the units
    /// </summary>
    /// <param name="units">Units to convert</param>
    /// <returns>String representation of the age units</returns>
    public static string  ConvertToString(AgeUnit units) 
    { 
      return m_validAgeUnits[(int)units];
    } // ConvertToString(AgeUnit units) 

    /// <summary>
    /// converts the age units string representation to the enumerated value 
    /// </summary>
    /// <param name="units">Units to convert</param>
    /// <returns></returns>
    public static AgeUnit ConvertToAgeUnit(string units)
    {
      AgeUnit unit = AgeUnit.YEAR;
      if ( string.Compare(units,"D",true) == 0 )
      {
        unit = AgeUnit.DAY;
      }
      else if ( string.Compare(units,"W",true) == 0 )
      {
        unit = AgeUnit.WEEK;
      }
      else if ( string.Compare(units,"M",true) == 0 )
      {
        unit = AgeUnit.MONTH;
      }
      else if ( string.Compare(units,"Y",true) == 0 )
      {
        unit = AgeUnit.YEAR;
      }

      return unit;
    } // ConvertToAgeUnit(string units)
  
    /// <summary>
    /// Creates a new instance of a PersonAge object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a PersonAge object.</param>
    /// <returns>A PersonAge object.</returns>
    public static PersonAge Create( XmlNode node )
    {
      PersonAge age = new PersonAge();

      if (age == null)
        throw new OutOfMemoryException("Failed to allocate PersonAge object");

      age.Age = Int32.Parse( node.Attributes[ PersonAgeFields.AGE.ToString() ].Value );
      age.Units = (AgeUnit) Enum.Parse( typeof( AgeUnit ), node.Attributes[ PersonAgeFields.AGE_UNITS.ToString() ].Value, true );

      return age;
    } // FromXml( node )

    #endregion

	} // class PersonAge

} // namespace Viatronix.Data



#region revision history

// $Log: PersonAge.cs,v $
// Revision 1.16.2.2  2009/02/06 19:19:12  kchalupa
// Region crashproofing.
//
// Revision 1.16.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.16  2007/03/07 19:05:53  geconomos
// code review
//
// Revision 1.15  2007/03/06 21:17:27  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Enterprise.Entities/data/PersonAge.cs,v 1.16.2.2 2009/02/06 19:19:12 kchalupa Exp $
// $Id: PersonAge.cs,v 1.16.2.2 2009/02/06 19:19:12 kchalupa Exp $

#endregion
