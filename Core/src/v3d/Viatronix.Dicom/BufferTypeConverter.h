
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Globalization;

OPEN_DICOM_NS



/**
 * Type converter used for the buffer
 */
public ref class BufferTypeConverter : TypeConverter
{

#pragma region overrides

public:

  /// Detemines if the source type can be converted to a Buffer
  virtual bool CanConvertFrom( ITypeDescriptorContext ^ mpContext, Type ^ mpSourceType ) override;

  /// Determines if the Buffer can be convertered to a destination type
  virtual bool CanConvertTo( ITypeDescriptorContext ^ mpContext,  Type ^ mpDestinationType ) override;

  /// Converts the the object to a Buffer
  virtual Object ^ ConvertFrom( ITypeDescriptorContext ^ mpContext, CultureInfo ^ mpCulture, Object ^ mpValue)  override;

#pragma endregion

};  // class BufferTypeConverter



CLOSE_DICOM_NS

