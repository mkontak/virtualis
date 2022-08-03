// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.ComponentModel;
using System.Configuration;

namespace Viatronix.Colon.Configuration
{
  
  #region class ConfigElement<T>

  public class ConfigElement<T> : ConfigurationElement
  {

    #region properties

    /// <summary>
    /// Returns the value attribute of the element
    /// </summary>
    [ConfigurationProperty( "value", IsRequired = true )]
    public T Value
    {
      get { return (T) this["value"]; }
    } // Value

    #endregion

  } // class ConfigElement

  #endregion

  #region class FloatElement

  public class FloatElement : ConfigElement< float >
  {

    #region properties

    /// <summary>
    /// Returns the value attribute as a float
    /// </summary>
    [ConfigurationProperty( "value", IsRequired = true )]
    [TypeConverter( typeof( FloatConverter ))]
    public new float Value
    {
      get { return (float) this["value"]; }
    } // Value

    #endregion

  } // class ConfigElement

  #endregion

  #region class FloatConverter

  public class FloatConverter : ConfigurationConverterBase
  {

    /// <summary>
    /// Converts the object to a float
    /// </summary>
    /// <param name="context"></param>
    /// <param name="culture"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    public override object ConvertFrom( System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value )
    {
      return (float) float.Parse( ( (string) value ).TrimEnd( 'F' ), System.Globalization.NumberFormatInfo.InvariantInfo );
    } // ConvertFrom( context, culture, value )


    /// <summary>
    /// Converts the float to a string
    /// </summary>
    /// <param name="context"></param>
    /// <param name="culture"></param>
    /// <param name="value"></param>
    /// <param name="destinationType"></param>
    /// <returns></returns>
    public override object ConvertTo( System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType )
    {
      return value.ToString() + "F";
    } // ConvertTo( context, culture, value, destinationType )

  } // class FloatConverter

  #endregion

} // namespace Viatonix.Colon.Configuration
