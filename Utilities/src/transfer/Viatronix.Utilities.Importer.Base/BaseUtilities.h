#pragma once

// namespaces
OPEN_IMPBASE_NS

/**
 * Miscellanious utility functions for the importer.
 */
public ref class BaseUtilities
{
// methods
public:

  /// Reads the dicom header from the volume file and returns it as a managed string.
  static System::String ^ ReadDicomHeaderFromVolumeFile(System::String ^ mpVolumeFile);

  /// Reads the image count from the volume file and returns it as a managed 32 bit integer.
  static System::Tuple<System::Int32, System::Int32, System::Int32> ^ ReadVolumeDimensionsFromVolumeFile(System::String ^ mpVolumeFile);

}; // class BaseUtilities

// namespaces
CLOSE_IMPBASE_NS
