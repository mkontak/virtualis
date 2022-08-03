// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Messaging
{
  public class Range< T > where T : struct, IFormattable
  {

    #region fields

    /// <summary>
    /// xml tag name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// start value
    /// </summary>
    private T? m_from = null;

    /// <summary>
    /// end value
    /// </summary>
    private T? m_to = null;

    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="name">xml tag name</param>
    internal Range( string name )
    {
      m_name = name;
    } // Range( name )


    #endregion

    #region properties

    /// <summary>
    /// Returns the name
    /// </summary>
    public string Name
    {
      get { return m_name; }
    } // Name


    /// <summary>
    /// Gets or sets the start value
    /// </summary>
    public virtual T? StartValue
    {
      get { return m_from; }
      set { m_from = value; }
    } // StartValue


    /// <summary>
    /// Gets or sets the end value
    /// </summary>
    public virtual T? EndValue
    {
      get { return m_to; }
      set { m_to = value; }
    } // EndValue


    /// <summary>
    /// Determines if the range has a value
    /// </summary>
    public bool HasValue
    {
      get { return m_from.HasValue || m_to.HasValue; }
    } // HasValue

    #endregion

    #region methods

    /// <summary>
    /// Checks equality among Range objects
    /// </summary>
    /// <param name="other">other object</param>
    /// <returns>true if equal</returns>
    public bool Equals( Range< T > other )
    {
      return other != null &&
             Nullable.Equals( m_from, other.m_from ) && 
             Nullable.Equals( m_to, other.m_to );
    } // Equals( other )


    /// <summary>
    /// Outputs xml string
    /// </summary>
    /// <returns>xml</returns>
    public string ToXml()
    {
      if( !this.HasValue )
        return string.Empty;

      const string formatString = "<{0} from=\"{1}\" to=\"{2}\" />";
      return string.Format( formatString, m_name, GetValue( m_from ), GetValue( m_to ));
    } // ToXml()


    /// <summary>
    /// Loads data from xml
    /// </summary>
    /// <param name="node">xml</param>
    public void LoadXml( XmlNode node )
    {
      if( node == null )
        return;

      node.TryGetAttribute( "from", ref m_from );
      node.TryGetAttribute( "to", ref m_to );
    } // LoadXml( node )
    

    /// <summary>
    /// Returns the value.
    /// </summary>
    /// <param name="value">value</param>
    /// <returns>string representation</returns>
    protected virtual string GetValue( T? value )
    {
      return ( value.HasValue ) ? value.Value.ToString() : string.Empty;
    } // GetValue( value )

    #endregion

  } // class Range< T >


  public class DateRange : Range< DateTimeOffset >
  {

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="name">xml tag name</param>
    internal DateRange( string name ) : base( name )
    {
    } // DateRange( name )

    #endregion 

    #region properties

    /// <summary>
    /// Gets or sets the start value
    /// </summary>
    public override DateTimeOffset? StartValue
    {
      get { return base.StartValue; }
      set { base.StartValue = ( value.HasValue ) ? DateTime.Parse( value.Value.ToXmlDateString() ) : value; }
    } // StartValue


    /// <summary>
    /// Gets or sets the end value
    /// </summary>
    public override DateTimeOffset? EndValue
    {
      get { return base.EndValue; }
      set { base.EndValue = ( value.HasValue ) ? DateTime.Parse( value.Value.ToXmlDateString() ) : value; }
    } // EndValue

    #endregion

    #region methods

    /// <summary>
    /// Returns the value.
    /// </summary>
    /// <param name="value">value</param>
    /// <returns>string representation</returns>
    protected override string GetValue( DateTimeOffset? value )
    {
      return ( value.HasValue ) ? value.Value.ToXmlDateString() : string.Empty;
    } // GetValue( value )

    #endregion

  } // class DateRange


  public class DateTimeRange : Range< DateTimeOffset >
  {
   
    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    /// <param name="name">xml tag name</param>
    internal DateTimeRange( string name ) : base( name )
    {
    } // DateTimeRange( name )

    #endregion 

    #region methods

    /// <summary>
    /// Returns the value.
    /// </summary>
    /// <param name="value">value</param>
    /// <returns>string representation</returns>
    protected override string GetValue( DateTimeOffset? value )
    {
      return ( value.HasValue ) ? value.Value.ToXmlDateTimeString() : string.Empty;
    } // GetValue( value )

    #endregion

  } // class DateTimeRange 

} // namespace Viatronix.Enterprise.Messaging
