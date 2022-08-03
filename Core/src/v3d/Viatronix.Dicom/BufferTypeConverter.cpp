#include "Stdafx.h"
#include "Buffer.h"
#include "BufferTypeConverter.h"


USING_DICOM_NS

/**
  * Determines if the source type can be converterd to a Buffer
  *
  * @param mpContext       
  * @param mpSouceType     Tye of source you wan to convert from
  *
  * @return true if conversion is possible, false if not
  */
bool BufferTypeConverter::CanConvertFrom( ITypeDescriptorContext ^ mpContext, Type ^ mpSourceType )
{
  if ( mpSourceType->Equals(Viatronix::Dicom::Buffer::typeid) )
    return true;
  else
    return TypeConverter::CanConvertFrom( mpContext, mpSourceType );
} // CanConvertFrom( ITypeDescriptorContext ^ mpContext, Type ^ mpSourceType )

/**
 * Determines of the Buffer object can be converted to another object type
 *
 * @param mpContext
 * @param mpDestinationType
 *
 * @return  Returns trues if it can be convert false otherwise
 */
bool BufferTypeConverter::CanConvertTo( ITypeDescriptorContext ^ mpContext,  Type ^ mpDestinationType )
{
  if ( mpDestinationType->Equals(Viatronix::Dicom::Buffer::typeid) )
    return true;
  else
    return TypeConverter::CanConvertTo( mpContext,mpDestinationType ) ;
} // CanConvertTo( ITypeDescriptorContext ^ mpContext,  Type ^ mpDestinationType ) 


/**
 * Preforms the conversion from the values old type to a Buffer
 *
 * @param mpContext
 * @param mpCulture
 * @param mpValue           Value to be converted
 *
 * @return   Newly converted object
 */
Object ^ BufferTypeConverter::ConvertFrom( ITypeDescriptorContext ^ mpContext, CultureInfo ^ mpCulture, Object ^ mpValue) 
{
  Type ^ mpType(mpValue->GetType());

  if ( mpType->Equals(Buffer::typeid) )
    return mpValue;
  else
    return TypeConverter::ConvertFrom(mpContext, mpCulture, mpValue);
} // ConvertFrom( ITypeDescriptorContext ^ mpContext, CultureInfo ^ mpCulture, Object ^ mpValue)

