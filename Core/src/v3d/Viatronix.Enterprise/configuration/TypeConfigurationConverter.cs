using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace Viatronix.Enterprise.Configuration
{
  public class TypeConfigurationConverter : ConfigurationConverterBase
  {

    public override object ConvertFrom( System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value )
    {
      return Type.GetType( (string) value );
    }


    public override object ConvertTo( System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType )
    {
      return value.GetType().FullName;
    }

  }
}
